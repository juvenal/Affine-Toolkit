/* $RCSfile: rtga.c,v $  $Revision: 1.2 $ $Date: 2000/02/03 07:32:29 $
 * 
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
 * FILE:  rtga.c
 *
 *
 * DESCRIPTION:  This code is mainly for the files created by such programs 
 *               as 3D Studio that default to TGA format for their output.
 *   
 *    Contains:
 *
 *          ReadTga() - Read TGA bitmap file.
 *
 *       ?-?-96   Created.  Need to look back over archives for actual date.
 *      12-12-97  Comment grammar change.
 *
 *    References:
 *          [BROW95]  Brown, C. Wayne and Shepherd, Barry J., Graphics File
 *                    Formats: Reference and Guide, Manning Publications 
 *                    Co., 1995, pp. 434-439.
 *          [KAYD95]  Kay, David C. and Levine, John R., Graphics File 
 *                    Formats: 2nd Edition, Windcrest/McGraw-Hill, 1995, 
 *                    p. 156.
 *          [MURR96]  Murray, James D. and VanRyper, William, Graphics
 *                    File Formats: Second Edition, O'Reilly & Associates, 
 *                    Inc., 1996, pp. 860-879.
 *          [TRUE91]  Truevision Inc.,  Truevision TGA: File Format 
 *                    Specification, Version 2.0, 1991.  
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "tga.h"
#include "rtga.h"


PBITMAP ReadTga( char *filename )
{
   unsigned char  b[5];
   FILE           *tga;
   PBITMAP        pBmp = NULL;
   TGAHEADER      tgaheader;
   unsigned int   xres, yres, nsamples;
   int            bitsperpel;
   int            rle;
   register char  *p,*pp;
   register unsigned int  i,n,nbytes;

   
   tga = fopen( filename, "rb" );
   if (!tga) 
     return NULL;
   
   /* Read TGA header. */
   if ( fread( (char*)&tgaheader, 1, sizeof(TGAHEADER), tga ) 
       < sizeof(TGAHEADER) )
     goto Error;
   
   /* TGA uses little-endian. */
   xres = tgaheader.width2<<8  | tgaheader.width1;
   yres = tgaheader.height2<<8 | tgaheader.height1;
   
   bitsperpel = tgaheader.pixeldepth;
   
   /* Allow only 16, 24 and 32 bits per pixel. */
   switch ( bitsperpel ) {
    case 16:
    case 24:
      nsamples = 3;
      break;
    case 32:
      nsamples = 4;
      break;
    default:
      goto Error;
   }

   /* Get the image type. */
   switch (tgaheader.imagetype)
   {
    case 10: /* RLE Truecolor */
      rle = ENCODING;
      break;
    case 2: /* Truecolor */
      rle = NO_ENCODING;
      break;
      /* case 0: */  /* No image in file */
      /* case 1: */  /* Colormapped */
      /* case 3: */  /* Monochrome */
      /* case 9: */  /* RLE Colormapped */
                /* case 11: */ /* RLE Monchrome */
    default:
      goto Error;
   }

   pBmp = CreateBitmap( xres, yres, 0, 0, nsamples, BITMAP_UINT8, 8 );
   if (!pBmp)
     goto Error;

   /* Get image ID string length. */
   /* This is a string located after the header. */
   nbytes = tgaheader.idlength;
   
   /* Color maps can be present even when the image doesn't need it. */
   if (tgaheader.clrmaptype==1)
   {
      /* Get color map length. */
      nbytes += tgaheader.clrmaplen2<<8 | tgaheader.clrmaplen1;
   }
   else if (tgaheader.clrmaptype!=0)
   {
      goto Error;
   }

   /* Skip over image ID string and possible color map. */
   if (nbytes && fseek( tga, (long)nbytes, SEEK_CUR ))
     goto Error;

   /* Read in image data. */
   p = (char*)(pBmp->pbits);
   pp = p + pBmp->rowbytes * pBmp->yres;
   if ( rle )
   {
      if ( bitsperpel == 32)
      {
         while (p < pp)
         {
            if ( fread( b, 1, 5, tga ) < 5 )
               goto Error;
            i = b[0];
            n = i & 0x007f; /* n is now from 0 to 127. */
            if ( i & 0x80 )
            {
               n++;
               for ( i=0;i<n; i++ )
               {
                  *p++ = b[3]; /* R */ 
                  *p++ = b[2]; /* G */
                  *p++ = b[1]; /* B */
                  *p++ = b[4]; /* A */
               }               
            }
            else
            {
               *p++ = b[3]; /* R */ 
               *p++ = b[2]; /* G */
               *p++ = b[1]; /* B */
               *p++ = b[4]; /* A */
               for ( i=0;i<n; i++ )
               {
                  if ( fread( b, 1, 4, tga ) < 4 )
                    goto Error;
                  *p++ = b[2]; /* R */ 
                  *p++ = b[1]; /* G */
                  *p++ = b[0]; /* B */
                  *p++ = b[3]; /* A */
               }               
            }
         }
      } else if ( bitsperpel == 24)
      {
         while (p < pp)
         {
            if ( fread( b, 1, 4, tga ) < 4 )
              goto Error;
            i = b[0];
            n = i & 0x007f;
            if ( i & 0x0080 )
            {
               n++;
               for ( i=0; i<n; i++ )
               {
                  *p++ = b[3]; /* R */ 
                  *p++ = b[2]; /* G */
                  *p++ = b[1]; /* B */
               }               
            }
            else
            {
               *p++ = b[3]; /* R */ 
               *p++ = b[2]; /* G */
               *p++ = b[1]; /* B */
               for ( i=0; i<n; i++ )
               {
                  if ( fread( b, 1, 3, tga ) < 3 )
                    goto Error;
                  *p++ = b[2]; /* R */ 
                  *p++ = b[1]; /* G */
                  *p++ = b[0]; /* B */
               }               
            }
         }
      } else if ( bitsperpel == 16 )
      {
         while (p < pp)
         {
            if ( fread( b, 1, 3, tga ) < 3 )
              goto Error;
            i = b[0];
            n = i & 0x7f;
            if ( i & 0x80 )
            {
               n++;
               b[0] = (b[2]&0x7c)<<1;                     /* R */
               b[2] = ((b[2]&0x03)<<6)|((b[1]&0xe0)>>2);  /* G */
               b[1] = (b[1]&0x1f)<<3;                     /* B */
               for ( i=0;i<n; i++ )
               {
                  *p++ = b[0];  /* R */
                  *p++ = b[2];  /* G */
                  *p++ = b[1];  /* B */
               }               
            }
            else
            {
               *p++ = (b[2]&0x7c)<<1;                     /* R */
               *p++ = ((b[2]&0x03)<<6)|((b[1]&0xe0)>>2);  /* G */
               *p++ = (b[1]&0x1f)<<3;                     /* B */
               for ( i=0;i<n; i++ )
               {
                  if ( fread( b, 1, 2, tga ) < 2 )
                    goto Error;
                  *p++ = (b[1]&0x7c)<<1;                     /* R */
                  *p++ = ((b[1]&0x03)<<6)|((b[0]&0xe0)>>2);  /* G */
                  *p++ = (b[0]&0x1f)<<3;                     /* B */
               }               
            }
         }
      }
      else
      {
         goto Error;
      }
   }
   else /* not rle */
   {
      if ( bitsperpel == 32)
      {
         while (p < pp)
         {
            if ( fread( b, 1, 4, tga ) < 4 )
              goto Error;
            *p++ = b[2]; /* R */ 
            *p++ = b[1]; /* G */
            *p++ = b[0]; /* B */
            *p++ = b[3]; /* A */
         }
      } else if ( bitsperpel == 24)
      {
         while (p < pp)
         {
            if ( fread( b, 1, 3, tga ) < 3 )
              goto Error;
            *p++ = b[2]; /* R */
            *p++ = b[1]; /* G */
            *p++ = b[0]; /* B */
         }
      } else if ( bitsperpel == 16 )
      {
         /* For the 16 bit format each channel gets 5 bits.  
          *    A 5 bit value is turned into an 8 bit value
          *    by shifting everything up by 3 bits.
          *    The format for 16 bit is 
          * (junk or alpha bit)(5 Red bits)(5 green bits)(5 blue bits).
          */
         while (p < pp)
         {
            if ( fread( b, 1, 2, tga ) < 2 )
              goto Error;
            *p++ = (b[1]&0x7c)<<1;                     /* R */
            *p++ = ((b[1]&0x03)<<6)|((b[0]&0xe0)>>2);  /* G */
            *p++ = (b[0]&0x1f)<<3;                     /* B */
         }
      }
      else
      {
         goto Error;
      }
   }

   /* Most TGA writers have it so the data is organized bottom to
    *    to top and left to right.  So the above routines can be
    *    optimized to read in the data and store it based on the 
    *    format given by the (origin_alphabits & 0x30) bits. 
    * But to keep it simple, just flip the image when needed. 
    *    Probably should have made it store the data originally as
    *    bottom-to-top as this seems the most popular with software
    *    writing out tga files. (TO-DO item)
    *
    * 5-23-98:  Have the caller of ReadTga() also call OrientBitmap()
    *           if they want a particular orientation.  This is because
    *           such programs as showbits() need the bitmap to be
    *           bottom-to-top and if the bitmap was originally that
    *           orientation, it's a waste to have the reading routine
    *           flip the image when the caller has to flip it back.
    */
   if ( !(tgaheader.origin_alphabits & kTGA_TOP_TO_BOTTOM) )
   {
      pBmp->orientation = BITMAP_BOTLEFT;
   }
   /* Else the default of BITMAP_TOPLEFT is used. */

   if ( tgaheader.origin_alphabits & kTGA_RIGHT_TO_LEFT )
   {
      /* If BITMAP_TOPLEFT, then BITMAP_TOPRIGHT.
       * If BITMAP_BOTLEFT, then BITMAP_BOTRIGHT. 
       */
      if (pBmp->orientation==BITMAP_TOPLEFT)
	 pBmp->orientation=BITMAP_TOPRIGHT;
      else
	 pBmp->orientation=BITMAP_BOTRIGHT;
   }
   /* Else the default BITMAP_TOPLEFT or BITMAP_BOTLEFT is used. */

   fclose( tga );
   
   return pBmp;
   
 Error:

   if (pBmp)
     free( pBmp );
   /* The FILE pointer tga will not be NULL if the code reaches here. */
   fclose( tga );
   return NULL;
}
