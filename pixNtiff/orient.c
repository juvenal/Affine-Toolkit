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
 * FILE:  orient.c
 *
 * DESCRIPTION:
 *
 *    Contains:
 * 
 *    To-Do:
 * 
 *    References:
 *        
 */
#define __ORIENT_FILE__
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

static int HorizontalFlipImage( PBITMAP pBmp );
static int VerticalFlipImage( PBITMAP pBmp, int bitsperpixel );
static PBITMAP DiagonalFlipImage( PBITMAP pBmp, int bitsperpixel );


/* Orientation Flags defined in bitmap.h:
 *
 *   0 BITMAP_TOPLEFT,  -- Top is row 0.     Left is column 0.
 *   1 BITMAP_TOPRIGHT, -- Top is row 0.     Right is column 0.
 *   2 BITMAP_BOTRIGHT, -- Bottom is row 0.  Right is column 0.
 *   3 BITMAP_BOTLEFT,  -- Bottom is row 0.  Left is column 0.
 *   4 BITMAP_LEFTTOP,  -- Left is row 0.    Top is column 0.
 *   5 BITMAP_RIGHTTOP, -- Right is row 0.   Top is column 0.
 *   6 BITMAP_RIGHTBOT, -- Right is row 0.   Bottom is column 0.
 *   7 BITMAP_LEFTBOT,  -- Left is row 0.    Bottom is column 0.
 */

/* Diagonal flip */
#define D 0x01
/* Vertical flip */
#define V 0x02
/* Horizontal flip */
#define H 0x04
/* Combinations */
#define HV   (H|V)
#define DH   (D|H)
#define DV   (D|V)
#define DHV  (D|H|V)

/* Illustration of where the zero rows and columns are located:
 *
 *  |-r--   -r--|       |   |
 *  c 0      1  c    2  c   c 3
 *  |           |   -r--|   |-r--
 *
 *  |-c--   -c--|       |   |
 *  r 4      5  r    6  r   r 7
 *  |           |   -c--|   |-c--
 *
 */

static char fromto[8][8] = {
/*        0     1     2    3    4    5    6    7  */

/*0*/     0,    V,   HV,   H,   D,  DV, DHV,  DH,

/*1*/     V,    0,    H,  HV,  DV,   D,  DH, DHV,

/*2*/    HV,    H,    0,   V, DHV,  DH,   D,  DV,

/*3*/     H,   HV,    V,   0,  DH, DHV,  DV,   D,

/*4*/     D,   DV,  DHV,  DH,   0,   V,  HV,   H,

/*5*/    DV,    D,   DH, DHV,   V,   0,   H,  HV,

/*6*/   DHV,   DH,    D,  DV,  HV,   H,   0,   V,

/*7*/    DH,  DHV,   DV,   D,   H,  HV,   V,   0
};


PBITMAP OrientBitmap( PBITMAP pBmp, int orientation )
{
   int  flags = 0;
   int  pixelsize;


   if (pBmp->orientation>7)
   {
      printf( "ERROR:  Bad orientation data in bitmap structure.\n" );
      return pBmp;
   }

   /* Get the operation flags set. */
   flags = fromto[pBmp->orientation][orientation];

   /* Get the number of bits per pixel. */
   pixelsize = pBmp->bitspersample * pBmp->nsamples;

   /* Reject unsupported formats. */
   if ( pBmp->sampleformat == BITMAP_MONO )
      return pBmp; 

   if ( pBmp->sampleformat == BITMAP_UINT8 && pBmp->nsamples>4 )
      return pBmp; 

   if ( pBmp->sampleformat == BITMAP_UINT16 && pBmp->nsamples>4 )
      return pBmp; 

   if ( pBmp->sampleformat == BITMAP_IEEE && pBmp->nsamples!=1 )
      return pBmp; 

   /* Based on the flags, orient the image. */
   if ( flags & D )
      pBmp = DiagonalFlipImage(pBmp,pixelsize);   

   if ( flags & H )
      HorizontalFlipImage(pBmp);

   if ( flags & V )
      VerticalFlipImage(pBmp,pixelsize);

   pBmp->orientation = orientation;

   return pBmp;
}


/* 
 *  Flip image around center horizontal line. 
 */
static int HorizontalFlipImage( PBITMAP pBmp )
{
   unsigned int           end;
   register char          *p,*pp;
   register unsigned int  count,i,n,nbytes;


   /* See if there is more than one scanline. */
   if (pBmp->yres<=1)
      return 0;

   /* Flip image so it's not upside down. */
   nbytes = pBmp->rowbytes;
   end = nbytes*(pBmp->yres-1);

   i=pBmp->yres >> 1;
   do
   {
      i--;
      n = nbytes*i;
      p = (char*)(pBmp->pbits) + n;
      pp = (char*)(pBmp->pbits) + (end - n);
      for ( count = nbytes; count; count-- )
      {
         n = *p;
         *p++ = *pp;
         *pp++ = n;
      }
   } while (i);

   return 0;
}


/* 
 *  Flip image around vertical line. 
 */
static int VerticalFlipImage( PBITMAP pBmp, int bitsperpixel )
{
   register char          *p,*pp;
   register unsigned int  *u,*uu;
   register unsigned int  count,i,n,nbytes;


   /* See if there is more than one column. */
   if (pBmp->xres<=1)
      return 0;

   nbytes = pBmp->rowbytes;

   switch (bitsperpixel)
   {
    case 8:
      for ( i=0; i < pBmp->yres; i++ )
      {
	 p = (char*)(pBmp->pbits) + nbytes*i;
	 n = pBmp->xres;
	 pp = p + (n-1);
	 for ( count = (n >> 1); count; count-- )
	 {
	    n     = p[0];
	    p[0]  = pp[0];
	    pp[0] = n;
	    p++;
	    pp--;
	 }
      }
      break;
    case 16:
      for ( i=0; i < pBmp->yres; i++ )
      {
	 p = (char*)(pBmp->pbits) + nbytes*i;
	 n = pBmp->xres;
	 pp = p + (n-1)*2;
	 for ( count = (n >> 1); count; count-- )
	 {
	    n     = p[0];
	    p[0]  = pp[0];
	    pp[0] = n;
	    n     = p[1];
	    p[1]  = pp[1];
	    pp[1] = n;
	    p+=2;
	    pp-=2;
	 }
      }
      break;
    case 24:
      for ( i=0; i < pBmp->yres; i++ )
      {
	 p = (char*)(pBmp->pbits) + nbytes*i;
	 n = pBmp->xres;
	 pp = p + (n-1)*3;
	 for ( count = (n >> 1); count; count-- )
	 {
	    n     = p[0];
	    p[0]  = pp[0];
	    pp[0] = n;
	    n     = p[1];
	    p[1]  = pp[1];
	    pp[1] = n;
	    n     = p[2];
	    p[2]  = pp[2];
	    pp[2] = n;
	    p+=3;
	    pp-=3;
	 }
      }
      break;
    case 32:
      for ( i=0; i < pBmp->yres; i++ )
      {
	 p = (char*)(pBmp->pbits) + nbytes*i;
	 n = pBmp->xres;
	 pp = p + (n-1)*4;
	 
	 u = (unsigned int*)p;
	 uu = (unsigned int*)pp;
	 for ( count = (n >> 1); count; count-- )
	 {
	    n   = *u;
	    *u  = *uu;
	    *uu = n;
	    u++;
	    uu--;
	 }
      }
      break;
    case 48:
      for ( i=0; i < pBmp->yres; i++ )
      {
	 p = (char*)(pBmp->pbits) + nbytes*i;
	 n = pBmp->xres;
	 pp = p + (n-1)*6;
	 for ( count = (n >> 1); count; count-- )
	 {
	    n     = p[0];
	    p[0]  = pp[0];
	    pp[0] = n;
	    n     = p[1];
	    p[1]  = pp[1];
	    pp[1] = n;
	    n     = p[2];
	    p[2]  = pp[2];
	    pp[2] = n;
	    n     = p[3];
	    p[3]  = pp[3];
	    pp[3] = n;
	    n     = p[4];
	    p[4]  = pp[4];
	    pp[4] = n;
	    n     = p[5];
	    p[5]  = pp[5];
	    pp[5] = n;
	    p+=6;
	    pp-=6;
	 }
      }
      break;
    case 64:
      for ( i=0; i < pBmp->yres; i++ )
      {
	 p = (char*)(pBmp->pbits) + nbytes*i;
	 n = pBmp->xres;
	 pp = p + (n-1)*4;
	 
	 u = (unsigned int*)p;
	 uu = (unsigned int*)pp;
	 for ( count = (n >> 1); count; count-- )
	 {
	    n     = u[0];
	    u[0]  = uu[0];
	    uu[0] = n;
	    n     = u[1];
	    u[1]  = uu[1];
	    uu[1] = n;
	    u+=2;
	    uu-=2;
	 }
      }
      break;
    default:
      return 1; /* Unexpected number of samples per pixel. */
   }

   return 0;
}


/* 
 *  Flip image around diagonal line from upper left to lower right. 
 */
static PBITMAP DiagonalFlipImage( PBITMAP pBmp, int bitsperpixel )
{
   PBITMAP            target = NULL;
   unsigned int       nbytes;
   auto unsigned int  count,i,n,tmp;
   auto char          *p,*pp;


   /* If a square bitmap, then just move the pixels around. */
   if ( pBmp->yres == pBmp->xres )
   {
      /* See if there is more than one row and column. */
      if (pBmp->yres<=1) /* Note xres=yres, so just check one of them. */
	 return pBmp;

      /* Swap xpos, ypos values. */
      tmp = pBmp->xpos;
      pBmp->xpos = pBmp->ypos;
      pBmp->ypos = tmp;

      n = pBmp->xres-1;
      nbytes = pBmp->rowbytes;

      switch (bitsperpixel)
      {
       case 8:
	 for ( i=0; i<n; i++ )
	 {
	    p  = (char*)(pBmp->pbits) + i + i*nbytes;
	    pp = p + nbytes;
	    p++;
	    for ( count = n-i; count; count-- )
	    {
	       tmp   = p[0];
	       p[0]  = pp[0];
	       pp[0] = tmp;
	       p++;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 16:
	 for ( i=0; i<n; i++ )
	 {
	    p  = (char*)(pBmp->pbits) + 2*i + i*nbytes;
	    pp = p + nbytes;
	    p+=2;
	    for ( count = n-i; count; count-- )
	    {
	       tmp   = p[0];
	       p[0]  = pp[0];
	       pp[0] = tmp;
	       tmp   = p[1];
	       p[1]  = pp[1];
	       pp[1] = tmp;
	       p+=2;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 24:
	 for ( i=0; i<n; i++ )
	 {
	    p  = (char*)(pBmp->pbits) + 3*i + i*nbytes;
	    pp = p + nbytes;
	    p+=3;
	    for ( count = n-i; count; count-- )
	    {
	       tmp   = p[0];
	       p[0]  = pp[0];
	       pp[0] = tmp;
	       tmp   = p[1];
	       p[1]  = pp[1];
	       pp[1] = tmp;
	       tmp   = p[2];
	       p[2]  = pp[2];
	       pp[2] = tmp;
	       p+=3;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 32:
	 for ( i=0; i<n; i++ )
	 {
	    p  = (char*)(pBmp->pbits) + 4*i + i*nbytes;
	    pp = p + nbytes;
	    p+=4;
	    for ( count = n-i; count; count-- )
	    {
	       tmp   = p[0]; p[0]  = pp[0]; pp[0] = tmp;
	       tmp   = p[1]; p[1]  = pp[1]; pp[1] = tmp;
	       tmp   = p[2]; p[2]  = pp[2]; pp[2] = tmp;
	       tmp   = p[3]; p[3]  = pp[3]; pp[3] = tmp;
	       p+=4;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 48:
	 for ( i=0; i<n; i++ )
	 {
	    p  = (char*)(pBmp->pbits) + 6*i + i*nbytes;
	    pp = p + nbytes;
	    p+=6;
	    for ( count = n-i; count; count-- )
	    {
	       tmp   = p[0]; p[0]  = pp[0]; pp[0] = tmp;
	       tmp   = p[1]; p[1]  = pp[1]; pp[1] = tmp;
	       tmp   = p[2]; p[2]  = pp[2]; pp[2] = tmp;
	       tmp   = p[3]; p[3]  = pp[3]; pp[3] = tmp;
	       tmp   = p[4]; p[4]  = pp[4]; pp[4] = tmp;
	       tmp   = p[5]; p[5]  = pp[5]; pp[5] = tmp;
	       p+=6;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 64:
	 for ( i=0; i<n; i++ )
	 {
	    p  = (char*)(pBmp->pbits) + 8*i + i*nbytes;
	    pp = p + nbytes;
	    p+=8;
	    for ( count = n-i; count; count-- )
	    {
	       tmp   = p[0]; p[0]  = pp[0]; pp[0] = tmp;
	       tmp   = p[1]; p[1]  = pp[1]; pp[1] = tmp;
	       tmp   = p[2]; p[2]  = pp[2]; pp[2] = tmp;
	       tmp   = p[3]; p[3]  = pp[3]; pp[3] = tmp;
	       tmp   = p[4]; p[4]  = pp[4]; pp[4] = tmp;
	       tmp   = p[5]; p[5]  = pp[5]; pp[5] = tmp;
	       tmp   = p[6]; p[6]  = pp[6]; pp[6] = tmp;
	       tmp   = p[7]; p[7]  = pp[7]; pp[7] = tmp;
	       p+=8;
	       pp+=nbytes;
	    }
	 }
	 break;
       default:
	 /* Unexpected pixel size, just return bitmap untouched. */
	 return pBmp; 
      }
   }
   else
   {
      target = CreateBitmap( pBmp->yres, pBmp->xres, /* Swap x and y. */
			     pBmp->ypos, pBmp->xpos, /* Swap x and y. */
			     pBmp->nsamples,
			     pBmp->sampleformat,
			     pBmp->bitspersample );
      if (!target)
	 return pBmp;

      p  = (char*)(pBmp->pbits);

      nbytes = target->rowbytes;

      switch (bitsperpixel)
      {
       case 8:
	 for ( i=0; i < pBmp->yres; i++ )
	 {
	    pp = (char*)(target->pbits) + i;

	    for ( count = pBmp->xres; count; count-- )
	    {
	       *pp = *p;
	       p++;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 16:
	 for ( i=0; i < pBmp->yres; i++ )
	 {
	    pp = (char*)(target->pbits) + 2*i;

	    for ( count = pBmp->xres; count; count-- )
	    {
	       pp[0] = p[0];
	       pp[1] = p[1];
	       p+=2;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 24:
	 for ( i=0; i < pBmp->yres; i++ )
	 {
	    pp = (char*)(target->pbits) + 3*i;

	    for ( count = pBmp->xres; count; count-- )
	    {
	       pp[0] = p[0];
	       pp[1] = p[1];
	       pp[2] = p[2];
	       p+=3;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 32:
	 for ( i=0; i < pBmp->yres; i++ )
	 {
	    pp = (char*)(target->pbits) + 4*i;

	    for ( count = pBmp->xres; count; count-- )
	    {
	       pp[0] = p[0];
	       pp[1] = p[1];
	       pp[2] = p[2];
	       pp[3] = p[3];
	       p+=4;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 48:
	 for ( i=0; i < pBmp->yres; i++ )
	 {
	    pp = (char*)(target->pbits) + 6*i;

	    for ( count = pBmp->xres; count; count-- )
	    {
	       pp[0] = p[0];
	       pp[1] = p[1];
	       pp[2] = p[2];
	       pp[3] = p[3];
	       pp[4] = p[4];
	       pp[5] = p[5];
	       p+=6;
	       pp+=nbytes;
	    }
	 }
	 break;
       case 64:
	 for ( i=0; i < pBmp->yres; i++ )
	 {
	    pp = (char*)(target->pbits) + 8*i;

	    for ( count = pBmp->xres; count; count-- )
	    {
	       pp[0] = p[0];
	       pp[1] = p[1];
	       pp[2] = p[2];
	       pp[3] = p[3];
	       pp[4] = p[4];
	       pp[5] = p[5];
	       pp[6] = p[6];
	       pp[7] = p[7];
	       p+=8;
	       pp+=nbytes;
	    }
	 }
	 break;
       default:
	 return pBmp; /* Unexpected number of samples per pixel. */
      }
   }

   if (target)
   {
      DestroyBitmap( pBmp );
      return target;
   }

   return pBmp;
}
