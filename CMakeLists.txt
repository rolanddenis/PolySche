cmake_minimum_required(VERSION 3.5)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

# Project version
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" POLYSCHE_VERSION)
string(STRIP "${POLYSCHE_VERSION}" POLYSCHE_VERSION)

# Project
project(polysche VERSION "${POLYSCHE_VERSION}" LANGUAGES CXX)

# Library
add_library(polysche INTERFACE)
set_target_properties(polysche
  PROPERTIES
    CMAKE_CXX_STANDARD 17
    CMAKE_CXX_STANDARD_REQUIRED TRUE
    CMAKE_CXX_EXTENSIONS FALSE
)

# Custom build options
add_compile_options(-Wall -Wextra -pedantic)
add_compile_options($<$<CONFIG:Release>:-march=native>)
add_compile_options($<$<CONFIG:Release>:-mtune=native>)

# Includes
include_directories("${PROJECT_SOURCE_DIR}/include")
target_include_directories(polysche INTERFACE
  $<BUILD_INTERFACE:"${CMAKE_CURRENT_SOURCE_DIR}/include">
  $<INSTALL_INTERFACE:"${CMAKE_INSTALL_INCLUDEDIR}/include">
)

# Tests
OPTION(BUILD_TESTING "Build tests." OFF)
if (BUILD_TESTING)
  enable_testing()
  add_subdirectory("${PROJECT_SOURCE_DIR}/tests")
endif (BUILD_TESTING)

# Examples
OPTION(BUILD_EXAMPLES "Build examples." OFF)
if (BUILD_EXAMPLES)
  add_subdirectory("${PROJECT_SOURCE_DIR}/examples")
endif (BUILD_EXAMPLES)

# Installation
install(TARGETS polysche
  PUBLIC_HEADER DESTINATION include
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include)
