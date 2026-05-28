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
 * FILE:  ribnop.h
 *
 * DESCRIPTION:  
 *   
 *    References:
 *          [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, 
 *                     Richmond, CA, September 1989.
 *
 *    History:
 *       10-12-98  Added RiNopSubdivisionMeshV.
 *       04-27-99  Added RiNopNuCurvesV() and RiNopImplicitV().
 *    
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *              Renderman (R) is a registered trademark of Pixar
 */
#ifndef RIBNOP_HEADER
#define RIBNOP_HEADER

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid    
   RiNopBegin( RtToken name ),RiNopEnd( void ),
   RiNopFrameBegin( RtInt frame ),RiNopFrameEnd( void ),
   RiNopObjectEnd( void ), 
   RiNopObjectInstance( RtObjectHandle handle ),
   RiNopMotionBeginV( RtInt n, RtFloat times[] ), 
   RiNopMotionEnd( void ),
   RiNopSolidBegin( RtToken operation ),RiNopSolidEnd( void ),
   RiNopWorldBegin( void ),RiNopWorldEnd( void );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtObjectHandle
   RiNopObjectBegin( void );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid
   RiNopErrorHandler( RtErrorHandler handler ),
   RiNopErrorIgnore( RtInt code, RtInt severity, char *msg ),
   RiNopErrorPrint( RtInt code, RtInt severity, char *msg ),
   RiNopErrorAbort( RtInt code, RtInt severity, char *msg );


#ifndef __RIB_FILE_RIBNOP
extern int RtIntRiNopLastError;
#endif


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid
   RiNopFormat( RtInt xres, RtInt yres, RtFloat aspect ),
   RiNopFrameAspectRatio( RtFloat aspect ),
   RiNopScreenWindow( RtFloat left, RtFloat right, 
                     RtFloat bottom, RtFloat top ),
   RiNopClipping( RtFloat hither, RtFloat yon ),
   RiNopCropWindow( RtFloat xmin, RtFloat xmax, RtFloat ymin, RtFloat ymax ),
   RiNopDepthOfField( RtFloat fstop, 
                     RtFloat focallength, RtFloat focaldistance ),
   RiNopProjectionV( RtToken name, 
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopShutter( RtFloat min, RtFloat max );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid
   RiNopDisplayV( char *name, RtToken type, RtToken mode, 
                  RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopExposure( RtFloat gain, RtFloat gamma ),
   RiNopImagerV( RtToken name,
                  RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopPixelFilter( RtFilterFunc filterfunc, RtFloat xwidth, RtFloat ywidth ),
   RiNopPixelSamples( RtFloat xsamples, RtFloat ysamples ),
   RiNopPixelVariance( RtFloat variation ),
   RiNopQuantize( RtToken type, 
                 RtInt one, RtInt min, RtInt max, RtFloat ampl );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid
   RiNopConcatTransform( RtMatrix transform),
   RiNopCoordinateSystem( RtToken space ),
   RiNopCoordSysTransform( RtToken space ),
   RiNopIdentity( void ),
   RiNopPerspective( RtFloat fov ),
   RiNopRotate( RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz ),
   RiNopScale( RtFloat dx, RtFloat dy, RtFloat dz ),
   RiNopSkew( RtFloat angle, 
             RtFloat dx1, RtFloat dy1, RtFloat dz1,
             RtFloat dx2, RtFloat dy2, RtFloat dz2 ),
   RiNopTransform( RtMatrix transform ),
   RiNopTransformBegin( void ),
   RiNopTransformEnd( void ),
   RiNopTranslate( RtFloat dx, RtFloat dy, RtFloat dz );

#ifndef __RIB_FILE_RIBNOP
extern 
#endif
    RtPoint*RiNopTransformPoints( RtToken fromspace, RtToken tospace,
                  RtInt n, RtPoint points[] );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid
   RiNopAtmosphereV( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopDeformationV( RtToken name,
                  RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopDisplacementV( RtToken name,
                  RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopExteriorV( RtToken name, 
                RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopIlluminate( RtLightHandle light, RtBoolean onoff ),
   RiNopInteriorV( RtToken name, 
                RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopMatte( RtBoolean onoff ),
   RiNopShadingRate( RtFloat size ),
   RiNopShadingInterpolation( RtToken type ),
   RiNopSurfaceV( RtToken name, 
                  RtInt n, RtToken tokens[], RtPointer parms[] );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid
   RiNopArchiveRecord( RtToken type, char *format, char *s ),
   RiNopReadArchiveV( RtToken name, 
                   RtVoid (*callback)( RtToken, char*, char* ),
                   RtInt n, RtToken tokens[], RtPointer parms[] );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid
   RiNopAttributeV( RtToken name,
                  RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopAttributeBegin( void ),
   RiNopAttributeEnd( void ),
   RiNopBound( RtBound bound ),
   RiNopColor( RtColor color ),
   RiNopOpacity( RtColor color),
   RiNopOptionV( RtToken name, 
              RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopReverseOrientation( void ),
   RiNopTextureCoordinates( RtFloat s1, RtFloat t1, 
                          RtFloat s2, RtFloat t2,
                          RtFloat s3, RtFloat t3, 
                          RtFloat s4, RtFloat t4 ),
   RiNopSides( RtInt sides );   


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtToken
   RiNopDeclare( char *name, char *declaration );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtLightHandle
   RiNopLightSourceV( RtToken name, 
                   RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopAreaLightSourceV( RtToken name, 
                   RtInt n, RtToken tokens[], RtPointer parms[] );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif 
RtVoid
   RiNopBasis( RtBasis ubasis, RtInt ustep, RtBasis vbasis, RtInt vstep ),
   RiNopPatchV( RtToken type, RtInt n, RtToken tokens[], 
              RtPointer parms[] ),
   RiNopPatchMeshV( RtToken type, RtInt nu, RtToken uwrap, 
                  RtInt nv, RtToken vwrap, 
                RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopPointsV( RtInt npoints, RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopCurvesV( RtToken type, RtInt ncurves, RtInt nvertices[], 
                RtToken wrap, 
                RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopNuCurvesV( RtInt ncurves, RtInt nvertices[], RtInt order[],
		RtFloat knot[], RtFloat min[], RtFloat max[], 
		RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopImplicitV( RtInt a, RtInt b[], RtInt c, RtFloat d[],
		   RtInt e, RtFloat f[], RtInt g, RtFloat h[],
		   RtInt n, RtToken tokens[], RtPointer parms[] );


#ifndef __RIB_FILE_RIBNOP
extern
#endif
RtVoid
   RiNopNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
                 RtFloat umin, RtFloat umax, 
                 RtInt nv, RtInt vorder, RtFloat vknot[],
                 RtFloat vmin, RtFloat vmax,
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopTrimCurve( RtInt nloops, 
                  RtInt ncurves[], RtInt order[], RtFloat knot[],
                  RtFloat min[], RtFloat max[], RtInt n[],
                  RtFloat u[], RtFloat v[], RtFloat w[] );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid    
   RiNopConeV( RtFloat height, RtFloat radius, RtFloat thetamax,
              RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiNopCylinderV( RtFloat radius,RtFloat zmin,RtFloat zmax,RtFloat thetamax,
               RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopDiskV( RtFloat height, RtFloat radius, RtFloat thetamax,
           RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiNopHyperboloidV( RtPoint point1, RtPoint point2, RtFloat thetamax,
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopParaboloidV( RtFloat rmax,RtFloat zmin,RtFloat zmax,RtFloat thetamax,
                RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopSphereV( RtFloat radius,RtFloat zmin,RtFloat zmax, RtFloat thetamax,
              RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiNopTorusV( RtFloat majorradius, RtFloat minorradius, 
            RtFloat phimin, RtFloat phimax, RtFloat thetamax, 
            RtInt n, RtToken tokens[], RtPointer parms[] ); 


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid    
   RiNopGeneralPolygonV( RtInt nloops, RtInt nvertices[],
                        RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiNopPointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
                               RtInt nvertices[], RtInt vertices[], 
                               RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopPointsPolygonsV( RtInt npolys, RtInt nvertices[], RtInt vertices[],
                        RtInt n, RtToken tokens[], RtPointer parms[] ), 
   RiNopPolygonV( RtInt nvertices,
                 RtInt n, RtToken tokens[], RtPointer parms[] );

#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid    
   RiNopSubdivisionMeshV( RtToken scheme, RtInt nfaces, 
			 RtInt nvertices[], RtInt vertices[],
			 RtInt ntags, RtToken tags[],
			 RtInt nargs[], RtInt intargs[], RtFloat floatargs[],
			 RtInt n, RtToken tokens[], RtPointer parms[] );

#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid    
   RiNopColorSamples( RtInt n, RtFloat nRGB[], RtFloat RGBn[] ),
   RiNopHiderV( RtToken type,
             RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopDetail( RtBound bound ),
   RiNopDetailRange( RtFloat minvisible, RtFloat lowertransition, 
                 RtFloat uppertransition, RtFloat maxvisible ),
   RiNopGeometricApproximation( RtToken type, RtFloat value ),
   RiNopGeometryV( RtToken type, 
               RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopOrientation( RtToken orientation ),
   RiNopProcedural( RtPointer data, RtBound bound,
                 RtVoid (*subdivfunc)(RtPointer, RtFloat),
                 RtVoid (*freefunc)(RtPointer) ),
   RiNopRelativeDetail( RtFloat relativedetail ),
   RiNopProcDelayedReadArchive( RtPointer data, RtFloat detail ),
   RiNopProcRunProgram( RtPointer data, RtFloat detail ),
   RiNopProcDynamicLoad( RtPointer data, RtFloat detail );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid
   RiNopMakeBumpV( char *picturename, char *texturename, 
                RtToken swrap, RtToken twrap,
                RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
                RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopMakeCubeFaceEnvironmentV( char *px, char *nx, char *py, char *ny,
                              char *pz, char *nz, char *texturename,
                               RtFloat fov,
                               RtFilterFunc filterfunc, 
                               RtFloat swidth, RtFloat twidth,
                               RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopMakeLatLongEnvironmentV( char *picturename, char *texturename, 
                              RtFilterFunc filterfunc,
                              RtFloat swidth, RtFloat twidth,
                              RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopMakeShadowV( char *picturename, char *texturename,
                 RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopMakeTextureV( char *picturename, char *texturename, 
                   RtToken swrap, RtToken twrap,
                   RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
                   RtInt n, RtToken tokens[], RtPointer parms[] );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid
   RiNopResource( RtToken handle, RtToken type, ... ),
   RiNopResourceV( RtToken handle, RtToken type,
		RtInt n, RtToken tokens[], RtPointer parms[] ),
   RiNopGeometricRepresentation( RtToken type );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtFloat
   RiNopBoxFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth ),
   RiNopTriangleFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth ),
   RiNopCatmullRomFilter( RtFloat x, RtFloat y, 
                        RtFloat xwidth, RtFloat ywidth ),
   RiNopGaussianFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth ),
   RiNopSincFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth );


#ifndef __RIB_FILE_RIBNOP
extern 
#endif
RtVoid RiNop_version( RtFloat version );


/* 
 *   Globals
 *
 */
#ifndef __RIB_FILE_RIBNOP
extern PRIB_RIPROC gRibNopRITable[];
#endif


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
/* endif RIBNOP_HEADER */
