# Affine Toolkit

[![CI](https://github.com/juvenal/Affine-Toolkit/actions/workflows/ci.yml/badge.svg)](https://github.com/juvenal/Affine-Toolkit/actions/workflows/ci.yml)

RenderMan RIB reader/writer libraries and conversion utilities.  
Originally written by Thomas E. Burge; modernized to C17 with a CMake build system.

---

## Overview

The Affine Toolkit provides:

- **`libribrdr`** — full RIB parser (binary, ASCII, gzip-compressed)
- **`libribhash`** — hash table for RIB tokens, declarations, and light handles
- **`libsribw`** — RIB writer (ASCII/binary output)
- **`libributil`** — aggregate utility library (ribrdr + ribhash + sribw + extras)
- **`libribattrstore`** — attribute block storage for RIB readers
- **`libribtrimstore`** — trim curve storage
- **`libribnop`** — no-op RenderMan interface stubs

Conversion and inspection tools:

| Category | Tools |
|---|---|
| PIX ↔ TIFF | `pix2tiff` `tiff2pix` `tif2tif` |
| TGA ↔ TIFF | `tga2tiff` `tiff2tga` `tga2tga` |
| IFF ↔ TIFF | `iff2tiff` `tiff2iff` `iff2iff` |
| Float TIFF | `tiff2fp` `fp2tiff` `normfptiff` `iff2fp` |
| Image info | `pixinfo` `tgainfo` `iffinfo` `cininfo` |
| Image processing | `tiffedge` `tifftile` `tiffinvert` `tifforient` `busybits` |
| RIB utilities | `ribdump` `typerib` `ribbish` `ribfixer` `ribobjmod` |
| Binary encoding | `oct2bin` `bin2oct` `hex2bin` `bin2hex` `dbl2oct` `flt2oct` `dec2hex` `hex2dec` `str2oct` |
| Text utilities | `mac2unix` `ibm2unix` `untab` `tokentbl` |

---

## Requirements

| Dependency | Required | Notes |
|---|---|---|
| C17 compiler | Yes | GCC ≥ 9 or Clang ≥ 10 |
| CMake | Yes | ≥ 3.16 |
| zlib | Yes | `libz-dev` / `zlib1g-dev` |
| libtiff | No | Only for `pixNtiff` tools |
| X11 | No | Only for `tiff2X` and `showbits` |

---

## Building

```sh
# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --parallel

# Install (default prefix: /usr/local)
cmake --install build
```

### Build options

| Option | Default | Description |
|---|---|---|
| `ENABLE_SANITIZERS` | `OFF` | AddressSanitizer + UBSan |
| `ENABLE_COVERAGE` | `OFF` | gcov/lcov instrumentation |
| `ENABLE_FUZZING` | `OFF` | libFuzzer targets (Clang only) |

Example — build with sanitizers:

```sh
cmake -S . -B build/asan -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build/asan --parallel
```

---

## Testing

```sh
# Build (includes test executables)
cmake -S . -B build
cmake --build build --parallel

# Run all tests
ctest --test-dir build --output-on-failure

# Run with verbose output
ctest --test-dir build -V
```

### Code coverage

```sh
cmake -S . -B build/cov -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build/cov --parallel
ctest --test-dir build/cov
lcov --capture --directory build/cov --output-file coverage.info --no-external
genhtml coverage.info --output-directory coverage/html
```

---

## API Quick Start

```c
#include <ribrdr.h>

/* Open a RIB file and read all tokens */
RIB_HANDLE rib = RibOpen("scene.rib", kRIB_LAST_RI, gRibRITable);
if (rib == kRIB_ERRRC_PTR) {
    /* error */
}
while (RibRead(rib) != EOF)
    ;
RibClose(rib);
```

See `examples/closequad/` and `examples/ribtree/` for complete programs,  
and `examples/hello_rib/` for a minimal read-and-print example.

Full API documentation can be generated with Doxygen:

```sh
doxygen Doxyfile          # output → docs/html/index.html
```

---

## Static Analysis

The repository ships a `.clang-tidy` configuration. With a compile-commands
database from CMake:

```sh
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
run-clang-tidy -p build $(find ribhash sribw readrib ributil -name '*.c')
```

Code style is defined in `.clang-format` (LLVM-based, 3-space indent).

---

## Notices

    The RenderMan (R) Interface Procedures and Protocol are:
          Copyright 1988, 1989, Pixar — All Rights Reserved
          RenderMan (R) is a registered trademark of Pixar

    The libtiff library is:
          Copyright (c) 1988-1996 Sam Leffler
          Copyright (c) 1991-1996 Silicon Graphics, Inc.

    The zlib library is:
          Copyright (c) 1995-1996 Jean-loup Gailly and Mark Adler

    The Affine (R) Libraries and Tools are:
          Copyright (c) 1995-2000 Thomas E. Burge — All rights reserved
          Affine (R) is a registered trademark of Thomas E. Burge
