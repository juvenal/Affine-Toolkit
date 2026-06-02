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

include(GNUInstallDirs)

enable_testing()

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/include")

macro(add_affine_library target_name)
    # Static version
    add_library(${target_name}_static STATIC ${ARGN})
    set_target_properties(${target_name}_static PROPERTIES OUTPUT_NAME ${target_name})
    
    # Shared/Dynamic version
    add_library(${target_name}_shared SHARED ${ARGN})
    set_target_properties(${target_name}_shared PROPERTIES 
        OUTPUT_NAME ${target_name}
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        MACHO_COMPATIBILITY_VERSION ${PROJECT_VERSION_MAJOR}.0.0
        MACHO_CURRENT_VERSION ${PROJECT_VERSION}
    )
    if(APPLE)
        set_target_properties(${target_name}_shared PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif()
    
    target_include_directories(${target_name}_static PUBLIC . "${CMAKE_SOURCE_DIR}/src/include")
    target_include_directories(${target_name}_shared PUBLIC . "${CMAKE_SOURCE_DIR}/src/include")
    
    # Install both
    install(TARGETS ${target_name}_static ${target_name}_shared
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    
    # Default alias for internal linking
    add_library(${target_name} ALIAS ${target_name}_shared)
endmacro()

macro(affine_target_link_libraries target_name)
    target_link_libraries(${target_name}_static ${ARGN})
    target_link_libraries(${target_name}_shared ${ARGN})
endmacro()


add_subdirectory(src/3dtorib)
add_subdirectory(src/ribdump)
add_subdirectory(src/ribhash)
add_subdirectory(src/sribw)
add_subdirectory(src/readrib)
add_subdirectory(src/ributil)
add_subdirectory(src/tokentbl)
add_subdirectory(src/pcutil)
add_subdirectory(src/pixNtiff)
add_subdirectory(docs/examples)
add_subdirectory(tests)

install(DIRECTORY src/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/affine)
install(DIRECTORY docs/ DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES README.md LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})
