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
 * FILE:  wpix.c
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          WritePix() - Write Alias Research pix bitmap file.
 *
 *    History:
 *        ?-?-96  Created.  Need to look back over archives for actual date.
 *      12-12-97  Comment grammar change.
 *
 *    References:
 *          [ALIA94]   Alias Research Inc., Alias Reference Guide Vol. 2, 
 *                     110 Richmond St. East, Toronto, Ontario, 
 *                     Canada, M5C 1P1, 1994, pp. 1489-1492.
 *          [ALIA95a]  Alias Research, Alias Sketch!: Reference Guide,
 *                     110 Richmond St. East, Toronto, Ontario, 
 *                     Canada, M5C 1P1, 1995, p. 19.
 *          [ALIA95b]  Alias|Wavefront, Alias Menu Book V7.0 (PowerAnimator), 
 *                     110 Richmond St. East, Toronto, Ontario, 
 *                     Canada, M5C 1P1, 1995, pp. 901-903.
 * 
 *    Notes:
 *       [ALIA95] states the order is bgr not rgb.  Also a run length encoded
 *       entry can not continue onto the next scan line.  Must check
 *       for the end of the scan line when going through the data and 
 *       counting how many sequential pixels are of the same color.
 *
 *       The rgb+alpha format doesn't appear to be liked by Power Animator's
 *       "File/Show pix" tool.  You get an "Incorrect File Type:" message
 *       box.  Sketch!'s "File/Show image..." appears to accept the alpha
 *       channel.
 *
 *    Assumptions:
 *       The internal data structure bitmap is assumed to contain bitmap
 *       data with no padding separating the scanlines.  This should not
 *       prove to be a big deal since only 8, 24 and 32 bit per pixel is
 *       supported, so no padding should be used anyway.
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "wpix.h"


int WritePix( PBITMAP pBmp, char *filename )
{
   FILE           *pix;
   unsigned char  b[5];
   int    pixelsize; /* number of bits per pixel */
   register unsigned char  *p, *pp, *last;
   register unsigned int  count;


   if ( pBmp->sampleformat != BITMAP_UINT8 
	|| pBmp->bitspersample != 8 
	|| ( pBmp->nsamples != 1
	     && pBmp->nsamples != 3
	     && pBmp->nsamples != 4) )
   {
      return 1;
   }

   pixelsize = pBmp->nsamples * pBmp->bitspersample;

   pix = fopen( filename, "wb" );
   if (!pix) 
     return 1;

   /* Write xres. */
   b[0] = (pBmp->xres & 0x0000ff00) >> 8;
   b[1] = (pBmp->xres & 0x000000ff);
   if ( fwrite( b, 1, 2, pix ) < 2 )
     goto Error;     

   /* Write yres. */
   b[0] = (pBmp->yres & 0x0000ff00) >> 8;
   b[1] = (pBmp->yres & 0x000000ff);
   if ( fwrite( b, 1, 2, pix ) < 2 )
     goto Error;

   /* Write out two unused fields.  Using File/ShowImage[] and choosing Header,
    *    Alias PA will write out a header.  Along with the format, resolution, 
    *    number of channels and bits per channel, PA 7.5 displays the two 
    *    unused fields as a pair called an "offset".
    * These fields are supposed to be unused, but the yoffset always seems
    *    to be set to yres-1.  So far I've only seen the xoffset as a 0.
    */
   count = pBmp->yres - 1;
   b[0] = 0;
   b[1] = 0;
   b[2] = (count & 0x0000ff00) >> 8;
   b[3] = (count & 0x000000ff);
   if ( fwrite( b, 1, 4, pix ) < 4 )
     goto Error;

   /* Write bits per pixel. */
   b[0] = (pixelsize & 0x0000ff00) >> 8;
   b[1] = (pixelsize & 0x000000ff);
   if ( fwrite( b, 1, 2, pix ) < 2 )
     goto Error;

   p = pp = pBmp->pbits;
   pp += pBmp->rowbytes;
   last = p + pBmp->rowbytes * pBmp->yres;
   if ( pixelsize == 32)
   {
      while ( p < last )
      {
         count = 1; 
         b[1] = p[2]; /* r */
         b[2] = p[1]; /* g */
         b[3] = p[0]; /* b */
         b[4] = p[3]; /* a */
         p += 4;
         while ( p < pp
                && b[1]==p[2] && b[2]==p[1] && b[3]==p[0] && b[4]==p[3] )
         {
            p+=4;
            count++;
         }
         if (p==pp)
         {
            pp += pBmp->rowbytes;
         }
         if ( count > 255 )
         {
            b[0] = 255;
            do
            {
               if ( fwrite( b, 1, 5, pix ) < 5 )
                 goto Error;               
               count-=255;
            }
            while ( count > 255 );
         }
         b[0] = count;
         if ( fwrite( b, 1, 5, pix ) < 5 )
           goto Error;
      }
   } else if ( pixelsize == 24)
   {
      while ( p < last )
      {
         count = 1;
         b[1] = p[2]; /* r */
         b[2] = p[1]; /* g */
         b[3] = p[0]; /* b */
         p += 3;
         while ( p < pp && b[1]==p[2] && b[2]==p[1] && b[3]==p[0] )
         {
            p+=3;
            count++;
         }
         if (p==pp)
         {
            pp += pBmp->rowbytes;
         }
         if ( count > 255 )
         {
            b[0] = 255;
            do
            {
               if ( fwrite( b, 1, 4, pix ) < 4 )
                 goto Error;               
               count-=255;
            }
            while ( count > 255 );
         }
         b[0] = count;
         if ( fwrite( b, 1, 4, pix ) < 4 )
           goto Error;
      }
   } else if ( pixelsize == 8 )
   {
      while ( p < last )
      {
         count = 1;
         b[1] = p[0]; /* a */
         p++;
         while ( p < pp && b[1]==p[0] )
         {
            p++;
            count++;
         }
         if (p==pp)
         {
            pp += pBmp->rowbytes;
         }
         if ( count > 255 )
         {
           b[0] = 255;
           do
           {
              if ( fwrite( b, 1, 2, pix ) < 2 )
                goto Error;               
              count-=255;
           }
           while ( count > 255 );
        }
         b[0] = count;
         if ( fwrite( b, 1, 2, pix ) < 2 )
           goto Error;
      }
   }
   else
   {
      goto Error;
   }
   
   fclose( pix );

   return 0;

 Error:
   fclose( pix );
   return 1;
}
