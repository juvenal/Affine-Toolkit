/*
 * test_rib_reader.c -- integration tests for RibOpen / RibRead public API.
 *
 * Exercises the const-correct entry points added in Phase 3 and verifies
 * that the reader handles both valid and degenerate input without crashing.
 * Exit code: 0 = all pass, 1 = one or more failures.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ribrdr.h>

static int g_failures = 0;

#define CHECK(expr, msg) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "FAIL [%s:%d] %s\n", __FILE__, __LINE__, msg); \
            g_failures++; \
        } else { \
            printf("PASS  %s\n", msg); \
        } \
    } while(0)

/* Drain the RIB handle until EOF or a hard error; return final code */
static int drain(RIB_HANDLE h)
{
    int rc;
    /* kRIB_ERRRC_STREAMIO (-2) signals end-of-stream in some cases */
    while ((rc = RibRead(h)) != EOF && rc > kRIB_ERRRC_STREAMIO)
        ;
    return rc;
}

/* RibOpen with const filename literal */

static void test_open_nonexistent(void)
{
    const char *path = "/nonexistent/path/to/file.rib";
    RIB_HANDLE h = RibOpen(path, 0, NULL);
    CHECK(h == kRIB_ERRRC_PTR, "RibOpen non-existent file returns error");
}

static void test_open_drain_valid(void)
{
    const char *tmppath = "/tmp/affine_test_minimal.rib";
    FILE *f = fopen(tmppath, "w");
    if (!f) {
        fprintf(stderr, "SKIP  test_open_drain_valid: cannot create temp file\n");
        return;
    }
    fprintf(f, "##RenderMan RIB\n");
    fclose(f);

    /* const char * parameter -- verifies Phase 3 API signature */
    const char *cpath = tmppath;
    RIB_HANDLE h = RibOpen(cpath, 0, NULL);
    CHECK(h != kRIB_ERRRC_PTR, "RibOpen valid RIB returns non-NULL handle");

    if (h != kRIB_ERRRC_PTR) {
        int rc = drain(h);
        CHECK(rc == EOF || rc == kRIB_ERRRC_STREAMIO,
              "RibRead drains header-only RIB to EOF");
        RibClose(h);
    }

    remove(tmppath);
}

static void test_open_drain_empty(void)
{
    const char *tmppath = "/tmp/affine_test_empty.rib";
    FILE *f = fopen(tmppath, "w");
    if (!f) {
        fprintf(stderr, "SKIP  test_open_drain_empty: cannot create temp file\n");
        return;
    }
    fclose(f);

    RIB_HANDLE h = RibOpen(tmppath, 0, NULL);
    if (h != kRIB_ERRRC_PTR) {
        int rc = drain(h);
        CHECK(rc == EOF || rc == kRIB_ERRRC_STREAMIO,
              "RibRead drains empty file to EOF");
        RibClose(h);
    } else {
        printf("PASS  RibOpen empty file handled gracefully\n");
    }

    remove(tmppath);
}

/* RibGetRIFromString -- const char * parameter */

static void test_get_ri_from_string(void)
{
    const char *sphere = "Sphere";
    int code = RibGetRIFromString(sphere);
    CHECK(code >= 0, "RibGetRIFromString(\"Sphere\") >= 0");

    const char *unknown = "NotARICall_xyz";
    int bad = RibGetRIFromString(unknown);
    CHECK(bad < 0, "RibGetRIFromString unknown call < 0");
}

/* Library version */

static void test_version(void)
{
    char *ver = RibGetLibraryVersionName();
    CHECK(ver != NULL, "RibGetLibraryVersionName returns non-NULL");
    CHECK(strlen(ver) > 0, "RibGetLibraryVersionName returns non-empty string");
}

int main(void)
{
    printf("=== test_rib_reader ===\n");
    test_open_nonexistent();
    test_open_drain_valid();
    test_open_drain_empty();
    test_get_ri_from_string();
    test_version();
    printf("=== %d failure(s) ===\n", g_failures);
    return g_failures ? 1 : 0;
}
