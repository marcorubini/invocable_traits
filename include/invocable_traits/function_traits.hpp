#ifndef INV_FUNCTION_TRAITS_HPP
#define INV_FUNCTION_TRAITS_HPP

#include "hedley.hpp"

#include <tuple>
#include <type_traits>
#include <utility>

namespace inv
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

  namespace impl
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
  using function_add_const_t = typename impl::function_modify<T>::add_const;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_const_t = typename impl::function_modify<T>::remove_const;

  template<typename T>
    requires std::is_function_v<T>
  using function_add_volatile_t = typename impl::function_modify<T>::add_volatile;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_volatile_t = typename impl::function_modify<T>::remove_volatile;

  template<typename T>
    requires std::is_function_v<T>
  using function_add_cv_t = function_add_const_t<function_add_volatile_t<T>>;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_cv_t = function_remove_const_t<function_remove_volatile_t<T>>;

  template<typename T>
    requires std::is_function_v<T>
  using function_add_variadic_t = typename impl::function_modify<T>::add_variadic;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_variadic_t = typename impl::function_modify<T>::remove_variadic;

  template<typename T>
    requires std::is_function_v<T>
  using function_add_noexcept_t = typename impl::function_modify<T>::add_noexcept;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_noexcept_t = typename impl::function_modify<T>::remove_noexcept;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_lvalue_reference_t =
      typename impl::function_modify<T>::remove_lvalue_reference;

  template<typename T>
    requires std::is_function_v<T>
  using function_remove_rvalue_reference_t =
      typename impl::function_modify<T>::remove_rvalue_reference;

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

#define ADD_CVREF_000
#define ADD_CVREF_010 volatile
#define ADD_CVREF_100 const
#define ADD_CVREF_110 const volatile
#define ADD_CVREF_001 &
#define ADD_CVREF_011 volatile &
#define ADD_CVREF_101 const &
#define ADD_CVREF_111 const volatile &
#define ADD_CVREF_002 &&
#define ADD_CVREF_012 volatile &&
#define ADD_CVREF_102 const &&
#define ADD_CVREF_112 const volatile &&

#define VARIADIC_0
#define VARIADIC_1 ...

#define CONCAT2(X, Y) X##Y
#define CONCAT4(X, A, B, C) X##A##B##C
#define MAYBE_CVREF(X, Y, Z) CONCAT4(ADD_CVREF_, X, Y, Z)
#define MAYBE_VARIADIC(X) CONCAT2(VARIADIC_, X)

#define DEFINE_MAKE_FUNCTION_IMPL(C, V, R, IV, Qual, Pack) \
  template<bool IN, typename Ret, typename... Args>        \
  struct make_function<C, V, R, IV, IN, Ret, Args...>      \
  {                                                        \
    using type = Ret(Args... Pack) Qual noexcept(IN);      \
  };

#define DEFINE_MAKE_FUNCTION(C, V, R, IV) \
  DEFINE_MAKE_FUNCTION_IMPL(C, V, R, IV, MAYBE_CVREF(C, V, R), MAYBE_VARIADIC(IV))

#define DEFINE_FUNCTION_TRAITS_IMPL(C, V, R, IV, Qual, Pack)  \
  template<bool IN, typename Ret, typename... Args>           \
  struct function_traits<Ret(Args... Pack) Qual noexcept(IN)> \
    : function_types<C, V, R, IV, IN, Ret, Args...>           \
  {};

#define DEFINE_FUNCTION_TRAITS(C, V, R, IV) \
  DEFINE_FUNCTION_TRAITS_IMPL(C, V, R, IV, MAYBE_CVREF(C, V, R), MAYBE_VARIADIC(IV))

  HEDLEY_DIAGNOSTIC_PUSH
  HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wambiguous-ellipsis"

  DEFINE_MAKE_FUNCTION(0, 0, 0, 0);
  DEFINE_MAKE_FUNCTION(0, 0, 0, 1);
  DEFINE_MAKE_FUNCTION(0, 0, 1, 0);
  DEFINE_MAKE_FUNCTION(0, 0, 1, 1);
  DEFINE_MAKE_FUNCTION(0, 1, 0, 0);
  DEFINE_MAKE_FUNCTION(0, 1, 0, 1);
  DEFINE_MAKE_FUNCTION(0, 1, 1, 0);
  DEFINE_MAKE_FUNCTION(0, 1, 1, 1);
  DEFINE_MAKE_FUNCTION(1, 0, 0, 0);
  DEFINE_MAKE_FUNCTION(1, 0, 0, 1);
  DEFINE_MAKE_FUNCTION(1, 0, 1, 0);
  DEFINE_MAKE_FUNCTION(1, 0, 1, 1);
  DEFINE_MAKE_FUNCTION(1, 1, 0, 0);
  DEFINE_MAKE_FUNCTION(1, 1, 0, 1);
  DEFINE_MAKE_FUNCTION(1, 1, 1, 0);
  DEFINE_MAKE_FUNCTION(1, 1, 1, 1);

  DEFINE_MAKE_FUNCTION(0, 0, 2, 0);
  DEFINE_MAKE_FUNCTION(0, 0, 2, 1);
  DEFINE_MAKE_FUNCTION(0, 1, 2, 0);
  DEFINE_MAKE_FUNCTION(0, 1, 2, 1);
  DEFINE_MAKE_FUNCTION(1, 0, 2, 0);
  DEFINE_MAKE_FUNCTION(1, 0, 2, 1);
  DEFINE_MAKE_FUNCTION(1, 1, 2, 0);
  DEFINE_MAKE_FUNCTION(1, 1, 2, 1);

  DEFINE_FUNCTION_TRAITS(0, 0, 0, 0);
  DEFINE_FUNCTION_TRAITS(0, 0, 0, 1);
  DEFINE_FUNCTION_TRAITS(0, 0, 1, 0);
  DEFINE_FUNCTION_TRAITS(0, 0, 1, 1);
  DEFINE_FUNCTION_TRAITS(0, 1, 0, 0);
  DEFINE_FUNCTION_TRAITS(0, 1, 0, 1);
  DEFINE_FUNCTION_TRAITS(0, 1, 1, 0);
  DEFINE_FUNCTION_TRAITS(0, 1, 1, 1);
  DEFINE_FUNCTION_TRAITS(1, 0, 0, 0);
  DEFINE_FUNCTION_TRAITS(1, 0, 0, 1);
  DEFINE_FUNCTION_TRAITS(1, 0, 1, 0);
  DEFINE_FUNCTION_TRAITS(1, 0, 1, 1);
  DEFINE_FUNCTION_TRAITS(1, 1, 0, 0);
  DEFINE_FUNCTION_TRAITS(1, 1, 0, 1);
  DEFINE_FUNCTION_TRAITS(1, 1, 1, 0);
  DEFINE_FUNCTION_TRAITS(1, 1, 1, 1);

  DEFINE_FUNCTION_TRAITS(0, 0, 2, 0);
  DEFINE_FUNCTION_TRAITS(0, 0, 2, 1);
  DEFINE_FUNCTION_TRAITS(0, 1, 2, 0);
  DEFINE_FUNCTION_TRAITS(0, 1, 2, 1);
  DEFINE_FUNCTION_TRAITS(1, 0, 2, 0);
  DEFINE_FUNCTION_TRAITS(1, 0, 2, 1);
  DEFINE_FUNCTION_TRAITS(1, 1, 2, 0);
  DEFINE_FUNCTION_TRAITS(1, 1, 2, 1);

  HEDLEY_DIAGNOSTIC_POP

#undef DEFINE_FUNCTION_TRAITS
#undef DEFINE_FUNCTION_TRAITS_IMPL

#undef DEFINE_MAKE_FUNCTION
#undef DEFINE_MAKE_FUNCTION_IMPL

#undef CONCAT2
#undef CONCAT4

#undef ADD_CVREF_000
#undef ADD_CVREF_010
#undef ADD_CVREF_100
#undef ADD_CVREF_110
#undef ADD_CVREF_001
#undef ADD_CVREF_011
#undef ADD_CVREF_101
#undef ADD_CVREF_111
#undef ADD_CVREF_002
#undef ADD_CVREF_012
#undef ADD_CVREF_102
#undef ADD_CVREF_112

#undef VARIADIC_0
#undef VARIADIC_1

#undef MAYBE_VARIADIC
#undef MAYBE_CVREF

} // namespace inv

#endif
