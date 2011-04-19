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
 * FILE:  write.c
 *
 * DESCRIPTION:  Absolute bare bones, quick and dirty RIB writer.  Basically 
 *               many fprintf()'s, a cheap hash table and several strcmp()'s 
 *               are used.
 *
 *               Written more as a reference for someone learning about the
 *               specification.  There's a lot of redundancy in the code.
 *
 *               Use libribwrtr.a for a more complete version.  The write.c 
 *               file is just provided for those who want something simple 
 *               to refer to.
 *   
 *    Contains:
 *  
 *    History:
 *       6-20-97  Arrays of strings were not supported as of 3.6.  I don't
 *                have 3.7 yet, but a posting by Larry Gritz indicates that 
 *                PRMan 3.7 supports atleast arrays of strings when there 
 *                are multiple uniform values across a piece of geometry.  
 *                Added support for string arrays.
 *       7-10-97  Added variable declarations for 3.7 Curves and Points
 *                support.  Added class "constant" and type "matrix".
 *                Added RiCurves, RiCurvesV, RiPoints and RiPointsV.
 *       7-14-97  Added RiProcDelayedReadArchive, RiProcRunProgram
 *                and RiProcDynamicLoad.  Updated RiProcedural to 3.7.
 *       1-05-98  Was relayed problem about Alpha systems compiling
 *                libsribw.a.  It appears to be a problem with va_list
 *                being a struct on Alpha's and a pointer on the other
 *                systems.  Changes were made to if statements and calls to 
 *                functions with a va_list parameter where the va_list was
 *                treated as a pointer.
 *                Also removed an extra '\n' in RiHider() and RiHiderV().
 *       1-06-98  Fixed oversight in 1-5-98 changes with PatchMesh().  
 *                Second param to HandleParamList() needed to be a -1 not 
 *                a zero.
 *       1-07-98  Fixed RiCurves() and RiCurvesV().
 *      03-15-98  Added code to check if first line was printed or not.
 *                This prevents multiple "##RenderMan RIB-Structure 1.0"
 *                from being printed as output from a client library is 
 *                reentered into a program using this library.  
 *                Added "version 3.03" line.
 *                Added proper Attribute block scoping for Basis statements.
 *                Refer to "4.3. RIB Client Library Known Problems and Bugs" 
 *                in PRMan 3.7 HTML documentation which states that the RIB
 *                Client Library ignores scope of Basis statements.  
 *      04-06-98  Found problem with my printed copy of [PIXA89] and fixed 
 *                RiPatchMesh() and RiPatchMeshV().
 *      10-12-98  Added RiReadSubdivisionMesh and RiReadSubdivisionMeshV.
 *      12-05-98  Added RiGeometricRepresentation, RiResource and RiResourceV.
 *      04-27-99  Added RiNuCurves() and RiNuCurvesV() to become more
 *                worldly and support RenderDotC extensions.
 *      06-09-99  Inline declaration support added.
 *      10-10-99  Added "archive" for "searchpath" option.
 *      10-17-99  Added server searchpath token names to hash table.
 *      11-28-99  Fixed RiBasis() output for nonstandard basis. [EPPS99]
 *      02-15-00  Fixed gridsize and merge declarations.  Added declaration
 *                type "int" [EVES00].
 *
 *
 *    References:
 *        [GRITZ97] Gritz, Larry, "Re: texture names per mesh face?",
 *                  comp.graphics.rendering.renderman, June 6, 1997.
 *        [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, Richmond, CA, 
 *                  September, 1989.
 *        [PIXA93a] Pixar, PhotoRealistic RenderMan User's Manual: UNIX(TM)
 *                  Version, Richmond, CA, January, 1993, p. 17.
 *        [PIXA97a] Pixar, PhotoRealistic RenderMan: Application Note #18,
 *                  Using the RiPoints Primitive, Richmond, CA, March, 1997.
 *        [PIXA97b] Pixar, PhotoRealistic RenderMan: Application Note #19,
 *                  Using the RiCurves Primitive, Richmond, CA, March, 1997.
 *        [PIXA97c] Pixar, PhotoRealistic RenderMan: Application Note #22,
 *                  Class Specifiers for Primitive Variables, Richmond, CA, 
 *                  March, 1997.
 *        [PIXA97d] Pixar, PhotoRealistic RenderMan: Application Note #23,
 *                  Using Procedural Primitives in PhotoRealistic RenderMan, 
 *                  Richmond, CA, March, 1997.
 *        [PIXA97e] Pixar, prman3.7c/html/Toolkit/rnotes-3.7.html, section
 *                  "4.3. RIB Client Library Known Problems and Bugs", 
 *                  Richmond, CA, March, 1997.
 *        [PIXA98a] Pixar Animation Studios, cineon.4.html,
 *                  Reserved RenderMan Parameters Names, Richmond, CA,
 *                  1998.
 *        [PIXA98b] Pixar Animation Studios, user.html,
 *                  Reserved RenderMan Parameters Names, Richmond, CA,
 *                  1998.
 *        [UPST90]  Upstill, Steve, The RenderMan Companion: A Programmer's
 *                  Guide to Realistic Computer Graphics, Addison Wesley, 
 *                  1990, 1992, pp. 249-250.
 *
 *    References for the additional items to add to the hash table:
 *
 *        [GRITZ95] Gritz, Larry I., Blue Moon Rendering Tools: User 
 *                  Guide, Febuary 27, 1995.
 *        [PIXA93b] Pixar, PhotoRealistic RenderMan Tutorial: UNIX Version, 
 *                  Richmond, CA, January, 1993.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define _RIPRIV_TYPES
/* #define __FILE_CNSTNT */
/* #define __FILE_SBASIS */
#define __FILE_RIBE
#define __FILE_RIERROR
#define __FILE_RIVIEW
#define __FILE_RIDSPY
#define __FILE_RIATRANS
#define __FILE_RISHDR
#define __FILE_RIFILES
#define __FILE_RIATTRIB
#define __FILE_RIPATCH
#define __FILE_NURBS
#define __FILE_RIQUADS
#define __FILE_RIPOLY
#define __FILE_RIEXTRA
#define __FILE_RITEX
#define __FILE_PIXFILTR
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <ripriv.h>
#include "write.h"


#define VERTEX      0x00010000
#define VARYING     0x00020000
#define UNIFORM     0x00040000
#define CONSTANT    0x00080000
#define FLOAT       0x00000001
#define STRING      0x00000003
#define POINT       0x00000004
#define COLOR       0x0000000C
#define NORMAL      0x00000014
#define VECTOR      0x00000024
#define HPOINT      0x00000040
#define INT         0x00000080
#define FLOATPAIR   0x00000100
#define INTPAIR     0x00000200
#define MATRIX      0x80000400
#define ARRAY       0x80000000
#define UNKNOWN     0x00000000
#define CLASS       0x7fff0000
#define TYPE        0x0000ffff
#define CLASSTYPE   0x7fffffff

static void SetUpHashTable( void );
static void ClearHashTable( void );
char *GetClassNType( char *name, char *declaration, int *classNtype, int *n );
static void AddToHashTable( char *name, int classNtype, int n );
static int GetFromHashTable( char *name, int* classNtype, int *n );
static void HandleParamList( va_list ap, 
			    RtInt n, RtToken tokens[], RtPointer parms[],
			    int nvertex, int nuniform, int nvarying );
static char *HandleGettingFilterName( RtFilterFunc filterfunc );
static RtVoid HandleFirstLine( int notacomment );

#define CheckIfFirstLine()  if (gSRIBW.firstline) \
                               HandleFirstLine(RI_FALSE)


PRIVATESTATEDATA  gSRIBW = {
   /* RtBoolean     hashtablecleared */ RI_TRUE,
   /* unsigned int  LastObjectHandle */ 1,
   /* unsigned int  LastLightHandle */  1,
   /* RtInt         nustep */           3,
   /* RtInt         nvstep */           3,
   /* UVSTEPS       *stepstack */       NULL,
   /* RtInt         blocklevel */       0,
   /* RtInt         ncolor */           3,
   /* RtInt         firstline */        2,
   /* RtBoolean     prman36changes */   RI_TRUE
};
RtInt RiLastError = 0;
static FILE          *fp = NULL;
static va_list       noap;
static HASHATOM      *hashtable[HASHMOD];


static void SetUpHashTable( void )
{
   if (!gSRIBW.hashtablecleared)
   {
      ClearHashTable();
   }
   gSRIBW.hashtablecleared = RI_FALSE;

   AddToHashTable( RI_INTENSITY, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_LIGHTCOLOR, UNIFORM | COLOR, 1 );
   AddToHashTable( RI_FROM, UNIFORM | POINT, 1 );
   AddToHashTable( RI_TO, UNIFORM | POINT, 1 );
   AddToHashTable( RI_CONEANGLE, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_CONEDELTAANGLE, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_BEAMDISTRIBUTION, UNIFORM | FLOAT, 1 );

   AddToHashTable( RI_KA, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_KD, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_KS, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_ROUGHNESS, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_KR, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_TEXTURENAME, UNIFORM | STRING, 1 );
   AddToHashTable( RI_SPECULARCOLOR, UNIFORM | COLOR, 1 );

   AddToHashTable( RI_DISTANCE, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_MINDISTANCE, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_MAXDISTANCE, UNIFORM | FLOAT, 1 );
   AddToHashTable( RI_BACKGROUND, UNIFORM | COLOR, 1 );
   AddToHashTable( RI_AMPLITUDE, UNIFORM | FLOAT, 1 );

   AddToHashTable( RI_P, VERTEX | POINT, 1 );
   AddToHashTable( RI_PZ, VERTEX | FLOAT, 1 );
   AddToHashTable( RI_PW, VERTEX | HPOINT, 1 );

   AddToHashTable( RI_N, VARYING | NORMAL, 1 );
   AddToHashTable( RI_NP, UNIFORM | NORMAL, 1 );
   AddToHashTable( RI_CS, VARYING | COLOR, 1 );
   AddToHashTable( RI_OS, VARYING | COLOR, 1 );
   AddToHashTable( RI_S, VARYING | FLOAT, 1 );
   AddToHashTable( RI_T, VARYING | FLOAT, 1 );
   AddToHashTable( RI_ST, VARYING | FLOATPAIR, 1 );

   /* The following was added for the RiDisplay call. */
   AddToHashTable( RI_ORIGIN, INTPAIR, 1 );

   /*    References For additional items to add to the hash table:
    *
    *        [GRIT95]  Gritz, Larry I., Blue Moon Rendering Tools: User 
    *                  Guide, Febuary 27, 1995.
    *        [PIXA93]  Pixar, PhotoRealistic RenderMan Tutorial: UNIX Version, 
    *                  Richmond, CA, January, 1993.
    */

   /* 
    *   The following was added for the RiOption call. 
    *
    */

   /* The "rib" options of Pixar's prman: */
   AddToHashTable( "format", UNIFORM | STRING, 1 );

   /* The "limits" options of Pixar's prman: */
   AddToHashTable( "bucketsize", UNIFORM | INTPAIR, 1 );
   AddToHashTable( "gridsize", UNIFORM | INT, 1 );
   AddToHashTable( "texturememory", UNIFORM | INT, 1 );
   AddToHashTable( "zthreshold", UNIFORM | POINT, 1 );
   AddToHashTable( "extremedisplacement", UNIFORM | INT, 1 );
   AddToHashTable( "eyesplits", UNIFORM | INT, 1 );

   /* The "limits" options of BMRT's rendrib (2.3.5): */ 
   /* BMRT's "texturememory" defined above. */
   AddToHashTable( "geommemory", UNIFORM | INT, 1 );

   /* The "shadow" options of Pixar's prman: */
   AddToHashTable( "bias0", UNIFORM | FLOAT, 1 );
   AddToHashTable( "bias1", UNIFORM | FLOAT, 1 );  
   AddToHashTable( "bias", UNIFORM | FLOAT, 1 );  

   /* The "searchpath" options of Pixar's prman: */
   AddToHashTable( "shader", UNIFORM | STRING, 1 );
   AddToHashTable( "texture", UNIFORM | STRING, 1 );
   AddToHashTable( "vfxmaster", UNIFORM | STRING, 1 );
   AddToHashTable( "vfxinstance", UNIFORM | STRING, 1 );
   AddToHashTable( "archive", UNIFORM | STRING, 1 );
   AddToHashTable( "resource", UNIFORM | STRING, 1 );
   AddToHashTable( "display", UNIFORM | STRING, 1 );
   AddToHashTable( "servershader", UNIFORM | STRING, 1 );
   AddToHashTable( "servertexture", UNIFORM | STRING, 1 );
   AddToHashTable( "servervfxmaster", UNIFORM | STRING, 1 );
   AddToHashTable( "servervfxinstance", UNIFORM | STRING, 1 );
   AddToHashTable( "serverarchive", UNIFORM | STRING, 1 );
   AddToHashTable( "serverresource", UNIFORM | STRING, 1 );
   AddToHashTable( "serverdisplay", UNIFORM | STRING, 1 );

   /* The "render" options of BMRT's rendrib: */ 
   AddToHashTable( "minsamples", UNIFORM | INT, 1 );
   AddToHashTable( "maxsamples", UNIFORM | INT, 1 );
   AddToHashTable( "max_raylevel", UNIFORM | INT, 1 );
   AddToHashTable( "branch_ratio", UNIFORM | INT, 1 );
   AddToHashTable( "max_branch_ratio", UNIFORM | INT, 1 );
   AddToHashTable( "minshadowbias", UNIFORM | FLOAT, 1 );

   /* The "radiosity" options of BMRT's rendrib: */
   AddToHashTable( "steps", UNIFORM | INT, 1 );
   AddToHashTable( "minpatchsamples", UNIFORM | INT, 1 );

   /* The "runtime" options of BMRT's rendrib: */
   AddToHashTable( "verbosity", UNIFORM | STRING, 1 );

   /* The "searchpath" options of BMRT's rendrib: */ 
   AddToHashTable( "include", UNIFORM | STRING, 1 );

   /* The "texture" options of PRMan: */
   AddToHashTable( "enable gaussian", UNIFORM | FLOAT, 1 );
   AddToHashTable( "enable lerp", UNIFORM | FLOAT, 1 );

   /* 
    *  The following was added for the RiAttribute call.               
    *
    */

   /* The "identifier" attributes of Pixar's prman: */ 
   AddToHashTable( "name", UNIFORM | STRING, 1 );

   /* Added because Alias Sketch! uses it and the Pixar tools don't
    *    have any problems with it.  
    * So one more for the "identifier" attributes of Pixar's prman: 
    */ 
   AddToHashTable( "shadinggroup", UNIFORM | STRING, 1 );

   /* The "dice" attributes of Pixar's prman: */ 
   AddToHashTable( "binary", UNIFORM | INT, 1 );

   /* The "displacementbound" attributes of Pixar's prman: */ 
   AddToHashTable( "coordinatesystem", UNIFORM | STRING, 1 );
   AddToHashTable( "sphere", UNIFORM | FLOAT, 1 );

   /* The "trimcurve" attributes of Pixar's prman (also BMRT 2.3.5): */ 
   AddToHashTable( "sense", UNIFORM | STRING, 1 );

   /* The "radiosity" attributes of BMRT's rendrib: */
   AddToHashTable( "averagecolor", UNIFORM | COLOR, 1 );
   AddToHashTable( "emissioncolor", UNIFORM | COLOR, 1 );
   AddToHashTable( "patchsize", UNIFORM | INT, 1 );
   AddToHashTable( "elemsize", UNIFORM | INT, 1 );
   AddToHashTable( "minsize", UNIFORM | INT,1 );
   AddToHashTable( "zonal", UNIFORM | STRING, 1 );
   AddToHashTable( "casts_shadows", UNIFORM | STRING, 1 );
   /* The variable "specularcolor" is defined with RI_SPECULARCOLOR above. */

   /* The "render" attributes of BMRT's rendrib: */ 
   AddToHashTable( "patch_maxlevel", UNIFORM | INT, 1 );
   AddToHashTable( "patch_minlevel", UNIFORM | INT, 1 );/* 2.3.5 */
   AddToHashTable( "patch_multiplier", UNIFORM | INT, 1 );/* 2.3.6? */
   AddToHashTable( "truedisplacement", UNIFORM | INT, 1 );/* 2.3.5 */

   /* The "light" attributes of BMRT's rendrib. */
   AddToHashTable( "shadows", UNIFORM | STRING, 1 );
   AddToHashTable( "cache", UNIFORM | STRING, 1 );
   AddToHashTable( "nsamples", UNIFORM | INT, 1 );/* 2.3.5 */

   /* 
    *   The following was added for the RiDisplay call. 
    *
    */

   /* A required option of RiDisplay() is "origin". */
   AddToHashTable( "origin", UNIFORM | INTPAIR, 1 );

   /* More Display options of Pixar's prman: */
   AddToHashTable( "merge", UNIFORM | INT, 1 );
   AddToHashTable( "compression", UNIFORM | STRING, 1 );
   AddToHashTable( "resolution", UNIFORM | INTPAIR, 1 );
   AddToHashTable( "resolutionunit", UNIFORM | STRING, 1 );

   /* MTOR and possible new PRMan 3.9 feature. */
   AddToHashTable( "dspyParams", UNIFORM | STRING, 1 );

   /* 
    *   The following was added for the RiHider call. 
    *
    */
   /* Hider's "hidden" options: */
   AddToHashTable( "jitter", UNIFORM | INT, 1 );
   AddToHashTable( "pdisc", UNIFORM | INT, 1 );

   /* 
    *   The following was added for the RiProjection call. 
    *
    */
   AddToHashTable( "fov", UNIFORM | FLOAT, 1 );

   /* 
    *   The following was added for the RiGeometricApproximation call. 
    *
    */
   AddToHashTable( "flatness", UNIFORM | FLOAT, 1 );
   AddToHashTable( "motionfactor", UNIFORM | FLOAT, 1 );

   /* 
    *   The following were added for the RiCurves and RiPoints calls that
    *      are new in PRMan 3.7.
    *
    */
   AddToHashTable( "width", VARYING | FLOAT, 1 );
   AddToHashTable( "constantwidth", CONSTANT | FLOAT, 1 );

   /* 
    *   The following were added for the RiPoints call that is new in 
    *      PRMan 3.7.   (HTML files described both "width" and "radius".  
    *      The "radius" variable doesn't seem to affect the particle sizes,
    *      but catrib and prman seem to have radius declared.
    */
    AddToHashTable( "radius", VARYING | FLOAT, 1 );

   return;
}


static void AddToHashTable( char *name, int classNtype, int n )
{
   int                 hashvalue;
   register  char      *p;
   register  HASHATOM  *h;

   
   p = name;
   hashvalue = *p;
   while ( *p++ )
     hashvalue = ( hashvalue * 32 + *p ) % HASHMOD;

   h = hashtable[hashvalue];
   while (h)
   {
      if (!strcmp( name, h->name ))
      {
	 h->classNtype = classNtype;
	 return;
      }
      h = h->next;
   }
   
   h = (HASHATOM*)malloc( sizeof(HASHATOM) );
   if (!h)
     return;
   
   p = (char*)malloc(strlen(name)+1);
   if (!p)
      return;
   strcpy( p, name );

   h->name = p;
   h->next = hashtable[hashvalue];
   h->classNtype = classNtype;
   h->n = n;
   hashtable[hashvalue] = h;
   
   return;
}


static int GetFromHashTable( char *name, int* classNtype, int *n )
{
   int                 hashvalue;
   register  char      *p;
   register  HASHATOM  *h;
   
   p = name;
   hashvalue = *p;
   while ( *p++ )
     hashvalue = ( hashvalue * 32 + *p ) % HASHMOD;
   
   h = hashtable[hashvalue];
   while (h)
   {
      if (!strcmp( name, h->name ))
      {
	 *classNtype = h->classNtype;
	 *n = h->n;
	 return 1; /* item found */
      }
      h = h->next;
   }
   
   return 0; /* nothing found */
}


static void ClearHashTable( void )
{
   register HASHATOM  *p;
   register HASHATOM  *pp;
   register int       i;


   for ( i=0; i<HASHMOD; i++ )
   {
      if (hashtable[i])
      {
	 p = hashtable[i];
	 while (p)
	 {
	    pp = p->next;
	    free( p );
	    p = pp;
	 }
	 hashtable[i] = NULL;
      }
   }
   
   return;
}


char *GetClassNType( char *name, char *declaration, 
		     int *pclassNtype, int *pn )
{
   char          *p;
   int           classNtype = *pclassNtype;
   int           n = *pn;
   register int  i;


   p = declaration ? declaration : name;
   
   /* get class */
   while( isspace(*p) )
     p++;

   if ( !strncmp( "uniform", p, 7 ) && isspace( p[7] ) )
   {
      classNtype = UNIFORM;
      p += 7;
   }
   else if ( !strncmp( "varying", p, 7 ) && isspace( p[7] ) )
   {
      classNtype = VARYING;
      p += 7;
   }
   else if ( !strncmp( "vertex", p, 6 ) && isspace( p[6] ) )
   {
      classNtype = VERTEX;
      p += 6;
   }
   else if ( !strncmp( "constant", p, 8 ) && isspace( p[8] ) )
   {
      classNtype = CONSTANT;
      p += 8;
   }
   else
   {
      classNtype = UNIFORM;
   }

   while( isspace(*p) )
     p++;

   /* get type */
   if ( !strncmp( "float", p, 5 ) )
   {
      classNtype |= FLOAT;
      p += 5;
   }
   else if ( !strncmp( "point", p, 5 ) )
   {
      classNtype |= POINT;
      p += 5;
   }
   else if ( !strncmp( "string", p, 6 ) )
   {
      classNtype |= STRING;
      p += 6;
   }
   else if ( !strncmp( "color", p, 5 ) )
   {
      classNtype |= COLOR;
      p += 5;
   }
   else if ( !strncmp( "normal", p, 6 ) )
   {
      classNtype |= NORMAL;
      p += 6;
   }
   else if ( !strncmp( "hpoint", p, 6 ) )
   {
      classNtype |= HPOINT;
      p += 6;
   }
   else if ( !strncmp( "vector", p, 6 ) )
   {
      classNtype |= VECTOR;
      p += 6;
   }
   else if ( !strncmp( "integer", p, 7 ) )
   {
      classNtype |= INT;
      p += 7;
   }
   else if ( !strncmp( "int", p, 3 ) )
   {
      classNtype |= INT;
      p += 3;
   }
   else if ( !strncmp( "matrix", p, 6 ) )
   {
      classNtype |= MATRIX;
      p += 6;
   }
   else
   {
      return NULL; /* error no type given */
   }
   if ( declaration && *p!='\0' && *p!='[' ) 
      return NULL;

   if ( *p == '[' )
   {
      p++;
      n = 0;
      i = 0;

      while ( *p >= '0' && *p <= '9' && i < 5 )
      {
	 n = n*10 + (*p-'0');
	 p++;
	 i++;
      }

      if ( *p != ']' || n == 0 )
      {
	return NULL; /* error bad array number given */
      }

      /* move p past the ']' character. */
      p++;

      /* If declaration is NULL and there is no white space 
       *    then give an error for an invalid inline declaration.
       */
      if (!declaration && !isspace(*p))
	 return NULL;

      if ( n==2 )
      {
	 i = classNtype & TYPE;
	 
	 if ( i == FLOAT )
	 {
	    classNtype = (classNtype & ~TYPE) | FLOATPAIR;
	    n = 1;
	 }
	 else if ( i == INT )
	 {
	    classNtype = (classNtype & ~TYPE) | INTPAIR;
	    n = 1;
	 }
	 else
	 {
	    classNtype |= ARRAY;
	 }
      }
      else
      {
	 classNtype |= ARRAY;
      }
   }
   else
   {
      n = 1;
   }

   if (pclassNtype)
      *pclassNtype = classNtype;
   if (pn)
      *pn = n;
   if (!declaration)
   {
      /* Move over to the start of the name. 
       *   Note that p points to either a whitespace or the first 
       *   character of the name.
       */
      while( isspace(*p) )
	 p++;
   }

   return p;
}



RtToken RiDeclare( char *name, char *declaration )
{
   int   classNtype;
   int   n;


   GetClassNType( name, declaration, &classNtype, &n );

   AddToHashTable( name, classNtype, n );
   
   if (fp)
   {
      CheckIfFirstLine();

      fprintf( fp, "Declare \"%s\" \"%s\"\n", name, declaration );
   }
  
   return name;
}


/* Varying and vertex declared variables default to having a single value
 *    for calls that have no determined number of vertices associated with
 *    the data given them.  For example, how many vertices should there be 
 *    for a light source?  
 *
 *
 * This default value of 1 was determined with the following experiments:
 *  example.rib:
 *   Declare "bob" "varying float"
 *   Declare "tom" "vertex float"
 *   Declare "harry" "vertex point"
 *   Display "a.tif" "framebuffer" "rgba" "bob" 1
 *   Projection "perspective" "bob" 1
 *   LightSource "ambientlight" 2 "intensity" [0.3] "bob" [1 2 3 4 5 6 7 8 9]
 *           "tom" [1 2 3 4 5 6 7 8 9] "harry" [1 2 3 4 5]
 *
 * %catrib example.rib 
 * ##RenderMan RIB
 * version 3.03
 * Declare "bob" "varying float"
 * Declare "tom" "vertex float"
 * Declare "harry" "vertex point"
 * Display "a.tif" "framebuffer" "rgba" "bob" [1]
 * Projection "perspective" "bob" [1]
 * LightSource "ambientlight" 1 "intensity" [0.3]"bob" [1]"tom" [1]
 * "harry" [1 2 3]
 * LightSource "ambientlight" 1 "intensity" [0.3]"bob" [1]"tom" [1]
 * 
 * Notice that the variables bob and tom are reduced to single values.  The
 *    variable harry is reduced to a single point (3 floats).
 *
 * From the above experiments, HandleParamList is called with nvertex, 
 *    nuniform, and nvarying set to 1's when the present RIB statement
 *    is not defining geometry.
 */
static void HandleParamList( va_list ap, 
			    RtInt n, RtToken tokens[], RtPointer parms[],
			    int nvertex, int nuniform, int nvarying )
{
   RtToken  paramlist;
   int      t, classNtype, ntype, narray;
   int      IsaLinefeedNeeded = RI_TRUE;
   char              *c;
   unsigned int      u;
   RtToken           *tp;
   RtBoolean         vectorform;
   register RtFloat  *p;
   register RtInt    *ip;
   register int      i;


   /* To avoid problems with some compilers using a struct for va_list
    *    and not a pointer, n is set to -1 to indicate that "va_list ap"
    *    is being used for the token/parameter pairs.
    */
   vectorform = ( n==-1 ? RI_FALSE : RI_TRUE );

   t=0;
   /* If va_list form or vector form then look at ap or (n,tokens,parms)
    *    respectively.
    *
    * Note:  To check that va_arg form is being used find out if 
    *        vectorform is being used and negate that answer.
    */
   if ( !vectorform || (n>0 && tokens && parms) )
   {
      paramlist = ( vectorform ? tokens[t] : va_arg(ap,RtToken) );
      while ( paramlist )
      {
	 c = GetClassNType( paramlist, NULL, &classNtype, &narray );
	 if ( c )
	 {
	    i = 1; /* set a fake return code for the next if(). */
	 }
	 else
	 {
	    i = GetFromHashTable( paramlist, &classNtype, &narray );
	 }
	 if ( i )
	 {
	    switch ( classNtype & CLASS )
	    {
	     case VERTEX:
	       ntype = nvertex;
	       break;
	     case VARYING:
	       ntype = nvarying;
	       break;
	     case UNIFORM:
	       ntype = nuniform;
	       break;
	     case CONSTANT:
	       ntype = 1;
	       break;
	    }
	    
	    if ( ARRAY && classNtype )
	    {
	       ntype *= narray;
	    }
	    
	    fprintf( fp, "\"%s\" [", paramlist );

	    /* Although entirely wacky, I'll stick in a quick check to
             *    to handle the case where ntype is zero.  Note though
             *    that catrib 3.7f gives a syntax error for the 
             *    following RIB statement:
             *
             *      NuPatch   0 1 [0] 0 0 0 1 [0] 0 0 "Pw" []
             */
	    if (!ntype)
	    {
	       fprintf( fp, "]\n" );
	       goto NextIteration; 
	    }
	    
	    u = classNtype & TYPE;
	    if ( INT == u )
	    {
	       ip = ( vectorform ? (RtInt*)parms[t] : va_arg(ap,RtInt*) );
	       for ( i=0; i<(ntype - 1); i++ )
		  fprintf( fp, "%d ", ip[i] );
	       fprintf( fp, "%d]\n", ip[i] );
	       IsaLinefeedNeeded = RI_FALSE;
	    }
	    else if ( INTPAIR == u )
	    {
	       ip = ( vectorform ? (RtInt*)parms[t] : va_arg(ap,RtInt*) );
	       if (ntype>1)
		  fputc( '\n', fp );
	       for ( i=0; i<2*(ntype - 1); i+=2 )
		  fprintf( fp, "%d %d\n", ip[i], ip[i+1] );
	       fprintf( fp, "%d %d]\n", ip[i], ip[i+1] );
	       IsaLinefeedNeeded = RI_FALSE;
	    }
	    else if ( STRING == u )
	    {
	       /* 6-20-97 -- Originally this only printed one string
		*            regardless of what ntype was set to.
		*            PRMan 3.7 is supposed to support string
		*            arrays now, so I'll add support to this 
		*            library.
		*/
	       tp = (RtToken*)( vectorform ? (RtToken*)parms[t] 
				: va_arg(ap,RtToken*));
	       for ( i=0; i<(ntype-1); i++ )
		  fprintf( fp, "\"%s\" ", *tp++ );
	       fprintf( fp, "\"%s\"]", *tp );
	       IsaLinefeedNeeded = RI_TRUE;
	    }
	    else
	    {
	       p = ( vectorform ? (RtFloat*)parms[t] : va_arg(ap,RtFloat*) );
	       
	       switch ( u )
	       {
		case FLOAT:
		  for ( i=0; i<(ntype-1); i++ )
		     fprintf( fp, "%g ", p[i] );
		  fprintf( fp, "%g]", p[i] );
		  IsaLinefeedNeeded = RI_TRUE;
		  break;
		case POINT: 
		case NORMAL:
		case VECTOR:
		  if (ntype>1)
		     fputc( '\n', fp );
		  for ( i=0; i<(3*(ntype-1)); i+=3 )
		     fprintf( fp, "%g %g %g\n", p[i], p[i+1], p[i+2] );
		  fprintf( fp, "%g %g %g]\n", p[i], p[i+1], p[i+2] );
		  IsaLinefeedNeeded = RI_FALSE;
		  break;		   
		case COLOR: 
		  if (ntype>1)
		     fputc( '\n', fp );
		  if (gSRIBW.ncolor==3)
		  {
		     for ( i=0; i<3*(ntype-1); i+=3 )
			fprintf( fp, "%g %g %g\n", p[i], p[i+1], p[i+2] );
		     fprintf( fp, "%g %g %g]\n", p[i], p[i+1], p[i+2] );
		  }
		  else
		  {
		     for ( i=0; i<(ntype*gSRIBW.ncolor - 1); i++ )
			fprintf( fp, "%g ", p[i] );
		     fprintf( fp, "%g]\n", p[i] );
		  }
		  IsaLinefeedNeeded = RI_FALSE;
		  break;
		case FLOATPAIR:
		  if (ntype>1)
		     fputc( '\n', fp );
		  for ( i=0; i<2*(ntype - 1); i+=2 )
		     fprintf( fp, "%g %g\n", p[i], p[i+1] );
		  fprintf( fp, "%g %g]\n", p[i], p[i+1] );
		  IsaLinefeedNeeded = RI_FALSE;
		  break;
		case HPOINT:
		  if (ntype>1)
		     fputc( '\n', fp );
		  for ( i=0; i<4*(ntype-1); i+=4 )
		     fprintf( fp, "%g %g %g %g\n", 
			      p[i],p[i+1],p[i+2],p[i+3] );
		  fprintf( fp, "%g %g %g %g]\n", 
			   p[i],p[i+1],p[i+2],p[i+3] );
		  IsaLinefeedNeeded = RI_FALSE;
		  break;
		case MATRIX:
		  fputc( '\n', fp );
		  for ( i=0; i<16*(ntype-1); i+=4 )
		     fprintf( fp, "%g %g %g %g\n", 
			      p[i],p[i+1],p[i+2],p[i+3] );
		  fprintf( fp, "%g %g %g %g]\n", 
			   p[i],p[i+1],p[i+2],p[i+3] );
		  IsaLinefeedNeeded = RI_FALSE;
		  break;
	       }
	    }
	    
	 NextIteration:
	    if ( !vectorform )
	    {
	       paramlist = va_arg(ap,RtToken);
	    }
	    else if ( --n )
	    {
	       paramlist = tokens[++t];
	    }
	    else
	    {
	       paramlist = NULL;
	    }
	 }
	 else
	 {
	    /* Undefined variable, skip to next token by taking
	     *    a pointer off the stack that can only be
	     *    assumed to be there.  If vector format used
	     *    the index t will be increment when paramlist
	     *    is set.
	     */
	    fprintf( stderr, "ERROR: Undefined user parameter: %s\n", 
		     paramlist );
	    if ( !vectorform )
	    {
	       (void)va_arg(ap,RtPointer);
	    }
	    
	    paramlist = ( vectorform ? tokens[++t] : va_arg(ap,RtToken) );
	 }
      }
   }
   if (IsaLinefeedNeeded)
   {
      fputc( '\n', fp );
      return;
   }
}


static char *HandleGettingFilterName( RtFilterFunc filterfunc )
{
   register char *p;


   if ( filterfunc == RiBoxFilter )
     p = "box";
   else if ( filterfunc == RiTriangleFilter )
     p = "triangle";
   else if ( filterfunc == RiCatmullRomFilter )
     p = "catmull-rom";
   else if ( filterfunc == RiGaussianFilter )
     p = "gaussian";
   else if ( filterfunc == RiSincFilter )
     p = "sinc";
   else 
     return NULL;
   
   return p;
}


/* The following is called using a macro CheckIfFirstLine().  
 *    Do not use CheckIfFirstLine() with RiArchiveRecord() or "version 3.03\n"
 *    will not appear in the output correctly.
 */
static RtVoid HandleFirstLine( int isacomment )
{
   /* Refer to RiArchiveRecord() below for more information regarding when
    *    to print ##RenderMan hints and verion statements.
    */

   if (gSRIBW.firstline==2 || isacomment)
   {
      fprintf( fp, "##RenderMan RIB-Structure 1.0\n" );

      /* If !isacomment, the following if will mark gSRIBW.firstline as having
       *    printed both a ##RenderMan hint and a version statement.
       * If isacomment, then RiArchiveRecord() called HandleFirstLine() and
       *    it will mark gSRIBW.firstline as having printed a ##RenderMan
       *    hint.
       */
      /*gSRIBW.firstline=1;*/
   }

   /* If HandleFirstLine() was called because a RIB statement (not a comment
    *    or hint) was read, print a version statement.
    */
   if (!isacomment)
   {
      fprintf( fp, "version 3.03\n" );
      gSRIBW.firstline=0;
   }
}


RtVoid RiBegin( RtToken name )
{
   if (name)
     fp = fopen( name, "w" );
   else
     fp = stdout;

   gSRIBW.LastObjectHandle = 1;
   gSRIBW.LastLightHandle = 1;
   gSRIBW.nustep = 3;
   gSRIBW.nvstep = 3;
   gSRIBW.stepstack = NULL;
   gSRIBW.blocklevel = 0;
   gSRIBW.ncolor = 3;
   gSRIBW.firstline = 2;
   gSRIBW.prman36changes = RI_TRUE;
   RiLastError = 0;

   if (!fp)
     return;

   SetUpHashTable();
}


RtVoid RiEnd( void )
{
   auto UVSTEPS  *p,*pp;


   if (fp && fp!=stdout)
   {
      fclose(fp);
   }
   fp = NULL;
   p = gSRIBW.stepstack;
   while (p)
   {
      pp = p;
      p = p->next;
      free(pp);
   }  
   gSRIBW.stepstack = NULL;    
}


RtVoid RiFrameBegin( RtInt frame )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "FrameBegin %d\n", frame );
}


RtVoid RiFrameEnd( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "FrameEnd\n" );
}


RtObjectHandle RiObjectBegin( void )
{
   register RtObjectHandle  object;


   if (!fp)
     return NULL;

   CheckIfFirstLine();

   fprintf( fp, "ObjectBegin %u\n", gSRIBW.LastObjectHandle );
   
   object = (RtObjectHandle)gSRIBW.LastObjectHandle++;

   return object;
}


RtVoid RiObjectEnd( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "ObjectEnd\n" );
}


RtVoid RiObjectInstance( RtObjectHandle handle )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "ObjectInstance %u\n", (unsigned int)(size_t)handle );
}


RtVoid RiMotionBegin( RtInt n, ... )
{
   va_list  ap;
   RtFloat  f;
   int i;
   

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MotionBegin [" );
   va_start(ap,n);
   for ( i=0; i<(n-1); i++)
   {
      /* RtFloat's get passed on the stack as doubles. */
      f = va_arg(ap,double);
      fprintf( fp, "%g ", f );
   }
   if (n>0)
   {
      /* RtFloat's get passed on the stack as doubles. */
      f = va_arg(ap,double);
      fprintf( fp, "%g]\n", f );
   }
   va_end(ap);
}


RtVoid RiMotionBeginV( RtInt n, RtFloat times[] )
{
   int i;
   

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MotionBegin [" );
   for ( i=0; i<(n-1); i++)
      fprintf( fp, "%g ", times[i] );
   fprintf( fp, "%g]\n", times[i] );
}


RtVoid RiMotionEnd( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MotionEnd\n" );
}


RtVoid RiSolidBegin( RtToken operation )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   if (   !strcmp( operation, "primitive" )
       || !strcmp( operation, "intersection" )
       || !strcmp( operation, "union" )
       || !strcmp( operation, "difference" ))
   {
      fprintf( fp, "SolidBegin \"%s\"\n", operation );
   }
}

 
RtVoid RiSolidEnd( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "SolidEnd\n" );
}


RtVoid RiAtmosphere( RtToken name, ... )
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Atmosphere \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiAtmosphereV( RtToken name, 
		     RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Atmosphere \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiConcatTransform( RtMatrix transform)
{
   register int i;
   

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "ConcatTransform [\n" );
   
   for ( i=0; i < 4; i++)
     fprintf( fp, "%g %g %g %g\n", transform[i][0], transform[i][1], 
	     transform[i][2], transform[i][3] );
   fprintf( fp, "]\n" );
}


RtVoid RiCoordinateSystem( RtToken space )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "CoordinateSystem \"%s\"\n", space );
}


RtVoid RiCoordSysTransform( RtToken space )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "CoordSysTransform \"%s\"\n", space );
}


RtVoid RiDeformation( RtToken name, ...)
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Deformation \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiDeformationV( RtToken name,
		      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Deformation \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiDisplacement( RtToken name, ...)
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Displacement \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiDisplacementV( RtToken name,
		       RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Displacement \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiExterior( RtToken name, ... )
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Exterior \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiExteriorV( RtToken name, 
		   RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Exterior \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiFormat( RtInt xres, RtInt yres, RtFloat aspect )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Format %d %d %g\n", xres, yres, aspect );
}


RtVoid RiFrameAspectRatio( RtFloat aspect )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "FrameAspectRatio %g\n", aspect );
}


RtVoid RiIlluminate( RtLightHandle light, RtBoolean onoff )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Illuminate %u %d\n", 
	   (unsigned int)(size_t)light, (onoff ? 1 : 0) );
}


RtVoid RiInterior( RtToken name, ... )
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Interior \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiInteriorV( RtToken name, 
		   RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Interior \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiScreenWindow( RtFloat left, RtFloat right, 
		      RtFloat bottom, RtFloat top )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "ScreenWindow %g %g %g %g\n",
	   left, right, bottom, top );
}


RtVoid RiClipping( RtFloat hither, RtFloat yon )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Clipping %g %g\n", hither, yon );
}


RtVoid RiCropWindow( RtFloat xmin, RtFloat xmax, RtFloat ymin, RtFloat ymax )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "CropWindow %g %g %g %g\n", xmin, xmax, ymin, ymax );
}


RtVoid RiDepthOfField( RtFloat fstop, RtFloat focallength, 
		      RtFloat focaldistance )
{  
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "DepthOfField " );
   if ( fstop == RI_INFINITY ) /* See pin-hole camera p. 26 [PIXA93]. */
     fputc( '\n', fp );
   else
     fprintf( fp, "%g %g %g\n", fstop, focallength, focaldistance );
}


RtVoid RiProjection( RtToken name, ... )
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Projection \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiProjectionV( RtToken name, 
		     RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Projection \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiShutter( RtFloat min, RtFloat max )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   /* NOTE:  If min==max then no motion blur is done.  See RiShutter()
    *        p. 26 [PIXA93]. 
    */
   fprintf( fp, "Shutter %g %g\n", min, max );
}


RtVoid RiDisplay( char *name, RtToken type, RtToken mode, ... )
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Display \"%s\" \"%s\" \"%s\" ", name, type, mode );
   va_start(ap,mode);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiDisplayV( char *name, RtToken type, RtToken mode, 
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Display \"%s\" \"%s\" \"%s\" ", name, type, mode );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiExposure( RtFloat gain, RtFloat gamma )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Exposure %g %g\n", gain, gamma );
}


RtVoid RiImager( RtToken name, ... )
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Imager \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiImagerV( RtToken name,
                 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Imager \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiImplicit( RtInt a, RtInt b[], RtInt c, RtFloat d[],
		   RtInt e, RtFloat f[], RtInt g, RtFloat h[], ... )
{
   (void)a; (void)b; (void)c; (void)d; 
   (void)e; (void)f; (void)g; (void)h;  
}


RtVoid RiImplicitV( RtInt a, RtInt b[], RtInt c, RtFloat d[],
		    RtInt e, RtFloat f[], RtInt g, RtFloat h[],
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)a; (void)b; (void)c; (void)d; 
   (void)e; (void)f; (void)g; (void)h;  
   (void)n; (void)tokens; (void)parms;
}


RtVoid RiIdentity( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Identity\n" );
}


RtVoid RiPerspective( RtFloat fov )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Perspective %g\n", fov );
}


RtVoid RiPixelFilter( RtFilterFunc filterfunc, 
		     RtFloat xwidth, RtFloat ywidth )
{
   char *p;


   if (!fp)
     return;

   CheckIfFirstLine();

   p = HandleGettingFilterName( filterfunc );
   if (!p)
     return;
   fprintf( fp, "PixelFilter \"%s\" %g %g\n", p, xwidth, ywidth );
}


RtVoid RiPixelSamples( RtFloat xsamples, RtFloat ysamples )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "PixelSamples %g %g\n", xsamples, ysamples );
}


RtVoid RiPixelVariance( RtFloat variation )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "PixelVariance %g\n", variation );
}


RtVoid RiQuantize( RtToken type, RtInt one, 
		  RtInt min, RtInt max, RtFloat ampl )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   /* PRMan default is RiQuantize(RI_RGBA, 255, 0, 255, .5). */
   fprintf( fp, "Quantize \"%s\" %d %d %d %g\n", 
	   type, one, min, max, ampl ); 
}


RtVoid RiRotate( RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Rotate %g %g %g %g\n", angle, dx, dy, dz );
}


RtVoid RiScale( RtFloat dx, RtFloat dy, RtFloat dz )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Scale %g %g %g\n", dx, dy, dz );
}


RtVoid RiSkew( RtFloat angle, RtFloat dx1, RtFloat dy1, RtFloat dz1,
              RtFloat dx2, RtFloat dy2, RtFloat dz2 )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Skew %g %g %g %g %g %g %g\n", 
	   angle, dx1, dy1, dz1, dx2, dy2, dz2 );
}


RtVoid RiTransform( RtMatrix transform )
{
   register int i;
   

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Transform [\n" );
   
   for ( i=0; i < 4; i++)
     fprintf( fp, "%g %g %g %g\n", transform[i][0], transform[i][1], 
	     transform[i][2], transform[i][3] );
   fprintf( fp, "]\n" );
}


RtVoid RiTransformBegin( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "TransformBegin\n" );
}


RtVoid RiTransformEnd( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "TransformEnd\n" );
}


RtPoint* RiTransformPoints( RtToken fromspace, RtToken tospace,
			   RtInt n, RtPoint points[] )
{
   (void)fromspace; (void)tospace; (void)n; (void)points;

   return NULL;
}


RtVoid RiTranslate( RtFloat dx, RtFloat dy, RtFloat dz )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Translate %g %g %g\n", dx, dy, dz );
}


RtVoid RiMatte( RtBoolean onoff )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Matte %d\n", (onoff ? 1 : 0) );
}


RtVoid RiShadingRate( RtFloat size )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "ShadingRate %g\n", size );
}


RtVoid RiShadingInterpolation( RtToken type )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   /* Pixar's catrib seems to let anything through, so I'm removing
    *  the checking.
    */
#ifdef CHECKING
   if ( !strcmp( type, "constant" ) || !strcmp( type, "smooth" ) )
   {
      fprintf( fp, "ShadingInterpolation \"%s\"\n", type );
   }
#else
   fprintf( fp, "ShadingInterpolation \"%s\"\n", type );
#endif
}


RtVoid RiSubdivisionMesh( RtToken scheme, RtInt nfaces, 
			  RtInt nvertices[], RtInt vertices[],
			  RtInt ntags, RtToken tags[],
			  RtInt nargs[], RtInt intargs[], RtFloat floatargs[],
			  ...)
{
   va_list  ap;
   register int  l,v,i,a,ia,fa;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "SubdivisionMesh \"%s\" [", scheme );
   v = 0;
   for ( i=0; i<nfaces-1; i++ )
   {
      fprintf( fp, "%d ", nvertices[i] );
      v += nvertices[i];
   }
   fprintf( fp, "%d] [", nvertices[i] );
   v += nvertices[i];

   l = vertices[0];
   for ( i=0; i<v-1; i++ )
   {
      fprintf( fp, "%d ", vertices[i] );
      if ( vertices[i] > l )
	l = vertices[i];
   }
   if ( vertices[i] > l )
     l = vertices[i];
   fprintf( fp, "%d] ", vertices[i] );

   if ( ntags )
   {
      /* Print tags[]. */
      fprintf( fp, "[" );
      for ( i=0; i<ntags-1; i++ )
      {
	 fprintf( fp, "\"%s\" ", tags[i] );
      }
      fprintf( fp, "\"%s\"] [", tags[i] );

      /* Print nargs[]. */
      a = 2*(ntags-1);
      for ( i=0; i<a; i+=2 )
      {
	 fprintf( fp, "%d %d", nargs[i], nargs[i+1] );
      }
      fprintf( fp, "%d %d] [", nargs[i], nargs[i+1] );

      /* Print intargs[]. */
      ia = 0;
      fa = 0;
      for ( i=0; i < ntags; i+=2 )
      {
	 ia += nargs[i];
	 fa += nargs[i+1];
      }      
      for ( i=0; i<ia-1; i++ )
      {
	 fprintf( fp, "%d", intargs[i] );
      }
      if (ia)
	 fprintf( fp, "%d]\n[", intargs[i] );
      else
	 fprintf( fp, "] [" );
      
      /* Print floatargs[]. */
      for ( i=0; i<fa-1; i++ )
      {
	 fprintf( fp, "%g", floatargs[i] );
      }
      if (fa)
	 fprintf( fp, "%g]\n", floatargs[i] );
      else
	 fprintf( fp, "]\n" );	 
   }

   va_start(ap,floatargs);
   HandleParamList( ap, -1, NULL, NULL, l+1, nfaces, l+1 );
   va_end(ap);  
}


RtVoid RiSubdivisionMeshV( RtToken scheme, RtInt nfaces, 
			   RtInt nvertices[], RtInt vertices[],
			   RtInt ntags, RtToken tags[],
			   RtInt nargs[], RtInt intargs[], RtFloat floatargs[],
			   RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register int  l,v,i,a,ia,fa;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "SubdivisionMesh \"%s\" [", scheme );
   v = 0;
   for ( i=0; i<nfaces-1; i++ )
   {
      fprintf( fp, "%d ", nvertices[i] );
      v += nvertices[i];
   }
   fprintf( fp, "%d] [", nvertices[i] );
   v += nvertices[i];

   l = vertices[0];
   for ( i=0; i<v-1; i++ )
   {
      fprintf( fp, "%d ", vertices[i] );
      if ( vertices[i] > l )
	l = vertices[i];
   }
   if ( vertices[i] > l )
     l = vertices[i];
   fprintf( fp, "%d] ", vertices[i] );

   if ( ntags )
   {
      /* Print tags[]. */
      fprintf( fp, "[" );
      for ( i=0; i<ntags-1; i++ )
      {
	 fprintf( fp, "\"%s\" ", tags[i] );
      }
      fprintf( fp, "\"%s\"] [", tags[i] );

      /* Print nargs[]. */
      a = 2*(ntags-1);
      for ( i=0; i<a; i+=2 )
      {
	 fprintf( fp, "%d %d ", nargs[i], nargs[i+1] );
      }
      fprintf( fp, "%d %d] [", nargs[i], nargs[i+1] );

      /* Print intargs[]. */
      ia = 0;
      fa = 0;
      for ( i=0; i < 2*ntags; i+=2 )
      {
	 ia += nargs[i];
	 fa += nargs[i+1];
      }      
      for ( i=0; i<ia-1; i++ )
      {
	 fprintf( fp, "%d ", intargs[i] );
      }
      if (ia)
	 fprintf( fp, "%d]\n[", intargs[i] );
      else
	 fprintf( fp, "] [" );
      
      /* Print floatargs[]. */
      for ( i=0; i<fa-1; i++ )
      {
	 fprintf( fp, "%g ", floatargs[i] );
      }
      if (fa)
	 fprintf( fp, "%g]\n", floatargs[i] );
      else
	 fprintf( fp, "]\n" );	 
   }

   HandleParamList( noap, n, tokens, parms, l+1, nfaces, l+1 );
}


RtVoid RiSurface( RtToken name, ... )
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Surface \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiSurfaceV( RtToken name, 
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Surface \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiArchiveRecord( RtToken type, char *format, ... )
{
   char     *p;
   va_list  ap;
   int      hint = 0;


   if (!fp)
     return;

   if ( !strcmp( type, "comment" ) )
   {
      p = "#";
   }
   else if ( !strcmp( type, "structure" ) )
   {
      p = "##";
      hint = 1;
   }
   else
     return;

   /* When writing a RIB a problem appears for such tools as typerib if
    *    the RIB write (client library) does not check the very first
    *    line for a "##RenderMan" hint.  The problem is that two such
    *    hints will appear in typerib's output if the writer outputs
    *    ##RenderMan for RiBegin() and the input from typerib already has
    *    a first line ##RenderMan hint.  
    * When BMRT 2.3.6 libribout.a is linked with typerib, it can be seen
    *    that libribout.a prints "##RenderMan RIB-Structure 1.0\n" when 
    *    RiBegin() is called.  Using Pixar's librib.a with typerib, an extra
    *    ##RenderMan hint does not occur.  Using catrib also shows that
    *    extra code exists to check for a ##RenderMan hint.
    *
    *        % echo "##RenderMan Rib" | catrib
    *        ##RenderMan Rib
    *        % echo "##RenderMan" | catrib
    *        ##RenderMan
    *        % echo "##RenderMa" | catrib
    *        ##RenderMan RIB
    *        ##RenderMa
    *   
    *    The "RenderMa" was intentional and shows how much of the string
    *    is actually being compared since the official hint given on the
    *    specification is of the format: "##RenderMan RIB-Structure 1.0"
    *    with an optional "Entity" append.
    *
    *    Also note with catrib what happens if only hints and comments
    *    are in a RIB and no RIB statements verses having RIB statements:
    *
    *    To show how command shell handles "\n" example input is 
    *    first shown in detail using ribdump.  Note in the following
    *    examples an '!' is used just to show that catrib is not simply
    *    printing its own ##RenderMan hint and is instead taking the 
    *    given one:
    *         % echo "##RenderMan!\nSides 1" | ribdump
    *           #    #    R    e    n    d    e    r   # ##Render
    *           M    a    n    !   \n    S    i    d   # Man!\nSid
    *           e    s  040    1   \n                  # es 1\n
    *
    *    With only hints and comments, catrib doesn't print "version 3.03":
    *         % echo "##RenderMan!\n" | catrib
    *         ##RenderMan!
    *         % echo "##RenderMan!\n#a comment" | catrib
    *         ##RenderMan!
    *         #a comment
    *         % echo "#comment1\n#comment2" | catrib
    *         ##RenderMan RIB
    *         #comment1
    *         #comment2
    *         % echo "##RenderMan RIB\n#a comment" | catrib
    *         ##RenderMan RIB
    *         #a comment
    *
    *    With a RIB statement present, catrib adds a "version 3.03":
    *         % echo "##RenderMan!\nSides 1" | catrib
    *         ##RenderMan!
    *         version 3.03
    *         Sides 1
    *         % echo "#comment1\n#comment2\nSides 1" | catrib
    *         ##RenderMan RIB
    *         #comment1
    *         #comment2
    *         version 3.03
    *         Sides 1
    *         
    *    Some more good test cases:
    *         % echo "#a comment\nSides 1" | catrib
    *         ##RenderMan RIB
    *         #a comment
    *         version 3.03
    *         Sides 1
    *
    *         % echo "##a hint\nSides 1" | catrib
    *         ##RenderMan RIB
    *         ##a hint
    *         version 3.03
    *         Sides 1
    *
    *         % echo "version 3.03\nversion 3.03\nSides 1" | catrib
    *         ##RenderMan RIB
    *         version 3.03
    *         Sides 1
    *
    */
   if ( gSRIBW.firstline==2 )
   {
      /* If a RenderMan RIB hint, the following strncmp will return zero, 
       *    so if a hint and strncmp() returns a nonzero value call 
       *    HandleFirstLine(). 
       */
      if ( !hint 
	   || hint && strncmp( "RenderMan", format, sizeof("RenderMan")-1) )
      {
	 /* RI_TRUE indicates to HandleFirstLine() that it is being called
          *    from RiArchiveRecord().
          */
	 HandleFirstLine( RI_TRUE );
      }
      /* Mark gSRIBW.firstline as having print a ##RenderMan hint. */
      gSRIBW.firstline=1;
   }

   fprintf( fp, "%s", p );
   va_start(ap,format);
   vfprintf( fp, format, ap );
   va_end(ap);
   fprintf( fp, "\n" );
}


RtVoid RiReadArchive( RtToken name, 
		     RtVoid (*callback)(RtToken,char*, ... ), ... )
{
   va_list  ap;


   (void)callback;

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "ReadArchive \"%s\" ", name );
   va_start(ap,callback);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiReadArchiveV( RtToken name, 
		      RtVoid (*callback)( RtToken, char*, ... ),
		      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)callback;

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "ReadArchive \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiAttribute( RtToken name, ... )
{
   va_list  ap;

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Attribute \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiAttributeV( RtToken name,
                 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Attribute \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiAttributeBegin( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "AttributeBegin\n" );

   gSRIBW.blocklevel++;
}


RtVoid RiAttributeEnd( void )
{
   auto UVSTEPS  *p;


   if (!fp)
     return;

   /* If the blocklevel matches the level on the stack
    *    restore the data.
    */
   if ( gSRIBW.stepstack && gSRIBW.blocklevel==gSRIBW.stepstack->blocklevel )
   {
      /* Restore the data. */
      gSRIBW.nustep = gSRIBW.stepstack->nustep;
      gSRIBW.nvstep = gSRIBW.stepstack->nvstep;

      /* Remove item from stack. */
      p = gSRIBW.stepstack;
      gSRIBW.stepstack = gSRIBW.stepstack->next;
      free(p);
   }

   /* Don't let blocklevel go negative. */
   if (gSRIBW.blocklevel)
   {
      gSRIBW.blocklevel--;
   }

   CheckIfFirstLine();

   fprintf( fp, "AttributeEnd\n" );
}


RtVoid RiBound( RtBound bound )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Bound %g %g %g %g %g %g\n",
	   bound[0], bound[1], bound[2], bound[3], bound[4], bound[5] );
}


RtVoid RiColor( RtColor color )
{
   auto int  i;
   

   if (!fp)
     return;

   CheckIfFirstLine();

   if (gSRIBW.ncolor==3)
   {
      fprintf( fp, "Color %g %g %g\n", color[0], color[1], color[2] );
   }
   else
   {
      fprintf( fp, "Color " );
      for ( i=0; i<(gSRIBW.ncolor-1); i++ )
	fprintf( fp, "%g ", color[i] );
      fprintf( fp, "%g\n", color[i] );
   }
}


RtVoid RiOpacity( RtColor color)
{
   auto int  i;
   

   if (!fp)
     return;

   CheckIfFirstLine();

   if (gSRIBW.ncolor==3)
   {
      fprintf( fp, "Opacity %g %g %g\n", color[0], color[1], color[2] );
   }
   else
   {
      fprintf( fp, "Opacity " );
      for ( i=0; i<(gSRIBW.ncolor-1); i++ )
	fprintf( fp, "%g ", color[i] );
      fprintf( fp, "%g\n", color[i] );
   }
}


RtVoid RiOption( RtToken name, ... )
{
   va_list  ap;
   char     *option;
   char     *setting;


   /* Pixar's tools have RiOption("rib","format", ..., NULL ).
    *    Such format options as "binary" and "ascii" are signals to the
    *    RIB writer library and shouldn't be printed out to the RIB.
    *    So skip all calls of the form: RiOption( "rib", ... ).
    *    Atleast this appears to be the behavior of librib.a and libribout.a.
    *
    * Refer to [PIXA93a].
    */
   if (name[0]=='r' && name[1]=='i' && name[2]=='b' && name[3]=='\0' )
   {
      /* The Simple RIB Writer doesn't support binary output and
       *    defaults to ASCII output, so ignore RiOption calls that
       *    tell it which one to use.  
       * RiOption("rib",...) is used with the following parameters:
       *           Option "rib" "format" "ascii"
       *           Option "rib" "format" "binary"
       *
       * 7-16-97:  As of PRMan 3.7 'Option "rib" "compression" ["gzip"]'
       *           was added.
       */

      /* Only set these options before RiBegin().  The global fp is NULL
       * outside of RiBegin()/RiEnd() blocks, so use it to indicate if this
       * call was made before RiBegin()..
       */
      if (!fp)
      {
	 va_start(ap,name);
	 do 
	 {
	    option  = (char*)va_arg(ap,RtToken);
	    if ( option )
	    {
	       setting = (char*)*(void**)va_arg(ap,RtToken);
	       if ( !strcmp( option, "format" ) )
	       {
		  if ( !strcmp(setting,"RISpec3.1") )
		  {
		     gSRIBW.prman36changes = RI_FALSE;
		  }
		  else if ( !strcmp( setting,"NuPatch3.6") )
		  {
		     gSRIBW.prman36changes = RI_TRUE;
		  }
	       }
	    }
	 } while (option);
	 va_end(ap);
      }

      return;
   }

   /* The specification states that RiOption() and RiErrorHandler() can
    *    be called outside of a RiBegin-RiEnd block, which means fp has
    *    to be checked.  See page 15 [PIXA93]. 
    */
   if (!fp)
   {
      /* Don't start calling fprintf calls with a NULL file pointer. 
       *    Escaping now.
       */
      return;
   }

   CheckIfFirstLine();

   fprintf( fp, "Option \"%s\" ", name );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiOptionV( RtToken name, 
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   int  i;


   if (name[0]=='r' && name[1]=='i' && name[2]=='b' && name[3]=='\0' )
   {
      if (!fp)
      {
	 for ( i=0; i<n; i++ )
	 {
	    if ( tokens[i] && !strcmp( tokens[i], "format" ) )
	    {
	       if ( !strcmp( (char*)*(void**)parms[i],"RISpec3.1") )
	       {
		  gSRIBW.prman36changes = RI_FALSE;
	       }
	       else if ( !strcmp( (char*)*(void**)parms[i],"NuPatch3.6") )
	       {
		  gSRIBW.prman36changes = RI_TRUE;
	       }
	    }
	 }
      }

      return;
   }
   /* The specification states that RiOption() and RiErrorHandler() can
    *    be called outside of a RiBegin-RiEnd block, which means fp has
    *    to be checked.  See page 15 [PIXA93]. 
    */
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Option \"%s\" ", name );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiResource( RtToken handle, RtToken type, ... )
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Resource \"%s\" \"%s\" ", handle, type );
   va_start(ap,type);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);
}


RtVoid RiResourceV( RtToken handle, RtToken type,
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Resource \"%s\" \"%s\" ", handle, type );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiReverseOrientation( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "ReverseOrientation\n" );
}


RtVoid RiTextureCoordinates( RtFloat s1, RtFloat t1, 
                         RtFloat s2, RtFloat t2,
                         RtFloat s3, RtFloat t3, 
		         RtFloat s4, RtFloat t4 )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "TextureCoordinates %g %g %g %g %g %g %g %g\n",
	   s1, t1, s2, t2, s3, t3, s4, t4 );
}


RtVoid RiSides( RtInt sides )
{   
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Sides %d\n", sides );
}


RtLightHandle RiLightSource( RtToken name, ... )
{
   va_list  ap;
   register RtLightHandle  light;


   if (!fp)
     return NULL;

   CheckIfFirstLine();

   light = (RtLightHandle)gSRIBW.LastLightHandle++;
   fprintf( fp, "LightSource \"%s\" %u ", name, (unsigned int)(size_t)light );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);

   return light;
}


RtLightHandle RiLightSourceV( RtToken name, 
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register RtLightHandle  light;

   if (!fp)
     return NULL;

   CheckIfFirstLine();

   light = (RtLightHandle)gSRIBW.LastLightHandle++;
   fprintf( fp, "LightSource \"%s\" %u ", name, (unsigned int)(size_t)light );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );

   return light;
}


RtLightHandle RiAreaLightSource( RtToken name, ... )
{
   va_list  ap;
   register RtLightHandle  light;

   if (!fp)
     return NULL;

   CheckIfFirstLine();

   light = (RtLightHandle)gSRIBW.LastLightHandle++;
   fprintf( fp, "AreaLightSource \"%s\" %u ", name, 
	    (unsigned int)(size_t)light );
   va_start(ap,name);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);

   return light;
}


RtLightHandle RiAreaLightSourceV( RtToken name, 
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register RtLightHandle  light;

   if (!fp)
     return NULL;

   CheckIfFirstLine();

   light = (RtLightHandle)gSRIBW.LastLightHandle++;
   fprintf( fp, "AreaLightSource \"%s\" %u ", name, 
	    (unsigned int)(size_t)light );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );

   return light;
}


RtVoid RiBasis( RtBasis ubasis, RtInt ustep, RtBasis vbasis, RtInt vstep )
{
   register int      i;
   register RtFloat  *basis;
   int               uv,step;
   auto UVSTEPS      *p;


   if (!fp)
     return;

   /* If not in an attribute block, then the default values
    *    are written over and saving them on the stack does 
    *    not do anything as there is no way to pop the data
    *    back off.  So check if at level zero before adding
    *    to the step stack.
    */
   if ( gSRIBW.blocklevel )
   {
      p = (UVSTEPS*)malloc(sizeof(UVSTEPS));
      if (!p)
	 return;

      /* Save old uv step values. */
      p->nustep = gSRIBW.nustep;
      p->nvstep = gSRIBW.nvstep;

      /* Save attribute scope level. */
      p->blocklevel = gSRIBW.blocklevel;

      /* Add to the stack. */
      p->next = gSRIBW.stepstack;
      gSRIBW.stepstack = p;
   }

   CheckIfFirstLine();

   fprintf( fp, "Basis " );
   uv = 1;
   do
   {
      basis = (uv ? (RtFloat*)ubasis : (RtFloat*)vbasis );
      step = ( uv ? ustep : vstep );
      if (basis == &RiBezierBasis[0][0] )
      {
	 fprintf( fp, "\"bezier\" %d ", step );
      }
      else if (basis == &RiBSplineBasis[0][0] )
      {
	 fprintf( fp, "\"b-spline\" %d ", step );
      }
      else if (basis == &RiCatmullRomBasis[0][0] )
      {
	 fprintf( fp, "\"catmull-rom\" %d ", step );
      }
      else if (basis == &RiHermiteBasis[0][0] )
      {
	 fprintf( fp, "\"hermite\" %d ", step );
      }
      else if (basis == &RiPowerBasis[0][0] )
      {
	 fprintf( fp, "\"power\" %d ", step );
      }
      else 
      {
	 fputc( '[', fp );
	 for ( i=0; i < 3; i++)
	   fprintf( fp, "%g %g %g %g ", basis[i*4], basis[i*4+1], 
		   basis[i*4+2], basis[i*4+3] );
	 fprintf( fp, "%g %g %g %g] %d ", basis[i*4], basis[i*4+1], 
	         basis[i*4+2], basis[i*4+3], step );
      }
      if (!uv)
	fputc( '\n', fp );
   } 
   while (uv--);

   gSRIBW.nustep = ustep;
   gSRIBW.nvstep = vstep;
}


RtVoid RiPatch( RtToken type, ... )
{
   register int  nvertex;
   va_list       ap;
		

   if (!fp)
     return;

   if ( !strcmp( type, "bilinear") )
      nvertex = 4;
   else if ( !strcmp( type, "bicubic") )
      nvertex = 16;
   else
      return;
      
   CheckIfFirstLine();

   fprintf( fp, "Patch \"%s\" \n", type );
   va_start(ap,type);
   HandleParamList( ap, -1, NULL, NULL, nvertex, 1, 4 );
   va_end(ap);
}


RtVoid RiPatchV( RtToken type, 
		RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register int  nvertex;
		

   if (!fp)
     return;

   if ( !strcmp( type, "bilinear") )
      nvertex = 4;
   else if ( !strcmp( type, "bicubic") )
      nvertex = 16;
   else
      return;
      
   CheckIfFirstLine();

   fprintf( fp, "Patch \"%s\" \n", type );
   HandleParamList( noap, n, tokens, parms, nvertex, 1, 4 ); 
}


RtVoid RiPatchMesh( RtToken type, RtInt nu, RtToken uwrap, 
                RtInt nv, RtToken vwrap, ... )
{
   int           nupatches, nvpatches;
   int           unowrap, vnowrap;
   int           nvarying;
   va_list       ap;


   if (!fp)
     return;

   if ( !strcmp( uwrap, "periodic") )
   {
      unowrap = 0;
   }
   else if ( !strcmp( uwrap, "nonperiodic") )
   {
      unowrap = 1;
   }
   else
   {
      return; /* Error */
   }
   if ( !strcmp( vwrap, "periodic") )
   {
      vnowrap = 0;
   }
   else if ( !strcmp( vwrap, "nonperiodic") )
   {
      vnowrap = 1;
   }
   else
   {
      return; /* Error */
   }
	
   if ( !strcmp( type, "bilinear") )
   {
      nupatches = nu - unowrap;
      nvpatches = nv - vnowrap;
   }
   else if ( !strcmp( type, "bicubic") )
   {
      /* Note that copies of the old RI Specification [PIXA89] are missing
       *    an addition of 1 for nonperiodic.
       */
      nupatches = (nu - (4*unowrap)) / gSRIBW.nustep + unowrap;
      nvpatches = (nv - (4*vnowrap)) / gSRIBW.nvstep + vnowrap;
   }
   else
   {
      return;
   }

   nvarying = (nupatches+unowrap) * (nvpatches+vnowrap);

   CheckIfFirstLine();

   fprintf( fp, "PatchMesh \"%s\" %d \"%s\" %d \"%s\" \n",
	   type, nu, uwrap, nv, vwrap );
   va_start(ap,vwrap);
   HandleParamList( ap, -1, NULL, NULL, nu*nv, nupatches*nvpatches, nvarying );
   va_end(ap);
}


RtVoid RiPatchMeshV( RtToken type, RtInt nu, RtToken uwrap, 
		    RtInt nv, RtToken vwrap, 
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   int           nupatches, nvpatches;
   int           unowrap, vnowrap;
   int           nvarying;


   if (!fp)
     return;

   if ( !strcmp( uwrap, "periodic") )
   {
      unowrap = 0;
   }
   else if ( !strcmp( uwrap, "nonperiodic") )
   {
      unowrap = 1;
   }
   else
   {
      return; /* Error */
   }
   if ( !strcmp( vwrap, "periodic") )
   {
      vnowrap = 0;
   }
   else if ( !strcmp( vwrap, "nonperiodic") )
   {
      vnowrap = 1;
   }
   else
   {
      return; /* Error */
   }

   if ( !strcmp( type, "bilinear") )
   {
      nupatches = nu - unowrap;
      nvpatches = nv - vnowrap;
   }
   else if ( !strcmp( type, "bicubic") )
   {
      /* Note that copies of the old RI Specification [PIXA89] are missing
       *    an addition of 1 for nonperiodic.
       */
      nupatches = (nu - (4*unowrap)) / gSRIBW.nustep + unowrap;
      nvpatches = (nv - (4*vnowrap)) / gSRIBW.nvstep + vnowrap;
   }
   else
   {
      return;
   }

   nvarying = (nupatches+unowrap) * (nvpatches+vnowrap);

   CheckIfFirstLine();

   fprintf( fp, "PatchMesh \"%s\" %d \"%s\" %d \"%s\" \n",
	   type, nu, uwrap, nv, vwrap );
   HandleParamList( noap, n,tokens,parms, nu*nv,nupatches*nvpatches,nvarying );
}


RtVoid RiNuCurves( RtInt ncurves, RtInt nvertices[], RtInt order[],
	       RtFloat knot[], RtFloat min[], RtFloat max[], ... )
{
   int           N; /* number of control vertices */
   int           O; /* sum of order[] values */
   va_list       ap;
   register int  i;


   if (!fp)
     return;

   CheckIfFirstLine();

   /* When writing the NuCurves RIB statement ncurves is not printed.
    * Instead the length of nvertices is used to set the ncurves value
    * when reading a NuCurves statement.
    */
   fprintf( fp, "NuCurves [");
   N = 0;
   O = 0;
   for ( i=0; i<ncurves; i++ )
   {
     fprintf( fp, "%d ", nvertices[i] );
      N += nvertices[i];
      O += order[i];
   }
   fprintf( fp, "] [" );
   for ( i=0; i<ncurves; i++ )
   {
     fprintf( fp, "%d ", order[i] );
   }
   /* The number of knots is the number of control vertices plus the order. */
   fprintf( fp, "]\n[" );
   for ( i=0; i<N+O; i++ )
   {
     fprintf( fp, "%g ", knot[i] );
   }
   fprintf( fp, "]\n[" );
   for ( i=0; i<ncurves; i++ )
   {
     fprintf( fp, "%g ", min[i] );
   }
   fprintf( fp, "]\n" );
   for ( i=0; i<ncurves; i++ )
   {
     fprintf( fp, "%g ", max[i] );
   }

   /* Calculate the number of uniform values. */
   i = N - O + ncurves;

   va_start(ap,max);
   HandleParamList( ap, -1, NULL, NULL, 
		    /* uniform */ i, 
		    /* vertex  */ N, 
		    /* varying */ ncurves + i );
   va_end(ap);
}


RtVoid RiNuCurvesV( RtInt ncurves, RtInt nvertices[], RtInt order[],
		    RtFloat knot[], RtFloat min[], RtFloat max[], 
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   int           N; /* number of control vertices */
   int           O; /* sum of order[] values */
   register int  i;


   if (!fp)
     return;

   CheckIfFirstLine();

   /* When writing the NuCurves RIB statement ncurves is not printed.
    * Instead the length of nvertices is used to set the ncurves value
    * when reading a NuCurves statement.
    */
   fprintf( fp, "NuCurves [");
   N = 0;
   O = 0;
   for ( i=0; i<ncurves; i++ )
   {
     fprintf( fp, "%d ", nvertices[i] );
      N += nvertices[i];
      O += order[i];
   }
   fprintf( fp, "] [" );
   for ( i=0; i<ncurves; i++ )
   {
     fprintf( fp, "%d ", order[i] );
   }
   /* The number of knots is the number of control vertices plus the order. */
   fprintf( fp, "]\n[" );
   for ( i=0; i<N+O; i++ )
   {
     fprintf( fp, "%g ", knot[i] );
   }
   fprintf( fp, "]\n[" );
   for ( i=0; i<ncurves; i++ )
   {
     fprintf( fp, "%g ", min[i] );
   }
   fprintf( fp, "]\n" );
   for ( i=0; i<ncurves; i++ )
   {
     fprintf( fp, "%g ", max[i] );
   }

   /* Calculate the number of uniform values. */
   i = N - O + ncurves;

   HandleParamList( noap, n,tokens,parms, 
		    /* uniform */ i, 
		    /* vertex  */ N, 
		    /* varying */ ncurves + i );
}


RtVoid RiNuPatch( RtInt nu, RtInt uorder, RtFloat uknot[], 
		 RtFloat umin, RtFloat umax,
		 RtInt nv, RtInt vorder, RtFloat vknot[], 
		 RtFloat vmin, RtFloat vmax, ... )
{
   register int  i;
   int           nvertex, nvarying, nuniform;
   va_list       ap;


   if (!fp)
     return;

   nvertex  = nu*nv;
   if ( gSRIBW.prman36changes )
   {
      nuniform = (1+nu-uorder) * (1+nv-vorder);
      nvarying = (2+nu-uorder) * (2+nv-vorder);
   }
   else
   {
      nuniform = 1;
      nvarying = 4;
   }

   CheckIfFirstLine();

   fprintf( fp, "NuPatch %d %d [", nu, uorder );
   for ( i=0; i<(uorder+nu); i++ )
     fprintf( fp, "%g ", uknot[i] );
   fprintf( fp, "] %g %g ", umin, umax );

   fprintf( fp, "%d %d [", nv, vorder );
   for ( i=0; i<(vorder+nv); i++ )
     fprintf( fp, "%g ", vknot[i] );
   fprintf( fp, "] %g %g ", vmin, vmax );

   va_start(ap,vmax);
   HandleParamList( ap, -1, NULL, NULL, nvertex, nuniform, nvarying );
   va_end(ap);   
}


RtVoid RiNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
		  RtFloat umin, RtFloat umax, 
		  RtInt nv, RtInt vorder, RtFloat vknot[],
		  RtFloat vmin, RtFloat vmax,
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register int  i;
   int           nvertex, nvarying, nuniform;
		

   if (!fp)
     return;

   nvertex  = nu*nv;
   if ( gSRIBW.prman36changes )
   {
      nuniform = (1+nu-uorder) * (1+nv-vorder);
      nvarying = (2+nu-uorder) * (2+nv-vorder);
   }
   else
   {
      nuniform = 1;
      nvarying = 4;
   }

   CheckIfFirstLine();

   fprintf( fp, "NuPatch %d %d [", nu, uorder );
   for ( i=0; i<(uorder+nu); i++ )
     fprintf( fp, "%g ", uknot[i] );
   fprintf( fp, "] %g %g ", umin, umax );

   fprintf( fp, "%d %d [", nv, vorder );
   for ( i=0; i<(vorder+nv); i++ )
     fprintf( fp, "%g ", vknot[i] );
   fprintf( fp, "] %g %g ", vmin, vmax );

   HandleParamList( noap, n, tokens, parms, nvertex, nuniform, nvarying );
}


/* Refer to [UPST90] pages 249 to 250 for information on the sizes 
 *    of the arrays used in RiTrimCurve().
 */
RtVoid RiTrimCurve( RtInt nloops, RtInt ncurves[], RtInt order[], 
		   RtFloat knot[], RtFloat min[], RtFloat max[], 
		   RtInt n[], RtFloat u[], RtFloat v[], RtFloat w[] )
{
   int  C; /* number of curves */
   int  nknots; /* number of knots */
   int  ncvs; /* number of control vertices */
   register int  i;
	
	
   if (!fp)
     return;

   CheckIfFirstLine();

   C = 0;
   fprintf( fp, "TrimCurve [" );
   for ( i=0; i<nloops-1; i++ )
   {
      fprintf( fp, "%d ", ncurves[i] );
      C += ncurves[i];
   }
   fprintf( fp, "%d] [", ncurves[i] );
   C += ncurves[i];

   nknots = 0;
   ncvs = 0;
   for ( i=0; i<C-1; i++ )
   {
      fprintf( fp, "%d ", order[i] );
      nknots += order[i] + n[i];
      ncvs += n[i];
   }
   fprintf( fp, "%d] [", order[i] );
   nknots += order[i] + n[i];
   ncvs += n[i];

   for ( i=0; i<nknots-1; i++ )
     fprintf( fp, "%g ", knot[i] );
   fprintf( fp, "%g] [", knot[i] );

   for ( i=0; i<C-1; i++ )
     fprintf( fp, "%g ", min[i] );
   fprintf( fp, "%g] [", min[i] );

   for ( i=0; i<C-1; i++ )
     fprintf( fp, "%g ", max[i] );
   fprintf( fp, "%g] [", max[i] );

   for ( i=0; i<C-1; i++ )
     fprintf( fp, "%d ", n[i] );
   fprintf( fp, "%d]\n[", n[i] );

   for ( i=0; i<ncvs-1; i++ )
     fprintf( fp, "%g ", u[i] );
   fprintf( fp, "%g]\n[", u[i] );

   for ( i=0; i<ncvs-1; i++ )
     fprintf( fp, "%g ", v[i] );
   fprintf( fp, "%g]\n[", v[i] );

   for ( i=0; i<ncvs-1; i++ )
     fprintf( fp, "%g ", w[i] );
   fprintf( fp, "%g]\n", w[i] );
}


RtVoid RiCone( RtFloat height, RtFloat radius, RtFloat thetamax, ... )
{
   va_list ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Cone %g %g %g ", height, radius, thetamax );
   va_start(ap,thetamax);
   HandleParamList( ap, -1, NULL, NULL, 4, 1, 4 );
   va_end(ap);  
}


RtVoid RiConeV( RtFloat height, RtFloat radius, RtFloat thetamax,
	       RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Cone %g %g %g ", height, radius, thetamax );
   HandleParamList( noap, n, tokens, parms, 4, 1, 4 );
}

 
RtVoid RiCurves( RtToken type, RtInt ncurves, RtInt nvertices[], 
		 RtToken wrap, ... )
{
   RtInt            nonperiodic;
   RtInt            uniform,varying,vertex;
   va_list          ap;
   register  RtInt  i;


   if (!fp || gSRIBW.nvstep==0 )
     return;

   if ( !strcmp( wrap, "periodic") )
     nonperiodic = 0;
   else if ( !strcmp( wrap, "nonperiodic") )
     nonperiodic = 1;
   else
     return;

   vertex = 0;
   for ( i=0; i<ncurves; i++ )
   {
      vertex += nvertices[i];
   }
   if ( !strcmp( type, "linear") )
   {
      if ( nonperiodic )
      {
	 uniform = vertex - ncurves;
	 varying = vertex;
      }
      else
      {
	 varying = uniform = vertex;
      }
   }
   else if ( !strcmp( type, "cubic") )
   {
      if ( nonperiodic )
      {
	 uniform = ncurves + (vertex - 4*ncurves) / gSRIBW.nvstep;
	 varying = ncurves + uniform;	 
      }
      else
      {
	 varying = uniform = vertex / gSRIBW.nvstep;
      }
   }
   else
   {
      return; /* Error */
   }

   CheckIfFirstLine();

   fprintf( fp, "Curves \"%s\" [", type );
   for ( i=0; i<(ncurves-1); i++)
   {
      fprintf( fp, "%d ", nvertices[i] );
   }
   fprintf( fp, "%d] \"%s\" ", nvertices[i], wrap );

   va_start(ap,wrap);
   HandleParamList( ap, -1, NULL, NULL, 
		   /* vertex */  vertex, 
		   /* uniform */ uniform, 
		   /* varying */ varying );
   va_end(ap);  
}


RtVoid RiCurvesV( RtToken type, RtInt ncurves, RtInt nvertices[], 
		 RtToken wrap, 
		 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   RtInt           nonperiodic;
   RtInt           uniform,varying,vertex;
   register RtInt  i;


   if (!fp || gSRIBW.nvstep==0 )
     return;

   if ( !strcmp( wrap, "periodic") )
     nonperiodic = 0;
   else if ( !strcmp( wrap, "nonperiodic") )
     nonperiodic = 1;
   else
     return;

   vertex = 0;
   for ( i=0; i<ncurves; i++ )
   {
      vertex += nvertices[i];
   }
   if ( !strcmp( type, "linear") )
   {
      if ( nonperiodic )
      {
	 uniform = vertex - ncurves;
	 varying = vertex;
      }
      else
      {
	 varying = uniform = vertex;
      }
   }
   else if ( !strcmp( type, "cubic") )
   {
      if ( nonperiodic )
      {
	 uniform = ncurves + (vertex - 4*ncurves) / gSRIBW.nvstep;
	 varying = ncurves + uniform;	 
      }
      else
      {
	 varying = uniform = vertex / gSRIBW.nvstep;
      }
   }
   else
   {
      return; /* Error */
   }

   CheckIfFirstLine();

   fprintf( fp, "Curves \"%s\" [", type );
   for ( i=0; i<(ncurves-1); i++)
   {
      fprintf( fp, "%d ", nvertices[i] );
   }
   fprintf( fp, "%d] \"%s\" ", nvertices[i], wrap );

   HandleParamList( noap, 
		   n, tokens, parms, 
		   /* vertex */  vertex, 
		   /* uniform */ uniform, 
		   /* varying */ varying );
}


RtVoid RiCylinder( RtFloat radius,RtFloat zmin,RtFloat zmax,
		  RtFloat thetamax, ... )
{
   va_list ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Cylinder %g %g %g %g ", radius, zmin, zmax, thetamax );
   va_start(ap,thetamax);
   HandleParamList( ap, -1, NULL, NULL, 4, 1, 4 );
   va_end(ap);  
}


RtVoid RiCylinderV( RtFloat radius,RtFloat zmin,RtFloat zmax,RtFloat thetamax,
	      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Cylinder %g %g %g %g ", radius, zmin, zmax, thetamax );
   HandleParamList( noap, n, tokens, parms, 4, 1, 4 );
}


RtVoid RiDisk( RtFloat height, RtFloat radius, RtFloat thetamax, ... )
{
   va_list ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Disk %g %g %g ", height, radius, thetamax );
   va_start(ap,thetamax);
   HandleParamList( ap, -1, NULL, NULL, 4, 1, 4 );
   va_end(ap);  
}


RtVoid RiDiskV( RtFloat height, RtFloat radius, RtFloat thetamax,
	  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Disk %g %g %g ", height, radius, thetamax );
   HandleParamList( noap, n, tokens, parms, 4, 1, 4 );
}


RtVoid RiHyperboloid( RtPoint point1, RtPoint point2, RtFloat thetamax, ... )
{
   va_list ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Hyperboloid %g %g %g %g %g %g %g ", 
	   point1[0], point1[1], point1[2], 
	   point2[0], point2[1], point2[2], 
	   thetamax );
   va_start(ap,thetamax);
   HandleParamList( ap, -1, NULL, NULL, 4, 1, 4 );
   va_end(ap);  
}


RtVoid RiHyperboloidV( RtPoint point1, RtPoint point2, RtFloat thetamax,
		 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Hyperboloid %g %g %g %g %g %g %g ", 
	   point1[0], point1[1], point1[2], 
	   point2[0], point2[1], point2[2], 
	   thetamax );
   HandleParamList( noap, n, tokens, parms, 4, 1, 4 );
}


RtVoid RiParaboloid( RtFloat rmax,RtFloat zmin,RtFloat zmax, 
		    RtFloat thetamax,... )
{
   va_list ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Paraboloid %g %g %g %g ", rmax, zmin, zmax, thetamax );
   va_start(ap,thetamax);
   HandleParamList( ap, -1, NULL, NULL, 4, 1, 4 );
   va_end(ap);  
}


RtVoid RiParaboloidV( RtFloat rmax,RtFloat zmin,RtFloat zmax,RtFloat thetamax,
		RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Paraboloid %g %g %g %g ", rmax, zmin, zmax, thetamax );
   HandleParamList( noap, n, tokens, parms, 4, 1, 4 );
}


RtVoid RiSphere( RtFloat radius,RtFloat zmin,RtFloat zmax, 
		RtFloat thetamax, ... )
{
   va_list ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Sphere %g %g %g %g ", radius, zmin, zmax, thetamax );
   va_start(ap,thetamax);
   HandleParamList( ap, -1, NULL, NULL, 4, 1, 4 );
   va_end(ap);  
}


RtVoid RiSphereV( RtFloat radius,RtFloat zmin,RtFloat zmax, RtFloat thetamax,
	     RtInt n, RtToken tokens[], RtPointer parms[] ) 
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Sphere %g %g %g %g ", radius, zmin, zmax, thetamax );
   HandleParamList( noap, n, tokens, parms, 4, 1, 4 );
}


RtVoid RiTorus( RtFloat majorradius, RtFloat minorradius, 
	       RtFloat phimin, RtFloat phimax, RtFloat thetamax, ... )
{
   va_list ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Torus %g %g %g %g %g ", majorradius, minorradius,
	   phimin, phimax, thetamax );
   va_start(ap,thetamax);
   HandleParamList( ap, -1, NULL, NULL, 4, 1, 4 );
   va_end(ap);  
}


RtVoid RiTorusV( RtFloat majorradius, RtFloat minorradius, 
		RtFloat phimin, RtFloat phimax, RtFloat thetamax, 
		RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Torus %g %g %g %g %g ", majorradius, minorradius,
	   phimin, phimax, thetamax );
   HandleParamList( noap, n, tokens, parms, 4, 1, 4 );
}


RtVoid RiGeneralPolygon( RtInt nloops, RtInt nvertices[], ... )
{
   register int  i;
   register int  v = 0;
   va_list ap;
		

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "GeneralPolygon [" );
   for ( i=0; i< nloops-1; i++ )
   {
      fprintf( fp, "%d ", nvertices[i] );
      v += nvertices[i];
   }
   fprintf( fp, "%d] ", nvertices[i] );
   v += nvertices[i];

   va_start(ap,nvertices);
   HandleParamList( ap, -1, NULL, NULL, v, 1, v );
   va_end(ap);  
}


RtVoid RiGeneralPolygonV( RtInt nloops, RtInt nvertices[],
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register int  i;
   register int  v = 0;
		

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "GeneralPolygon [" );
   for ( i=0; i< nloops-1; i++ )
   {
      fprintf( fp, "%d ", nvertices[i] );
      v += nvertices[i];
   }
   fprintf( fp, "%d]\n ", nvertices[i] );
   v += nvertices[i];

   HandleParamList( noap, n, tokens, parms, v, 1, v );
}

 
RtVoid RiPoints( RtInt npoints, ... )
{
   va_list ap;
		

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Points " );

   va_start(ap,npoints);
   HandleParamList( ap, -1, NULL, NULL, npoints, 1, npoints );
   va_end(ap);  
}


RtVoid RiPointsV( RtInt npoints, RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Points " );

   HandleParamList( noap, n, tokens, parms, npoints, 1, npoints );
}


RtVoid RiPointsGeneralPolygons( RtInt npolys, RtInt nloops[], 
			RtInt nvertices[], RtInt vertices[], ... )
{
   register int  l,v,i;
   va_list       ap;
		

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "PointsGeneralPolygons [" );
   l = 0;
   for ( i=0; i< npolys-1; i++ )
   {
      fprintf( fp, "%d ", nloops[i] );
      l += nloops[i];
   }
   fprintf( fp, "%d] [", nloops[i] );
   l += nloops[i];

   v = 0;
   for ( i=0; i< l-1; i++ )
   {
      fprintf( fp, "%d ", nvertices[i] );
      v += nvertices[i];
   }
   fprintf( fp, "%d] [", nvertices[i] );
   v += nvertices[i];

   l = vertices[0];
   for ( i=0; i< v-1; i++ )
   {
      fprintf( fp, "%d ", vertices[i] );
      if ( vertices[i] > l )
	l = vertices[i];
   }
   if ( vertices[i] > l )
     l = vertices[i];
   fprintf( fp, "%d] ", vertices[i] );

   va_start(ap,vertices);
   HandleParamList( ap, -1, NULL, NULL, l+1, npolys, l+1 );
   va_end(ap);  
}


RtVoid RiPointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
			 RtInt nvertices[], RtInt vertices[], 
			 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register int  l,v,i;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "PointsGeneralPolygons [" );
   l = 0;
   for ( i=0; i< npolys-1; i++ )
   {
      fprintf( fp, "%d ", nloops[i] );
      l += nloops[i];
   }
   fprintf( fp, "%d] [", nloops[i] );
   l += nloops[i];

   v = 0;
   for ( i=0; i< l-1; i++ )
   {
      fprintf( fp, "%d ", nvertices[i] );
      v += nvertices[i];
   }
   fprintf( fp, "%d] [", nvertices[i] );
   v += nvertices[i];

   l = vertices[0];
   for ( i=0; i< v-1; i++ )
   {
      fprintf( fp, "%d ", vertices[i] );
      if ( vertices[i] > l )
	l = vertices[i];
   }
   if ( vertices[i] > l )
     l = vertices[i];
   fprintf( fp, "%d] ", vertices[i] );

   HandleParamList( noap, n, tokens, parms, l+1, npolys, l+1 );
}


RtVoid RiPointsPolygons( RtInt npolys, 
			RtInt nvertices[], RtInt vertices[], ... )
{
   register int  l,v,i;
   va_list       ap;
		

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "PointsPolygons [" );

   v = 0;
   for ( i=0; i < npolys-1; i++ )
   {
      fprintf( fp, "%d ", nvertices[i] );
      v += nvertices[i];
   }
   fprintf( fp, "%d] [", nvertices[i] );
   v += nvertices[i];

   l = vertices[0];
   for ( i=0; i< v-1; i++ )
   {
      fprintf( fp, "%d ", vertices[i] );
      if ( vertices[i] > l )
	l = vertices[i];
   }
   if ( vertices[i] > l )
     l = vertices[i];
   fprintf( fp, "%d] ", vertices[i] );

   va_start(ap,vertices);
   HandleParamList( ap, -1, NULL, NULL, l+1, npolys, l+1 );
   va_end(ap);  
}


RtVoid RiPointsPolygonsV( RtInt npolys, RtInt nvertices[], RtInt vertices[],
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register int  l,v,i;
		

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "PointsPolygons [" );

   v = 0;
   for ( i=0; i < npolys-1; i++ )
   {
      fprintf( fp, "%d ", nvertices[i] );
      v += nvertices[i];
   }
   fprintf( fp, "%d] [", nvertices[i] );
   v += nvertices[i];

   l = vertices[0];
   for ( i=0; i< v-1; i++ )
   {
      fprintf( fp, "%d ", vertices[i] );
      if ( vertices[i] > l )
	l = vertices[i];
   }
   if ( vertices[i] > l )
     l = vertices[i];
   fprintf( fp, "%d] ", vertices[i] );

   HandleParamList( noap, n, tokens, parms, l+1, npolys, l+1 );
}


RtVoid RiPolygon( RtInt nvertices, ... )
{
   va_list       ap;
		

   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Polygon " );

   va_start(ap,nvertices);
   HandleParamList( ap, -1, NULL, NULL, nvertices, 1, nvertices );
   va_end(ap);  
}


RtVoid RiPolygonV( RtInt nvertices,
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Polygon " );

   HandleParamList( noap, n, tokens, parms, nvertices, 1, nvertices );
}


RtVoid RiColorSamples( RtInt n, RtFloat nRGB[], RtFloat RGBn[] )
{
   register int  i;


   if (!fp)
     return;

   gSRIBW.ncolor = n;

   CheckIfFirstLine();

   fprintf( fp, "ColorSamples [" );
   for ( i=0; i<(n*3 - 1); i++ )
     fprintf( fp, "%g ", nRGB[i] );
   fprintf( fp, "%g] [", nRGB[i] );

   for ( i=0; i<(n*3-1); i++ )
     fprintf( fp, "%g ", RGBn[i] );
   fprintf( fp, "%g]\n", RGBn[i] );
}


RtVoid RiHider( RtToken type, ... )
{
   va_list       ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Hider \"%s\" ", type );

   va_start(ap,type);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);  
}


RtVoid RiHiderV( RtToken type,
	    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Hider \"%s\" ", type );

   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiDetail( RtBound bound )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Detail %g %g %g %g %g %g\n",
	   bound[0], bound[1], bound[2], bound[3], bound[4], bound[5] );
}


RtVoid RiDetailRange( RtFloat minvisible, RtFloat lowertransition, 
		 RtFloat uppertransition, RtFloat maxvisible )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "DetailRange %g %g %g %g\n", 
	   minvisible, lowertransition, 
	   uppertransition, maxvisible );
}


RtVoid RiGeometricApproximation( RtToken type, RtFloat value )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "GeometricApproximation \"%s\" %g\n", type, value );
}


RtVoid RiGeometricRepresentation( RtToken type )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "GeometricRepresentation \"%s\"\n", type );
}


RtVoid RiGeometry( RtToken type, ... )
{
   va_list ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Geometry \"%s\" ", type );
   va_start(ap,type);
   HandleParamList( ap, -1, NULL, NULL, 4, 1, 4 );
   va_end(ap);  
}


RtVoid RiGeometryV( RtToken type, 
	      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "Geometry \"%s\" ", type );
   HandleParamList( noap, n, tokens, parms, 4, 1, 4 );
}


RtVoid RiOrientation( RtToken orientation )
{
   if (!fp)
     return;

   if (   !strcmp( orientation, "lh" )
       || !strcmp( orientation, "rh" ))
   {
      CheckIfFirstLine();

      fprintf( fp, "Orientation \"%s\"\n", orientation );
   }
}


RtVoid RiProcedural( RtPointer data, RtBound bound,
                RtVoid (*subdivfunc)(RtPointer, RtFloat),
                RtVoid (*freefunc)(RtPointer) )
{
   char  *type;
   char  *data1;
   char  *data2;

   (void)freefunc;

   if (!fp)
     return;

   data1 = ((char**)data)[0];
   if ( RiProcDelayedReadArchive == subdivfunc )
   {
      type = "DelayedReadArchive";
      data2 = NULL;
   }
   else if ( RiProcDynamicLoad == subdivfunc )
   {
      type = "DynamicLoad";
      data2 = ((char**)data)[1];
   }
   else if ( RiProcRunProgram == subdivfunc )
   {
      type = "RunProgram";
      data2 = ((char**)data)[1];
   }
   else
   {
      return;
   }

   CheckIfFirstLine();

   fprintf( fp, "Procedural \"%s\" ", type );
   if (data2)
   {
      fprintf( fp, "[\"%s\" \"%s\"] ", data1, data2 );
   }
   else
   {
      fprintf( fp, "[\"%s\"] ", data1 );
   }
   fprintf( fp, "[%g %g %g %g %g %g]\n", 
	   bound[0], bound[1], bound[2], bound[3], bound[4], bound[5] );

   return;
}


RtVoid RiRelativeDetail( RtFloat relativedetail )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "RelativeDetail %g\n", relativedetail );
}


RtVoid RiMakeBump( char *picturename, char *texturename, 
		  RtToken swrap, RtToken twrap,
		  RtFilterFunc filterfunc, 
		  RtFloat swidth, RtFloat twidth, ... )
{
   char     *p;
   va_list  ap;


   if (!fp)
     return;

   p = HandleGettingFilterName( filterfunc );
   if (!p)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeBump \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %g %g ",
	  picturename, texturename, swrap, twrap, p, swidth, twidth );
   va_start(ap,twidth);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);  
}


RtVoid RiMakeBumpV( char *picturename, char *texturename, 
		   RtToken swrap, RtToken twrap,
		   RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
		   RtInt n, RtToken tokens[], RtPointer parms[] )
{
   char     *p;


   if (!fp)
     return;

   p = HandleGettingFilterName( filterfunc );
   if (!p)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeBump \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %g %g ",
	  picturename, texturename, swrap, twrap, p, swidth, twidth );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiMakeCubeFaceEnvironment( char *px, char *nx, char *py, char *ny,
				 char *pz, char *nz, char *texturename, 
				 RtFloat fov,
				 RtFilterFunc filterfunc, 
				 RtFloat swidth, RtFloat twidth, ... )
{
   char     *p;
   va_list  ap;


   if (!fp)
     return;

   p = HandleGettingFilterName( filterfunc );
   if (!p)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeCubeFaceEnvironment "                             \
	  "\"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %g "  \
	  "\"%s\" %g %g ",
	  px, nx, py, ny, pz, nz, texturename, fov, p, swidth, twidth );
   va_start(ap,twidth);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);  
}


RtVoid RiMakeCubeFaceEnvironmentV( char *px, char *nx, char *py, char *ny,
				  char *pz, char *nz, char *texturename, 
				  RtFloat fov,
				  RtFilterFunc filterfunc, 
				  RtFloat swidth, RtFloat twidth,
				  RtInt n, 
				  RtToken tokens[], RtPointer parms[] )
{
   char     *p;


   if (!fp)
     return;

   p = HandleGettingFilterName( filterfunc );
   if (!p)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeCubeFaceEnvironment "                        \
	  "\"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %g "  \
	  "\"%s\" %g %g ",
	  px, nx, py, ny, pz, nz, texturename, fov, p, swidth, twidth );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiMakeLatLongEnvironment( char *picturename, char *texturename, 
				RtFilterFunc filterfunc,
				RtFloat swidth, RtFloat twidth, ...)
{
   char     *p;
   va_list  ap;


   if (!fp)
     return;

   p = HandleGettingFilterName( filterfunc );
   if (!p)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeLatLongEnvironment \"%s\" \"%s\" \"%s\" %g %g ",
	  picturename, texturename, p, swidth, twidth );
   va_start(ap,twidth);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);  
}


RtVoid RiMakeLatLongEnvironmentV( char *picturename, char *texturename, 
				 RtFilterFunc filterfunc,
				 RtFloat swidth, RtFloat twidth,
				 RtInt n, 
				 RtToken tokens[], RtPointer parms[] )
{
   char     *p;


   if (!fp)
     return;

   p = HandleGettingFilterName( filterfunc );
   if (!p)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeLatLongEnvironment \"%s\" \"%s\" \"%s\" %g %g ",
	  picturename, texturename, p, swidth, twidth );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiMakeShadow( char *picturename, char *texturename, ... )
{
   va_list  ap;


   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeShadow \"%s\" \"%s\" ", picturename, texturename );
   va_start(ap,texturename);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);  
}


RtVoid RiMakeShadowV( char *picturename, char *texturename,
		 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeShadow \"%s\" \"%s\" ", picturename, texturename );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}


RtVoid RiMakeTexture( char *picturename, char *texturename, 
		     RtToken swrap, RtToken twrap,
		     RtFilterFunc filterfunc, 
		     RtFloat swidth, RtFloat twidth, ... )
{
   char     *p;
   va_list  ap;


   if (!fp)
     return;

   p = HandleGettingFilterName( filterfunc );
   if (!p)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeTexture \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %g %g ",
	  picturename, texturename, swrap, twrap, p, swidth, twidth );
   va_start(ap,twidth);
   HandleParamList( ap, -1, NULL, NULL, 1, 1, 1 );
   va_end(ap);  
}


RtVoid RiMakeTextureV( char *picturename, char *texturename, 
		      RtToken swrap, RtToken twrap,
		      RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
		      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   char     *p;


   if (!fp)
     return;

   p = HandleGettingFilterName( filterfunc );
   if (!p)
     return;

   CheckIfFirstLine();

   fprintf( fp, "MakeTexture \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %g %g ",
	  picturename, texturename, swrap, twrap, p, swidth, twidth );
   HandleParamList( noap, n, tokens, parms, 1, 1, 1 );
}

RtVoid RiWorldBegin( void ) 
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "WorldBegin\n" );
}


RtVoid RiWorldEnd( void )
{
   if (!fp)
     return;

   CheckIfFirstLine();

   fprintf( fp, "WorldEnd\n" );
}


RtVoid RiErrorHandler( /* RtFunc (3.5 CHANGE)*/ RtErrorHandler handler )
{
   char  *s;


   /* The specification states that RiOption() and RiErrorHandler() can
    *    be called outside of a RiBegin-RiEnd block, which means fp has
    *    to be checked.  See page 15 [PIXA93]. 
    */
   if (!fp)
     return;

   if ( handler == RiErrorIgnore )
     s = "ignore";
   else if ( handler == RiErrorPrint )
     s = "print";
   else if ( handler == RiErrorAbort )
     s = "abort";
   else
     return;

   CheckIfFirstLine();

   fprintf( fp, "ErrorHandler \"%s\"\n", s );

   return;
}


RtVoid RiErrorIgnore( RtInt code, RtInt severity, char *msg )
{
   (void)code; (void)severity; (void)msg;

   return;
}


RtVoid RiErrorPrint( RtInt code, RtInt severity, char *msg )
{
   (void)code; (void)severity; (void)msg;

   return;
}

RtVoid RiErrorAbort( RtInt code, RtInt severity, char *msg )
{
   (void)code; (void)severity; (void)msg;

   return;
}


RtFloat RiBoxFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtFloat RiTriangleFilter( RtFloat x, RtFloat y, 
			 RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtFloat RiCatmullRomFilter( RtFloat x, RtFloat y, 
			   RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtFloat RiGaussianFilter( RtFloat x, RtFloat y, 
			 RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtFloat RiSincFilter( RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth )
{
   (void)x; (void)y; (void)xwidth; (void)ywidth;

   return 0.0;
}


RtVoid RiProcDelayedReadArchive( RtPointer data, RtFloat detail )
{
   (void)data; (void)detail;
}


RtVoid RiProcRunProgram( RtPointer data, RtFloat detail )
{
   (void)data; (void)detail;
}

RtVoid RiProcDynamicLoad( RtPointer data, RtFloat detail )
{
   (void)data; (void)detail;
}
