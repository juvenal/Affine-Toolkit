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
 * FILE:  rz.c
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          ReadZ() - Read Depth File.
 *
 *    References:
 *           When using the "zfile" driver to output a Z-file of 300x300,
 *           I noticed that the size was about right for uncompressed IEEE
 *           floating point data.  Checking 300 * 300 * 4 equals 360000 
 *           and the Z file size was 360136 it looked like the file format 
 *           could be just a header and the raw floating point data.  
 *           
 *           It appears that the header has some 32bit number, xres and 
 *           yres followed by two transformation matrices and the IEEE 
 *           floats in big-endian form.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "rz.h"


PBITMAP ReadZ( char *filename )
{
   static unsigned char    b[4];
   FILE                    *z;
   PBITMAP                 pBmp = NULL;
   unsigned int            xres, yres;
   register unsigned char  *p,*pp;


   z = fopen( filename, "rb" );
   if (!z) 
      return NULL;

   /* Read through first four bytes. */
   if ( fread( b, 1, 4, z ) < 4 )
     goto Error;

   /* Read xres. */
   if ( fread( b, 1, 2, z ) < 2 )
     goto Error;
   xres = (b[0]<<8) | b[1];

   /* Read yres. */
   if ( fread( b, 1, 2, z ) < 2 )
     goto Error;
   yres = (b[0]<<8) | b[1];

   /* Read through transformations. */
   if ( fseek( z, 128, SEEK_CUR ) )
     goto Error;

   pBmp = CreateBitmap( xres, yres, 0, 0, 1, BITMAP_IEEE, 32 );
   if (!pBmp)
     goto Error;

   pp = p = pBmp->pbits;
   pp += pBmp->rowbytes * pBmp->yres;
   while (p < pp)
   {
#ifdef LITTLE_ENDIAN
      if ( fread( b, 1, 4, z ) < 4 )
        goto Error;
      *p++ = b[3];
      *p++ = b[2];
      *p++ = b[1];
      *p++ = b[0];
#else
      if ( fread( p, 1, 4, z ) < 4 )
        goto Error;
      p += 4;
#endif
   }
   
   fclose( z );

   return pBmp;

 Error:
   free( pBmp );
   fclose( z );
   return NULL;
}

