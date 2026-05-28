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
 * FILE:  tga.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *    References:
 * 
 */
#ifndef __TGA__HEADER__
#define __TGA__HEADER__

#define NO_ENCODING   0
#define ENCODING      1

typedef struct _TGAHEADER {
   char  idlength;
   char  clrmaptype;
   char  imagetype;
   char  clrmapoffset1;
   char  clrmapoffset2;
   char  clrmaplen1;
   char  clrmaplen2;
   char  clrmapdepth;
   char  xorigin1;
   char  xorigin2;
   char  yorigin1;
   char  yorigin2;
   char  width1;
   char  width2;
   char  height1;
   char  height2;
   char  pixeldepth;
   char  origin_alphabits;
} TGAHEADER;
typedef TGAHEADER *PTGAHEADER;

/* Masks to AND with origin_alphabits. */
#define kTGA_RIGHT_TO_LEFT  0x10
#define kTGA_TOP_TO_BOTTOM  0x20

#endif


