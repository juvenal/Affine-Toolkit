/* $RCSfile: hashtest.c,v $  $Revision: 1.3 $ $Date: 1999/10/12 02:27:00 $
 * 
 * Copyright (c) 1995, 1996, 1997, 1998 Thomas E. Burge.  All rights reserved.
 * 
 * Affine (R) is a registered trademark of Thomas E. Burge
 *
 * THIS SOFTWARE IS DISTRIBUTED "AS-IS" WITHOUT WARRANTY OF ANY KIND
 * AND WITHOUT ANY GUARANTEE OF MERCHANTABILITY OR FITNESS FOR A 
 * PARTICULAR PURPOSE.  
 *
 * In no event shall Thomas E. Burge be liable for any indirect or
 * consequential damages or loss of data resulting from use or performance 
 * of this software.
 * 
 * Permission is granted to include compiled versions of this code in
 * noncommercially sold software provided the following copyrights and
 * notices appear in all software and any related documentation:
 *
 *                 The Affine (R) Libraries and Tools are 
 *          Copyright (c) 1995, 1996, 1997, 1998 Thomas E. Burge.  
 *                          All rights reserved.
 *         Affine (R) is a registered trademark of Thomas E. Burge.
 *
 * Also refer to any additional requirements presently set by Pixar 
 * in regards to the RenderMan (R) Interface Procedures and Protocol.
 *
 * Those wishing to distribute this software commercially and those wishing 
 * to redistribute the source code must get written permission from the 
 * author, Thomas E. Burge.  
 *
 * Basically for now, I would like folks to get the source code directly 
 * from me rather than to have a bunch of different versions circulating
 * about.
 *
 *
 * Affine Toolkit
 *
 * FILE:  hashtest.c
 *
 * DESCRIPTION:  Hash library tests.  Really just a gravity check.
 *   
 *    Contains:
 * 
 *    History:
 *          10-11-99   Some C libs have printf()'s that can't take a NULL 
 *                     pointer for a string, so added extra code to handle
 *                     this.
 *          10-10-99   Added test case for searchpath archive option.
 *
 *    References:
 *
 */
#define _RIPRIV_TYPES
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ripriv.h>
#include <ribhash.h>
#include "hash.h"
#define  TESTCALLS

int main(int argc, char **argv)
{
   RIB_HASHHND hash;
   PRIB_HASHATOM p;
   int i,j,rc;
   PRIB_HASHTABLE  pt;
   unsigned int  u, u1;
   char  *s;
   char  str[] = "malloc() test string";


   hash = RibCreateHashTable( kRIB_HASH_TABLESIZE );

   if ( RibHashValueFromStringN( hash, "abcdefgh", 5 )!=
       RibHashValueFromString( hash, "abcde") )
   {
      printf("Error: RibHashValueFromStringN!=RibHashValueFromString\n");
      printf("RibHashValueFromStringN:%d  RibHashValueFromString:%d\n",
             RibHashValueFromStringN( hash, "abcdefgh", 5),
             RibHashValueFromString( hash, "abcde"));
   }

   pt = (PRIB_HASHTABLE)hash;

   printf( "modnumber %d, 2ndmod %d\n", 
          pt->modnumber,
          pt->mod2number );

   for ( i=0; i<hash->modnumber; i++ )
     for ( j=0; j<hash->mod2number; j++ )
       if ( hash->apAtoms[i][j] )
       {
          p = hash->apAtoms[i][j];
          while (p)
          {
             printf( "[%d][%d] p->code=%x \"%s\"\n", 
                    i, j, p->code, (char*)p->data );
             p=p->next;
          }
       }

#ifdef TESTCALLS
   printf("\n---------------------------------------------------------\n");

   /* Find "zonal". */
   p = RibFindMatch( hash, kRIB_HASH_VARIABLE, 
                    kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, "zonal" );
   if(!p)
     printf("Error:  zonal not found.\n");
   else
   {
      printf( "found p->data: %s\n", (char*)p->data );

      if ( (kRIB_CLASS_MASK & p->code) != kRIB_UNIFORMCLASS )
        printf( "Error:  class not correct %04x\n", 
               kRIB_CLASS_MASK & p->code );
      else
        printf( "Class is correct: UNIFORM\n" );

      if ( (kRIB_TYPE_MASK & p->code) != kRIB_STRINGTYPE )
        printf( "Error:  type not correct %04x\n", kRIB_TYPE_MASK & p->code );
      else
        printf( "Type is correct: STRING\n" );
   }
   /* Find "Kr". */
   p = RibFindMatch( hash, kRIB_HASH_VARIABLE, 
                    kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, "Kr" );
   if(!p)
     printf("Error:  Kr not found\n");
   else
     printf( "found p->data: %s\n", (char*)p->data );


   /* Should not find "junk". */
   p = RibFindMatch( hash, kRIB_HASH_VARIABLE, 
                    kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, "junk" );
   if(!p)
     printf("\"junk\" not found (which is good)\n");
   else
     printf( "ERROR: p->data: %s\n", (char*)p->data );


   /* Should find "archive". */
   p = RibFindMatch( hash, kRIB_HASH_VARIABLE, 
                    kRIB_UNIFORMCLASS | kRIB_STRINGTYPE, "archive" );
   if(p)
     printf("\"archive\" found (which is good)\n");
   else
     printf( "ERROR:  archive not found\n" );


   /* Add a RI Request definition of 123 for ripID == 2 */
   RibAddItem( hash, kRIB_HASH_RIREQUESTCODE, 123, 2 );
   /* Find the added RI Request. */
   p = RibFindMatch( hash, kRIB_HASH_RIREQUESTCODE, 123, NULL );
   if( !p && p->with.ripID!=2 )
     printf("Error:  RI Request not found\n");
   else
     printf( "found p->with.ripID: %d\n", (int)p->with.ripID );


   /* Add string definitions. */
   RibAddItem( hash, kRIB_HASH_STRINGTOKEN, 216, "Tom" );
   RibAddItem( hash, kRIB_HASH_STRINGTOKEN, 217, "Dick" );
   RibAddItem( hash, kRIB_HASH_STRINGTOKEN, 218, "Harry" );
   /* Find string "Tom". */
   p = RibFindMatch( hash, kRIB_HASH_STRINGTOKEN, 216, NULL ); 
   if( !p )
     printf("Error:  Tom not found.\n");
   else
     printf( "found Tom p->with.extdata: %s\n", (char*)p->with.extdata );


   /* Add encoded string definitions. */
   RibAddItem( hash, kRIB_HASH_STRING, 216, "Tom" );
   RibAddItem( hash, kRIB_HASH_STRING, 217, "Dick" );
   RibAddItem( hash, kRIB_HASH_STRING, 218, "Harry" );
   /* Find string "Tom". */
   p = RibFindMatch( hash, kRIB_HASH_STRING, 0, "Tom" ); 
   if( !p )
     printf("Error:  Tom not found.\n");
   else
   {
      printf( "found Tom p->with.data: %s\n", (char*)p->data );
      
      /* Check that token is correct. */
      if ( p->code != 216 )
      {
         printf("Error:  Tom's string ID  not correct.\n"\
                "        Got %d expected 216.\n", (int)p->code );
      }
      else
      {
         printf( "found Tom p->code: %d\n", (int)p->code );
      }
   }

   /* Add a Coordinate System name. */
   RibAddItem( hash, kRIB_HASH_COORDSYS, "Joey's Place", "nothing special" );
   /* Find the added coordinate space. */
   p = RibFindMatch( hash, kRIB_HASH_COORDSYS, 0, "Joey's Place" );
   if( !p )
     printf("Error:  Coordinate space not found.\n");
   else
   {
      printf( "found p->data: %s\n", (char*)p->data );
      printf( "      p->with.extdata: %s\n", (char*)p->with.extdata );
   }
        /* User's responsibility to free p->with.extdata if it points 
         *    to allocated memory.
         */


   /* Add a light handle of 123. */
   RibAddItem( hash, kRIB_HASH_LIGHTHANDLE, 123, "light data 123" );
   /* Find the added light handle. */
   p = RibFindMatch( hash, kRIB_HASH_LIGHTHANDLE, 123, NULL );
   if( !p )
   {
      printf("Light handle not found\n");
   }
   else
   {
      printf( "Light handle found p->with.extdata: %s\n", 
             (char*)p->with.extdata );
   }

   /* Add a light handle of 234. */
   RibAddItem( hash, kRIB_HASH_LIGHTHANDLE, 234, "light data 234" );
   /* Find the added light handle. */
   p = RibFindMatch( hash, kRIB_HASH_LIGHTHANDLE, 234, NULL );
   if( !p )
   {
      printf("Light handle not found\n");
   }
   else
   {
      printf( "Light handle found p->with.extdata: %s\n", 
             (char*)p->with.extdata );
   }

   /* Add a object handle of 123. */
   RibAddItem( hash, kRIB_HASH_OBJECTHANDLE, 123, "object data" );
   /* Find the added light handle. */
   p = RibFindMatch( hash, kRIB_HASH_OBJECTHANDLE, 123, NULL );
   if( !p )
   {
      printf("Object handle not found\n");
   }
   else
   {
      printf( "Object handle found p->with.extdata: %s\n", 
             (char*)p->with.extdata );
   }

   /* Add a user data. */
   RibAddItem( hash, kRIB_HASH_USER, 123, "user data", "user extended data" );
   /* Find the added coordinate space. */
   p = RibFindMatch( hash, kRIB_HASH_USER, 123, NULL );
   if( !p )
     printf("Error:  User data not found.\n");
   else
   {
      printf( "found p->data: %s\n", (char*)p->data );
      printf( "      p->with.extdata: %s\n", (char*)p->with.extdata );
   }

   /* Add a second piece of user data. */
   RibAddItem(hash, kRIB_HASH_USER, 123, "2user data", "2user extended data");
   /* Find the added coordinate space. */
   p = RibFindMatch( hash, kRIB_HASH_USER, 123, NULL );
   while ( p && strcmp((char*)p->data, "2user data")!=0 )
   {
      p = RibFindNextMatch( hash, p );
   }
   if( !p || strcmp((char*)p->data, "2user data")!=0 )
     printf("Error:  User data not found.\n");
   else
   {
      printf( "found p->data: %s\n", (char*)p->data );
      printf( "      p->with.extdata: %s\n", (char*)p->with.extdata );
   }

   /* Add more user data. */
   RibAddItem( hash, kRIB_HASH_USER, 122, 
	       NULL, /* give it a p->data of NULL. */
	       "user extended data" );
   /* Find the added coordinate space. */
   p = RibFindMatch( hash, kRIB_HASH_USER, 122, NULL );
   if( !p )
     printf("Error:  User data (122 hash value) not found.\n");
   else
   {
      s = (char*)p->data;
      if (!s)
      {
	 s = "(nil)"; 
      }
      else
      {
	 printf("Error:  p->data expected to be NULL for this item.\n");
      }
      printf( "found p->data: %s\n", s );
      printf( "      p->with.extdata: %s\n", (char*)p->with.extdata );
   }

   /* Add an item to then remove. */
   p = RibAddItem(hash, kRIB_HASH_USER, 101, "3user data", "3user extdata");
   /* Remove an item. */
   if (RibRemoveItem( hash, p ))
     printf("Error removing item\n" );
   else
     printf("Removed item\n");
   p = RibFindMatch( hash, kRIB_HASH_USER, 101, NULL );
   if (p)
   {
      printf( "Error:  Item still there.\n" );
   }
      
   /* Add a variable with an array declaration. */
   p = RibAddItem( hash, kRIB_HASH_VARIABLE, 
                   kRIB_VERTEXCLASS | kRIB_POINTTYPE | kRIB_ARRAY_DECLARED,
                   "array variable",
                   12 );
   if (!p)
     printf("Error: RibAddItem() didn't create an atom for \"array "\
            "variable\"\n" );
   else
     printf("Added: \"array variable\"\n");

   /* Find "array variable". */
   p = RibFindMatch( hash, kRIB_HASH_VARIABLE, 
                    kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, "array variable" );
   if(!p)
     printf("Error: \"array variable\" not found\n");
   else
   {
      printf( "found p->data: \"%s\"\n", (char*)p->data );
      if ( p->with.n != 12 )
           printf("Error: \"array variable\" not defined as 12 wide.\n" );
   }

   /* Add a variable with an array declaration and a malloced string. */
   s = (char*)malloc(sizeof(str));
   strcpy( s, str );
   p = RibAddItem( hash, kRIB_HASH_VARIABLE | kRIB_HASH_FREEDATA, 
                   kRIB_VERTEXCLASS | kRIB_POINTTYPE | kRIB_ARRAY_DECLARED,
                   s, 12 );
   if (!p)
     printf("Error: RibAddItem() didn't create an atom for \"%s\"\n",str);
   else
     printf("Added: \"%s\" n:%d\n",(char*)p->data,(int)p->with.n);

   /* Find "array variable". */
   p = RibFindMatch( hash, kRIB_HASH_VARIABLE, 
                    kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, str );
   if(!p)
     printf("Error: \"%s\" not found\n",str );
   else
   {
      printf( "found p->data: \"%s\"\n", (char*)p->data );
      if ( p->with.n != 12 )
           printf("Error: \"%s\" not defined as 12 wide.\n", str );
   }


   u1 = RibQueryClassType( hash, "array variable", NULL );
   if ( u1 != (kRIB_VERTEXCLASS | kRIB_POINTTYPE) )
   {
      printf("Error: RibQueryClassType( hash, \"array variable\", NULL ) " \
             "== %x\n" \
             "   Not defined as kRIB_VERTEXCLASS | kRIB_POINTTYPE == %x.\n",
             u1, kRIB_VERTEXCLASS | kRIB_POINTTYPE );
   }
   else
   {
      printf("RibQueryClassType(...,NULL) returned correct class and type.\n");
   }

   u1 = RibQueryClassType( hash, "array variable", &u );
   if ( u1 != (kRIB_VERTEXCLASS | kRIB_POINTTYPE) )
   {
      printf("Error: RibQueryClassType( hash, \"array variable\", NULL ) " \
             "== %x\n" \
             "   Not defined as kRIB_VERTEXCLASS | kRIB_POINTTYPE == %x.\n",
             u1, kRIB_VERTEXCLASS | kRIB_POINTTYPE );
   }
   else
   {
      printf( "RibQueryClassType(...,&u) returned correct class and type.\n" );
   }
   if ( u!=12 )
   {
      printf( "Error: RibQueryClassType \"array variable\" not " \
             "defined as 12 wide.\n" );
   }
   else
   {
      printf( "RibQueryClassType correctly returned 12 for \"array " \
             "variable\" width.\n" );
   }

   printf("\n---------------------------------------------------------\n");

   for ( i=0; i < hash->modnumber; i++ )
     for ( j=0; j < hash->mod2number; j++ )
     {
        if ( hash->apAtoms[i][j] )
        {
           p = hash->apAtoms[i][j];
           while (p)
           {
	      s = (char*)p->data;
	      if (!s)
		 s = "(nil)";

              printf( "[%d][%d] p->type=%d p->code=%x \"%s\"\n", 
                      i, j, (int)(((unsigned int)p->type)&kRIB_HASH_TYPEMASK), 
                      p->code, s );
              p=p->next;
           }
        }
     }
     
     

#endif
   
   if( (rc=RibDestroyHashTable(hash)) ) /* Extra () needed for gcc warning. */
     printf("RibDestroyHashTableKeepData(%p)=%d\n",hash,rc);

   return 0;
}


