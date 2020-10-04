#ifndef INV_MEMBER_OBJECT_POINTER_TRAITS_HPP
#define INV_MEMBER_OBJECT_POINTER_TRAITS_HPP

#include <functional>
#include <type_traits>

namespace inv
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

} // namespace inv

#endif
