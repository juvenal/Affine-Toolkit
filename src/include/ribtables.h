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
 * FILE:  ribtables.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *
 *    References:
 *          [PIXA89]  Pixar Animation Studios, The RenderMan Interface, 
 *                    Version 3.1, Richmond, CA, September 1989.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 */
#ifndef __RIBTABLE_HEADER__
#define __RIBTABLE_HEADER__

enum {
   kRIBTABLES_BEZIERBASIS,
   kRIBTABLES_BSPLINEBASIS,
   kRIBTABLES_CATMULLROMBASIS,
   kRIBTABLES_HERMITEBASIS,
   kRIBTABLES_POWERBASIS
};

enum {
   kRIBTABLES_UNIFORMCLASS,
   kRIBTABLES_VARYINGCLASS,
   kRIBTABLES_VERTEXCLASS,
   kRIBTABLES_CONSTANTCLASS
};

enum {
   kRIBTABLES_LINEAR,
   kRIBTABLES_CUBIC
};

enum {
   kRIBTABLES_BOXFILTER,
   kRIBTABLES_CATMULLROMFILTER,
   kRIBTABLES_GAUSSIANFILTER,
   kRIBTABLES_SINCFILTER,
   kRIBTABLES_TRIANGLEFILTER
};


/* gRibErrorHandlerTable */
enum {
   kRIBTABLES_IGNORE,
   kRIBTABLES_PRINT,
   kRIBTABLES_ABORT
};


/* gRibProceduralTable */
enum {
   kRIBTABLES_DELAYEDREADARCHIVE,
   kRIBTABLES_DYNAMICLOAD,
   kRIBTABLES_RUNPROGRAM
};


/* gRibFormatTable */
enum {
   kRIBTABLES_RISPEC31FORMAT,
   kRIBTABLES_NUPATCH36FORMAT,
   kRIBTABLES_INDENTFORMAT,
   kRIBTABLES_LONGFORMAT,
   kRIBTABLES_SHORTFORMAT
};

enum {
   kRIBTABLES_CONSTANT,
   kRIBTABLES_SMOOTH
};

enum {
   kRIBTABLES_LHORIENT,
   kRIBTABLES_RHORIENT,
   kRIBTABLES_OUTSIDEORIENT,
   kRIBTABLES_INSIDEORIENT
};

enum {
   kRIBTABLES_BICUBIC,
   kRIBTABLES_BILINEAR
};

enum {
   kRIBTABLES_PERIODIC,
   kRIBTABLES_NONPERIODIC
};

enum {
   kRIBTABLES_COMMENT,
   kRIBTABLES_STRUCTURE
};

enum {
   kRIBTABLES_PRIMITIVE,
   kRIBTABLES_INTERSECTION,
   kRIBTABLES_UNION,
   kRIBTABLES_DIFFERENCE
};

enum {
   kRIBTABLES_FLOATTYPE,
   kRIBTABLES_STRINGTYPE,
   kRIBTABLES_POINTTYPE,
   kRIBTABLES_COLORTYPE,
   kRIBTABLES_NORMALTYPE,
   kRIBTABLES_VECTORTYPE,
   kRIBTABLES_HPOINTTYPE,
   kRIBTABLES_INTEGERTYPE,
   kRIBTABLES_MATRIXTYPE
};

enum {
   kRIBTABLES_CAMERA_CS,
   kRIBTABLES_RASTER_CS,
   kRIBTABLES_OBJECT_CS,
   kRIBTABLES_WORLD_CS,
   kRIBTABLES_SCREEN_CS,
   kRIBTABLES_NDC_CS
};


#endif
