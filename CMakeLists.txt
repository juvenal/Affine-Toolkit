cmake_minimum_required(VERSION 3.16)
project(AffineToolkit
    VERSION 3.11.4
    DESCRIPTION "Affine RenderMan Toolkit - RIB reader/writer libraries and utilities"
    LANGUAGES C)

# Require C17
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Compiler flags ─────────────────────────────────────────────────────────────
add_compile_definitions(_POSIX_C_SOURCE=200809L)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    add_compile_options(
        -Wall -Wextra
        -Wformat=2 -Wformat-security
        -Wstrict-overflow=2
        -Wcast-align
        -Wpointer-arith
        -Wmissing-prototypes
        -Wshadow
        -Wno-cast-function-type
    )
endif()

# ── Optional sanitizers ────────────────────────────────────────────────────────
option(ENABLE_SANITIZERS "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# ── Optional code coverage ─────────────────────────────────────────────────────
option(ENABLE_COVERAGE "Build with gcov/lcov code coverage instrumentation" OFF)
if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage)
    endif()
endif()

# ── Optional fuzzing ───────────────────────────────────────────────────────────
option(ENABLE_FUZZING "Build libFuzzer fuzz targets" OFF)
if(ENABLE_FUZZING)
    if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang|AppleClang")
        message(FATAL_ERROR "ENABLE_FUZZING requires Clang (libFuzzer is Clang-only)")
    endif()
endif()

# ── Dependencies ───────────────────────────────────────────────────────────────
find_package(ZLIB REQUIRED)
find_package(TIFF)          # Optional — only needed for pixNtiff tools

# ── Include paths ──────────────────────────────────────────────────────────────
include_directories(${CMAKE_SOURCE_DIR}/include)

# ── Testing — must be enabled before add_subdirectory ─────────────────────────
enable_testing()

# ── Subdirectories ─────────────────────────────────────────────────────────────
add_subdirectory(ribhash)
add_subdirectory(sribw)
add_subdirectory(readrib)
add_subdirectory(ributil)
add_subdirectory(ribdump)
add_subdirectory(tokentbl)
add_subdirectory(pcutil)
if(TIFF_FOUND)
    add_subdirectory(pixNtiff)
endif()
add_subdirectory(examples)
add_subdirectory(tests)
if(ENABLE_FUZZING)
    add_subdirectory(fuzz)
endif()

# ── Installation layout ────────────────────────────────────────────────────────
include(GNUInstallDirs)

install(FILES
    include/ripriv.h
    include/ribrdr.h
    include/ribhash.h
    include/ributil.h
    include/ribnop.h
    include/ribattrstore.h
    include/ribtrimstore.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/affine)
