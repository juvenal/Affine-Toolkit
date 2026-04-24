/*
 * fuzz_rib_parser.c — libFuzzer entry point for the Affine RIB reader.
 *
 * Build with:  cmake -DENABLE_FUZZING=ON -DCMAKE_C_COMPILER=clang ...
 * Run with:    ./fuzz_rib_parser corpus/ -max_len=65536
 *
 * The fuzzer writes each test case to a temp file and passes it through the
 * full RibOpen → RibRead → RibClose path so every code path reachable from
 * a real RIB file is exercised.
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ribrdr.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Write fuzz bytes to a temp file */
    char tmppath[] = "/tmp/fuzz_rib_XXXXXX";
    int fd = mkstemp(tmppath);
    if (fd < 0)
        return 0;

    if (write(fd, data, size) != (ssize_t)size) {
        close(fd);
        unlink(tmppath);
        return 0;
    }
    close(fd);

    /* Open and read through the full parser pipeline, no RI table */
    RIB_HANDLE h = RibOpen(tmppath, 0, NULL);
    if (h != kRIB_ERRRC_PTR) {
        /* Drain all tokens — ignore return value, errors are expected */
        while (RibRead(h) != EOF)
            ;
        RibClose(h);
    }

    unlink(tmppath);
    return 0;
}
