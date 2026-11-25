cmake_minimum_required(VERSION 3.21)

project(e4dccommlib_python_wrapper
    VERSION 0.1.1
    LANGUAGES CXX
)

# Dependencies
#
# The Python interpreter decides the ABI the module is built for: pass
# -DPython_EXECUTABLE=<path to python.exe> to pick the version (3.10, 3.11, ...).
# pybind11 then names the output with the interpreter's EXT_SUFFIX, e.g.
#   e4dccommlib_python_wrapper.cp310-win_amd64.pyd
#   e4dccommlib_python_wrapper.cp311-win_amd64.pyd
# so modules for different Python versions can live side by side in one folder
# and each interpreter imports only the one built for it.

find_package(Python 3.10 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# Backwards compatible with the old env-var based local setup.
if(DEFINED ENV{E4DCCOMMLIB_PATH})
    cmake_path(CONVERT "$ENV{E4DCCOMMLIB_PATH}" TO_CMAKE_PATH_LIST _e4dccommlib_prefix NORMALIZE)
    list(APPEND CMAKE_PREFIX_PATH "${_e4dccommlib_prefix}")
endif()
find_package(e4dccommlib REQUIRED CONFIG)

# Module

pybind11_add_module(e4dccommlib_python_wrapper main.cpp)

# e4dccommlib itself requires C++20 (see its own CMakeLists.txt).
target_compile_features(e4dccommlib_python_wrapper PRIVATE cxx_std_20)
set_target_properties(e4dccommlib_python_wrapper PROPERTIES CXX_EXTENSIONS OFF)

# NOTE: e4dccommlib declares E4DCCOMMLIB_STATIC as a PUBLIC compile definition,
# so linking e4dccommlib::e4dccommlib propagates it transitively - do NOT
# redefine it here, and do NOT define E4DCCOMMLIB_LIBRARY (that switch is for
# building e4dccommlib itself, not for consuming it).
target_link_libraries(e4dccommlib_python_wrapper PRIVATE e4dccommlib::e4dccommlib)

# `cmake --install <build> --prefix <dir>` drops the .pyd straight into <dir>.
install(TARGETS e4dccommlib_python_wrapper
    LIBRARY DESTINATION .
    RUNTIME DESTINATION .
)

# okFrontPanel.dll is needed to import the module and Python does not search the
# PATH for it, so it ships next to the .pyd. The FTDI libraries are loaded at
# runtime by ftdi_utils and are not needed to import the module.
install(IMPORTED_RUNTIME_ARTIFACTS FrontPanel::FrontPanel DESTINATION .)
