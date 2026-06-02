#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <stdio.h>

extern "C" {
#define _RIPRIV_TYPES
#include <ripriv.h>
#include <ribrdr.h>
}

TEST_CASE("CParams gravity check", "[ribhash]") {
    REQUIRE(kRIB_LAST_RI > 0);

    for (int i = 0; i < kRIB_LAST_RI; i++) {
        char *p = (char *)gRibCParams[i];
        REQUIRE(p != NULL);
        // Just ensure we can iterate through them without crashing
        int c = 0;
        if (i != kRIB__VERSION) {
            while (p[c] != kRIB_CPARAMS_NULL && p[c] != kRIB_CPARAMS_VECTORFORMAT)
                c++;
        }
    }
}
