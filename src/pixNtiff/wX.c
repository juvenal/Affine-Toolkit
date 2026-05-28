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
 * FILE:  wX.c
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          WriteX() - Write a X window text format bitmap file.
 *
 *    References:
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "bitmap.h"
#include "wX.h"


int WriteX( PBITMAP pBmp, char *filename )
{
   static char  b[30];
   int  i;
   FILE     *X;
   register int   x,y;
   register unsigned char  *p,c;

   /* Only allow pixel depth of one bit. */
   if (pBmp->sampleformat!=BITMAP_MONO 
       || pBmp->nsamples != 1 || pBmp->bitspersample!=1 )
     return 1;

   i = 0;
   while ( isalpha(filename[i]) )
     i++;

   strncpy( b, filename, (i>(sizeof(b)-1) ? sizeof(b)-1 : i) );

   X = fopen( filename, "w" );
   if (!X) 
     return 1;

   /* Write xres. */
   if (fprintf( X, "#define %s_width %u\n",b,pBmp->xres ) < 0 )
     goto Error;

   /* Write yres. */
   if (fprintf( X, "#define %s_height %u\n",b,pBmp->yres ) < 0 )
     goto Error;

   /* Write bits. */
   if (fprintf( X, "static unsigned char %s_bits[] = {",b ) < 0 )
     goto Error;

   p = pBmp->pbits;
   y = 0;
   for ( y=1; y<=pBmp->yres; y++ )
   {
      for ( x=1; x<=pBmp->rowbytes; x++ )
      {
         c = *p++;

         c = (c&0x80)>>7 | (c&0x40)>>5 | (c&0x20)>>3 | (c&0x10)>>1 |
           (c&0x08)<<1 | (c&0x04)<<3 | (c&0x02)<<5 | (c&0x01)<<7;
         
         if ( x%12 == 1 )
           if (fprintf( X, "\n   " )<0)
             goto Error;

         if (fprintf( X, ( y==pBmp->yres && x==pBmp->rowbytes 
                      ? "0x%02x " : "0x%02x, "), c) < 0)
           goto Error;
      }
   }
   if (fprintf( X, "};\n" ) < 0)
     goto Error;         

   fclose( X );

   return 0;

 Error:
   fclose( X );
   return 1;
}
