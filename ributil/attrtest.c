/* 
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
 * FILE:  attrtest.c
 *
 * DESCRIPTION:  Private tests for the RIB Attribute Storage Library.
 *   
 *    Uses:      
 *
 *    Contains:
 *       int main( void );
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define RIB_INCLUDE_RITABLE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ripriv.h>
#include <ributil.h>
#include <ribattrstore.h>


int main(int argc, char **argv);


RtToken tokens1[] = { "tokens1" };
RtToken tokens2[] = { "tokens2a", "tokens2b" };
RtToken parms1[] = { "parms1" };
RtToken parms2[] = { "parms2a", "parms2b" };



int main(int argc, char **argv) 
{
   PATTR      a;
   PATTRITEM  item;

   printf( "# Gravity Tests For attrstore.c\n" );
   printf( "# -----------------------------\n" );

   a = AttrInitRITable( NULL, 0 );

   if ( AttrSetFlags( a, ATTR_DONTFREEDATA, 0 ) )
   {
      printf("ERROR:  AttrSetFlags(a,...) returned error. (%d)\n", __LINE__ );
   }

   if (AttrBeginBlock(a))
   {
      printf("ERROR:  AttrBeginBlock(a) returned error. (%d)\n", __LINE__ );
   }

   AttrUse( a );

   AttrAttributeV( "name1", 1, tokens1, parms1 );
   item = AttrGetFirstAttribute(a);
   if (!item)
   {
      printf( "ERROR:  NULL pointer for item.\n" );
      return 1;
   }
   if (!item->name)
   {
      printf( "ERROR:  NULL pointer for item->name.\n" );
      return 1;
   }
   if (strcmp(item->name,"name1"))
   {
      printf( "ERROR:  item->name!=\"name1\".  Got \"%s\".\n", item->name );
   }
   if (AttrGetFirstAttribute(a)!=AttrGetLastAttribute(a))
   {
      printf( "ERROR:  "\
	     "AttrGetFirstAttribute(a)!=AttrGetLastAttribute(a)\n" );
   }

   AttrAttributeV( "name2", 2, tokens2, parms2 );

   if ( strcmp((char*)AttrQueryAttribute(a, "name1","tokens1"),
		"parms1" ) )
   {
      printf( "ERROR:  Attribute \"name1\",\"tokens1\" missing.\n" );
   }

   if ( strcmp((char*)AttrQueryAttribute(a, "name2","tokens2a"),
		"parms2a" ) )
   {
      printf( "ERROR:  Attribute \"name2\",\"tokens2a\" missing.\n" );
   }
   if ( strcmp((char*)AttrQueryAttribute(a, "name2","tokens2b"),
		"parms2b" ) )
   {
      printf( "ERROR:  Attribute \"name2\",\"tokens2b\" missing.\n" );
   }
   if (AttrEndBlock(a))
   {
      printf("ERROR:  AttrEndBlock(a) returned error. (%d)\n", __LINE__ );
   }

   if (AttrDestroy(a) )
   {
      printf("ERROR:  AttrDestroy(a) returned error. (%d)\n", __LINE__ );
   }

#ifdef RIB_MEMORY_TESTS
   if (RibMemoryTestResults( gPMemTest ))
     printf("No memory test results\n" );
#endif

   return 0;
}
