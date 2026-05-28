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
 * FILE:  rpix.c
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          ReadPix() - Read Alias Research pix bitmap file.
 *
 *    References:
 *          [ALIA94]   Alias Research Inc., Alias Reference Guide Vol. 2, 
 *                     110 Richmond St. East, Toronto, Ontario, 
 *                     Canada, M5C 1P1, 1994, pp. 1489-1492.
 *          [ALIA95a]  Alias Research, Alias Sketch: Reference Guide,
 *                     110 Richmond St. East, Toronto, Ontario, 
 *                     Canada, M5C 1P1, 1995, p. 19.
 *          [ALIA95b]  Alias|Wavefront, Alias Menu Book V7.0 (PowerAnimator), 
 *                     110 Richmond St. East, Toronto, Ontario, 
 *                     Canada, M5C 1P1, 1995, pp. 901-903.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "rpix.h"

PBITMAP ReadPix( char *filename )
{
   FILE           *pix;
   PBITMAP        pBmp; 
   unsigned int   xres, yres, nsamples;
   int            bitsperpel;
   unsigned char  b[5];
   register char  *p;
   register unsigned int  count,i,n,nbytes;


   pix = fopen( filename, "rb" );
   if (!pix) 
      return NULL;

   /* Read xres. */
   if ( fread( b, 1, 2, pix ) < 2 )
     goto Error;
   xres = (b[0]<<8) | b[1];

   /* Read yres. */
   if ( fread( b, 1, 2, pix ) < 2 )
     goto Error;
   yres = (b[0]<<8) | b[1];

   /* Read through unused fields. 
    * These fields are supposed to be unused, but the yoffset always seems
    *    to be set to yres-1. 
    */
   if ( fread( b, 1, 4, pix ) < 4 )
     goto Error;

   /* Read bits per pixel. */
   if ( fread( b, 1, 2, pix ) < 2 )
     goto Error;
   bitsperpel = (b[0]<<8) | b[1];

   switch ( bitsperpel ) {
    case 8:      
      nsamples = 1;
      break;
    case 24:
      nsamples = 3;
      break;
    case 32:
      nsamples = 4;
      break;
    default:
      goto Error;
   }
      
   pBmp = CreateBitmap( xres, yres, 0, 0, nsamples, BITMAP_UINT8, 8 );
   if (!pBmp)
     goto Error;

   p = (char*)(pBmp->pbits);
   nbytes = pBmp->rowbytes * pBmp->yres;
   count=0;
   if ( bitsperpel == 32)
   {
      while (count < nbytes)
      {
         if ( fread( b, 1, 5, pix ) < 5 )
           goto Error;
         n = b[0];
         count += 4*n;
         if ( count > nbytes )
            goto Error;
         for ( i=0;i<n; i++ )
         {
            *p++ = b[3]; /* r */
            *p++ = b[2]; /* g */
            *p++ = b[1]; /* b */
            *p++ = b[4]; /* a */
         }
      }
   } else if ( bitsperpel == 24)
   {      
      while (count < nbytes)
      {
         if ( fread( b, 1, 4, pix ) < 4 )
           goto Error;
         n = b[0];
         count += 3*n;
         if ( count > nbytes )
            goto Error;
         for ( i=0;i<n; i++ )
         {
            *p++ = b[3]; /* r */
            *p++ = b[2]; /* g */
            *p++ = b[1]; /* b */
         }
      }
   } else if ( bitsperpel == 8 )
   {
      while (count < nbytes)
      {
         if ( fread( b, 1, 2, pix ) < 2 )
           goto Error;
         n = b[0];
         count += n;
         if ( count > nbytes )
            goto Error;
         for ( i=0;i<n; i++ )
         {
            *p++ = b[1]; /* a */
         }
      }
   }
   else
   {
      goto Error;
   }
   
   fclose( pix );

   return pBmp;

 Error:
   free( pBmp );
   fclose( pix );
   return NULL;
}

