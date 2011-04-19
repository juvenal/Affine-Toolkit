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
 * FILE:  bitmap.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          Structures used to create a bitmap area.
 *
 *    References:
 * 
 */
#ifndef __BITMAP_HEADER__
#define __BITMAP_HEADER__

#define BITMAP_MONO    1
#define BITMAP_UINT8   2
#define BITMAP_UINT16  3
#define BITMAP_IEEE    4

/* Note that these are the TIFF orientation values minus one. */
enum {
   BITMAP_TOPLEFT,  /* Top is row 0.     Left is column 0.   */
   BITMAP_TOPRIGHT, /* Top is row 0.     Right is column 0.  */
   BITMAP_BOTRIGHT, /* Bottom is row 0.  Right is column 0.  */
   BITMAP_BOTLEFT,  /* Bottom is row 0.  Left is column 0.   */
   BITMAP_LEFTTOP,  /* Left is row 0.    Top is column 0.    */
   BITMAP_RIGHTTOP, /* Right is row 0.   Top is column 0.    */
   BITMAP_RIGHTBOT, /* Right is row 0.   Bottom is column 0. */
   BITMAP_LEFTBOT   /* Left is row 0.    Bottom is column 0. */
};

typedef unsigned char  *PBITS;

typedef struct _BITMAP {
   unsigned int  xres;
   unsigned int  yres;
   unsigned int  xpos;
   unsigned int  ypos;
   unsigned int  sampleformat;
   unsigned int  orientation;
   unsigned int  nsamples;
   unsigned int  bitspersample;
   unsigned int  rowbytes;  /* for an RGB image rowbytes = 3*xres */
                     /* rowbytes is needed incase padding is needed */
   PBITS  pbits;
} BITMAP;
typedef BITMAP *PBITMAP;

#ifndef __BITMAP_FILE__
extern
#endif
PBITMAP CreateBitmap( unsigned int  xres,
		      unsigned int  yres,
		      unsigned int  xpos,
		      unsigned int  ypos,
		      unsigned int  nsamples,
		      unsigned int  sampleformat,
		      unsigned int  bitspersample );

#ifndef __ORIENT_FILE__
extern
#endif
PBITMAP OrientBitmap( PBITMAP pBmp, int orientation );

#ifndef __BITMAP_FILE__
extern
#endif
int DestroyBitmap( PBITMAP pBmp );

#ifdef BITMAP_PRIV
#endif 

#endif 

