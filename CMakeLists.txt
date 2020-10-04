cmake_minimum_required(VERSION 3.18.3)
project(invocable_traits
  VERSION 0.0.1
  DESCRIPTION "C++20 function object destructuring and type traits."
  LANGUAGES CXX
  HOMEPAGE_URL "https://github.com/marcorubini/invocable_traits")

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

if(ENABLE_TESTING)
  enable_testing()
  message("Building constexpr tests.")
  set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
  add_subdirectory(test)
endif()

