/* 
 * Copyright (c) 1995, 1996, 1997, 1998 Thomas E. Burge.  All rights reserved.
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
 * FILE:  write.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          Structures used for the Simple RIB Writer library (client library).
 *
 *    References:
 *          [PIXA89]  Pixar Animation Studios, The RenderMan Interface, 
 *                    Version 3.1, Richmond, CA, September 1989.
 *
 *    History:
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 */
#ifndef __WRITE_HEADER__
#define __WRITE_HEADER__

#define HASHMOD 211
typedef struct _HASHATOM 
{
   char              *name;
   int               classNtype;
   int               n;
   struct _HASHATOM  *next;
} HASHATOM;

typedef struct _UVSTEPS 
{
   RtInt            nustep;
   RtInt            nvstep;
   RtInt            blocklevel;
   struct _UVSTEPS  *next;
} UVSTEPS;

typedef struct _PRIVATESTATEDATA 
{
   RtBoolean     hashtablecleared;
   unsigned int  LastObjectHandle;
   unsigned int  LastLightHandle;
   RtInt         nustep;
   RtInt         nvstep;
   UVSTEPS       *stepstack;
   RtInt         blocklevel;
   RtInt         ncolor;
   RtInt         firstline;
   RtBoolean     prman36changes;
} PRIVATESTATEDATA;

#endif
