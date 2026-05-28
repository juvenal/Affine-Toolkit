/* $RCSfile: ribfunc.h,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:37 $
 *
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
 * FILE:  ribfunc.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *          Structures used for the RIB Reader library.
 *
 *    History:
 *       01-06-98  Fixed s2Bs which was only accounting for one pointer
 *                 not two.
 *       10-12-98  Added RibReadSubdivsionMeshV.
 *       12-05-98  Added RibReadGeometricRepresentation and RibReadResourceV.
 *       04-27-99  Added RiNopNuCurvesV() and RiNopImplicitV().
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
#ifndef __RIBFUNC_HEADER__
#define __RIBFUNC_HEADER__

#define STATIC static 
STATIC int RibReadAreaLightSourceV( PRIB_INSTANCE rib );
STATIC int RibReadAtmosphereV( PRIB_INSTANCE rib );
STATIC int RibReadAttributeV( PRIB_INSTANCE rib );
STATIC int RibReadAttributeBegin( PRIB_INSTANCE rib );
STATIC int RibReadAttributeEnd( PRIB_INSTANCE rib );
STATIC int RibReadBasis( PRIB_INSTANCE rib );
STATIC int RibReadBound( PRIB_INSTANCE rib );
STATIC int RibReadClipping( PRIB_INSTANCE rib );
STATIC int RibReadColor( PRIB_INSTANCE rib );
STATIC int RibReadColorSamples( PRIB_INSTANCE rib );
STATIC int RibReadConcatTransform( PRIB_INSTANCE rib );
STATIC int RibReadConeV( PRIB_INSTANCE rib );
STATIC int RibReadCoordinateSystem( PRIB_INSTANCE rib );
STATIC int RibReadCoordSysTransform( PRIB_INSTANCE rib );
STATIC int RibReadCropWindow( PRIB_INSTANCE rib );
STATIC int RibReadCurvesV( PRIB_INSTANCE rib );
STATIC int RibReadCylinderV( PRIB_INSTANCE rib );
STATIC int RibReadDeclare( PRIB_INSTANCE rib );
STATIC int RibReadDeformationV( PRIB_INSTANCE rib );
STATIC int RibReadDepthOfField( PRIB_INSTANCE rib );
STATIC int RibReadDetail( PRIB_INSTANCE rib );
STATIC int RibReadDetailRange( PRIB_INSTANCE rib );
STATIC int RibReadDiskV( PRIB_INSTANCE rib );
STATIC int RibReadDisplacementV( PRIB_INSTANCE rib );
STATIC int RibReadDisplayV( PRIB_INSTANCE rib );
STATIC int RibReadErrorHandler( PRIB_INSTANCE rib );
STATIC int RibReadExposure( PRIB_INSTANCE rib );
STATIC int RibReadExteriorV( PRIB_INSTANCE rib );
STATIC int RibReadFormat( PRIB_INSTANCE rib );
STATIC int RibReadFrameAspectRatio( PRIB_INSTANCE rib );
STATIC int RibReadFrameBegin( PRIB_INSTANCE rib );
STATIC int RibReadFrameEnd( PRIB_INSTANCE rib );
STATIC int RibReadGeneralPolygonV( PRIB_INSTANCE rib );
STATIC int RibReadGeometricApproximation( PRIB_INSTANCE rib );
STATIC int RibReadGeometricRepresentation( PRIB_INSTANCE rib );
STATIC int RibReadGeometryV( PRIB_INSTANCE rib );
STATIC int RibReadHiderV( PRIB_INSTANCE rib );
STATIC int RibReadHyperboloidV( PRIB_INSTANCE rib );
STATIC int RibReadIdentity( PRIB_INSTANCE rib );
STATIC int RibReadIlluminate( PRIB_INSTANCE rib );
STATIC int RibReadImagerV( PRIB_INSTANCE rib );
STATIC int RibReadImplicitV( PRIB_INSTANCE rib );
STATIC int RibReadInteriorV( PRIB_INSTANCE rib );
STATIC int RibReadLightSourceV( PRIB_INSTANCE rib );
STATIC int RibReadMakeBumpV( PRIB_INSTANCE rib );
STATIC int RibReadMakeCubeFaceEnvironmentV( PRIB_INSTANCE rib );
STATIC int RibReadMakeLatLongEnvironmentV( PRIB_INSTANCE rib );
STATIC int RibReadMakeShadowV( PRIB_INSTANCE rib );
STATIC int RibReadMakeTextureV( PRIB_INSTANCE rib );
STATIC int RibReadMatte( PRIB_INSTANCE rib );
STATIC int RibReadMotionBeginV( PRIB_INSTANCE rib );
STATIC int RibReadMotionEnd( PRIB_INSTANCE rib );
STATIC int RibReadNuCurvesV( PRIB_INSTANCE rib );
STATIC int RibReadNuPatchV( PRIB_INSTANCE rib );
STATIC int RibReadObjectBegin( PRIB_INSTANCE rib );
STATIC int RibReadObjectEnd( PRIB_INSTANCE rib );
STATIC int RibReadObjectInstance( PRIB_INSTANCE rib );
STATIC int RibReadOpacity( PRIB_INSTANCE rib );
STATIC int RibReadOptionV( PRIB_INSTANCE rib );
STATIC int RibReadOrientation( PRIB_INSTANCE rib );
STATIC int RibReadParaboloidV( PRIB_INSTANCE rib );
STATIC int RibReadPatchV( PRIB_INSTANCE rib );
STATIC int RibReadPatchMeshV( PRIB_INSTANCE rib );
STATIC int RibReadPerspective( PRIB_INSTANCE rib );
STATIC int RibReadPixelFilter( PRIB_INSTANCE rib );
STATIC int RibReadPixelSamples( PRIB_INSTANCE rib );
STATIC int RibReadPixelVariance( PRIB_INSTANCE rib );
STATIC int RibReadPointsGeneralPolygonsV( PRIB_INSTANCE rib );
STATIC int RibReadPointsV( PRIB_INSTANCE rib );
STATIC int RibReadPointsPolygonsV( PRIB_INSTANCE rib );
STATIC int RibReadPolygonV( PRIB_INSTANCE rib );
STATIC int RibReadProcedural( PRIB_INSTANCE rib );
STATIC int RibReadProjectionV( PRIB_INSTANCE rib );
STATIC int RibReadQuantize( PRIB_INSTANCE rib );
STATIC int RibReadReadArchiveV( PRIB_INSTANCE rib );
STATIC int RibReadRelativeDetail( PRIB_INSTANCE rib );
STATIC int RibReadResourceV( PRIB_INSTANCE rib );
STATIC int RibReadReverseOrientation( PRIB_INSTANCE rib );
STATIC int RibReadRotate( PRIB_INSTANCE rib );
STATIC int RibReadScale( PRIB_INSTANCE rib );
STATIC int RibReadScreenWindow( PRIB_INSTANCE rib );
STATIC int RibReadShadingInterpolation( PRIB_INSTANCE rib );
STATIC int RibReadShadingRate( PRIB_INSTANCE rib );
STATIC int RibReadShutter( PRIB_INSTANCE rib );
STATIC int RibReadSides( PRIB_INSTANCE rib );
STATIC int RibReadSkew( PRIB_INSTANCE rib );
STATIC int RibReadSolidBegin( PRIB_INSTANCE rib );
STATIC int RibReadSolidEnd( PRIB_INSTANCE rib );
STATIC int RibReadSphereV( PRIB_INSTANCE rib );
STATIC int RibReadSubdivisionMeshV( PRIB_INSTANCE rib );
STATIC int RibReadSurfaceV( PRIB_INSTANCE rib );
STATIC int RibReadTextureCoordinates( PRIB_INSTANCE rib );
STATIC int RibReadTorusV( PRIB_INSTANCE rib );
STATIC int RibReadTransform( PRIB_INSTANCE rib );
STATIC int RibReadTransformBegin( PRIB_INSTANCE rib );
STATIC int RibReadTransformEnd( PRIB_INSTANCE rib );
STATIC int RibReadTranslate( PRIB_INSTANCE rib );
STATIC int RibReadTrimCurve( PRIB_INSTANCE rib );
STATIC int RibReadWorldBegin( PRIB_INSTANCE rib );
STATIC int RibReadWorldEnd( PRIB_INSTANCE rib );
STATIC int RibRead_Version( PRIB_INSTANCE rib );

STATIC int RibReadVer35NuPatchV( PRIB_INSTANCE rib );

#define  CheckForError(rc)  if(rc) goto Error

#define  BasisFromPtrAndInt(p,i) ((RtFloat(*)[4])*((void**)((char*)(p)+(i))))
#define  TknFromPtrAndInt(p,i) ((RtToken)*((void**)((char*)(p)+(i))))
#define  ChrFromPtrAndInt(p,i) ((char*)*((void**)((char*)(p)+(i))))
#define  PtrFromPtrAndInt(p,i) ((void*)*((void**)((char*)(p)+(i))))
#define  FltFromPtrAndInt(p,i) ((RtFloat)*((RtFloat*)((char*)(p)+(i))))
#define  IntFromPtrAndInt(p,i) ((RtInt)*((RtInt*)((char*)(p)+(i))))

#define  s1B  (sizeof(PRtBound))
#define  s1Bl (sizeof(RtBoolean))
#define  s1M  (sizeof(PRtMatrix))
#define  s1Lh (sizeof(RtLightHandle))
#define  s1Oh (sizeof(RtObjectHandle))
#define  s1Eh (sizeof(RtErrorHandler))
#define  s1Ff (sizeof(RtFilterFunc))

#define  s1C  (sizeof(PRtColor))

#define  s1Bs  (  sizeof(PRtBasis))
#define  s2Bs  (2*sizeof(PRtBasis))

#define  s1Pt (  sizeof(PRtPoint))
#define  s2Pt (2*sizeof(PRtPoint))

#define  s1I (  sizeof(RtInt))
#define  s2I (2*sizeof(RtInt))
#define  s3I (3*sizeof(RtInt))
#define  s4I (4*sizeof(RtInt))

#define  s1Ia (  sizeof(PRtInt))
#define  s2Ia (2*sizeof(PRtInt))
#define  s3Ia (3*sizeof(PRtInt))

#define  s1F (  sizeof(RtFloat))
#define  s2F (2*sizeof(RtFloat))
#define  s3F (3*sizeof(RtFloat))
#define  s4F (4*sizeof(RtFloat))
#define  s5F (5*sizeof(RtFloat))
#define  s6F (6*sizeof(RtFloat))
#define  s7F (7*sizeof(RtFloat))
#define  s8F (8*sizeof(RtFloat))

#define  s1Fa (  sizeof(PRtFloat))
#define  s2Fa (2*sizeof(PRtFloat))
#define  s3Fa (3*sizeof(PRtFloat))
#define  s4Fa (4*sizeof(PRtFloat))
#define  s5Fa (5*sizeof(PRtFloat))
#define  s6Fa (6*sizeof(PRtFloat))
#define  s7Fa (7*sizeof(PRtFloat))

#define  s1Pta (  sizeof(PRtPoint))
#define  s2Pta (2*sizeof(PRtPoint))

#define  s1T (  sizeof(RtToken))
#define  s2T (2*sizeof(RtToken))
#define  s3T (3*sizeof(RtToken))

#define  s1Cpr (  sizeof(char*))
#define  s2Cpr (2*sizeof(char*))
#define  s3Cpr (3*sizeof(char*))
#define  s4Cpr (4*sizeof(char*))
#define  s5Cpr (5*sizeof(char*))
#define  s6Cpr (6*sizeof(char*))
#define  s7Cpr (7*sizeof(char*))

#endif
