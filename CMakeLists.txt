cmake_minimum_required(VERSION 3.8)
project(xeus-stata VERSION 0.1.0)

# C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Find dependencies
find_package(xeus 5.0 REQUIRED)
find_package(xeus-zmq 3.0 REQUIRED)
find_package(xtl 0.7 REQUIRED)
find_package(nlohmann_json 3.11 REQUIRED)
find_package(Threads REQUIRED)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find Stata (optional - for setting default path)
find_program(STATA_EXECUTABLE
    NAMES stata stata-mp stata-se StataMP-64.exe StataSE-64.exe StataIC-64.exe
    PATHS
        "/usr/local/stata18"
        "/usr/local/stata17"
        "/usr/local/stata"
        "/Applications/Stata"
        "C:/Program Files/Stata18"
        "C:/Program Files/Stata17"
    PATH_SUFFIXES bin
)

if(STATA_EXECUTABLE)
    message(STATUS "Found Stata: ${STATA_EXECUTABLE}")
    add_definitions(-DDEFAULT_STATA_PATH="${STATA_EXECUTABLE}")
else()
    message(WARNING "Stata not found. Kernel will require STATA_PATH environment variable.")
endif()

# Source files
set(XEUS_STATA_SRC
    src/main.cpp
    src/xinterpreter.cpp
    src/stata_session.cpp
    src/stata_parser.cpp
    src/completion.cpp
    src/inspection.cpp
    src/base64.cpp
)

set(XEUS_STATA_HEADERS
    include/xeus-stata/xeus_stata_config.hpp
    include/xeus-stata/xinterpreter.hpp
    include/xeus-stata/stata_session.hpp
    include/xeus-stata/stata_parser.hpp
    include/xeus-stata/completion.hpp
    include/xeus-stata/inspection.hpp
    include/xeus-stata/base64.hpp
)

# Executable
add_executable(xstata ${XEUS_STATA_SRC} ${XEUS_STATA_HEADERS})

# Platform-specific libraries for PTY support
if(UNIX AND NOT APPLE)
    set(PLATFORM_LIBS util)
elseif(APPLE)
    set(PLATFORM_LIBS util)
endif()

# Link libraries
target_link_libraries(xstata
    PRIVATE
        xeus
        xeus-zmq
        nlohmann_json::nlohmann_json
        Threads::Threads
        ${PLATFORM_LIBS}
)

# Include directories
target_include_directories(xstata
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(xstata PRIVATE -Wall -Wextra -pedantic)
elseif(MSVC)
    target_compile_options(xstata PRIVATE /W4)
endif()

# Installation
include(GNUInstallDirs)

install(TARGETS xstata
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install kernel spec
set(KERNELSPEC_DIR ${CMAKE_INSTALL_PREFIX}/share/jupyter/kernels/xeus-stata)
configure_file(
    share/jupyter/kernels/xeus-stata/kernel.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/kernel.json
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel.json
    DESTINATION ${KERNELSPEC_DIR}
)

# Install logos (if they exist)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xeus-stata/logo-32x32.png")
    install(FILES
        share/jupyter/kernels/xeus-stata/logo-32x32.png
        share/jupyter/kernels/xeus-stata/logo-64x64.png
        DESTINATION ${KERNELSPEC_DIR}
    )
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Documentation
if(BUILD_DOCS)
    add_subdirectory(docs)
endif()
