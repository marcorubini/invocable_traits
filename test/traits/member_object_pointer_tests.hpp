
#include <concepts>
#include <ruby/invocable_traits/member_object_pointer_traits.hpp>

namespace member_object_pointer_tests
{
  using namespace ruby::inv;

  namespace test0
  {
    struct Fn0
    {
      int volatile x;
    };

    struct Fn1
    {
      int x() const;
    };

    template<typename T>
    concept CanDeduceMemberObject = requires
    {
      typename member_object_pointer_object_t<T>;
    };

    inline void test_member_object_pointer_object_t()
    {
      using x0 = decltype(&Fn0::x);
      using x1 = decltype(&Fn1::x);
      using x2 = int const Fn1::*;

      static_assert(CanDeduceMemberObject<x0>);
      static_assert(!CanDeduceMemberObject<x1>);

      static_assert(std::same_as<member_object_pointer_object_t<x0>, int volatile>);
      static_assert(std::same_as<member_object_pointer_object_t<x2>, int const>);
    }
  } // namespace test0

  namespace test1
  {
    struct Fn0
    {
      int volatile x;
    };

    struct Fn1
    {
      int x() const;
    };

    template<typename T>
    concept CanDeduceMemberClass = requires
    {
      typename member_object_pointer_class_t<T>;
    };

    inline void test_member_object_pointer_class_t()
    {
      using x0 = decltype(&Fn0::x);
      using x1 = decltype(&Fn1::x);

      static_assert(CanDeduceMemberClass<x0>);
      static_assert(!CanDeduceMemberClass<x1>);

      static_assert(std::same_as<member_object_pointer_class_t<x0>, Fn0>);
    }
  } // namespace test1

} // namespace member_object_pointer_tests

