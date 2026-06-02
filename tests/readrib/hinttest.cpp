#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#define _RIPRIV_TYPES
#include <ripriv.h>
#include <ribrdr.h>

extern char gRibHintTokenTable[];
}

TEST_CASE("Hint library tests", "[readrib]") {
   SECTION("gRibHintTokenTable lookup") {
      // CameraOrientation is 0
      REQUIRE(RibReadStringForToken((char*)"CameraOrientation", gRibHintTokenTable, NULL) == 0);
      // Include is 6
      REQUIRE(RibReadStringForToken((char*)"Include", gRibHintTokenTable, NULL) == 6);
      // Scene is 8
      REQUIRE(RibReadStringForToken((char*)"Scene", gRibHintTokenTable, NULL) == 8);
   }
}
