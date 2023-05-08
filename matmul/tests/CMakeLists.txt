cmake_minimum_required(VERSION 3.2)
project(tests)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG release-1.12.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
  # URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  baseline_test
  baseline_test.cc
)
target_include_directories(baseline_test PRIVATE MATMUL_HEADERS_DIR)
target_link_libraries(
  baseline_test
  PRIVATE
  GTest::gtest_main
  utils
)

include(GoogleTest)
gtest_discover_tests(baseline_test)
