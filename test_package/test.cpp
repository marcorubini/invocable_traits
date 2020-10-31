#include <ruby/invocable_traits/invocable_traits.hpp>
#include <iostream>

static_assert( inv::function_is_const_v<void() const> );

int main()
{
  puts("OK");
}
