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
 * Affine Toolkit
 *
 * FILE:  ripriv.h
 *
 * DESCRIPTION:  Private include file to handle Ri extern definitions.
 *
 *     References:
 *          [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, 
 *                    Richmond, CA, September 1989.
 *
 *     History:
 *                Incorporated changes to RtFloatFunc and RtFunc 
 *                (RT 3.5 vol. 1).  Added 3.6 functions.
 *      10-12-98  Added PRiSubdivisionMeshV.
 *      12-05-98  Added PRiGeometricRepresentation and PRiReadResourceV.
 *      04-27-99  Added PRiNuCurvesV and PRiImplicitV.
 *
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#ifndef __RIPRIVF_HEADER__
#define __RIPRIVF_HEADER__


typedef RtVoid (*PRiBegin)( RtToken name );
typedef RtVoid (*PRiEnd)( void );
typedef RtVoid (*PRiFrameBegin)( RtInt frame );
typedef RtVoid (*PRiFrameEnd)( void );
typedef RtVoid (*PRiObjectEnd)( void ); 
typedef RtVoid (*PRiObjectInstance)( RtObjectHandle handle );
typedef RtVoid (*PRiMotionBeginV)( RtInt n, RtFloat times[] );
typedef RtVoid (*PRiMotionEnd)( void );
typedef RtVoid (*PRiSolidBegin)( RtToken operation );
typedef RtVoid (*PRiSolidEnd)( void );
typedef RtVoid (*PRiWorldBegin)( void );
typedef RtVoid (*PRiWorldEnd)( void );
typedef RtObjectHandle (*PRiObjectBegin)( void );

typedef RtVoid (*PRiErrorHandler)( RtErrorHandler handler );
typedef RtVoid (*PRiErrorIgnore)( RtInt code, RtInt severity, char *msg );
typedef RtVoid (*PRiErrorPrint)( RtInt code, RtInt severity, char *msg );
typedef RtVoid (*PRiErrorAbort)( RtInt code, RtInt severity, char *msg );

typedef RtVoid (*PRiFormat)( RtInt xres, RtInt yres, RtFloat aspect );
typedef RtVoid (*PRiFrameAspectRatio)( RtFloat aspect );
typedef RtVoid (*PRiScreenWindow)( RtFloat left, RtFloat right, 
                                  RtFloat bottom, RtFloat top );
typedef RtVoid (*PRiClipping)( RtFloat hither, RtFloat yon );
typedef RtVoid (*PRiCropWindow)( RtFloat xmin, RtFloat xmax, 
                                RtFloat ymin, RtFloat ymax );
typedef RtVoid (*PRiDepthOfField)( RtFloat fstop, RtFloat focallength, 
                                  RtFloat focaldistance );
typedef RtVoid (*PRiProcedural)( RtPointer data, RtBound bound,
                RtVoid (*subdivfunc)(RtPointer, RtFloat),
                RtVoid (*freefunc)(RtPointer) );
typedef RtVoid (*PRiProjectionV)( RtToken name, 
                 RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiShutter)( RtFloat min, RtFloat max );

typedef RtVoid (*PRiDisplayV)( char *name, RtToken type, RtToken mode, 
                 RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiExposure)( RtFloat gain, RtFloat gamma );
typedef RtVoid (*PRiImagerV)( RtToken name,
                 RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiPixelFilter)( RtFilterFunc filterfunc, 
                                 RtFloat xwidth, RtFloat ywidth );
typedef RtVoid (*PRiPixelSamples)( RtFloat xsamples, RtFloat ysamples );
typedef RtVoid (*PRiPixelVariance)( RtFloat variation );
typedef RtVoid (*PRiQuantize)( RtToken type, RtInt one, 
                              RtInt min, RtInt max, RtFloat ampl );

typedef RtVoid (*PRiConcatTransform)( RtMatrix transform);
typedef RtVoid (*PRiCoordinateSystem)( RtToken space );
typedef RtVoid (*PRiCoordSysTransform)( RtToken space );
typedef RtVoid (*PRiIdentity)( void );
typedef RtVoid (*PRiPerspective)( RtFloat fov );
typedef RtVoid (*PRiRotate)( RtFloat angle, 
                            RtFloat dx, RtFloat dy, RtFloat dz );
typedef RtVoid (*PRiScale)( RtFloat dx, RtFloat dy, RtFloat dz );
typedef RtVoid (*PRiSkew)( RtFloat angle, 
                          RtFloat dx1, RtFloat dy1, RtFloat dz1,
                          RtFloat dx2, RtFloat dy2, RtFloat dz2 );
typedef RtVoid (*PRiTransform)( RtMatrix transform );
typedef RtVoid (*PRiTransformBegin)( void );
typedef RtVoid (*PRiTransformEnd)( void );
typedef RtVoid (*PRiTranslate)( RtFloat dx, RtFloat dy, RtFloat dz );
typedef RtPoint* (*PRiTransformPoints)( RtToken fromspace, RtToken tospace,
                                       RtInt n, RtPoint points[] );

typedef RtVoid (*PRiAtmosphereV)( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiDeformationV)( RtToken name,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiDisplacementV)( RtToken name,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiExteriorV)( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiIlluminate)( RtLightHandle light, RtBoolean onoff );
typedef RtVoid (*PRiInteriorV)( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiMatte)( RtBoolean onoff );
typedef RtVoid (*PRiShadingRate)( RtFloat size );
typedef RtVoid (*PRiShadingInterpolation)( RtToken type );
typedef RtVoid (*PRiSurfaceV)( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );

typedef RtVoid (*PRiArchiveRecord)( RtToken type, char *format );
typedef RtVoid (*PRiReadArchiveV)( RtToken name, 
                  RtVoid (*callback)( RtToken, char* ),
                  RtInt n, RtToken tokens[], RtPointer parms[] );

typedef RtVoid (*PRiAttributeV)( RtToken name,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiAttributeBegin)( void );
typedef RtVoid (*PRiAttributeEnd)( void );
typedef RtVoid (*PRiBound)( RtBound bound );
typedef RtVoid (*PRiColor)( RtColor color );
typedef RtVoid (*PRiOpacity)( RtColor color);
typedef RtVoid (*PRiOptionV)( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiReverseOrientation)( void );
typedef RtVoid (*PRiTextureCoordinates)( RtFloat s1, RtFloat t1, 
                                        RtFloat s2, RtFloat t2,
                                        RtFloat s3, RtFloat t3, 
                                        RtFloat s4, RtFloat t4 );
typedef RtVoid (*PRiSides)( RtInt sides );   

typedef RtToken (*PRiDeclare)( char *name, char *declaration );

typedef RtLightHandle (*PRiLightSourceV)( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtLightHandle (*PRiAreaLightSourceV)( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );

typedef RtVoid (*PRiBasis)( RtBasis ubasis, RtInt ustep, 
                           RtBasis vbasis, RtInt vstep );
typedef RtVoid (*PRiPatchV)( RtToken type, 
                            RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiPatchMeshV)( RtToken type, 
                  RtInt nu, RtToken uwrap, 
                  RtInt nv, RtToken vwrap, 
                  RtInt n, RtToken tokens[], RtPointer parms[] ); 

typedef RtVoid (*PRiNuPatchV)( RtInt nu, RtInt uorder, RtFloat uknot[], 
                  RtFloat umin, RtFloat umax, 
                  RtInt nv, RtInt vorder, RtFloat vknot[],
                  RtFloat vmin, RtFloat vmax,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiTrimCurve)( RtInt nloops, RtInt ncurves[], 
                               RtInt order[], RtFloat knot[],
                               RtFloat min[], RtFloat max[], RtInt n[],
                               RtFloat u[], RtFloat v[], RtFloat w[] );

typedef RtVoid (*PRiConeV)( RtFloat height, RtFloat radius, 
                  RtFloat thetamax,
                  RtInt n, RtToken tokens[], RtPointer parms[] ); 
typedef RtVoid (*PRiCurvesV)( RtToken type, RtInt ncurves, RtInt nvertices[], 
                             RtToken wrap,
                             RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiCylinderV)( RtFloat radius,
                  RtFloat zmin, RtFloat zmax, RtFloat thetamax,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiDiskV)( RtFloat height, RtFloat radius, 
                  RtFloat thetamax,
                  RtInt n, RtToken tokens[], RtPointer parms[] ); 
typedef RtVoid (*PRiHyperboloidV)( RtPoint point1, RtPoint point2, 
                  RtFloat thetamax,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiParaboloidV)( RtFloat rmax, 
                  RtFloat zmin, RtFloat zmax, RtFloat thetamax,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiSphereV)( RtFloat radius, 
                  RtFloat zmin, RtFloat zmax, RtFloat thetamax,
                  RtInt n, RtToken tokens[], RtPointer parms[] ); 
typedef RtVoid (*PRiTorusV)( RtFloat majorradius, RtFloat minorradius, 
                  RtFloat phimin, RtFloat phimax, RtFloat thetamax, 
                  RtInt n, RtToken tokens[], RtPointer parms[] ); 

typedef RtVoid (*PRiPointsV)( RtInt npoints, 
                             RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiGeneralPolygonV)( RtInt nloops, RtInt nvertices[],
                  RtInt n, RtToken tokens[], RtPointer parms[] ); 
typedef RtVoid (*PRiPointsGeneralPolygonsV)( RtInt npolys, RtInt nloops[], 
                  RtInt nvertices[], RtInt vertices[], 
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiPointsPolygonsV)( RtInt npolys, 
                  RtInt nvertices[], RtInt vertices[],
                  RtInt n, RtToken tokens[], RtPointer parms[] ); 
typedef RtVoid (*PRiPolygonV)( RtInt nvertices,
                  RtInt n, RtToken tokens[], RtPointer parms[] );

typedef RtVoid (*PRiSubdivisionMeshV)( RtToken scheme, RtInt nfaces, 
		  RtInt nvertices[], RtInt vertices[],
		  RtInt ntags, RtToken tags[],
		  RtInt nargs[], RtInt intargs[], RtFloat floatargs[],
		  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiImplicitV)( RtInt a, RtInt b[], RtInt, RtFloat c[],
		RtInt d, RtFloat e[], RtInt f, RtFloat g[],
		RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiNuCurvesV)( RtInt ncurves, RtInt nvertices[],
		RtInt order[], RtFloat knot[], RtFloat min[], RtFloat max[], 
		RtInt n, RtToken tokens[], RtPointer parms[] );

typedef RtVoid (*PRiColorSamples)( RtInt n, RtFloat nRGB[], RtFloat RGBn[] );
typedef RtVoid (*PRiHiderV)( RtToken type,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiDetail)( RtBound bound );
typedef RtVoid (*PRiDetailRange)( RtFloat minvisible, RtFloat lowertransition, 
                  RtFloat uppertransition, RtFloat maxvisible );
typedef RtVoid (*PRiGeometricApproximation)( RtToken type, RtFloat value );
typedef RtVoid (*PRiGeometryV)( RtToken type, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiOrientation)( RtToken orientation );
typedef RtVoid (*PRiRelativeDetail)( RtFloat relativedetail );

typedef RtVoid (*PRiMakeBumpV)( char *picturename, char *texturename, 
               RtToken swrap, RtToken twrap,
               RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
               RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiMakeCubeFaceEnvironmentV)( char *px, char *nx, 
                  char *py, char *ny,
                  char *pz, char *nz, char *texturename,
                  RtFloat fov,
                  RtFilterFunc filterfunc, 
                  RtFloat swidth, RtFloat twidth,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiMakeLatLongEnvironmentV)( 
                  char *picturename, char *texturename, 
                  RtFilterFunc filterfunc,
                  RtFloat swidth, RtFloat twidth,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiMakeShadowV)( char *picturename, char *texturename,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiMakeTextureV)( char *picturename, char *texturename, 
                  RtToken swrap, RtToken twrap,
                  RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
                  RtInt n, RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiResourceV)( RtToken handle, RtToken type,
				RtInt n, 
				RtToken tokens[], RtPointer parms[] );
typedef RtVoid (*PRiGeometricRepresentation)( RtToken type );


typedef RtFloat (*PRiBoxFilter)( RtFloat x, RtFloat y, 
                  RtFloat xwidth, RtFloat ywidth );
typedef RtFloat (*PRiTriangleFilter)( RtFloat x, RtFloat y, 
                  RtFloat xwidth, RtFloat ywidth );
typedef RtFloat (*PRiCatmullRomFilter)( RtFloat x, RtFloat y, 
                  RtFloat xwidth, RtFloat ywidth );
typedef RtFloat (*PRiGaussianFilter)( RtFloat x, RtFloat y, 
                  RtFloat xwidth, RtFloat ywidth );
typedef RtFloat (*PRiSincFilter)( RtFloat x, RtFloat y, 
                  RtFloat xwidth, RtFloat ywidth );

typedef RtVoid (*PRiProcDelayedReadArchive)( RtPointer data, RtFloat detail );
typedef RtVoid (*PRiProcDynamicLoad)( RtPointer data, RtFloat detail );
typedef RtVoid (*PRiProcRunProgram)( RtPointer data, RtFloat detail );


#endif
/* endif __RIPRIVF_HEADER__ */
