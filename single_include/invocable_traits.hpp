#ifndef RUBY_INVOCABLE_TRAITS_HPP
#define RUBY_INVOCABLE_TRAITS_HPP

#ifndef RUBY_INVOCABLE_FUNCTION_TRAITS
#define RUBY_INVOCABLE_FUNCTION_TRAITS

#include <tuple>
#include <type_traits>
#include <utility>

namespace ruby::invocable
{

  // clang-format off

  /**
   * make_function builds a function types with the given qualifiers, return type and argument types.
   * The primarty template is not defined, because all cases are handled by the template specializations.
   */
  template<
    bool IsConst, bool IsVolatile, unsigned NumRef,
    bool IsVariadic,  bool IsNoexcept, typename Ret, typename... Args>
      requires (NumRef < 3)
  struct make_function;

  /** Returns a function type with the given qualifiers, return type and argument types.
   */
  template<
    bool IsConst, bool IsVolatile,  unsigned NumRef, 
    bool IsVariadic, bool IsNoexcept, typename Ret, typename... Args>
      requires (NumRef < 3)
  using make_function_t = typename make_function<IsConst, IsVolatile, NumRef, IsVariadic,
                                                 IsNoexcept, Ret, Args...>::type;

  template<bool IsConst, bool IsVolatile, unsigned NumRef, bool IsVariadic,
           bool IsNoexcept, typename Ret, typename... Args>
             requires (NumRef < 3)
  struct function_types
  {
    static constexpr bool is_const = IsConst;
    static constexpr bool is_volatile = IsVolatile;

    static constexpr unsigned num_references = NumRef;

    static constexpr bool is_noexcept = IsNoexcept;
    static constexpr bool is_variadic = IsVariadic;

    using function_type = make_function_t<IsConst, IsVolatile, NumRef, IsVariadic, IsNoexcept,  Ret, Args...>;
    using return_type = Ret;
    using argument_types = std::tuple<Args...>;

    static constexpr auto arity = sizeof...(Args);

    template<std::size_t index>
      requires(index < arity) 
    using argument = std::tuple_element_t<index, argument_types>;
  };

  /** function_traits is specialized for all function types (s.t std::function_v is true for that type)
   */
  template<typename T>
  struct function_traits
  {};

  /** Returns the function return type of the template argument */
  template<typename T>
    requires std::is_function_v<T>
  using function_ret_t = typename function_traits<T>::return_type;

  /** Returns the function argument types of the template argument as a std::tuple */
  template<typename T>
    requires std::is_function_v<T>
  using function_args_t = typename function_traits<T>::argument_types;

  /** Returns the function argument type of 'T' at position 'index' */
  template<typename T, std::size_t index>
    requires std::is_function_v<T>
  using function_arg_t = typename function_traits<T>::template argument<index>;

  /** Returns the number of function arguments of the template argument */
  template<typename T>
    requires std::is_function_v<T>
  inline constexpr auto function_arity_v = function_traits<T>::arity;

  // clang-format on

  template<typename T>
  inline constexpr auto function_is_const_v = [] {
    if constexpr(std::is_function_v<T>)
      return function_traits<T>::is_const;
    else
      return false;
  }();

  template<typename T>
  inline constexpr auto function_is_volatile_v = [] {
    if constexpr(std::is_function_v<T>)
      return function_traits<T>::is_volatile;
    else
      return false;
  }();

  template<typename T>
  inline constexpr auto function_is_lvalue_reference_v = [] {
    if constexpr(std::is_function_v<T>)
      return function_traits<T>::num_references == 1;
    else
      return false;
  }();

  template<typename T>
  inline constexpr auto function_is_rvalue_reference_v = [] {
    if constexpr(std::is_function_v<T>)
      return function_traits<T>::num_references == 2;
    else
      return false;
  }();

  template<typename T>
  inline constexpr auto function_is_reference_v = [] {
    if constexpr(std::is_function_v<T>)
      return function_is_lvalue_reference_v<T> || function_is_rvalue_reference_v<T>;
    else
      return false;
  }();

  template<typename T>
  inline constexpr auto function_is_noexcept_v = [] {
    if constexpr(std::is_function_v<T>)
      return function_traits<T>::is_noexcept;
    else
      return false;
  }();

  template<typename T>
  inline constexpr auto function_is_variadic_v = [] {
    if constexpr(std::is_function_v<T>)
      return function_traits<T>::is_variadic;
    else
      return false;
  }();

  namespace invocable_impl
  {
    template<typename...>
    struct function_modify;

    template<typename T>
    struct function_modify<T> : function_modify<T, function_args_t<T>>
    {};

    template<typename T, typename... Args>
    struct function_modify<T, std::tuple<Args...>>
    {
  private:
      static constexpr auto C = function_is_const_v<T>;
      static constexpr auto V = function_is_volatile_v<T>;
      static constexpr auto R = function_traits<T>::num_references;
      static constexpr auto IV = function_is_variadic_v<T>;
      static constexpr auto IN = function_is_noexcept_v<T>;
      using ret = function_ret_t<T>;

  public:
      using add_const = make_function_t<true, V, R, IV, IN, ret, Args...>;
      using remove_const = make_function_t<false, V, R, IV, IN, ret, Args...>;

      using add_volatile = make_function_t<C, true, R, IV, IN, ret, Args...>;
      using remove_volatile = make_function_t<C, false, R, IV, IN, ret, Args...>;

      using add_variadic = make_function_t<C, V, R, true, IN, ret, Args...>;
      using remove_variadic = make_function_t<C, V, R, false, IN, ret, Args...>;

      using add_noexcept = make_function_t<C, V, R, IV, true, ret, Args...>;
      using remove_noexcept = make_function_t<C, V, R, IV, false, ret, Args...>;

      using remove_lvalue_reference = make_function_t<C, V, R & 0b10, IV, IN, ret, Args...>;
      using remove_rvalue_reference = make_function_t<C, V, R & 0b01, IV, IN, ret, Args...>;
    };
  } // namespace impl

  // clang-format off

  template<typename T>
    requires std::is_function_v<T>
  using function_add_const_t = typename invocable_impl::function_modify<T>::add_const;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_const_t = typename invocable_impl::function_modify<T>::remove_const;

  template<typename T>
    requires std::is_function_v<T>
  using function_add_volatile_t = typename invocable_impl::function_modify<T>::add_volatile;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_volatile_t = typename invocable_impl::function_modify<T>::remove_volatile;

  template<typename T>
    requires std::is_function_v<T>
  using function_add_cv_t = function_add_const_t<function_add_volatile_t<T>>;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_cv_t = function_remove_const_t<function_remove_volatile_t<T>>;

  template<typename T>
    requires std::is_function_v<T>
  using function_add_variadic_t = typename invocable_impl::function_modify<T>::add_variadic;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_variadic_t = typename invocable_impl::function_modify<T>::remove_variadic;

  template<typename T>
    requires std::is_function_v<T>
  using function_add_noexcept_t = typename invocable_impl::function_modify<T>::add_noexcept;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_noexcept_t = typename invocable_impl::function_modify<T>::remove_noexcept;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_lvalue_reference_t =
      typename invocable_impl::function_modify<T>::remove_lvalue_reference;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_rvalue_reference_t =
      typename invocable_impl::function_modify<T>::remove_rvalue_reference;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_reference_t =
      function_remove_lvalue_reference_t<function_remove_rvalue_reference_t<T>>;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_cvref_t = function_remove_cv_t<function_remove_reference_t<T>>;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_qualifiers_t = function_remove_cvref_t<function_remove_noexcept_t<T>>;

  // clang-format on

#define RUBY_ADD_CVREF_000
#define RUBY_ADD_CVREF_010 volatile
#define RUBY_ADD_CVREF_100 const
#define RUBY_ADD_CVREF_110 const volatile
#define RUBY_ADD_CVREF_001 &
#define RUBY_ADD_CVREF_011 volatile &
#define RUBY_ADD_CVREF_101 const &
#define RUBY_ADD_CVREF_111 const volatile &
#define RUBY_ADD_CVREF_002 &&
#define RUBY_ADD_CVREF_012 volatile &&
#define RUBY_ADD_CVREF_102 const &&
#define RUBY_ADD_CVREF_112 const volatile &&

#define RUBY_VARIADIC_0
#define RUBY_VARIADIC_1 , ...

#define RUBY_CONCAT2(X, Y) X##Y
#define RUBY_CONCAT4(X, A, B, C) X##A##B##C
#define RUBY_MAYBE_CVREF(X, Y, Z) RUBY_CONCAT4(RUBY_ADD_CVREF_, X, Y, Z)
#define RUBY_MAYBE_VARIADIC(X) RUBY_CONCAT2(RUBY_VARIADIC_, X)

#define RUBY_DEFINE_MAKE_FUNCTION_IMPL(C, V, R, IV, Qual, Pack) \
  template<bool IN, typename Ret, typename... Args>        \
  struct make_function<C, V, R, IV, IN, Ret, Args...>      \
  {                                                        \
    using type = Ret(Args... Pack) Qual noexcept(IN);      \
  };

#define RUBY_DEFINE_MAKE_FUNCTION(C, V, R, IV) \
  RUBY_DEFINE_MAKE_FUNCTION_IMPL(C, V, R, IV, RUBY_MAYBE_CVREF(C, V, R), RUBY_MAYBE_VARIADIC(IV))

#define RUBY_DEFINE_FUNCTION_TRAITS_IMPL(C, V, R, IV, Qual, Pack)  \
  template<bool IN, typename Ret, typename... Args>           \
  struct function_traits<Ret(Args... Pack) Qual noexcept(IN)> \
    : function_types<C, V, R, IV, IN, Ret, Args...>           \
  {};

#define RUBY_DEFINE_FUNCTION_TRAITS(C, V, R, IV) \
  RUBY_DEFINE_FUNCTION_TRAITS_IMPL(C, V, R, IV, RUBY_MAYBE_CVREF(C, V, R), RUBY_MAYBE_VARIADIC(IV))

  RUBY_DEFINE_MAKE_FUNCTION(0, 0, 0, 0);
  RUBY_DEFINE_MAKE_FUNCTION(0, 0, 0, 1);
  RUBY_DEFINE_MAKE_FUNCTION(0, 0, 1, 0);
  RUBY_DEFINE_MAKE_FUNCTION(0, 0, 1, 1);
  RUBY_DEFINE_MAKE_FUNCTION(0, 1, 0, 0);
  RUBY_DEFINE_MAKE_FUNCTION(0, 1, 0, 1);
  RUBY_DEFINE_MAKE_FUNCTION(0, 1, 1, 0);
  RUBY_DEFINE_MAKE_FUNCTION(0, 1, 1, 1);
  RUBY_DEFINE_MAKE_FUNCTION(1, 0, 0, 0);
  RUBY_DEFINE_MAKE_FUNCTION(1, 0, 0, 1);
  RUBY_DEFINE_MAKE_FUNCTION(1, 0, 1, 0);
  RUBY_DEFINE_MAKE_FUNCTION(1, 0, 1, 1);
  RUBY_DEFINE_MAKE_FUNCTION(1, 1, 0, 0);
  RUBY_DEFINE_MAKE_FUNCTION(1, 1, 0, 1);
  RUBY_DEFINE_MAKE_FUNCTION(1, 1, 1, 0);
  RUBY_DEFINE_MAKE_FUNCTION(1, 1, 1, 1);

  RUBY_DEFINE_MAKE_FUNCTION(0, 0, 2, 0);
  RUBY_DEFINE_MAKE_FUNCTION(0, 0, 2, 1);
  RUBY_DEFINE_MAKE_FUNCTION(0, 1, 2, 0);
  RUBY_DEFINE_MAKE_FUNCTION(0, 1, 2, 1);
  RUBY_DEFINE_MAKE_FUNCTION(1, 0, 2, 0);
  RUBY_DEFINE_MAKE_FUNCTION(1, 0, 2, 1);
  RUBY_DEFINE_MAKE_FUNCTION(1, 1, 2, 0);
  RUBY_DEFINE_MAKE_FUNCTION(1, 1, 2, 1);

  RUBY_DEFINE_FUNCTION_TRAITS(0, 0, 0, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 0, 0, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 0, 1, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 0, 1, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 1, 0, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 1, 0, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 1, 1, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 1, 1, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 0, 0, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 0, 0, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 0, 1, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 0, 1, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 1, 0, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 1, 0, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 1, 1, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 1, 1, 1);

  RUBY_DEFINE_FUNCTION_TRAITS(0, 0, 2, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 0, 2, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 1, 2, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(0, 1, 2, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 0, 2, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 0, 2, 1);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 1, 2, 0);
  RUBY_DEFINE_FUNCTION_TRAITS(1, 1, 2, 1);

#undef RUBY_DEFINE_FUNCTION_TRAITS
#undef RUBY_DEFINE_FUNCTION_TRAITS_IMPL

#undef RUBY_DEFINE_MAKE_FUNCTION
#undef RUBY_DEFINE_MAKE_FUNCTION_IMPL

#undef RUBY_CONCAT2
#undef RUBY_CONCAT4

#undef RUBY_ADD_CVREF_000
#undef RUBY_ADD_CVREF_010
#undef RUBY_ADD_CVREF_100
#undef RUBY_ADD_CVREF_110
#undef RUBY_ADD_CVREF_001
#undef RUBY_ADD_CVREF_011
#undef RUBY_ADD_CVREF_101
#undef RUBY_ADD_CVREF_111
#undef RUBY_ADD_CVREF_002
#undef RUBY_ADD_CVREF_012
#undef RUBY_ADD_CVREF_102
#undef RUBY_ADD_CVREF_112

#undef RUBY_VARIADIC_0
#undef RUBY_VARIADIC_1

#undef RUBY_MAYBE_VARIADIC
#undef RUBY_MAYBE_CVREF

} // namespace inv

#endif
#ifndef RUBY_INVOCABLE_MEMBER_FUNCTION_POINTER_TRAITS_HPP
#define RUBY_INVOCABLE_MEMBER_FUNCTION_POINTER_TRAITS_HPP

#include <functional>
#include <type_traits>

namespace ruby::invocable
{
  template<typename T>
  struct member_function_pointer_traits
  {};

  // clang-format off

  template<typename T, typename C>
    requires std::is_function_v<T> 
  struct member_function_pointer_traits<T C::*>
  {
    using function_type = T;
    using class_type = C;
  };

  template<typename T>
    requires std::is_member_function_pointer_v<T>
  using member_function_pointer_function_t = typename member_function_pointer_traits<T>::function_type;

  template<typename T>
    requires std::is_member_function_pointer_v<T>
  using member_function_pointer_class_t = typename member_function_pointer_traits<T>::class_type;

  // clang-format on

} // namespace ruby

#endif
#ifndef RUBY_INVOCABLE_MEMBER_OBJECT_POINTER_TRAITS_HPP
#define RUBY_INVOCABLE_MEMBER_OBJECT_POINTER_TRAITS_HPP

#include <functional>
#include <type_traits>

namespace ruby::invocable
{
  template<typename T>
  struct member_object_pointer_traits
  {};

  // clang-format off

  template<typename T, typename C>
    requires(!std::is_function_v<T>) 
  struct member_object_pointer_traits<T C::*>
  {
    using object_type = T;
    using class_type = C;
  };

  template<typename T>
    requires std::is_member_object_pointer_v<T>
  using member_object_pointer_object_t = typename member_object_pointer_traits<T>::object_type;

  template<typename T>
    requires std::is_member_object_pointer_v<T>
  using member_object_pointer_class_t = typename member_object_pointer_traits<T>::class_type;

  // clang-format on

} // namespace ruby

#endif

namespace ruby::invocable
{

  template<typename T>
  struct invocable_traits
  {};

  // clang-format off

  namespace invocable_impl{
    template<typename T>
    inline constexpr bool is_reference_wrapper_v = false;

    template<typename T>
    inline constexpr bool is_reference_wrapper_v<std::reference_wrapper<T>> = true;
    
    template<typename T>
    concept InvokeDeducible =
      std::is_function_v<T> ||
      std::is_member_function_pointer_v<T> ||
      std::is_member_object_pointer_v<T> ||
      std::is_member_function_pointer_v<decltype(&T::operator())>;
  }
  
  template<typename T>
  concept InvokeDeducible= 
    invocable_impl::InvokeDeducible<T> ||
    (std::is_reference_v<T> && invocable_impl::InvokeDeducible<std::remove_reference_t<T>>) ||
    (std::is_pointer_v<T> && invocable_impl::InvokeDeducible<std::remove_pointer_t<T>>) ||
    (invocable_impl::is_reference_wrapper_v<T> && invocable_impl::InvokeDeducible<typename T::type>);

  template<typename T>
    requires std::is_function_v<T>
  struct invocable_traits<T> : function_traits<T>{};

  template<typename T>
    requires  std::is_member_function_pointer_v<T>
  struct invocable_traits<T> : invocable_traits<member_function_pointer_function_t<T>>{};
  
  template<typename T>
    requires std::is_member_object_pointer_v<T>
  struct invocable_traits<T> : function_types<0,0,0,0,0,member_object_pointer_object_t<T>>{};

  template<typename T>
    requires std::is_member_function_pointer_v<decltype(&T::operator())>
  struct invocable_traits<T> : invocable_traits<decltype(&T::operator())>
  {};

  template<typename T>
  struct invocable_traits<std::reference_wrapper<T>> : invocable_traits<T>
  {};

  template<typename T>
  struct invocable_traits<T&> : invocable_traits<T>{};

  template<typename T>
  struct invocable_traits<T*> : invocable_traits<T>{};
  
  template<InvokeDeducible T>
  using invocable_function_t = typename invocable_traits<T>::function_type;

  template<InvokeDeducible T>
  using invocable_ret_t = function_ret_t<invocable_function_t<T>>;

  template<InvokeDeducible T>
  using invocable_args_t = function_args_t<invocable_function_t<T>>;

  template<InvokeDeducible T, std::size_t index>
  using invocable_arg_t = function_arg_t<invocable_function_t<T>, index>;

  namespace invocable_impl{
    struct ARGUMENT_TYPE_IS_NOT_DEDUCIBLE{};

    template<typename T>
    using maybe_function_t = std::conditional_t<ruby::invocable::InvokeDeducible<T>, invocable_function_t<T>, ARGUMENT_TYPE_IS_NOT_DEDUCIBLE>;
  }

  // clang-format on

  template<typename T>
  inline constexpr auto invocable_arity_v = function_arity_v<invocable_impl::maybe_function_t<T>>;

  template<typename T>
  inline constexpr auto invocable_is_const_v =
      function_is_const_v<invocable_impl::maybe_function_t<T>>;

  template<typename T>
  inline constexpr auto invocable_is_volatile_v =
      function_is_volatile_v<invocable_impl::maybe_function_t<T>>;

  template<typename T>
  inline constexpr auto invocable_is_variadic_v =
      function_is_variadic_v<invocable_impl::maybe_function_t<T>>;

  template<typename T>
  inline constexpr auto invocable_is_noexcept_v =
      function_is_noexcept_v<invocable_impl::maybe_function_t<T>>;

  template<typename T>
  inline constexpr auto invocable_is_lvalue_reference_v =
      function_is_lvalue_reference_v<invocable_impl::maybe_function_t<T>>;

  template<typename T>
  inline constexpr auto invocable_is_rvalue_reference_v =
      function_is_rvalue_reference_v<invocable_impl::maybe_function_t<T>>;

  template<typename T>
  inline constexpr auto invocable_is_reference_v =
      function_is_reference_v<invocable_impl::maybe_function_t<T>>;
} // namespace ruby::invocable

#endif
