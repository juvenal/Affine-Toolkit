/* 
 * Copyright (c) 1996, 1997, 1998 Thomas E. Burge.  All rights reserved.  
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
 * FILE:  pix2tiff.c
 *
 * DESCRIPTION:  Utility to convert Alias/WaveFront pix files to tiff
 *               format.
 *   
 *
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "bitmap.h"
#include "rpix.h"
#include "wtiff.h"


void PrintVersion(const char* toolname)
{
   printf("%s version %s\n", toolname, AFFINE_VERSION);
   printf("%s", RAT_COPYRIGHT_STATEMENT);
   printf("%s", RENDERMAN_COPYRIGHT_STATEMENT);
}

void PrintHelp(const char* toolname)
{
   printf("%s\n", toolname);
   printf("%s", RAT_COPYRIGHT_STATEMENT);
   printf("%s", RENDERMAN_COPYRIGHT_STATEMENT);
   printf("\nUsage: %s pix_filename tiff_filename\n"                      \
          "   pix_filename        Alias/WaveFront pix file to read.\n"     \
          "   tiff_filename       TIFF file to write to.\n", toolname);
}


int main(int argc, char **argv) 
{
   PBITMAP  pix;
   const char *toolname = argv[0];

   if (argc > 1) {
      if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
         PrintVersion(toolname);
         return 0;
      }
      if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
         PrintHelp(toolname);
         return 0;
      }
   }

   if ( argc < 3 )
   {
      /* User didn't provide enough arguments, so just print help text. */
      PrintHelp(toolname);
      return 1;
   }
   
   /* Check that the input file name doesn't match the output file name. */
   if (!strcmp(argv[1],argv[2]))
   {
      fprintf( stderr, "Filenames can not refer to the same file: \"%s\".\n", 
              argv[1] );
      return 1;            
   }

   pix = ReadPix( argv[1] );

   if (!pix)
   {
      fprintf( stderr, "Can't read file: \"%s\"\n", argv[1] );
      return 1;
   }

   if (WriteTiff( pix, argv[2] ))
   {
      fprintf( stderr, "Can't write file: \"%s\"\n", argv[2] );
      DestroyBitmap( pix );
      return 1;
   }

   DestroyBitmap( pix );

   return 0;
}
