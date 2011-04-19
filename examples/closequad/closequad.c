/* 
 * Copyright (c) 1998-1999 Thomas E. Burge.  All rights reserved.  
 * 
 * Affine is a trademark of Thomas E. Burge
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
 *                  The Affine Libraries and Tools are 
 *                Copyright (c) 1995-1999 Thomas E. Burge  
 *                          All rights reserved.
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
 * FILE:  closequad.c
 *
 * DESCRIPTION:  Takes quadrics and closes them to make solids.
 *               When needed disks, bilinear and bicubic patches 
 *               are added to make the quadric represent a solid.
 *
 *    Contains:
 *  
 *     History:
 *        07-06-98  Created based on some previously written quadric split
 *                  written a few years back based on the references listed
 *                  below.
 *        10-17-99  Fixed update made a while back to have RibOpen() with
 *                  a table length parameter.
 *
 *    References:
 *        [FAUX79]  Faux, I. D. and Pratt, M. J., Computational Geometry 
 *                  for Design and Manufacture, Ellis Horwood Ltd., 
 *                  Chichester England, p. 134 1979.
 *        [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, Richmond, CA, 
 *                  September 1989.
 *        [UPST90]  Upstill, Steve, The RenderMan Companion: A Programmer's
 *                  Guide to Realistic Computer Graphics, Addison Wesley, 
 *                  1990, pp. 129.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define  _RIPRIV_FUNC_TYPES 
#include <string.h>
#include <math.h>
#include <ributil.h>


#define X 0
#define Y 1
#define Z 2


int main(int argc, char **argv); 
void PrintHelp( void );
void PrintError( char *file );
int CloseQuadric( char *input, char* output, RtBoolean readarchives );


RtVoid ClosedConeV( RtFloat height, RtFloat radius, RtFloat thetamax,
		    RtInt n, RtToken tokens[], RtPointer parms[] );
RtVoid ClosedCylinderV( RtFloat radius,RtFloat zmin,RtFloat zmax,
			RtFloat thetamax,
			RtInt n, RtToken tokens[], RtPointer parms[] );
RtVoid ClosedHyperboloidV( RtPoint point1, RtPoint point2, RtFloat thetamax,
			   RtInt n, RtToken tokens[], RtPointer parms[] );
RtVoid ClosedParaboloidV( RtFloat rmax,RtFloat zmin,RtFloat zmax,
			  RtFloat thetamax,
			  RtInt n, RtToken tokens[], RtPointer parms[] );
RtVoid ClosedSphereV( RtFloat radius, RtFloat zmin, RtFloat zmax, 
		      RtFloat thetamax,
		      RtInt n, RtToken tokens[], RtPointer parms[] );
RtVoid ClosedTorusV( RtFloat majorradius, RtFloat minorradius, 
		     RtFloat phimin, RtFloat phimax, RtFloat thetamax, 
		     RtInt n, RtToken tokens[], RtPointer parms[] ); 

/*
 * Have rib handle set up as a global variable.
 *
 */
RIB_HANDLE  rib;

#define DEG_TO_RAD   (3.141592654/180.0)
#define RAD_TO_DEG   (180.0/3.141592654)
#define PI           (3.141592654)
#define HALF_PI      (3.141592654/2.0)
#define UNIT_CIRCLE  (0.5522847)


int main(int argc, char **argv)
{
   char       *outputfilename = NULL;
   char       *fout = RI_NULL;
   RtBoolean  readarchives = RI_FALSE;
   int        i,n;

   
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
	 if ( (i+1<argc) && argv[i][1]=='o' && argv[i][2]=='\0' )
	 {
	    i++;
	    if ( fout == RI_NULL )
	    {
	       outputfilename = argv[i];
	       fout = argv[i];
	    }
	    else
	    {
	       printf( "Error: Option -o specified more than once.\n" );
	       goto CommandLineError;
	    }
	 }
	 else if (!strcmp(&argv[i][1],"r"))
	 {
	    readarchives = RI_TRUE;
	 }
	 else
	 {
	    goto CommandLineError;
	 }
      }
      else
      {
	 break;
      }
      i++;
   }
   
   /* Check that none of the input file names matches the output file name. */
   if ( i < argc && outputfilename )
   {
      n = i;
      while ( n < argc )
      {
	 if (!strcmp(argv[n],outputfilename))
	 {
	    fprintf( stderr, 
		    "Output filename \"%s\" matches an input filename.\n\n",
		    argv[n] );
	    goto CommandLineError;
	 }
	 n++;
      }
   }

   gRibRITable[kRIB_CONE]        = (PRIB_RIPROC)ClosedConeV;
   gRibRITable[kRIB_CYLINDER]    = (PRIB_RIPROC)ClosedCylinderV;
   gRibRITable[kRIB_HYPERBOLOID] = (PRIB_RIPROC)ClosedHyperboloidV;
   gRibRITable[kRIB_PARABOLOID]  = (PRIB_RIPROC)ClosedParaboloidV;
   gRibRITable[kRIB_SPHERE]      = (PRIB_RIPROC)ClosedSphereV;
   gRibRITable[kRIB_TORUS]       = (PRIB_RIPROC)ClosedTorusV;

   if ( i < argc )
   {
      while ( i < argc )
      {
	 CloseQuadric( argv[i], fout, readarchives );
	 i++;
      }     
   }
   else
   {
      CloseQuadric( argv[i], fout, readarchives );
   }

   return 0;
   
 CommandLineError:
   PrintHelp();
   return 1;
}


void PrintHelp( void )
{
  printf( 
"closequad [-o file] [-r] [filename . . .]\n"                              \
"   [-o file]          Output file name.  If not given, stdout is used.\n" \
"   [-r]               Recursively expand ReadArchive statements.\n"       \
"   [filename . . .]   If no file names are given then closequad will\n"     \
"                      use standard input.\n" );
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}


int CloseQuadric( char *input, char* output, RtBoolean readarchives  )
{
   int  err = kRIB_OK;


   rib = RibOpen( input, kRIB_LAST_RI, gRibRITable );
   if (!rib)
   {
      PrintError( input );
      return 1;
   }
   if ( readarchives )
   {
      RibSetOptions( rib, 
		     kRIB_OPTION_READ_SUBFILES, kRIB_OPTION_READ_SUBFILES );
   }
   if ( output )
   {
      if (RibBegin( rib, output ))
      {
	 PrintError( output );
	 return 1;	
      }
   }
   if (RibRead(rib))
   {
      err = 1;
   }
   RibClose(rib);

   return err;
}


RtVoid ClosedConeV( RtFloat height, RtFloat radius, RtFloat thetamax,
		   RtInt n, RtToken tokens[], RtPointer parms[] )
{
   static RtPoint  P[4];


   /* Could call RiCylinder() with a degenerate top or could call 
    *    RiHyperboloid().  
    * For this program we'll put the code here in this function. 
    */
   RiConeV( height, radius, thetamax, n, tokens, parms ); 

   /* Bottom. */
   RiAttributeBegin();
   RiReverseOrientation();
   RiDiskV( 0, radius, thetamax, n, tokens, parms ); 
   RiAttributeEnd();

   /* Don't add patches for wedge shape if they are not needed. */
   if (thetamax==360.0)
      return;

   /* Increase the vector data by one so a positional array "P" can be
    *   added.  Don't do this unless it was the Affine RIB Reader that
    *   setup the tokens[] and parms[] arrays.
    */
   if (RibCacheVector( rib, 1, &tokens, &parms ) )
      return; /* Oops, out of memory.  Just return. */

   /* Now add in the positional data. */
   tokens[n] = RI_P;
   parms[n] = (RtPointer)P;
   n++;

   P[0][X] = 0.0;    P[0][Y] = 0.0; P[0][Z] = height; 
   P[1][X] = 0.0;    P[1][Y] = 0.0; P[1][Z] = height; 
   P[2][X] = radius; P[2][Y] = 0.0; P[2][Z] = 0.0; 
   P[3][X] = 0.0;    P[3][Y] = 0.0; P[3][Z] = 0.0; 

   RiPatchV( RI_BILINEAR, n, tokens, parms ); 

   RiAttributeBegin();
   RiRotate( thetamax, 0, 0, 1 );
   RiReverseOrientation();
   RiPatchV( RI_BILINEAR, n, tokens, parms ); 
   RiAttributeEnd();
}


RtVoid ClosedCylinderV( RtFloat radius,RtFloat zmin,RtFloat zmax,
		       RtFloat thetamax,
		       RtInt n, RtToken tokens[], RtPointer parms[] )
{
   static RtPoint  P[4];


   if ( zmin == zmax )
   {
      RiArchiveRecord( RI_COMMENT, 
		       "WARNING:  Invalid zmin Cylinder (line:%u).",
		       RibGetLineCount(rib) );
      return;
   }

   /* Could call RiHyperboloid() to create a cylinder, but
    *    for this program we'll put the code here in this function. 
    */
   RiCylinderV( radius, zmin, zmax, thetamax, n, tokens, parms );

   /* Top. */
   RiDiskV( zmax, radius, thetamax, n, tokens, parms ); 

   /* Bottom. */
   RiAttributeBegin();
   RiReverseOrientation();
   RiDiskV( zmin, radius, thetamax, n, tokens, parms ); 
   RiAttributeEnd();

   /* Don't add patches for wedge shape if they are not needed. */
   if (thetamax==360.0)
      return;

   /* Increase the vector data by one so a positional array "P" can be
    *   added.  Don't do this unless it was the Affine RIB Reader that
    *   setup the tokens[] and parms[] arrays.
    */
   if (RibCacheVector( rib, 1, &tokens, &parms ) )
      return; /* Oops, out of memory.  Just return. */

   /* Now add in the positional data. */
   tokens[n] = RI_P;
   parms[n] = (RtPointer)P;
   n++;

   P[0][X] = 0.0;    P[0][Y] = 0.0; P[0][Z] = zmin; 
   P[1][X] = radius; P[1][Y] = 0.0; P[1][Z] = zmin; 
   P[2][X] = 0.0;    P[2][Y] = 0.0; P[2][Z] = zmax; 
   P[3][X] = radius; P[3][Y] = 0.0; P[3][Z] = zmax; 

   RiPatchV( RI_BILINEAR, n, tokens, parms ); 
   
   RiAttributeBegin();
   RiRotate( thetamax, 0, 0, 1 );
   RiReverseOrientation();
   RiPatchV( RI_BILINEAR, n, tokens, parms ); 
   RiAttributeEnd();
   
}


RtFloat CalcAngle( RtFloat x, RtFloat y );
RtFloat CalcAngle( RtFloat x, RtFloat y )
{
   RtFloat  angle;

   angle = RAD_TO_DEG * acos( x/sqrt( x*x + y*y ) );
   if ( y < 0.0 )
      /*      angle += 90;*/
      angle = -angle;

   return angle;
}


RtVoid ClosedHyperboloidV( RtPoint point1, RtPoint point2, RtFloat thetamax,
			  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   static RtPoint  P1[4];
   static RtPoint  P2[4];
   RtBoolean       twisted = RI_FALSE;
   RtFloat         angle, radius, z;


   if (thetamax==0.0)
      return;

   RiHyperboloidV( point1, point2, thetamax, n, tokens, parms );

   /* Hyperboloid is really a disk.  Nothing more can be added. */
   if ( point1[Z]==point2[Z] )
      return;

   /* Create cap. */
   angle = CalcAngle(point2[X],point2[Y]);
   radius = sqrt(point2[X]*point2[X]+point2[Y]*point2[Y]);
   RiTransformBegin();
   RiRotate( angle, 0, 0, 1 );
   RiDiskV( point2[Z], radius, thetamax, n, tokens, parms ); 
   RiTransformEnd();

   /* Create bottom. */
   angle = CalcAngle(point1[X],point1[Y]);
   radius = sqrt(point1[X]*point1[X]+point1[Y]*point1[Y]);
   RiAttributeBegin();
   RiRotate( angle, 0, 0, 1 );
   RiReverseOrientation();
   RiDiskV( point1[Z], radius, thetamax, n, tokens, parms ); 
   RiAttributeEnd();

   /* Don't add patches for wedge shape if they are not needed. */
   if (thetamax==360.0)
      return;

   /* Increase the vector data by one so a positional array "P" can be
    *   added.  Don't do this unless it was the Affine RIB Reader that
    *   setup the tokens[] and parms[] arrays.
    */
   if (RibCacheVector( rib, 1, &tokens, &parms ) )
      return; /* Oops, out of memory.  Just return. */

   /* One patch from a hyperboloid's straight edge to the vertical axis
    *    might get twisted if the line between point1 and point2 goes
    *    through the Z axis.
    * A twist would result in the normal being reversed and pointing into
    *    the solid on one half of the twisted patch, so use use two patches.
    */
   if ( (point1[Y] < 0.0 && point2[Y] > 0.0
	 || point2[Y] < 0.0 && point1[Y] > 0.0 )
	&& fabs((point1[X]/point1[Y]) + (point2[X]/point2[Y])) < 1E-05 )
   {
      z = point1[Z] - (point2[Z] * point1[Y])/point2[Y];
      twisted = RI_TRUE;
   }
   else if ( (point1[X] < 0.0 && point2[X] > 0.0 
	      || point2[X] < 0.0 && point1[X] > 0.0 )
	     && fabs((point1[Y]/point1[X]) + (point2[Y]/point2[X])) < 1E-05 )
   {
      z = point1[Z] - (point2[Z] * point1[X])/point2[X];
      twisted = RI_TRUE;
   }

   /* Check if hyperboloid is twisted at one point. */
   if (twisted)
   {
      P1[0][X] = 0.0;        P1[0][Y] = 0.0;       P1[0][Z] = z; 
      P1[1][X] = 0.0;        P1[1][Y] = 0.0;       P1[1][Z] = z; 
      P1[2][X] = point1[X];  P1[2][Y] = point1[Y]; P1[2][Z] = point1[Z];
      P1[3][X] = 0.0;        P1[3][Y] = 0.0;       P1[3][Z] = point1[Z]; 

      P2[0][X] = point2[X];  P2[0][Y] = point2[Y]; P2[0][Z] = point2[Z]; 
      P2[1][X] = 0.0;        P2[1][Y] = 0.0;       P2[1][Z] = point2[Z]; 
      P2[2][X] = 0.0;        P2[2][Y] = 0.0;       P2[2][Z] = z;
      P2[3][X] = 0.0;        P2[3][Y] = 0.0;       P2[3][Z] = z;
   }
   else
   {
      P1[0][X] = point2[X];  P1[0][Y] = point2[Y]; P1[0][Z] = point2[Z]; 
      P1[1][X] = 0.0;        P1[1][Y] = 0.0;       P1[1][Z] = point2[Z]; 
      P1[2][X] = point1[X];  P1[2][Y] = point1[Y]; P1[2][Z] = point1[Z];
      P1[3][X] = 0.0;        P1[3][Y] = 0.0;       P1[3][Z] = point1[Z];
   }

   /* Now add in the positional data. */
   tokens[n] = RI_P;

   parms[n] = (RtPointer)P1;
   RiPatchV( RI_BILINEAR, n+1, tokens, parms );
   if (twisted && P2[0][Z]!=P2[2][Z])
   {
      parms[n] = (RtPointer)P2;
      RiPatchV( RI_BILINEAR, n+1, tokens, parms );
      parms[n] = (RtPointer)P1;
   }

   RiAttributeBegin();
   RiRotate( thetamax, 0, 0, 1 );
   RiReverseOrientation();
   RiPatchV( RI_BILINEAR, n+1, tokens, parms ); 
   if (twisted && P2[0][Z]!=P2[2][Z])
   {
      parms[n] = (RtPointer)P2;
      RiPatchV( RI_BILINEAR, n+1, tokens, parms );
   }
   RiAttributeEnd();
}


RtVoid ClosedParaboloidV( RtFloat rmax,RtFloat zmin,RtFloat zmax,
			 RtFloat thetamax,
			 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   static RtPoint  P[16];
   RtFloat  rmin = 0.0;
   RtFloat  Sx, Sy, B0, B3, K;


   if ( zmin < 0.0 )
   {
      RiArchiveRecord( RI_COMMENT, "WARNING:  "                       \
		       "Invalid zmin (%g) value given to Paraboloid " \
		       "(line:%u).", zmin, RibGetLineCount(rib) );
      return;
   }
   if ( zmax <= 0.0 )
   {
      RiArchiveRecord( RI_COMMENT, "WARNING:  "                       \
		       "Invalid zmax (%g) value given to Paraboloid " \
		       "(line:%u).", zmax, RibGetLineCount(rib) );
      return;
   }
   RiParaboloidV( rmax, zmin, zmax, thetamax, n, tokens, parms ); 

   /* Top. */
   RiDiskV( zmax, rmax, thetamax, n, tokens, parms ); 

   /* Bottom if needed. */
   if (zmin>0.0)
   {
      rmin = sqrt(zmin/zmax);
      RiAttributeBegin();
      RiReverseOrientation();
      RiDiskV( zmin, rmin, thetamax, n, tokens, parms ); 
      RiAttributeEnd();
   }

   /* Don't add patches for wedge shape if they are not needed. */
   if (thetamax==360.0)
      return;

   /* Increase the vector data by one so a positional array "P" can be
    *   added.  Don't do this unless it was the Affine RIB Reader that
    *   setup the tokens[] and parms[] arrays.
    */
   if (RibCacheVector( rib, 1, &tokens, &parms ) )
      return; /* Oops, out of memory.  Just return. */

   /* Now add in the positional data. */
   tokens[n] = RI_P;
   parms[n] = (RtPointer)P;
   n++;

   /* The Y coordinates need to be zero, so just clear everything. */
   memset( P, '\0', 16*sizeof(RtFloat) );

   /* While calculating zmin check if the paraboloid is closed at the bottom */
   P[0][Z] = P[1][Z] = P[2][Z] = P[3][Z] = zmin; /* height */
   /* P[0][X] = 0.0; */ /* Note memset() did this already. */
   P[3][X] = ( zmin>0.0 ? rmax * rmin : 0.0 ); /* radius */
   P[1][X] = P[3][X]/3.0;
   P[2][X] = 2*P[1][X];

   /* Calculate zmax. */
   P[12][Z] = P[13][Z] = P[14][Z] = P[15][Z] = zmax;
   /* P[12][X] = 0.0; */ /* Note memset() did this already. */
   P[15][X] = rmax;
   P[13][X] = P[15][X]/3.0;
   P[14][X] = 2*P[13][X];
   
   /* If the tangent lines at S0 and S3 are extended, they will intersect
    *    at a point.  Call this point Si.  [FAUX79]
    *
    *  Given a parabola S=S0*(1-u)^2 + 2u*Si*(1-u) + u^2*S3
    *
    *  S1 = (S0 + 2Si)/3,  S2 = (S3 + 2Si)/3
    *
    *   S0 = (X0,Y0) with slope Y0'
    *   S3 = (X3,Y3) with slope Y3'       
    *
    *   Tangent at S0 is the line L0 = Y0'*X + B0
    *   Tangent at S3 is the line L3 = Y3'*X + B3       
    *
    *   Set L0 and L3 equal:   Y0'*X + B0 = Y3'*X + B3
    * 
    *   Intersection of the two tangents at S is (Sx,Sy) where
    *      Sx = (B3-B0) / (Y0' - Y3')
    *      Sy = Y0'*Sx + B0
    */
   
   /* If y = mx + b, b= y - mx.  Let K = (2*zmax)/(rmax*rmax).
    *    B0 = P[0][Z]  - K*rmin*rmin;
    *    B3 = P[12][Z] - K*rmax*rmax;
    *
    * With rmin = sqrt(zmin/zmax) and K = (2*zmax)/(rmax*rmax):
    *    B0 = P[0][Z]  - ((2*zmax)/(rmax*rmax)) * (zmin/zmax)
    *    B3 = P[12][Z] - ((2*zmax)/(rmax*rmax)) * rmax * rmax
    *
    * Simplify:
    *    B0 = P[0][Z]  - (2*zmin)/(rmax*rmax)
    *    B3 = P[12][Z] - 2*zmax
    */
   K = rmax*rmax;
   B0 = P[0][Z]  - (2*zmin)/K;
   B3 = P[12][Z] - 2*zmax;
   K = (2*zmax)/K;
   Sx = (B3 - B0)/( K*(rmin - rmax) );
   Sy = K*rmin * Sx + B0;
   
   /* Calculate CVs one up from zmin. */
   P[4][Z] = P[5][Z] = P[6][Z] = P[7][Z] = (P[0][Z]  + 2*Sy)/3;
   /* P[4][X] = 0.0; */ /* Note memset() did this already. */
   P[7][X]  = (rmin + 2*Sx)/3;
   P[5][X] = P[7][X]/3.0;
   P[6][X] = 2*P[5][X];

   /* Calculate CVs two up from zmin and one down from zmax. */
   P[8][Z]  = P[9][Z] = P[10][Z] = P[11][Z] = (P[12][Z] + 2*Sy)/3;
   /* P[8][X]  = 0.0; */ /* Note memset() did this already. */
   P[11][X] = (rmax + 2*Sx)/3;
   P[9][X]  = P[11][X]/3.0;
   P[10][X] = 2*P[9][X];

   RiPatchV( RI_BICUBIC, n, tokens, parms ); 
   RiAttributeBegin();
   RiRotate( thetamax, 0, 0, 1 );
   RiReverseOrientation();
   RiPatchV( RI_BICUBIC, n, tokens, parms ); 
   RiAttributeEnd();
}


RtVoid ClosedSphereV( RtFloat radius, RtFloat zmin, RtFloat zmax, 
		     RtFloat thetamax,
		     RtInt n, RtToken tokens[], RtPointer parms[] )
{
   static RtPoint  P1[16];
   static RtPoint  P2[16];
   RtFloat         rmin, rmax;
   RtFloat         xmid, zmid;
   RtFloat         angle, circle;
   RtFloat         phimax, phimin, phidiff;


   if ( radius == 0.0 )
      return;

   RiSphereV( radius, zmin, zmax, thetamax, n, tokens, parms ); 

   /* Top if needed. */
   if ( fabs(zmax) < radius )
   {
      /* Refer to [UPST90]. */
      rmax = sqrt(radius*radius-zmax*zmax);
      RiDiskV( zmax, rmax, thetamax, n, tokens, parms ); 

      phimax = asin(zmax/radius);
   }
   else
   {
      rmax = 0.0;
      phimax = HALF_PI;
   }

   /* Bottom if needed. */
   if ( fabs(zmin) < radius )
   {
      RiAttributeBegin();
      RiReverseOrientation();
      /* Refer to [UPST90]. */
      rmin = sqrt(radius*radius-zmin*zmin);
      RiDiskV( zmin, rmin, thetamax, n, tokens, parms ); 
      RiAttributeEnd();

      phimin = asin(zmin/radius);
   }
   else
   {
      rmin = 0.0;
      phimin = -HALF_PI;
   }

   /* Don't add patches for wedge shape if they are not needed. */
   if (thetamax==360.0)
      return;

   phidiff = phimax - phimin;
   zmid = sin( phimin + phidiff/2.0 );
   xmid = sqrt(radius*radius-zmid*zmid);

   /* Increase the vector data by one so a positional array "P" can be
    *   added.  Don't do this unless it was the Affine RIB Reader that
    *   setup the tokens[] and parms[] arrays.
    */
   if (RibCacheVector( rib, 1, &tokens, &parms ) )
      return; /* Oops, out of memory.  Just return. */

   /* Now add in the positional data. */
   tokens[n] = RI_P;

   /* The Y coordinates need to be zero, so just clear everything. */
   memset( P1, '\0', 16*sizeof(RtFloat) );
   memset( P2, '\0', 16*sizeof(RtFloat) );

   /* Calculate the patch from bottom to middle. */
   P1[0][Z] = P1[1][Z] = P1[2][Z] = P1[3][Z] = zmin; /* height */
   /* P1[0][X] = 0.0; */ /* Note memset() did this already. */
   P1[3][X] = rmin; /* radius */
   P1[1][X] = P1[3][X]/3.0;
   P1[2][X] = 2*P1[1][X];

   P1[12][Z] = P1[13][Z] = P1[14][Z] = P1[15][Z] = zmid; /* middle's height */
   /* P1[12][X] = 0.0; */ /* Note memset() did this already. */
   P1[15][X] = xmid;
   P1[13][X] = P1[15][X]/3.0;
   P1[14][X] = 2*P1[13][X];
   
   angle = phimin + HALF_PI;			
   circle = (UNIT_CIRCLE/PI)*radius*phidiff;

   P1[4][Z] = P1[5][Z] = P1[6][Z] = P1[7][Z] = zmin + circle*sin(angle);
   /* P1[4][X] = 0.0; */ /* Note memset() did this already. */
   P1[7][X]  = rmin + circle*cos(angle);
   P1[5][X] = P1[7][X]/3.0;
   P1[6][X] = 2*P1[5][X];

   angle = phimin + phidiff/2.0 - HALF_PI;			
   P1[8][Z]  = P1[9][Z] = P1[10][Z] = P1[11][Z] = zmid + circle*sin(angle);
   /* P1[8][X]  = 0.0; */ /* Note memset() did this already. */
   P1[11][X] = xmid + circle*cos(angle);
   P1[9][X]  = P1[11][X]/3.0;
   P1[10][X] = 2*P1[9][X];


   /* Calculate the patch from equator to top. */
   P2[0][Z] = P2[1][Z] = P2[2][Z] = P2[3][Z] = zmid; /* middle's height */
   /* P2[0][X] = 0.0; */ /* Note memset() did this already. */
   P2[3][X] = xmid;
   P2[1][X] = P2[3][X]/3.0;
   P2[2][X] = 2*P2[1][X];

   P2[12][Z] = P2[13][Z] = P2[14][Z] = P2[15][Z] = zmax; /* height */
   /* P2[12][X] = 0.0; */ /* Note memset() did this already. */
   P2[15][X] = rmax; /* Radius at zmax. */
   P2[13][X] = P2[15][X]/3.0;
   P2[14][X] = 2*P2[13][X];
   
   angle = phimin + phidiff/2.0 + HALF_PI;			
   circle = (UNIT_CIRCLE/PI)*radius*phidiff;

   P2[4][Z] = P2[5][Z] = P2[6][Z] = P2[7][Z] = zmid + circle*sin(angle); 
   /* P2[4][X] = 0.0; */ /* Note memset() did this already. */
   P2[7][X]  = xmid + circle*cos(angle);
   P2[5][X] = P2[7][X]/3.0;
   P2[6][X] = 2*P2[5][X];

   angle = phimin + phidiff - HALF_PI;			
   P2[8][Z]  = P2[9][Z] = P2[10][Z] = P2[11][Z] = zmax + circle*sin(angle);
   /* P2[8][X]  = 0.0; */ /* Note memset() did this already. */
   P2[11][X] = rmax + circle*cos(angle);
   P2[9][X]  = P2[11][X]/3.0;
   P2[10][X] = 2*P2[9][X];

   parms[n] = (RtPointer)P1;
   RiPatchV( RI_BICUBIC, n+1, tokens, parms ); 
   parms[n] = (RtPointer)P2;
   RiPatchV( RI_BICUBIC, n+1, tokens, parms );
   RiAttributeBegin();
   RiRotate( thetamax, 0, 0, 1 );
   RiReverseOrientation();
   parms[n] = (RtPointer)P1;
   RiPatchV( RI_BICUBIC, n+1, tokens, parms );
   parms[n] = (RtPointer)P2;
   RiPatchV( RI_BICUBIC, n+1, tokens, parms );
   RiAttributeEnd();
}


RtVoid ClosedTorusV( RtFloat majorradius, RtFloat minorradius, 
		    RtFloat phimin, RtFloat phimax, RtFloat thetamax, 
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   RtFloat  phidiff,phiminR,phidiffR;
   RtPoint  center;
   RtPoint  rim;


   /* Probably could divide the following case up a bit more. 
    * Note that if phimax and phimin are set so the torus is open
    *    on the top and major < minor, the result is a flower like
    *    geometry where one cup is inside another.  Use PRMan to 
    *    see this.
    */
   if ( fabs(majorradius) - fabs(minorradius) < 0.0 )
   {
      RiArchiveRecord( RI_COMMENT, 
		       "WARNING:  Can't close given torus (line:%u).",
		       RibGetLineCount(rib) );
      return;
   }

   RiTorusV( majorradius, minorradius, phimin, phimax, thetamax, 
	     n, tokens, parms ); 

   phidiff = phimax - phimin;
   if ( fabs(phidiff) < 360.0 )
   {
      center[X] = majorradius; center[Y] = 0.0; center[Z] = 0.0;

      phiminR  = phimin*DEG_TO_RAD;
      rim[X] = majorradius + minorradius*cos(phiminR); 
      rim[Y] = 0.0; 
      rim[Z] = minorradius*sin(phiminR);
      
      RiHyperboloidV( center, rim, thetamax, n, tokens, parms );

      phidiffR  = phidiff*DEG_TO_RAD;
      phiminR += phidiffR;
      rim[X] = majorradius + minorradius*cos(phiminR);
      rim[Z] = minorradius*sin(phiminR);

      RiAttributeBegin();
      RiReverseOrientation();
      RiHyperboloidV( center, rim, thetamax, n, tokens, parms );
      RiAttributeEnd();
   }

   /* Don't add disks for wedge shape if they are not needed. */
   if (thetamax==360.0)
      return;

   RiAttributeBegin();
   RiTranslate( majorradius, 0.0, 0.0 );
   RiRotate( 90, 1, 0, 0 );
   RiRotate( phimin, 0, 0, 1 );
   RiDiskV( 0, minorradius, phidiff, n, tokens, parms ); 
   RiAttributeEnd();

   RiAttributeBegin();
   RiRotate( thetamax, 0, 0, 1 );
   RiTranslate( majorradius, 0.0, 0.0 );
   RiRotate( 90, 1, 0, 0 );
   RiRotate( phimin, 0, 0, 1 );
   RiReverseOrientation();
   RiDiskV( 0, minorradius, phidiff, n, tokens, parms ); 
   RiAttributeEnd();
}
