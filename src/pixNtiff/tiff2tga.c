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
 * FILE:  tiff2tga.c
 *
 * DESCRIPTION:  Utility to convert 24 and 32 bit TIFF files
 *               to TGA files.
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
#include "rtiff.h"
#include "wtga.h"


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
   printf("\nUsage: %s [-rle] tiff_filename tga_filename\n"               \
          "   [-rle]              Compress image using RLE.\n"             \
          "   tiff_filename       TIFF file to read from.\n"               \
          "   tga_filename        TGA file to write to.\n", toolname);
}


int main(int argc, char **argv) 
{
   PBITMAP  tiff;
   int  rle = 0;
   int  i = 1;
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


   if ( argc < 3 || (argc > 1 && argv[1][0]=='-'
       && !(i++,rle=!strcmp("rle",&argv[1][1])) ) )
   {
      PrintHelp(toolname);
      return 1;
   }

   /* Check that the input file name doesn't match the output file name. */   if (!strcmp(argv[i],argv[i+1]))
   {
      fprintf( stderr, "Filenames can not refer to the same file: \"%s\".\n", 
              argv[i] );
      return 1;            
   }

   tiff = ReadTiff( argv[i], READTIFF_CONVERT16TO8 );
   if (!tiff)
   {
      fprintf( stderr, "Can't read %s\n", argv[i] );
      return 1;
   }

   /* The TGA writer outputs bottom-to-top given a top-to-bottom image. 
    *    Most TIFFs are top-to-bottom, so the odds are good that the follow
    *    call is a simple return.
    */
   tiff = OrientBitmap( tiff, BITMAP_TOPLEFT );

   i++;
   if (WriteTga( tiff, argv[i], rle ))
   {
      fprintf( stderr, "Can't write %s\n", argv[i] );
      DestroyBitmap( tiff );
      return 1;
   }

   DestroyBitmap( tiff );
   
   return 0;
}
