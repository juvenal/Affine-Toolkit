#define CATCH_CONFIG_MAIN
#include "catch.hpp"

extern "C" {
#define _RIPRIV_TYPES
#include <ripriv.h>
#include <ributil.h>
#include <ribattrstore.h>
}

TEST_CASE("Attribute store tests", "[ributil]") {
    PATTR attr;
    attr = AttrInitRITable(NULL, 0);
    REQUIRE(attr != NULL);
    AttrDestroy(attr);
}
