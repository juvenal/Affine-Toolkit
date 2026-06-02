#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <string.h>
#include <stdio.h>

extern "C" {
#define _RIPRIV_TYPES
#include <ripriv.h>
#include <ribrdr.h>
}

/* Table of tokens. */
char *tokens[] = {
    (char *)"abc",
    (char *)"abcd",
    (char *)"bdefghij",
    (char *)"defghijk",
    (char *)"lmnop",
    (char *)"qrstuv",
    (char *)"  abc"};

char *validtokens[] = {
    (char *)"abcd ",
    (char *)"qrstuv "};

int validindices[] = {1, 5};

char *nontokens[] = {
    (char *)" abcd",
    (char *)"aBc",
    (char *)"bbdefghij",
    (char *)"defghi",
    (char *)"defghij",
    (char *)"lmnop12",
    (char *)"qrst_uv"};

char table[] = {
    12,
    3,
    'a',
    'b',
    'c',
    2,
    2,
    '\0',
    0,
    0,
    3,
    'd',
    '\0',
    1,
    10,
    10,
    'b',
    'd',
    'e',
    'f',
    'g',
    'h',
    'i',
    'j',
    '\0',
    2,
    10,
    10,
    'd',
    'e',
    'f',
    'g',
    'h',
    'i',
    'j',
    'k',
    '\0',
    3,
    7,
    7,
    'l',
    'm',
    'n',
    'o',
    'p',
    '\0',
    4,
    8,
    8,
    'q',
    'r',
    's',
    't',
    'u',
    'v',
    '\0',
    5,
    0,
    7,
    ' ',
    ' ',
    'a',
    'b',
    'c',
    '\0',
    6,
};

char vartable[] = {
    15,
    1,
    'P',
    2,
    2,
    '\0',
    0,
    3,
    3,
    'w',
    '\0',
    1,
    0,
    3,
    'z',
    '\0',
    2,
    10,
    1,
    'N',
    2,
    2,
    '\0',
    3,
    0,
    3,
    'p',
    '\0',
    4,
    3,
    3,
    's',
    '\0',
    5,
    0,
    3,
    't',
    '\0',
    6,
};
RtToken vartablelist[] = {(char *)"P", (char *)"Pw", (char *)"Pz", (char *)"N", (char *)"Np", (char *)"s", (char *)"t"};
RtPointer varoutput[sizeof(vartablelist) / sizeof(RtToken)];
RtToken vartokens[] = {(char *)"P", (char *)"st", (char *)"N", (char *)"Np", (char *)"s"};
RtPointer varparms[] = {
    (char *)"P's data", (char *)"st's data", (char *)"N's data", (char *)"Np's data", (char *)"s's data"};
RtPointer expectedoutput[sizeof(vartablelist) / sizeof(RtToken)];

TEST_CASE("String and Parameter tests", "[readrib]") {

    SECTION("RibReadStringForToken") {
        for (int i = 0; i < (int)(sizeof(tokens) / sizeof(char *)); i++) {
            int index = RibReadStringForToken(tokens[i], table, NULL);
            REQUIRE(index == i);
        }

        for (int i = 0; i < (int)(sizeof(validtokens) / sizeof(char *)); i++) {
            int index = RibReadStringForToken(validtokens[i], table, NULL);
            REQUIRE(index == validindices[i]);
        }

        for (int i = 0; i < (int)(sizeof(nontokens) / sizeof(char *)); i++) {
            int index = RibReadStringForToken(nontokens[i], table, NULL);
            REQUIRE(index < 0);
        }
    }

    SECTION("RibReadStringForToken with pointer update") {
        char *p, *pp;
        p = (char *)"abc";
        int index = RibReadStringForToken(p, table, &pp);
        REQUIRE(index == 0);
        REQUIRE(pp == p + strlen(p));

        p = (char *)"abc ";
        index = RibReadStringForToken(p, table, &pp);
        REQUIRE(index == 0);
        REQUIRE(pp == p + strlen("abc"));

        p = (char *)"abc\n";
        index = RibReadStringForToken(p, table, &pp);
        REQUIRE(index == 0);
        REQUIRE(pp == p + strlen("abc"));
    }

    SECTION("RibGetUserParameters") {
        expectedoutput[0] = varparms[0]; /*P */
        expectedoutput[1] = NULL;        /*Pw*/
        expectedoutput[2] = NULL;        /*Pz*/
        expectedoutput[3] = varparms[2]; /*N */
        expectedoutput[4] = varparms[3]; /*Np*/
        expectedoutput[5] = varparms[4]; /*s */
        expectedoutput[6] = NULL;        /*t */

        REQUIRE(RibGetUserParameters(vartable, sizeof(vartablelist) / sizeof(RtToken),
                                     sizeof(varparms) / sizeof(RtPointer),
                                     vartokens, varparms, varoutput) == 0);

        for (int i = 0; i < (int)(sizeof(vartablelist) / sizeof(RtPointer)); i++) {
            REQUIRE(varoutput[i] == expectedoutput[i]);
        }
    }
}
