/* $RCSfile: cnsttest.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 06:30:52 $
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
 * FILE:  cnsttest.c
 *
 * DESCRIPTION:  Gravity check.
 *   
 *    Contains:
 * 
 *    References:
 *
 */
#define _RIPRIV_TYPES
#include <string.h>
#include <stdio.h>
#include <ripriv.h>
#include <ribhash.h>


int main(int argc, char **argv);


int main(int argc, char **argv)
{
   static char *BasisNames[] = { 
      "Bezier", 
      "B-Spline", 
      "Catmull-Rom", 
      "Hermite", 
      "Power" 
     };
   RtBasis  *p;
   RtFloat  f;
   int  b,x,y;


   printf( "Bezier Basis = {\n" );
   for ( y=0; y<4; y++ )
   {
      for ( x=0; x<4; x++ )
      {
         printf( "%g  ", RiBezierBasis[y][x] );
      }
      putchar( '\n' );
   }
   printf( "};\n" );
   printf( "BSpline Basis = {\n" );
   for ( y=0; y<4; y++ )
   {
      for ( x=0; x<4; x++ )
      {
         printf( "%g  ", RiBSplineBasis[y][x] );
      }
      putchar( '\n' );
   }
   printf( "};\n" );
   printf( "CatmullRom Basis = {\n" );
   for ( y=0; y<4; y++ )
   {
      for ( x=0; x<4; x++ )
      {
         printf( "%g  ", RiCatmullRomBasis[y][x] );
      }
      putchar( '\n' );
   }
   printf( "};\n" );
   printf( "Hermite Basis = {\n" );
   for ( y=0; y<4; y++ )
   {
      for ( x=0; x<4; x++ )
      {
         printf( "%g  ", RiHermiteBasis[y][x] );
      }
      putchar( '\n' );
   }
   printf( "};\n" );
   printf( "Power Basis = {\n" );
   for ( y=0; y<4; y++ )
   {
      for ( x=0; x<4; x++ )
      {
         printf( "%g  ", RiPowerBasis[y][x] );
      }
      putchar( '\n' );
   }
   printf( "};\n" );

   printf( "Bezier: %p\n", RiBezierBasis );
   printf( "BSpline: %p\n", RiBSplineBasis );
   printf( "CatmullRom: %p\n", RiCatmullRomBasis );
   printf( "Hermite: %p\n", RiHermiteBasis );
   printf( "Power: %p\n", RiPowerBasis );
   for ( b=0; b < sizeof(RiStdBasisTbl)/sizeof(PRtBasis); b++ )
   {
      p = (RtBasis*)RiStdBasisTbl[b];
      printf( "%s Basis (%p) = {\n", BasisNames[b], p );
      for ( y=0; y<4; y++ )
      {
         for ( x=0; x<4; x++ )
         {
            f = (*p)[y][x];
            printf( "%6g  ", f );
         }
         putchar( '\n' );
      }
      printf( "};\n" );
   }

   return 0;
}
