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
 * FILE:  tiffinvert.c
 *
 * DESCRIPTION:  Test application.
 *   
 *
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>
#include <tiffio.h>
#include "bitmap.h"
#include "rtiff.h"
#include "wtiff.h"


int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   PBITMAP      tiff;
   uint32       n;
   uint8        *p8;
   uint16       *p16;
   auto uint32  i,j,x,y;


   if ( argc < 2 || (argc > 1 && argv[1][0]=='-') )
   {
      /* User tried to specify an option, so just print help text. */
      printf( "tiffinvert tiff_filename1 ...\n"                         \
          "   tiff_filename1 ...   TIFF file(s) to overwrite with an\n" \
          "                        an inverted version of their image.\n" );
      return 1;
   }

   for ( i=1; i<argc; i++ )
   {
      tiff = ReadTiff( argv[i], 0 );
   
      if ( !tiff )
      {
	 fprintf( stderr, "Can't read %s\n", argv[i] );
	 return 1;
      }

      if ( tiff->sampleformat!=BITMAP_UINT8 
	   && tiff->sampleformat!=BITMAP_UINT16 )
      {
	 fprintf( stderr, 
		  "TIFF file %s is not an \"r\", \"rgb\" or \"rgba\" image\n", 
		  argv[i] );
	 goto Skip;
      }

      if ( tiff->sampleformat==BITMAP_UINT8 )
      {
	 n = tiff->yres * tiff->rowbytes;
	 p8 = tiff->pbits;
	 for ( j=0; j<n; j++ )
	    *p8++ = 0xff - *p8;	 
      }
      else /* tiff->sampleformat==BITMAP_UINT16 */
      {
	 n = tiff->xres * tiff->nsamples;
	 p16 = (uint16*)tiff->pbits;
	 for ( y=0; y<tiff->yres; y++ )
	    for ( x=0; x<n; x++ )
	       *p16++ = 0xffff - *p16;	 
      }

      if (WriteTiff( tiff, argv[i] ))
      {
	 fprintf( stderr, "Can't write file: \"%s\"\n", argv[i] );
	 goto Error;
      }

   Skip:
      DestroyBitmap( tiff );
   }

   return 0;

 Error:
   DestroyBitmap( tiff );
   return 1;
}
