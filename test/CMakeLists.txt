
add_executable(constexpr_tests constexpr_tests.cpp)
target_link_libraries(constexpr_tests PRIVATE ${main_target})
add_test(NAME ConstexprTests COMMAND constexpr_tests)

