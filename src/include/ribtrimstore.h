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
 * FILE:  ribtrimstore.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          Structures used for the hash portion of RIB Reader library.
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
#ifndef __RIBTRIMSTORE_HEADER__
#define __RIBTRIMSTORE_HEADER__

#define  TRIMSTORE_DONTFREEDATA    0x00000001
#define  TRIMSTORE_REVERSETRIM     0x00000004
#define  TRIMSTORE_APPLYALL        0x00000008

enum TRIMSTORECALLS { 
   TS_ATTRIBUTE,
   TS_ATTRIBUTEBEGIN,
   TS_ATTRIBUTEEND,
   TS_BEGIN,
   TS_END,
   TS_FRAMEBEGIN,
   TS_FRAMEEND,
   TS_TRIMCURVE,
   TS_WORLDBEGIN,
   TS_WORLDEND,
   TS_LAST
};


typedef struct _TRIMCURVE {
   RtInt nloops;
   RtInt *ncurves;
   RtInt *order;
   RtFloat *knot;
   RtFloat *min;
   RtFloat *max;
   RtInt *n;
   RtFloat *u;
   RtFloat *v;
   RtFloat *w;
   int  level;
   int  applied;
   RtBoolean  reverse;
   RIB_HANDLE   rib; 
   struct _TRIMCURVE  *next;
   struct _TRIMCURVE  *prev;
} TRIMCURVE;
typedef TRIMCURVE  *PTRIMCURVE;


typedef struct _TRIMSTORE {
   RIB_HANDLE     rib;
   RIB_UINT32     flags;
   int            presentlevel;
   RtBoolean      senseoutside;
   PTRIMCURVE     first;
   PTRIMCURVE     last;
   PRIB_RIPROC    ricalls[TS_LAST];
} TRIMSTORE;
typedef TRIMSTORE  *PTRIMSTORE;


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#ifdef __RIB_TRIMSTORE_FILE
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN RtVoid TsAttributeBegin( void );
EXTERN RtVoid TsAttributeEnd( void );
EXTERN RtVoid TsAttributeV( RtToken name,
                           RtInt n, RtToken tokens[], RtPointer parms[] );
EXTERN RtVoid TsBegin( RtToken name );
EXTERN RtVoid TsEnd( void );
EXTERN RtVoid TsFrameBegin( RtInt frame );
EXTERN RtVoid TsFrameEnd( void );
EXTERN RtVoid TsTrimCurve( RtInt nloops, 
                   RtInt ncurves[], RtInt order[], RtFloat knot[],
                   RtFloat min[], RtFloat max[], RtInt n[],
                   RtFloat u[], RtFloat v[], RtFloat w[] );
EXTERN RtVoid TsWorldBegin( void );
EXTERN RtVoid TsWorldEnd( void );

EXTERN PTRIMSTORE TrimStoreInitRITable( PRIB_RITABLE ritable, 
                                       RIB_UINT32 flags );
EXTERN int TrimStoreUseRIB( PTRIMSTORE p, RIB_HANDLE rib );
EXTERN int TrimStoreUse( PTRIMSTORE p );
EXTERN PTRIMSTORE TrimStoreInitRIB( RIB_HANDLE rib, RIB_UINT32 flags );
EXTERN int TrimStoreRestoreRITable( PTRIMSTORE p, PRIB_RITABLE ritable );
EXTERN int TrimStoreSetFlags( PTRIMSTORE p, 
                             RIB_UINT32 mask, RIB_UINT32 flags );
EXTERN RIB_UINT32 TrimStoreGetFlags( PTRIMSTORE p );
EXTERN RtBoolean  TrimStoreIsaTrimmedSurface( PTRIMSTORE p );
EXTERN PTRIMCURVE TrimStoreGetFirstCurve( PTRIMSTORE p );
EXTERN PTRIMCURVE TrimStoreGetLastCurve( PTRIMSTORE p );
EXTERN int TrimStoreBeginBlock( PTRIMSTORE p );
EXTERN PTRIMCURVE TrimStoreCurves( PTRIMSTORE p, 
                           RtInt nloops, 
                           RtInt ncurves[], RtInt order[], RtFloat knot[],
                           RtFloat min[], RtFloat max[], RtInt n[],
                           RtFloat u[], RtFloat v[], RtFloat w[] );
EXTERN int TrimStoreApplyCurves( PTRIMSTORE p );
EXTERN int TrimStoreEndBlock( PTRIMSTORE p );
EXTERN int TrimStoreDestroy( PTRIMSTORE p );

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif 
