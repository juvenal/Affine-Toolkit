cmake_minimum_required(VERSION 3.10)
project(AffineToolkit VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prefer static linking for external dependencies
if(APPLE)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
else()
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib ${CMAKE_FIND_LIBRARY_SUFFIXES})
endif()

# External Dependencies
find_package(ZLIB REQUIRED)
find_package(TIFF REQUIRED)
find_package(JPEG REQUIRED)
find_library(LZMA_LIBRARY NAMES lzma liblzma REQUIRED)
find_library(ZSTD_LIBRARY NAMES zstd libzstd REQUIRED)

# Version and Copyright Info
set(AFFINE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(AFFINE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(AFFINE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(AFFINE_COPYRIGHT "Copyright (c) 1995-2026 Thomas E. Burge. All rights reserved.")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(src/3dtorib)
add_subdirectory(ribdump)
add_subdirectory(ribhash)
add_subdirectory(sribw)
add_subdirectory(readrib)
add_subdirectory(ributil)
add_subdirectory(tokentbl)
add_subdirectory(pcutil)
add_subdirectory(examples)
add_subdirectory(pixNtiff)
