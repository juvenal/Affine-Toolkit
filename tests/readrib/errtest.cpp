#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#define _RIPRIV_TYPES
#include <ripriv.h>
#include <ribrdr.h>
#include <ribtables.h>

extern char gRibErrorHandlerTable[];
}

TEST_CASE("Error library tests", "[readrib]") {
    SECTION("gRibErrorHandlerTable lookup") {
        REQUIRE(RibReadStringForToken((char *)"ignore", gRibErrorHandlerTable, NULL) == kRIBTABLES_IGNORE);
        REQUIRE(RibReadStringForToken((char *)"print", gRibErrorHandlerTable, NULL) == kRIBTABLES_PRINT);
        REQUIRE(RibReadStringForToken((char *)"abort", gRibErrorHandlerTable, NULL) == kRIBTABLES_ABORT);
    }
}
