/* $RCSfile: names.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 06:30:52 $
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
 * FILE:  names.c
 *
 * DESCRIPTION:  
 *   
 *    References:
 *          [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, 
 *                     Richmond, CA, September 1989.
 *
 *    History:
 *       04-27-99  Added NuCurves, Implicit, Resource.
 *   
 *
 * 
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_NAMES

static char scRibAreaLightSource[] =         "AreaLightSource";
static char scRibAtmosphere[] =              "Atmosphere";
static char scRibAttribute[] =               "Attribute";
static char scRibAttributeBegin[] =          "AttributeBegin";
static char scRibAttributeEnd[] =            "AttributeEnd";
static char scRibBasis[] =                   "Basis";
static char scRibBound[] =                   "Bound";
static char scRibClipping[] =                "Clipping";
static char scRibColor[] =                   "Color";
static char scRibColorSamples[] =            "ColorSamples";
static char scRibConcatTransform[] =         "ConcatTransform";
static char scRibCone[] =                    "Cone";
static char scRibCoordinateSystem[] =        "CoordinateSystem";
static char scRibCoordSysTransform[] =       "CoordSysTransform";
static char scRibCropWindow[] =              "CropWindow";
static char scRibCurves[] =                  "Curves";
static char scRibCylinder[] =                "Cylinder";
static char scRibDeclare[] =                 "Declare";
static char scRibDeformation[] =             "Deformation";
static char scRibDepthOfField[] =            "DepthOfField";
static char scRibDetail[] =                  "Detail";
static char scRibDetailRange[] =             "DetailRange";
static char scRibDisk[] =                    "Disk";
static char scRibDisplacement[] =            "Displacement";
static char scRibDisplay[] =                 "Display";
static char scRibErrorHandler[] =            "ErrorHandler";
static char scRibExposure[] =                "Exposure";
static char scRibExterior[] =                "Exterior";
static char scRibFormat[] =                  "Format";
static char scRibFrameAspectRatio[] =        "FrameAspectRatio";
static char scRibFrameBegin[] =              "FrameBegin";
static char scRibFrameEnd[] =                "FrameEnd";
static char scRibGeneralPolygon[] =          "GeneralPolygon";
static char scRibGeometricApproximation[] =  "GeometricApproximation";
static char scRibGeometricRepresentation[] = "GeometricRepresentation";
static char scRibGeometry[] =                "Geometry";
static char scRibHider[] =                   "Hider";
static char scRibHyperboloid[] =             "Hyperboloid";
static char scRibIdentity[] =                "Identity";
static char scRibIlluminate[] =              "Illuminate";
static char scRibImager[] =                  "Imager";
static char scRibImplicit[] =                "Implicit";
static char scRibInterior[] =                "Interior";
static char scRibLightSource[] =             "LightSource";
static char scRibMakeBump[] =                "MakeBump";
static char scRibMakeCubeFaceEnvironment[] = "MakeCubeFaceEnvironment";
static char scRibMakeLatLongEnvironment[] =  "MakeLatLongEnvironment";
static char scRibMakeShadow[] =              "MakeShadow";
static char scRibMakeTexture[] =             "MakeTexture";
static char scRibMatte[] =                   "Matte";
static char scRibMotionBegin[] =             "MotionBegin";
static char scRibMotionEnd[] =               "MotionEnd";
static char scRibNuCurves[] =                "NuCurves";
static char scRibNuPatch[] =                 "NuPatch";
static char scRibObjectBegin[] =             "ObjectBegin";
static char scRibObjectEnd[] =               "ObjectEnd";
static char scRibObjectInstance[] =          "ObjectInstance";
static char scRibOpacity[] =                 "Opacity";
static char scRibOption[] =                  "Option";
static char scRibOrientation[] =             "Orientation";
static char scRibParaboloid[] =              "Paraboloid";
static char scRibPatch[] =                   "Patch";
static char scRibPatchMesh[] =               "PatchMesh";
static char scRibPerspective[] =             "Perspective";
static char scRibPixelFilter[] =             "PixelFilter";
static char scRibPixelSamples[] =            "PixelSamples";
static char scRibPixelVariance[] =           "PixelVariance";
static char scRibPoints[] =                  "Points";
static char scRibPointsGeneralPolygons[] =   "PointsGeneralPolygons";
static char scRibPointsPolygons[] =          "PointsPolygons";
static char scRibPolygon[] =                 "Polygon";
static char scRibProcedural[] =              "Procedural";
static char scRibProjection[] =              "Projection";
static char scRibQuantize[] =                "Quantize";
static char scRibReadArchive[] =             "ReadArchive";
static char scRibRelativeDetail[] =          "RelativeDetail";
static char scRibResource[] =                "Resource";
static char scRibReverseOrientation[] =      "ReverseOrientation"; 
static char scRibRotate[] =                  "Rotate";
static char scRibScale[] =                   "Scale";
static char scRibScreenWindow[] =            "ScreenWindow";
static char scRibShadingInterpolation[] =    "ShadingInterpolation";
static char scRibShadingRate[] =             "ShadingRate";
static char scRibShutter[] =                 "Shutter";
static char scRibSides[] =                   "Sides";
static char scRibSkew[] =                    "Skew";
static char scRibSolidBegin[] =              "SolidBegin";
static char scRibSolidEnd[] =                "SolidEnd";
static char scRibSphere[] =                  "Sphere";
static char scRibSubdivisionMesh[] =         "SubdivisionMesh";
static char scRibSurface[] =                 "Surface";
static char scRibTextureCoordinates[] =      "TextureCoordinates";
static char scRibTorus[] =                   "Torus";
static char scRibTransform[] =               "Transform";
static char scRibTransformBegin[] =          "TransformBegin";
static char scRibTransformEnd[] =            "TransformEnd";
static char scRibTranslate[] =               "Translate";
static char scRibTrimCurve[] =               "TrimCurve";
static char scRibWorldBegin[] =              "WorldBegin";
static char scRibWorldEnd[] =                "WorldEnd";
static char scRibversion[] =                 "version";
static char scRibBegin[] =                   "Begin";
static char scRibEnd[] =                     "End";
static char scRibArchiveRecord[] =           "ArchiveRecord";
static char scRibTransformPoints[] =         "TransformPoints";
static char scRibErrorAbort[] =              "ErrorAbort"; 
static char scRibErrorIgnore[] =             "ErrorIgnore";
static char scRibErrorPrint[] =              "ErrorPrint";
static char scRibBoxFilter[] =               "BoxFilter";
static char scRibCatmullRomFilter[] =        "CatmullRomFilter";
static char scRibGaussianFilter[] =          "GaussianFilter";
static char scRibSincFilter[] =              "SincFilter";
static char scRibTriangleFilter[] =          "TriangleFilter";
static char scRibProcDelayedReadArchive[] =  "ProcDelayedReadArchive";
static char scRibProcDynamicLoad[] =         "ProcDynamicLoad";
static char scRibProcRunProgram[] =          "ProcRunProgram";


char *gRibNames[] = {
   scRibAreaLightSource,
   scRibAtmosphere,
   scRibAttribute,
   scRibAttributeBegin,
   scRibAttributeEnd,
   scRibBasis,
   scRibBound,
   scRibClipping,
   scRibColor,
   scRibColorSamples,
   scRibConcatTransform,
   scRibCone,
   scRibCoordinateSystem,
   scRibCoordSysTransform,
   scRibCropWindow,
   scRibCurves,
   scRibCylinder,
   scRibDeclare,
   scRibDeformation,
   scRibDepthOfField,
   scRibDetail,
   scRibDetailRange,
   scRibDisk,
   scRibDisplacement,
   scRibDisplay,
   scRibErrorHandler,
   scRibExposure,
   scRibExterior,
   scRibFormat,
   scRibFrameAspectRatio,
   scRibFrameBegin,
   scRibFrameEnd,
   scRibGeneralPolygon,
   scRibGeometricApproximation,
   scRibGeometricRepresentation,
   scRibGeometry,
   scRibHider,
   scRibHyperboloid,
   scRibIdentity,
   scRibIlluminate,
   scRibImager,
   scRibImplicit,
   scRibInterior,
   scRibLightSource,
   scRibMakeBump,
   scRibMakeCubeFaceEnvironment,
   scRibMakeLatLongEnvironment,
   scRibMakeShadow,
   scRibMakeTexture,
   scRibMatte,
   scRibMotionBegin,
   scRibMotionEnd,
   scRibNuCurves,
   scRibNuPatch,
   scRibObjectBegin,
   scRibObjectEnd,
   scRibObjectInstance,
   scRibOpacity,
   scRibOption,
   scRibOrientation,
   scRibParaboloid,
   scRibPatch,
   scRibPatchMesh,
   scRibPerspective,
   scRibPixelFilter,
   scRibPixelSamples,
   scRibPixelVariance,
   scRibPoints,
   scRibPointsGeneralPolygons,
   scRibPointsPolygons,
   scRibPolygon,
   scRibProcedural,
   scRibProjection,
   scRibQuantize,
   scRibReadArchive,
   scRibRelativeDetail,
   scRibResource,
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
   scRibSphere,
   scRibSubdivisionMesh,
   scRibSurface,
   scRibTextureCoordinates,
   scRibTorus,
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
