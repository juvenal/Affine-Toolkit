/* 
 * Copyright (c) 1998 Thomas E. Burge.  All rights reserved.  
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
 * FILE:  tifftile.c
 *
 * DESCRIPTION:  Assembles CropWindow images into a single image.
 *   
 *
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"
#include "rtiff.h"
#include "wtiff.h"


int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   PBITMAP        *tifftiles;
   PBITMAP        tile;
   char           *tilename;
   int            ntifftiles;
   PBITMAP        tiff = NULL;
   char           *tiffname;
   int            bitdepth, xres, yres, res;
   int            bytesperpixel;
   unsigned char  *s, *t;
   int            i,j;
   int            rc = 0;


   if ( argc < 3 || (argc > 1 && argv[1][0]=='-') )
   {
      /* User tried to specify an option, so just print help text. */
      printf( "tifftile [tiff_tile1 ...] tiff_finalimage\n"              \
          "   tiff_tile1 ...       List of TIFF CropWindow images.\n"    \
          "   tiff_filename2       TIFF file to write final image to.\n" );
      return 1;
   }

   tiffname = argv[argc-1];
   ntifftiles = argc-2;
   tifftiles = (PBITMAP*)malloc( ntifftiles * sizeof(PBITMAP) );
   if (!tifftiles)
      return 1;

   /* Read the tiles. */
   for ( i=0; i<ntifftiles; i++ )
   {
      tilename = argv[i+1];

      /* Check that the input file name doesn't match the output file name. */
      if ( !strcmp(tilename,tiffname) )
      {
	 fprintf( stderr, 
		  "Filenames can not refer to the same file: \"%s\".\n", 
		  tilename );
	 rc = 1;
	 goto Error;
      }
      
      tile = ReadTiff( tilename, READTIFF_ALLOW8ONLY );
      if ( !tile )
      {
	 fprintf( stderr, "Can't read %s\n", tilename );
	 rc = 1;
	 goto Error;
      }

      tile = OrientBitmap( tile, BITMAP_TOPLEFT );

      /* Keep the types of input TIFF files simple. */
      bitdepth = tile->nsamples * tile->bitspersample;
      if ( bitdepth!=8 && bitdepth!=24 && bitdepth!=32 )
      {
	 fprintf( stderr, 
		  "TIFF file %s is not an \"r\", \"rgb\" or \"rgba\" image\n", 
		  tilename );
	 rc = 1;
	 goto Error;
      }

      tifftiles[i] = tile;
   }

   /* Check if all the tiles are the same bit depth and get maximum
    *    x and y resolutions. 
    */
   xres = 0;
   yres = 0;
   bitdepth = tifftiles[0]->nsamples * tifftiles[0]->bitspersample;
   for ( i=0; i<ntifftiles; i++ )
   {
      tile = tifftiles[i];
      if ( bitdepth != tile->nsamples * tile->bitspersample )
      {
	 fprintf( stderr, "TIFF files can not be different bit depths.\n" );
	 rc = 1;
	 goto Error;	 
      }
      res = tile->xres + tile->xpos;
      if ( xres < res )
	 xres = res;
      res = tile->yres + tile->ypos;
      if ( yres < res )
	 yres = res;
   }
   if ( xres==0 || yres==0 )
   {
      /* Something went wrong when reading in the tiles. */
      fprintf( stderr, "Error reading TIFF files.\n" );
      rc = 1;
      goto Error;	 
   }

   /* Create target image. */
   tiff = CreateBitmap( xres, yres, 0, 0, 
			tifftiles[0]->nsamples, 
			tifftiles[0]->sampleformat, 
			tifftiles[0]->bitspersample );
   if (!tiff)
   {
      fprintf( stderr, "Memory allocation error.\n" );
      rc = 1;
      goto Error;	 
   }

   /* Clear the target image incase the tiles do not completely cover it. */
   memset( tiff->pbits, 0, tiff->rowbytes * tiff->yres );

   /* Calculate how many bytesperpixel. */
   bytesperpixel = bitdepth / 8;

   /* Assemble tiles. */
   for ( i=0; i<ntifftiles; i++ )
   {
      tile = tifftiles[i];

      /* Get the pointer to the source image. */
      s = tile->pbits;

      /* Calculate starting point in target image. 
       *    A bitmap's pbits points to the upper left corner of an image.
       *    The xpos and ypos values give the tile's upper left corner in
       *    the target image.
       */
      t = tiff->pbits 
	 + tile->ypos*tiff->rowbytes /* full scanlines above tile. */
	 + tile->xpos*bytesperpixel; /* bytes left of tile. */

      /* BitBlt the tile into the target image. */
      for ( j=0; j<tile->yres; j++ )
      {
	 memcpy( t, s, tile->rowbytes );
	 s += tile->rowbytes;
	 t += tiff->rowbytes;
      }
   }   

   if (WriteTiff( tiff, tiffname ))
   {
      fprintf( stderr, "Can't write file: \"%s\"\n", tiffname );
      rc = 1;
      goto Error;
   }

   /* No errors, but fall through to clean-up code anyway 
    *    with rc already set to zero.
    */

 Error:
   for ( i=0; i<ntifftiles; i++ )
   {
      if (tifftiles[i])
	 DestroyBitmap( tifftiles[i] );
   }
   if (tiff)
   {
      DestroyBitmap( tiff );
   }

   return rc;
}
