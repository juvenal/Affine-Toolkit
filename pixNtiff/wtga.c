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
 * FILE:  wtga.c
 *
 *
 * DESCRIPTION:  Write TGA files.
 *   
 *    Contains:
 *
 *          WriteTga() - Write TGA bitmap file.
 *
 *    References:
 *          [BROW95]  Brown, C. Wayne and Shepherd, Barry J., Graphics File
 *                    Formats: Reference and Guide, Manning Publications 
 *                    Co., 1995, pp. 434-439.
 *          [MURR96]  Murray, James D. and VanRyper, William, Graphics
 *                    File Formats: Second Edition, O'Reilly & Associates, 
 *                    Inc., 1996, pp. 860-879.
 *
 *    Note:
 *       The 16bpp format is not supported: I just don't use it that much --
 *       if at all.  The ReadTga() function does support 16bpp and shows
 *       what the format looks like.  Maybe later I'll add an "int SaveAs16Bpp"
 *       parameter to WriteTga, but for now there just didn't seem to be 
 *       a need for it.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "tga.h"
#include "wtga.h"


int WriteTga( PBITMAP pBmp, char *filename, int rle )
{
   FILE           *tga;
   TGAHEADER      tgaheader;
   unsigned char  b[5],bb[4];
   int    pixelsize; /* number of bits per pixel */
   auto unsigned char      *last, *eor;
   register unsigned char  *p, *pp, *s;
   register unsigned int   count, nbytes;


   if ( pBmp->sampleformat != BITMAP_UINT8 
	|| pBmp->bitspersample != 8 
	|| ( pBmp->nsamples != 3
	     && pBmp->nsamples != 4) )
   {
      return 1;
   }

   pixelsize = pBmp->nsamples * pBmp->bitspersample;

   tgaheader.idlength = 0;
   tgaheader.clrmaptype = 0; /* No color map given. */
   tgaheader.imagetype = (rle ? 10 : 2);
   tgaheader.clrmapoffset1 = 0;
   tgaheader.clrmapoffset2 = 0;
   tgaheader.clrmaplen1 = 0;
   tgaheader.clrmaplen2 = 0;
   tgaheader.clrmapdepth = 0;
   tgaheader.xorigin1 = 0;
   tgaheader.xorigin2 = 0;
   tgaheader.yorigin1 = 0;
   tgaheader.yorigin2 = 0;
   tgaheader.width1 = pBmp->xres & 0x00ff;
   tgaheader.width2 = (pBmp->xres & 0xff00) >> 8;
   tgaheader.height1 = pBmp->yres & 0x00ff;
   tgaheader.height2 = (pBmp->yres & 0xff00) >> 8;
   tgaheader.pixeldepth = pixelsize;
   tgaheader.origin_alphabits = ( pixelsize==32 ? 8 : 0);

   /* Set orientation bits. 
    *   Note that the writer code below writes the data out from
    *   bottom to top, since most TGA files are written bottom to top
    *   and the odds are good that the given orientation will be
    *   top to bottom.  Inshort when writing the given image out
    *   starting with the last scanline, the odds are that last scanline
    *   will infact be the bottom of the image.
    */
   if ( pBmp->orientation!=BITMAP_TOPLEFT
       && pBmp->orientation!=BITMAP_TOPRIGHT )
   {
      /* If the image has the first scanline being the top, the writer
       *    below is set up to write that first scanline last resulting
       *    in a botom-to-top orientation.  See above comment.
       */
      tgaheader.origin_alphabits |= kTGA_TOP_TO_BOTTOM;
   }
   if ( pBmp->orientation==BITMAP_TOPRIGHT
	|| pBmp->orientation==BITMAP_BOTRIGHT )
   {
      tgaheader.origin_alphabits |= kTGA_RIGHT_TO_LEFT;
   }
   /* Any other orientation and the user gets a badly oriented TGA image. */
   /* Assume whoever is calling this function set up the bitmap to have
    *   an orientation that tga can handle. 
    */

   tga = fopen( filename, "wb" );
   if (!tga) 
     return 1;
   
   /* Write TGA header. */
   if ( fwrite( (char*)&tgaheader, 1, sizeof(TGAHEADER), tga ) 
       < sizeof(TGAHEADER) )
     goto Error;

   if (rle)
   {
      nbytes = pBmp->rowbytes;
      p = pp = pBmp->pbits + nbytes*(pBmp->yres-1);
      if ( pixelsize == 32)
      {
         last = pp+nbytes;
         while ( pp >= pBmp->pbits )
         {
            count = 1;
            b[1] = p[2]; /* B */
            b[2] = p[1]; /* G */
            b[3] = p[0]; /* R */
            b[4] = p[3]; /* A */
            p += 4;
            if ( b[1]!=p[2] || b[2]!=p[1] || b[3]!=p[0] || b[4]!=p[3] )
            {
               s = p; 
               while ( p < last )
               {
                  if ( ((p+4) == last ) 
                      || ( (p+4) < last
                        && (p[0]!=p[3]||p[1]!=p[4]||p[2]!=p[5]||p[3]!=p[6])))
                  {
                     p+=4;
                     count++;
                  }
                  else
                  {                
                     break;
                  }
               }
               b[0] = 0;
               while ( count > 128 )
               {
                  b[0] = 0x7f; /* Zero for top bit to indicate raw data. */
                  if ( fwrite( b, 1, 5, tga ) < 5 )
                    goto Error;
                  /* Calculate end of run. */
                  eor = s + (128*4 - 4); /* Subtract 4, s is 1 pel ahead. */
                  while ( s < eor )
                  {
                     bb[0] = s[2]; /* B */
                     bb[1] = s[1]; /* G */
                     bb[2] = s[0]; /* R */
                     bb[3] = s[3]; /* A */
                     s += 4;
                     if ( fwrite( bb, 1, 4, tga ) < 4 )
                       goto Error;
                  }
                  count -= 128;
               }
               if ( count > 0 )
               {
                  if ( b[0] )
                  {
                     b[1] = s[2]; /* B */
                     b[2] = s[1]; /* G */
                     b[3] = s[0]; /* R */
                     b[4] = s[3]; /* A */
                     s += 4;
                  }
                  b[0] = --count;
                  if ( fwrite( b, 1, 5, tga ) < 5 )
                    goto Error;
                  while ( count )
                  {
                     b[0] = s[2]; /* B */
                     b[1] = s[1]; /* G */
                     b[2] = s[0]; /* R */
                     b[3] = s[3]; /* A */
                     s += 4;
                     if ( fwrite( b, 1, 4, tga ) < 4 )
                       goto Error;
                     count--;
                  }
               }
               if ( p == last )
               {
                  last = pp;
                  pp -= nbytes; 
                  p = pp;
               }
            }
            else /* encoded packet with a count and one pixel value */
            {
               if ( p < last )
               {
                  count++;
                  p+=4;
                  if ( p < last )
                  {
                     do
                     {
                        if ( b[1]!=p[2]||b[2]!=p[1]||b[3]!=p[0]||b[4]!=p[3] )
                        {
                           break;
                        }
                        p+=4;
                        count++;
                     } while ( p < last );
                  }
               }
               if ( count > 128 )
               {
                  b[0] = 0xff; /* (128-1)|0x80 = (all bits set to 1) = 0xff */
                  do
                  {
                     if ( fwrite( b, 1, 5, tga ) < 5 )
                       goto Error;               
                     count-=128;
                  }
                  while ( count > 128 );
               }
               count--;
               b[0] = count | 0x0080;/* OR in top bit to indicate encoding. */
               if ( fwrite( b, 1, 5, tga ) < 5 )
                 goto Error;
               if ( p == last )
               {
                  last = pp;
                  pp -= nbytes;/* Writing from bottom to top, so decrement.*/
                  p = pp;
               }
            }
         }
      } else if ( pixelsize == 24)
      {
         last = pp+nbytes;
         while ( pp >= pBmp->pbits )
         {
            count = 1;
            b[1] = p[2]; /* B */
            b[2] = p[1]; /* G */
            b[3] = p[0]; /* R */
            p += 3;
            if ( b[1]!=p[2] || b[2]!=p[1] || b[3]!=p[0] )
            {
               s = p; 
               while ( p < last )
               {
                  if ( ((p+3) == last) 
                      || ( (p+3) < last
                          && (p[0]!=p[3] || p[1]!=p[4] || p[2]!=p[5]) ) )
                  {
                     p+=3;
                     count++;
                  }
                  else
                  {                
                     break;
                  }
               }
               b[0] = 0;
               while ( count > 128 )
               {
                  b[0] = 0x7f; /* Zero for top bit to indicate raw data. */
                  if ( fwrite( b, 1, 4, tga ) < 4 )
                    goto Error;
                  /* Calculate end of run. */
                  eor = s + (128*3 - 3); /* Subtract 3, s is 1 pel ahead. */
                  while ( s < eor )
                  {
                     bb[0] = s[2]; /* B */
                     bb[1] = s[1]; /* G */
                     bb[2] = s[0]; /* R */
                     s += 3;
                     if ( fwrite( bb, 1, 3, tga ) < 3 )
                       goto Error;
                  }
                  count -= 128;
               }
               if ( count > 0 )
               {
                  if ( b[0] )
                  {
                     b[1] = s[2]; /* B */
                     b[2] = s[1]; /* G */
                     b[3] = s[0]; /* R */
                     s += 3;
                  }
                  b[0] = --count;
                  if ( fwrite( b, 1, 4, tga ) < 4 )
                    goto Error;
                  while ( count )
                  {
                     b[0] = s[2]; /* B */
                     b[1] = s[1]; /* G */
                     b[2] = s[0]; /* R */
                     s += 3;
                     if ( fwrite( b, 1, 3, tga ) < 3 )
                       goto Error;
                     count--;
                  }
               }
               if (p==last)
               {
                  last = pp;
                  pp -= nbytes; 
                  p = pp;
               }
            }
            else /* encoded packet with a count and one pixel value */
            {
               if ( p < last )
               {
                  count++;
                  p+=3;
                  if ( p < last )
                  {
                     do
                     {
                        if ( b[1]!=p[2] || b[2]!=p[1] || b[3]!=p[0] )
                        {
                           break;
                        }
                        p+=3;
                        count++;
                     } while ( p < last );
                  }
               }
               if ( count > 128 )
               {
                  b[0] = 0xff; /* (128-1)|0x80 = (all bits set to 1) = 0xff */
                  do
                  {
                     if ( fwrite( b, 1, 4, tga ) < 4 )
                       goto Error;               
                     count-=128;
                  }
                  while ( count > 128 );
               }
               count--;
               b[0] = count | 0x0080;/* OR in top bit to indicate encoding. */
               if ( fwrite( b, 1, 4, tga ) < 4 )
                 goto Error;
               if ( p==last )
               {
                  last = pp;
                  pp -= nbytes;/* Writing from bottom to top, so decrement.*/
                  p = pp;
               }
            }
         }
      }
   }
   else
   {
      nbytes = pBmp->rowbytes;
      p = pp = pBmp->pbits + nbytes*(pBmp->yres-1);
      if ( pixelsize == 32)
      {
         while ( pp >= pBmp->pbits )
         {
            while( p < pp+nbytes )
            {
               putc( p[2], tga ); /* B */
               putc( p[1], tga ); /* G */
               putc( p[0], tga ); /* R */
               putc( p[3], tga ); /* A */
               p += 4;
            }
            pp -= nbytes; /* Writing from bottom to top, so decrement. */
            p = pp;
         }
      } else if ( pixelsize == 24)
      {
         while ( pp >= pBmp->pbits )
         {
            while( p < pp+nbytes )
            {
               putc( p[2], tga ); /* B */
               putc( p[1], tga ); /* G */
               putc( p[0], tga ); /* R */
               p += 3;
            }
            pp -= nbytes; /* Writing from bottom to top, so decrement. */
            p = pp;
         }
      }
   }

   fclose( tga );
   return 0;

 Error:
   fclose( tga );
   return 1;
}
