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

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt)

# executable and linking
add_executable(${PROJECT_NAME}_test test/runner_test.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME} fmt)

set_debug_options(${PROJECT_NAME}_test)
enable_sanitizers(${PROJECT_NAME}_test)

# Install public headers so that the INSTALL_INTERFACE path exists.
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
)

# Export the target
install(
    TARGETS ${PROJECT_NAME}
    EXPORT single-task-runnerTargets
    DESTINATION lib
    INCLUDES DESTINATION include
)

# Install the export file
install(
    EXPORT single-task-runnerTargets
    FILE single-task-runnerTargets.cmake
    NAMESPACE single-task-runner::
    DESTINATION lib/cmake/single-task-runner
)

# Configure package files
include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/single-task-runnerConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/single-task-runnerConfig.cmake"
  INSTALL_DESTINATION lib/cmake/single-task-runner
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Install the package configuration files
install(
  FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/single-task-runnerConfig.cmake"
  DESTINATION lib/cmake/single-task-runner
)
