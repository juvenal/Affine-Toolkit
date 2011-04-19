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
 * FILE:  busybits.c
 *
 * DESCRIPTION:  Test application that creates an image of a given resolution
 *               that is difficult to rle compress.
 *   
 *
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.h"
#include "wtiff.h"


void PrintHelp( void );
int main(int argc, char **argv);


void PrintHelp( void )
{
   printf( "Test application that creates an image of a given\n" \
	   "resolution that is difficult to rle compress.\n\n" );
   printf( "busybits xres yres bitdepth nchannels tiff_filename\n"      \
	   "   xres            Test image width.\n"                     \
	   "   yres            Test image height.\n"                    \
	   "   bitdepth        Number of bits per channel (8 or 16).\n" \
	   "   nchannels       Number of channels (1,3, or 4).\n"       \
	   "   tiff_filename   Output TIFF file name.\n" );
}


int main(int argc, char **argv) 
{
   PBITMAP        tiff = NULL;
   unsigned int   xres;
   unsigned int   yres;
   unsigned int   bitdepth;
   unsigned int   nchannels;
   unsigned int   x,y;
   unsigned char  *p;


   if ( argc < 6 )
      goto Error;

   xres = atoi(argv[1]);
   if ( xres==0 || xres>32768 )
      goto Error;

   yres = atoi(argv[2]);
   if ( yres==0 || yres>32768 )
      goto Error;

   bitdepth = atoi(argv[3]);
   if ( bitdepth!=8 && bitdepth!=16 )
      goto Error;

   nchannels = atoi(argv[4]);
   if ( nchannels !=1 && nchannels !=3 && nchannels!=4 )
      goto Error;

   tiff = CreateBitmap( xres, yres, 
			0, 0, 
			nchannels, 
			(bitdepth==8 ? BITMAP_UINT8 : BITMAP_UINT16),
			bitdepth );
   if (!tiff)
   {
      fprintf( stderr, "Can't allocate memory for TIFF %s\n", argv[5] );
      return 1;
   }

   p = tiff->pbits;
   for ( y=0; y<tiff->yres; y++ )
   {
      for ( x=0; x<tiff->rowbytes; x++ )
      {
	 *p++ = (unsigned char)(0x000000ff & rand());
      }
   }

   if (WriteTiff( tiff, argv[5] ))
   {
      fprintf( stderr, "Can't write file: \"%s\"\n", argv[5] );
      goto Exit;
   }

   DestroyBitmap( tiff );

   return 0;

 Error:
   PrintHelp();
 Exit:
   if (tiff)
      DestroyBitmap( tiff );
   return 1;
}
