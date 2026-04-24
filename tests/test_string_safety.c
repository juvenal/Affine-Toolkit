/*
 * test_string_safety.c — tests for bounded string operations introduced
 * during the Phase 1 security hardening pass.
 *
 * These are pure-C tests (no library dependencies) that verify the boundary
 * conditions of snprintf-based replacements for the removed strcpy/sprintf
 * calls.  Exit code: 0 = all pass, 1 = one or more failures.
 */
#include <stdio.h>
#include <string.h>
#include <stddef.h>

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

/* Reproduce the SCAT-style pattern from ribdump.c */
#define BUF_SIZE 32

static void scat_append(char *buf, size_t bufsz, char **pp, const char *fmt, const char *arg)
{
    int n = snprintf(*pp, (size_t)(buf + bufsz - *pp), fmt, arg);
    if (n > 0 && *pp + n < buf + (ptrdiff_t)bufsz)
        *pp += n;
}

static void test_scat_normal(void)
{
    char buf[BUF_SIZE] = {0};
    char *p = buf;

    scat_append(buf, BUF_SIZE, &p, "%s", "hello");
    scat_append(buf, BUF_SIZE, &p, " %s", "world");

    CHECK(strcmp(buf, "hello world") == 0, "SCAT normal concatenation");
    CHECK(p == buf + 11, "SCAT pointer advances correctly");
}

static void test_scat_truncation(void)
{
    char buf[8] = {0};
    char *p = buf;

    /* Write a string that exceeds the buffer */
    scat_append(buf, sizeof(buf), &p, "%s", "1234567890");

    /* snprintf must NUL-terminate within buf[7] */
    CHECK(buf[sizeof(buf) - 1] == '\0', "SCAT truncation: buffer not overrun");
    /* Pointer must not exceed the buffer end */
    CHECK(p <= buf + sizeof(buf), "SCAT truncation: pointer within buffer");
}

static void test_scat_exact_fill(void)
{
    char buf[6] = {0};
    char *p = buf;

    scat_append(buf, sizeof(buf), &p, "%s", "hello");   /* 5 chars + NUL = 6 */

    CHECK(strcmp(buf, "hello") == 0, "SCAT exact-fill content correct");
    /* A second write must not write beyond the buffer */
    scat_append(buf, sizeof(buf), &p, "%s", "X");
    CHECK(buf[5] == '\0', "SCAT exact-fill second write: NUL preserved");
}

/* Verify snprintf(dst, n, "%s", src) is safe when src equals dst size - 1 */
static void test_snprintf_boundary(void)
{
    char dst[16];
    const char *src = "abcdefghijklmno";   /* 15 chars, fits in dst[16] */

    int n = snprintf(dst, sizeof(dst), "%s", src);
    CHECK(n == 15, "snprintf returns correct length");
    CHECK(dst[15] == '\0', "snprintf NUL-terminates at boundary");
    CHECK(strcmp(dst, src) == 0, "snprintf content correct at boundary");
}

static void test_snprintf_overlong(void)
{
    char dst[4];
    int n = snprintf(dst, sizeof(dst), "%s", "toolong");

    /* snprintf returns the length that would have been written */
    CHECK(n == 7, "snprintf returns full length when truncated");
    CHECK(dst[3] == '\0', "snprintf NUL-terminates when truncated");
    CHECK(strncmp(dst, "too", 3) == 0, "snprintf keeps first n-1 chars");
}

/* memcpy replacement for malloc+strcpy pattern */
static void test_memcpy_clone(void)
{
    const char *src = "RenderMan Interface";
    size_t l = strlen(src) + 1;
    char *copy = malloc(l);

    memcpy(copy, src, l);

    CHECK(strcmp(copy, src) == 0, "memcpy clone: content matches");
    CHECK(copy[l - 1] == '\0',    "memcpy clone: NUL-terminated");

    free(copy);
}

int main(void)
{
    printf("=== test_string_safety ===\n");
    test_scat_normal();
    test_scat_truncation();
    test_scat_exact_fill();
    test_snprintf_boundary();
    test_snprintf_overlong();
    test_memcpy_clone();
    printf("=== %d failure(s) ===\n", g_failures);
    return g_failures ? 1 : 0;
}
