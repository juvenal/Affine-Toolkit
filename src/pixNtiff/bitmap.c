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
 * FILE:  bitmap.c
 *
 * HISTORY:
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          CreateBitmap() - 
 *          
 *    References:
 *
 */
#define __BITMAP_FILE__
#define BITMAP_PRIV
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef LIBTIFF
#include <tiffio.h>
#endif
#include "bitmap.h"



PBITMAP CreateBitmap( unsigned int  xres,
		      unsigned int  yres,
		      unsigned int  xpos,
		      unsigned int  ypos,
		      unsigned int  nsamples,
		      unsigned int  sampleformat,
		      unsigned int  bitspersample )
{
    PBITMAP  p;
    unsigned int  rowsize,size,pixelsize;

    switch (bitspersample) {
     case 1:
       if (sampleformat!=BITMAP_MONO)
	  return NULL;
       break;
     case 8:
       if (sampleformat!=BITMAP_UINT8)
	  return NULL;
       break;
     case 16:
       if (sampleformat!=BITMAP_UINT16)
	  return NULL;
       break;
     case 32:
       if (sampleformat!=BITMAP_IEEE)
	  return NULL;
       break;
    }
       
    pixelsize = nsamples * bitspersample;
    rowsize = xres * pixelsize; /* Number of bits, change to bytes later. */

    /* If a pel takes some weird number of bits, the last pel may
     *   not end on a byte boundary.  Padding of a byte maybe needed.
     */
    if ( rowsize & 0x0007 )
      rowsize = (rowsize>>3) + 1;
    else
      rowsize = rowsize>>3;
    size = yres * rowsize + (unsigned int)sizeof(BITMAP);

#ifdef LIBTIFF
    p = (PBITMAP)_TIFFmalloc( (long)size );
#else
    p = (PBITMAP)malloc( size );
#endif
    if ( !p )
      return NULL;

    memset( p, '\0', size );
    
    p->xres = xres;
    p->yres = yres;
    p->xpos = xpos;
    p->ypos = ypos;
    p->sampleformat  = sampleformat;
    p->orientation   = BITMAP_TOPLEFT; /* BITMAP_TOPLEFT is zero. */
    p->nsamples      = nsamples;
    p->bitspersample = bitspersample;
    p->rowbytes      = rowsize;
    p->pbits         = (PBITS)p + sizeof(BITMAP);

    return p;
}


int DestroyBitmap( PBITMAP pBmp )
{
#ifdef LIBTIFF
    _TIFFfree( pBmp );
#else
    free( pBmp );
#endif   
    return 0;
}
