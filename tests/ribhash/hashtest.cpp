#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

extern "C" {
#define _RIPRIV_TYPES
#include <ripriv.h>
#include <ribhash.h>
#include "../../src/ribhash/hash.h"
}

TEST_CASE("Hash library tests", "[ribhash]") {
   RIB_HASHHND hash;
   PRIB_HASHATOM p;
   PRIB_HASHTABLE pt;
   unsigned int u, u1;
   char str[] = "malloc() test string";

   hash = RibCreateHashTable( kRIB_HASH_TABLESIZE );
   REQUIRE(hash != NULL);

   SECTION("RibHashValueFromStringN vs RibHashValueFromString") {
      REQUIRE( RibHashValueFromStringN( hash, (char*)"abcdefgh", 5 ) == RibHashValueFromString( hash, (char*)"abcde") );
   }

   pt = (PRIB_HASHTABLE)hash;

   SECTION("Find zonal") {
      p = RibFindMatch( hash, kRIB_HASH_VARIABLE, kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, (void*)"zonal" );
      REQUIRE(p != NULL);
      REQUIRE( (kRIB_CLASS_MASK & p->code) == kRIB_UNIFORMCLASS );
      REQUIRE( (kRIB_TYPE_MASK & p->code) == kRIB_STRINGTYPE );
   }

   SECTION("Find Kr") {
      p = RibFindMatch( hash, kRIB_HASH_VARIABLE, kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, (void*)"Kr" );
      REQUIRE(p != NULL);
   }

   SECTION("Should not find junk") {
      p = RibFindMatch( hash, kRIB_HASH_VARIABLE, kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, (void*)"junk" );
      REQUIRE(p == NULL);
   }

   SECTION("Should find archive") {
      p = RibFindMatch( hash, kRIB_HASH_VARIABLE, kRIB_UNIFORMCLASS | kRIB_STRINGTYPE, (void*)"archive" );
      REQUIRE(p != NULL);
   }

   SECTION("Add and find RI Request") {
      RibAddItem( hash, kRIB_HASH_RIREQUESTCODE, 123, (void*)(size_t)2 );
      p = RibFindMatch( hash, kRIB_HASH_RIREQUESTCODE, 123, NULL );
      REQUIRE(p != NULL);
      REQUIRE(p->with.ripID == 2);
   }

   SECTION("Add and find string definitions") {
      RibAddItem( hash, kRIB_HASH_STRINGTOKEN, 216, (void*)"Tom" );
      RibAddItem( hash, kRIB_HASH_STRINGTOKEN, 217, (void*)"Dick" );
      RibAddItem( hash, kRIB_HASH_STRINGTOKEN, 218, (void*)"Harry" );
      p = RibFindMatch( hash, kRIB_HASH_STRINGTOKEN, 216, NULL ); 
      REQUIRE(p != NULL);
      REQUIRE(strcmp((char*)p->with.extdata, "Tom") == 0);
   }

   SECTION("Add and find encoded string definitions") {
      RibAddItem( hash, kRIB_HASH_STRING, 216, (void*)"Tom" );
      p = RibFindMatch( hash, kRIB_HASH_STRING, 0, (void*)"Tom" ); 
      REQUIRE(p != NULL);
      REQUIRE(strcmp((char*)p->data, "Tom") == 0);
      REQUIRE(p->code == 216);
   }

   SECTION("Coordinate System") {
      RibAddItem( hash, kRIB_HASH_COORDSYS, (void*)"Joey's Place", (void*)"nothing special" );
      p = RibFindMatch( hash, kRIB_HASH_COORDSYS, 0, (void*)"Joey's Place" );
      REQUIRE(p != NULL);
      REQUIRE(strcmp((char*)p->data, "Joey's Place") == 0);
      REQUIRE(strcmp((char*)p->with.extdata, "nothing special") == 0);
   }

   SECTION("Light handle") {
      RibAddItem( hash, kRIB_HASH_LIGHTHANDLE, 123, (void*)"light data 123" );
      p = RibFindMatch( hash, kRIB_HASH_LIGHTHANDLE, 123, NULL );
      REQUIRE(p != NULL);
      REQUIRE(strcmp((char*)p->with.extdata, "light data 123") == 0);
   }

   SECTION("User data") {
      RibAddItem( hash, kRIB_HASH_USER, 123, (void*)"user data", (void*)"user extended data" );
      p = RibFindMatch( hash, kRIB_HASH_USER, 123, NULL );
      REQUIRE(p != NULL);
      REQUIRE(strcmp((char*)p->data, "user data") == 0);
      REQUIRE(strcmp((char*)p->with.extdata, "user extended data") == 0);

      RibAddItem(hash, kRIB_HASH_USER, 123, (void*)"2user data", (void*)"2user extended data");
      p = RibFindMatch( hash, kRIB_HASH_USER, 123, NULL );
      while ( p && strcmp((char*)p->data, "2user data")!=0 ) {
         p = RibFindNextMatch( hash, p );
      }
      REQUIRE(p != NULL);
      REQUIRE(strcmp((char*)p->data, "2user data") == 0);
   }

   SECTION("Remove item") {
      p = RibAddItem(hash, kRIB_HASH_USER, 101, (void*)"3user data", (void*)"3user extdata");
      REQUIRE(RibRemoveItem( hash, p ) == 0);
      p = RibFindMatch( hash, kRIB_HASH_USER, 101, NULL );
      REQUIRE(p == NULL);
   }

   SECTION("Array variable") {
      p = RibAddItem( hash, kRIB_HASH_VARIABLE, 
                      kRIB_VERTEXCLASS | kRIB_POINTTYPE | kRIB_ARRAY_DECLARED,
                      (void*)"array variable",
                      (void*)(size_t)12 );
      REQUIRE(p != NULL);
      p = RibFindMatch( hash, kRIB_HASH_VARIABLE, kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, (void*)"array variable" );
      REQUIRE(p != NULL);
      REQUIRE(p->with.n == 12);

      u1 = RibQueryClassType( hash, (char*)"array variable", &u );
      REQUIRE(u1 == (kRIB_VERTEXCLASS | kRIB_POINTTYPE));
      REQUIRE(u == 12);
   }

   REQUIRE(RibDestroyHashTable(hash) == 0);
}
