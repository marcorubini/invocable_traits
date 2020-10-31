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