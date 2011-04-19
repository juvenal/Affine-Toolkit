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
 * FILE:  ripriv.h
 *
 * DESCRIPTION:  Private include file to handle Ri extern definitions.
 *   
 * CHANGES:
 *    Incorporated changes to RtFloatFunc and RtFunc (RT 3.5 vol. 1).
 *    Added 3.6 functions.
 *
 *    Include file which uses parts of the RenderMan Interface.
 *
 *    References:
 *          [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, 
 *                    Richmond, CA, September 1989.
 *          [PIXA98]  Pixar, Advanced RenderMan: Beyond the Companion,
 *                    SIGGRAPH Course Notes #11, Richmond, CA, July, 1998, 
 *                    p. 12.
 *
 *     History:
 *       7-10-97  Added support for new PRMan 3.7 Points and Curve primitives.
 *       7-14-97  Added RiProcDelayedReadArchive, RiProcDynamicLoad and
 *                RiProcRunProgram.
 *      10-12-98  Added RiSubdivisionMesh and RiSubdivisionMeshV.
 *      12-05-98  Added RiGeometricRepresentation, RiResource and RiResourceV.
 *      04-27-99  Added RiNuCurves() and RiNuCurvesV() to become more
 *                worldly and support RenderDotC extensions.  Added 
 *                RiImplicit() and RiImplicitV().
 *
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#ifndef RI_HEADER
#define RI_HEADER

typedef short      RtBoolean;
typedef int        RtInt;
typedef float      RtFloat;
typedef char       *RtToken;

typedef RtFloat    RtColor[3];
typedef RtFloat    RtPoint[3];
typedef RtFloat    RtMatrix[4][4];
typedef RtFloat    RtBasis[4][4];
typedef RtFloat    RtBound[6];
typedef char       *RtString;
typedef char       *RtPointer;
/* Changed from original RI 3.1 spec. 
 * typedef int        RtVoid; 
 */
#define RtVoid     void

typedef RtPointer  RtLightHandle;
typedef RtPointer  RtObjectHandle;

/* (3.5 CHANGE)
typedef RtFloat    (*RtFloatFunc)();
typedef RtVoid     (*RtFunc)();
*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif
typedef RtFloat    (*RtFilterFunc)(RtFloat, RtFloat, RtFloat, RtFloat );
typedef RtVoid     (*RtErrorHandler)(RtInt, RtInt, char * );

#ifdef _RIPRIV_TYPES 
typedef RtToken           *PRtToken;         
typedef RtPointer         *PRtPointer;         
typedef void              *PRtVoid;         /* pvd   */
typedef RtPoint           *PRtPoint;        /* ppt   */
typedef RtFloat           RtHPoint[4];      /* hpt   */
typedef RtFloat           *PRtHPoint;       /* phpt  */
typedef RtFloat           *PRtMatrix;       /* pmtrx */ 
typedef RtFloat           *PRtBasis;        /* pbs   */ 
typedef RtBound           *PRtBound;        /* pbnd  */ 
typedef RtBoolean         *PRtBoolean;      /* pbln  */
typedef RtInt             *PRtInt;          /* pint  */
typedef RtFloat           *PRtFloat;        /* pflt  */
typedef RtLightHandle     *PRtLightHandle;  /* phlt  */
typedef RtPoint           RtBCPatch[4][4];  /* bcp   */ /* BiCubic Patch */
typedef RtFloat           *PRtBCPatch;      /* pbcp  */ 
typedef RtPoint           RtBLPatch[2][2];  /* blp   */ /* BiLinear Patch */
typedef RtFloat           *PRtBLPatch;      /* pblp  */ 
#endif

#define RI_FALSE  0
#define RI_TRUE   (!RI_FALSE)
#define RI_NULL   ((RtToken)0)
/* The affine/sribw/testrib DepthOfField(fstop==RI_INFINITY,0,0) test failed
 * without placing a (RtFloat) in front of the 1.0e38.  While here also cast 
 * the 1.0e-10 as well.  
 */
#define RI_INFINITY  (RtFloat)1.0e38 
#define RI_EPSILON   (RtFloat)1.0e-10

#ifndef __FILE_CNSTNT
extern RtToken RI_FRAMEBUFFER, RI_FILE;
extern RtToken RI_RGB, RI_RGBA, RI_RGBZ, RI_RGBAZ, RI_A, RI_Z, RI_AZ;
extern RtToken RI_PERSPECTIVE, RI_ORTHOGRAPHIC, RI_AFFINENOTICE;
extern RtToken RI_HIDDEN, RI_PAINT;
extern RtToken RI_CONSTANT, RI_SMOOTH;
extern RtToken RI_FLATNESS, RI_FOV;

extern RtToken RI_AMBIENTLIGHT, RI_POINTLIGHT,
               RI_DISTANTLIGHT, RI_SPOTLIGHT;
extern RtToken RI_INTENSITY, RI_LIGHTCOLOR, RI_FROM, RI_TO,
               RI_CONEANGLE, RI_CONEDELTAANGLE, RI_BEAMDISTRIBUTION;

extern RtToken RI_MATTE, RI_METAL, RI_SHINYMETAL; 
extern RtToken RI_PLASTIC, RI_PAINTEDPLASTIC;

extern RtToken RI_KA, RI_KD, RI_KS;
extern RtToken RI_ROUGHNESS, RI_KR, RI_TEXTURENAME,
               RI_SPECULARCOLOR;

extern RtToken RI_DEPTHCUE, RI_FOG, RI_BUMPY;
extern RtToken RI_DISTANCE, RI_MINDISTANCE, RI_MAXDISTANCE;
extern RtToken RI_BACKGROUND;

extern RtToken RI_RASTER, RI_SCREEN, RI_CAMERA, RI_WORLD, RI_OBJECT;
extern RtToken RI_INSIDE, RI_OUTSIDE, RI_LH, RI_RH;

extern RtToken RI_P, RI_PZ, RI_PW;
extern RtToken RI_N, RI_NP, RI_CS, RI_OS,
                    RI_S, RI_T, RI_ST;

extern RtToken RI_PRIMITIVE, RI_INTERSECTION, RI_UNION, RI_DIFFERENCE;
extern RtToken RI_PERIODIC, RI_PIXARNOTICE, RI_NONPERIODIC;
extern RtToken RI_CLAMP, RI_BLACK;
extern RtToken RI_IGNORE, RI_PRINT, RI_ABORT, RI_HANDLER;
extern RtToken RI_TEAPOT;
extern RtToken RI_COMMENT, RI_STRUCTURE;
extern RtToken RI_AMPLITUDE;
extern RtToken RI_ORIGIN;
#endif

#ifndef __FILE_SBASIS
extern RtToken RI_BILINEAR, RI_BICUBIC;
extern RtToken RI_PERIODIC, RI_NONPERIODIC;
extern RtBasis RiBezierBasis, RiBSplineBasis, RiCatmullRomBasis,
               RiHermiteBasis, RiPowerBasis;
#endif

#define RI_BEZIERSTEP      ((RtInt)3)
#define RI_BSPLINESTEP     ((RtInt)1)
#define RI_CATMULLROMSTEP  ((RtInt)1)
#define RI_HERMITESTEP     ((RtInt)2)
#define RI_POWERSTEP       ((RtInt)4)

#ifndef __FILE_RIBE
extern 
#endif
RtVoid    
   RiBegin( RtToken name ), RiEnd( void ),
   RiFrameBegin( RtInt frame ), RiFrameEnd( void ),
   RiObjectEnd( void ), 
   RiObjectInstance( RtObjectHandle handle ),
   RiMotionBegin( RtInt n, ... ), RiMotionBeginV( RtInt n, RtFloat times[] ), 
   RiMotionEnd( void ),
   RiSolidBegin( RtToken operation ), RiSolidEnd( void ),
   RiWorldBegin( void ), RiWorldEnd( void );
#ifndef __FILE_RIBE
extern 
#endif
RtObjectHandle
   RiObjectBegin( void );

#ifndef __FILE_RIERROR
extern 
#endif
RtVoid
   RiErrorHandler( /* RtFunc (3.5 CHANGE)*/ RtErrorHandler handler ),
   RiErrorIgnore( RtInt code, RtInt severity, char *msg ),
   RiErrorPrint( RtInt code, RtInt severity, char *msg ),
   RiErrorAbort( RtInt code, RtInt severity, char *msg );

#ifndef __FILE_RIERROR
extern RtInt RiLastError;
#endif


#ifndef __FILE_RIVIEW
extern 
#endif
RtVoid
   RiFormat( RtInt xres, RtInt yres, RtFloat aspect ),
   RiFrameAspectRatio( RtFloat aspect ),
   RiScreenWindow( RtFloat left, RtFloat right, RtFloat bottom, RtFloat top ),
   RiClipping( RtFloat hither, RtFloat yon ),
   RiCropWindow( RtFloat xmin, RtFloat xmax, RtFloat ymin, RtFloat ymax ),
   RiDepthOfField( RtFloat fstop, RtFloat focallength, RtFloat focaldistance ),
   RiProjection( RtToken name, ... ),
   RiProjectionV( RtToken name, 
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiShutter( RtFloat min, RtFloat max );


#ifndef __FILE_RIDSPY
extern 
#endif
RtVoid
   RiDisplay( char *name, RtToken type, RtToken mode, ... ),
   RiDisplayV( char *name, RtToken type, RtToken mode, 
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiExposure( RtFloat gain, RtFloat gamma ),
   RiImager( RtToken name, ... ),
   RiImagerV( RtToken name,
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiPixelFilter( RtFilterFunc filterfunc, RtFloat xwidth, RtFloat ywidth ),
   /*RiPixelFidelity( RtFloat variation ),*//* Replaced with RiPixelVariance.*/
   RiPixelSamples( RtFloat xsamples, RtFloat ysamples ),
   RiPixelVariance( RtFloat variation ),
   RiQuantize( RtToken type, RtInt one, RtInt min, RtInt max, RtFloat ampl );

#ifndef __FILE_RIATRANS
extern 
#endif
RtVoid
   RiConcatTransform( RtMatrix transform),
   RiCoordinateSystem( RtToken space ),
   RiCoordSysTransform( RtToken space ),
   RiIdentity( void ),
   RiPerspective( RtFloat fov ),
   RiRotate( RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz ),
   RiScale( RtFloat dx, RtFloat dy, RtFloat dz ),
   RiSkew( RtFloat angle, 
          RtFloat dx1, RtFloat dy1, RtFloat dz1,
          RtFloat dx2, RtFloat dy2, RtFloat dz2 ),
   RiTransform( RtMatrix transform ),
   RiTransformBegin( void ),
   RiTransformEnd( void ),
   RiTranslate( RtFloat dx, RtFloat dy, RtFloat dz );
#ifndef __FILE_RIATRANS
extern 
#endif
   RtPoint* RiTransformPoints( RtToken fromspace, RtToken tospace,
                 RtInt n, RtPoint points[] );


#ifndef __FILE_RISHDR
extern 
#endif
RtVoid
   RiAtmosphere( RtToken name, ... ),
   RiAtmosphereV( RtToken name, 
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiDeformation( RtToken name, ...),
   RiDeformationV( RtToken name,
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiDisplacement( RtToken name, ...),
   RiDisplacementV( RtToken name,
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiExterior( RtToken name, ... ),
   RiExteriorV( RtToken name, 
               RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiIlluminate( RtLightHandle light, RtBoolean onoff ),
   RiInterior( RtToken name, ... ),
   RiInteriorV( RtToken name, 
               RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiMatte( RtBoolean onoff ),
   RiShadingRate( RtFloat size ),
   RiShadingInterpolation( RtToken type ),
   RiSurface( RtToken name, ... ),
   RiSurfaceV( RtToken name, 
                 RtInt n, RtToken tokens[], RtPointer parms[] );

#ifndef __FILE_RIFILES
extern 
#endif
RtVoid
   RiArchiveRecord( RtToken type, char *format, ... ),
   RiReadArchive( RtToken name, 
                 RtVoid (*callback)(RtToken,char*, ... ), 
                 ... ),
   RiReadArchiveV( RtToken name, 
                  RtVoid (*callback)( RtToken, char*, ... ),
                  RtInt n, RtToken tokens[], RtPointer parms[] );


#ifndef __FILE_RIATTRIB
extern 
#endif
RtVoid
   RiAttribute( RtToken name, ... ),
   RiAttributeV( RtToken name,
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiAttributeBegin( void ),
   RiAttributeEnd( void ),
   RiBound( RtBound bound ),
   RiColor( RtColor color ),
   RiOpacity( RtColor color),
   RiOption( RtToken name, ... ),
   RiOptionV( RtToken name, 
             RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiReverseOrientation( void ),
   RiTextureCoordinates( RtFloat s1, RtFloat t1, 
                         RtFloat s2, RtFloat t2,
                         RtFloat s3, RtFloat t3, 
                         RtFloat s4, RtFloat t4 ),
   RiSides( RtInt sides );   
#ifndef __FILE_RIATTRIB
extern 
#endif
RtToken
   RiDeclare( char *name, char *declaration );

#ifndef __FILE_RIATTRIB
extern 
#endif
RtLightHandle
   RiLightSource( RtToken name, ... ),
   RiLightSourceV( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiAreaLightSource( RtToken name, ... ),
   RiAreaLightSourceV( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );


#ifndef __FILE_RIPATCH
extern 
#endif 
RtVoid
   RiBasis( RtBasis ubasis, RtInt ustep, RtBasis vbasis, RtInt vstep ),
   RiPatch( RtToken type, ... ),
   RiPatchMesh( RtToken type, RtInt nu, RtToken uwrap, 
                RtInt nv, RtToken vwrap, ... ),
   RiPatchV( RtToken type, 
            RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiPatchMeshV( RtToken type, RtInt nu, RtToken uwrap, 
                 RtInt nv, RtToken vwrap, 
                RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiPoints( RtInt npoints, ... ),
   RiPointsV( RtInt npoints, RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiCurves( RtToken type, RtInt ncurves, RtInt nvertices[], 
            RtToken wrap, ... ),
   RiCurvesV( RtToken type, RtInt ncurves, RtInt nvertices[], 
             RtToken wrap, 
             RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNuCurves( RtInt ncurves, RtInt nvertices[], RtInt order[],
	       RtFloat knot[], RtFloat min[], RtFloat max[], ... ),
   RiNuCurvesV( RtInt ncurves, RtInt nvertices[], RtInt order[],
		RtFloat knot[], RtFloat min[], RtFloat max[], 
		RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiImplicit( RtInt a, RtInt b[], RtInt c, RtFloat d[],
	       RtInt e, RtFloat f[], RtInt g, RtFloat h[], ... ),
   RiImplicitV( RtInt a, RtInt b[], RtInt c, RtFloat d[],
		RtInt e, RtFloat f[], RtInt g, RtFloat h[],
		RtInt n, RtToken tokens[], RtPointer parms[] );


#ifndef __FILE_RINURBS
extern
#endif
RtVoid
   RiNuPatch( RtInt nu, RtInt uorder, RtFloat uknot[], 
             RtFloat umin, RtFloat umax,
             RtInt nv, RtInt vorder, RtFloat vknot[], 
             RtFloat vmin, RtFloat vmax, ... ),
   RiNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
              RtFloat umin, RtFloat umax, 
              RtInt nv, RtInt vorder, RtFloat vknot[],
              RtFloat vmin, RtFloat vmax,
              RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiTrimCurve( RtInt nloops, RtInt ncurves[], RtInt order[], RtFloat knot[],
               RtFloat min[], RtFloat max[], RtInt n[],
               RtFloat u[], RtFloat v[], RtFloat w[] );


#ifndef __FILE_RIQUADS
extern 
#endif
RtVoid    
   RiCone( RtFloat height, RtFloat radius, RtFloat thetamax, ... ),
   RiConeV( RtFloat height, RtFloat radius, RtFloat thetamax,
          RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiCylinder( RtFloat radius,RtFloat zmin,RtFloat zmax,RtFloat thetamax,... ),
   RiCylinderV( RtFloat radius,RtFloat zmin,RtFloat zmax,RtFloat thetamax,
              RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiDisk( RtFloat height, RtFloat radius, RtFloat thetamax, ... ),
   RiDiskV( RtFloat height, RtFloat radius, RtFloat thetamax,
          RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiHyperboloid( RtPoint point1, RtPoint point2, RtFloat thetamax, ... ),
   RiHyperboloidV( RtPoint point1, RtPoint point2, RtFloat thetamax,
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiParaboloid( RtFloat rmax,RtFloat zmin,RtFloat zmax,RtFloat thetamax,... ),
   RiParaboloidV( RtFloat rmax,RtFloat zmin,RtFloat zmax,RtFloat thetamax,
                RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiSphere( RtFloat radius,RtFloat zmin,RtFloat zmax, RtFloat thetamax, ... ),
   RiSphereV( RtFloat radius,RtFloat zmin,RtFloat zmax, RtFloat thetamax,
             RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiTorus( RtFloat majorradius, RtFloat minorradius, 
           RtFloat phimin, RtFloat phimax, RtFloat thetamax, ... ),
   RiTorusV( RtFloat majorradius, RtFloat minorradius, 
           RtFloat phimin, RtFloat phimax, RtFloat thetamax, 
           RtInt n, RtToken tokens[], RtPointer parms[] ); 


#ifndef __FILE_RIPOLY
extern 
#endif
RtVoid    
   RiGeneralPolygon( RtInt nloops, RtInt nvertices[], ... ),
   RiGeneralPolygonV( RtInt nloops, RtInt nvertices[],
                     RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiPointsGeneralPolygons( RtInt npolys, RtInt nloops[], 
                           RtInt nvertices[], RtInt vertices[], ... ),
   RiPointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
                            RtInt nvertices[], RtInt vertices[], 
                            RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiPointsPolygons( RtInt npolys, RtInt nvertices[], RtInt vertices[], ... ),
   RiPointsPolygonsV( RtInt npolys, RtInt nvertices[], RtInt vertices[],
                     RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiPolygon( RtInt nvertices, ... ),
   RiPolygonV( RtInt nvertices,
              RtInt n, RtToken tokens[], RtPointer parms[] );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid    
   RiSubdivisionMesh( RtToken scheme, RtInt nfaces, 
		      RtInt nvertices[], RtInt vertices[],
		      RtInt ntags, RtToken tags[],
		      RtInt nargs[], RtInt intargs[], RtFloat floatargs[],...),
   RiSubdivisionMeshV( RtToken scheme, RtInt nfaces, 
		       RtInt nvertices[], RtInt vertices[],
		       RtInt ntags, RtToken tags[],
		       RtInt nargs[], RtInt intargs[], RtFloat floatargs[],
		       RtInt n, RtToken tokens[], RtPointer parms[] );

#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid    
   RiImplicit( RtInt, RtInt [], RtInt, RtFloat [],
	       RtInt, RtFloat [], RtInt, RtFloat [],...),
   RiImplicitV( RtInt, RtInt [], RtInt, RtFloat [],
		RtInt, RtFloat [], RtInt, RtFloat [],
		RtInt n, RtToken tokens[], RtPointer parms[]);


#ifndef __FILE_RIEXTRA
extern 
#endif
RtVoid    
   RiColorSamples( RtInt n, RtFloat nRGB[], RtFloat RGBn[] ),
   RiHider( RtToken type, ... ),
   RiHiderV( RtToken type,
            RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiDetail( RtBound bound ),
   RiDetailRange( RtFloat minvisible, RtFloat lowertransition, 
                 RtFloat uppertransition, RtFloat maxvisible ),
   RiGeometricApproximation( RtToken type, RtFloat value ),
   RiGeometry( RtToken type, ... ),
   RiGeometryV( RtToken type, 
              RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiOrientation( RtToken orientation ),
/* (3.5 CHANGE)
   RiProcedural( RtPointer data, RtBound bound,
                RtFunc subdivfunc, RtFunc freefunc ),
 */
   RiProcedural( RtPointer data, RtBound bound,
                RtVoid (*subdivfunc)(RtPointer, RtFloat),
                RtVoid (*freefunc)(RtPointer) ),
   RiRelativeDetail( RtFloat relativedetail ),
   RiProcDelayedReadArchive( RtPointer data, RtFloat detail ),
   RiProcDynamicLoad( RtPointer data, RtFloat detail ),
   RiProcRunProgram( RtPointer data, RtFloat detail );


#ifndef __FILE_RITEX
extern 
#endif
RtVoid
   RiMakeBump( char *picturename, char *texturename, 
              RtToken swrap, RtToken twrap,
              RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, ... ),
   RiMakeBumpV( char *picturename, char *texturename, 
               RtToken swrap, RtToken twrap,
               RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
               RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiMakeCubeFaceEnvironment( char *px, char *nx, char *py, char *ny,
                             char *pz, char *nz, char *texturename, 
                             RtFloat fov,
                             RtFilterFunc filterfunc, 
                             RtFloat swidth, RtFloat twidth, ... ),
   RiMakeCubeFaceEnvironmentV( char *px, char *nx, char *py, char *ny,
                              char *pz, char *nz, char *texturename,
                              RtFloat fov,
                              RtFilterFunc filterfunc, 
                              RtFloat swidth, RtFloat twidth,
                              RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiMakeLatLongEnvironment( char *picturename, char *texturename, 
                            RtFilterFunc filterfunc,
                            RtFloat swidth, RtFloat twidth, ...),
   RiMakeLatLongEnvironmentV( char *picturename, char *texturename, 
                             RtFilterFunc filterfunc,
                             RtFloat swidth, RtFloat twidth,
                             RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiMakeShadow( char *picturename, char *texturename, ... ),
   RiMakeShadowV( char *picturename, char *texturename,
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiMakeTexture( char *picturename, char *texturename, 
                 RtToken swrap, RtToken twrap,
                 RtFilterFunc filterfunc, 
                 RtFloat swidth, RtFloat twidth, ... ),
   RiMakeTextureV( char *picturename, char *texturename, 
                  RtToken swrap, RtToken twrap,
                  RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
                  RtInt n, RtToken tokens[], RtPointer parms[] );

#ifndef __FILE_ADDITIONS
extern 
#endif
RtVoid
   RiResource( RtToken handle, RtToken type, ... ),
   RiResourceV( RtToken handle, RtToken type,
		RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiGeometricRepresentation( RtToken type );


#ifdef _RIPRIV_FUNC_TYPES 
#include <riprivf.h>
#endif


#ifndef __FILE_PIXFILTR
extern 
#endif
RtFloat
   RiBoxFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth ),
   RiTriangleFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth ),
   RiCatmullRomFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth ),
   RiGaussianFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth ),
   RiSincFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth );

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


#define RIE_NOERROR       ((RtInt)0)
#define RIE_NOMEM         ((RtInt)1)  
#define RIE_SYSTEM        ((RtInt)2)  
#define RIE_NOFILE        ((RtInt)3)  
#define RIE_BADFILE       ((RtInt)4)  
#define RIE_VERSION       ((RtInt)5)  

#define RIE_INCAPABLE     ((RtInt)11)  
#define RIE_UNIMPLEMENT   ((RtInt)12)  
#define RIE_LIMIT         ((RtInt)13)  
#define RIE_BUG           ((RtInt)14)  

#define RIE_NOTSTARTED    ((RtInt)23)  
#define RIE_NESTING       ((RtInt)24)  
#define RIE_NOTOPTIONS    ((RtInt)25)  
#define RIE_NOTATTRIBS    ((RtInt)26)  
#define RIE_NOTPRIMS      ((RtInt)27)  
#define RIE_ILLSTATE      ((RtInt)28)  
#define RIE_BADMOTION     ((RtInt)29)  
#define RIE_BADSOLID      ((RtInt)30)  

#define RIE_BADTOKEN      ((RtInt)41)  
#define RIE_RANGE         ((RtInt)42)  
#define RIE_CONSISTENCY   ((RtInt)43)  
#define RIE_BADHANDLE     ((RtInt)44)  
#define RIE_NOSHADER      ((RtInt)45)  
#define RIE_MISSINGDATA   ((RtInt)46)  
#define RIE_SYNTAX        ((RtInt)47)

#define RIE_MATH          ((RtInt)61)    

#define RIE_INFO          ((RtInt)0)  
#define RIE_WARNING       ((RtInt)1)  
#define RIE_ERROR         ((RtInt)2)  
#define RIE_SEVERE        ((RtInt)3)  

#endif
/* endif RI_HEADER */
