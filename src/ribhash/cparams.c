/* $RCSfile: cparams.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 06:30:52 $
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
 * FILE:  cparams.c
 *
 * DESCRIPTION:  
 *   
 *
 *    References:
 *        [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, Richmond, CA, 
 *                  September 1989.
 *        [PIXA98]  Pixar, Advanced RenderMan: Beyond the Companion,
 *                  SIGGRAPH Course Notes #11, Richmond, CA, July, 1998, 
 *                  p. 12.
 *
 *    History:
 *       10-12-98  Added RibReadSubdivisionMeshV.
 *       04-27-99  Added NuCurves and Implicit.
 * 
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_CPARAMS
#include <ribhash.h>

#define N   kRIB_CPARAMS_NULL      
#define V   kRIB_CPARAMS_VECTORFORMAT
#define T   kRIB_CPARAMS_TOKEN
#define Pr  kRIB_CPARAMS_POINTER
#define I   kRIB_CPARAMS_INT
#define F   kRIB_CPARAMS_FLOAT
#define M   kRIB_CPARAMS_MATRIX
#define B   kRIB_CPARAMS_BOUND
#define Bs  kRIB_CPARAMS_BASIS
#define C   kRIB_CPARAMS_COLOR
#define Pt  kRIB_CPARAMS_POINT
#define Lh  kRIB_CPARAMS_LIGHTHANDLE
#define Oh  kRIB_CPARAMS_OBJECTHANDLE
#define Bl  kRIB_CPARAMS_BOOLEAN
#define Cpr kRIB_CPARAMS_CHAR_POINTER
#define Eh  kRIB_CPARAMS_ERRORHANDLER
#define Fn  kRIB_CPARAMS_FUNC
#define Ff  kRIB_CPARAMS_FILTERFUNC
#define Ta  kRIB_CPARAMS_TOKEN_ARRAY
#define Pra kRIB_CPARAMS_POINTER_ARRAY
#define Ia  kRIB_CPARAMS_INT_ARRAY
#define Fa  kRIB_CPARAMS_FLOAT_ARRAY
#define Pta kRIB_CPARAMS_POINT_ARRAY
#define VA  kRIB_CPARAMS_VARIABLE_ARG


static char scRibAreaLightSourceV[] = { T, V };
static char scRibAtmosphereV[] = { T, V };
static char scRibAttributeV[] = { T, V };
static char scRibAttributeBegin[] = { N };
static char scRibAttributeEnd[] = { N };
static char scRibBasis[] = { Bs, I, Bs, I, N };
static char scRibBound[] = { B, N };
static char scRibClipping[] = { F, F, N };
static char scRibColor[] = { C, N };
static char scRibColorSamples[] = { I, Fa, Fa, N };
static char scRibConcatTransform[] = { M, N };
static char scRibConeV[] = { F, F, F, V };
static char scRibCoordinateSystem[] = { T, N };
static char scRibCoordSysTransform[] = { T, N };
static char scRibCropWindow[] = { F, F, F, F, N };
static char scRibCurvesV[] = { T, I, Ia, T, V };
static char scRibCylinderV[] = { F, F, F, F, V };
static char scRibDeclare[] = { Cpr, Cpr, N };
static char scRibDeformationV[] = { T, V };
static char scRibDepthOfField[] = { F, F, F, N };
static char scRibDetail[] = { B, N };
static char scRibDetailRange[] = { F, F, F, F, N };
static char scRibDiskV[] = { F, F, F, V };
static char scRibDisplacementV[] = { T, V };
static char scRibDisplayV[] = { Cpr, T, T, V };
static char scRibErrorHandler[] = { Eh, N };
static char scRibExposure[] = { F, F, N };
static char scRibExteriorV[] = { T, V };
static char scRibFormat[] = { I, I, F, N };
static char scRibFrameAspectRatio[] = { F, N };
static char scRibFrameBegin[] = { I, N };
static char scRibFrameEnd[] = { N };
static char scRibGeneralPolygonV[] = { I, Ia, V };
static char scRibGeometricApproximation[] = { T, F, N };
static char scRibGeometricRepresentation[] = { T, N };
static char scRibGeometryV[] = { T, V };
static char scRibHiderV[] = { T, V };
static char scRibHyperboloidV[] = { Pt, Pt, F, V };
static char scRibIdentity[] = { N };
static char scRibIlluminate[] = { Lh, Bl, N };
static char scRibImagerV[] = { T, V };
static char scRibImplicitV[] = { I, Ia, I, Fa, I, Fa, I, Fa, V };
static char scRibInteriorV[] = { T, V };
static char scRibLightSourceV[] = { T, V };
static char scRibMakeBumpV[] = { Cpr, Cpr, T, T, Ff, F, F, V };
static char scRibMakeCubeFaceEnvironmentV[] = { Cpr, Cpr, Cpr, Cpr, Cpr, Cpr, 
                                               Cpr, F, Ff, F, F, V };
static char scRibMakeLatLongEnvironmentV[] = { Cpr, Cpr, Ff, F, F, V };
static char scRibMakeShadowV[] = { Cpr, Cpr, V };
static char scRibMakeTextureV[] = { Cpr, Cpr, T, T, Ff, F, F, V };
static char scRibMatte[] = { Bl, N };
static char scRibMotionBeginV[] = { I, Fa, N };
static char scRibMotionEnd[] = { N };
static char scRibNuCurvesV[] = { I, Ia, Ia, Fa, Fa, Fa, V };
static char scRibNuPatchV[] = { I, I, Fa, F, F, I, I, Fa, F, F, V };
static char scRibObjectBegin[] = { N };
static char scRibObjectEnd[] = { N };
static char scRibObjectInstance[] = { Oh, N };
static char scRibOpacity[] = { C, N };
static char scRibOptionV[] = { T, V };
static char scRibOrientation[] = { T, N };
static char scRibParaboloidV[] = { F, F, F, F, V };
static char scRibPatchV[] = { T, V };
static char scRibPatchMeshV[] = { T, I, T, I, T, V };
static char scRibPerspective[] = { F, N };
static char scRibPixelFilter[] = { Ff, F, F, N };
static char scRibPixelSamples[] = { F, F, N };
static char scRibPixelVariance[] = { F, N };
static char scRibPointsV[] = { I, V };
static char scRibPointsGeneralPolygonsV[] = { I, Ia, Ia, Ia, V };
static char scRibPointsPolygonsV[] = { I, Ia, Ia, V };
static char scRibPolygonV[] = { I, V };
static char scRibProcedural[] = { Pr, B, Fn, Fn, N };
/* In RIB files, Procedural binding uses (token, string array, bound). */
static char scRibProjectionV[] = { T, V };
static char scRibQuantize[] = { T, I, I, I, F, N };
static char scRibReadArchiveV[] = { T, Fn, V };
static char scRibRelativeDetail[] = { F, N };
static char scRibResourceV[] = { T, T, V };
static char scRibReverseOrientation[] = { N };
static char scRibRotate[] = { F, F, F, F, N };
static char scRibScale[] = { F, F, F, N };
static char scRibScreenWindow[] = { F, F, F, F, N };
static char scRibShadingInterpolation[] = { T, N };
static char scRibShadingRate[] = { F, N };
static char scRibShutter[] = { F, F, N };
static char scRibSides[] = { I, N };
static char scRibSkew[] = { F, F, F, F, F, F, F, N };
static char scRibSolidBegin[] = { T, N };
static char scRibSolidEnd[] = { N };
static char scRibSphereV[] = { F, F, F, F, V };
static char scRibSubdivisionMeshV[] = { T, I, Ia, Ia, I, Ta, Ia, Ia, Fa, V };
static char scRibSurfaceV[] = { T, V };
static char scRibTextureCoordinates[] = { F, F, F, F, F, F, F, F, N };
static char scRibTorusV[] = { F, F, F, F, F, V };
static char scRibTransform[] = { M, N };
static char scRibTransformBegin[] = { N };
static char scRibTransformEnd[] = { N };
static char scRibTranslate[] = { F, F, F, N };
static char scRibTrimCurve[] = { I, Ia, Ia, Fa, Fa, Fa, Ia, Fa, Fa, Fa, N };
static char scRibWorldBegin[] = { N };
static char scRibWorldEnd[] = { N };
static char scRibversion[] = { F, N };
static char scRibBegin[] = { T, N };
static char scRibEnd[] = { N };
static char scRibArchiveRecord[] = { T, Cpr, VA, N };
static char scRibTransformPoints[] = { T, T, I, Pta, N };
static char scRibErrorAbort[] = { I, I, Cpr, N };
static char scRibErrorIgnore[] = { I, I, Cpr, N };
static char scRibErrorPrint[] = { I, I, Cpr, N };
static char scRibBoxFilter[] = { F, F, F, F, N };
static char scRibCatmullRomFilter[] = { F, F, F, F, N };
static char scRibGaussianFilter[] = { F, F, F, F, N };
static char scRibSincFilter[] = { F, F, F, F, N };
static char scRibTriangleFilter[] = { F, F, F, F, N };
static char scRibProcDelayedReadArchive[] = { Pr, F, N };
static char scRibProcDynamicLoad[] = { Pr, F, N };
static char scRibProcRunProgram[] = { Pr, F, N };

char *gRibCParams[] = {
   scRibAreaLightSourceV,
   scRibAtmosphereV,
   scRibAttributeV,
   scRibAttributeBegin,
   scRibAttributeEnd,
   scRibBasis,
   scRibBound,
   scRibClipping,
   scRibColor,
   scRibColorSamples,
   scRibConcatTransform,
   scRibConeV,
   scRibCoordinateSystem,
   scRibCoordSysTransform,
   scRibCropWindow,
   scRibCurvesV,
   scRibCylinderV,
   scRibDeclare,
   scRibDeformationV,
   scRibDepthOfField,
   scRibDetail,
   scRibDetailRange,
   scRibDiskV,
   scRibDisplacementV,
   scRibDisplayV,
   scRibErrorHandler,
   scRibExposure,
   scRibExteriorV,
   scRibFormat,
   scRibFrameAspectRatio,
   scRibFrameBegin,
   scRibFrameEnd,
   scRibGeneralPolygonV,
   scRibGeometricApproximation,
   scRibGeometricRepresentation,
   scRibGeometryV,
   scRibHiderV,
   scRibHyperboloidV,
   scRibIdentity,
   scRibIlluminate,
   scRibImagerV,
   scRibImplicitV,
   scRibInteriorV,
   scRibLightSourceV,
   scRibMakeBumpV,
   scRibMakeCubeFaceEnvironmentV,
   scRibMakeLatLongEnvironmentV,
   scRibMakeShadowV,
   scRibMakeTextureV,
   scRibMatte,
   scRibMotionBeginV,
   scRibMotionEnd,
   scRibNuCurvesV,
   scRibNuPatchV,
   scRibObjectBegin,
   scRibObjectEnd,
   scRibObjectInstance,
   scRibOpacity,
   scRibOptionV,
   scRibOrientation,
   scRibParaboloidV,
   scRibPatchV,
   scRibPatchMeshV,
   scRibPerspective,
   scRibPixelFilter,
   scRibPixelSamples,
   scRibPixelVariance,
   scRibPointsV,
   scRibPointsGeneralPolygonsV,
   scRibPointsPolygonsV,
   scRibPolygonV,
   scRibProcedural,
   scRibProjectionV,
   scRibQuantize,
   scRibReadArchiveV,
   scRibRelativeDetail,
   scRibResourceV,
   scRibReverseOrientation,
   scRibRotate,
   scRibScale,
   scRibScreenWindow,
   scRibShadingInterpolation,
   scRibShadingRate,
   scRibShutter,
   scRibSides,
   scRibSkew,
   scRibSolidBegin,
   scRibSolidEnd,
   scRibSphereV,
   scRibSubdivisionMeshV,
   scRibSurfaceV,
   scRibTextureCoordinates,
   scRibTorusV,
   scRibTransform,
   scRibTransformBegin,
   scRibTransformEnd,
   scRibTranslate,
   scRibTrimCurve,
   scRibWorldBegin,
   scRibWorldEnd,
   scRibversion,
   scRibBegin,
   scRibEnd,
   scRibArchiveRecord,
   scRibTransformPoints,
   scRibErrorAbort,
   scRibErrorIgnore,
   scRibErrorPrint,
   scRibBoxFilter,
   scRibCatmullRomFilter,
   scRibGaussianFilter,
   scRibSincFilter,
   scRibTriangleFilter,
   scRibProcDelayedReadArchive,
   scRibProcDynamicLoad,
   scRibProcRunProgram
};
