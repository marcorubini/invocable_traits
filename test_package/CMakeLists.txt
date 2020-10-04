cmake_minimum_required(VERSION 3.18.2)
project(marcorubini_invocable_traits_test_package)


find_package(marcorubini_invocable_traits REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(test test.cpp)
target_link_libraries(test PRIVATE marcorubini::marcorubini_invocable_traits)

