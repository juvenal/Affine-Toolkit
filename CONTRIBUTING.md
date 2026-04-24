# Contributing to Affine Toolkit

## Getting started

```sh
git clone https://github.com/juvenal/Affine-Toolkit.git
cd Affine-Toolkit
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Code standards

### Language

All source is **C17** (`-std=c17`). Extensions (`-fno-extensions`) are disabled.
POSIX baseline is `_POSIX_C_SOURCE=200809L`.

### Types

Use `<stdint.h>` types (`uint32_t`, `int32_t`, …) for sized fields.  
Use the project typedefs `RIB_UINT32`, `RIB_INT32`, `RtInt` etc. in public API.

### String safety

- Never use `strcpy` or unbounded `sprintf`. Use `snprintf` or `memcpy` instead.
- For accumulating into a fixed buffer, follow the `SCAT` macro pattern in
  `ribdump/ribdump.c`.
- Malloc + copy: allocate `strlen(src) + 1`, then `memcpy(dst, src, len)`.

### Const correctness

String input parameters that are not modified must be declared `const char *`.
See `include/ribrdr.h` and `include/ribhash.h` for examples.

### No new warnings

The CI build uses `-Wall -Wextra -Wformat=2 -Wformat-security -Wstrict-overflow=2
-Wcast-align -Wpointer-arith -Wmissing-prototypes -Wshadow`. All warnings are
treated as errors; patches that introduce new warnings will not be merged.

### Formatting

Run `clang-format` before committing:

```sh
find ribhash sribw readrib ributil ribdump tokentbl pcutil examples tests \
     -name '*.c' -o -name '*.h' | xargs clang-format -i
```

The style is defined in `.clang-format` (LLVM-based, 3-space indent).

## Running tests

```sh
ctest --test-dir build --output-on-failure
```

To run with AddressSanitizer:

```sh
cmake -S . -B build/asan -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build/asan --parallel
ctest --test-dir build/asan --output-on-failure
```

## Static analysis

```sh
run-clang-tidy -p build $(find ribhash sribw readrib ributil -name '*.c')
```

## Adding a new test

Place new test source files in `tests/` and register them in `tests/CMakeLists.txt`:

```cmake
add_executable(test_myfeature test_myfeature.c)
target_link_libraries(test_myfeature PRIVATE ribhash)   # or whichever lib
add_test(NAME test_myfeature COMMAND test_myfeature)
```

Tests must exit with code `0` for pass and non-zero for failure.  
Print `FAIL <description>` to stderr for each individual check that fails.

## Pull requests

1. Fork the repository and create a feature branch.
2. Make changes following the standards above.
3. Ensure `cmake --build build --parallel` succeeds with zero warnings.
4. Ensure `ctest --test-dir build --output-on-failure` passes.
5. Open a pull request with a clear description of what changed and why.
