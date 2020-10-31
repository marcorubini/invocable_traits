#ifndef RUBY_INVOCABLE_TRAITS_HPP
#define RUBY_INVOCABLE_TRAITS_HPP

#include "./function_traits.hpp"
#include "./member_function_pointer_traits.hpp"
#include "./member_object_pointer_traits.hpp"

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
  struct invocable_traits<T> : function_types<0,0,0,0,0,member_object_pointer_object_t<T>, member_object_pointer_class_t<T>* >{};

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