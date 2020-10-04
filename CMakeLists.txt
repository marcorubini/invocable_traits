cmake_minimum_required(VERSION 3.18.3)
project(
  invocable_traits
  VERSION 0.0.1
  DESCRIPTION "C++20 function object destructuring and type traits."
  LANGUAGES CXX
  HOMEPAGE_URL "https://github.com/marcorubini/invocable_traits")

set(header_files
    ${PROJECT_SOURCE_DIR}/include/marcorubini/invocable_traits/function_traits.hpp
    ${PROJECT_SOURCE_DIR}/include/marcorubini/invocable_traits/member_function_pointer_traits.hpp
    ${PROJECT_SOURCE_DIR}/include/marcorubini/invocable_traits/member_object_pointer_traits.hpp
    ${PROJECT_SOURCE_DIR}/include/marcorubini/invocable_traits/invocable_traits.hpp
)

# main target
add_library(marcorubini_invocable_traits INTERFACE)
add_library(marcorubini::marcorubini_invocable_traits ALIAS
            marcorubini_invocable_traits)

target_sources(marcorubini_invocable_traits
               INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_compile_features(marcorubini_invocable_traits INTERFACE cxx_std_20)
target_include_directories(
  marcorubini_invocable_traits SYSTEM
  INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>"
            "$<INSTALL_INTERFACE:include/>")

if(NOT dependency_via_submodule)
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    marcorubini_invocable_traits-config-version.cmake
    COMPATIBILITY ExactVersion)
  install(
    TARGETS marcorubini_invocable_traits
    EXPORT marcorubini_invocable_traits_targets
    INCLUDES
    DESTINATION include)
  install(
    EXPORT marcorubini_invocable_traits_targets
    DESTINATION lib/cmake/marcorubini_invocable_traits
    FILE marcorubini_invocable_traits-targets.cmake
    NAMESPACE marcorubini::)
  install(DIRECTORY include/ DESTINATION include)
  install(
    FILES
      marcorubini_invocable_traits-config.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/marcorubini_invocable_traits-config-version.cmake
    DESTINATION lib/cmake/marcorubini_invocable_traits)
else()
  message(STATUS "Dependency installed via submodule, installation unavailable")
endif()

# testing
option(INVOCABLE_TRAITS_BUILD_TEST
       "build tests of marcorubini/invocable_traits" OFF)
if(${INVOCABLE_TRAITS_BUILD_TEST} OR (CMAKE_CURRENT_SOURCE_DIR STREQUAL
                                      CMAKE_SOURCE_DIR))
  set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
  message("Building of invocable_traits constexpr tests enabled.")
  enable_testing()
  add_subdirectory(test)
endif()

