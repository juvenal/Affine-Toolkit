/* 
 * Copyright (c) 1997, 1998 Thomas E. Burge.  All rights reserved.
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
 * FILE:  trimtest.c
 *
 * DESCRIPTION:  Private tests for the RIB Trim Curve Storage Library.
 *   
 *    Uses:      
 *
 *    Contains:
 *       int main( void );
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define RIB_INCLUDE_RITABLE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ripriv.h>
#include <ributil.h>
#include <ribtrimstore.h>


RtVoid TestTrimCurve( RtInt nloops, 
		     RtInt ncurves[], RtInt order[], RtFloat knot[],
		     RtFloat min[], RtFloat max[], RtInt n[],
		     RtFloat u[], RtFloat v[], RtFloat w[] );
int main(int argc, char **argv);


RtInt   ncurves[] = { 1 };
RtInt   order[] = { 2 };
RtFloat knot[] = { 0., 0., 1., 1. };
RtFloat min[] = { 0. };
RtFloat max[] = { 1. };
RtInt   n[] = { 2 };
RtFloat u1[] = { 1., 1. };
RtFloat u2[] = { 2., 2. };
RtFloat u3[] = { 3., 3. };
RtFloat v[] = { 1., 1. };
RtFloat w[] = { 1., 1.};
int gExpectedLevel;
int gExpectedNCalls;

int main(int argc, char **argv) 
{
   PTRIMSTORE  t;
   PTRIMCURVE  tt,curve;


   printf( "# Gravity Tests For trimstore.c\n" );
   printf( "# -----------------------------\n" );

   t = TrimStoreInitRITable( NULL, 0 );

   if (TrimStoreIsaTrimmedSurface(t))
     printf("ERROR:  "\
	    "TrimStoreIsaTrimmedSurface(t) should have returned false." \
	    "(%d)\n", __LINE__ );

   if (TrimStoreBeginBlock(t))
     printf("ERROR:  "\
	    "TrimStoreBeginBlock(t) returned error. (%d)\n", __LINE__ );

   tt = TrimStoreCurves( t, 1, ncurves, order, knot, min, max, n, u1, v, w );
   if (!tt)
     printf("ERROR:  "\
	    "TrimStoreCurves(t,...) returned error.\n" );

   if (!TrimStoreIsaTrimmedSurface(t))
     printf("ERROR:  "\
	    "TrimStoreIsaTrimmedSurface(t) should have returned true." \
	    "(%d)\n", __LINE__ );

   curve = TrimStoreGetLastCurve(t);
   if (tt!=curve)
     printf("ERROR:  "\
	    "TrimStoreGetLastCurve(t)%p!=lastcurve:%p." \
	    "(%d)\n", tt, curve, __LINE__ );

   curve = TrimStoreGetFirstCurve(t);
   if (tt!=curve)
     printf("ERROR:  "\
	    "TrimStoreGetFirstCurve(t)%p!=firstcurve:%p." \
	    "(%d)\n", tt, curve, __LINE__ );

   if (TrimStoreBeginBlock(t))
     printf("ERROR:  "\
	    "TrimStoreBeginBlock(t) returned error. (%d)\n", __LINE__ );

   tt = TrimStoreCurves( t, 1, ncurves, order, knot, min, max, n, u2, v, w );
   if (!tt)
     printf("ERROR:  "\
	    "TrimStoreCurves(t,...) returned error.\n" );

   t->ricalls[TS_TRIMCURVE] = (PRIB_RIPROC)TestTrimCurve;
   gExpectedLevel = t->presentlevel;
   gExpectedNCalls = 2;
   if (TrimStoreApplyCurves(t))
     printf("ERROR:  "\
	    "TrimStoreApplyCurves(t) returned error. (%d)\n", __LINE__ );
   if (gExpectedNCalls)
     printf("ERROR:  Missing a trim curve. (%d)\n", __LINE__ );

   tt = TrimStoreCurves( t, 1, ncurves, order, knot, min, max, n, u2, v, w );
   if (!tt)
     printf("ERROR:  "\
	    "TrimStoreCurves(t,...) returned error.\n" );

   printf("Test applying only unapplied curves.\n");
   gExpectedLevel = t->presentlevel;
   gExpectedNCalls = 1;
   if (TrimStoreApplyCurves(t))
     printf("ERROR:  "\
	    "TrimStoreApplyCurves(t) returned error. (%d)\n", __LINE__ );
   if (gExpectedNCalls)
     printf("ERROR:  Missing a trim curve. (%d)\n", __LINE__ );

   if (TrimStoreEndBlock(t))
     printf("ERROR:  "\
	    "TrimStoreEndBlock(t) returned error. (%d)\n", __LINE__ );

   printf("Testing that one curve gets applied.\n");
   gExpectedLevel = t->presentlevel;
   gExpectedNCalls = 1;
   if (TrimStoreApplyCurves(t))
     printf("ERROR:  "\
	    "TrimStoreApplyCurves(t) returned error. (%d)\n", __LINE__ );
   if (gExpectedNCalls)
     printf("ERROR:  Missing a trim curve. (%d)\n", __LINE__ );

   printf("Testing that all curves get applied.\n");
   TrimStoreSetFlags( t, TRIMSTORE_APPLYALL, TRIMSTORE_APPLYALL );
   gExpectedLevel = t->presentlevel;
   gExpectedNCalls = 1;
   if (TrimStoreApplyCurves(t))
     printf("ERROR:  "\
	    "TrimStoreApplyCurves(t) returned error. (%d)\n", __LINE__ );
   if (gExpectedNCalls)
     printf("ERROR:  Missing a trim curve. (%d)\n", __LINE__ );

   tt = TrimStoreCurves( t, 1, ncurves, order, knot, min, max, n, u3, v, w );
   if (!tt)
     printf("ERROR:  "\
	    "TrimStoreCurves(t,...) returned error.\n" );

   if (TrimStoreEndBlock(t))
     printf("ERROR:  "\
	    "TrimStoreEndBlock(t) returned error. (%d)\n", __LINE__ );

   if (TrimStoreDestroy(t) )
     printf("ERROR:  "\
	    "TrimStoreDestroy(t) returned error. (%d)\n", __LINE__ );

#ifdef RIB_MEMORY_TESTS
   if (RibMemoryTestResults( gPMemTest ))
     printf("No memory test results\n" );
#endif

   return 0;
}


RtVoid TestTrimCurve( RtInt nloops, 
		     RtInt ncurves[], RtInt order[], RtFloat knot[],
		     RtFloat min[], RtFloat max[], RtInt n[],
		     RtFloat u[], RtFloat v[], RtFloat w[] )
{
   (void)nloops;
   (void)ncurves;  (void)order;  (void)knot;
   (void)min;      (void)max;    (void)n;
   (void)v;        (void)w;

   if (gExpectedLevel!=u[0])
     printf("ERROR:  Expected level:%d got %g,\n",gExpectedLevel,u[0]);
   gExpectedLevel--;

   if (gExpectedNCalls<=0)
     printf("ERROR:  Received an extra trimcurve call.\n" );
   gExpectedNCalls--;
}
