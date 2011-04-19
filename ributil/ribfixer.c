/* 
 * Copyright (c) 1995-1999 Thomas E. Burge.  All rights reserved.
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
 *                Copyright (c) 1995-1999 Thomas E. Burge.  
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
 * FILE:  ribfixer.c
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *  
 *    History:
 *        ?-?-96  Created.  
 *      12-14-97  Added AliasToRenderMan 8.2 flag.
 *      10-17-99  Sometime back had added table size to RibOpen() and
 *                now changed the constant from kRIB_RILEVEL98 to be
 *                kRIB_LAST_RI.
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
 * 
 *  PowerAnimator, Designer, Sketch! and Alias are trademarks of 
 *     Alias|Wavefront.
 *  MacroModel is a trademark of MacroMedia.
 *  Unix is a registered trademark of SCO -- of course it might be somebody 
 *     else's this week -- I lost track.
 *  Amapi is a trademark of Yonowat.
 *
 */
#define  _RIPRIV_FUNC_TYPES 
#include <string.h>
#include <ributil.h>
#include <ribattrstore.h>
#include <ribtrimstore.h>


typedef struct _HPOINT {
   RtFloat  x;
   RtFloat  y;
   RtFloat  z;
   RtFloat  h;
} HPOINT;

typedef struct _ROTATE {
   RtFloat  angle;
   RtFloat  dx;
   RtFloat  dy;
   RtFloat  dz;
} ROTATE;


enum ALIASTORENDERMAN70 { 
   AR70_ATTRIBUTE,
   AR70_ATTRIBUTEBEGIN,
   AR70_ATTRIBUTEEND,
   AR70_ARCHIVERECORD,
   AR70_GENERALPOLYGON, /* Used by "# Face". */
   AR70_NUPATCH,
   AR70_POINTSGENERALPOLYGONS, /* Used by "# Polyset". */
   AR70_TEXTURECOORDINATES,
   AR70_LAST 
};
enum ALIASTORENDERMANZUP { 
   ARZUP_ATTRIBUTEBEGIN,
   ARZUP_ATTRIBUTEEND,
   ARZUP_FRAMEBEGIN,
   ARZUP_FRAMEEND,
   ARZUP_GENERALPOLYGON, /* Used by "# Face". */
   ARZUP_LIGHTSOURCE,
   ARZUP_NUPATCH,
   ARZUP_POINTSGENERALPOLYGONS, /* Used by "# Polyset". */
   ARZUP_ROTATE,
   ARZUP_SCALE,
   ARZUP_TRANSLATE,
   ARZUP_TRANSFORMBEGIN,
   ARZUP_TRANSFORMEND,
   ARZUP_WORLDBEGIN,
   ARZUP_WORLDEND,
   ARZUP_LAST
};
enum AMAPI205 { 
   A205_ATTRIBUTE,
   A205_NUPATCH,
   A205_POINTSGENERALPOLYGONS,
   A205_REVERSEORIENTATION,
   A205_LAST
};
enum SKETCH20 { 
   SK20_ATTRIBUTE,
   SK20_DECLARE,
   SK20_LIGHTSOURCE,
   SK20_LAST
};
enum TEXPATHS { 
   TP_MAKEBUMP,
   TP_MAKECUBEFACEENVIRONMENT,
   TP_MAKELATLONGENVIRONMENT,
   TP_MAKESHADOW,
   TP_MAKETEXTURE,
   TP_LAST
};
enum UNITPARAM { 
   UP_ATTRIBUTE,
   UP_ATTRIBUTEBEGIN,
   UP_ATTRIBUTEEND,
   UP_BEGIN,
   UP_END,
   UP_FRAMEBEGIN,
   UP_FRAMEEND,
   UP_NUPATCH,
   UP_TRIMCURVE,
   UP_WORLDBEGIN,
   UP_WORLDEND,
   UP_LAST
};


RtVoid
   ar70ArchiveRecord( RtToken type, char *format, char *s ),
   ar70GeneralPolygonV( RtInt nloops, RtInt nvertices[],
		       RtInt n, RtToken tokens[], RtPointer parms[] ),
   ar70NuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
	   RtFloat umin, RtFloat umax, 
	   RtInt nv, RtInt vorder, RtFloat vknot[],
	   RtFloat vmin, RtFloat vmax,
	   RtInt n, RtToken tokens[], RtPointer parms[] ),
   ar70PointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
			      RtInt nvertices[], RtInt vertices[], 
			      RtInt n, RtToken tokens[], RtPointer parms[] ),
   ar70TextureCoordinates( RtFloat s1, RtFloat t1, 
                         RtFloat s2, RtFloat t2,
                         RtFloat s3, RtFloat t3, 
		         RtFloat s4, RtFloat t4 );

RtVoid
   arzupAttributeBegin( void ),
   arzupAttributeEnd( void ),
   arzupFrameBegin( RtInt frame ), 
   arzupFrameEnd( void ),
   arzupGeneralPolygonV( RtInt nloops, RtInt nvertices[],
			RtInt n, RtToken tokens[], RtPointer parms[] ),
   arzupLightSourceV( RtToken name,
		    RtInt n, RtToken tokens[], RtPointer parms[] ),
   arzupNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
		 RtFloat umin, RtFloat umax, 
		 RtInt nv, RtInt vorder, RtFloat vknot[],
		 RtFloat vmin, RtFloat vmax,
	 	 RtInt n, RtToken tokens[], RtPointer parms[] ),
   arzupPointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
			       RtInt nvertices[], RtInt vertices[], 
			       RtInt n, RtToken tokens[], RtPointer parms[] ),
   arzupRotate( RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz ),
   arzupScale( RtFloat dx, RtFloat dy, RtFloat dz ),
   arzupTranslate( RtFloat dx, RtFloat dy, RtFloat dz ),
   arzupTransformBegin( void ),
   arzupTransformEnd( void ),
   arzupWorldBegin( void ), 
   arzupWorldEnd( void );


RtVoid
   a205AttributeV( RtToken name,
		  RtInt n, RtToken tokens[], RtPointer parms[] ),
   a205NuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
	   RtFloat umin, RtFloat umax, 
	   RtInt nv, RtInt vorder, RtFloat vknot[],
	   RtFloat vmin, RtFloat vmax,
	   RtInt n, RtToken tokens[], RtPointer parms[] ),
   a205PointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
			 RtInt nvertices[], RtInt vertices[], 
			 RtInt n, RtToken tokens[], RtPointer parms[] );

RtVoid
   sk20AttributeV( RtToken name,
		  RtInt n, RtToken tokens[], RtPointer parms[] ),
   sk20Declare( char *name, char *declaration ),
   sk20LightSourceV( RtToken name,
		    RtInt n, RtToken tokens[], RtPointer parms[] );

/* 
 *  TEXPATHS structures, enumerators and functions.
 *
 */
typedef struct _PATHFILENAME {
   int   oldtype;
   int   newtype;
   char  *oldname;
   char  *newname;
} PATHFILENAME;
enum {
   TP_NONE,
   TP_MACPATH,
   TP_UNIXPATH,
   TP_PCPATH,
   TP_PATHFILENAME,
   TP_MACPATHFILENAME,
   TP_UNIXPATHFILENAME,
   TP_PCPATHFILENAME
};
int GetNamingConvention( char *name );
int AddFilePathName( char *oldname, char *newname );
char *ReplacementFilePathName( char *name );
void RemoveSpaces( char *s );
RtVoid
   tpMakeBumpV( char *picturename, char *texturename, 
	       RtToken swrap, RtToken twrap,
	       RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
	       RtInt n, RtToken tokens[], RtPointer parms[] ),
   tpMakeCubeFaceEnvironmentV( char *px, char *nx, char *py, char *ny,
			      char *pz, char *nz, char *texturename,
			      RtFloat fov,
			      RtFilterFunc filterfunc, 
			      RtFloat swidth, RtFloat twidth,
			      RtInt n, RtToken tokens[], RtPointer parms[] ),
   tpMakeLatLongEnvironmentV( char *picturename, char *texturename, 
			     RtFilterFunc filterfunc,
			     RtFloat swidth, RtFloat twidth,
			     RtInt n, RtToken tokens[], RtPointer parms[] ),
   tpMakeShadowV( char *picturename, char *texturename,
		 RtInt n, RtToken tokens[], RtPointer parms[] ),
   tpMakeTextureV( char *picturename, char *texturename, 
		  RtToken swrap, RtToken twrap,
		  RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
		  RtInt n, RtToken tokens[], RtPointer parms[] );

/* 
 *  UNITPARAM functions.
 *
 */
RtVoid
   upNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
	   RtFloat umin, RtFloat umax, 
	   RtInt nv, RtInt vorder, RtFloat vknot[],
	   RtFloat vmin, RtFloat vmax,
	   RtInt n, RtToken tokens[], RtPointer parms[] ),
   upTrimCurve( RtInt nloops, RtInt ncurves[], RtInt order[], RtFloat knot[],
	    RtFloat min[], RtFloat max[], RtInt n[],
	    RtFloat u[], RtFloat v[], RtFloat w[] );

/* 
 *  MacroModel RIB fixer function.
 *
 */
int mmErrorHandler(RIB_HANDLE rib, int code, int severity, PRIB_ERROR error);


int main(int argc, char **argv); 
void PrintHelp( void );
void PrintError( char *file );
void MissingNameAttribute( int ricall );
RtBoolean NegateZCoordinate( int nvertices, 
			    RtInt n, RtToken tokens[], RtPointer parms[] );
void SwapYZCoordinate( int nvertices, 
		      RtInt n, RtToken tokens[], RtPointer parms[] );
int Transpose9x5PwArray( RtInt nu, RtInt nv, 
			RtInt n, RtToken tokens[], RtPointer parms[] );
int PrintRIB( char *input, char* output,
	     RtBoolean binary );

/* 
 * Globals shared among callback functions.
 * 
 */
RIB_HANDLE      rib;
PRIB_HASHTABLE  hashtable = NULL;
PRIB_RIPROC  ar70[AR70_LAST];
PRIB_RIPROC  arzup[ARZUP_LAST];
PRIB_RIPROC  a205[A205_LAST];
PRIB_RIPROC  sk20[SK20_LAST];
PRIB_RIPROC  tp[TP_LAST];
PRIB_RIPROC  up[UP_LAST];

RtBoolean    AliasToRenderman = 0;
RtBoolean    AliasToRenderman70 = RI_FALSE;
RtBoolean    AliasToRenderman75 = RI_FALSE;
RtBoolean    AliasToRenderman80 = RI_FALSE;
RtBoolean    AliasToRenderman81 = RI_FALSE;
RtBoolean    AliasToRenderman82 = RI_FALSE;
RtBoolean    AliasZup = RI_FALSE;
RtBoolean    Amapi205 = RI_FALSE;
RtBoolean    Sketch20 = RI_FALSE;
RtBoolean    texpaths = RI_FALSE;
RtBoolean    unitparam = RI_FALSE;
RtBoolean    macromodel = RI_FALSE;

PATTR        ar70Attr = NULL;
char         *ar70LastName = NULL;

RtBoolean    arzupInWorldBlock = RI_FALSE;

PTRIMSTORE   upTrimStore = NULL;
RtFloat      upUmin = 0.0;
RtFloat      upVmin = 0.0;
RtFloat      upUmax = 0.0;
RtFloat      upVmax = 0.0;

int main(int argc, char **argv)
{
   char            *fout = NULL;
   RtBoolean       binary = RI_FALSE;
   int             i,j;

   
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( (i+1<argc) && argv[i][1]=='o' && argv[i][2]=='\0' )
         {
            i++;
            if ( fout == NULL )
            {
               fout = argv[i];
            }
            else
            {
               printf( "Error: Option -o specified more than once.\n" );
	       goto CommandLineError;
            }
         }
         else if (!strcmp(&argv[i][1],"binary"))
         {
            binary = RI_TRUE;
         }
         else if (!strncmp(&argv[i][1],"AliasToRenderman",16))
         {
	    if (!strcmp(&argv[i][17],"7.0"))
	    {
	       AliasToRenderman70 = RI_TRUE;
	    }
	    else if (!strcmp(&argv[i][17],"7.5"))
	    {
	       AliasToRenderman75 = RI_TRUE;
	    }
	    else if (!strcmp(&argv[i][17],"8.0"))
	    {
	       AliasToRenderman80 = RI_TRUE;
	    }
	    else if (!strcmp(&argv[i][17],"8.1"))
	    {
	       AliasToRenderman81 = RI_TRUE;
	    }
	    else if (!strcmp(&argv[i][17],"8.2"))
	    {
	       AliasToRenderman82 = RI_TRUE;
	    }
	    else
	    {
	       goto CommandLineError;
	    }
	    AliasToRenderman++;;
 	 }
         else if (!strcmp(&argv[i][1],"AliasZup"))
         {
	    AliasZup = RI_TRUE;
         }
         else if (!strcmp(&argv[i][1],"Amapi2.05"))
         {
	    Amapi205 = RI_TRUE;
         }
         else if (!strcmp(&argv[i][1],"Sketch2.0"))
         {
	    Sketch20 = RI_TRUE;
	 }
         else if (!strcmp(&argv[i][1],"texpaths"))
         {
	    if (i+2<argc)
	    {
	       i++;
	       if (AddFilePathName( argv[i], argv[i+1] ) )
	       {
		  goto CommandLineError;
	       }
	       i++;
	       
	       /* Mark option as being used. */
	       texpaths = RI_TRUE;
	    }
	    else
	    {
	       printf("Error:  Missing filename.\n" );
	       goto CommandLineError;
	    }
         }
         else if (!strcmp(&argv[i][1],"unitparam"))
         {
	    unitparam = RI_TRUE;
         }
         else if (!strcmp(&argv[i][1],"macromodel"))
         {
	    macromodel = RI_TRUE;
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

   /* 
    *  Avoid odd combo's of options. 
    *
    */
   if ( AliasToRenderman > 1 )
   {
      printf( "Error:  Don't use different levels of AliasToRenderman fixes "\
	     "together.\n" );
      goto CommandLineError;
   }
   if ( AliasToRenderman && Amapi205 )
   {
      printf( "Error:  Don't use AliasToRenderman fixes with Amapi fixes." );
      goto CommandLineError;
   }
   if ( (macromodel && AliasToRenderman) || (macromodel && Amapi205) )
   {
      printf( "Error:  Don't mix fixes for different packages." );
      goto CommandLineError;
   }

   /* Check that none of the input file names matches the output file name.
    *
    */
   if ( i < argc && fout )
   {
      j = i;
      while ( j < argc )
      {
	 if (!strcmp(argv[j],fout))
	 {
	    fprintf( stderr, 
		    "Output filename %s matches an input filename.\n",
		    argv[j] );
	    goto CommandLineError;
	 }
	 j++;
      }
   }

   /* Hook out functions from the table gRibRITable.  First one hooking a 
    * a function is the last one called.  
    */

   if ( unitparam )
   {
      up[UP_NUPATCH] = gRibRITable[kRIB_NUPATCH];
      up[UP_TRIMCURVE] = gRibRITable[kRIB_TRIMCURVE];
      gRibRITable[kRIB_NUPATCH] = (PRIB_RIPROC)upNuPatchV;
      gRibRITable[kRIB_TRIMCURVE] = (PRIB_RIPROC)upTrimCurve;

      upTrimStore = TrimStoreInitRITable( gRibRITable, 0 );
      if (!upTrimStore)
      {
	 return 0; /* Error */
      }
   }

   if ( texpaths )
   {
      tp[TP_MAKEBUMP] = gRibRITable[kRIB_MAKEBUMP];
      tp[TP_MAKECUBEFACEENVIRONMENT] 
	= gRibRITable[kRIB_MAKECUBEFACEENVIRONMENT];
      tp[TP_MAKELATLONGENVIRONMENT] 
	= gRibRITable[kRIB_MAKELATLONGENVIRONMENT];
      tp[TP_MAKESHADOW] = gRibRITable[kRIB_MAKESHADOW];
      tp[TP_MAKETEXTURE] = gRibRITable[kRIB_MAKETEXTURE];
      gRibRITable[kRIB_MAKEBUMP] = (PRIB_RIPROC)tpMakeBumpV;
      gRibRITable[kRIB_MAKECUBEFACEENVIRONMENT] 
	= (PRIB_RIPROC)tpMakeCubeFaceEnvironmentV;
      gRibRITable[kRIB_MAKELATLONGENVIRONMENT] 
	= (PRIB_RIPROC)tpMakeLatLongEnvironmentV;
      gRibRITable[kRIB_MAKESHADOW] = (PRIB_RIPROC)tpMakeShadowV;
      gRibRITable[kRIB_MAKETEXTURE] = (PRIB_RIPROC)tpMakeTextureV;
   }

   if ( Sketch20 )
   {
      sk20[SK20_ATTRIBUTE] = gRibRITable[kRIB_ATTRIBUTE];
      sk20[SK20_DECLARE] = gRibRITable[kRIB_DECLARE];
      sk20[SK20_LIGHTSOURCE] = gRibRITable[kRIB_LIGHTSOURCE];
      gRibRITable[kRIB_ATTRIBUTE] = (PRIB_RIPROC)sk20AttributeV;
      gRibRITable[kRIB_DECLARE] = (PRIB_RIPROC)sk20Declare;
      gRibRITable[kRIB_LIGHTSOURCE] = (PRIB_RIPROC)sk20LightSourceV;
   }

   if ( Amapi205 )
   {
      /* Hook out Attribute, NuPatch and PointsGeneralPolygons. */
      a205[A205_ATTRIBUTE] = gRibRITable[kRIB_ATTRIBUTE];
      a205[A205_NUPATCH] = gRibRITable[kRIB_NUPATCH];
      a205[A205_POINTSGENERALPOLYGONS]=gRibRITable[kRIB_POINTSGENERALPOLYGONS];
      gRibRITable[kRIB_ATTRIBUTE] = (PRIB_RIPROC)a205AttributeV;
      gRibRITable[kRIB_NUPATCH] = (PRIB_RIPROC)a205NuPatchV;
      gRibRITable[kRIB_POINTSGENERALPOLYGONS] = 
                                       (PRIB_RIPROC)a205PointsGeneralPolygonsV;

      /* Need to call kRIB_REVERSEORIENTATION, so store pointer to function. */
      a205[A205_REVERSEORIENTATION] = gRibRITable[kRIB_REVERSEORIENTATION];
   }
   if ( AliasZup )
   {
      arzup[ARZUP_ATTRIBUTEBEGIN] = gRibRITable[kRIB_ATTRIBUTEBEGIN];
      arzup[ARZUP_ATTRIBUTEEND] = gRibRITable[kRIB_ATTRIBUTEEND];
      arzup[ARZUP_FRAMEBEGIN] = gRibRITable[kRIB_FRAMEBEGIN];
      arzup[ARZUP_FRAMEEND] = gRibRITable[kRIB_FRAMEEND];
      arzup[ARZUP_GENERALPOLYGON] = gRibRITable[kRIB_GENERALPOLYGON];
      arzup[ARZUP_LIGHTSOURCE] = gRibRITable[kRIB_LIGHTSOURCE];
      arzup[ARZUP_NUPATCH] = gRibRITable[kRIB_NUPATCH];
      arzup[ARZUP_POINTSGENERALPOLYGONS] 
	= gRibRITable[kRIB_POINTSGENERALPOLYGONS];
      arzup[ARZUP_ROTATE] = gRibRITable[kRIB_ROTATE];
      arzup[ARZUP_SCALE] = gRibRITable[kRIB_SCALE];
      arzup[ARZUP_TRANSLATE] = gRibRITable[kRIB_TRANSLATE];
      arzup[ARZUP_TRANSFORMBEGIN] = gRibRITable[kRIB_TRANSFORMBEGIN];
      arzup[ARZUP_TRANSFORMEND] = gRibRITable[kRIB_TRANSFORMEND];
      arzup[ARZUP_WORLDBEGIN] = gRibRITable[kRIB_WORLDBEGIN];
      arzup[ARZUP_WORLDEND] = gRibRITable[kRIB_WORLDEND];

      gRibRITable[kRIB_ATTRIBUTEBEGIN] = (PRIB_RIPROC)arzupAttributeBegin;
      gRibRITable[kRIB_ATTRIBUTEEND] = (PRIB_RIPROC)arzupAttributeEnd;
      gRibRITable[kRIB_FRAMEBEGIN] 
	= (PRIB_RIPROC)arzupFrameBegin;      
      gRibRITable[kRIB_FRAMEEND] = (PRIB_RIPROC)arzupFrameEnd;      
      gRibRITable[kRIB_GENERALPOLYGON] = (PRIB_RIPROC)arzupGeneralPolygonV;
      gRibRITable[kRIB_LIGHTSOURCE] = (PRIB_RIPROC)arzupLightSourceV;
      gRibRITable[kRIB_NUPATCH] = (PRIB_RIPROC)arzupNuPatchV;
      gRibRITable[kRIB_POINTSGENERALPOLYGONS] 
	= (PRIB_RIPROC)arzupPointsGeneralPolygonsV;
      gRibRITable[kRIB_ROTATE] = (PRIB_RIPROC)arzupRotate;
      gRibRITable[kRIB_SCALE] = (PRIB_RIPROC)arzupScale;
      gRibRITable[kRIB_TRANSLATE] = (PRIB_RIPROC)arzupTranslate;      
      gRibRITable[kRIB_TRANSFORMBEGIN] 
	= (PRIB_RIPROC)arzupTransformBegin;      
      gRibRITable[kRIB_TRANSFORMEND] = (PRIB_RIPROC)arzupTransformEnd;      
      gRibRITable[kRIB_WORLDBEGIN] 
	= (PRIB_RIPROC)arzupWorldBegin;      
      gRibRITable[kRIB_WORLDEND] = (PRIB_RIPROC)arzupWorldEnd;      
   }

   /* AliasToRenderman70 has two additional items beyond the version 7.5 
    *    fixes to worry about. 
    */
   if ( AliasToRenderman82 || AliasToRenderman81 || AliasToRenderman80 
       || AliasToRenderman75 || AliasToRenderman70 )
   {
      /* Store off Attribute calls, for reference later. */
      ar70[AR70_ATTRIBUTE] = gRibRITable[kRIB_ATTRIBUTE];
      ar70[AR70_ATTRIBUTEBEGIN] = gRibRITable[kRIB_ATTRIBUTEBEGIN];
      ar70[AR70_ATTRIBUTEEND] = gRibRITable[kRIB_ATTRIBUTEEND];

      /* Hook out the following calls. */
      ar70[AR70_ARCHIVERECORD] = gRibRITable[kRIB_ARCHIVERECORD];
      ar70[AR70_GENERALPOLYGON] = gRibRITable[kRIB_GENERALPOLYGON];
      ar70[AR70_NUPATCH] = gRibRITable[kRIB_NUPATCH];
      ar70[AR70_POINTSGENERALPOLYGONS] 
	= gRibRITable[kRIB_POINTSGENERALPOLYGONS];
      ar70[AR70_TEXTURECOORDINATES] = gRibRITable[kRIB_TEXTURECOORDINATES];

      gRibRITable[kRIB_ARCHIVERECORD] = (PRIB_RIPROC)ar70ArchiveRecord;
      gRibRITable[kRIB_GENERALPOLYGON] = (PRIB_RIPROC)ar70GeneralPolygonV;
      gRibRITable[kRIB_NUPATCH] = (PRIB_RIPROC)ar70NuPatchV;
      gRibRITable[kRIB_POINTSGENERALPOLYGONS] 
	= (PRIB_RIPROC)ar70PointsGeneralPolygonsV;
      gRibRITable[kRIB_TEXTURECOORDINATES]=(PRIB_RIPROC)ar70TextureCoordinates;

      ar70Attr = AttrInitRITable( gRibRITable, 0 );
      if (!ar70Attr)
      {
	 return 0; /* Error */
      }
   }   

   if ( i < argc )
   {
      while ( i < argc )
      {
         PrintRIB( argv[i], fout, binary );
         i++;
      }     
   }
   else
   {
      PrintRIB( NULL, fout, binary );
   }

   if (upTrimStore)
   {
      TrimStoreDestroy( upTrimStore );
   }
   if (ar70Attr)
   {
      AttrDestroy(ar70Attr);
   }
   if (hashtable)
   {
      RibDestroyHashTable( hashtable );
   }
   
   return 0;
   
 CommandLineError:
   PrintHelp();
   return 1;
}


void PrintHelp( void )
{
  fprintf( stderr, 
"ribfixer [-o file] [-binary] [-<fixname>. . .] [filename . . .]\n\n"      \
" [-o file]                 Output file name.  If not given, stdout is\n"  \
"                           used.\n"                                       \
" [-binary]                 Print RIB in encoded format.\n"                \
" [-<fixname>. . .]         See below for list of fixes.\n"                \
" [filename . . .]          If no file names are given then ribfixer\n"    \
"                           will use standard input.\n\n" );
  fprintf( stderr, 
" RIB Fixes:\n"                                                             \
" Several \"fixes\" and touch-ups are provided with ribfixer.  Please note,\n"\
" I would like to avoid having ribfixer ending up with a reputation of\n"\
" being a RIB breaker.  If you are not seeing the specific problems listed\n"\
" below, do not use this program.  And do not use it on a RIB file more\n"  \
" than once.  Although some fixes could be run n times without problems,\n" \
" many fixes can not check if the fix was already made and therefore\n"     \
" ruins a RIB file while applying the fix more than once.\n\n" );
  fprintf( stderr,
" The following fixes are available with corresponding names for\n"          \
" [-<fixname>. . .] and are applied in the order they are listed below:\n\n" );
  fprintf( stderr,
"   [-AliasToRenderman7.0]  Version 7.0.1 of Alias's AliasToRenderman\n"    \
"                           utility had a problem with NuPatch's min/max\n" \
"                           values of periodic surfaces.  The min and max\n"\
"                           were simply assigned the minimum and maximum\n" \
"                           values of their associated knot vector.  The\n" \
"                           valid range for choosing min/max values is\n"\
"                           from the (order-1)th to nth knot values.\n"  \
"                           The range is altered to be valid.\n\n" );
  fprintf( stderr,
"                           Add Attribute \"identifier\" \"name\" [name]\n" \
"                           for the comments of the form \"# Face name\",\n"\
"                           \"# Polyset name\" and \"# Surface name\".\n\n" );
  fprintf( stderr,
"                           Calls to \"TextureCoordinates 0 0 0 -1 1 0\n" \
"                           1 -1\" are removed.  \"#Scene\" is\n"\
"                           replaced with \"##Scene\".\n\n" );
  fprintf( stderr,
"                           GeneralPolygon is used incorrectly for faces\n"   \
"                           made of multiple curves forming separate faces.\n"\
"                           It appears that the loops following the first\n"  \
"                           where expected to produce polygons not holes.\n"  \
"                           The result is that only one face is seen if\n"    \
"                           the \"Set planar\"(with faces set) is used with\n"\
"                           more than one curve at a time.  Fix by using\n"   \
"                           multiple GeneralPolygon statements.\n\n" );
  fprintf( stderr,
"                           GeneralPolygon is also given concave polygons\n" \
"                           described in the first loop.  The RenderMan\n"   \
"                           standard requires that the first loop outline\n" \
"                           the boundaries.\n\n" );
  fprintf( stderr,
"   [-AliasToRenderman7.5   Implements all the fixes mentioned above except\n"\
"                           the min/max fix, missing Surface and Polyset\n"   \
"                           name attribute fix (# Face is still missing)\n"   \
"                           which were fixed in the 7.5 releases.\n\n" );
  fprintf( stderr,
"                           Also fix the ##Creator version number.\n\n" );
  fprintf( stderr,
"   [-AliasToRenderman8.0   Implements all the fixes for 7.5.\n\n" );
  fprintf( stderr,
"   [-AliasToRenderman8.1   Implements all the fixes for 7.5.\n\n" );
  fprintf( stderr,
"   [-AliasToRenderman8.2   Implements all the fixes for 7.5.\n"            \
"                           Note:  As of 12-14-97 I haven't updated to\n"   \
"                           8.2 yet, but I have tried a few 8.2 Designer\n" \
"                           RIB files.  For now just check your RIB files\n"\
"                           at first when using this flag.\n\n" );
  fprintf( stderr,
"   [-AliasZUp]             If modeling in Alias with Z as being up,\n"      \
"                           problems may appear with how the Z-axis is set\n"\
"                           \"up\".  The Z values will increase in height \n"\
"                           as Z coordinates go in the negative direction.\n"\
"                           To fix the orientation, a new camera angle is\n" \
"                           calculated, Y and Z coordinates in positional\n" \
"                           data are swapped while the Z value is also\n"    \
"                           negated.  Rotate, Scale and Translate are also\n"\
"                           altered to compensate.\n" );
  fprintf( stderr,
"                           If Y is set to being up, the RIB geometry is\n"   \
"                           correct, although the modeling windows label\n"   \
"                           the Z-axis as being negative pointing away from\n"\
"                           the camera's point of view\n\n" );
  fprintf( stderr,
"   [-Amapi2.05]            Amapi 2.05 outputs a mirror image of a scene.\n"\
"                           Also NURB cubes have normals pointing in.\n"    \
"                           The fix is done by negating the Z coordinate\n" \
"                           adding a ReverseOrientation statement before\n" \
"                           the geometry statements.\n\n" );
  fprintf( stderr,
"                           Note:  Don't export to RIB with anything grouped\n"\
"                           together.  Amapi 2.05 turns a set of grouped\n"    \
"                           NURBS and polygons into one huge PointsGeneral-\n" \
"                           Polygons call.\n\n" );
  fprintf( stderr,
"                           Also fix problem of 0x7f characters appearing\n"  \
"                           as the first characters in the name attribute\n"  \
"                           strings. (Amapi 2.05 Mac)\n\n" );
  fprintf( stderr,
"   [-Sketch2.0]            Sketch! 2.02 outputs RIBs that have an added\n" \
"                           space in front of the user given names\n"       \
"                           shown in Attribute \"identifier\" statements.\n"\
"                           Spotlight shaders are given an unknown\n"       \
"                           parameter called \"falloff\".  A Declare is\n"  \
"                           given for the \"falloff\".  For now, just\n"    \
"                           simply remove \"falloff\" from these calls.\n\n" );
  fprintf( stderr,
"                           Sketch! 2.02 will also include extra Make-\n"   \
"                           Texture statements for texture maps not even\n" \
"                           used.  For now just leave them in the RIB.\n\n" );
  fprintf( stderr,
"   [-texpaths old new]...  Sketch! uses the pathnames from the system\n"    \
"                           the model was created on.  When rendering on\n"  \
"                           other systems the pathnames will probably not\n" \
"                           work and need to be replaced.  Useful for\n"     \
"                           replacing Mac colon pathnames with path using\n" \
"                           Unix slashes.  End each path with a ':' or '/'\n"\
"                           or '\\'(Wintel,OS//2) so ribfixer knows which\n" \
"                           system to use.  This option can be used more\n"  \
"                           than once.\n\n" ); 
  fprintf( stderr,
"                           If no slash or colon ends the string old, then\n" \
"                           ribfixer will assume it is a full file pathname\n"\
"                           and will replace the entire string old with the\n"\
"                           string new.\n\n" );
  fprintf( stderr,
"                           Note:  Right now -texpaths only fixes Make...\n"  \
"                           statements since it's hard to even find\n"        \
"                           modeler that even does that.  A full fix would\n" \
"                           require not only all the Make... statements,\n"   \
"                           but also the parameter data given to shaders."    \
                            "\n\n" );

/*                          Inshort a fully automated fixer would have to read
 *                          the shaders (.sl, .so or .slo) and determine how a
 *                          variable declared of type string is used.  If it 
 *                          is in a bump(), texture() or shadow() call then 
 *                          it's safe to assume that the string is a file's
 *                          path name.  For now ribfixer fixes Sketch! files.
 */
  fprintf( stderr,
"   [-unitparam]            It is possible to change the range of a knot\n" \
"                           vector's values and not change the shape of\n"  \
"                           the NURB geometry, just the min/max ranges.\n"  \
"                           If you alter the knot vectors and change the\n" \
"                           u,v ranges that trim curves are written to,\n"  \
"                           a NURB surface can be reparametrized for any\n" \
"                           min/max range.  A nice parametrization is 0\n"  \
"                           to 1 which allows simple shaders such as the\n" \
"                           paintedplastic shader to be used.\n\n" );
  fprintf( stderr,
"                           Note:  For now ribfixer assumes that a NuPatch\n" \
"                                  call and all its trimcurves are together\n"\
"                                  in the same Attribute block and there is\n"\
"                                  only one NuPatch in the Attribute block.\n"\
"                                  Most modelers do it this way, so this\n"   \
"                                  restriction shouldn't be very limiting"    \
                                   "\n\n");
  fprintf( stderr,
"   [-macromodel]  MacroMedia's MacroModel software created RIBs with some\n"\
"                  RIB statements starting with a lower case letter.  The\n" \
"                  Spec has the statements starting with capital letters,\n" \
"                  but Pixar's programs do take RIB statements starting\n"   \
"                  with lower case letters, if the statement's name does\n"  \
"                  not have any capital letters in the middle of the name\n" \
"                  listed in the spec.  (That's my latest theory anyway.)\n"\
"                  BMRT follows the RenderMan specification and does not\n" \
"                  like MacroModel's RIBs.\n"                                \
"                  To fix this problem simply run Pixar's catrib or this\n"  \
"                  option with ribfixer, both take lower/upper case\n"       \
"                  in the way mentioned above, but then they create output\n"\
"                  that matches the RI Spec.\n" );
}


void PrintError( char *file )
{
   fprintf( stderr, "Error:  Couldn't open file: %s\n", file );
}


RtBoolean NegateZCoordinate( int nvertices, 
			    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   RtBoolean     allzeros = RI_TRUE;
   RtInt         t;
   auto RtInt    i;
   auto RtFloat  *p;
   

   for ( t=0; t<n; t++ )
   {
      if ( (tokens[t][0]=='P' || tokens[t][0]=='N') && tokens[t][1]=='\0' )
      {
	 p = (RtFloat*)parms[t];
	 p += 2;
	 for ( i=0; i<nvertices; i++ )
	 {
	    if (*p!=0.0)
	    {
	       allzeros = RI_FALSE;
	       break;
	    }
	    *p = - (*p);
	    p += 3;
	 }
	 if ( !allzeros )
	 {
	    while (i<nvertices)
	    {
	       *p = - (*p);
	       p += 3;
	       i++;
	    }
	 }
      }
      else if (tokens[t][0]=='P' && tokens[t][1]=='w' && tokens[t][2]=='\0')
      {
	 p = (RtFloat*)parms[t];
	 p += 2;
	 for ( i=0; i<nvertices; i++ )
	 {
	    if (*p!=0.0)
	    {
	       allzeros = RI_FALSE;
	       break;
	    }
	    *p = - (*p);
	    p += 4;
	 }
	 if ( !allzeros )
	 {
	    while (i<nvertices)
	    {
	       *p = - (*p);
	       p += 4;
	       i++;
	    }
	 }
      }
   }

   return allzeros;
}


void SwapYZCoordinate( int nvertices, 
		      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   RtInt         t;
   auto RtInt    i;
   auto RtFloat  *p;
   auto RtFloat  f;
   

   for ( t=0; t<n; t++ )
   {
      if ( (tokens[t][0]=='P' || tokens[t][0]=='N') && tokens[t][1]=='\0' )
      {
	 p = (RtFloat*)parms[t];
	 for ( i=0; i<nvertices; i++ )
	 {
	    f = p[1];
	    p[1] = p[2];
	    p[2] = f;
	    p += 3;
	 }
      }
      else if (tokens[t][0]=='P' && tokens[t][1]=='w' && tokens[t][2]=='\0')
      {
	 p = (RtFloat*)parms[t];
	 for ( i=0; i<nvertices; i++ )
	 {
	    f = p[1];
	    p[1] = p[2];
	    p[2] = f;
	    p += 4;
	 }
      }
   }
}


int Transpose9x5PwArray( RtInt nu, RtInt nv, 
		     RtInt n, RtToken tokens[], RtPointer parms[] )
{
   HPOINT        h;
   RtInt         t;
   auto HPOINT   *p;
   (void)nu;
   (void)nv;

   for ( t=0; t<n; t++ )
   {
      if (tokens[t][0]=='P' && tokens[t][1]=='w' && tokens[t][2]=='\0')
      {
	 p = (HPOINT*)parms[t];
	 h=p[1];  p[1]=p[5];   p[5]=p[25];  p[25]=p[37]; p[37]=p[9];  p[9]=h;
	 h=p[2];  p[2]=p[10];  p[10]=p[6];  p[6]=p[30];  p[30]=p[18]; p[18]=h;
	 h=p[3];  p[3]=p[15];  p[15]=p[31]; p[31]=p[23]; p[23]=p[27]; p[27]=h;
	 h=p[4];  p[4]=p[20];  p[20]=p[12]; p[12]=p[16]; p[16]=p[36]; p[36]=h;
	 h=p[7];  p[7]=p[35];  p[35]=p[43]; p[43]=p[39]; p[39]=p[19]; p[19]=h;
	 h=p[8];  p[8]=p[40];  p[40]=p[24]; p[24]=p[32]; p[32]=p[28]; p[28]=h;
	 h=p[13]; p[13]=p[21]; p[21]=p[17]; p[17]=p[41]; p[41]=p[29]; p[29]=h;
	 h=p[14]; p[14]=p[26]; p[26]=p[42]; p[42]=p[34]; p[34]=p[38]; p[38]=h;
      }
   }

   return 0;
}


int PrintRIB( char *input, char* output, 
	     RtBoolean binary )
{

   if (hashtable)
   {
      rib = RibOpenWithHashTable( input, hashtable );
      RibSetRITable( rib, kRIB_LAST_RI, gRibRITable );
   }
   else
   {
      rib = RibOpen( input, kRIB_LAST_RI, gRibRITable );
   }
   if (!rib)
   {
      PrintError( input );
      return 1;
   }
   if (macromodel)
   {
      RibSetErrorHandler( rib, mmErrorHandler );
   }
   if (AliasToRenderman)
   {
      AttrUseRIB( ar70Attr, rib );
   }
   if (unitparam)
   {
      TrimStoreUseRIB( upTrimStore, rib );
   }
   if ( output )
   {
      if (RibBegin( rib, output ))
      {
	 PrintError( output );
	 return 1;	
      }
   }
   if (binary)
   {
      RibOption( rib, "rib", "format", "binary" );
   }
   if (RibRead(rib))
   {
      RibClose(rib);
      return 1;
   }
   RibClose(rib);

   return kRIB_OK;
}


/*
 *
 * AliasToRenderman 7.0, 7.5, 8.0 and 8.1 fixes and touch-ups follow.
 *
 */


/* MissingNameAttribute()
 *
 * AliasToRenderman precedes GeneralPolygon, NuPatch, 
 * PointsGeneralPolygons statements with one of the following
 * comments specifying a name for the geometry:
 *
 *               # Face "name"
 *               # Polyset "name"
 *               # Surface "name"
 *
 * The mentioned name should be placed in an identifier attribute.
 *
 * AliasToRenderman 7.0 didn't set any attributes.  Version 7.5
 *   set up attributes for Polyset and Surface but not Face.
 */        
void MissingNameAttribute( int ricall )
{
   static RtToken  token[1] = { "name" };
   RtPointer  param[1];
   auto char  *s;


   /* To make this more bullet proof, check if the present name attribute
    *    matches that in the comment.  If there is a match then skip adding
    *    an attribute.
    */
   if (ar70LastName)
   {
      s = (char*)AttrQueryAttribute( ar70Attr, "identifier", "name" );
      if ( !s || (s && strcmp( s, ar70LastName )) )
      {
	 /* No name attribute present, so add one. */
	 if ( kRIB_GENERALPOLYGON == ricall )
	 {
	    /* GeneralPolygonV is used for Faces and Faces are missing
             *    surrounding AttributeBegin/End blocks.
	     *
             * NOTE:  The function ar70GeneralPolygonV() outputs the matching 
             *        AttributeEnd call.
             */
	    ((PRiAttributeBegin)*ar70[AR70_ATTRIBUTEBEGIN])();
	 }
	 param[0] = (RtPointer)&ar70LastName;
	 ((PRiAttributeV)*ar70[AR70_ATTRIBUTE])( "identifier",
                             1, token, param );
      }
   }

   return;
}


/* AliasToRenderman (7.x, 8.0 and 8.1) add the following hint to the header:
 *
 *              #Scene <scene_filepathname_with_no_quotes>
 *
 * For the above comment to be a valid Scene hint, an extra '#' is needed
 *    to make the statement appear as:
 *
 *              ##Scene <scene_filepathname_with_no_quotes>
 *
 * Note:  ##Scene doesn't require the name to be a full file pathname of the
 *        the RIB being created.  That's just what AliasToRenderman chooses
 *        to use as a name.  The spec gives an example name in the line 
 *        "##Scene Bouncing Ball".  
 */
RtVoid ar70ArchiveRecord( RtToken type, char *format, char *s )
{
   auto char    *p = s;
   auto size_t  l;

   if ( !strcmp(format,"%s") )
   {
      if ( type==RI_COMMENT || !strcmp(type,RI_COMMENT) )
      {
	 if (!strncmp(s, "Scene ", 6) )
	 {
	    type = RI_STRUCTURE;
	 }
	 else if ( !strncmp(s, " Face \"", 7) )
	 {
	    if (ar70LastName)
	      _RibFree(ar70LastName);
	    p += 7;
	    l = strlen(p);
	    /* Don't add one for '\0', overwrite closing quote instead. */
	    ar70LastName = (char*)_RibMalloc( l );
	    l--;
	    strncpy( ar70LastName, p, l );
	    ar70LastName[l] = '\0';
	 }
	 else if ( !strncmp(s, " Polyset \"", 10) )
	 {
	    if (ar70LastName)
	      _RibFree(ar70LastName);
	    p += 10;
	    l = strlen(p);
	    /* Don't add one for '\0', overwrite closing quote instead. */
	    ar70LastName = (char*)_RibMalloc( l );
	    l--;
	    strncpy( ar70LastName, p, l );
	    ar70LastName[l] = '\0';
	 }
	 else if ( !strncmp(s, " Surface \"", 10) )
	 {
	    if (ar70LastName)
	      _RibFree(ar70LastName);
	    p += 10;
	    l = strlen(p);
	    /* Don't add one for '\0', overwrite closing quote instead. */
	    ar70LastName = (char*)_RibMalloc( l );
	    l--;
	    strncpy( ar70LastName, p, l );
	    ar70LastName[l] = '\0';
	 }
      }
      /* While I'm here and since this function is called if any of the
       *    AliasToRenderman flags are set (versions 7.0, 7.5, 8.0 and 8.1),
       *    fix the version number.  Alias|Wavefront doesn't seem to 
       *    update their ##Creator hint's version number.
       *
       * Note:  This is a minor nit thing to fix -- just so I can keep test
       *        files straight.
       */
      else if (type==RI_STRUCTURE || !strcmp(type,RI_STRUCTURE) )
      {
	 if ( !strcmp(p, "Creator AliasToRenderman 7.0") )
	 {
	    if (AliasToRenderman82)
	    {
	       /* Change "##Creator AliasToRenderman 7.0" to be 
		*    "##Creator AliasToRenderman 8.2".
		*/
	       p[25] = '8'; p[27] = '2';
	    }
	    else if (AliasToRenderman81)
	    {
	       /* Change "##Creator AliasToRenderman 7.0" to be 
		*    "##Creator AliasToRenderman 8.1".
		*/
	       p[25] = '8'; p[27] = '1';
	    }
	    else if (AliasToRenderman80)
	    {
	       /* Change "##Creator AliasToRenderman 7.0" to be 
		*    "##Creator AliasToRenderman 8.0".
		*/
	       p[25] = '8';
	    }
	    else if (AliasToRenderman75)
	    {
	       /* Change "##Creator AliasToRenderman 7.0" to be 
		*    "##Creator AliasToRenderman 7.5".
		*/
	       p[27] = '5';
	    }
	    /* AliasToRenderman70 is already set correctly. */
	 }
      }
   }

   ((PRiArchiveRecord)*ar70[AR70_ARCHIVERECORD])( type, s );
}


/* ar70GeneralPolygonV()
 *
 * Check name attribute to see if set properly.
 */
RtVoid ar70GeneralPolygonV( RtInt nloops, RtInt nvertices[],
			   RtInt n, RtToken tokens[], RtPointer parms[] )
{
   /* 
    * The following table was created originally with the following command:
    *
    *            tokentbl -s pst.asc PST
    */
   static char PST[] = {
      3,  3 ,'P','\0',      0,
      0,  4 ,'s','t','\0',  1
   };
   RtPointer     p[2];
   register int  i;
		

   /* Now check for missing name attribute. */
   MissingNameAttribute(kRIB_GENERALPOLYGON);

   /* Break up GeneralPolygon calls with more than one loop.  Alias
    * intended the extra loops to be polygons not holes in the first loop.
    */
   if ( nloops > 1 )
   {
      RibGetUserParameters( PST, 2, n, tokens, parms, p );
      if ( !p[0] )
	return; /* Error in RIB file. */
      if ( !p[1] )
	n = 1;
      else
	n = 2;
      for ( i=0; i< nloops; i++ )
      {
	 ((PRiGeneralPolygonV)*ar70[AR70_GENERALPOLYGON])(
                                     1, &nvertices[i], 
                                     n, tokens, p );
	 p[0] = (RtPointer)(((RtFloat*)p[0]) + 3*nvertices[i]);
	 p[1] = (RtPointer)(((RtFloat*)p[1]) + 2*nvertices[i]);
      }
   }
   else
   {
      ((PRiGeneralPolygonV)*ar70[AR70_GENERALPOLYGON])(
		   nloops, nvertices, n, tokens, parms );
   }

   /* GeneralPolygonV is used for Faces and Faces are missing
    *    surrounding AttributeBegin/End blocks.
    */
   ((PRiAttributeEnd)*ar70[AR70_ATTRIBUTEEND])();
   
   /* To-Do:  Add concave polygon test and loop fixes after more testing. */
}


/* ar70NuPatchV()
 *
 */
RtVoid ar70NuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
		   RtFloat umin, RtFloat umax, 
		   RtInt nv, RtInt vorder, RtFloat vknot[],
		   RtFloat vmin, RtFloat vmax,
		   RtInt n, RtToken tokens[], RtPointer parms[] )
{
   if (AliasToRenderman70)
   {
      /*
       * Fix for periodic surfaces from Power Animator 7.0 Nov/95 
       *    and 7.0.1 Mar/96 versions of AliasToRenderman. 
       *
       * Tested only on degree 3 surface of revolutions and order 3 
       *    surface/skinnned closed curves.  Other order curves from Alias 
       *    Designer were not tested.
       *
       * Check name attribute to see if set properly.
       *
       * The real fix was to update to version 7.5 PowerAnimator.  Not having
       *    a 7.0 version running anymore, this fix is being restricted to what
       *    I needed to fix only the problems that I ran into.  Now that this 
       *    code is being published, it would have been nice to have had more
       *    test cases, but for now I'll try to restrict this fix to 
       *    finding and fixing only NuPatch statements that match the RIB 
       *    output I have examples for.
       */
      
      /* With AliasToRenderman, surface of revolutions will only have either 
       *    the u or the v as periodic.  But the following code shouldn't 
       *    have any problem with an unmodified AliasToRenderman 7.0 RIB file.
       */

      /* Check if the curve is order 4 and periodic. */
      if ( uorder==4 && uknot[0]!=uknot[3] )
      {
	 umin = uknot[uorder-1];
	 umax = uknot[nu];
      }

      /* Check if the curve is order 4 and periodic. */
      if ( vorder==4 && vknot[0]!=vknot[3] )
      {
	 vmin = vknot[vorder-1];
	 vmax = vknot[nv];
      }
   }

   /* Now check for missing name attribute. */
   MissingNameAttribute(kRIB_NUPATCH);

   ((PRiNuPatchV)*ar70[AR70_NUPATCH])(
      nu,uorder,uknot,umin,umax,nv,vorder,vknot,vmin,vmax,n,tokens,parms );
}


/* ar70PointsGeneralPolygonsV()
 *
 * Check name attribute to see if set properly.
 */
RtVoid ar70PointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
				  RtInt nvertices[], RtInt vertices[], 
				  RtInt n, 
				  RtToken tokens[], RtPointer parms[] )
{
   /* Now check for missing name attribute. */
   MissingNameAttribute(kRIB_POINTSGENERALPOLYGONS);

   ((PRiPointsGeneralPolygonsV)*ar70[AR70_POINTSGENERALPOLYGONS])(
               npolys, nloops, nvertices, vertices, n, tokens, parms );
}


/* AliasToRenderman 7.x, 8.0 and 8.1 like adding in the following 
 *   statement before each NuPatch statement: 
 *
 *              TextureCoordinates 0 0 0 -1 1 0 1 -1
 *
 * I never got much out of above call, so simply remove it.
 */
RtVoid ar70TextureCoordinates( RtFloat s1, RtFloat t1, 
                         RtFloat s2, RtFloat t2,
                         RtFloat s3, RtFloat t3, 
		         RtFloat s4, RtFloat t4 )
{
   if (   0.0==s1 &&  0.0==t1 
       && 0.0==s2 && -1.0==t2 
       && 1.0==s3 &&  0.0==t3
       && 1.0==s4 && -1.0==t4 )
   {
      /* Skip it. */
      return;
   }

   /* The TextureCoordinates call was modified or else this option was
    *   mistakenly given to ribfixer.  Just skip the fix and send this
    *   call along its way so that the RIB file's data is untouched.
    */
   ((PRiTextureCoordinates)*ar70[AR70_TEXTURECOORDINATES])( s1,t1,s2,t2,
							   s3,t3,s4,t4 );
}


/*
 *
 * Alias Z-up fixes follow.
 *
 */
RtVoid arzupAttributeBegin( void )
{
   ((PRiAttributeBegin)*arzup[ARZUP_ATTRIBUTEBEGIN])();
}


RtVoid arzupAttributeEnd( void )
{
   ((PRiAttributeEnd)*arzup[ARZUP_ATTRIBUTEEND])();
}


RtVoid arzupFrameBegin( RtInt frame )
{
   ((PRiFrameBegin)*arzup[ARZUP_FRAMEBEGIN])( frame );
}


RtVoid arzupFrameEnd( void )
{
   ((PRiFrameEnd)*arzup[ARZUP_FRAMEEND])();
}


RtVoid arzupGeneralPolygonV( RtInt nloops, RtInt nvertices[],
			RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register int  i;
   register int  v = 0;
		

   for ( i=0; i< nloops; i++ )
   {
      v += nvertices[i];
   }

   NegateZCoordinate( v, n, tokens, parms );
   SwapYZCoordinate( v, n, tokens, parms );
   ((PRiGeneralPolygonV)*arzup[ARZUP_GENERALPOLYGON])( nloops, nvertices,
						      n, tokens, parms );
}


RtVoid arzupLightSourceV( RtToken name,
			 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   /* Put a Rotate 90 1 0 0 in front of each light source. */
   /* Ok the following is a cheap solution.  I'll make it more official
    * when I stick in libribstore.a which allows RIB calls to be stored
    * up in a stack and playedback.  To fix this right I need to store-up
    * some of the transformations until, it is known that they refer to
    * a lightsource or not.
    */
   ((PRiRotate)*arzup[ARZUP_ROTATE])( 90.0, 1.0, 0.0, 0.0 );
   ((PRiLightSourceV)*arzup[ARZUP_LIGHTSOURCE])( name, n, tokens, parms );
}


RtVoid arzupNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
		 RtFloat umin, RtFloat umax, 
		 RtInt nv, RtInt vorder, RtFloat vknot[],
		 RtFloat vmin, RtFloat vmax,
	 	 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   NegateZCoordinate( nu*nv, n, tokens, parms );
   SwapYZCoordinate( nu*nv, n, tokens, parms );
   ((PRiNuPatchV)*arzup[ARZUP_NUPATCH])(
      nu,uorder,uknot,umin,umax,nv,vorder,vknot,vmin,vmax,n,tokens,parms );
}


RtVoid arzupPointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
			       RtInt nvertices[], RtInt vertices[], 
			       RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register int  l,v,i;
		

   l = 0;
   for ( i=0; i< npolys; i++ )
   {
      l += nloops[i];
   }

   v = 0;
   for ( i=0; i< l; i++ )
   {
      v += nvertices[i];
   }

   l = vertices[0];
   for ( i=0; i< v; i++ )
   {
      if ( vertices[i] > l )
	l = vertices[i];
   }

   NegateZCoordinate( l+1, n, tokens, parms );
   SwapYZCoordinate( l+1, n, tokens, parms );
   ((PRiPointsGeneralPolygonsV)*arzup[ARZUP_POINTSGENERALPOLYGONS])( 
        npolys, nloops, nvertices, vertices, n, tokens, parms );
}


RtVoid arzupRotate( RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz )
{
   RtFloat  f;


   if (arzupInWorldBlock)
   {
      f = dz;
      dz = dy;
      dy = f;
      if (dy==1.0)
      {
	 angle = -angle;
      }

      ((PRiRotate)*arzup[ARZUP_ROTATE])( angle, dx, dy, dz );
   }
   else
   {
      /* Assemble the camera information.  Note relies on the format
       *   that Alias uses: three Rotates followed by one Translate.
       */
      ROTATE   *rotate[2];
      RtFloat  *translate;
      int      i,rc;
      

      /* If the format doesn't match just cause the syntax error and
       *   return -- the file probably wasn't an Alias RIB file from
       *   one of the tested levels of PowerAnimator, Studio or Designer.
       *   -- I hope anyway.
       *
       *  Example from an AliasToRenderman RIB:
       *         ....
       *         Projection "perspective" "fov" [40.7]
       *         Rotate -68.0846 0 0 1
       *         Rotate 35.9775 1 0 0
       *         Rotate 55.5946 0 1 0
       *         Translate -294.204 243.568 296.917
       *         WorldBegin 
       *         ....
       */
      for ( i=0; i<2; i++ )
      {
	 rc = RibHandlePrefix( rib );
	 if (EOF==rc)
	   return;
	 if (rc!='R')
	 {
	    RibUngetChar(rib,rc);
	    return;
	 }
	 rc = RibReadFileForToken( rib, gRibRITokenTables['R'-'A'] );
	 if (rc!=kRIB_ROTATE)
	   return;
	 rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 
					4, (RtFloat**)&rotate[i] ); 
	 if (rc)
	   return;
      }
      /* Read translate call. */
      rc = RibHandlePrefix( rib );
      if (EOF==rc)
	return;
      if (rc!='T')
      {
	 RibUngetChar(rib,rc);
	 return;
      }
      rc = RibReadFileForToken( rib, gRibRITokenTables['T'-'A'] );
      if (rc!=kRIB_TRANSLATE)
	return;
      rc = RibReadFloatingPointArray( rib, 
				     kRIB_BRACKETS_OPTIONAL, 3, &translate ); 
      if (rc)
	return;

      /* Print out the new transformations. */
      ((PRiRotate)*arzup[ARZUP_ROTATE])( angle, dx, dy, dz );
      ((PRiRotate)*arzup[ARZUP_ROTATE])( rotate[0]->angle, 
					rotate[0]->dx, 
					rotate[0]->dy, 
					rotate[0]->dz );
      ((PRiRotate)*arzup[ARZUP_ROTATE])( rotate[1]->angle, 
					rotate[1]->dx, 
					rotate[1]->dy, 
					rotate[1]->dz );
      ((PRiTranslate)*arzup[ARZUP_TRANSLATE])( translate[0], 
					      translate[1], 
					      translate[2] );
      ((PRiRotate)*arzup[ARZUP_ROTATE])( -90.0, 1.0, 0.0, 0.0 );
      _RibFree(rotate[0]);
      _RibFree(rotate[1]);
      _RibFree(translate);
   }

}


RtVoid arzupScale( RtFloat dx, RtFloat dy, RtFloat dz )
{
   RtFloat  f;

   if (arzupInWorldBlock)
   {
      f = dz;
      dz = dy;
      dy = f;
   }
   
   ((PRiScale)*arzup[ARZUP_SCALE])( dx, dy, dz );
}


RtVoid arzupTranslate( RtFloat dx, RtFloat dy, RtFloat dz )
{
   RtFloat  f;

   if (arzupInWorldBlock)
   {
      f = -dz;
      dz = dy;
      dy = f;
   }

   ((PRiTranslate)*arzup[ARZUP_TRANSLATE])( dx, dy, dz );
}


RtVoid arzupTransformBegin( void )
{
   ((PRiTransformBegin)*arzup[ARZUP_TRANSFORMBEGIN])();
}


RtVoid arzupTransformEnd( void )
{
   ((PRiTransformEnd)*arzup[ARZUP_TRANSFORMEND])();
}


RtVoid arzupWorldBegin( void )
{
   arzupInWorldBlock = RI_TRUE;

   ((PRiWorldBegin)*arzup[ARZUP_WORLDBEGIN])();
}


RtVoid arzupWorldEnd( void )
{
   arzupInWorldBlock = RI_FALSE;

   ((PRiWorldEnd)*arzup[ARZUP_WORLDEND])();
}


/*
 *
 * Amapi 2.03 and 2.05 fixes follow.
 *
 */


/* a205AttributeV()
 *
 * Remove 0x7f characters (sometimes there are two) that appear at the
 * beginning of some object names.  I've run into this using Amapi version
 * 2.05 (Mac).
 */
RtVoid a205AttributeV( RtToken name,
		      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   char  *s,*t;


   if ( !strcmp(name,"identifier")
       && !strcmp( tokens[0], "name") )
   {
      s = *(char**)parms[0];
      while ( *s==(char)0x7f )
	s++;
      if ( s!=*(char**)parms[0])
      {
	 t = *(char**)parms[0];
	 do
	 {
	    *t++ = *s;
	 } while (*s++);
      }
   }

   /* Now send it off to be printed. */
   ((PRiAttributeV)*a205[A205_ATTRIBUTE])( name, n, tokens, parms );
}


RtVoid a205NuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
	   RtFloat umin, RtFloat umax, 
	   RtInt nv, RtInt vorder, RtFloat vknot[],
	   RtFloat vmin, RtFloat vmax,
	   RtInt n, RtToken tokens[], RtPointer parms[] )
{
   static RtFloat NURBSphereUKnot[] = {
      0.0, 0.0, 0.0, 0.25, 0.25, 0.5, 0.5, 0.75, 0.75, 1.0, 1.0, 1.0
   };
   static RtFloat NURBSphereVKnot[] = {
      0.0, 0.0, 0.0, 0.5, 0.5, 1.0, 1.0, 1.0
   };
   RtBoolean     allzeros;
   auto RtInt    i;
   auto RtFloat  f;
   auto RtFloat  *p;


   /* Amapi 2.03 (Windows NT) and Amapi 2.05 (Mac) create RIB files that
    *    need the equivalent of a "Scale 1 1 -1" infront of the RIB file
    *    to correct the orientation of the geometry.  This is fixed here
    *    by changing the "Pw" data to have negative Z coordinates.
    */
   allzeros = NegateZCoordinate( nu*nv, n, tokens, parms );

   /* A ReverseOrientation statement is needed for Amapi's NURB cubes
    *   and other NURB surfaces except for spheres.  We'll take advantage
    *   of the fact that Amapi's NURB based spheres are the only NURB
    *   surfaces with weights set to something else besides 1.
    */
   /* See if it is a NURB sphere. */
   if (   nu==5 && uorder==3 && umin==0.0 && umax==1.0
       && nv==9 && vorder==3 && vmin==0.0 && vmax==1.0 
       /* Yes, I'm comparing NURBSphereVKnot with uknot, see below for why. */
       && !memcmp(NURBSphereVKnot,uknot, 8*sizeof(RtFloat))
       /* Yes, I'm comparing NURBSphereUKnot with vknot, see below for why. */
       && !memcmp(NURBSphereUKnot,vknot,12*sizeof(RtFloat)) )
   {
      /* So why compare NURBSphereUKnot[] with vknot[] and NURBSphereVKnot[] 
       *    with uknot[]?  
       * The RIB Sphere primitive is parameterized to have the u varying
       *    when going around the sphere and v vary when going up and down
       *    the sphere.  Amapi 2.03 and 2.05 has u varying when going up 
       *    and down and v varying going around.
       * To fix this, swap the u and v data and arrays such as uknot and
       *    vknot.  Note that NURBSphereUKnot[] and NURBSphereVKnot[] are
       *    already set to the normal form for a sphere.
       */
      Transpose9x5PwArray( nu, nv, n, tokens, parms );
      i=nu;     nu=nv;         nv=i;
      i=uorder; uorder=vorder; vorder=i;
      p=uknot;  uknot=vknot;   vknot=p;
      f=umin;   umin=vmin;     vmin=f;
      f=umax;   umax=vmax;     vmax=f;
   }
   else
   {
      /* If all the Z coordinates are zero then skip calling
       * ReverseOrientation. 
       */
      if (!allzeros)
	((PRiReverseOrientation)*a205[A205_REVERSEORIENTATION])();
   }

   /* Now send it off to be printed. */
   ((PRiNuPatchV)*a205[A205_NUPATCH])(
      nu,uorder,uknot,umin,umax,nv,vorder,vknot,vmin,vmax,n,tokens,parms );
}


RtVoid a205PointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
				  RtInt nvertices[], RtInt vertices[], 
				  RtInt n, 
				  RtToken tokens[], RtPointer parms[] )
{
   register int  l,v,i;


   l = 0;
   for ( i=0; i< npolys; i++ )
   {
      l += nloops[i];
   }
   v = 0;
   for ( i=0; i< l; i++ )
   {
      v += nvertices[i];
   }
   l = vertices[0];
   for ( i=0; i< v; i++ )
   {
      if ( vertices[i] > l )
	l = vertices[i];
   }
   /* Total number of vertices is the maximum number in vertices[] +1. */
   l++;

   /* Amapi 2.03 (Windows NT) and Amapi 2.05 (Mac) create RIB files that
    *    need the equivalent of a "Scale 1 1 -1" infront of the RIB file
    *    to correct the orientation of the geometry.  This is fixed here
    *    by changing the "P" and "N" data to have negative Z coordinates.
    */
   NegateZCoordinate( l, n, tokens, parms );

   /* A ReverseOrientation statement is needed for Amapi's 
    *    PointsGeneralPolygons.
    */
   ((PRiReverseOrientation)*a205[A205_REVERSEORIENTATION])();

   /* Now send it off to be printed. */
   ((PRiPointsGeneralPolygonsV)*a205[A205_POINTSGENERALPOLYGONS])( 
                                  npolys, nloops, nvertices, vertices,
                                  n, tokens, parms );
}


/*
 *
 * Sketch! 2.02 fixes follow.
 *
 */

/* sk20AttributeV()
 *
 * Remove the extra space that Sketch! makes precede each identifier 
 * name and shading group.  
 */
RtVoid sk20AttributeV( RtToken name,
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   register char  *s,*t;


   s = *((RtString*)parms[0]);
   if ( !strcmp(name,"identifier")
       && (!strcmp( tokens[0], "name")||!strcmp( tokens[0], "shadinggroup")) 
       && *s==' ' )
   {
      t = s;
      s++;
      do
      {
	 *t++ = *s;
      } while (*s++);
   }

   /* Now send it off to be printed. */
   ((PRiAttributeV)*sk20[SK20_ATTRIBUTE])( name, n, tokens, parms );
}


/* sk20Declare()
 *
 * Remove from the RIB the declaration for "falloff" that Sketch! makes.
 *
 */
RtVoid sk20Declare( char *name, char *declaration )
{
   /* Only pass on the declaration if it is not for falloff. */
   if ( !strcmp(name,"falloff") )
   {
      ((PRiDeclare)*sk20[SK20_DECLARE])( name, declaration );
   }
}


/* sk20LightSourceV()
 *
 * Remove the "falloff" parameter that Sketch! adds to spotlighs.
 *
 */
RtVoid sk20LightSourceV(  RtToken name,
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   /* Sketch! 2.0 always places the "falloff" first. */
   if ( !strcmp(name,"spotlight") && !strcmp(tokens[0],"spotlight") 
       && n && !strcmp(tokens[0],"falloff") )
   {
      /* Cheat here and just change the pointer.  Note that this could
       *    cause problems if one of the subclassed functions being called
       *    does something fancy with the pointers tokens and parms.
       * But ribfixer doesn't have anything else subclassing LightSource,
       *    so there shouldn't be a problem.
       */
      ((PRiLightSourceV)*sk20[SK20_LIGHTSOURCE])( name, 
						 n, &tokens[1], &parms[1] );
   }
   else
   {
      ((PRiLightSourceV)*sk20[SK20_LIGHTSOURCE])( name, n, tokens, parms );
   }
}


/*
 *
 * Texture path change touch-up follows.
 *
 */
int GetNamingConvention( char *name )
{
   int   l;
   char  lastchar;
   char  *p;


   /* Look at the last character to distinguish between naming conventions. */
   l = (int)strlen(name);
   lastchar = (l ? name[l-1] : '\0');
   switch (lastchar)
   {
    case ':':
      return TP_MACPATH;
    case '/':
      return TP_UNIXPATH;
    case '\\':
      return TP_PCPATH;
    default:
      p = NULL;
      while (l>0)
      {
	 l--;
	 while ( l>=0 && name[l] != ':' && name[l] != '\\' && name[l] != '/' )
	 {
	   l--;
	 }
	 if ( l >= 0 )
	 {
	    p = &name[l];
	    break;
	 }
	 l--;
      }
      if (p)
      {
	 switch (*p)
	 {
	  case ':':
	    return TP_MACPATHFILENAME;
	  case '\\':
	    return TP_PCPATHFILENAME;
	 }
      }
   }

   return TP_UNIXPATHFILENAME;
}

int AddFilePathName( char *oldname, char *newname )
{
   int                hashvalue;
   auto PATHFILENAME  *pfn;
   

   pfn = (PATHFILENAME*)_RibMalloc( sizeof(PATHFILENAME) );
   if (!pfn)
     return 1;

   pfn->oldtype = GetNamingConvention(oldname);
   pfn->newtype = GetNamingConvention(newname);

   /* If replacing a path with a filename return an error. */
   if ( pfn->oldtype < TP_PATHFILENAME && pfn->newtype > TP_PATHFILENAME )
     return 1;
   /* If replacing a filename with a path return an error. */
   if ( pfn->oldtype > TP_PATHFILENAME && pfn->newtype < TP_PATHFILENAME )
     return 1;

   /* The values oldname and newname are from argv[], so don't worry
    *    worry about freeing this data. 
    */
   pfn->oldname = oldname; /* Don't worry about freeing this data. */
   pfn->newname = newname; /* Don't worry about freeing this data. */

   /* Add to hash table and mark the pointer pfn as being freed when
    *    hashtable is destroyed.
    */
   if (!hashtable)
   {
      hashtable = RibCreateHashTable( kRIB_HASH_TABLESIZE );
   }
   hashvalue = RibHashValueFromString( hashtable, oldname );
   if (!RibAddItem( hashtable, kRIB_HASH_USER/* & kRIB_HASH_FREEEXTDATA*/, 
	      hashvalue, NULL, (void*)pfn ))
   {
      return 1;
   }

   return 0;
}


char *ReplacementFilePathName( char *name )
{
   PRIB_HASHATOM      patom;
   auto int           hashvalue;
   auto int           l;
   auto PATHFILENAME  *pfn;
   char               *s = NULL;
   char               *p,*pp;
   char               olddelimitor='\n';
   char               newdelimitor='\n';


   hashvalue = RibHashValueFromString( RibGetHashHandle(rib), name );
   patom = RibFindMatch( RibGetHashHandle(rib), kRIB_HASH_USER, 
			hashvalue, NULL );
   if ( !patom )
   {
      pp = NULL;
      p = name;
      while (*p)
      {
	 while ( *p && *p != ':' && *p != '\\' && *p != '/' )
	 {
	    p++;
	 }
	 if (*p)
	 {
	    pp = p++;
	 }
      }
      if (pp)
      {
	 l = (int)(pp - name + 1);
	 hashvalue = RibHashValueFromStringN(RibGetHashHandle(rib), name, l );
	 patom = RibFindMatch( RibGetHashHandle(rib), kRIB_HASH_USER, 
			      hashvalue, NULL );
	 if ( !patom )
	   return NULL;     
      }
      else
      {
	 return NULL;
      }
   }

   /* Get the description of what to change. */
   pfn = (PATHFILENAME*)patom->with.extdata;
   switch (pfn->newtype)
   {
    case TP_MACPATHFILENAME:
    case TP_UNIXPATHFILENAME:
    case TP_PCPATHFILENAME:
      /* Replace entire name if match. */
      if (!strcmp( pfn->oldname, name ))
      {
	 s = (char*)_RibMalloc( strlen(pfn->newname)+1 );
	 strcpy( s, pfn->newname );
      }
      break;
    default:
      /* Use new path if old paths match. */
      l = (int)strlen(pfn->oldname);
      if (!strncmp( pfn->oldname, name, l ))
      {
	 s = (char*)_RibMalloc( strlen(pfn->newname)+(strlen(name)-l)+1);
	 p = s + strlen(pfn->newname);
	 strcpy( s, pfn->newname );
	 strcpy( p, name+l );
      }
   }

   /* If naming conventions are not the same, go through and
    *    replace the directory separators.
    */
   if ( pfn->oldtype != pfn->newtype )
   {
      switch (pfn->oldtype)
      {
       case TP_MACPATH:
       case TP_MACPATHFILENAME:
	 olddelimitor = ':';
	 break;
       case TP_UNIXPATH:
       case TP_UNIXPATHFILENAME:
	 olddelimitor = '/';
	 break;
       case TP_PCPATH:
       case TP_PCPATHFILENAME:
	 olddelimitor = '\\';
	 break;
      }
      switch (pfn->newtype)
      {
       case TP_MACPATH:
       case TP_MACPATHFILENAME:
	 newdelimitor = ':';
	 break;
       case TP_UNIXPATH:
       case TP_UNIXPATHFILENAME:
	 newdelimitor = '/';
	 break;
       case TP_PCPATH:
       case TP_PCPATHFILENAME:
	 newdelimitor = '\\';
	 break;
      }
      p = s;
      
      while (*p)
      {
	 if (*p==olddelimitor)
	   *p=newdelimitor;
	 /* Remove spaces. */
	 if (*p==' ')
	   *p='_';
	 p++;
      }
   }
   else
   {
      /* Remove spaces. */
      p = s;
      while (*p)
      {
	 if (*p==' ')
	   *p='_';
	 p++;
      }
   }
   
   return s;
}


void RemoveSpaces( char *s )
{
   auto char  *p;


   p = s;
   while (*p)
   {
      if (*p==' ')
	*p='_';
      p++;
   }
}


RtVoid tpMakeBumpV( char *picturename, char *texturename, 
	       RtToken swrap, RtToken twrap,
	       RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
	       RtInt n, RtToken tokens[], RtPointer parms[] )
{
   auto char  *s1, *s2;


   s1 = ReplacementFilePathName( picturename );
   if (s1)
   {
      picturename = s1;
   }
   else
   {
      RemoveSpaces( picturename );
   }
   s2 = ReplacementFilePathName( texturename );
   if (s2)
   {
      texturename = s2;
   }
   else
   {
      RemoveSpaces( texturename );
   }

   ((PRiMakeBumpV)*tp[TP_MAKEBUMP])( picturename, texturename, 
                                    swrap, twrap,
                                    filterfunc,
                                    swidth, twidth,
                                    n, tokens, parms );
   if (s1)
     _RibFree(s1);
   if (s2)
     _RibFree(s2);
}


RtVoid tpMakeCubeFaceEnvironmentV( char *px, char *nx, char *py, char *ny,
			      char *pz, char *nz, char *texturename,
			      RtFloat fov,
			      RtFilterFunc filterfunc, 
			      RtFloat swidth, RtFloat twidth,
			      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   auto char  *s1, *s2, *s3, *s4, *s5, *s6, *s7;


   s1 = ReplacementFilePathName( px );
   if (s1)
   {
      px = s1;
   }
   else
   {
      RemoveSpaces( px );
   }
   s2 = ReplacementFilePathName( nx );
   if (s2)
   {
      nx = s2;
   }
   else
   {
      RemoveSpaces( nx );
   }
   s3 = ReplacementFilePathName( py );
   if (s3)
   {
      py = s3;
   }
   else
   {
      RemoveSpaces( py );
   }
   s4 = ReplacementFilePathName( ny );
   if (s4)
   {
      ny = s4;
   }
   else
   {
      RemoveSpaces( ny );
   }
   s5 = ReplacementFilePathName( pz );
   if (s5)
   {
      pz = s5;
   }
   else
   {
      RemoveSpaces( pz );
   }
   s6 = ReplacementFilePathName( nz );
   if (s6)
   {
      nz = s6;
   }
   else
   {
      RemoveSpaces( nz );
   }
   s7 = ReplacementFilePathName( texturename );
   if (s7)
   {
      texturename = s7;
   }
   else
   {
      RemoveSpaces( texturename );
   }

   ((PRiMakeCubeFaceEnvironmentV)*tp[TP_MAKECUBEFACEENVIRONMENT])( 
                            px, nx, py, ny, pz, nz, texturename,
			    fov, filterfunc, swidth, twidth,
                            n, tokens, parms );
   if (s1)
     _RibFree(s1);
   if (s2)
     _RibFree(s2);
   if (s3)
     _RibFree(s3);
   if (s4)
     _RibFree(s4);
   if (s5)
     _RibFree(s5);
   if (s6)
     _RibFree(s6);
   if (s7)
     _RibFree(s7);
}


RtVoid tpMakeLatLongEnvironmentV( char *picturename, char *texturename, 
			     RtFilterFunc filterfunc,
			     RtFloat swidth, RtFloat twidth,
			     RtInt n, RtToken tokens[], RtPointer parms[] )
{
   auto char  *s1, *s2;


   s1 = ReplacementFilePathName( picturename );
   if (s1)
   {
      picturename = s1;
   }
   else
   {
      RemoveSpaces( picturename );
   }
   s2 = ReplacementFilePathName( texturename );
   if (s2)
   {
      texturename = s2;
   }
   else
   {
      RemoveSpaces( texturename );
   }

   ((PRiMakeLatLongEnvironmentV)*tp[TP_MAKELATLONGENVIRONMENT])( 
                                        picturename, texturename, 
					filterfunc,
					swidth, twidth,
					n, tokens, parms );
   if (s1)
     _RibFree(s1);
   if (s2)
     _RibFree(s2);
}


RtVoid tpMakeShadowV( char *picturename, char *texturename,
		 RtInt n, RtToken tokens[], RtPointer parms[] )
{
   auto char  *s1, *s2;


   s1 = ReplacementFilePathName( picturename );
   if (s1)
   {
      picturename = s1;
   }
   else
   {
      RemoveSpaces( picturename );
   }
   s2 = ReplacementFilePathName( texturename );
   if (s2)
   {
      texturename = s2;
   }
   else
   {
      RemoveSpaces( texturename );
   }

   ((PRiMakeShadowV)*tp[TP_MAKESHADOW])( picturename, texturename, 
					n, tokens, parms );
   if (s1)
     _RibFree(s1);
   if (s2)
     _RibFree(s2);
}


RtVoid tpMakeTextureV( char *picturename, char *texturename, 
		      RtToken swrap, RtToken twrap,
		      RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
		      RtInt n, RtToken tokens[], RtPointer parms[] )
{
   auto char  *s1, *s2;


   s1 = ReplacementFilePathName( picturename );
   if (s1)
   {
      picturename = s1;
   }
   else
   {
      RemoveSpaces( picturename );
   }
   s2 = ReplacementFilePathName( texturename );
   if (s2)
   {
      texturename = s2;
   }
   else
   {
      RemoveSpaces( texturename );
   }

   ((PRiMakeTextureV)*tp[TP_MAKETEXTURE])( picturename, texturename, 
					  swrap, twrap, 
					  filterfunc, 
					  swidth, twidth,
					  n, tokens, parms );
   if (s1)
     _RibFree(s1);
   if (s2)
     _RibFree(s2);
}


/*
 *
 * Unit parameterization touch-ups follow.
 *
 */


/* upNuPatchV()
 *
 * Change NuPatch and its TrimCurves to be parameterized from 0 to 1.
 *
 */
RtVoid upNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
		  RtFloat umin, RtFloat umax, 
		  RtInt nv, RtInt vorder, RtFloat vknot[],
		  RtFloat vmin, RtFloat vmax,
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   int  i;
   RtFloat  udiff,vdiff;


   /* Share max and min data with upTrimCurve by using two globals. */
   upUmin = umin;
   upUmax = umax;
   upVmin = vmin;
   upVmax = vmax;

   /* Applying trim curves will cause upTrimCurve to be called for each
    * trim curve that has been stored in upTrimStore.
    */
   TrimStoreApplyCurves(upTrimStore);

   /* Now change the min/max range to be 0 to 1 by shifting the knot values
    *    to the left by min and diving by max.
    */

   udiff = (umax-umin);
   for ( i=0; i<uorder+nu; i++ )
   {
      uknot[i] -= umin;
      uknot[i] = uknot[i] / udiff;
   }
   vdiff = (vmax-vmin);
   for ( i=0; i<vorder+nv; i++ )
   {
      vknot[i] -= vmin;
      vknot[i] = vknot[i] / vdiff;
   }
   umin = 0.0;
   umax = 1.0;
   vmin = 0.0;
   vmax = 1.0;

   /* Now print the NuPatch. */
   ((PRiNuPatchV)*up[UP_NUPATCH])(
      nu,uorder,uknot,umin,umax,nv,vorder,vknot,vmin,vmax,n,tokens,parms );
}


RtVoid upTrimCurve( RtInt nloops, 
		   RtInt ncurves[], RtInt order[], RtFloat knot[],
		   RtFloat min[], RtFloat max[], RtInt n[],
		   RtFloat u[], RtFloat v[], RtFloat w[] )
{
   int  C; /* number of curves */
   int  ncvs; /* number of control vertices */
   RtFloat  udiff,vdiff;
   register int  i;


   /* Shift values in u[] and v[] to the left by upUmin and upVmin
    *    respectively.  Then divide the values in u[] and v[] with 
    *    upUmax and upVmax respectively. 
    */
   C = 0;
   for ( i=0; i<nloops; i++ )
   {
      C += ncurves[i];
   }
   ncvs = 0;
   for ( i=0; i<C; i++ )
   {
      ncvs += n[i];
   }
   udiff = (upUmax-upUmin);
   vdiff = (upVmax-upVmin);
/*
if (udiff==0.0) {printf("udiff zero\n");
		 printf("upUmax:%g upUmin:%g\n",upUmax-upUmin);exit(0);}
if (vdiff==0.0) {printf("vdiff zero\n");
		 printf("upVmax:%g upVmin:%g\n",upVmax-upVmin);exit(0);}
*/
   for ( i=0; i<ncvs; i++ )
   {
      u[i] -= upUmin;
      v[i] -= upVmin;
      if (udiff!=0.0)
	u[i] = u[i] / udiff;
      if (vdiff!=0.0)
	v[i] = v[i] / vdiff;
   }

   /* Print new trimcurve. */
   ((PRiTrimCurve)*up[UP_TRIMCURVE])( nloops, ncurves,
				     order, knot, min, max,
				     n, u, v, w );
}


/*
 *
 * MacroModel fixes follow.
 *
 */

/* mmErrorHandler()
 *
 * This function is called when a syntax error has occurred and the 
 * macromodel fixer was requested.  Syntax errors in MacroModel files
 * are caused by lowercase letters being used as the first letter in
 * the RIB statements: Color, Opacity and Surface.  
 * 
 * So when this function is called, assume that a Color, Opacity or Surface
 * statment is being read and the first character caused the error.  If
 * the characters following the character that caused the error match 
 * one of the strings "olor", "pacity" ot "urface" than call the 
 * corresponding parameter reading function.
 */
int mmErrorHandler( RIB_HANDLE rib, int code, int severity, PRIB_ERROR error )
{
   /* The following table was originallu created with the command:
    *    tokentbl -s mmodel.asc MModel
    */
   static char MModel[] = {
      6,  6 ,'o','l','o','r','\0',          kRIB_COLOR,
      8,  8 ,'p','a','c','i','t','y','\0',  kRIB_OPACITY,
      0,  8 ,'u','r','f','a','c','e','\0',  kRIB_SURFACE
   };
   auto RtBoolean  notdone = RI_FALSE;
   auto int        i;


   if ( error->type == kRIB_ERRTYPE_SYNTAX )
   {
      i = RibReadFileForToken( rib, MModel );
      
      /* Check for invalid token. */
      do
      {
	 switch (i)
	 {
	  case kRIB_COLOR:
	  case kRIB_OPACITY:
	  case kRIB_SURFACE:
	    /* Handle RI binding. */
	    i = (*gRibReadTable[i])( (PRIB_INSTANCE)rib );
	    if ( i )
	    {
	       if ( EOF==i )
	       {
		  error->type = kRIB_ERRTYPE_UNEXPECTED_EOF;
		  RibSetError( rib, RIE_SYNTAX, RIE_SEVERE, error );
		  break;
	       }
	       else
	       {
		  error->type = kRIB_ERRTYPE_SYNTAX;
		  RibSetError( rib, RIE_SYNTAX, RIE_ERROR, error );
	       }
	       return 0; /* Not a valid MacroModel file. */
	    }
	    /* Handle white spaces and any RI request or string definitions. */
	    i = RibHandlePrefix( rib );
	    if (EOF==i)
	      return 0;

	    /* Check if another messed-up RIB call follows. */
	    if ( i=='c' || i=='o' || i=='s' )
	    {
	       /* Go back and do the next statement too.  Don't let control
                *    go back to the RibRead() function, because after it
                *    encounters a syntax error it stops calling RibSetError()
                *    until it finds something valid again.  
                * RibRead() sets an internal variable 'GotLostInRib' and does
                *    not call RibSetError() until it finds something 
                *    identifiable again.  This behavior is done to prevent
                *    hundreds of meaningless error messages. 
                */
	       i = RibReadFileForToken( rib, MModel );
	       notdone = RI_TRUE;
	    }
	    else
	    {
	       RibUngetChar(rib,i);
	       notdone = RI_FALSE;
	    }
	    break;
	  default:
	    return 0; /* Not a valid MacroModel file. */
	 }
      } while (notdone);
            
      return 0;
   }

   /* Some other error, so pass it onto the default handler. */
   RibDefaultErrorHandler( rib, code, severity, error );

   return 0;
}
