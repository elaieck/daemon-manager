cmake_minimum_required(VERSION 3.14)
project(daemon-manager)
include(GNUInstallDirs)
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

set (CMAKE_CXX_STANDARD 11)

add_subdirectory(src)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
