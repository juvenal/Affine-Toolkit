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
 * FILE:  ribattrstore.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          Structures used for attributes set while reading a RIB.
 *
 *    References:
 *          [PIXA89]  Pixar Animation Studios, The RenderMan Interface, 
 *                    Version 3.1, Richmond, CA, September 1989.
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#ifndef __ATTRSTORE_HEADER__
#define __ATTRSTORE_HEADER__

#define  ATTR_DONTFREEDATA    0x00000001

enum ATTRSTORECALLS { 
   ATTR_ATTRIBUTE,
   ATTR_ATTRIBUTEBEGIN,
   ATTR_ATTRIBUTEEND,
   ATTR_BEGIN,
   ATTR_END,
   ATTR_FRAMEBEGIN,
   ATTR_FRAMEEND,
   ATTR_TRIMCURVE,
   ATTR_WORLDBEGIN,
   ATTR_WORLDEND,
   ATTR_LAST
};


typedef struct _ATTRITEM {
   int               level;
   char              *name;
   char              *property;
   void              *setting;
   struct _ATTRITEM  *prev;
   struct _ATTRITEM  *next;
} ATTRITEM;
typedef ATTRITEM  *PATTRITEM;


typedef struct _ATTR {
   RIB_UINT32     flags;
   int            presentlevel;
   PATTRITEM      first;
   PATTRITEM      last;
   PRIB_RIPROC    ricalls[ATTR_LAST];
} ATTR;
typedef ATTR  *PATTR;


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#ifdef __RIB_ATTRSTORE_FILE
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN RtVoid AttrAttributeBegin( void );
EXTERN RtVoid AttrAttributeEnd( void );
EXTERN RtVoid AttrAttributeV( RtToken name,
                             RtInt n, RtToken tokens[], RtPointer parms[] );
EXTERN RtVoid AttrBegin( RtToken name );
EXTERN RtVoid AttrEnd( void );
EXTERN RtVoid AttrFrameBegin( RtInt frame );
EXTERN RtVoid AttrFrameEnd( void );
EXTERN RtVoid AttrWorldBegin( void );
EXTERN RtVoid AttrWorldEnd( void );

EXTERN PATTR AttrInitRITable( PRIB_RITABLE ritable, 
                             RIB_UINT32 flags );
EXTERN int AttrUseRIB( PATTR p, RIB_HANDLE rib );
EXTERN int AttrUse( PATTR p );
EXTERN PATTR AttrInitRIB( RIB_HANDLE rib, RIB_UINT32 flags );
EXTERN int AttrRestoreRITable( PATTR p, PRIB_RITABLE ritable );
EXTERN int AttrSetFlags( PATTR p, 
                        RIB_UINT32 mask, RIB_UINT32 flags );
EXTERN RIB_UINT32 AttrGetFlags( PATTR p );
EXTERN PATTRITEM AttrGetFirstAttribute( PATTR p );
EXTERN PATTRITEM AttrGetLastAttribute( PATTR p );
EXTERN void *AttrQueryAttribute( PATTR p, char *name, char *property );
EXTERN int AttrBeginBlock( PATTR p );
EXTERN int AttrEndBlock( PATTR p );
EXTERN int AttrDestroy( PATTR p );

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif 
