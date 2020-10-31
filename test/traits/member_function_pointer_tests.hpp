#ifndef INV_TEST_TRAITS_MEMBER_FUNCTION_POINTER
#define INV_TEST_TRAITS_MEMBER_FUNCTION_POINTER

#include <concepts>
#include <ruby/invocable_traits/member_function_pointer_traits.hpp>

namespace member_function_pointer_tests
{
  using namespace ruby;

  namespace test0
  {

    struct Fn0
    {
      int fun1();
      int fun2(int x) const noexcept;
    };

    struct Fn1
    {
      int fun;
    };

    template<typename T>
    concept CanDeduceFunction = requires
    {
      typename member_function_pointer_function_t<T>;
    };

    inline void test_member_function_pointer_function_t()
    {
      static_assert(CanDeduceFunction<decltype(&Fn0::fun1)>);
      static_assert(!CanDeduceFunction<decltype(&Fn1::fun)>);

      static_assert(std::same_as<member_function_pointer_function_t<decltype(&Fn0::fun1)>, int()>);
      static_assert(std::same_as<member_function_pointer_function_t<decltype(&Fn0::fun2)>,
                                 int(int) const noexcept>);
    }

  } // namespace test0

  namespace test1
  {
    struct Fn0
    {
      int fun();
    };

    struct Fn1
    {
      int fun;
    };

    template<typename T>
    concept CanDeduceClass = requires
    {
      typename member_function_pointer_class_t<T>;
    };

    inline void test_member_function_pointer_class_t()
    {
      static_assert(CanDeduceClass<decltype(&Fn0::fun)>);
      static_assert(!CanDeduceClass<decltype(&Fn1::fun)>);

      static_assert(std::same_as<member_function_pointer_class_t<decltype(&Fn0::fun)>, Fn0>);
    }

  } // namespace test1
} // namespace member_function_pointer_tests

#endif
