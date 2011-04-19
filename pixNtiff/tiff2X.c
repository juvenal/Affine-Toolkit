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
 * DESCRIPTION:  Utility to convert 1-bit per pel (black and white) TIFF files
 *               to X window text bitmaps.
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
#include "rtiff.h"
#include "wX.h"


int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   PBITMAP  tiff;

   if ( argc < 3 || (argc > 1 && argv[1][0]=='-') )
   {
      /* User tried to specify an option, so just print help text. */
      printf( "tiff2pix tiff_filename X_filename\n"                      \
          "   tiff_filename     TIFF file to read from.\n"               \
          "   X_filename        X windows bitmap file.\n" );
      return 1;
   }
   
   /* Check that the input file name doesn't match the output file name. */
   if (!strcmp(argv[1],argv[2]))
   {
      fprintf( stderr, "Filenames can not refer to the same file: \"%s\".\n", 
              argv[1] );
      return 1;            
   }

   tiff = ReadTiff( argv[1], 0 );
   if (!tiff)
   {
      fprintf( stderr, "Can't read %s\n", argv[1] );
      return 1;
   }
   tiff = OrientBitmap( tiff, BITMAP_TOPLEFT );

   if (tiff->nsamples!=1 && tiff->bitspersample!=1)
   {
      fprintf( stderr, "Requires data to be 1 bit per pixel.\n" );
      return 1;
   }     

   if (WriteX( tiff, argv[2] ))
   {
      fprintf( stderr, "Can't write %s\n", argv[2] );
      return 1;
   }

   DestroyBitmap( tiff );
   
   return 0;
}
