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
 * FILE:  test3-7.c
 *
 * DESCRIPTION:  Gravity test for PRMan 3.7 compatability for libsribw.a.
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
 *             Renderman (R) is a registered trademark of Pixar
 */
#include <stdio.h>
#include <ripriv.h>

int test37( void );

int test37( void )
{
   RtInt nvertices[] = { 3, 3, 3 };
   RtInt vertices9[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
   RtPoint points9[] = {	 
    {0.,0.,0.}, {1.,1.,1.}, {2.,2.,2.}, {3.,3.,3.}, {4.,4.,4.}, 
    {5.,5.,5.}, {6.,6.,6.}, {7.,7.,7.}, {8.,8.,8.} };
   RtInt vertices3[] = { 0, 1, 2, 0, 1, 2, 0, 1, 2,};
   RtPoint points3[] = {	 
    {0.,0.,0.}, {1.,1.,1.}, {2.,2.,2.} };
   RtToken texturenames[] = { "tex1",  "tex2",  "tex3",
			      "tex4",  "tex5",  "tex6",
			      "tex7",  "tex8",  "tex9",
			      "tex10", "tex11", "tex12",
			      "tex13", "tex14", "tex15",
			      "tex16", "tex17", "tex18",
			      "tex19", "tex20", "tex21",
			      "tex22", "tex23", "tex24"
   };
   RtPoint verticesCrv3[] = { {1., 2., 3.}, {4., 5., 6.}, {7., 8., 9.1},  
			      {999., 999., 999.} 
   };
   RtPoint verticesCrv4[] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9},  
			      {10, 11, 12.1} 
   };
   RtPoint vertices15[] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9},  
			    {10, 11, 12}, {13, 14, 15}, {16, 17, 18}, 
			    {19, 20, 21}, {22, 23, 24}, {25, 26, 27}, 
			    {28, 29, 30}, {31, 32, 33}, {34, 35, 36}, 
			    {37, 38, 39}, {40, 41, 42}, {43, 44, 45.1}, 
			    /* Should not see the nines in the output. */
			    {999,999,999}
			   };
   RtPoint vertices21[] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9},  
			    {10, 11, 12}, {13, 14, 15}, {16, 17, 18}, 
			    {19, 20, 21}, {22, 23, 24}, {25, 26, 27}, 
			    {28, 29, 30}, {31, 32, 33}, {34, 35, 36}, 
			    {37, 38, 39}, {40, 41, 42}, {43, 44, 45}, 
			    {46, 47, 48}, {49, 50, 51}, {52, 53, 54}, 
			    {55, 56, 57}, {58, 59, 60}, {61, 62, 63.1},
			    /* Should not see the nines in the output. */
			    {999,999,999} 
			   };
   RtPoint verticesCrv22[] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9},  
			       {10, 11, 12}, {13, 14, 15}, {16, 17, 18}, 
			       {19, 20, 21}, {22, 23, 24}, {25, 26, 27}, 
			       {28, 29, 30}, {31, 32, 33}, {34, 35, 36}, 
			       {37, 38, 39}, {40, 41, 42}, {43, 44, 45}, 
			       {46, 47, 48}, {49, 50, 51}, {52, 53, 54}, 
			       {55, 56, 57}, {58, 59, 60}, {61, 62, 63},
			       {64, 65, 66.1},
			       /* Should not see the nines in the output. */
			       {999,999,999} 
   };
   RtInt periodic1[] = { 3 };
   RtInt nonperiodic1[] = { 4 };
   RtInt nonperiodic3[] = { 4, 7, 4 };
   RtInt nverticesCrv6[]= { 2, 2, 3, 4, 5, 6 };
   RtFloat width1[]= { 1.1 };
   RtFloat width4[]= { 1, 2, 3, 4.1 };
   RtFloat width7[]= { 1, 2, 3, 4, 5, 6, 7.1 };
   RtFloat widthCrv22[]= { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 
			14, 15, 16, 17, 18, 19, 20, 21, 22.123 };
   RtFloat constantwidth = 1.0;
   RtToken tokens[10];
   RtPointer values[10];


   RiBegin( RI_NULL );

   RiBasis( RiBezierBasis, 3, RiBezierBasis, 3 );

   RiPointsPolygons( 3, nvertices, vertices9, "P", points9,
		    "texturename", &texturenames, RI_NULL );
   RiPointsPolygons( 3, nvertices, vertices3, "P", points3,
		    "texturename", &texturenames, RI_NULL );

   RiPoints( 21, "P", vertices21, RI_NULL );

   /* 
    *   Test linear type curves. 
    */

   /* ncurves = 6
    * nvertices = 22
    * uniform = 22
    * varying = 22
    * vertex = 22
    */
   RiArchiveRecord( "comment", 
		    "\n#   linear periodic\n#" );
   RiArchiveRecord( "comment", 
		    "   P(22*3=66) width(22) constantwidth(1) "\
		    "texturename(22=tex22)" );
   RiCurves( "linear", 6, nverticesCrv6, "periodic", 
	     "P", verticesCrv22, 
	     "width", widthCrv22,
	     "constantwidth", &constantwidth,
	     "texturename", &texturenames,
	     RI_NULL );
   tokens[0] = "P"; values[0] = (RtPointer)verticesCrv22;
   tokens[1] = "width"; values[1] = (RtPointer)widthCrv22;
   tokens[2] = "constantwidth"; values[2] = (RtPointer)&constantwidth;
   tokens[3] = "texturename"; values[3] = (RtPointer)&texturenames;
   RiCurvesV( "linear", 6, nverticesCrv6, "periodic", 
	      4, tokens, values );
   
   /* ncurves = 6
    * nvertices = 22
    * uniform = 16
    * varying = 22
    * vertex = 22
    */
   RiArchiveRecord( "comment", "\n#   linear nonperiodic\n#" );
   RiArchiveRecord( "comment", 
		    "   P(22*3=66) width(22) constantwidth(1) "\
		    "texturename(16=tex16)" );
   RiCurves( "linear", 6, nverticesCrv6, "nonperiodic", 
	     "P", verticesCrv22, /* vertex */
	     "width", widthCrv22, /* varying */
	     "constantwidth", &constantwidth, /* constant */
	     "texturename", &texturenames, /* uniform */
	     RI_NULL );
   tokens[0] = "P"; values[0] = (RtPointer)verticesCrv22;
   tokens[1] = "width"; values[1] = (RtPointer)widthCrv22;
   tokens[2] = "constantwidth"; values[2] = (RtPointer)&constantwidth;
   tokens[3] = "texturename"; values[3] = (RtPointer)&texturenames;
   RiCurvesV( "linear", 6, nverticesCrv6, "nonperiodic", 
	     4, tokens, values );
   
   /* Test cubic type curves. */
   /* ncurves = 1
    * nvertices = 4
    * uniform = 1 
    * varying = 
    * vertex = 
    */
   RiArchiveRecord( "comment", 
		    "\n#   cubic nonperiodic\n#" );
   RiArchiveRecord( "comment", 
		    "   P(4*3=12) width(4) constantwidth(1) "\
		    "texturename(1=tex1)" );
   RiCurves( "cubic", 1, nonperiodic1, "nonperiodic", 
	    "P", verticesCrv4, 
	     "width", width4, 
	     "constantwidth", &constantwidth,
	     "texturename", &texturenames,
	     RI_NULL );
   tokens[0] = "P"; values[0] = (RtPointer)verticesCrv4;
   tokens[1] = "width"; values[1] = (RtPointer)width4;
   tokens[2] = "constantwidth"; values[2] = (RtPointer)&constantwidth;
   tokens[3] = "texturename"; values[3] = (RtPointer)&texturenames;
   RiCurvesV( "cubic", 1, nonperiodic1, "nonperiodic", 
	     4, tokens, values );



   /* ncurves = 1
    * nvertices = 3
    * uniform = 1
    * varying = 1
    * vertex = 3
    */
   RiArchiveRecord( "comment", 
		    "\n#   cubic periodic\n#" );
   RiArchiveRecord( "comment", 
		    "   P(3*3=9) width(1) constantwidth(1) "\
		    "texturename(1=tex1)" );
   RiCurves( "cubic", 1, periodic1, "periodic", 
	     "P", verticesCrv3, "width", width1,
	     "constantwidth", &constantwidth,
	     "texturename", &texturenames,
	     RI_NULL );
   tokens[0] = "P"; values[0] = (RtPointer)verticesCrv3;
   tokens[1] = "width"; values[1] = (RtPointer)width1;
   tokens[2] = "constantwidth"; values[2] = (RtPointer)&constantwidth;
   tokens[3] = "texturename"; values[3] = (RtPointer)&texturenames;
   RiCurvesV( "cubic", 1, periodic1, "periodic", 
	     4, tokens, values );

   /* ncurves = 3 (4,7,4)
    * nvertices = 15
    * uniform = 4
    * varying = 7
    * vertex = 15
    */
   RiArchiveRecord( "comment", 
		    "\n#   cubic nonperiodic\n#" );
   RiArchiveRecord( "comment", 
		    "   P(4*3+7*3+4*3=45) width(7) constantwidth(1) "\
		    "texturename(4=tex4)" );
   RiCurves( "cubic", 3, nonperiodic3, "nonperiodic", 
	    "P", vertices15, "width", width7,
	    "constantwidth", &constantwidth,
	     "texturename", &texturenames,
	    RI_NULL );

   RiEnd();

   return 0;
}

