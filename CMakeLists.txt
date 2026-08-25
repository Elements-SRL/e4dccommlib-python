cmake_minimum_required(VERSION 3.21)

project(e4dccommlib_python_wrapper
    VERSION 0.1.0
    LANGUAGES CXX
)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Library

add_library(e4dccommlib_python_wrapper SHARED
    main.cpp
)

add_library(e4dccommlib_python_wrapper::e4dccommlib_python_wrapper ALIAS e4dccommlib_python_wrapper)

# C++ standard
# e4dccommlib itself requires C++20 (see its own CMakeLists.txt), so the
# wrapper is built with the same standard.

set_target_properties(e4dccommlib_python_wrapper PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    SUFFIX ".pyd"
    PREFIX ""     # so the output is e4dccommlib_python_wrapper.pyd, not libe4dccommlib_python_wrapper.pyd
)

# Headers

cmake_path(SET PYTHON_3_11_7_PATH_CMAKE "$ENV{PYTHON_3_11_7_PATH}")
cmake_path(SET PYBIND_11_PATH_CMAKE "$ENV{PYBIND_11_PATH}")

target_include_directories(e4dccommlib_python_wrapper
    PUBLIC
    $<BUILD_INTERFACE:${PYTHON_3_11_7_PATH_CMAKE}/include>
    $<BUILD_INTERFACE:${PYBIND_11_PATH_CMAKE}/include>
)

# Dependencies

cmake_path(SET E4DCCOMMLIB_PATH_CMAKE "$ENV{E4DCCOMMLIB_PATH}")
list(APPEND CMAKE_PREFIX_PATH "${E4DCCOMMLIB_PATH_CMAKE}")

find_package(e4dccommlib REQUIRED CONFIG)

target_link_libraries(e4dccommlib_python_wrapper
    PUBLIC
    e4dccommlib::e4dccommlib
)

# NOTE: e4dccommlib's own CMakeLists.txt declares E4DCCOMMLIB_STATIC as a
# PUBLIC compile definition on the e4dccommlib target itself. Linking
# e4dccommlib::e4dccommlib above therefore already propagates
# -DE4DCCOMMLIB_STATIC to this target transitively - do NOT redefine it here,
# and do NOT define E4DCCOMMLIB_LIBRARY (that switch is for building
# e4dccommlib itself, not for consuming it).

target_link_libraries(e4dccommlib_python_wrapper PUBLIC "${PYTHON_3_11_7_PATH_CMAKE}/libs/python311.lib")
