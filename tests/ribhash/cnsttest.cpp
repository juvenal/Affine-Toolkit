#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <string.h>
#include <stdio.h>

extern "C" {
#define _RIPRIV_TYPES
#include <ripriv.h>
#include <ribhash.h>
}

TEST_CASE("Basis constants check", "[ribhash]") {
    SECTION("Static Basis Matrices") {
        REQUIRE(RiBezierBasis[0][0] == -1.0f);
        REQUIRE(RiBSplineBasis[0][0] == -1.0f / 6.0f);
    }

    SECTION("Standard Basis Table") {
        REQUIRE(sizeof(RiStdBasisTbl) / sizeof(PRtBasis) >= 5);
        for (int b = 0; b < (int)(sizeof(RiStdBasisTbl) / sizeof(PRtBasis)); b++) {
            REQUIRE(RiStdBasisTbl[b] != NULL);
        }
    }
}
