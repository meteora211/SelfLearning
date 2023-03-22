cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0057 NEW)


project(calender)
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -O3")

# FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git GIT_TAG 9.0.0)
# FetchContent_MakeAvailable(fmt)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR})

set(SOURCE_FILES calender.cc)
add_executable(calender ${SOURCE_FILES})
# target_link_libraries(calender fmt)
target_link_libraries(calender)
install(TARGETS calender)
