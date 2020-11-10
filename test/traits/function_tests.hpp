#ifndef INV_TEST_TRAITS_FUNCTION_HPP
#define INV_TEST_TRAITS_FUNCTION_HPP

#include <ruby/invocable_traits/function_traits.hpp>

namespace function_tests
{
  using namespace ruby::invocable;

  using Fn0 = int();
  using Fn1 = int const();
  using Fn2 = int &();
  using Fn3 = int const &();
  using Fn4 = int && ();
  using Fn5 = int const && ();

  inline void test_function_ret_t()
  {
    static_assert(std::same_as<function_ret_t<Fn0>, int>);
    static_assert(std::same_as<function_ret_t<Fn1>, int const>);
    static_assert(std::same_as<function_ret_t<Fn2>, int &>);
    static_assert(std::same_as<function_ret_t<Fn3>, int const &>);
    static_assert(std::same_as<function_ret_t<Fn4>, int &&>);
    static_assert(std::same_as<function_ret_t<Fn5>, int const &&>);
  }

  using Fn6 = void();
  using Fn7 = void(int);
  using Fn8 = void(int, int);
  using Fn9 = void(int, int, int);

  inline void test_function_args_t()
  {
    static_assert(std::same_as<function_args_t<Fn6>, std::tuple<>>);
    static_assert(std::same_as<function_args_t<Fn7>, std::tuple<int>>);
    static_assert(std::same_as<function_args_t<Fn8>, std::tuple<int, int>>);
    static_assert(std::same_as<function_args_t<Fn9>, std::tuple<int, int, int>>);
  }

  using Fn10 = void(int, float, double);
  using Fn11 = void(int const &, float &, double &&);

  inline void test_function_arg_t()
  {
    static_assert(std::same_as<function_arg_t<Fn10, 0>, int>);
    static_assert(std::same_as<function_arg_t<Fn10, 1>, float>);
    static_assert(std::same_as<function_arg_t<Fn10, 2>, double>);

    static_assert(std::same_as<function_arg_t<Fn11, 0>, int const &>);
    static_assert(std::same_as<function_arg_t<Fn11, 1>, float &>);
    static_assert(std::same_as<function_arg_t<Fn11, 2>, double &&>);
  }

  inline void test_function_arity_v()
  {
    static_assert(function_arity_v<Fn6> == 0);
    static_assert(function_arity_v<Fn7> == 1);
    static_assert(function_arity_v<Fn8> == 2);
  }

  using Fn12 = void() const;
  using Fn13 = void();
  struct NotFunction
  {};

  inline void test_function_is_const_v()
  {
    static_assert(function_is_const_v<Fn12>);
    static_assert(!function_is_const_v<Fn13>);
    static_assert(!function_is_const_v<NotFunction>);
  }

  using Fn14 = void() volatile;

  inline void test_function_is_volatile_v()
  {
    static_assert(function_is_volatile_v<Fn14>);
    static_assert(!function_is_volatile_v<Fn13>);
    static_assert(!function_is_volatile_v<NotFunction>);
  }

  using Fn15 = void() &;
  using Fn16 = void() &&;

  inline void test_function_is_lvalue_reference_v()
  {
    static_assert(function_is_lvalue_reference_v<Fn15>);
    static_assert(!function_is_lvalue_reference_v<Fn16>);
    static_assert(!function_is_lvalue_reference_v<Fn13>);
    static_assert(!function_is_lvalue_reference_v<NotFunction>);
  }

  inline void test_function_is_rvalue_reference_v()
  {
    static_assert(function_is_rvalue_reference_v<Fn16>);
    static_assert(!function_is_rvalue_reference_v<Fn15>);
    static_assert(!function_is_rvalue_reference_v<Fn13>);
    static_assert(!function_is_rvalue_reference_v<NotFunction>);
  }

  inline void test_function_is_reference_v()
  {
    static_assert(function_is_reference_v<Fn15>);
    static_assert(function_is_reference_v<Fn16>);
    static_assert(!function_is_reference_v<Fn13>);
    static_assert(!function_is_reference_v<NotFunction>);
  }

  using Fn17 = void(...);

  inline void test_function_is_variadic_v()
  {
    static_assert(function_is_variadic_v<Fn17>);
    static_assert(!function_is_variadic_v<Fn13>);
    static_assert(!function_is_variadic_v<NotFunction>);
  }

  using Fn18 = void() noexcept;

  inline void test_function_is_noexcept()
  {
    static_assert(function_is_noexcept_v<Fn18>);
    static_assert(!function_is_noexcept_v<Fn13>);
    static_assert(!function_is_noexcept_v<NotFunction>);
  }

  inline void test_function_modify_const()
  {
    static_assert(std::same_as<function_add_const_t<void()>, void() const>);
    static_assert(std::same_as<function_add_const_t<void() const>, void() const>);
    static_assert(std::same_as<function_remove_const_t<void() const>, void()>);
    static_assert(std::same_as<function_remove_const_t<void()>, void()>);
  }

  inline void test_function_modify_volatile()
  {
    static_assert(std::same_as<function_add_volatile_t<void()>, void() volatile>);
    static_assert(std::same_as<function_add_volatile_t<void() volatile>, void() volatile>);
    static_assert(std::same_as<function_remove_volatile_t<void() volatile>, void()>);
    static_assert(std::same_as<function_remove_volatile_t<void()>, void()>);
  }

  inline void test_function_modify_cv()
  {
    static_assert(std::same_as<function_add_cv_t<void()>, void() const volatile>);
    static_assert(std::same_as<function_add_cv_t<void() volatile>, void() const volatile>);
    static_assert(std::same_as<function_add_cv_t<void() const>, void() const volatile>);

    static_assert(std::same_as<function_remove_cv_t<void() const volatile>, void()>);
    static_assert(std::same_as<function_remove_cv_t<void() const>, void()>);
    static_assert(std::same_as<function_remove_cv_t<void() volatile>, void()>);
  }

  inline void test_function_modify_variadic()
  {
    static_assert(std::same_as<function_add_variadic_t<void()>, void(...)>);
    static_assert(std::same_as<function_add_variadic_t<void(...)>, void(...)>);
    static_assert(std::same_as<function_remove_variadic_t<void()>, void()>);
    static_assert(std::same_as<function_remove_variadic_t<void(...)>, void()>);
  }

  inline void test_function_modify_noexcept()
  {
    static_assert(std::same_as<function_add_noexcept_t<void()>, void() noexcept>);
    static_assert(std::same_as<function_add_noexcept_t<void() noexcept>, void() noexcept>);
    static_assert(std::same_as<function_remove_noexcept_t<void() noexcept>, void()>);
    static_assert(std::same_as<function_remove_noexcept_t<void()>, void()>);
  }

  inline void test_function_remove_qualifiers()
  {
    static_assert(std::same_as<function_remove_lvalue_reference_t<void() &>, void()>);
    static_assert(std::same_as<function_remove_lvalue_reference_t<void()>, void()>);

    static_assert(std::same_as<function_remove_rvalue_reference_t<void() &&>, void()>);
    static_assert(std::same_as<function_remove_rvalue_reference_t<void()>, void()>);

    static_assert(std::same_as<function_remove_reference_t<void() &>, void()>);
    static_assert(std::same_as<function_remove_reference_t<void() &&>, void()>);
    static_assert(std::same_as<function_remove_reference_t<void()>, void()>);

    static_assert(
        std::same_as<function_remove_cvref_t<void() const volatile && noexcept>, void() noexcept>);
    static_assert(
        std::same_as<function_remove_qualifiers_t<void() const volatile && noexcept>, void()>);

    static_assert(std::same_as<function_remove_cvref_t<void()>, void()>);
    static_assert(std::same_as<function_remove_qualifiers_t<void()>, void()>);
  }

} // namespace function_tests

#endif
