#ifndef INV_TEST_TRAITS_INVOCABLE
#define INV_TEST_TRAITS_INVOCABLE

#include <ruby/invocable_traits/invocable_traits.hpp>

namespace invocable_tests
{
  using namespace ruby::invocable;
  
  constexpr auto fn1 = [](int x){ return 0; };
  constexpr auto fn2 = [](int x) mutable{ return 0.0f; };
  constexpr auto fn3 = [](int x) mutable noexcept{ return 0.0; };

  using Fn0 = void(int);
  using Fn1 = decltype(fn1);
  using Fn2 = decltype(fn2);
  using Fn3 = decltype(fn3);

  struct Fn4{
    char x;
  };

  inline void test_deducible_invocable_function()
  {
    static_assert( InvokeDeducible<Fn0> );
  }

  inline void test_deducible_invocable_member_function()
  {
    static_assert( InvokeDeducible<decltype(&Fn1::operator())> );
    static_assert( InvokeDeducible<decltype(&Fn2::operator())> );
    static_assert( InvokeDeducible<decltype(&Fn3::operator())> );
  }

  inline void test_deducible_invocable_member_object()
  {
    static_assert( InvokeDeducible<decltype(&Fn4::x)> );
    using const_mem = int const Fn4::*;
    static_assert( InvokeDeducible<const_mem> );
    static_assert( std::same_as<invocable_ret_t<const_mem>, int const> );
    static_assert( std::same_as<invocable_arg_t<const_mem, 0>, Fn4 const&> );
  }

  inline void test_deducible_invocable_functor()
  {
    static_assert( InvokeDeducible<Fn1> );
    static_assert( InvokeDeducible<Fn2> );
    static_assert( InvokeDeducible<Fn3> );
  }

  inline void test_deducible_invocable_function_reference()
  {
    static_assert( InvokeDeducible<Fn0&> );
    static_assert( InvokeDeducible<Fn0&&> );
    static_assert( InvokeDeducible<Fn1&> );
    static_assert( InvokeDeducible<Fn1&&> );
    static_assert( InvokeDeducible<Fn2&> );
    static_assert( InvokeDeducible<Fn2&&> );
    static_assert( InvokeDeducible<Fn3&> );
    static_assert( InvokeDeducible<Fn3&&> );
    static_assert( InvokeDeducible<decltype(&Fn4::x) &> );
    static_assert( InvokeDeducible<decltype(&Fn4::x) &&> );
  }

  inline void test_deducible_invocable_function_pointer()
  {
    static_assert( InvokeDeducible<Fn0*> );
    static_assert( InvokeDeducible<Fn1*> );
    static_assert( InvokeDeducible<Fn2*> );
    static_assert( InvokeDeducible<Fn3*> );
    static_assert( InvokeDeducible<decltype(&Fn4::x) &> );
  }

  inline void test_deducible_invocable_reference_wrapper()
  {
    static_assert( InvokeDeducible<std::reference_wrapper<Fn0>> );
    static_assert( InvokeDeducible<std::reference_wrapper<Fn1>> );
    static_assert( InvokeDeducible<std::reference_wrapper<Fn2>> );
    static_assert( InvokeDeducible<std::reference_wrapper<Fn3>> );
    static_assert( InvokeDeducible<decltype(&Fn4::x)> );
    static_assert( ! InvokeDeducible<std::reference_wrapper<int>> );
  }

  inline void test_invocable_function_t()
  {
    static_assert( std::same_as<invocable_function_t<Fn0>, void(int)> );
    static_assert( std::same_as<invocable_function_t<Fn1>, int(int) const> );
    static_assert( std::same_as<invocable_function_t<Fn2>, float(int)> );
    static_assert( std::same_as<invocable_function_t<Fn3>, double(int) noexcept> );
    static_assert( std::same_as<invocable_function_t<decltype(&Fn4::x)>, char(Fn4&)> );
  }

}

#endif
