cmake_minimum_required(VERSION 3.20)
project(single-task-runner)

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# default to release build for the library if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type" FORCE)
endif()
message(STATUS "Library will be built using CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

# Find dependencies
find_package(cmake-library REQUIRED)
find_package(fmt REQUIRED)

# Runner library
add_library(${PROJECT_NAME} STATIC src/runner.cpp)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set_release_options(${PROJECT_NAME})
else()
    set_debug_options(${PROJECT_NAME})
endif()
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt)

# executable and linking
add_executable(${PROJECT_NAME}_test test/runner_test.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME} fmt)

set_debug_options(${PROJECT_NAME}_test)
enable_sanitizers(${PROJECT_NAME}_test)

# installation
install(
    TARGETS ${PROJECT_NAME} DESTINATION lib
    INCLUDES DESTINATION include
)
install(TARGETS ${PROJECT_NAME}_test DESTINATION bin)