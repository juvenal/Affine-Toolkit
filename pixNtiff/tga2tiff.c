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
 * FILE:  tga2tiff.c
 *
 * DESCRIPTION:  Utility to convert TGA files to TIFF format.
 *   
 *
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>
#include "bitmap.h"
#include "rtga.h"
#include "wtiff.h"


int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   PBITMAP  tga;

   if ( argc < 3 || (argc > 1 && argv[1][0]=='-') )
   {
      /* User tried to specify an option, so just print help text. */
      printf( "tga2tiff tga_filename tiff_filename\n"                      \
          "   tga_filename        TGA file to read.\n"     \
          "   tiff_filename       TIFF file to write to.\n" );
      return 1;
   }
   
   /* Check that the input file name doesn't match the output file name. */
   if (!strcmp(argv[1],argv[2]))
   {
      fprintf( stderr, "Filenames can not refer to the same file: \"%s\".\n", 
              argv[1] );
      return 1;            
   }

   tga = ReadTga( argv[1] );

   if (!tga)
   {
      fprintf( stderr, "Can't read file: \"%s\"\n", argv[1] );
      return 1;
   }

   /* Most TGA writer output BITMAP_BOTLEFT not BITMAP_TOPLEFT. 
    * Most TIFF readers assume BITMAP_TOPLEFT. 
    */
   tga = OrientBitmap( tga, BITMAP_TOPLEFT );

   if (WriteTiff( tga, argv[2] ))
   {
      fprintf( stderr, "Can't write file: \"%s\"\n", argv[2] );
      DestroyBitmap( tga );
      return 1;
   }

   DestroyBitmap( tga );

   return 0;
}
