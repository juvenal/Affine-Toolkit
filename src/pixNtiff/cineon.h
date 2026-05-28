/* 
 * Copyright (c) 1999 Thomas E. Burge.  All rights reserved.  
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
 *       Copyright (c) 1995, 1996, 1997, 1998, 1999 Thomas E. Burge.  
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
 * FILE:  cineon.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          Structures used for Kodak Cineon file format.
 *
 *    References:
 *          [CINE99]  Version 4.5 , November 30, 1990 4.5 Draft - Image 
 *                    File Format Proposal for Digital Pictures, Kodak, 
 *                    November 30, 1990 draft from www.cineon.com.
 *          [MURR96]  Murray, James D. and VanRyper, William, Graphics
 *                    File Formats: Second Edition, O'Reilly & Associates, 
 *                    Inc., 1996, pp. 350-367.
 *
 *          Note:  [MURR96] does not provide much of any information 
 *                 regarding the Kodak Cineon standard.  I found the 
 *                 statements about DPX being a super set of the Kodak
 *                 Cineon format to be incorrect as the generic file
 *                 header is not the one used by the .cin files created
 *                 with Alias or PRMan.
 * 
 */
#ifndef __CINEON_HEADER__
#define __CINEON_HEADER__

/* Get BITMAP_ orientation flags. */
#include "bitmap.h"

/* Some standard header files already define TRUE and FALSE. */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


typedef struct _CINEON_FILEHEADER {
   unsigned int   magicnumber;
   unsigned int   imageoffset;
   unsigned int   genericlength;
   unsigned int   industrylength;
   unsigned int   variablelength;
   unsigned int   filesize;
   char           version[8];
   char           filename[100];
   char           creationdate[12]; /* yyyy:mm:dd */
   char           creationtime[12]; /* hh:mm:ssxxx, xxx has been PDT */
   char           reserved[36];
} CINEON_FILEHEADER;
typedef CINEON_FILEHEADER *PCINEON_FILEHEADER;


enum {
   CINEON_LEFTTOP,  /* Left is row 0.    Top is column 0.    */
   CINEON_LEFTBOT,  /* Left is row 0.    Bottom is column 0. */
   CINEON_RIGHTTOP, /* Right is row 0.   Top is column 0.    */
   CINEON_RIGHTBOT, /* Right is row 0.   Bottom is column 0. */
   CINEON_TOPLEFT,  /* Top is row 0.     Left is column 0.   */
   CINEON_TOPRIGHT, /* Top is row 0.     Right is column 0.  */
   CINEON_BOTLEFT,  /* Bottom is row 0.  Left is column 0.   */
   CINEON_BOTRIGHT  /* Bottom is row 0.  Right is column 0.  */
};

/* To convert orientation with orient.c code use these CIN2BMP_ 
 *    orientation values to create a conversion table to the 
 *    bitmap.h's BITMAP_ orientation values.
 */
enum {
   CIN2BMP_LEFTTOP  = BITMAP_LEFTTOP,
   CIN2BMP_LEFTBOT  = BITMAP_LEFTBOT,
   CIN2BMP_RIGHTTOP = BITMAP_RIGHTTOP,
   CIN2BMP_RIGHTBOT = BITMAP_RIGHTBOT,
   CIN2BMP_TOPLEFT  = BITMAP_TOPLEFT, 
   CIN2BMP_TOPRIGHT = BITMAP_TOPRIGHT,
   CIN2BMP_BOTLEFT  = BITMAP_BOTLEFT, 
   CIN2BMP_BOTRIGHT = BITMAP_BOTRIGHT 
};


/* Using the term "image description" (DESC) rather than "image information"
 * as the word information may imply that the actualy image data is present.
 */
typedef struct _CINEON_IMAGEDESC {
   unsigned char  orientation;
   unsigned char  nchannels;
   unsigned char  unused1[2];
   unsigned char  chan1;
   unsigned char  chan2;
   unsigned char  bitsperpixel;
   unsigned char  unused2;
   unsigned int   width;
   unsigned int   height;
   float          min1datavalue;
   float          min1quantity;
   float          max1datavalue; /* Docs seem to be wrong with R32 vs. 32 */
   float          max1quantity;
   char           a[28];
   char           b[28];
   char           c[28];
   char           d[28];
   char           e[28];
   char           f[28];
   char           g[28];
   float          white[2];
   float          red[2];
   float          green[2];
   float          blue[2];
   char           label[200];
   char           reserved[28];
} CINEON_IMAGEDESC;
typedef CINEON_IMAGEDESC *PCINEON_IMAGEDESC;

typedef struct _CINEON_DATAFORMAT {
   unsigned char  dateinterleave;
   unsigned char  packing;
   unsigned char  signeddata;
   unsigned char  imagesense;
   unsigned int   eolpadding; /* End Of Line Padding */
   unsigned int   eocpadding; /* End Of Channel Padding */
   char           reserved[20];
} CINEON_DATAFORMAT;
typedef CINEON_DATAFORMAT *PCINEON_DATAFORMAT;

typedef struct _CINEON_CREATIONDESC {
   signed int     xoffset;
   signed int     yoffset;
   char           filename[100];
   char           creationdate[12]; /* yyyy:mm:dd */
   char           creationtime[12]; /* hh:mm:ssxxx, xxx has been PDT */
   char           inputdevice[64];
   char           devicemodelnumber[32];
   char           deviceserialnumber[32];
   float          xdevicepitch;
   float          ydevicepitch;
   float          gamma;
   char           reserved[40];
} CINEON_CREATIONDESC;
typedef CINEON_CREATIONDESC *PCINEON_CREATIONDESC;

/* Not very descriptive: */
typedef struct _CINEON_FILMDESC {
   unsigned char  a;
   unsigned char  b;
   unsigned char  c;
   unsigned char  d;
   unsigned int   e;
   unsigned int   f;
   char           g[32];
   unsigned int   h;
   float          i;
   char           j[32];
   char           k[2000];
   char           reserved[740];
} CINEON_FILMDESC;
typedef CINEON_FILMDESC *PCINEON_FILMDESC;


#ifdef CINEON_PRIV
#endif 

#endif 

