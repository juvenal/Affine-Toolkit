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
 * FILE:  test.c
 *
 * DESCRIPTION:  Gravity test for libsribw.a.
 *   
 *    Contains:
 *  
 *    History:
 *      01-03-98  Added texturenames array for RiPointsGeneralPolygonsV test.
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
 *             Renderman (R) is a registered trademark of Pixar
 */
#include <stdio.h>
#include <ripriv.h>
#include "write.h"

extern int test37( void );
extern PRIVATESTATEDATA gSRIBW;

/* Set SRIBW_TESTS 1 to do privtate tests for libsribw.a. */
#define SRIBW_TESTS 0

/* Set to 1 to do RiOption() and RiOptionV() tests before RiBegin(). */
#define RIOPTION_TESTS 1


int main(int argc, char **argv)
{
#if !SRIBW_TESTS
   RtBound bound = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };
   RtFloat intensity = 1;
   RtColor varyingcolor[] = { 
      {0.1,0.1,0.1}, {0.1,0.1,0.1}, 
      {0.1,0.1,0.1}, {0.1,0.1,0.1} };
   RtColor varyingcolor12[] = { 
    {0.,0.,0.}, {1.,1.,1.}, {2.,2.,2.}, {3.,3.,3.}, {4.,4.,4.}, 
    {5.,5.,5.}, {6.,6.,6.}, {7.,7.,7.}, {8.,8.,8.}, {9.,9.,9.},
    {10.,10.,10.}, {11.,11.,11.} };
   RtFloat nRGB[] = { 1, 2, 3, 4, 5, 6 };
   RtFloat RGBn[] = { 7, 8, 9, 10, 11, 12 };
   RtFloat RGB2RGB[] = {   
      1, 1, 1,  1, 1, 1,  1, 1, 1, 
      1, 1, 1,  1, 1, 1,  1, 1, 1, 
      1, 1, 1,  1, 1, 1,  1, 1, 1  };
   RtFloat singlecolor[] = { 123, 456 };
   RtInt nloops[] = { 1, 2 };
   RtInt nvertices[] = { 3, 4, 5 };
   RtInt vertices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
   RtPoint points[] = {	 
    {0.,0.,0.}, {1.,1.,1.}, {2.,2.,2.}, {3.,3.,3.}, {4.,4.,4.}, 
    {5.,5.,5.}, {6.,6.,6.}, {7.,7.,7.}, {8.,8.,8.}, {9.,9.,9.}, 
    {10.,10.,10.}, {11.,11.,11.} };
   RtPoint vpt3[] = {
    {0.,0.,0.},   {0.,0.,1.},    {0.,0.,2.}, 
    {1.,1.,0.},   {1.,1.,1.},    {1.,1.,2.}, 
    {2.,2.,0.},   {2.,2.,1.},    {2.,2.,2.}, 
    {3.,3.,0.},   {3.,3.,1.},    {3.,3.,2.}, 
    {4.,4.,0.},   {4.,4.,1.},    {4.,4.,2.}, 
    {5.,5.,0.},   {5.,5.,1.},    {5.,5.,2.}, 
    {6.,6.,0.},   {6.,6.,1.},    {6.,6.,2.}, 
    {7.,7.,0.},   {7.,7.,1.},    {7.,7.,2.}, 
    {8.,8.,0.},   {8.,8.,1.},    {8.,8.,2.}, 
    {9.,9.,0.},   {9.,9.,1.},    {9.,9.,2.}, 
    {10.,10.,0.}, {10.,10.,1.},  {10.,10.,2.}, 
    {11.,11.,0.}, {11.,11.,11.}, {11.,11.,2.} 
   };
   RtInt singleint1[] = { 1 };
   RtInt singleint2[] = { 2 };
   RtInt singleint11[] = { 11 };
   RtFloat singlefloat0[] = { 0.0 };
   RtFloat thirteenfloats[] = {
      0.,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.};
   RtFloat eightfloats[] = {0.,1.,2.,3.,4.,5.,6.,7.}; 
   RtFloat elevenfloats[] = {0.,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.}; 
   RtFloat twelvefloats[] = {0.,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.}; 
   RtFloat umpteenhpoints[] = {
      0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
      1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 
      2., 2., 2., 2., 2., 2., 2., 2., 2., 2., 2., 2., 
      3., 3., 3., 3., 3., 3., 3., 3., 3., 3., 3., 3., 
      4., 4., 4., 4., 4., 4., 4., 4., 4., 4., 4., 4., 
      5., 5., 5., 5., 5., 5., 5., 5., 5., 5., 5., 5., 
      6., 6., 6., 6., 6., 6., 6., 6., 6., 6., 6., 6., 
      7., 7., 7., 7., 7., 7., 7., 7., 7., 7., 7., 7., 
      8., 8., 8., 8., 8., 8., 8., 8., 8., 8., 8., 8., 
      9., 9., 9., 9., 9., 9., 9., 9., 9., 9., 9., 9., 
      10., 10., 10., 10., 10., 10., 10., 10., 10., 10., 10., 10., 
      11., 11., 11., 11., 11., 11., 11., 11., 11., 11., 11., 11., 
      12., 12., 12., 12., 12., 12., 12., 12., 12., 12., 12., 12., 
      13., 13., 13., 13., 13., 13., 13., 13., 13., 13., 13., 13., 
      14., 14., 14., 14., 14., 14., 14., 14., 14., 14., 14., 14. 
   };
   RtToken tokens[10];
   RtPointer values[10];
   char *texturename = "texturename";
   char *texturenames[] = {"texturename1","texturename2"};
#if RIOPTION_TESTS
   char *qwertyuiop[] = {"qwertyuiop"};
#endif


#if RIOPTION_TESTS
   tokens[0] = "format";
   values[0] = (RtPointer)&qwertyuiop;
   tokens[1] = "compression";
   values[1] = (RtPointer)&qwertyuiop;

   RiOptionV( "rib", 2, tokens, values );
   RiBegin( RI_NULL );
   RiEnd();

   RiOption( "rib", "format", qwertyuiop, RI_NULL );
   RiOption( "rib", "format", qwertyuiop, "compression", qwertyuiop, RI_NULL );
   RiBegin( RI_NULL );
   RiEnd();
#endif

   RiBegin( RI_NULL );
   
   RiArchiveRecord( RI_STRUCTURE, "%s", "arcrec-struct" );

   RiDisplay("a.tif", "framebuffer", RI_RGBA, RI_NULL);
   RiFormat((RtInt) 250, (RtInt) 250, (RtFloat) 1.0);
   RiProjection( RI_PERSPECTIVE, RI_NULL );
   RiDepthOfField( RI_INFINITY, 0.0, 0.0 );
   RiTranslate(0.0, 0.0, 3.5);
   RiRotate( 90., 1., 0., 0. );

   RiGeometricRepresentation( "lines" );

   RiWorldBegin();

   RiLightSource( "distantlight", "intensity", &intensity, RI_NULL ); 
   RiLightSource( "ambientlight", "intensity", &intensity, RI_NULL );

   RiBasis( RiBezierBasis, 3, RiPowerBasis, 4 );

   RiSphere( 1., -1., 1., 360., "Cs", varyingcolor, RI_NULL ); 
   RiSphereV( 1., -0.8, 0.8, 36., 0, NULL, NULL );  

   /* Note that catrib 3.7 gives errors when changing the number of color
    *    samples to something other than 3.  
    * Example message produced using test.script:
    *
    *  P08001 Wrong number of color samples: 2, expecting: 3. 
    *     (at line 17 in stdin)
    */  
   RiColorSamples( 2, nRGB, RGBn );
   RiColor( singlecolor );

   RiPointsGeneralPolygons( 2, nloops, nvertices, vertices, 
			   RI_P, points, RI_NULL );
   tokens[0] = RI_P;
   values[0] = (RtPointer)points;
   tokens[1] = "texturename";
   values[1] = (RtPointer)&texturenames;
   RiPointsGeneralPolygonsV( 2, nloops, nvertices, vertices, 
			    2, tokens, values );

   RiPointsPolygons( 2, nvertices, vertices, RI_P, points, RI_NULL );
   tokens[0] = RI_P;
   values[0] = (RtPointer)points;
   RiPointsPolygonsV( 2, nvertices, vertices, 1, tokens, values );
   RiPolygon( 5, RI_P, points, RI_NULL );
   tokens[0] = RI_P;
   values[0] = (RtPointer)points;
   RiPolygonV( 5, 1, tokens, values );

   RiGeneralPolygon( 2, nvertices, RI_P, points, RI_NULL );
   tokens[0] = RI_P;
   values[0] = (RtPointer)points;
   RiGeneralPolygonV( 2, nvertices, 1, tokens, values );

   RiDeclare( "uniformpoint3", "varying point[3]" );
   RiGeneralPolygon( 2, nvertices, RI_P, points, 
		    "uniformpoint3", vpt3, RI_N, points, RI_NULL );

   RiBound( bound );
   RiDetail( bound );
   RiDetailRange( 0.0, 1.1, 2.2, 3.3 );
   RiGeometricApproximation( "flatness", 2.5 );
   RiReverseOrientation();
   RiSides(1);
   RiSides(2);
   RiTextureCoordinates(1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8);
   RiShadingInterpolation("constant");
   RiShadingInterpolation("bob");
   RiAreaLightSource( "Bob", "intensity", &intensity, 
		     "texturename", &texturename, RI_NULL );
   RiTrimCurve( 1, singleint1, singleint2, thirteenfloats, singlefloat0,
	       singlefloat0, singleint11, elevenfloats, elevenfloats,
	       elevenfloats );
   RiNuPatch( 9, 3, twelvefloats, 0., 1., 5, 3, eightfloats, 0., 1.,
	     "Pw", umpteenhpoints, RI_NULL );
   tokens[0] = RI_PW;
   values[0] = (RtPointer)umpteenhpoints;
   RiNuPatchV( 9, 3, twelvefloats, 0., 1., 5, 3, eightfloats, 0., 1.,
	      1, tokens, values );

   /* Reset ColorSamples. */
   RiColorSamples( 3, RGB2RGB, RGB2RGB ); 

   tokens[0] = "P";
   values[0] = (RtPointer)points;
   tokens[1] = "Cs";
   values[1] = (RtPointer)varyingcolor12;
   RiPatchMeshV( "bilinear", 4, "nonperiodic", 3, "periodic", 
	       2, tokens, values );

   /* Test returning graphics state when dealing with Basis statements. */
   RiAttributeBegin();
   RiBasis( RiHermiteBasis, 2, RiPowerBasis, 4 );
   /*
   RiPatchMesh( "bicubic", 4, "nonperiodic", 3, "periodic", 
	       "P", points, "Cs", varyingcolor12, RI_NULL );
	       */
   RiAttributeEnd();

   /* Should match the RiPatchMeshV() call above. */
   RiPatchMesh( "bilinear", 4, "nonperiodic", 3, "periodic", 
	       "P", points, "Cs", varyingcolor12, RI_NULL );

   RiWorldEnd();

   RiEnd();

   RiBegin( "risphere.rib" ); 
   RiDisplay("a.tif", "framebuffer", "rgba", RI_NULL);
   RiFormat((RtInt) 250, (RtInt) 250, (RtFloat) 1.0);
   RiProjection( RI_ORTHOGRAPHIC, RI_NULL );
   RiTranslate(0.0, 0.0, 3.5);
   RiRotate( 90., 1., 0., 0. );

   /* Note 04-04-98: The following problem is also with 3.7f. */
   /* Pixar's librib.a (3.6c) doesn't appear to reset itself after being 
    *    called once.  Inshort the ColorSamples call above remains in effect 
    *    and causes librib.a (3.6c) to print:
    *
    * Sphere 1 -1 1 360 "Cs" [0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1]
    *
    * instead of printing:
    *
    *  Sphere 1 -1 1 360 "Cs" [0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1]
    *
    * To get around this problem the following ColorSamples call was added
    *   above:
    *
    *        RiColorSamples( 3, RGB2RGB, RGB2RGB ); 
    *
    */

   /* BMRT's libribout.a (2.3.5) appears to ignore RiColorSamples(). */

   RiSphere( 1., -1., 1., 360., "Cs", varyingcolor, RI_NULL ); 

   RiArchiveRecord( "structure", "End of first tests." );
   RiEnd();

   /* Test ##RenderMan comment code.  To really test it refer to 
    *    example calls to catrib shown in write.c's RiARchiveRecord().
    *    Use those test on typerib.
    */
   RiBegin( RI_NULL );
   RiArchiveRecord( RI_STRUCTURE, "RenderMan!", "arcrec-struct" );
   RiArchiveRecord( RI_COMMENT, 
		    " Should only be one ##RenderMan a couple lines above." );
   RiArchiveRecord( RI_COMMENT, 
		    " It should have an '!' appended." );
   RiArchiveRecord( "structure", "End of second tests." );
   RiEnd();
#endif

#if SRIBW_TESTS
#define  CheckSteps(u,v)  if (gSRIBW.nustep!=(u) || gSRIBW.nvstep!=(v) )   \
              printf( "#ERROR: uvsteps are incorrect\n"                    \
                      "#       expected ustep:%d vstep:%d (%s %d).\n",     \
	              (u), (v), __FILE__,__LINE__ );                       \
              else printf( "# uvstep test OK.\n" )
#define  PrintUVSteps()  printf( "#ustep:%d vstep:%d\n",         \
                                 gSRIBW.nustep, gSRIBW.nvstep )
   /* Test scope of RiBasis() with RiAttributeBegin/End(). */
   RiBegin( RI_NULL );
   CheckSteps(3,3);
   PrintUVSteps();
   RiAttributeBegin();

   RiBasis( RiHermiteBasis, 2, RiPowerBasis, 4 );
   CheckSteps(2,4);
   PrintUVSteps();
   RiAttributeEnd();
   CheckSteps(3,3);
   PrintUVSteps();
   RiBasis( RiPowerBasis, 4, RiHermiteBasis, 2 );
   RiAttributeBegin();   
   RiAttributeBegin();
   CheckSteps(4,2);
   PrintUVSteps();
   RiAttributeEnd();
   RiAttributeEnd();
   CheckSteps(4,2);
   PrintUVSteps();
   RiEnd();
   RiBegin( RI_NULL );
   CheckSteps(3,3);
   PrintUVSteps();

   RiEnd();
#endif

#if !SRIBW_TESTS
   test37();
#endif

   return 0;
}

