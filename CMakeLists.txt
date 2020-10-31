cmake_minimum_required(VERSION 3.18.3)
project(
  invocable_traits
  VERSION 0.0.1
  DESCRIPTION "C++20 function object destructuring and type traits."
  LANGUAGES CXX
  HOMEPAGE_URL "https://github.com/marcorubini/invocable_traits")

set(header_files
    ${PROJECT_SOURCE_DIR}/include/ruby/invocable_traits/function_traits.hpp
    ${PROJECT_SOURCE_DIR}/include/ruby/invocable_traits/member_function_pointer_traits.hpp
    ${PROJECT_SOURCE_DIR}/include/ruby/invocable_traits/member_object_pointer_traits.hpp
    ${PROJECT_SOURCE_DIR}/include/ruby/invocable_traits/invocable_traits.hpp
)

# main target
set(main_target ruby_${PROJECT_NAME})
add_library(${main_target} INTERFACE)
add_library(ruby::${main_target} ALIAS ${main_target})

target_sources(${main_target} INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_compile_features(${main_target} INTERFACE cxx_std_20)
target_include_directories(
  ${main_target} SYSTEM
  INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>"
            "$<INSTALL_INTERFACE:include/>")

if(NOT dependency_via_submodule)
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(${main_target}-config-version.cmake
                                   COMPATIBILITY ExactVersion)
  install(
    TARGETS ${main_target}
    EXPORT ${main_target}_targets
    INCLUDES
    DESTINATION include)
  install(
    EXPORT ${main_target}_targets
    DESTINATION lib/cmake/${main_target}
    FILE ${main_target}-targets.cmake
    NAMESPACE ruby::)
  install(DIRECTORY include/ DESTINATION include)
  install(FILES ${main_target}-config.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/${main_target}-config-version.cmake
          DESTINATION lib/cmake/${main_target})
else()
  message(STATUS "Dependency installed via submodule, installation unavailable")
endif()

# testing
option(INVOCABLE_TRAITS_BUILD_TEST "build tests of ruby/${PROJECT_NAME}"
       OFF)
if(${INVOCABLE_TRAITS_BUILD_TEST} OR (CMAKE_CURRENT_SOURCE_DIR STREQUAL
                                      CMAKE_SOURCE_DIR))
  set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
  message("Building of ${PROJECT_NAME} constexpr tests enabled.")
  enable_testing()
  add_subdirectory(test)
endif()
