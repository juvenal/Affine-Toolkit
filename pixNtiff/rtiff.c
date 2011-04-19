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
 * FILE:  rtiff.c
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          ReadTiff() - using libtiff library.
 *
 *    References:
 *          [ALDU92]  TIFF: Revision 6.0, Final -- June 3, 1992, 
 *                    Aldus Corp., CA, June 1992.
 * 
 *    History:
 *        05-24-99  Changed READTIFF_CONVERT8TO16 to fill the 8 bottom
 *                  bits that opened up when going from 8 to 16 to be
 *                  a copy of the 8 top bits.  
 *
 *
 *             The libtiff.a library used by this program is:
 *          Copyright (c) 1988, 1989, 1990, 1991, 1992 Sam Leffler
 *             Copyright (c) 1991, 1992 Silicon Graphics, Inc.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tiffio.h>
#include "bitmap.h"
#include "rtiff.h"


PBITMAP ReadTiff( char *filename, unsigned int flags )
{
   TIFF             *tif;
   uint32           xres = 0;
   uint32           yres = 0;
   float            xpos = 0.0;
   float            ypos = 0.0;
   uint16           bitspersample = 0;
   uint16           nsamples = 0;
   uint16           config = PLANARCONFIG_CONTIG;
   uint16           orientation = ORIENTATION_TOPLEFT;
   uint16           samples,*sampleinfo;
   uint16           sampleformat;
   PBITMAP          pBmp;
   unsigned char    *p8 = NULL;
   uint16           *p16 = NULL;
   uint16           *pp16;
   unsigned char    *p,*c;
   register uint32  i;


   tif = TIFFOpen( filename, "r" );
   if (!tif) 
   {
      printf( "Error opening %s\n", filename );
      return NULL;
   }

   samples = 0;
   sampleinfo = NULL;
   TIFFGetField( tif, TIFFTAG_EXTRASAMPLES,&samples, &sampleinfo );
   if (sampleinfo && samples!=1 && sampleinfo[0] != EXTRASAMPLE_ASSOCALPHA)
   {
       printf("Extra channel not an alpha channel\n" );
       goto Error;
   }
   
   samples = SAMPLEFORMAT_UINT;
   TIFFGetField( tif, TIFFTAG_SAMPLEFORMAT, (uint16*)&samples );
   TIFFGetField( tif, TIFFTAG_SAMPLESPERPIXEL, (uint16*)&nsamples );
   TIFFGetField( tif, TIFFTAG_PLANARCONFIG, (uint16*)&config );
   if (config!=PLANARCONFIG_CONTIG)
   {
      printf( "Input file not PLANARCONFIG_CONTIG\n" );
      goto Error;
   }

   TIFFGetField( tif, TIFFTAG_BITSPERSAMPLE, &bitspersample );
   if ( (flags & READTIFF_ALLOW8ONLY) && bitspersample!=8 )
   {
      printf( "TIFF needs to be 8 bits per channel.\n" );
      goto Error;
   }
   if ( (bitspersample!=8 && bitspersample!=16)
	&& !(bitspersample==1 && nsamples==1) 
	&& !(bitspersample==32 && nsamples==1) )
   {
      printf( "TIFF needs to be 1, 8, 16 or 32 bits per channel.  If 32\n" \
	      "bits then the values must be IEEE floating point values.\n"  \
	      "One bit per channel is allowed only if there is only one\n"  \
	      "channel.\n" );
      goto Error;
   }
   TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &xres );
   TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &yres );   

   /* Note that libtiff leaves xpos and ypos untouched if the TIFF file
    *   does not have the POSITION tags.  So initialize xpos and ypos
    *   to 0.0.  This initialization is done above where the variables
    *   are declared.
    */
   TIFFGetField( tif, TIFFTAG_XPOSITION, &xpos );
   TIFFGetField( tif, TIFFTAG_YPOSITION, &ypos );   

   /* 7-10-97 PRMan 3.7 stopped specifiying the sample format field. */
   /*SEE ABOVE: if ( samples==SAMPLEFORMAT_IEEEFP ) */
   if ( nsamples == 1 && bitspersample==32 )
   {
      sampleformat = BITMAP_IEEE;
   }
   else if ( bitspersample==1 )
   {
      sampleformat = BITMAP_MONO;      
   }
   else if ( bitspersample==8 )
   {
      if ( flags & READTIFF_CONVERT8TO16 )
      {
	 sampleformat = BITMAP_UINT16;
	 bitspersample = 16;
	 p8 = (unsigned char*)_TIFFmalloc( (long)(xres*nsamples) );
	 if (!p8)
	 {
	    goto MemoryAllocationError;
	 }
      }
      else
      {
	 sampleformat = BITMAP_UINT8;      
      }
   }
   else if ( bitspersample==16 )
   {
      if ( flags & READTIFF_CONVERT16TO8 )
      {
	 sampleformat = BITMAP_UINT8;
	 bitspersample = 8;
	 p16 = (uint16*)_TIFFmalloc( (long)(xres*nsamples*sizeof(uint16)) );
	 if (!p16)
	 {
	    goto MemoryAllocationError;
	 }
      }
      else
      {
	 sampleformat = BITMAP_UINT16;
      }
   }
   else
   {
      goto Error;
   }

   pBmp = CreateBitmap( xres, yres, xpos, ypos, 
			nsamples, sampleformat, bitspersample );
   if (!pBmp)
   {
      goto MemoryAllocationError;
   }

   /* Assign target address. */
   p = pBmp->pbits;

   /* If a conversion from 16 to 8 bits per sample is needed, p16 is
    *    not NULL and points to a 16 bit scanline that should be used.
    */
   if ( p16 )
   {
      for ( i=0; i<yres; i++ )
      {
	 if (TIFFReadScanline( tif, p16, i, 0 ) < 0)
	 {
	    printf( "Error reading %s\n", filename );
	    goto Error;
	 }
	 pp16 = &p16[0];
	 while ( pp16 < (p16 + pBmp->rowbytes) )
	 {
	    *p = (unsigned char)(*pp16 >> 8);
	    p++;
	    pp16++;
	 }
      }
   }
   else if ( p8 )
   {
      p16 = (uint16*)p;
      for ( i=0; i<yres; i++ )
      {
	 if (TIFFReadScanline( tif, p8, i, 0 ) < 0)
	 {
	    printf( "Error reading %s\n", filename );
	    p16=NULL; /* Set to NULL to avoid freeing p16 which equals p. */
	    goto Error;
	 }
	 c = &p8[0];
	 while ( c < p8 + pBmp->xres*pBmp->nsamples )
	 {
	    /* Don't just have the lower byte be zero.  If you add bits to
	     * a channel shift the data to the left but also take the upper 
	     * bits of the original data a copy them to the bits that open
	     * up on the right side.  So if going from 8 bits to 10 bits
	     * copy the upper two bits of the 8 bit data to the lower 2 bits
	     * of the 10 bit data.  
	     * 
	     * This copying of upper bits gives a better ramping of the
	     * the colors to the larger channel width because the brighter
	     * color with more upper bits set gets a slightly brighter
	     * color having those bits copied to the bottom.
	     */
	    *p16 = (((uint16)*c) << 8) | ((uint16)*c);
	    p16++;
	    c++;
	 }
      }
      p16=NULL; /* Set to NULL to avoid freeing p16 which equals p. */
   }
   else
   {
      for ( i=0; i<yres; i++ )
      {
	 if (TIFFReadScanline( tif, p, i, 0 ) < 0)
	 {
	    printf( "Error reading %s\n", filename );
	    goto Error;
	 }
	 p += pBmp->rowbytes;
      }
   }

   /* At this point the orientation should be checked and
    *    the bitmap flipped over from top-to-bottom or left-to-right
    *    as needed.  
    *
    * Note that if an TIFFTAG_ORIENTATION tag is not present, orientation
    *    is set to 32767.
    */
   TIFFGetField( tif, TIFFTAG_ORIENTATION, (uint16*)&orientation );

   /* Note that the flags for orientation in bitmap.h are the TIFF 
    *    orientation values minus one. 
    * The libtiff orientation flags are from 1 to 7.
    */
   if (orientation>0 && orientation<9) 
      pBmp->orientation = orientation - 1;
   /* Else keep default of orientation BITMAP_TOPLEFT CreateBitmap() set. */
   
   if (p8)
      _TIFFfree(p8);
   if (p16)
      _TIFFfree(p16);
   TIFFClose( tif );

   return pBmp;

 MemoryAllocationError:
   printf( "Memory allocation error.\n" );

 Error:
   if (p8)
      _TIFFfree(p8);
   if (p16)
      _TIFFfree(p16);
   TIFFClose( tif );
   return NULL;
}
