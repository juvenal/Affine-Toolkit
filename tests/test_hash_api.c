/*
 * test_hash_api.c -- unit tests for ribhash public API
 *
 * Verifies const-correct signatures, hash correctness, and error paths.
 * Exit code: 0 = all pass, 1 = one or more failures.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ribhash.h>

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

/* Hash table lifecycle */

static void test_create_destroy(void)
{
    RIB_HASHHND h = RibCreateHashTable(kRIB_HASH_TABLESIZE);
    CHECK(h != NULL, "RibCreateHashTable returns non-NULL");
    CHECK(RibDestroyHashTable(h) == kRIB_OK, "RibDestroyHashTable returns OK");
}

/* RibHashValueFromStringN -- const char * parameter */

static void test_hash_value(void)
{
    RIB_HASHHND h = RibCreateHashTable(kRIB_HASH_TABLESIZE);

    /* Hashing a string normally vs. via prefix should match */
    int full   = RibHashValueFromStringN(h, "abcde", 5);
    int prefix = RibHashValueFromStringN(h, "abcdefgh", 5);
    CHECK(full == prefix, "RibHashValueFromStringN prefix match");

    /* Const literal compiles and returns a non-negative value */
    const char *lit = "constant";
    int vlit = RibHashValueFromStringN(h, lit, (int)strlen(lit));
    CHECK(vlit >= 0, "RibHashValueFromStringN with const literal");

    RibDestroyHashTable(h);
}

/* RibAddItem / RibFindMatch round-trip */

static void test_add_find(void)
{
    RIB_HASHHND h = RibCreateHashTable(kRIB_HASH_TABLESIZE);

    /* RibAddItem returns PRIB_HASHATOM on success */
    PRIB_HASHATOM added = RibAddItem(h, kRIB_HASH_STRING, 0, (void*)"hello");
    CHECK(added != NULL, "RibAddItem kRIB_HASH_STRING returns non-NULL atom");

    /* Find it back */
    PRIB_HASHATOM atom = RibFindMatch(h, kRIB_HASH_STRING, 0, "hello");
    CHECK(atom != NULL, "RibFindMatch finds inserted atom");

    /* Non-existent key */
    PRIB_HASHATOM miss = RibFindMatch(h, kRIB_HASH_STRING, 0, "missing_xyz");
    CHECK(miss == NULL, "RibFindMatch returns NULL for missing key");

    RibDestroyHashTable(h);
}

/* RibQueryClassType -- const char * parameter */

static void test_query_class_type(void)
{
    RIB_HASHHND h = RibCreateHashTable(kRIB_HASH_TABLESIZE);

    /* Querying an unknown variable on an empty hash returns UNKNOWN */
    const char *var = "UnknownVar_xyz";
    RIB_UINT32 result = RibQueryClassType(h, var, NULL);
    CHECK(result == 0u, "RibQueryClassType unknown var returns 0 (UNKNOWN)");

    RibDestroyHashTable(h);
}

/* RibSetDefinedString / RibGetDefinedString round-trip */

static void test_defined_string(void)
{
    RIB_HASHHND h = RibCreateHashTable(kRIB_HASH_TABLESIZE);

    PRIB_HASHATOM a = RibAddItem(h, kRIB_HASH_STRINGTOKEN, 42, NULL);
    CHECK(a != NULL, "RibAddItem STRINGTOKEN returns non-NULL atom");

    if (a) {
        int rc = RibSetDefinedString(a, 42, (char*)"test_value");
        CHECK(rc == kRIB_OK, "RibSetDefinedString returns OK");
        char *got = RibGetDefinedString(a);
        CHECK(got != NULL && strcmp(got, "test_value") == 0,
              "RibGetDefinedString returns stored value");
    }

    RibDestroyHashTable(h);
}

/* Error code sentinel values */

static void test_error_sentinels(void)
{
    CHECK(kRIB_OK == 0,          "kRIB_OK == 0");
    CHECK(kRIB_ERRRC_PTR == NULL, "kRIB_ERRRC_PTR == NULL");
    CHECK(kRIB_ERRRC_INDEX < 0,  "kRIB_ERRRC_INDEX < 0");
}

int main(void)
{
    printf("=== test_hash_api ===\n");
    test_create_destroy();
    test_hash_value();
    test_add_find();
    test_query_class_type();
    test_defined_string();
    test_error_sentinels();
    printf("=== %d failure(s) ===\n", g_failures);
    return g_failures ? 1 : 0;
}
