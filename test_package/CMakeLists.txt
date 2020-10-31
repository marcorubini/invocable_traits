cmake_minimum_required(VERSION 3.18.2)
project(ruby_invocable_traits_test_package)


find_package(ruby_invocable_traits REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(test test.cpp)
target_link_libraries(test PRIVATE ruby::ruby_invocable_traits)

