#define CATCH_CONFIG_MAIN
#include "catch.hpp"

extern "C" {
#define _RIPRIV_TYPES
#include <ripriv.h>
#include <ributil.h>
#include <ribtrimstore.h>
}

TEST_CASE("Trim store tests", "[ributil]") {
   PTRIMSTORE trim;
   trim = TrimStoreInitRITable(NULL, 0);
   REQUIRE(trim != NULL);
   TrimStoreDestroy(trim);
}
