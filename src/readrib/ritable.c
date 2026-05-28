/* $RCSfile: ritable.c,v $  $Revision: 1.3 $ $Date: 2000/02/24 18:14:28 $
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
 * FILE:  table.c
 *
 * DESCRIPTION:  
 *   
 *    Contains:  
 *
 *     History:
 *       10-12-98  Added RiSubdivisionMeshV.
 *       12-05-98  Added RiGeometricRepresentation and RiResourceV.
 *       04-27-99  Added kRIB_NUCURVES and kRIB_IMPLICIT.
 *       02-24-00  Added RibNuCurvesV and RibImplicitV to allow linking with 
 *                 client libraries such as librib.a which do not have the
 *                 Ri functions.  Also RibGeometricRepresentation.
 * 
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_RITABLE
#include <stdio.h>
#include <stdlib.h>
#include <ripriv.h>
#include <ribrdr.h>


/* 
 *   Table to include in RibOpen() call unless you make your own.
 */
PRIB_RIPROC gRibRITable[] = {
   (PRIB_RIPROC)RiAreaLightSourceV,
   (PRIB_RIPROC)RiAtmosphereV,
   (PRIB_RIPROC)RiAttributeV,
   (PRIB_RIPROC)RiAttributeBegin,
   (PRIB_RIPROC)RiAttributeEnd,
   (PRIB_RIPROC)RiBasis,
   (PRIB_RIPROC)RiBound,
   (PRIB_RIPROC)RiClipping,
   (PRIB_RIPROC)RiColor,
   (PRIB_RIPROC)RiColorSamples,
   (PRIB_RIPROC)RiConcatTransform,
   (PRIB_RIPROC)RiConeV,
   (PRIB_RIPROC)RiCoordinateSystem,
   (PRIB_RIPROC)RiCoordSysTransform,
   (PRIB_RIPROC)RiCropWindow,
   (PRIB_RIPROC)RiCurvesV,
   (PRIB_RIPROC)RiCylinderV,
   (PRIB_RIPROC)RiDeclare,
   (PRIB_RIPROC)RiDeformationV,
   (PRIB_RIPROC)RiDepthOfField,
   (PRIB_RIPROC)RiDetail,
   (PRIB_RIPROC)RiDetailRange,
   (PRIB_RIPROC)RiDiskV,
   (PRIB_RIPROC)RiDisplacementV,
   (PRIB_RIPROC)RiDisplayV,
   (PRIB_RIPROC)RiErrorHandler,
   (PRIB_RIPROC)RiExposure,
   (PRIB_RIPROC)RiExteriorV,
   (PRIB_RIPROC)RiFormat,
   (PRIB_RIPROC)RiFrameAspectRatio,
   (PRIB_RIPROC)RiFrameBegin,
   (PRIB_RIPROC)RiFrameEnd,
   (PRIB_RIPROC)RiGeneralPolygonV,
   (PRIB_RIPROC)RiGeometricApproximation,
#ifdef BMRT
   (PRIB_RIPROC)RiGeometricRepresentation,
#else
   (PRIB_RIPROC)RibGeometricRepresentation,
#endif
   (PRIB_RIPROC)RiGeometryV,
   (PRIB_RIPROC)RiHiderV,
   (PRIB_RIPROC)RiHyperboloidV,
   (PRIB_RIPROC)RiIdentity,
   (PRIB_RIPROC)RiIlluminate,
   (PRIB_RIPROC)RiImagerV,
   (PRIB_RIPROC)RibImplicitV, /* PrMan 3.8 */
   (PRIB_RIPROC)RiInteriorV,
   (PRIB_RIPROC)RiLightSourceV,
   (PRIB_RIPROC)RiMakeBumpV,
   (PRIB_RIPROC)RiMakeCubeFaceEnvironmentV,
   (PRIB_RIPROC)RiMakeLatLongEnvironmentV,
   (PRIB_RIPROC)RiMakeShadowV,
   (PRIB_RIPROC)RiMakeTextureV,
   (PRIB_RIPROC)RiMatte,
   (PRIB_RIPROC)RiMotionBeginV,
   (PRIB_RIPROC)RiMotionEnd,
#ifdef RDC
   (PRIB_RIPROC)RiNuCurvesV, /* RenderDotC */
#else
   (PRIB_RIPROC)RibNuCurvesV, /* Local version to allow PRMan libs to work. */
#endif
   (PRIB_RIPROC)RiNuPatchV,
   (PRIB_RIPROC)RiObjectBegin,
   (PRIB_RIPROC)RiObjectEnd,
   (PRIB_RIPROC)RiObjectInstance,
   (PRIB_RIPROC)RiOpacity,
   (PRIB_RIPROC)RiOptionV,
   (PRIB_RIPROC)RiOrientation,
   (PRIB_RIPROC)RiParaboloidV,
   (PRIB_RIPROC)RiPatchV,
   (PRIB_RIPROC)RiPatchMeshV,
   (PRIB_RIPROC)RiPerspective,
   (PRIB_RIPROC)RiPixelFilter,
   (PRIB_RIPROC)RiPixelSamples,
   (PRIB_RIPROC)RiPixelVariance,
   (PRIB_RIPROC)RiPointsV,
   (PRIB_RIPROC)RiPointsGeneralPolygonsV,
   (PRIB_RIPROC)RiPointsPolygonsV,
   (PRIB_RIPROC)RiPolygonV,
   (PRIB_RIPROC)RiProcedural,
   (PRIB_RIPROC)RiProjectionV,
   (PRIB_RIPROC)RiQuantize,
   (PRIB_RIPROC)RiReadArchiveV,
   (PRIB_RIPROC)RiRelativeDetail,
   (PRIB_RIPROC)RiResourceV,
   (PRIB_RIPROC)RiReverseOrientation,
   (PRIB_RIPROC)RiRotate,
   (PRIB_RIPROC)RiScale,
   (PRIB_RIPROC)RiScreenWindow,
   (PRIB_RIPROC)RiShadingInterpolation,
   (PRIB_RIPROC)RiShadingRate,
   (PRIB_RIPROC)RiShutter,
   (PRIB_RIPROC)RiSides,
   (PRIB_RIPROC)RiSkew,
   (PRIB_RIPROC)RiSolidBegin,
   (PRIB_RIPROC)RiSolidEnd,
   (PRIB_RIPROC)RiSphereV,
   (PRIB_RIPROC)RiSubdivisionMeshV,
   (PRIB_RIPROC)RiSurfaceV,
   (PRIB_RIPROC)RiTextureCoordinates,
   (PRIB_RIPROC)RiTorusV,
   (PRIB_RIPROC)RiTransform,
   (PRIB_RIPROC)RiTransformBegin,
   (PRIB_RIPROC)RiTransformEnd,
   (PRIB_RIPROC)RiTranslate,
   (PRIB_RIPROC)RiTrimCurve,
   (PRIB_RIPROC)RiWorldBegin,
   (PRIB_RIPROC)RiWorldEnd,
   (PRIB_RIPROC)Rib_version,
/* No RIB statements for the following: */
   (PRIB_RIPROC)RiBegin,
   (PRIB_RIPROC)RiEnd,
   (PRIB_RIPROC)RiArchiveRecord,
   (PRIB_RIPROC)RiTransformPoints,
   (PRIB_RIPROC)RiErrorAbort,
   (PRIB_RIPROC)RiErrorIgnore,
   (PRIB_RIPROC)RiErrorPrint,
   (PRIB_RIPROC)RiBoxFilter,
   (PRIB_RIPROC)RiCatmullRomFilter,
   (PRIB_RIPROC)RiGaussianFilter,
   (PRIB_RIPROC)RiSincFilter,
   (PRIB_RIPROC)RiTriangleFilter,
   (PRIB_RIPROC)RiProcDelayedReadArchive,
   (PRIB_RIPROC)RiProcDynamicLoad,
   (PRIB_RIPROC)RiProcRunProgram
};
