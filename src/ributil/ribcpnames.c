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
 * FILE:  ribcpnames.c
 *
 * DESCRIPTION:  Table of the C parameter names used in the Ri functions.
 *
 *    Contains:  
 * 
 *    References:
 *        [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, Richmond, CA, 
 *                  September 1989.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 */
#define  __RIB_FILE_CPNAMES
#include <stdlib.h>


static char *cpRiAreaLightSourceV[] = { "name" };
static char *cpRiAtmosphereV[] = { "name" };
static char *cpRiAttributeV[] = { "name" };
static char *cpRiAttributeBegin[] = { NULL };
static char *cpRiAttributeEnd[] = { NULL };
static char *cpRiBasis[] = { "ubasis", "ustep", "vbasis", "vstep" };
static char *cpRiBound[] = { "bound" };
static char *cpRiClipping[] = { "hither", "yon" };
static char *cpRiColor[] = { "color" };
static char *cpRiColorSamples[] = { "n", "nRGB[]", "RGBn[]" };
static char *cpRiConcatTransform[] = { "transform" };
static char *cpRiConeV[] = { "height", "radius", "thetamax" };
static char *cpRiCoordinateSystem[] = { "space" };
static char *cpRiCoordSysTransform[] = { "space" };
static char *cpRiCropWindow[] = { "xmin", "xmax", "ymin", "ymax" };
static char *cpRiCurvesV[] = { "type", "ncurves", "nvertices", "wrap"}; 
static char *cpRiCylinderV[] = { "radius", "zmin", "zmax", "thetamax" };
static char *cpRiDeclare[] = { "name", "declaration" };
static char *cpRiDeformationV[] = { "name" };
static char *cpRiDepthOfField[] = { "fstop", "focallength", "focaldistance" };
static char *cpRiDetail[] = { "bound" };
static char *cpRiDetailRange[] = { "minvisible", "lowertransition",
                                  "uppertransition", "maxvisible" };
static char *cpRiDiskV[] = { "height", "radius", "thetamax" };
static char *cpRiDisplacementV[] = { "name" };
static char *cpRiDisplayV[] = { "name", "type", "mode" };
static char *cpRiErrorHandler[] = { "handler" };
static char *cpRiExposure[] = { "gain", "gamma" };
static char *cpRiExteriorV[] = { "name" };
static char *cpRiFormat[] = { "xres", "yres", "aspect" };
static char *cpRiFrameAspectRatio[] = { "aspect" };
static char *cpRiFrameBegin[] = { "frame" };
static char *cpRiFrameEnd[] = { NULL };
static char *cpRiGeneralPolygonV[] = { "nloops", "nvertices" };
static char *cpRiGeometricApproximation[] = { "type", "value" };
static char *cpRiGeometricRepresentation[] = { "type" };
static char *cpRiGeometryV[] = { "type" };
static char *cpRiHiderV[] = { "type" };
static char *cpRiHyperboloidV[] = { "point1", "point2", "thetamax" };
static char *cpRiIdentity[] = { NULL };
static char *cpRiIlluminate[] = { "light", "onoff" };
static char *cpRiImagerV[] = { "name" };
static char *cpRiInteriorV[] = { "name" };
static char *cpRiLightSourceV[] = { "name" };
static char *cpRiMakeBumpV[] = { "char *picturename", "texturename",
                                  "swrap", "twrap",
                                  "filterfunc", "swidth", "twidth" };
static char *cpRiMakeCubeFaceEnvironmentV[] = { "px", "nx", "py", "ny",
				  "pz", "nz", "texturename",
				  "fov", "filterfunc", "swidth", "twidth" };
static char *cpRiMakeLatLongEnvironmentV[] = { "picturename", "texturename",
                                  "filterfunc", "swidth", "twidth" };
static char *cpRiMakeShadowV[] = { "picturename", "texturename" };
static char *cpRiMakeTextureV[] = { "picturename", "texturename",
		      "swrap", "twrap", "filterfunc", "swidth", "twidth" };
static char *cpRiMatte[] = { "onoff" };
static char *cpRiMotionBeginV[] = { "n", "times" };
static char *cpRiMotionEnd[] = { NULL };
static char *cpRiNuPatchV[] = { "nu", "uorder", "uknot", "umin", "umax", 
                                "nv", "vorder", "vknot", "vmin", "vmax" };
static char *cpRiObjectBegin[] = { NULL };
static char *cpRiObjectEnd[] = { NULL };
static char *cpRiObjectInstance[] = { "handle" };
static char *cpRiOpacity[] = { "color" };
static char *cpRiOptionV[] = { "name" };
static char *cpRiOrientation[] = { "orientation" };
static char *cpRiParaboloidV[] = { "rmax", "zmin", "zmax", "thetamax" };
static char *cpRiPatchV[] = { "type" };
static char *cpRiPatchMeshV[] = { "type", "nu", "uwrap", "nv", "vwrap" };
static char *cpRiPerspective[] = { "fov" };
static char *cpRiPixelFilter[] = { "filterfunc", "xwidth", "ywidth" };
static char *cpRiPixelSamples[] = { "xsamples", "ysamples" };
static char *cpRiPixelVariance[] = { "variation" };
static char *cpRiPointsV[] = { "npoints" };
static char *cpRiPointsGeneralPolygonsV[] = { "npolys", "nloops",
                                             "nvertices", "vertices" };
static char *cpRiPointsPolygonsV[] = { "npolys", "nvertices", "vertices" };
static char *cpRiPolygonV[] = { "nvertices" };
static char *cpRiProjectionV[] = { "name" };
static char *cpRiQuantize[] = { "type", "one", "min", "max", "ampl" };
static char *cpRiReadArchiveV[] = { "name", "callback", "format", "s" };
static char *cpRiRelativeDetail[] = { "relativedetail" };
static char *cpRiReverseOrientation[] = { NULL };
static char *cpRiRotate[] = { "angle", "dx", "dy", "dz" };
static char *cpRiScale[] = { "dx", "dy", "dz" };
static char *cpRiScreenWindow[] = { "left", "right", "bottom", "top" };
static char *cpRiShadingInterpolation[] = { "type" };
static char *cpRiShadingRate[] = { "size" };
static char *cpRiShutter[] = { "min", "max" };
static char *cpRiSides[] = { "sides" };
static char *cpRiSkew[] = { "angle", "dx1","dy1","dz1", "dx2","dy2","dz2" };
static char *cpRiSolidBegin[] = { "operation" };
static char *cpRiSolidEnd[] = { NULL };
static char *cpRiSphereV[] = { "radius", "zmin", "zmax", "thetamax" };
static char *cpRiSurfaceV[] = { "name" };
static char *cpRiTextureCoordinates[] = { "s1", "t1", "s2", "t2",
                                          "s3", "t3", "s4", "t4" };
static char *cpRiTorusV[] = { "majorradius", "minorradius",
                             "phimin", "phimax", "thetamax" };
static char *cpRiTransform[] = { "transform" };
static char *cpRiTransformBegin[] = { NULL };
static char *cpRiTransformEnd[] = { NULL };
static char *cpRiTranslate[] = { "dx", "dy", "dz" };
static char *cpRiTrimCurve[] = { "nloops", "ncurves", "order",
                                 "knot", "min", "max", "n", "u", "v", "w" };
static char *cpRiWorldBegin[] = { NULL };
static char *cpRiWorldEnd[] = { NULL };
static char *cpRiBegin[] = { "name" };
static char *cpRiEnd[] = { NULL };
static char *cpRiArchiveRecord[] = { "type", "format", "s" };
static char *cpRiProcedural[] = { "data", "bound",
                                  "subdivfunc", "freefunc" };
static char *cpRiTransformPoints[] = { "fromspace", "tospace", "n", "points" };
static char *cpRiErrorIgnore[] = { "code", "severity", "msg" };
static char *cpRiErrorPrint[] = { "code", "severity", "msg" };
static char *cpRiErrorAbort[] = { "code", "severity", "msg" };
static char *cpRiBoxFilter[] = { "x", "y", "xwidth", "ywidth" };
static char *cpRiTriangleFilter[] = { "x", "y", "xwidth", "ywidth" };
static char *cpRiCatmullRomFilter[] = { "x", "y", "xwidth", "ywidth" };
static char *cpRiGaussianFilter[] = { "x", "y", "xwidth", "ywidth" };
static char *cpRiSincFilter[] = { "x", "y", "xwidth", "ywidth" };
static char *cpRiProcDelayedReadArchive[] = { "data", "detail" };
static char *cpRiProcRunProgram[] = { "data", "detail" };
static char *cpRiProcDynamicLoad[] = { "data", "detail" };
static char *cpRi_version[] = { "version" };


char **gRibCPNamesTable[] = {
   cpRiAreaLightSourceV,
   cpRiAtmosphereV,
   cpRiAttributeV,
   cpRiAttributeBegin,
   cpRiAttributeEnd,
   cpRiBasis,
   cpRiBound,
   cpRiClipping,
   cpRiColor,
   cpRiColorSamples,
   cpRiConcatTransform,
   cpRiConeV,
   cpRiCoordinateSystem,
   cpRiCoordSysTransform,
   cpRiCropWindow,
   cpRiCurvesV,
   cpRiCylinderV,
   cpRiDeclare,
   cpRiDeformationV,
   cpRiDepthOfField,
   cpRiDetail,
   cpRiDetailRange,
   cpRiDiskV,
   cpRiDisplacementV,
   cpRiDisplayV,
   cpRiErrorHandler,
   cpRiExposure,
   cpRiExteriorV,
   cpRiFormat,
   cpRiFrameAspectRatio,
   cpRiFrameBegin,
   cpRiFrameEnd,
   cpRiGeneralPolygonV,
   cpRiGeometricApproximation,
   cpRiGeometricRepresentation,
   cpRiGeometryV,
   cpRiHiderV,
   cpRiHyperboloidV,
   cpRiIdentity,
   cpRiIlluminate,
   cpRiImagerV,
   cpRiInteriorV,
   cpRiLightSourceV,
   cpRiMakeBumpV,
   cpRiMakeCubeFaceEnvironmentV,
   cpRiMakeLatLongEnvironmentV,
   cpRiMakeShadowV,
   cpRiMakeTextureV,
   cpRiMatte,
   cpRiMotionBeginV,
   cpRiMotionEnd,
   cpRiNuPatchV,
   cpRiObjectBegin,
   cpRiObjectEnd,
   cpRiObjectInstance,
   cpRiOpacity,
   cpRiOptionV,
   cpRiOrientation,
   cpRiParaboloidV,
   cpRiPatchV,
   cpRiPatchMeshV,
   cpRiPerspective,
   cpRiPixelFilter,
   cpRiPixelSamples,
   cpRiPixelVariance,
   cpRiPointsV,
   cpRiPointsGeneralPolygonsV,
   cpRiPointsPolygonsV,
   cpRiPolygonV,
   cpRiProjectionV,
   cpRiQuantize,
   cpRiReadArchiveV,
   cpRiRelativeDetail,
   cpRiReverseOrientation,
   cpRiRotate,
   cpRiScale,
   cpRiScreenWindow,
   cpRiShadingInterpolation,
   cpRiShadingRate,
   cpRiShutter,
   cpRiSides,
   cpRiSkew,
   cpRiSolidBegin,
   cpRiSolidEnd,
   cpRiSphereV,
   cpRiSurfaceV,
   cpRiTextureCoordinates,
   cpRiTorusV,
   cpRiTransform,
   cpRiTransformBegin,
   cpRiTransformEnd,
   cpRiTranslate,
   cpRiTrimCurve,
   cpRiWorldBegin,
   cpRiWorldEnd,
   cpRi_version,
   /* No RIB statements for the following: */
   cpRiBegin,
   cpRiEnd,
   cpRiArchiveRecord,
   cpRiProcedural,
   cpRiTransformPoints,
   cpRiErrorAbort,
   cpRiErrorIgnore,
   cpRiErrorPrint,
   cpRiBoxFilter,
   cpRiCatmullRomFilter,
   cpRiGaussianFilter,
   cpRiSincFilter,
   cpRiTriangleFilter,
   cpRiProcDelayedReadArchive,
   cpRiProcRunProgram,
   cpRiProcDynamicLoad
};
