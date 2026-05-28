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
 * FILE:  ribnop.c
 *
 * DESCRIPTION:  
 *
 *    Contains:  
 * 
 *    References:
 *        [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, Richmond, CA, 
 *                  September 1989.
 *
 *    History:
 *       10-12-98  Added RiNopSubdivisionMeshV.
 *       12-05-98  Added RiNopGeometricRepresentation and RiNopResourceV.
 *       04-27-99  Added RiNopNuCurvesV() and RiNopImplicitV().
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 */
#define  __RIB_FILE_RIBNOP
#include <ributil.h>
#include <ribnop.h>


/* 
 *   Table to include in RibOpen() call unless you make your own.
 */
PRIB_RIPROC gRibNopRITable[] = {
   (PRIB_RIPROC)RiNopAreaLightSourceV,
   (PRIB_RIPROC)RiNopAtmosphereV,
   (PRIB_RIPROC)RiNopAttributeV,
   (PRIB_RIPROC)RiNopAttributeBegin,
   (PRIB_RIPROC)RiNopAttributeEnd,
   (PRIB_RIPROC)RiNopBasis,
   (PRIB_RIPROC)RiNopBound,
   (PRIB_RIPROC)RiNopClipping,
   (PRIB_RIPROC)RiNopColor,
   (PRIB_RIPROC)RiNopColorSamples,
   (PRIB_RIPROC)RiNopConcatTransform,
   (PRIB_RIPROC)RiNopConeV,
   (PRIB_RIPROC)RiNopCoordinateSystem,
   (PRIB_RIPROC)RiNopCoordSysTransform,
   (PRIB_RIPROC)RiNopCropWindow,
   (PRIB_RIPROC)RiNopCurvesV,
   (PRIB_RIPROC)RiNopCylinderV,
   (PRIB_RIPROC)RiNopDeclare,
   (PRIB_RIPROC)RiNopDeformationV,
   (PRIB_RIPROC)RiNopDepthOfField,
   (PRIB_RIPROC)RiNopDetail,
   (PRIB_RIPROC)RiNopDetailRange,
   (PRIB_RIPROC)RiNopDiskV,
   (PRIB_RIPROC)RiNopDisplacementV,
   (PRIB_RIPROC)RiNopDisplayV,
   (PRIB_RIPROC)RiNopErrorHandler,
   (PRIB_RIPROC)RiNopExposure,
   (PRIB_RIPROC)RiNopExteriorV,
   (PRIB_RIPROC)RiNopFormat,
   (PRIB_RIPROC)RiNopFrameAspectRatio,
   (PRIB_RIPROC)RiNopFrameBegin,
   (PRIB_RIPROC)RiNopFrameEnd,
   (PRIB_RIPROC)RiNopGeneralPolygonV,
   (PRIB_RIPROC)RiNopGeometricApproximation,
   (PRIB_RIPROC)RiNopGeometricRepresentation,
   (PRIB_RIPROC)RiNopGeometryV,
   (PRIB_RIPROC)RiNopHiderV,
   (PRIB_RIPROC)RiNopHyperboloidV,
   (PRIB_RIPROC)RiNopIdentity,
   (PRIB_RIPROC)RiNopIlluminate,
   (PRIB_RIPROC)RiNopImagerV,
   (PRIB_RIPROC)RiNopImplicitV,
   (PRIB_RIPROC)RiNopInteriorV,
   (PRIB_RIPROC)RiNopLightSourceV,
   (PRIB_RIPROC)RiNopMakeBumpV,
   (PRIB_RIPROC)RiNopMakeCubeFaceEnvironmentV,
   (PRIB_RIPROC)RiNopMakeLatLongEnvironmentV,
   (PRIB_RIPROC)RiNopMakeShadowV,
   (PRIB_RIPROC)RiNopMakeTextureV,
   (PRIB_RIPROC)RiNopMatte,
   (PRIB_RIPROC)RiNopMotionBeginV,
   (PRIB_RIPROC)RiNopMotionEnd,
   (PRIB_RIPROC)RiNopNuCurvesV,
   (PRIB_RIPROC)RiNopNuPatchV,
   (PRIB_RIPROC)RiNopObjectBegin,
   (PRIB_RIPROC)RiNopObjectEnd,
   (PRIB_RIPROC)RiNopObjectInstance,
   (PRIB_RIPROC)RiNopOpacity,
   (PRIB_RIPROC)RiNopOptionV,
   (PRIB_RIPROC)RiNopOrientation,
   (PRIB_RIPROC)RiNopParaboloidV,
   (PRIB_RIPROC)RiNopPatchV,
   (PRIB_RIPROC)RiNopPatchMeshV,
   (PRIB_RIPROC)RiNopPerspective,
   (PRIB_RIPROC)RiNopPixelFilter,
   (PRIB_RIPROC)RiNopPixelSamples,
   (PRIB_RIPROC)RiNopPixelVariance,
   (PRIB_RIPROC)RiNopPointsV,
   (PRIB_RIPROC)RiNopPointsGeneralPolygonsV,
   (PRIB_RIPROC)RiNopPointsPolygonsV,
   (PRIB_RIPROC)RiNopPolygonV,
   (PRIB_RIPROC)RiNopProcedural,
   (PRIB_RIPROC)RiNopProjectionV,
   (PRIB_RIPROC)RiNopQuantize,
   (PRIB_RIPROC)RiNopReadArchiveV,
   (PRIB_RIPROC)RiNopRelativeDetail,
   (PRIB_RIPROC)RiNopResourceV,
   (PRIB_RIPROC)RiNopReverseOrientation,
   (PRIB_RIPROC)RiNopRotate,
   (PRIB_RIPROC)RiNopScale,
   (PRIB_RIPROC)RiNopScreenWindow,
   (PRIB_RIPROC)RiNopShadingInterpolation,
   (PRIB_RIPROC)RiNopShadingRate,
   (PRIB_RIPROC)RiNopShutter,
   (PRIB_RIPROC)RiNopSides,
   (PRIB_RIPROC)RiNopSkew,
   (PRIB_RIPROC)RiNopSolidBegin,
   (PRIB_RIPROC)RiNopSolidEnd,
   (PRIB_RIPROC)RiNopSphereV,
   (PRIB_RIPROC)RiNopSubdivisionMeshV,
   (PRIB_RIPROC)RiNopSurfaceV,
   (PRIB_RIPROC)RiNopTextureCoordinates,
   (PRIB_RIPROC)RiNopTorusV,
   (PRIB_RIPROC)RiNopTransform,
   (PRIB_RIPROC)RiNopTransformBegin,
   (PRIB_RIPROC)RiNopTransformEnd,
   (PRIB_RIPROC)RiNopTranslate,
   (PRIB_RIPROC)RiNopTrimCurve,
   (PRIB_RIPROC)RiNopWorldBegin,
   (PRIB_RIPROC)RiNopWorldEnd,
   (PRIB_RIPROC)RiNop_version,
/* No RIB statements for the following: */
   (PRIB_RIPROC)RiNopBegin,
   (PRIB_RIPROC)RiNopEnd,
   (PRIB_RIPROC)RiNopArchiveRecord,
   (PRIB_RIPROC)RiNopTransformPoints,
   (PRIB_RIPROC)RiNopErrorAbort,
   (PRIB_RIPROC)RiNopErrorIgnore,
   (PRIB_RIPROC)RiNopErrorPrint,
   (PRIB_RIPROC)RiNopBoxFilter,
   (PRIB_RIPROC)RiNopCatmullRomFilter,
   (PRIB_RIPROC)RiNopGaussianFilter,
   (PRIB_RIPROC)RiNopSincFilter,
   (PRIB_RIPROC)RiNopTriangleFilter,
   (PRIB_RIPROC)RiNopProcDelayedReadArchive,
   (PRIB_RIPROC)RiNopProcRunProgram,
   (PRIB_RIPROC)RiNopProcDynamicLoad
};
static unsigned int LastLightHandle = 1;
static unsigned int LastObjectHandle = 1;


RtLightHandle RiNopAreaLightSourceV( RtToken name, 
			     RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)name; (void)n; (void)tokens; (void)parms;
   
   /* Fake it. */
   return (RtLightHandle)LastLightHandle++;
}


RtVoid RiNopAtmosphereV( RtToken name, 
                      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopAttributeV( RtToken name,
                     RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopAttributeBegin( void )
{ 
}


RtVoid RiNopAttributeEnd( void )
{
}


RtVoid RiNopBasis( RtBasis ubasis, RtInt ustep, RtBasis vbasis, RtInt vstep )
{
   (void)ubasis; (void)ustep; (void)vbasis; (void)vstep; 
}


RtVoid RiNopBound( RtBound bound )
{ 
   (void)bound;
}


RtVoid RiNopClipping( RtFloat hither, RtFloat yon )
{ 
   (void)hither; (void)yon; 
}


RtVoid RiNopColor( RtColor color )
{ 
   (void)color;
}


RtVoid RiNopColorSamples( RtInt n, RtFloat nRGB[], RtFloat RGBn[] )
{ 
   (void)n; (void)nRGB; (void)RGBn;
}


RtVoid RiNopConcatTransform( RtMatrix transform)
{ 
   (void)transform;
}


RtVoid RiNopConeV( RtFloat height, RtFloat radius, RtFloat thetamax,
	       RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)height; (void)radius; (void)thetamax; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopCoordinateSystem( RtToken space )
{ 
   (void)space; 
}


RtVoid RiNopCoordSysTransform( RtToken space )
{ 
   (void)space; 
}


RtVoid RiNopCropWindow( RtFloat xmin, RtFloat xmax, 
		    RtFloat ymin, RtFloat ymax )
{ 
   (void)xmin; (void)xmax; (void)ymin; (void)ymax; 
}


RtVoid RiNopCurves( RtToken type, RtInt ncurves, RtInt nvertices[], 
            RtToken wrap, ... )
{
   (void)type; (void)ncurves; (void)nvertices; (void)wrap;
}


RtVoid RiNopCurvesV( RtToken type, RtInt ncurves, RtInt nvertices[], 
             RtToken wrap, 
             RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)type; (void)ncurves; (void)nvertices; (void)wrap;
   (void)n; (void)tokens; (void)parms; 
}


RtVoid RiNopCylinderV( RtFloat radius, RtFloat zmin, RtFloat zmax,
                    RtFloat thetamax,
                    RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)radius; (void)zmin; (void)zmax; (void)thetamax; 
   (void)n; (void)tokens; (void)parms; 
}


RtToken RiNopDeclare( char *name, char *declaration )
{
   (void)declaration;

   return name;
}


RtVoid RiNopDeformationV( RtToken name,
		      RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopDepthOfField( RtFloat fstop, RtFloat focallength, 
		      RtFloat focaldistance )
{ 
   (void)fstop; (void)focallength; (void)focaldistance;
}


RtVoid RiNopDetail( RtBound bound )
{ 
   (void)bound;
}


RtVoid RiNopDetailRange( RtFloat minvisible, RtFloat lowertransition, 
		     RtFloat uppertransition, RtFloat maxvisible )
{ 
   (void)minvisible; (void)lowertransition; 
   (void)uppertransition; (void)maxvisible;
}


RtVoid RiNopDiskV( RtFloat height, RtFloat radius, RtFloat thetamax,
                RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)height; (void)radius; (void)thetamax;
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopDisplacementV( RtToken name,
		       RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopDisplayV( char *name, RtToken type, RtToken mode, 
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)type; (void)mode; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopErrorHandler( RtErrorHandler handler )
{ 
   (void)handler;
}


RtVoid RiNopExposure( RtFloat gain, RtFloat gamma )
{
   (void)gain; (void)gamma;
}


RtVoid RiNopExteriorV( RtToken name, 
		   RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopFormat( RtInt xres, RtInt yres, RtFloat aspect )
{ 
   (void)xres; (void)yres; (void)aspect;
}


RtVoid RiNopFrameAspectRatio( RtFloat aspect )
{
   (void)aspect; 
}


RtVoid RiNopFrameBegin( RtInt frame )
{ 
   (void)frame;
}


RtVoid RiNopFrameEnd( void )
{ 
}


RtVoid RiNopGeneralPolygonV( RtInt nloops, RtInt nvertices[],
                          RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)nloops; (void)nvertices; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopGeometricApproximation( RtToken type, RtFloat value )
{ 
   (void)type; (void)value;
}


RtVoid RiNopGeometricRepresentation( RtToken type )
{ 
   (void)type;
}


RtVoid RiNopGeometryV( RtToken type, 
		   RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)type; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopHiderV( RtToken type,
		RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)type; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopHyperboloidV( RtPoint point1, RtPoint point2, RtFloat thetamax,
		      RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)point1; (void)point2; (void)thetamax;
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopIdentity( void )
{ 
}


RtVoid RiNopIlluminate( RtLightHandle light, RtBoolean onoff )
{ 
   (void)light; (void)onoff;
}


RtVoid RiNopImagerV( RtToken name,
                 RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopImplicitV( RtInt a, RtInt b[], RtInt c, RtFloat d[],
		       RtInt e, RtFloat f[], RtInt g, RtFloat h[],
		       RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; (void)g; (void)h;
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopInteriorV( RtToken name, 
		   RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtLightHandle RiNopLightSourceV( RtToken name, 
                              RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)name; (void)n; (void)tokens; (void)parms;

   /* Fake it. */
   return (RtLightHandle)LastLightHandle++;
}


RtVoid RiNopMakeBumpV( char *picturename, char *texturename, 
		   RtToken swrap, RtToken twrap,
		   RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
		   RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)picturename; (void)texturename; 
   (void)swrap; (void)twrap; 
   (void)filterfunc; (void)swidth; (void)twidth; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopMakeCubeFaceEnvironmentV( char *px, char *nx, char *py, char *ny,
				  char *pz, char *nz, char *texturename, 
				  RtFloat fov,
				  RtFilterFunc filterfunc, 
				  RtFloat swidth, RtFloat twidth,
				  RtInt n, 
				  RtToken tokens[], RtPointer parms[] )
{ 
   (void)px; (void)nx; (void)py; (void)ny; 
   (void)pz; (void)nz; (void)texturename; 
   (void)fov; 
   (void)filterfunc; 
   (void)swidth; (void)twidth; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopMakeLatLongEnvironmentV( char *picturename, char *texturename, 
				 RtFilterFunc filterfunc,
				 RtFloat swidth, RtFloat twidth,
				 RtInt n, 
				 RtToken tokens[], RtPointer parms[] )
{ 
   (void)picturename; (void)texturename; 
   (void)filterfunc; 
   (void)swidth; (void)twidth; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopMakeShadowV( char *picturename, char *texturename,
		     RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)picturename; (void)texturename; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopMakeTextureV( char *picturename, char *texturename, 
		      RtToken swrap, RtToken twrap,
		      RtFilterFunc filterfunc, 
		      RtFloat swidth, RtFloat twidth,
		      RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)picturename; (void)texturename; 
   (void)swrap; (void)twrap; 
   (void)filterfunc; 
   (void)swidth; (void)twidth; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopMatte( RtBoolean onoff )
{ 
   (void)onoff;
}


RtVoid RiNopMotionBeginV( RtInt n, RtFloat times[] )
{ 
   (void)n; (void)times;
}


RtVoid RiNopMotionEnd( void )
{ 
}


RtVoid RiNopNuCurvesV( RtInt ncurves, RtInt nvertices[], RtInt order[],
		       RtFloat knot[], RtFloat min[], RtFloat max[], 
		       RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)ncurves; (void)nvertices; (void)order; (void)knot; 
   (void)min; (void)max; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
		  RtFloat umin, RtFloat umax, 
		  RtInt nv, RtInt vorder, RtFloat vknot[],
		  RtFloat vmin, RtFloat vmax,
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)nu; (void)uorder; (void)uknot; (void)umin; (void)umax; 
   (void)nv; (void)vorder; (void)vknot; (void)vmin; (void)vmax; 
   (void)n; (void)tokens; (void)parms;
}


RtObjectHandle RiNopObjectBegin( void )
{
   /* Fake it. */
   return (RtObjectHandle)LastObjectHandle++;
}


RtVoid RiNopObjectEnd( void )
{ 
}


RtVoid RiNopObjectInstance( RtObjectHandle handle )
{ 
   (void)handle;
}


RtVoid RiNopOpacity( RtColor color)
{ 
   (void)color;
}


RtVoid RiNopOptionV( RtToken name, 
		 RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopOrientation( RtToken orientation )
{ 
   (void)orientation;
}


RtVoid RiNopParaboloidV( RtFloat rmax, 
		     RtFloat zmin, RtFloat zmax, RtFloat thetamax,
		     RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)rmax; (void)zmin; (void)zmax; (void)thetamax;
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopPatchV( RtToken type, 
		RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)type; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopPatchMeshV( RtToken type, RtInt nu, RtToken uwrap, 
		    RtInt nv, RtToken vwrap, 
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)type; (void)nu; (void)uwrap; (void)nv; (void)vwrap; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopPerspective( RtFloat fov )
{
   (void)fov;
}


RtVoid RiNopPixelFilter( RtFilterFunc filterfunc, 
		     RtFloat xwidth, RtFloat ywidth )
{ 
   (void)filterfunc; (void)xwidth; (void)ywidth;
}


RtVoid RiNopPixelSamples( RtFloat xsamples, RtFloat ysamples )
{ 
   (void)xsamples; (void)ysamples;
}


RtVoid RiNopPixelVariance( RtFloat variation )
{ 
   (void)variation;
}


RtVoid RiNopPointsV( RtInt npoints, 
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)npoints;
   (void)n; (void)tokens; (void)parms; 
}


RtVoid RiNopPointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
				RtInt nvertices[], RtInt vertices[], 
				RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)npolys; (void)nloops; (void)nvertices; (void)vertices; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopPointsPolygonsV( RtInt npolys, RtInt nvertices[], RtInt vertices[],
			 RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)npolys; (void)nvertices; (void)vertices; 
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopPolygonV( RtInt nvertices,
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)nvertices; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopProjectionV( RtToken name, 
		     RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopQuantize( RtToken type, RtInt one, 
		  RtInt min, RtInt max, RtFloat ampl )
{ 
   (void)type; (void)one; (void)min; (void)max; (void)ampl;
}


RtVoid RiNopReadArchiveV( RtToken name, 
                       RtVoid (*callback)( RtToken, char*, char* ),
                       RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)callback;
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopRelativeDetail( RtFloat relativedetail )
{ 
   (void)relativedetail;
}


RtVoid RiNopResourceV( RtToken handle, RtToken type,
		RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)handle; (void)type;
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopReverseOrientation( void )
{ 
}


RtVoid RiNopRotate( RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz )
{ 
   (void)angle; (void)dx; (void)dy; (void)dz; 
}


RtVoid RiNopScale( RtFloat dx, RtFloat dy, RtFloat dz )
{ 
   (void)dx; (void)dy; (void)dz;
}


RtVoid RiNopScreenWindow( RtFloat left, RtFloat right, 
		      RtFloat bottom, RtFloat top )
{ 
   (void)left; (void)right; (void)bottom; (void)top;
}


RtVoid RiNopShadingInterpolation( RtToken type )
{ 
   (void)type;
}


RtVoid RiNopShadingRate( RtFloat size )
{ 
   (void)size;
}


RtVoid RiNopShutter( RtFloat min, RtFloat max )
{ 
   (void)min; (void)max;
}


RtVoid RiNopSides( RtInt sides )
{ 
   (void)sides;
}


RtVoid RiNopSkew( RtFloat angle, RtFloat dx1, RtFloat dy1, RtFloat dz1,
	      RtFloat dx2, RtFloat dy2, RtFloat dz2 )
{ 
   (void)angle; 
   (void)dx1; (void)dy1; (void)dz1; (void)dx2; (void)dy2; (void)dz2;
}


RtVoid RiNopSolidBegin( RtToken operation )
{ 
   (void)operation;
}


RtVoid RiNopSolidEnd( void )
{ 
}


RtVoid RiNopSphereV( RtFloat radius, RtFloat zmin, RtFloat zmax, 
		 RtFloat thetamax,
		 RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)radius; (void)zmin; (void)zmax; (void)thetamax;
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopSurfaceV( RtToken name, 
                   RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)name; (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopSubdivisionMeshV( RtToken scheme, RtInt nfaces, 
			      RtInt nvertices[], RtInt vertices[],
			      RtInt ntags, RtToken tags[],
			      RtInt nargs[], 
			      RtInt intargs[], RtFloat floatargs[],
			      RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)scheme;  (void)nfaces;
   (void)nvertices; (void)vertices;
   (void)ntags; (void)tags;
   (void)nargs; (void)intargs; (void)floatargs;
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopTextureCoordinates( RtFloat s1, RtFloat t1, 
                             RtFloat s2, RtFloat t2,
                             RtFloat s3, RtFloat t3, 
                             RtFloat s4, RtFloat t4 )
{ 
   (void)s1; (void)t1; 
   (void)s2; (void)t2; 
   (void)s3; (void)t3; 
   (void)s4; (void)t4;
}


RtVoid RiNopTorusV( RtFloat majorradius, RtFloat minorradius, 
                 RtFloat phimin, RtFloat phimax, RtFloat thetamax, 
                 RtInt n, RtToken tokens[], RtPointer parms[] )
{ 
   (void)majorradius; (void)minorradius; 
   (void)phimin; (void)phimax;
   (void)thetamax;
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiNopTransform( RtMatrix transform )
{ 
   (void)transform;
}


RtVoid RiNopTransformBegin( void )
{ 
}


RtVoid RiNopTransformEnd( void )
{ 
}


RtVoid RiNopTranslate( RtFloat dx, RtFloat dy, RtFloat dz )
{ 
   (void)dx; (void)dy; (void)dz;
}


RtVoid RiNopTrimCurve( RtInt nloops, RtInt ncurves[], RtInt order[], 
		   RtFloat knot[], RtFloat min[], RtFloat max[], 
		   RtInt n[], RtFloat u[], RtFloat v[], RtFloat w[] )
{
   (void)nloops; (void)ncurves; (void)order; 
   (void)knot; (void)min; (void)max; 
   (void)n; (void)u; (void)v; (void)w;
}


RtVoid RiNopWorldBegin( void )
{
}


RtVoid RiNopWorldEnd( void )
{ 
}


RtVoid RiNopBegin( RtToken name )
{ 
   (void)name;

   LastObjectHandle = 1;
   LastLightHandle = 1;
}


RtVoid RiNopEnd( void )
{ 
}


RtVoid RiNopArchiveRecord( RtToken type, char *format, char *s )
{ 
   (void)type; (void)format; (void)s;
}


RtVoid RiNopProcedural( RtPointer data, RtBound bound,
		       RtVoid (*subdivfunc)(RtPointer, RtFloat),
		       RtVoid (*freefunc)(RtPointer) )
{
   (void)data; (void)bound; (void)subdivfunc; (void)freefunc;

   return;
}


RtPoint* RiNopTransformPoints( RtToken fromspace, RtToken tospace,
			      RtInt n, RtPoint points[] )
{
   (void)fromspace; (void)tospace; (void)n; (void)points;

   return NULL;
}


RtVoid RiNopErrorIgnore( RtInt code, RtInt severity, char *msg )
{
   (void)code; (void)severity; (void)msg;

   return;
}


RtVoid RiNopErrorPrint( RtInt code, RtInt severity, char *msg )
{
   (void)code; (void)severity; (void)msg;

   return;
}

RtVoid RiNopErrorAbort( RtInt code, RtInt severity, char *msg )
{
   (void)code; (void)severity; (void)msg;

   return;
}


RtFloat RiNopBoxFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtFloat RiNopTriangleFilter( RtFloat x, RtFloat y, 
			 RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtFloat RiNopCatmullRomFilter( RtFloat x, RtFloat y, 
			   RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtFloat RiNopGaussianFilter( RtFloat x, RtFloat y, 
			 RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtFloat RiNopSincFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtVoid RiNop_version( RtFloat version )
{
   (void)version;
}


RtVoid RiNopProcDelayedReadArchive( RtPointer data, RtFloat detail )
{
   (void)data;
   (void)detail;
}


RtVoid RiNopProcRunProgram( RtPointer data, RtFloat detail )
{
   (void)data;
   (void)detail;
}

RtVoid RiNopProcDynamicLoad( RtPointer data, RtFloat detail )
{
   (void)data;
   (void)detail;
}
