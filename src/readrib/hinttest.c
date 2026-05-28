/* $RCSfile: hinttest.c,v $  $Revision: 1.3 $ $Date: 2000/02/27 07:34:00 $
 *
 * Copyright (c) 1995-1999 Thomas E. Burge.  All rights reserved.
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
 *                Copyright (c) 1995-1999 Thomas E. Burge.  
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
 * FILE:  hinttest.c
 *
 * DESCRIPTION:  Private tests for the RIB Reader Library's hint
 *               handling functions.
 *   
 *    Uses:      
 *       The files in affine/readrib/tests are needed for this program.
 *
 *    Contains:
 *       Tests for hint related functions.
 *
 *    History:
 *       10-17-99  RiSetHintTable takes a size now.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define  _RIPRIV_FUNC_TYPES 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ripriv.h>
#include <ribrdr.h>


char hintfile[]="./tests/hints.asc";

RtBoolean  RenderManRIBHintHit = RI_FALSE;


int RenderManRIBHintHandler( RIB_HANDLE hrib );
int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   RIB_HANDLE     rib;


   if ( argc > 1 && !strcmp(argv[1],"-") )
   {
      printf( "hinttest \n" );
      return 1;
   }

   printf( "# Gravity Tests For hintfunc.c\n" );
   printf( "# ----------------------------\n" );

#if 1
   rib = RibOpen( hintfile, kRIB_LAST_RI, gRibRITable );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",kRIB_LAST_RI, gRibRITable) failed.\n", 
	      hintfile );
      exit(1);
   }
   if (RibRead(rib))
      printf("ERROR:  RibRead(rib)\n" );
   if (RibClose(rib))
      printf("ERROR:  RibClose(rib)\n" );
#endif
#if 1
   printf( "# ----------------------------\n" );
   printf( "#    Adding hint table.\n#\n" );
   rib = RibOpen( hintfile, kRIB_LAST_RI, gRibRITable );
   RibSetHintTable( rib, kRIB_LAST_HINT, gRibHintTable );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",kRIB_LAST_RI, gRibRITable) failed.\n", 
	      hintfile );
      exit(1);
   }
   RibRead(rib);
   RibClose(rib);   
#endif
#if 1
   printf( "# ----------------------------\n" );
   gRibHintTable[kRIB_RENDERMAN_HINT] 
      = (PRIB_ARCRECFILTERPROC)RenderManRIBHintHandler;
   printf( "# Should see '"\
           "# @@@@@@@@ RenderMan RIB Hint Encountered' in following:.\n" );
   rib = RibOpen( hintfile, kRIB_LAST_RI, gRibRITable);
   RibSetHintTable( rib, kRIB_LAST_HINT, gRibHintTable );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",kRIB_LAST_RI, gRibRITable) failed.\n", 
	      hintfile );
      exit(1);
   }
   RibRead(rib);

   if (!RenderManRIBHintHit)
      printf("ERROR:  RenderMan RIB Hint Handler skipped.\n" );
   RibClose(rib);   
#endif 

   return 0;
}


int RenderManRIBHintHandler( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   register int   rc;
   register char  *s;

   RenderManRIBHintHit = RI_TRUE;

   printf( "# @@@@@@@@ RenderMan RIB Hint Encountered.\n" );
   rc = RibReadArchiveRecord(rib);
   if ( rc )
      return rc;

   s = RibCreateStringFromBuffer( rib, 0 );
   if (!s)
      return kRIB_ERRRC_INT;
   
   ((PRiArchiveRecord)*rib->ritable[kRIB_ARCHIVERECORD])( "structure", s );   
   if ( RibShouldFreeData(rib) )
   {
      _RibFree( s );
   }

   return kRIB_OK;
}
