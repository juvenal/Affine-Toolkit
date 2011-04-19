/* $RCSfile: ribfunc.c,v $  $Revision: 1.8 $ $Date: 2000/02/24 18:14:28 $
 *
 * Copyright (c) 1999 Thomas E. Burge.  All rights reserved.
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
 *       Copyright (c) 1995, 1996, 1997, 1998, 1999 Thomas E. Burge.  
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
 * FILE:  ribfunc.c
 *
 * DESCRIPTION:  Functions to read RIB bindings.
 *
 *    Contains:  
 *       - RIB binding reader functions.
 *       - gRibReadTable[] -- Table of pointers to the reader functions.
 * 
 *
 *    References:
 *        [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, Richmond, CA, 
 *                  September 1989.
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
 *        [PIXA98]  Pixar, Advanced RenderMan: Beyond the Companion,
 *                  SIGGRAPH Course Notes #11, Richmond, CA, July, 1998, 
 *                  p. 12.
 *        [UPST90]  Upstill, Steve, The RenderMan Companion: A Programmer's
 *                  Guide to Realistic Computer Graphics, Addison Wesley, 
 *                  1990, pp. 249-250.
 *
 *    History:
 *       07-11-97  Added reading functions for new PRMan 3.7 Curves and 
 *                 Points primitives.  Added new Procedural support.
 *       01-07-98  Fixed mistake in RibReadPointsPolygonsV() with the
 *                 number of expected uniform values.
 *       01-07-98  Added kRIB_ERRTYPE_BAD_BASISVSTEP error message to 
 *                 RibReadCurvesV() and fixed uniform and varying 
 *                 calculations.
 *       01-08-98  Wrote a comment on avoiding a possible problem with 
 *                 libraries freeing data during a Declare statement.
 *       03-15-98  Fixed problem with RibReadBasis() trying to free
 *                 static standard basis matrices.
 *       04-06-98  Found problem with my printed copy of [PIXA89] and fixed 
 *                 PatchMesh.
 *       04-16-98  Fixed ObjectInstance's look-up in the hash table for
 *                 an kRIB_HASH_OBJECTHANDLE.
 *       04-22-98  Added savedoptions.ncolorsamples to RIB_INSTANCE because
 *                 the number of color samples is an option that is to be
 *                 saved when FrameBegin is encountered and restored at a
 *                 matching FrameEnd.
 *       04-25-98  Added Frame and World block checking to use the new status
 *                 flags kRIB_STATUS_FRAMEBLOCK and kRIB_STATUS_WORLDBLOCK.
 *                 Also added a RIE_NOTOPTIONS error message when ColorSamples
 *                 is found in a Frame and World block.  Changed some
 *                 RIE_SYNTAX to a more specific RIE_BADHANDLE when undefined
 *                 light and object handles are encountered.
 *                 Added code to set the values rib->lastnuniform, 
 *                 rib->lastnvarying and rib->lastnvertex, so functions in the
 *                 Ri table can query for the number of values to expect for
 *                 a parameter rather than waste time recalculating it.
 *       10-12-98  Added RibReadSubdivisionMeshV.
 *       12-05-98  Added RibReadGeometricRepresentation and RibReadResourceV.
 *       04-27-99  Added RibReadNuCurvesV() and RibReadImplicitV().
 *       10-05-99  Added support for archivesearchpath in RibOptionV.
 *       02-24-00  Added RibNuCurvesV and RibImplicitV to allow linking with 
 *                 client libraries such as librib.a which do not have the
 *                 Ri functions.  Also RibGeometricRepresentation.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 */
#define  __RIB_FILE_RIBFUNC
#define  _RIPRIV_TYPES
#define  _RIPRIV_FUNC_TYPES 
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <ripriv.h>
#include <ribrdr.h>
#include "ribfunc.h"


PRIB_READPROC gRibReadTable[] = {
   (PRIB_READPROC)RibReadAreaLightSourceV,
   (PRIB_READPROC)RibReadAtmosphereV,
   (PRIB_READPROC)RibReadAttributeV,
   (PRIB_READPROC)RibReadAttributeBegin,
   (PRIB_READPROC)RibReadAttributeEnd,
   (PRIB_READPROC)RibReadBasis,
   (PRIB_READPROC)RibReadBound,
   (PRIB_READPROC)RibReadClipping,
   (PRIB_READPROC)RibReadColor,
   (PRIB_READPROC)RibReadColorSamples,
   (PRIB_READPROC)RibReadConcatTransform,
   (PRIB_READPROC)RibReadConeV,
   (PRIB_READPROC)RibReadCoordinateSystem,
   (PRIB_READPROC)RibReadCoordSysTransform,
   (PRIB_READPROC)RibReadCropWindow,
   (PRIB_READPROC)RibReadCurvesV,
   (PRIB_READPROC)RibReadCylinderV,
   (PRIB_READPROC)RibReadDeclare,
   (PRIB_READPROC)RibReadDeformationV,
   (PRIB_READPROC)RibReadDepthOfField,
   (PRIB_READPROC)RibReadDetail,
   (PRIB_READPROC)RibReadDetailRange,
   (PRIB_READPROC)RibReadDiskV,
   (PRIB_READPROC)RibReadDisplacementV,
   (PRIB_READPROC)RibReadDisplayV,
   (PRIB_READPROC)RibReadErrorHandler,
   (PRIB_READPROC)RibReadExposure,
   (PRIB_READPROC)RibReadExteriorV,
   (PRIB_READPROC)RibReadFormat,
   (PRIB_READPROC)RibReadFrameAspectRatio,
   (PRIB_READPROC)RibReadFrameBegin,
   (PRIB_READPROC)RibReadFrameEnd,
   (PRIB_READPROC)RibReadGeneralPolygonV,
   (PRIB_READPROC)RibReadGeometricApproximation,
   (PRIB_READPROC)RibReadGeometricRepresentation,
   (PRIB_READPROC)RibReadGeometryV,
   (PRIB_READPROC)RibReadHiderV,
   (PRIB_READPROC)RibReadHyperboloidV,
   (PRIB_READPROC)RibReadIdentity,
   (PRIB_READPROC)RibReadIlluminate,
   (PRIB_READPROC)RibReadImagerV,
   (PRIB_READPROC)RibReadImplicitV,
   (PRIB_READPROC)RibReadInteriorV,
   (PRIB_READPROC)RibReadLightSourceV,
   (PRIB_READPROC)RibReadMakeBumpV,
   (PRIB_READPROC)RibReadMakeCubeFaceEnvironmentV,
   (PRIB_READPROC)RibReadMakeLatLongEnvironmentV,
   (PRIB_READPROC)RibReadMakeShadowV,
   (PRIB_READPROC)RibReadMakeTextureV,
   (PRIB_READPROC)RibReadMatte,
   (PRIB_READPROC)RibReadMotionBeginV,
   (PRIB_READPROC)RibReadMotionEnd,
   (PRIB_READPROC)RibReadNuCurvesV,
   (PRIB_READPROC)RibReadNuPatchV,
   (PRIB_READPROC)RibReadObjectBegin,
   (PRIB_READPROC)RibReadObjectEnd,
   (PRIB_READPROC)RibReadObjectInstance,
   (PRIB_READPROC)RibReadOpacity,
   (PRIB_READPROC)RibReadOptionV,
   (PRIB_READPROC)RibReadOrientation,
   (PRIB_READPROC)RibReadParaboloidV,
   (PRIB_READPROC)RibReadPatchV,
   (PRIB_READPROC)RibReadPatchMeshV,
   (PRIB_READPROC)RibReadPerspective,
   (PRIB_READPROC)RibReadPixelFilter,
   (PRIB_READPROC)RibReadPixelSamples,
   (PRIB_READPROC)RibReadPixelVariance,
   (PRIB_READPROC)RibReadPointsV,
   (PRIB_READPROC)RibReadPointsGeneralPolygonsV,
   (PRIB_READPROC)RibReadPointsPolygonsV,
   (PRIB_READPROC)RibReadPolygonV,
   (PRIB_READPROC)RibReadProcedural,
   (PRIB_READPROC)RibReadProjectionV,
   (PRIB_READPROC)RibReadQuantize,
   (PRIB_READPROC)RibReadReadArchiveV,
   (PRIB_READPROC)RibReadRelativeDetail,
   (PRIB_READPROC)RibReadResourceV,
   (PRIB_READPROC)RibReadReverseOrientation,
   (PRIB_READPROC)RibReadRotate,
   (PRIB_READPROC)RibReadScale,
   (PRIB_READPROC)RibReadScreenWindow,
   (PRIB_READPROC)RibReadShadingInterpolation,
   (PRIB_READPROC)RibReadShadingRate,
   (PRIB_READPROC)RibReadShutter,
   (PRIB_READPROC)RibReadSides,
   (PRIB_READPROC)RibReadSkew,
   (PRIB_READPROC)RibReadSolidBegin,
   (PRIB_READPROC)RibReadSolidEnd,
   (PRIB_READPROC)RibReadSphereV,
   (PRIB_READPROC)RibReadSubdivisionMeshV,
   (PRIB_READPROC)RibReadSurfaceV,
   (PRIB_READPROC)RibReadTextureCoordinates,
   (PRIB_READPROC)RibReadTorusV,
   (PRIB_READPROC)RibReadTransform,
   (PRIB_READPROC)RibReadTransformBegin,
   (PRIB_READPROC)RibReadTransformEnd,
   (PRIB_READPROC)RibReadTranslate,
   (PRIB_READPROC)RibReadTrimCurve,
   (PRIB_READPROC)RibReadWorldBegin,
   (PRIB_READPROC)RibReadWorldEnd,
   (PRIB_READPROC)RibRead_Version,
   /* Have table include these just incase some program expects the
    *    table to be a certain length based on kRIB_LAST_RI.
    */
   (PRIB_READPROC)NULL, /* Begin */
   (PRIB_READPROC)NULL, /* End */
   (PRIB_READPROC)NULL, /* ArchiveRecord */
   (PRIB_READPROC)NULL, /* TransformPoints */
   (PRIB_READPROC)NULL, /* ErrorAbort */
   (PRIB_READPROC)NULL, /* ErrorIgnore */
   (PRIB_READPROC)NULL, /* ErrorPrint */
   (PRIB_READPROC)NULL, /* BoxFilter */
   (PRIB_READPROC)NULL, /* CatmullRomFilter */
   (PRIB_READPROC)NULL, /* GaussianFilter */
   (PRIB_READPROC)NULL, /* SincFilter */
   (PRIB_READPROC)NULL, /* TriangleFilter */
   (PRIB_READPROC)NULL, /* ProcDelayedReadArchive */
   (PRIB_READPROC)NULL, /* ProcDynamicLoad */
   (PRIB_READPROC)NULL  /* ProcRunProgram */
};


/* RtLightHandle RiAreaLightSourceV( RtToken name, 
 *                      RtInt n, RtToken tokens[], RtPointer parms[] );
 *
 */
STATIC int RibReadAreaLightSourceV( PRIB_INSTANCE rib ) 
{ 
   RtToken        name = NULL;
   RtInt          sequence;
   RtLightHandle  lh;
   RtInt          n = 0;
   RtToken        *tokens;
   RtPointer      *parms;
   PRIB_HASHATOM  a;
   auto int       rc;
   

   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);

   /* There is a sequence number when reading a RIB binding.  The 
    *    sequence number is not there when calling RiAreaLightSource().
    */
   /* Brackets cannot be arround the sequence number (light/object 
    *   handles):
    * % echo 'LightSource "ambientlight" 2' | catrib
    * ##RenderMan RIB
    * version 3.03
    * LightSource "ambientlight" 1 
    * % echo 'LightSource "ambientlight" [2]' | catrib
    * P79014 RIB syntax error (at line 1 in stdin, last token .
    *
    * Same goes for light handles given in Illuminate statements.
    */

   rc = RibReadInteger( rib, &sequence );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* Plug-ins that hook out kRIB_AREALIGHTSOURCE can use RibGetLastSequence()
    *    to get an ID number for the light source being referenced.
    */
   rc = RibSetLastSequence( rib, sequence );  CheckForError(rc);

   /* scRibAreaLightSourceV[] = { T, [sequencenumber,] V }; */
   lh = ((PRiAreaLightSourceV)*rib->ritable[kRIB_AREALIGHTSOURCE])( name, 
                                       n, tokens, parms );
   if (!lh)
   {
      rib->error.type = kRIB_ERRTYPE_UNDEFINED_LIGHT;
      rib->error.unexpected.user_int = 0; /* Got a bad light handle. */
      RibSetError( rib, RIE_BADHANDLE, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */   
   }
   if (!RibAddItem( rib->phashtable, kRIB_HASH_LIGHTHANDLE, sequence, lh )) 
   {
      a = RibGetConflictingHashAtom( rib->phashtable );
      if (!a)
      {
         /* There is no already existing atom to get in the way, 
          *    so there must have been a problem adding to the hash table.
          */
         rib->error.type = kRIB_ERRTYPE_ADDING_TO_HASH;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         return kRIB_ERRRC_INT; /* Error */   
      }
      /* Handle existing lighthandle stored in hash table. */
      RibSetDefinedLightHandle( a, sequence, lh );
   }

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiAtmosphereV( RtToken name, 
 *                      RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadAtmosphereV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibAtmosphere[] = { T, V }; */
   ((PRiAtmosphereV)*rib->ritable[kRIB_ATMOSPHERE])( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiAttributeV( RtToken name,
 *                     RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadAttributeV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibAttributeV[] = { T, V }; */
   ((PRiAttributeV)*rib->ritable[kRIB_ATTRIBUTE])( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiAttributeBegin( void )
 *
 *
 */
STATIC int RibReadAttributeBegin( PRIB_INSTANCE rib ) 
{ 
   /* scRibAttributeBegin[] = { N }; */
   ((PRiAttributeBegin)*rib->ritable[kRIB_ATTRIBUTEBEGIN])();

   /* Mark the level of attribute block. */
   rib->blocklevel++;

   return kRIB_OK; 
}


/* RtVoid RiAttributeEnd( void )
 *
 *
 */
STATIC int RibReadAttributeEnd( PRIB_INSTANCE rib ) 
{ 
   auto PRIB_UVSTEPS  p;


   /* scRibAttributeEnd[] = { N }; */
   ((PRiAttributeEnd)*rib->ritable[kRIB_ATTRIBUTEEND])();

   /* If the blocklevel matches the level on the stack
    *    restore the data.
    */
   if ( rib->stepstack && rib->blocklevel==rib->stepstack->blocklevel )
   {
      /* Restore the data. */
      rib->ustep = rib->stepstack->ustep;
      rib->vstep = rib->stepstack->vstep;
      
      /* Remove item from stack. */
      p = rib->stepstack;
      rib->stepstack = rib->stepstack->next;
      _RibFree(p);
   }

   /* Don't let blocklevel go negative. */
   if (rib->blocklevel)
   {
      rib->blocklevel--;
   }

   return kRIB_OK; 
}


/* RtVoid RiBasis( RtBasis ubasis, RtInt ustep, RtBasis vbasis, RtInt vstep )
 *
 *
 */
STATIC int RibReadBasis( PRIB_INSTANCE rib ) 
{ 
   unsigned char params[ s1Bs + s1I + s1Bs + s1I ];
   auto PRIB_UVSTEPS  steps;
   auto int           rc;
   auto void          *p;


   /* Only need to memset over the pointers, so s2Bs+s2I is not needed.
    *    Only s2Bs+s1I is needed.
    */
   memset( params, 0, s2Bs+s1I );
   /* scRibBasis[] = { Bs, I, Bs, I, N }; */
   rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_BASIS], 
                                 NULL, params );  CheckForError(rc);

   /* If not in an attribute block, then the default values
    *    are written over and saving them on the stack does 
    *    not do anything as there is no way to pop the data
    *    back off.  So check if at level zero before adding
    *    to the step stack.
    */
   if ( rib->blocklevel )
   {
      steps = (PRIB_UVSTEPS)_RibMalloc(sizeof(RIB_UVSTEPS));
      if (!steps)
      {
	 rc = kRIB_ERRRC_INT;
	 goto Error;
      }

      /* Save old uv step values. */
      steps->ustep = rib->ustep;
      steps->vstep = rib->vstep;

      /* Save attribute scope level. */
      steps->blocklevel = rib->blocklevel;

      /* Add to the stack. */
      steps->next = rib->stepstack;
      rib->stepstack = steps;
   }

   /* Save new uvstep values. */
   rib->ustep = IntFromPtrAndInt(params,s1Bs);
   rib->vstep = IntFromPtrAndInt(params,s2Bs+s1I);

   /* Call the RiBasis() function in the Ri table. */
   ((PRiBasis)*rib->ritable[kRIB_BASIS])( 
      BasisFromPtrAndInt(params,0),         
      IntFromPtrAndInt(params,s1Bs),
      BasisFromPtrAndInt(params,s1Bs+s1I),  
      IntFromPtrAndInt(params,s2Bs+s1I) );

   if ( RibShouldFreeData(rib) )
   {
      rc = kRIB_OK;
      /* Fall through to clean-up code. */
   }
   else
   {
      return kRIB_OK; 
   }

 Error:
   /*  Assume rc has been assigned a Return Code.
    *  Fall through to clean-up code. 
    */

   /* CleanUp: */
   /* 3-15-98:
    *    Do not free the standard basis matrices since they can
    *    be linked in as static data that is not free-able.
    */
   p = PtrFromPtrAndInt(params,0);
   if ( p
        && p != RiBezierBasis 
        && p != RiBSplineBasis
        && p != RiCatmullRomBasis
        && p != RiHermiteBasis
        && p != RiPowerBasis )
   {
      _RibFree( p );
   }
   p = PtrFromPtrAndInt(params,s1Bs+s1I);
   if ( p 
        && p != RiBezierBasis 
        && p != RiBSplineBasis
        && p != RiCatmullRomBasis
        && p != RiHermiteBasis
        && p != RiPowerBasis )
   {
      _RibFree( p );
   }
   return rc;
}


/* RtVoid RiBound( RtBound bound )
 *
 *
 */
STATIC int RibReadBound( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p;
   auto int  rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 
                                  sizeof(RtBound)/sizeof(RtFloat), &p );  
   CheckForError(rc);

   /* scRibBound[] = { B, N }; */
   ((PRiBound)*rib->ritable[kRIB_BOUND])( p );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc;
}


/* RtVoid RiClipping( RtFloat hither, RtFloat yon )
 *
 *
 */
STATIC int RibReadClipping( PRIB_INSTANCE rib ) 
{ 
   RtFloat   hither, yon;
   auto int  rc;


   rc = RibReadFloatingPoint( rib, &hither );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &yon );  CheckForError(rc);

   /* scRibClipping[] = { F, F, N }; */
   ((PRiClipping)*rib->ritable[kRIB_CLIPPING])( hither, yon );
   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiColor( RtColor color )
 *
 *
 */
STATIC int RibReadColor( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p;
   auto int  ncolorsamples;
   auto int  rc;


   /* Remember Colors aren't limited to being an array of
    *    three RtFloats.  ColorSamples can change that.
    */
   ncolorsamples = RibGetNColorSamples(rib);
   if (!ncolorsamples)
   {
      rc = kRIB_ERRRC_INT;
      goto Error;
   }

   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, ncolorsamples, 
                                  &p );  CheckForError(rc);
   /* scRibColor[] = { C, N }; */
   ((PRiColor)*rib->ritable[kRIB_COLOR])( p );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc;
}


/* RtVoid RiColorSamples( RtInt n, RtFloat nRGB[], RtFloat RGBn[] )
 *
 *
 */
STATIC int RibReadColorSamples( PRIB_INSTANCE rib ) 
{ 
   RtInt      i;
   RtInt      n = 0;
   RtFloat    *nRGB = NULL;
   RtFloat    *RGBn = NULL;
   RtBoolean  invalidcall = RI_FALSE;
   auto int   rc;


   /* Don't allow ColorSamples statements inside Frame and World blocks. */
   if ( RibGetStatus(rib) & (kRIB_STATUS_FRAMEBLOCK | kRIB_STATUS_WORLDBLOCK) )
   {
      /* ColorSamples was given inside a Frame or World Begin/End block, so 
       *    give an error message.
       */
      rib->error.type = kRIB_ERRTYPE_BAD_COLORSAMPLES;
      RibSetError( rib, RIE_NOTOPTIONS, RIE_ERROR, &(rib->error) );

      /* Continue reading even though an error was given, but mark
       *   this as invalid ColorSamples statement that should not be
       *   given to the kRIB_COLORSAMPLES Ri call.
       */
      invalidcall = RI_TRUE;
   }

   /* When reading from a RIB file the value n is not present.  
    * The value of n is determined by the size of nRGB[].
    */
   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_FLOAT, 
                              (int*)&i, (void**)&nRGB );  
                              CheckForError(rc);
   if ( i > 0 && !(i%3) )
   {
      n = i / 3;
      rc = RibSetNColorSamples( rib, n);  CheckForError(rc);
   }
   else
   {
      rib->error.type = kRIB_ERRTYPE_N_COLORSAMPLES;
      rib->error.unexpected.user_int = n;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */   
   }
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  i, &RGBn );  CheckForError(rc);

   /* Don't call the Ri function if the call was invalid.  This check
    *    is here so that the contents of the invalid ColorSamples statement
    *    could be read-in and gotten out of the way.
    */
   if ( invalidcall )
      goto Error;

   /* scRibColorSamples[] = { I, Fa, Fa, N }; */
   ((PRiColorSamples)*rib->ritable[kRIB_COLORSAMPLES])( n, nRGB, RGBn );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree(nRGB);
      _RibFree(RGBn);
   }

   return kRIB_OK; 

 Error:
   if (nRGB)
     _RibFree(nRGB);
   if (RGBn)
     _RibFree(RGBn);
   return rc;
}


/* RtVoid RiConcatTransform( RtMatrix transform)
 *
 *
 */
STATIC int RibReadConcatTransform( PRIB_INSTANCE rib ) 
{ 
   void      *p;
   auto int  rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  sizeof(RtMatrix)/sizeof(RtFloat), 
                                  (RtFloat**)&p );  CheckForError(rc);

   /* scRibConcatTransform[] = { M, N }; */
   ((PRiConcatTransform)*rib->ritable[kRIB_CONCATTRANSFORM])((RtFloat(*)[4])p);

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc;
}


/* RtVoid RiConeV( RtFloat height, RtFloat radius, RtFloat thetamax,
 *                RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadConeV( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 3, &p ); 
                                  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 4,
                             /* vertex  */ 4,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibConeV[] = { F, F, F, V }; */
   ((PRiConeV)*rib->ritable[kRIB_CONE])( p[0],p[1],p[2], n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( p );     
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (p)
     _RibFree(p);
   return rc;
}


/* RtVoid RiCoordinateSystem( RtToken space )
 *
 *
 */
STATIC int RibReadCoordinateSystem( PRIB_INSTANCE rib ) 
{ 
   RtToken   space;
   auto int  rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &space );  CheckForError(rc);

   /* scRibCoordinateSystem[] = { T, N }; */
   ((PRiCoordinateSystem)*rib->ritable[kRIB_COORDINATESYSTEM])( space );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree(space);
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for "space" if an error occurred. */
   return rc;
}


/* RtVoid RiCoordSysTransform( RtToken space )
 *
 *
 */
STATIC int RibReadCoordSysTransform( PRIB_INSTANCE rib ) 
{ 
   RtToken   space;
   auto int  rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &space );  CheckForError(rc);

   /* scRibCoordSysTransfrom[] = { T, N }; */
   ((PRiCoordSysTransform)*rib->ritable[kRIB_COORDSYSTRANSFORM])( space );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree(space);
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for "space" if an error occurred. */
   return rc;
}


/* RtVoid RiCropWindow( RtFloat xmin, RtFloat xmax, 
 *                      RtFloat ymin, RtFloat ymax )
 *
 *
 */
STATIC int RibReadCropWindow( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p;
   auto int  rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 4, &p ); 
   CheckForError(rc);

   /* scRibCropWindow[] = { F, F, F, F, N }; */
   ((PRiCropWindow)*rib->ritable[kRIB_CROPWINDOW])( p[0],p[1],p[2],p[3] );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree(p);
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc;
}


/* RtVoid RiCurvesV( RtToken type, RtInt ncurves, RtInt nvertices[], 
 *                   RtToken wrap, 
 *                   RtInt n, RtToken tokens[], RtPointer parms[] ); 
 *
 */
STATIC int RibReadCurvesV( PRIB_INSTANCE rib ) 
{ 
   RtToken    type = NULL;
   int        t;
   int        nstep;
   RtInt      ncurves;
   RtInt      *nvertices = NULL;
   RtToken    wrap = NULL;
   RtInt      nonperiodic;
   RtInt      uniform,varying,vertex;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   char       *p;
   auto int   i,rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);

   /* Get type. */
   t = RibReadStringForToken( type, gRibCurvesTable, &p );
   if ( t<0 || *p )
   {
      rib->error.type = kRIB_ERRTYPE_BAD_CURVETYPE;
      rib->error.unexpected.user_string = type;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      rc = kRIB_ERRRC_INT; /* Error */   
      goto Error;
   }

   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_INTEGER, 
                              (int*)&ncurves, (void**)&nvertices );  
                              CheckForError(rc);

   /* Get wrap. */
   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &wrap );  CheckForError(rc);
   i = RibReadStringForToken( wrap, gRibPeriodicTable, &p );
   if ( i<0 || *p )
   {
      rib->error.type = kRIB_ERRTYPE_BAD_WRAPMODE;
      rib->error.unexpected.user_string = wrap;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      rc = kRIB_ERRRC_INT; /* Error */   
      goto Error;
   }
   if ( 0==i ) /* periodic */
   {
      nonperiodic = 0;
   }
   else if ( 1==i ) /* nonperiodic */
   {
      nonperiodic = 1;
   }
   else /* oops */
   {
      rc = kRIB_ERRRC_INT; /* Error */   
      goto Error;
   }
   vertex = 0;
   for ( i=0; i<ncurves; i++ )
   {
      vertex += nvertices[i];
   }
   if ( 0==t ) /* linear */
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
   else if ( 1==t ) /* cubic */
   {
      if (rib->vstep<=0)
      {
         rib->error.type = kRIB_ERRTYPE_BAD_BASISVSTEP;
         rib->error.unexpected.user_int = rib->vstep;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         rc = kRIB_ERRRC_INT; /* Error */   
         goto Error;
      }

      if ( nonperiodic )
      {
	 nstep = vertex - 4*ncurves;
         uniform = ncurves + nstep / (rib->vstep);
         varying = ncurves + uniform;         
      }
      else
      {
	 nstep = vertex;
         varying = uniform = nstep / (rib->vstep);
      }
   }
   else
   {
      rc = kRIB_ERRRC_INT; /* Error */   
      goto Error;
   }

   rc = RibReadParameterList( rib, kRIB_REQUIRE_P,
                             /* uniform */ uniform, 
                             /* varying */ varying,
                             /* vertex  */ vertex,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibCurvesV[] = { T, I, Ia, T, V }; */
   ((PRiCurvesV)*rib->ritable[kRIB_CURVES])( type, ncurves, nvertices, 
                                            wrap, 
                                            n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(wrap);
      _RibFree(nvertices);
      _RibFree(type);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (wrap)
      _RibFree(wrap);
   if (nvertices)
     _RibFree(nvertices);
   if (type)
     _RibFree(type);
   return rc; 
}


/* RtVoid RiCylinderV( RtFloat radius, RtFloat zmin, RtFloat zmax,
 *                    RtFloat thetamax,
 *                    RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 *
 */
STATIC int RibReadCylinderV( PRIB_INSTANCE rib ) 
{ 
   RtFloat    *p = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 4, &p ); 
   CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 4,
                             /* vertex  */ 4,
                             &n, &tokens, &parms );  CheckForError(rc);
   /* scRibCylinderV[] = { F, F, F, F, V }; */
   ((PRiCylinderV)*rib->ritable[kRIB_CYLINDER])( p[0],p[1],p[2],p[3],
                                                n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(p);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (p)
     _RibFree(p);
   return rc;
}


/* RtToken RiDeclare( char *name, char *declaration )
 *
 *
 */
STATIC int RibReadDeclare( PRIB_INSTANCE rib ) 
{ 
   char                *name = NULL;
   char                *declaration = NULL;
   RtToken             t;
   auto PRIB_HASHATOM  a;
   auto int            rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name ); 
        CheckForError(rc);
   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &declaration ); 
        CheckForError(rc);
   a = RibDeclare( rib, name, declaration );
   /* Check for error. */
   if (!a)
   {
      rc = kRIB_ERRRC_INT;
      goto Error;
   }

   /* 06-04-99
    * NEW:  Use 'a' returned from RibDeclare() to get the token name and 
    *       copy it for ritable[]'s kRIB_DECLARE call.  Note that this 
    *       makes it so the ritable[] declaration no longer has the 
    *       inline details in the name string.
    */

   /* scRibDeclare[] = { Cpr, Cpr, N }; */
   t = ((PRiDeclare)*rib->ritable[kRIB_DECLARE])( name, declaration );

   /* Normally ribfunc.c code would at this point check if all the
    *    data should be freed, but the pointer to the string "name" 
    *    gets added into a hash table and shouldn't be freed.  
    *    The string declaration once parsed by a RIB writer 
    *    library is no longer needed.
    */
   /* Assume cleaning up of the hash table will handle deallocating the
    *    the string "name".
    */
   if ( RibShouldFreeData(rib) )
   {
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
      _RibFree( declaration );
      _RibFree( name );
   }
#ifdef DECLARE_PROBLEM
   else
   {
      /* 06-04-99 -- The following has been outdated with support
       *             added for inline declarations and copying of
       *             the name string.
       */
      /* OUTDATE 06-04-99:
       * 01-08-98 -- Because name was not copied, a library
       *             that hooked into the Ri Table could
       *             have called RibKeepData() and then
       *             freed name.  If the above call to
       *             RibShouldFreeData() ever returns RI_FALSE,
       *             it would mean someone's code just nuked
       *             the data pointed to by name.
       */
      /* OUTDATE 06-04-99:
       * I'm tempted to make a copy of name for the hash table
       *    and a copy for the Ri Table's kRIB_DECLARE function.
       *    It should be rare for anybody to hook into Declare
       *    so I'll leave this as is and require that libraries
       *    hooking into Declare not free name.  Besides they
       *    would have to access the hash table anyway if they
       *    wanted to redeclare something.
       */
   }
#endif        

   /* Save off the return code from RiDeclare into HASHATOM's extended 
    *    data field.  The return code from RiDeclare is supposed to be 
    *    a pointer to be used as an efficient way to refer to the 
    *    variable that was declared.  (Refer to spec.)
    *
    * If the user wants to use extdata for their own purpose, the user 
    *    will need to return a NULL from RiDeclare().  That way the 
    *    data won't be written over.    
    */
   /* Don't write over extended data if t is NULL. */
   if (!t) 
   {
      RibSetHashExtendedData( a, t );
   }

   return kRIB_OK; 

 Error:
   if (declaration)
     _RibFree(declaration);
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiDeformationV( RtToken name,
 *                       RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadDeformationV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibDeformationV[] = { T, V }; */
   ((PRiDeformationV)*rib->ritable[kRIB_DEFORMATION])
     ( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiDepthOfField( RtFloat fstop, RtFloat focallength, 
 *                       RtFloat focaldistance )
 *
 */
STATIC int RibReadDepthOfField( PRIB_INSTANCE rib ) 
{ 
   unsigned char  params[ s1F + s1F + s1F ];
   RtFloat        *f;
   auto int       rc;


   if (RibIsaNumberNext( rib ))
   {
      /* scRibDepthOfField[] = { F, F, F, N }; */
      rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_DEPTHOFFIELD], 
                                    NULL, params );  
      CheckForError(rc);
   }
   else
   {
      /* Catrib 3.6c handles DepthOfField with no data by printing
       *    "DepthOfField 1e+38 0 0".
       */
      f = (RtFloat*)params;
      *f++ = RI_INFINITY;
      *f++ = 0.0;
      *f++ = 0.0;
   }

   ((PRiDepthOfField)*rib->ritable[kRIB_DEPTHOFFIELD])(
        FltFromPtrAndInt(params,0),    FltFromPtrAndInt(params,s1F),
        FltFromPtrAndInt(params,s2F) );

   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiDetail( RtBound bound )
 *
 *
 */
STATIC int RibReadDetail( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p;
   auto int  rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 
                                  sizeof(RtBound)/sizeof(RtFloat), 
                                  &p );  CheckForError(rc);

   /* scRibDetail[] = { B, N }; */
   ((PRiDetail)*rib->ritable[kRIB_DETAIL])( p );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );
   }

   return kRIB_OK; 

 Error:
    /* Nothing would have been allocated for p if an error occurred. */
   return rc;
}


/* RtVoid RiDetailRange( RtFloat minvisible, RtFloat lowertransition, 
 *                      RtFloat uppertransition, RtFloat maxvisible )
 *
 */
STATIC int RibReadDetailRange( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p;
   auto int  rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 4, &p ); 
   CheckForError(rc);

   /* scRibDetailRange[] = { F, F, F, F, N }; */
   ((PRiDetailRange)*rib->ritable[kRIB_DETAILRANGE])( p[0],p[1],p[2],p[3] );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree(p);
   }

   return kRIB_OK; 

 Error:
    /* Nothing would have been allocated for p if an error occurred. */
   return rc; 
}


/* RtVoid RiDiskV( RtFloat height, RtFloat radius, RtFloat thetamax,
 *                RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadDiskV( PRIB_INSTANCE rib ) 
{ 
   RtFloat    *p = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 3, &p ); 
        CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 4,
                             /* vertex  */ 4,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibDiskV[] = { F, F, F, V }; */
   ((PRiDiskV)*rib->ritable[kRIB_DISK])( p[0],p[1],p[2], n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );     
   }

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
   }

   return kRIB_OK; 

 Error:
   if (p)
     _RibFree( p );     
   return rc;
}


/* RtVoid RiDisplacementV( RtToken name,
 *                        RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadDisplacementV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibDisplacementV[] = { T, V }; */
   ((PRiDisplacementV)*rib->ritable[kRIB_DISPLACEMENT])
     ( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( name );     
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree( name );     
   return rc;
}


/* RtVoid RiDisplayV( char *name, RtToken type, RtToken mode, 
 *                   RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadDisplayV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtToken    type = NULL;
   RtToken    mode = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);
   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &mode );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibDisplayV[] = { Cpr, T, T, V }; */
   ((PRiDisplayV)*rib->ritable[kRIB_DISPLAY])( name, type, mode,
                                              n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(mode);
      _RibFree(type);
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (mode)
      _RibFree(mode);
   if (type)
      _RibFree(type);
   if (name) 
     _RibFree(name);
   return rc;
}


/* RtVoid RiErrorHandler( RtErrorHandler handler )
 *
 *
 */
STATIC int RibReadErrorHandler( PRIB_INSTANCE rib ) 
{ 
   unsigned char params[ s1Eh ];
   auto int   rc;


   /* scRibErrorHandler[] = { Eh, N }; */
   rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_ERRORHANDLER], 
                                 NULL, params );  CheckForError(rc);

   ((PRiErrorHandler)*rib->ritable[kRIB_ERRORHANDLER])( 
                             (RtErrorHandler)PtrFromPtrAndInt(params,0) );
   /* Don't free PtrFromPtrAndInt(params,0).  It's a pointer to function. */
   return kRIB_OK; 

 Error:
   /* Don't free PtrFromPtrAndInt(params,0).  It's a pointer to function. */
   return rc;
}


/* RtVoid RiExposure( RtFloat gain, RtFloat gamma )
 *
 *
 */
STATIC int RibReadExposure( PRIB_INSTANCE rib ) 
{ 
   unsigned char params[ s2F ];
   auto int  rc;


   /* scRibExposure[] = { F, F, N }; */
   rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_EXPOSURE], 
                                 NULL, params );  CheckForError(rc);
   ((PRiExposure)*rib->ritable[kRIB_EXPOSURE])( FltFromPtrAndInt(params,0),
                                      FltFromPtrAndInt(params,s1Fa));
   return kRIB_OK; 

 Error:
   return rc; 
}


/* RtVoid RiExteriorV( RtToken name, 
 *                    RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadExteriorV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;

   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibExteriorV[] = { T, V }; */
   ((PRiExteriorV)*rib->ritable[kRIB_EXTERIOR])( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
      _RibFree(name);
   return rc;
}


/* RtVoid RiFormat( RtInt xres, RtInt yres, RtFloat aspect )
 *
 *
 */
STATIC int RibReadFormat( PRIB_INSTANCE rib ) 
{ 
   unsigned char  params[ s2I + s1F ];
   auto int       rc;


   /* scRibFormat[] = { I, I, F, N }; */
   rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_FORMAT], 
                                 NULL, params );  
                                 CheckForError(rc);

   ((PRiFormat)*rib->ritable[kRIB_FORMAT])(
        IntFromPtrAndInt(params,0),    IntFromPtrAndInt(params,s1I),
        FltFromPtrAndInt(params,s2I) );

   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiFrameAspectRatio( RtFloat aspect )
 *
 *
 */
STATIC int RibReadFrameAspectRatio( PRIB_INSTANCE rib ) 
{ 
   RtFloat   aspect;
   auto int  rc;


   rc = RibReadFloatingPoint( rib, &aspect );  CheckForError(rc);

   /* scRibFrameAspectRatio[] = { F, N }; */
   ((PRiFrameAspectRatio)*rib->ritable[kRIB_FRAMEASPECTRATIO])( aspect );

   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiFrameBegin( RtInt frame )
 *
 *
 */
STATIC int RibReadFrameBegin( PRIB_INSTANCE rib ) 
{ 
   RtInt     frame;
   auto int  rc;


   if ( RibGetStatus(rib) & kRIB_STATUS_FRAMEBLOCK )
   {
      /* A FrameBegin was given inside a FrameBegin/End block, so give
       *    an error message and then try to just read through the rest
       *    of the statement.
       */
      rib->error.type = kRIB_ERRTYPE_BAD_FRAMEBEGIN;
      RibSetError( rib, RIE_NESTING, RIE_ERROR, &(rib->error) );

      /* Continue reading even though an error was given. */
   }
   /* Mark that reader has entered a FrameBegin/End block. */
   RibSetStatus( rib, kRIB_STATUS_FRAMEBLOCK, kRIB_STATUS_FRAMEBLOCK );

   if (RibIsaNumberNext( rib ))
   {
      rc = RibReadInteger( rib, &frame );  CheckForError(rc);
   }
   else
   {
      /* FrameBegin has an optional framenumber, but what happens
       *    to the frame number value given to RiFrameBegin when
       *    no integer is given in a RIB file:
       *      
       *      % cat frame.rib
       *      FrameBegin -9
       *      FrameBegin 
       *      FrameBegin 0
       *      FrameBegin 9999
       *      
       *      % catrib frame.rib
       *      ##RenderMan RIB
       *      version 3.03
       *      FrameBegin -9
       *      FrameBegin 0
       *      FrameBegin 0
       *      FrameBegin 9999
       *
       * In the above example FrameBegin with no frame number 
       *    has its frame number set to zero.
       */
      frame = 0;
   }

   rib->savedoptions.ncolorsamples = rib->ncolorsamples;

   /* scRibFrameBegin[] = { I, N }; */
   ((PRiFrameBegin)*rib->ritable[kRIB_FRAMEBEGIN])( frame );

   return kRIB_OK; 

 Error:
   return rc; 
}


/* RtVoid RiFrameEnd( void )
 *
 *
 */
STATIC int RibReadFrameEnd( PRIB_INSTANCE rib ) 
{ 
   if ( !(RibGetStatus(rib) & kRIB_STATUS_FRAMEBLOCK) )
   {
      /* A FrameEnd was given without a matching FrameBegin, so give
       *    an error message and then try to just read through the rest
       *    of the statement.
       */
      rib->error.type = kRIB_ERRTYPE_BAD_FRAMEEND;
      RibSetError( rib, RIE_NESTING, RIE_ERROR, &(rib->error) );

      /* Continue reading even though an error was given. */
   }
   /* Mark that reader has left a FrameBegin/End block. */
   RibSetStatus( rib, kRIB_STATUS_FRAMEBLOCK, kRIB_STATUS_CLEAR );

   rib->ncolorsamples = rib->savedoptions.ncolorsamples;

   /* scRibFrameEndV[] = { N }; */
   ((PRiFrameEnd)*rib->ritable[kRIB_FRAMEEND])();

   return kRIB_OK; 
}


/* RtVoid RiGeneralPolygonV( RtInt nloops, RtInt nvertices[],
 *                          RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadGeneralPolygonV( PRIB_INSTANCE rib ) 
{
   RtInt         nloops;
   RtInt         *nvertices = NULL;
   RtInt         n = 0;
   RtToken       *tokens;
   RtPointer     *parms;
   auto int      i, v, rc;


   /* When reading from a RIB file the value nloops is not present.  
    * The value of nloops is determined by the size of nvertices[].
    */
   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_INTEGER, 
                              (int*)&nloops, (void**)&nvertices );  
                              CheckForError(rc);
   v = 0;
   for ( i=0; i< nloops; i++ )
   {
      v += nvertices[i];
   }

   rc = RibReadParameterList( rib, kRIB_REQUIRE_P,
                             /* uniform */ 1, 
                             /* varying */ v,
                             /* vertex  */ v,
                             &n, &tokens, &parms );  
                             CheckForError(rc);

   /* scRibGeneralPolygonV[] = { I, Ia, V }; */
   ((PRiGeneralPolygonV)*rib->ritable[kRIB_GENERALPOLYGON])( 
                                        nloops, nvertices, n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(nvertices);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (nvertices)
     _RibFree(nvertices);
   
   return rc;
}


/* RtVoid RiGeometricApproximation( RtToken type, RtFloat value )
 *
 *
 */
STATIC int RibReadGeometricApproximation( PRIB_INSTANCE rib ) 
{ 
   RtToken   type = NULL;
   RtFloat   value;
   auto int  rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &value );  CheckForError(rc);

   /* scRibGeometricApproximation[] = { T, F, N }; */
   ((PRiGeometricApproximation)*rib->ritable[kRIB_GEOMETRICAPPROXIMATION])( 
                                        type, value );
   if ( RibShouldFreeData(rib) )
   {
      _RibFree(type);
   }

   return kRIB_OK; 

 Error:
   if (type)
     _RibFree(type);
   return rc;
}


/* RtVoid RiGeometricRepresentation( RtToken type )
 *
 *
 */
STATIC int RibReadGeometricRepresentation( PRIB_INSTANCE rib ) 
{ 
   RtToken   type = NULL;
   auto int  rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);

   /* scRibGeometricRepresentation[] = { T, N }; */
   ((PRiGeometricRepresentation)*rib->ritable[kRIB_GEOMETRICREPRESENTATION])( 
                                        type );
   if ( RibShouldFreeData(rib) )
   {
      _RibFree(type);
   }

   return kRIB_OK; 

 Error:
   if (type)
     _RibFree(type);
   return rc;
}


RtVoid RibGeometricRepresentation( RtToken type )
{
   (void)type;
   /* Incase a client library does not have RiGeometricRepresentation, 
    *    provide this as an alternative to link with in the gRibRITable[].
    * Refer to ritable.c.
    */
   return;
}


/* RtVoid RiGeometryV( RtToken type, 
 *                    RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadGeometryV( PRIB_INSTANCE rib ) 
{ 
   RtToken    type = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibGeometryV[] = { T, V }; */
   ((PRiGeometryV)*rib->ritable[kRIB_GEOMETRY])( type, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(type);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (type)
     _RibFree(type);
   return rc;
}


/* RtVoid RiHiderV( RtToken type,
 *                 RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadHiderV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;

   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibHiderV[] = { T, V }; */
   ((PRiHiderV)*rib->ritable[kRIB_HIDER])( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);   
   return rc;
}


/* RtVoid RiHyperboloidV( RtPoint point1, RtPoint point2, RtFloat thetamax,
 *                       RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadHyperboloidV( PRIB_INSTANCE rib ) 
{ 
   RtFloat    *p = NULL;
   RtFloat    *point1 = NULL;
   RtFloat    *point2 = NULL;
   RtFloat    thetamax;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 7, &p ); 
        CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 4,
                             /* vertex  */ 4,
                             &n, &tokens, &parms );  CheckForError(rc);
   point1 = (RtFloat*)_RibMalloc(sizeof(RtPoint));
   if (!point1)
   {
      rc = kRIB_ERRRC_INT;
      goto Error;
   }
   point2 = (RtFloat*)_RibMalloc(sizeof(RtPoint));
   if (!point2)
   {
      rc = kRIB_ERRRC_INT;
      goto Error;
   }
   memcpy( point1, &p[0], sizeof(RtPoint) );
   memcpy( point2, &p[3], sizeof(RtPoint) );
   thetamax = FltFromPtrAndInt( p, 2*sizeof(RtPoint) );
#ifdef RIB_MEMORY_TESTS
               RibMemoryTestStepOverAllowed();
#endif
   /* Always free this array pointer.  The user never sees it. */
   _RibFree(p);
   
   /* scRibHyperboloidV[] = { Pt, Pt, F, V }; */
   ((PRiHyperboloidV)*rib->ritable[kRIB_HYPERBOLOID])( 
                                        point1, point2,
                                        thetamax, 
                                        n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      _RibFree(point2);
      _RibFree(point1);
      RibFreeVectorData( rib, n, tokens, parms );
   }

   return kRIB_OK; 

 Error:
   if (point2)
     _RibFree(point2);
   if (point1)
     _RibFree(point1);
   RibFreeVectorData( rib, n, tokens, parms );
   if (p)
     _RibFree(p);
   return rc;
}


/* RtVoid RiIdentity( void )
 *
 *
 */
STATIC int RibReadIdentity( PRIB_INSTANCE rib ) 
{ 
   /* scRibIdentity[] = { N }; */
   ((PRiIdentity)*rib->ritable[kRIB_IDENTITY])();

   return kRIB_OK; 
}


/* RtVoid RiIlluminate( RtLightHandle light, RtBoolean onoff )
 *
 *
 */
STATIC int RibReadIlluminate( PRIB_INSTANCE rib ) 
{ 
   RtInt          sequence;
   RtInt          onoff;
   PRIB_HASHATOM  p = NULL;
   RtLightHandle  lh;
   auto int       rc;


   /* RIB binding uses a sequence number instead of a handle. */
   rc = RibReadInteger( rib, &sequence );  CheckForError(rc);   
   if ( sequence > 0 )
   {
      p = RibFindMatch( rib->phashtable, kRIB_HASH_LIGHTHANDLE, 
                       sequence, NULL );
   }
   if (!p)
   {
      rib->error.type = kRIB_ERRTYPE_UNDEFINED_LIGHT;
      rib->error.unexpected.user_int = sequence;
      RibSetError( rib, RIE_BADHANDLE, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */   
   }

   lh = RibGetDefinedLightHandle( p );
   rc = RibReadInteger( rib, &onoff );  CheckForError(rc);

   /* scRibIlluminate[] = { Lh, Bl, N }; */
   ((PRiIlluminate)*rib->ritable[kRIB_ILLUMINATE])( lh, onoff );

   /* Clear keep data flag incase ritable[] function called RibKeepData(). */
   rib->status &= ~kRIB_STATUS_KEEP_DATA;

   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiImagerV( RtToken name,
 *                  RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadImagerV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibImagerV[] = { T, V }; */
   ((PRiImagerV)*rib->ritable[kRIB_IMAGER])( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiImplicitV( RtInt a, RtInt b[], RtInt, RtFloat c[],
 *                     RtInt d, RtFloat e[], RtInt f, RtFloat g[],
 *                     RtInt n, RtToken tokens[], RtPointer parms[] );
 */
STATIC int RibReadImplicitV( PRIB_INSTANCE rib ) 
{
   (void)rib;

   return 0;
}


RtVoid RibImplicitV( RtInt a, RtInt b[], RtInt c, RtFloat d[],
		     RtInt e, RtFloat f[], RtInt g, RtFloat h[],
		     RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; (void)g; (void)h;
   (void)n; (void)tokens; (void)parms;
   /* Incase a client library does not have RiImplicitV, provide this
    *    as an alternative to link with in the gRibRITable[].
    */
   return;
}


/* RtVoid RiInteriorV( RtToken name, 
 *                    RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadInteriorV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibInteriorV[] = { T, V }; */
   ((PRiInteriorV)*rib->ritable[kRIB_INTERIOR])( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
      _RibFree(name);
   return rc;
}


/* RtLightHandle RiLightSourceV( RtToken name, 
 *                              RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadLightSourceV( PRIB_INSTANCE rib ) 
{ 
   RtToken        name = NULL;
   RtInt          sequence;
   RtLightHandle  lh;
   RtInt          n = 0;
   RtToken        *tokens;
   RtPointer      *parms;
   auto int       rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);

   /* There is a sequence number when reading a RIB binding.  The 
    *    sequence number is not there when calling RiLightSource().
    */
   rc = RibReadInteger( rib, &sequence );  CheckForError(rc);

   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* Plug-ins that hook out kRIB_LIGHTSOURCE can use RibGetLastSequence()
    *    to get an ID number for the light source being referenced.
    */
   rc = RibSetLastSequence( rib, sequence );  CheckForError(rc);

   /* scRibLightSourceV[] = { T, [sequencenumber], V }; */
   lh = ((PRiLightSourceV)*rib->ritable[kRIB_LIGHTSOURCE])( name, 
                                                          n, tokens, parms );
   if (!lh)
   {
      rib->error.type = kRIB_ERRTYPE_UNDEFINED_LIGHT;
      rib->error.unexpected.user_int = 0; /* Got a bad light handle. */
      RibSetError( rib, RIE_BADHANDLE, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */   
   }
   if (!RibAddItem( rib->phashtable, kRIB_HASH_LIGHTHANDLE, sequence, lh )) 
   {
      PRIB_HASHATOM  a;

      a = RibGetConflictingHashAtom( rib->phashtable );
      if (!a)
      {
         /* There is no already existing atom to get in the way, 
          *    so there must have been a problem adding to the hash table.
          */
         rib->error.type = kRIB_ERRTYPE_ADDING_TO_HASH;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         return kRIB_ERRRC_INT; /* Error */   
      }
      /* Handle existing lighthandle stored in hash table. */
      RibSetDefinedLightHandle( a, sequence, lh );
   }
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiMakeBumpV( char *picturename, char *texturename, 
 *                    RtToken swrap, RtToken twrap,
 *                    RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
 *                    RtInt n, RtToken tokens[], RtPointer parms[] )
 */
STATIC int RibReadMakeBumpV( PRIB_INSTANCE rib ) 
{ 
   unsigned char  params[ s2Cpr + s2T + s1Ff + s2F ];
   RtInt          n = 0;
   RtToken        *tokens;
   RtPointer      *parms;
   auto int       rc;


   memset( params, 0, s2Cpr + s2T  );
   /* scRibMakeBumpV[] = { Cpr, Cpr, T, T, Ff, F, F, V }; */
   rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_MAKEBUMP], 
                                 NULL, params );  
   CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  
   CheckForError(rc);
   ((PRiMakeBumpV)*rib->ritable[kRIB_MAKEBUMP])(
       ChrFromPtrAndInt(params,0),         ChrFromPtrAndInt(params,s1Cpr),
       TknFromPtrAndInt(params,s2Cpr),     TknFromPtrAndInt(params,s2Cpr+s1T),
       (RtFilterFunc)PtrFromPtrAndInt(params,s2Cpr+s2T), 
       FltFromPtrAndInt(params,s2Cpr+s2T+s1Ff),
       FltFromPtrAndInt(params,s2Cpr+s2T+s1Ff+s1F),
       n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( PtrFromPtrAndInt(params,s2Cpr+s1T) );
      _RibFree( PtrFromPtrAndInt(params,s2Cpr) );
      _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
      _RibFree( PtrFromPtrAndInt(params,0) );
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if ( PtrFromPtrAndInt(params,s2Cpr+s1T) )
     _RibFree( PtrFromPtrAndInt(params,s2Cpr+s1T) );
   if ( PtrFromPtrAndInt(params,s2Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s2Cpr) );
   if ( PtrFromPtrAndInt(params,s1Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
   if (PtrFromPtrAndInt(params,0))
     _RibFree( PtrFromPtrAndInt(params,0) );
   
   return rc;
}


/* RtVoid RiMakeCubeFaceEnvironmentV( char *px, char *nx, char *py, char *ny,
 *                                   char *pz, char *nz, char *texturename, 
 *                                   RtFloat fov,
 *                                   RtFilterFunc filterfunc, 
 *                                   RtFloat swidth, RtFloat twidth,
 *                                   RtInt n, 
 *                                   RtToken tokens[], RtPointer parms[] )
 */
STATIC int RibReadMakeCubeFaceEnvironmentV( PRIB_INSTANCE rib ) 
{ 
   unsigned char  params[ s7Cpr + s1Ff + s3F ];
   RtInt          n = 0;
   RtToken        *tokens;
   RtPointer      *parms;
   auto int       rc;


   memset( params, 0, s7Cpr );
   /* scRibMakeCubeFaceEnvironmentV[] = { Cpr, Cpr, Cpr, Cpr, Cpr, Cpr, Cpr, 
    *                                     F, Ff, F, F, V }; 
    */
   rc = RibReadRIBCallParameters( rib,
                                 gRibCParams[kRIB_MAKECUBEFACEENVIRONMENT], 
                                 NULL, params );  
   CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  
   CheckForError(rc);
   ((PRiMakeCubeFaceEnvironmentV)*rib->ritable[kRIB_MAKECUBEFACEENVIRONMENT])(
       ChrFromPtrAndInt(params,0),         ChrFromPtrAndInt(params,s1Cpr),
       ChrFromPtrAndInt(params,s2Cpr),     ChrFromPtrAndInt(params,s3Cpr),
       ChrFromPtrAndInt(params,s4Cpr),     ChrFromPtrAndInt(params,s5Cpr),
       ChrFromPtrAndInt(params,s6Cpr),     FltFromPtrAndInt(params,s7Cpr),
       (RtFilterFunc)PtrFromPtrAndInt(params,s7Cpr+s1F),
       FltFromPtrAndInt(params,s7Cpr+s1F+s1Ff),
       FltFromPtrAndInt(params,s7Cpr+s1F+s1Ff+s1F),
       n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( PtrFromPtrAndInt(params,0) );
      _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
      _RibFree( PtrFromPtrAndInt(params,s2Cpr) );
      _RibFree( PtrFromPtrAndInt(params,s3Cpr) );
      _RibFree( PtrFromPtrAndInt(params,s4Cpr) );
      _RibFree( PtrFromPtrAndInt(params,s5Cpr) );
      _RibFree( PtrFromPtrAndInt(params,s6Cpr) );
   }

   return kRIB_OK; 

 Error:
   if ( PtrFromPtrAndInt(params,0) )
     _RibFree( PtrFromPtrAndInt(params,0) );
   if ( PtrFromPtrAndInt(params,s1Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
   if ( PtrFromPtrAndInt(params,s2Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s2Cpr) );
   if ( PtrFromPtrAndInt(params,s3Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s3Cpr) );
   if ( PtrFromPtrAndInt(params,s4Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s4Cpr) );
   if ( PtrFromPtrAndInt(params,s5Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s5Cpr) );
   if ( PtrFromPtrAndInt(params,s6Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s6Cpr) );
   
   return rc;
}


/* RtVoid RiMakeLatLongEnvironmentV( char *picturename, char *texturename, 
 *                                  RtFilterFunc filterfunc,
 *                                  RtFloat swidth, RtFloat twidth,
 *                                  RtInt n, 
 *                                  RtToken tokens[], RtPointer parms[] )
 */
STATIC int RibReadMakeLatLongEnvironmentV( PRIB_INSTANCE rib ) 
{ 
   unsigned char  params[ s2Cpr + s1Ff + s2F ];
   RtInt          n = 0;
   RtToken        *tokens;
   RtPointer      *parms;
   auto int       rc;


   memset( params, 0, s2Cpr );
   /* scRibMakeLatLongEnvironmentV[] = { Cpr, Cpr, Ff, F, F, V }; */
   rc = RibReadRIBCallParameters( rib, 
                                 gRibCParams[kRIB_MAKELATLONGENVIRONMENT], 
                                 NULL, params );  
   CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  
   CheckForError(rc);
   ((PRiMakeLatLongEnvironmentV)*rib->ritable[kRIB_MAKELATLONGENVIRONMENT])(
       ChrFromPtrAndInt(params,0),         ChrFromPtrAndInt(params,s1Cpr),
       (RtFilterFunc)PtrFromPtrAndInt(params,s2Cpr),    
       FltFromPtrAndInt(params,s2Cpr+s1Ff),
       FltFromPtrAndInt(params,s2Cpr+s1Ff+s1F),
       n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( PtrFromPtrAndInt(params,0) );
      _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
   }

   return kRIB_OK; 

 Error:
   if (PtrFromPtrAndInt(params,0))
     _RibFree( PtrFromPtrAndInt(params,0) );
   if ( PtrFromPtrAndInt(params,s1Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
   
   return rc;
}


/* RtVoid RiMakeShadowV( char *picturename, char *texturename,
 *                      RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadMakeShadowV( PRIB_INSTANCE rib ) 
{ 
   unsigned char  params[ s2Cpr ];
   RtInt          n = 0;
   RtToken        *tokens;
   RtPointer      *parms;
   auto int       rc;


   memset( params, 0, s2Cpr );
   /* scRibMakeShadowV[] = { Cpr, Cpr, V }; */
   rc = RibReadRIBCallParameters( rib, 
                                 gRibCParams[kRIB_MAKESHADOW], 
                                 NULL, params );  
   CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  
   CheckForError(rc);
   ((PRiMakeShadowV)*rib->ritable[kRIB_MAKESHADOW])(
                                         ChrFromPtrAndInt(params,0), 
                                         ChrFromPtrAndInt(params,s1Cpr),
                                         n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( PtrFromPtrAndInt(params,0) );
      _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
   }

   return kRIB_OK; 

 Error:
   if (PtrFromPtrAndInt(params,0))
     _RibFree( PtrFromPtrAndInt(params,0) );
   if ( PtrFromPtrAndInt(params,s1Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
   
   return rc;
}


/* RtVoid RiMakeTextureV( char *picturename, char *texturename, 
 *                       RtToken swrap, RtToken twrap,
 *                       RtFilterFunc filterfunc, 
 *                       RtFloat swidth, RtFloat twidth,
 *                       RtInt n, RtToken tokens[], RtPointer parms[] )
 */
STATIC int RibReadMakeTextureV( PRIB_INSTANCE rib ) 
{
   unsigned char  params[ s2Cpr + s2T + s1Ff + s2F ];
   RtInt          n = 0;
   RtToken        *tokens;
   RtPointer      *parms;
   auto int       rc;


   memset( params, 0, sizeof(params) );
   /* scRibMakeTextureV[] = { Cpr, Cpr, T, T, Ff, F, F, V }; */
   rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_MAKETEXTURE], 
                                 NULL, params );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);
   ((PRiMakeTextureV)*rib->ritable[kRIB_MAKETEXTURE])(
       ChrFromPtrAndInt(params,0),         ChrFromPtrAndInt(params,s1Cpr),
       TknFromPtrAndInt(params,s2Cpr),     TknFromPtrAndInt(params,s2Cpr+s1T),
       (RtFilterFunc)PtrFromPtrAndInt(params,s2Cpr+s2T), 
       FltFromPtrAndInt(params,s2Cpr+s2T+s1Ff),
       FltFromPtrAndInt(params,s2Cpr+s2T+s1Ff+s1F),
       n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( PtrFromPtrAndInt(params,s2Cpr+s1T) );
      _RibFree( PtrFromPtrAndInt(params,s2Cpr) );
      _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
      _RibFree( PtrFromPtrAndInt(params,0) );
   }

   return kRIB_OK; 

 Error:
   if ( PtrFromPtrAndInt(params,s2Cpr+s1T) )
     _RibFree( PtrFromPtrAndInt(params,s2Cpr+s1T) );
   if ( PtrFromPtrAndInt(params,s2Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s2Cpr) );
   if ( PtrFromPtrAndInt(params,s1Cpr) )
     _RibFree( PtrFromPtrAndInt(params,s1Cpr) );
   if (PtrFromPtrAndInt(params,0))
     _RibFree( PtrFromPtrAndInt(params,0) );
   
   return rc;
}


/* RtVoid RiMatte( RtBoolean onoff )
 *
 *
 */
STATIC int RibReadMatte( PRIB_INSTANCE rib ) 
{ 
   RtInt     onoff;
   auto int  rc;


   rc = RibReadInteger( rib, &onoff );  CheckForError(rc);
   /* scRibMatte[] = { Bl, N }; */ 
   ((PRiMatte)*rib->ritable[kRIB_MATTE])( (RtBoolean)onoff );

   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiMotionBeginV( RtInt n, RtFloat times[] )
 *
 *
 */
STATIC int RibReadMotionBeginV( PRIB_INSTANCE rib ) 
{ 
   RtInt     n = 0;
   RtFloat   *times;
   auto int  rc;


   /* When reading from a RIB file the value n is not present.  
    * The value of n is determined by the size of times[].
    */
   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_FLOAT, 
                              (int*)&n, (void**)&times );  
               CheckForError(rc);

   /* scRibMotionBeginV[] = { I, Fa, V }; */ 
   ((PRiMotionBeginV)*rib->ritable[kRIB_MOTIONBEGIN])( n, times );
   if ( RibShouldFreeData(rib) )
   {
      _RibFree( times );
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for "times" if an error occurred. */
   return rc;
}


/* RtVoid RiMotionEnd( void )
 *
 *
 */
STATIC int RibReadMotionEnd( PRIB_INSTANCE rib ) 
{ 
   /* scRibMotionEnd[] = { N }; */
   ((PRiMotionEnd)*rib->ritable[kRIB_MOTIONEND])();

   return kRIB_OK; 
}


/* RtVoid RiNuCurvesV( RtInt ncurves, RtInt nvertices[], RtInt order[],
 *                   RtFloat knot[], RtFloat min[], RtFloat max[], 
 *                   RtInt n, RtToken tokens[], RtPointer parms[] );
 */
STATIC int RibReadNuCurvesV( PRIB_INSTANCE rib ) 
{
   RtInt       nc;
   RtInt       *nv = NULL;
   int         N; /* number of control vertices */
   RtInt       *order = NULL;
   int         O; /* sum of order[] values */
   RtFloat     *knot = NULL;
   RtFloat     *min = NULL;
   RtFloat     *max = NULL;
   RtInt       n = 0;
   RtToken     *tokens;
   RtPointer   *parms;
   auto RtInt  i;
   auto int    rc;
                

   /* When reading from a RIB file the value ncurves is not present.  
    * The value of ncurves is determined by the size of nvertices[].
    */
   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_INTEGER, 
                              (int*)&nc, (void**)&nv );  
                              CheckForError(rc);
   rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED,
			     nc, &order );  CheckForError(rc);
   N = 0;
   O = 0;
   for ( i=0; i<nc; i++ )
   {
      N += nv[i];
      O += order[i];
   }
   /* The number of knots is the number of control vertices plus the order. */
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
				   N+O, &knot );  CheckForError(rc);
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
				   nc, &min );  CheckForError(rc);
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
				   nc, &max );  CheckForError(rc);
   i = N - O + nc;
   rc = RibReadParameterList( rib, kRIB_REQUIRE_P_OR_PW,
                             /* uniform */ i,
                             /* varying */ nc + i,
                             /* vertex  */ N, 
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibNuCurvesV[] = { I, Ia, Ia, Fa, Fa, Fa, V }; */
   ((PRiNuCurvesV)*rib->ritable[kRIB_NUCURVES])( nc,nv,order,knot,min,max,
						 n,tokens,parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(max);
      _RibFree(min);
      _RibFree(knot);
      _RibFree(order);
      _RibFree(nv);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (max)
      _RibFree(max);
   if (min)
      _RibFree(min);
   if (knot)
      _RibFree(knot);
   if (order)
      _RibFree(order);
   if (nv)
      _RibFree(nv);
   return rc;
}


RtVoid RibNuCurvesV( RtInt ncurves, RtInt nvertices[], RtInt order[],
		       RtFloat knot[], RtFloat min[], RtFloat max[], 
		       RtInt n, RtToken tokens[], RtPointer parms[] )
{
   (void)ncurves; (void)nvertices; (void)order; (void)knot; 
   (void)min; (void)max; 
   (void)n; (void)tokens; (void)parms;
   /* Incase a client library does not have RiNuCurvesV, provide this
    *    as an alternative to link with in the gRibRITable[].
    */
   return;
}

/* RtVoid RiNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
 *                    RtFloat umin, RtFloat umax, 
 *                    RtInt nv, RtInt vorder, RtFloat vknot[],
 *                    RtFloat vmin, RtFloat vmax,
 *                    RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadNuPatchV( PRIB_INSTANCE rib ) 
{    
   RtInt      nu, nv, uorder, vorder;
   RtFloat    umin, umax, vmin, vmax;
   RtFloat    *uknot = NULL;
   RtFloat    *vknot = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto RtInt  i,j;
   auto int    rc;
                

   rc = RibReadInteger( rib, &nu );  CheckForError(rc);
   rc = RibReadInteger( rib, &uorder );  CheckForError(rc);

   i = uorder+nu;
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  i, &uknot );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &umin );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &umax );  CheckForError(rc);
   rc = RibReadInteger( rib, &nv );  CheckForError(rc);
   rc = RibReadInteger( rib, &vorder );  CheckForError(rc);

   i = vorder+nv;
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  i, &vknot );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &vmin );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &vmax );  CheckForError(rc);
   i = nu-uorder;
   j = nv-vorder;

   rc = RibReadParameterList( rib, kRIB_REQUIRE_P_OR_PW,
                             /* uniform */ (1+i) * (1+j), 
                             /* varying */ (2+i) * (2+j),
                             /* vertex  */ nu*nv, 
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibNuPatchV[] = { I, I, Fa, F, F, I, I, Fa, F, F, V }; */
   ((PRiNuPatchV)*rib->ritable[kRIB_NUPATCH])(
       nu,uorder,uknot,umin,umax,nv,vorder,vknot,vmin,vmax,n,tokens,parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(vknot);
      _RibFree(uknot);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (vknot)
     _RibFree(vknot);
   if (uknot)
     _RibFree(uknot);
   return rc;
}


/* RtObjectHandle RiObjectBegin( void )
 *
 *
 */
STATIC int RibReadObjectBegin( PRIB_INSTANCE rib ) 
{ 
   RtInt           sequence;
   PRIB_HASHATOM   a;
   RtObjectHandle  oh;
   auto int        rc;


   /* There is a sequence number when reading a RIB binding.  The 
    *    sequence number is not a parameter when calling RiObjectBegin().
    */
   rc = RibReadInteger( rib, &sequence );  CheckForError(rc);

   /* Plug-ins that hook out kRIB_OBJECTBEGIN can use RibGetLastSequence()
    *    to get an ID number for the object being referenced.
    */
   rc = RibSetLastSequence( rib, sequence );  CheckForError(rc);

   /* scRibObjectBegin[] = { [sequencenumber,] N }; */
   oh = ((PRiObjectBegin)*rib->ritable[kRIB_OBJECTBEGIN])();

   if (!oh)
   {
      rib->error.type = kRIB_ERRTYPE_UNDEFINED_OBJECT;
      rib->error.unexpected.user_int = 0; /* Got a bad object handle. */
      RibSetError( rib, RIE_BADHANDLE, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */   
   }
   if (!RibAddItem( rib->phashtable, kRIB_HASH_OBJECTHANDLE, sequence, oh )) 
   {
      a = RibGetConflictingHashAtom( rib->phashtable );
      if (!a)
      {
         /* There is no already existing atom to get in the way, 
          *    so there must have been a problem adding to the hash table.
          */
         rib->error.type = kRIB_ERRTYPE_ADDING_TO_HASH;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         return kRIB_ERRRC_INT; /* Error */   
      }
      /* Handle existing objecthandle stored in hash table. */
      RibSetDefinedObjectHandle( a, sequence, oh );
   }

   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiObjectEnd( void )
 *
 *
 */
STATIC int RibReadObjectEnd( PRIB_INSTANCE rib ) 
{ 
   /* scRibObjectEnd[] = { N }; */
   ((PRiObjectEnd)*rib->ritable[kRIB_OBJECTEND])();

   return kRIB_OK; 
}


/* RtVoid RiObjectInstance( RtObjectHandle handle )
 *
 *
 */
STATIC int RibReadObjectInstance( PRIB_INSTANCE rib ) 
{ 
   RtInt           sequence;
   RtObjectHandle  oh;
   RIB_HASHATOM    *p = NULL;
   auto int        rc;


   rc = RibReadInteger( rib, &sequence );  CheckForError(rc);   
   if ( sequence <= 0 )
   {
      goto ErrorMsg;
   }

   /* Plug-ins that hook out kRIB_OBJECTINSTANCE can use RibGetLastSequence()
    *    to get an ID number for the object being referenced.
    */
   rc = RibSetLastSequence( rib, sequence );  CheckForError(rc);

   p = RibFindMatch( rib->phashtable, kRIB_HASH_OBJECTHANDLE, sequence, NULL );
   if (!p)
   {
      goto ErrorMsg;
   }

   oh = RibGetDefinedObjectHandle( p );

   /* scRibObjectInstance[] = { Oh, N }; */
   ((PRiObjectInstance)*rib->ritable[kRIB_OBJECTINSTANCE])( oh );

   return kRIB_OK; 

 ErrorMsg:

      rib->error.type = kRIB_ERRTYPE_UNDEFINED_OBJECT;
      rib->error.unexpected.user_int = sequence;
      RibSetError( rib, RIE_BADHANDLE, RIE_ERROR, &(rib->error) );
      rc = kRIB_ERRRC_INT; /* Error */   

 Error:

   return rc;
}


/* RtVoid RiOpacity( RtColor color)
 *
 *
 */
STATIC int RibReadOpacity( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p;
   auto int  ncolorsamples;
   auto int  rc;


   /* Remember Colors aren't limited to being an array of
    *    three RtFloats.  ColorSamples can change that.
    */
   ncolorsamples = RibGetNColorSamples(rib);
   if (!ncolorsamples)
   {
      rc = kRIB_ERRRC_INT;
      goto Error;
   }

   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 
                                  ncolorsamples, &p );  CheckForError(rc);

   /* scRibOpacity[] = { C, N }; */
   ((PRiOpacity)*rib->ritable[kRIB_OPACITY])( p );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred,
    *    so no clean-up is needed here. 
    */
   return rc;
}


/* RtVoid RiOptionV( RtToken name, 
 *                  RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadOptionV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   RtPointer  searchpath[1];
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* Handle Option "searchpath" "archive" ["s"].  The default is ["."]. */
   if ( !strcmp(name,"searchpath") )
   {
      /* If needed for checking, define DEBUG_SEARCHPATH for the following: */
#ifdef DEBUG_SEARCHPATH    
      printf( "RibReadOptionV n:%d, tokens[0]:%s parms[0]:%s\n",
	      n,tokens[0],(char*)*(void**)parms[0]);
#endif

      /* Look for "archive" token name. */
      RibGetUserParameters( gRibSearchArchiveTable, 1, n, tokens, parms, 
			    searchpath );
#ifdef DEBUG_SEARCHPATH    
      printf("RibReadOptionV searchpath[0]:%s\n",
	     (char*)*(void**)searchpath[0]);
#endif

      /* If "archive" given then setup a copy of the path list. */
      if ( searchpath[0] )
      {
	 auto  PRIB_INSTANCE  prib = (PRIB_INSTANCE)rib;
	 auto size_t          lpath, lsearchpath, l;
	 auto char            *path;
	 

	 /* Find the maximum path length and the searchpath length. */
	 path = (char*)*(void**)searchpath[0];

	 lpath = 0;
	 lsearchpath = 0;
	 do
	 {
	    l = 0;
	    while (*path!='\0')
	    {
	       if (*path++==':')
		  break;
	       l++;
	    }
	    if ( l>lpath )
	       lpath = l;
	    lsearchpath += l + 1;
	 } while (*path!='\0');

	 if ( prib->archivelmaxpath < lpath )
	 {
	    prib->archivelmaxpath = lpath;
	 }

	 if ( prib->archivesearchpath )
	 {
	    _RibFree(prib->archivesearchpath);
	 }
	 prib->archivesearchpath = (char*)_RibMalloc(lsearchpath);
	 if ( !prib->archivesearchpath )
	    goto Error;

	 memcpy( prib->archivesearchpath, 
		 (char*)*(void**)searchpath[0], lsearchpath );
      }
   }

   /* scRibOptionV[] = { T, V }; */
   ((PRiOptionV)*rib->ritable[kRIB_OPTION])( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiOrientation( RtToken orientation )
 *
 *
 */
STATIC int RibReadOrientation( PRIB_INSTANCE rib ) 
{ 
   char       *orientation;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &orientation ); 
   CheckForError(rc);
   
   /* scRibOrientation[] = { T, N }; */
   ((PRiOrientation)*rib->ritable[kRIB_ORIENTATION])( orientation );
   if ( RibShouldFreeData(rib) )
   {
      _RibFree(orientation);
   }
   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for "orientation" if an error 
    *    occurred. 
    */
   return rc;
}


/* RtVoid RiParaboloidV( RtFloat rmax, 
 *                      RtFloat zmin, RtFloat zmax, RtFloat thetamax,
 *                      RtInt n, RtToken tokens[], RtPointer parms[] )
 */
STATIC int RibReadParaboloidV( PRIB_INSTANCE rib ) 
{ 
   RtFloat    *p = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 4, &p ); 
        CheckForError(rc);

   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 4,
                             /* vertex  */ 4,
                             &n, &tokens, &parms );  CheckForError(rc);
   /* scRibParaboloidV[] = { F, F, F, F, V }; */
   ((PRiParaboloidV)*rib->ritable[kRIB_PARABOLOID])( p[0],p[1],p[2],p[3],
                                                    n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( p );     
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (p)
     _RibFree( p );     
   return rc; 
}


/* RtVoid RiPatchV( RtToken type, 
 *                 RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadPatchV( PRIB_INSTANCE rib ) 
{ 
   RtToken    type = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   int        nvertex;
   char       *p;
   auto int   i;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);

   /* Get type. */
   i = RibReadStringForToken( type, gRibPatchTable, &p );
   if ( i<0 || *p )
   {
      rib->error.type = kRIB_ERRTYPE_BAD_PATCHTYPE;
      rib->error.unexpected.user_string = type;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */   
   }

   if (0==i)   /* bilinear patch */
     nvertex = 4;
   else if (1==i) /* bicubic patch */
     nvertex = 16;
   else        /* oops */
     return kRIB_ERRRC_INT;   

   rc = RibReadParameterList( rib, kRIB_REQUIRE_P_OR_PZ,
                             /* uniform */ 1, 
                             /* varying */ 4,
                             /* vertex  */ nvertex,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibPatchV[] = { T, V }; */
   ((PRiPatchV)*rib->ritable[kRIB_PATCH])( type, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(type);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (type)
     _RibFree(type);
   return rc; 
}


/* RtVoid RiPatchMeshV( RtToken type, RtInt nu, RtToken uwrap, 
 *                     RtInt nv, RtToken vwrap, 
 *                     RtInt n, RtToken tokens[], RtPointer parms[] )
 */
STATIC int RibReadPatchMeshV( PRIB_INSTANCE rib ) 
{ 
   RtToken       type = NULL;
   RtInt         nu;
   RtToken       uwrap = NULL;
   RtInt         nv;
   RtToken       vwrap = NULL;
   int           nupatches = 0, nvpatches = 0;
   int           unowrap, vnowrap;
   int           nvarying;
   int           t,i;
   RtInt         n = 0;
   RtToken       *tokens;
   RtPointer     *parms;
   char          *p;
   auto int      rc;


   /* Check the patch type for either "bilinear" or "bicubic". */
   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);
   t = RibReadStringForToken( type, gRibPatchTable, &p );
   if ( t<0 || *p )
   {
      rib->error.type = kRIB_ERRTYPE_BAD_PATCHTYPE;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */   
   }

   /* Get nu. */
   rc = RibReadInteger( rib, &nu );  CheckForError(rc);   

   /* Get uwrap. */
   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &uwrap );  CheckForError(rc);
   i = RibReadStringForToken( uwrap, gRibPeriodicTable, &p );
   if ( i<0 || *p )
   {
      rib->error.type = kRIB_ERRTYPE_BAD_WRAPMODE;
      rib->error.unexpected.user_string = uwrap;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */   
   }
   if ( i==0 ) /* periodic */
   {
      unowrap = 0;
   }
   else if ( i==1 ) /* nonperiodic */
   {
      unowrap = 1;
   }
   else /* oops */
   {
      rc = 1;
      goto Error;
   }

   /* Get nv. */
   rc = RibReadInteger( rib, &nv );  CheckForError(rc);   

   /* Get vwrap. */
   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &vwrap );  CheckForError(rc);
   i = RibReadStringForToken( vwrap, gRibPeriodicTable, &p );
   if ( i<0 || *p )
   {
      rib->error.type = kRIB_ERRTYPE_BAD_WRAPMODE;
      rib->error.unexpected.user_string = vwrap;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      goto Error;
   }
   if ( i==0 )      /* periodic */
   {
      vnowrap = 0;
   }
   else if ( i==1 ) /* nonperiodic */
   {
      vnowrap = 1;
   }
   else             /* oops */
   {
      rc = 1;
      goto Error;
   }

   if ( 0==t )
   {
      nupatches = nu - unowrap;
      nvpatches = nv - vnowrap;
   }
   else if ( 1==t )
   {
      i = (nu - 4*unowrap);

      /* Note that copies of the old RI Specification [PIXA89] are missing
       *    an addition of 1 for nonperiodic.
       */
      nupatches = i / rib->ustep + unowrap; 

      i = (nv - 4*vnowrap);
      /* Note that copies of the old RI Specification [PIXA89] are missing
       *    an addition of 1 for nonperiodic.
       */
      nvpatches = i / rib->vstep + vnowrap; 

      /* It is safe to have assumed that rc was zero if no error. 
       *   Otherwise one of the previous CheckForError(rc) statements would
       *   have caught it.
       */
      if (rc)
	 goto Error;
   }

   nvarying = (nupatches+unowrap) * (nvpatches+vnowrap);

   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ nupatches*nvpatches, 
                             /* varying */ nvarying,
                             /* vertex  */ nu*nv,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibPatchMeshV[] = { T, I, T, I, T, V }; */
   ((PRiPatchMeshV)*rib->ritable[kRIB_PATCHMESH])( type, nu, uwrap, nv, vwrap,
                                         n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(vwrap);
      _RibFree(uwrap);
      _RibFree(type);
   }

   return kRIB_OK; 

 Error:
   if (vwrap)
     _RibFree(vwrap);
   if (uwrap)
     _RibFree(uwrap);
   if (type)
     _RibFree(type);
   return rc; 
}


/* RtVoid RiPerspective( RtFloat fov )
 *
 *
 */
STATIC int RibReadPerspective( PRIB_INSTANCE rib ) 
{ 
   RtFloat   fov;
   auto int  rc;


   rc = RibReadFloatingPoint( rib, &fov );  CheckForError(rc);

   /* scRibPerspective[] = { F, N }; */
   ((PRiPerspective)*rib->ritable[kRIB_PERSPECTIVE])( fov );
   return kRIB_OK; 

 Error:
   return rc; 
}


/* RtVoid RiPixelFilter( RtFilterFunc filterfunc, 
 *                      RtFloat xwidth, RtFloat ywidth )
 *
 */
STATIC int RibReadPixelFilter( PRIB_INSTANCE rib ) 
{ 
   unsigned char  params[ s1Ff + s2F ];
   auto int       rc;


   /* scRibPixelFilter[] = { Ff, F, F, N }; */
   rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_PIXELFILTER], 
                                 NULL, params );  
   CheckForError(rc);

   ((PRiPixelFilter)*rib->ritable[kRIB_PIXELFILTER])(
                                  (RtFilterFunc)PtrFromPtrAndInt(params,0), 
                                  FltFromPtrAndInt(params,s1Ff),
                                  FltFromPtrAndInt(params,s1Ff+s1F) );
                                                
   /* Don't free PtrFromPtrAndInt(params,0).  It's a pointer to function. */

   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiPixelSamples( RtFloat xsamples, RtFloat ysamples )
 *
 *
 */
STATIC int RibReadPixelSamples( PRIB_INSTANCE rib ) 
{ 
   RtFloat   xsamples, ysamples;
   auto int  rc;


   rc = RibReadFloatingPoint( rib, &xsamples );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &ysamples );  CheckForError(rc);

   /* scRibPixelSamples[] = { F, F, N }; */
   ((PRiPixelSamples)*rib->ritable[kRIB_PIXELSAMPLES])( xsamples, ysamples );

   return kRIB_OK; 

 Error:
   return rc; 
}


/* RtVoid RiPixelVariance( RtFloat variation )
 *
 *
 */
STATIC int RibReadPixelVariance( PRIB_INSTANCE rib ) 
{ 
   RtFloat   variation;
   auto int  rc;


   rc = RibReadFloatingPoint( rib, &variation );  CheckForError(rc);

   /* scRibPixelVariance[] = { F, N }; */
   ((PRiPixelVariance)*rib->ritable[kRIB_PIXELVARIANCE])( variation );
   return kRIB_OK; 

 Error:
   return rc; 
}


/* RtVoid RiPointsV( RtInt npoints, 
 *                  RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadPointsV( PRIB_INSTANCE rib ) 
{ 
   RtInt         n = 0;
   RtToken       *tokens;
   RtPointer     *parms;
   RtInt         npoints;
   unsigned int  *classNtypes;
   unsigned int  *sizes;
   int           problems;
   auto int      i,rc;


   rc = RibReadParameterList( rib, kRIB_REQUIRE_P,
                             /* uniform */ 1, 
                             /* varying */ kRIB_PARAML_UNKNOWN_SIZE,
                             /* vertex  */ kRIB_PARAML_UNKNOWN_SIZE,
                             &n, &tokens, &parms );  CheckForError(rc);

   RibGetVectorCacheInfo( rib, NULL, &npoints, &classNtypes, &sizes );

   /* If "npoints" is zero then no positional array was found. */
   if (!npoints)
   {
      /* Missing "P" data was already reported by RibReadParameterList(),
       *    so just return with an error return code set.  Can't go further
       *    since the npoints is unknown.  A simple return is possible 
       *    since all of the Polygon statement has been read.
       */
      return kRIB_ERRRC_INT;
   }

   problems = 0;
   for ( i=0; i<n; i++ )
   {
      /* kRIB_UNIFORMCLASS was a known length array that 
       * was already checked.  Just check varying and vertex classes.
       */
      switch (kRIB_CLASS_MASK & classNtypes[i])
      {
       case kRIB_VARYINGCLASS:
       case kRIB_VERTEXCLASS:
         if ( sizes[i] != npoints )
         {
            RibErrorNumberOfValues( rib, tokens[i], npoints, sizes[i] ); 
            /* The data is not correct and should be removed from the 
             *    list of token/parameter pairs (inshort the parameter vector).
             */
            problems++;
            /* Mark this as being bad by setting the data size to zero. */
            sizes[i] = 0;
         }
         break;
      }
   }

   if (problems)
   {
      rc = 0;
      i = 0;
      while ( i<n )
      {
         if ( !sizes[i] )
         {
            i++;
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
            _RibFree( tokens[rc] );
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
            _RibFree( parms[rc] );
         }
         if ( rc!=i )
         {
            tokens[rc]      = tokens[i];
            parms[rc]       = parms[i];
            classNtypes[rc] = classNtypes[i];
            sizes[rc]       = sizes[i];
         }
         rc++;
         i++;
      }
      RibCacheVector( rib, -problems, NULL, NULL );
      n -= problems;
   }

   /* Set these values so the match what is used in the parameter list. 
    *    RibReadPolygonV() and RibReadPointsV() are the only two reading
    *    functions that have to do this.  The other reading functions
    *    rely on RibReadParameterList() to set these values.
    */
   rib->lastnuniform = 1;
   rib->lastnvarying = npoints;
   rib->lastnvertex = npoints;

   /* scRibPointsV[] = { V }; */
   ((PRiPointsV)*rib->ritable[kRIB_POINTS])( npoints, n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
   }

   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiPointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], 
 *                            RtInt nvertices[], RtInt vertices[], 
 *                            RtInt n, RtToken tokens[], RtPointer parms[] )
 */
STATIC int RibReadPointsGeneralPolygonsV( PRIB_INSTANCE rib ) 
{ 
   RtInt         npolys;
   RtInt         *nloops = NULL;
   RtInt         *nvertices = NULL;
   RtInt         *vertices = NULL;
   RtInt         n = 0;
   RtToken       *tokens;
   RtPointer     *parms;
   auto int      i, l, v, rc;


   /* When reading from a RIB file the value npolys is not present.  
    * The value of npoly is determined by the size of nloops[].
    */
   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_INTEGER, 
                              (int*)&npolys, (void**)&nloops );  
                              CheckForError(rc);
   l = 0;
   for ( i=0; i< npolys; i++ )
   {
      l += nloops[i];
   }
   rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED,
                            l, &nvertices );  CheckForError(rc);
   v = 0;
   for ( i=0; i< l; i++ )
   {
      v += nvertices[i];
   }
   rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED,
                            v, &vertices );  CheckForError(rc);
   l = vertices[0];
   for ( i=0; i< v; i++ )
   {
      if ( vertices[i] > l )
        l = vertices[i];
   }
   l++;

   rc = RibReadParameterList( rib, kRIB_REQUIRE_P,
                             /* uniform */ npolys, 
                             /* varying */ l,
                             /* vertex  */ l,
                             &n, &tokens, &parms );  
                             CheckForError(rc);

   /* scRibPointsGeneralPolygonsV[] = { I, Ia, Ia, Ia, V } */
   ((PRiPointsGeneralPolygonsV)*rib->ritable[kRIB_POINTSGENERALPOLYGONS])( 
                                  npolys, nloops, nvertices, vertices,
                                  n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(vertices);
      _RibFree(nvertices);
      _RibFree(nloops);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (vertices)
     _RibFree(vertices);
   if (nvertices)
     _RibFree(nvertices);
   if (nloops)
     _RibFree(nloops);
   
   return rc;
}


/* RtVoid RiPointsPolygonsV( RtInt npolys, RtInt nvertices[], RtInt vertices[],
 *                          RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadPointsPolygonsV( PRIB_INSTANCE rib ) 
{ 
   RtInt         npolys;
   RtInt         *nvertices = NULL;
   RtInt         *vertices = NULL;
   RtInt         n = 0;
   RtToken       *tokens;
   RtPointer     *parms;
   auto int      i, v, l, rc;


   /* When reading from a RIB file the value npolys is not present.  
    * The value of npolys is determined by the size of nvertices[].
    */
   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_INTEGER, 
                              (int*)&npolys, (void**)&nvertices );  
                              CheckForError(rc);

   v = 0;
   for ( i=0; i< npolys; i++ )
   {
      v += nvertices[i];
   }
   rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED,
                            v, &vertices );  CheckForError(rc);

   l = vertices[0];
   for ( i=0; i< v; i++ )
   {
      if ( vertices[i] > l )
        l = vertices[i];
   }
   l++;

   rc = RibReadParameterList( rib, kRIB_REQUIRE_P,
                             /* uniform */ npolys,
                             /* varying */ l, 
                             /* vertex  */ l,
                             &n, &tokens, &parms );  
                             CheckForError(rc);

   /* scRibPointsPolygonsV[] = { I, Ia, Ia, V }; */
   ((PRiPointsPolygonsV)*rib->ritable[kRIB_POINTSPOLYGONS])( 
                                        npolys, nvertices, vertices,
                                        n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(vertices);
      _RibFree(nvertices);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (vertices)
     _RibFree(vertices);
   if (nvertices)
     _RibFree(nvertices);
   
   return rc;
}


/* RiPolygonV( RtInt nvertices,
 *            RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadPolygonV( PRIB_INSTANCE rib ) 
{ 
   RtInt         n = 0;
   RtToken       *tokens;
   RtPointer     *parms;
   RtInt         nvertices;
   unsigned int  *classNtypes;
   unsigned int  *sizes;
   int           problems;
   auto int      i,rc;


   rc = RibReadParameterList( rib, kRIB_REQUIRE_P,
                             /* uniform */ 1, 
                             /* varying */ kRIB_PARAML_UNKNOWN_SIZE,
                             /* vertex  */ kRIB_PARAML_UNKNOWN_SIZE,
                             &n, &tokens, &parms );  CheckForError(rc);

   RibGetVectorCacheInfo( rib, NULL, &nvertices, &classNtypes, &sizes );

   /* If "nvertices" is zero then no positional array was found. */
   if (!nvertices)
   {
      /* Missing "P" data was already reported by RibReadParameterList(),
       *    so just return with an error return code set.  Can't go further
       *    since the nvertices is unknown.  A simple return is possible 
       *    since all of the Polygon statement has been read.
       */
      return kRIB_ERRRC_INT;
   }

   problems = 0;
   for ( i=0; i<n; i++ )
   {
      /* kRIB_UNIFORMCLASS was a known length array that 
       * was already checked.  Just check varying and vertex classes.
       */
      switch (kRIB_CLASS_MASK & classNtypes[i])
      {
       case kRIB_VARYINGCLASS:
       case kRIB_VERTEXCLASS:
         if ( sizes[i] != nvertices )
         {
            RibErrorNumberOfValues( rib, tokens[i], nvertices, sizes[i] ); 
            /* The data is not correct and should be removed from the 
             *    list of token/parameter pairs (inshort the parameter vector).
             */
            problems++;
            /* Mark this as being bad by setting the data size to zero. */
            sizes[i] = 0;
         }
         break;
      }
   }

   if (problems)
   {
      rc = 0;
      i = 0;
      while ( i<n )
      {
         if ( !sizes[i] )
         {
            i++;
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
            _RibFree( tokens[rc] );
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
            _RibFree( parms[rc] );
         }
         if ( rc!=i )
         {
            tokens[rc]      = tokens[i];
            parms[rc]       = parms[i];
            classNtypes[rc] = classNtypes[i];
            sizes[rc]       = sizes[i];
         }
         rc++;
         i++;
      }
      RibCacheVector( rib, -problems, NULL, NULL );
      n -= problems;
   }

   /* Set these values so the match what is used in the parameter list. 
    *    RibReadPolygonV() and RibReadPointsV() are the only two reading
    *    functions that have to do this.  The other reading functions
    *    rely on RibReadParameterList() to set these values.
    */
   rib->lastnuniform = 1;
   rib->lastnvarying = nvertices;
   rib->lastnvertex = nvertices;

   /* scRibPolygonV[] = { I, V }; */
   ((PRiPolygonV)*rib->ritable[kRIB_POLYGON])( nvertices, n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
   }

   return kRIB_OK; 

 Error:
   return rc;
}


/* RtVoid RiProcedural( RtPointer data, RtBound bound,
 *                     RtVoid (*subdivfunc)(RtPointer, RtFloat),
 *                     RtVoid (*freefunc)(RtPointer) )
 *
 */
STATIC int RibReadProcedural( PRIB_INSTANCE rib ) 
{ 
   RtToken    type = NULL;
   char       *p;
   int        ndata;
   int        subdivfunc;
   RtString   *data;
   RtFloat    *bound = NULL;
   auto int   i;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);
   i = RibReadStringForToken( type, gRibProceduralTable, &p );
   if ( i<0 || *p )
   {
      rib->error.type = kRIB_ERRTYPE_BAD_PROCEDURALTYPE;
      rib->error.unexpected.user_string = type;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      goto Error;
   }
   switch ( i )
   {
    case 0: /* DelayedReadArchive */
      subdivfunc = kRIB_PROCDELAYEDREADARCHIVE;
      ndata = 1;
      break;
    case 1: /* DynamicLoad */
      subdivfunc = kRIB_PROCDYNAMICLOAD;
      ndata = 2;
      break;
    case 2: /* RunProgram */
      subdivfunc = kRIB_PROCRUNPROGRAM;
      ndata = 2;
      break;
    default:
      rc = 1;
      goto Error;
   }

   rc = RibReadStringArray( rib, kRIB_BRACKETS_REQUIRED, ndata, &data );
   CheckForError(rc);

   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 6, &bound ); 
   CheckForError(rc);

   /* static char scRibProcedural[] = { Pr, B, Fn, Fn, N }; */
   ((PRiProcedural)*rib->ritable[kRIB_PROCEDURAL])( (RtPointer)data, bound,
                                  (PRIB_SUBDIVFUNC)(rib->ritable[subdivfunc]),
                                  (PRIB_FREEFUNC)(RibFree) );
   if ( RibShouldFreeData(rib) )
   {
      _RibFree(bound);
      _RibFree(type);
   }

   return kRIB_OK;

 Error:
   if (bound)
     _RibFree(bound);
   if (type)
     _RibFree(type);
   return rc;
}


/* RtVoid RiProjectionV( RtToken name, 
 *                      RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadProjectionV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibProjectionV[] = { T, V }; */
   ((PRiProjectionV)*rib->ritable[kRIB_PROJECTION])( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiQuantize( RtToken type, RtInt one, 
 *                   RtInt min, RtInt max, RtFloat ampl )
 *
 */
STATIC int RibReadQuantize( PRIB_INSTANCE rib ) 
{ 
   unsigned char  params[ s1T + s3I + s1F ];
   auto int       rc;


   memset( params, 0, s1T );
   /* scRibQuantize[] = { T, I, I, I, F, N }; */
   rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_QUANTIZE], 
                                 NULL, params );  
                                 CheckForError(rc);

   ((PRiQuantize)*rib->ritable[kRIB_QUANTIZE])(
                                  TknFromPtrAndInt(params,0),     
                                  IntFromPtrAndInt(params,s1T),    
                                  IntFromPtrAndInt(params,s1T+s1I),    
                                  IntFromPtrAndInt(params,s1T+s2I),
                                  FltFromPtrAndInt(params,s1T+s3I) );
   if ( RibShouldFreeData(rib) )
   {
      _RibFree( PtrFromPtrAndInt(params,0) );
   }

   return kRIB_OK; 

 Error:
   if ( PtrFromPtrAndInt(params,0) )
     _RibFree( PtrFromPtrAndInt(params,0) );
   return rc;
}


/* RtVoid RiReadArchiveV( RtToken name, 
 *                       RtVoid (*callback)( RtToken, char*, ... ),
 *                       RtInt n, RtToken tokens[], RtPointer parms[] )
 */
STATIC int RibReadReadArchiveV( PRIB_INSTANCE rib ) 
{ 
   char        *filename = NULL;
   RtToken     *tokens;
   RtPointer   *parms;
   RtInt       n = 0;
   RIB_HANDLE  subfile;
   auto int    rc;


   /* The C argument callback is left as a NULL when the ReadArchive()
    *    function listed in the IP table is called. 
    */

   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &filename );  
   CheckForError(rc);

   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   if ( rib->options & kRIB_OPTION_READ_SUBFILES )
   {
      subfile = RibOpenSubfile( rib, filename );
      if (subfile)
      {
         RibRead( subfile );
         RibClose( subfile );
      }
      else
      {
         rib->error.type = kRIB_ERRTYPE_OPENING_FAILED;
         rib->error.unexpected.user_string = filename;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         goto Error;
      }
   }
   else
   {
      /* scRibReadArchiveV[] = { T, [Fn,] V }; */
      ((PRiReadArchiveV)*rib->ritable[kRIB_READARCHIVE])( filename, NULL,
                                                         n, tokens, parms );
   }
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(filename);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (filename)
     _RibFree(filename);
   return rc;
}


/* RtVoid RiRelativeDetail( RtFloat relativedetail )
 *
 *
 */
STATIC int RibReadRelativeDetail( PRIB_INSTANCE rib ) 
{ 
   RtFloat   relativedetail;
   auto int  rc;


   rc = RibReadFloatingPoint( rib, &relativedetail );  CheckForError(rc);

   /* scRibRelativeDetail[] = { F, N }; */
   ((PRiRelativeDetail)*rib->ritable[kRIB_RELATIVEDETAIL])( relativedetail );

   return kRIB_OK; 

 Error:
   return rc;  
}


/* RtVoid RiResourceV( RtToken handle, RtToken type,
 *                     RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadResourceV( PRIB_INSTANCE rib )
{ 
   RtToken    handle = NULL;
   RtToken    type = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &handle );  CheckForError(rc);
   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibResourceV[] = { T, T, V }; */
   ((PRiResourceV)*rib->ritable[kRIB_RESOURCE])( handle, type,
						 n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(type);
      _RibFree(handle);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (type)
     _RibFree(type);
   if (handle)
     _RibFree(handle);
   return rc;
}


/* RtVoid RiReverseOrientation( void )
 *
 *
 */
STATIC int RibReadReverseOrientation( PRIB_INSTANCE rib ) 
{ 
   /* scRibReverseOrientation[] = { N }; */
   ((PRiReverseOrientation)*rib->ritable[kRIB_REVERSEORIENTATION])();

   return kRIB_OK; 
}


/* RtVoid RiRotate( RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz )
 *
 *
 */
STATIC int RibReadRotate( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p;
   auto int  rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 4, &p ); 
   CheckForError(rc);
   /* scRibRotate[] = { F, F, F, F, N }; */
   ((PRiRotate)*rib->ritable[kRIB_ROTATE])( p[0],p[1],p[2],p[3] );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );     
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc; 
}


/* RtVoid RiScale( RtFloat dx, RtFloat dy, RtFloat dz )
 *
 *
 */
STATIC int RibReadScale( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p;
   auto int  rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 3, &p ); 
   CheckForError(rc);
   /* scRibScale[] = { F, F, F, N }; */
   ((PRiScale)*rib->ritable[kRIB_SCALE])( p[0],p[1],p[2] );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc; 
}


/* RtVoid RiScreenWindow( RtFloat left, RtFloat right, 
 *                       RtFloat bottom, RtFloat top )
 *
 */
STATIC int RibReadScreenWindow( PRIB_INSTANCE rib ) 
{ 
   RtFloat  *p;
   auto int  rc;
   

   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 4, &p ); 
   CheckForError(rc);
   /* scRibScreenWindow[] = { F, F, F, F, N }; */ 
   ((PRiScreenWindow)*rib->ritable[kRIB_SCREENWINDOW])( p[0],p[1],p[2],p[3] );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );     
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc;
}


/* RtVoid RiShadingInterpolation( RtToken type )
 *
 *
 */
STATIC int RibReadShadingInterpolation( PRIB_INSTANCE rib ) 
{ 
   RtToken   type;
   auto int  rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &type );  CheckForError(rc);

   /* scRibShadingInterpolation[] = { T, N }; */
   ((PRiShadingInterpolation)*rib->ritable[kRIB_SHADINGINTERPOLATION])(type);

   if ( RibShouldFreeData(rib) )
   {
      _RibFree(type);
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for "type" if an error occurred. */
   return rc;  
}


/* RtVoid RiShadingRate( RtFloat size )
 *
 *
 */
STATIC int RibReadShadingRate( PRIB_INSTANCE rib ) 
{ 
   RtFloat   size;
   auto int  rc;


   rc = RibReadFloatingPoint( rib, &size );  CheckForError(rc);

   /* scRibShadingRate[] = { F, N }; */
   ((PRiShadingRate)*rib->ritable[kRIB_SHADINGRATE])( size );

   return kRIB_OK; 

 Error:
   return rc;  
}


/* RtVoid RiShutter( RtFloat min, RtFloat max )
 *
 *
 */
STATIC int RibReadShutter( PRIB_INSTANCE rib ) 
{ 
   RtFloat   min;
   RtFloat   max;
   auto int  rc;


   rc = RibReadFloatingPoint( rib, &min );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &max );  CheckForError(rc);

   /* scRibShutter[] = { F, F, N }; */
   ((PRiShutter)*rib->ritable[kRIB_SHUTTER])( min, max );

   return kRIB_OK; 

 Error:
   return rc; 
}


/* RtVoid RiSides( RtInt sides )
 *
 *
 */
STATIC int RibReadSides( PRIB_INSTANCE rib ) 
{ 
   RtInt     sides;
   auto int  rc;


   rc = RibReadInteger( rib, &sides );  CheckForError(rc);   

   /* scRibSides[] = { I, N }; */
   ((PRiSides)*rib->ritable[kRIB_SIDES])( sides );

   return kRIB_OK; 

 Error:
   return rc; 
}


/* RtVoid RiSkew( RtFloat angle, RtFloat dx1, RtFloat dy1, RtFloat dz1,
 *               RtFloat dx2, RtFloat dy2, RtFloat dz2 )
 *
 */
STATIC int RibReadSkew( PRIB_INSTANCE rib ) 
{ 
   unsigned char params[ s7Fa ];
   auto int  rc;


   /* scRibSkew[] = { F, F, F, F, F, F, F, N }; */
   rc = RibReadRIBCallParameters( rib, gRibCParams[kRIB_SKEW], 
                                 NULL, params );  CheckForError(rc);
   ((PRiSkew)*rib->ritable[kRIB_SKEW])( FltFromPtrAndInt(params,0),
                                      FltFromPtrAndInt(params,s1Fa),
                                      FltFromPtrAndInt(params,s2Fa),
                                      FltFromPtrAndInt(params,s3Fa),
                                      FltFromPtrAndInt(params,s4Fa),
                                      FltFromPtrAndInt(params,s5Fa),
                                      FltFromPtrAndInt(params,s6Fa) );
   return kRIB_OK; 

 Error:
   return rc; 
}


/* RtVoid RiSolidBegin( RtToken operation )
 *
 *
 */
STATIC int RibReadSolidBegin( PRIB_INSTANCE rib ) 
{ 
   RtToken  operation;
   auto int   rc;


   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &operation ); 
   CheckForError(rc);

   /* scRibSolidBegin[] = { T, N }; */
   ((PRiSolidBegin)*rib->ritable[kRIB_SOLIDBEGIN])( operation );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree(operation);
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for "operation" if an error 
    *    occurred. 
    */
   return rc;
}


/* RtVoid RiSolidEnd( void )
 *
 *
 */
STATIC int RibReadSolidEnd( PRIB_INSTANCE rib ) 
{ 
   /* scRibSolidEnd[] = { N }; */
   ((PRiSolidEnd)*rib->ritable[kRIB_SOLIDEND])();

   return kRIB_OK; 
}


/* RtVoid RiSphereV( RtFloat radius, RtFloat zmin, RtFloat zmax, 
 *                  RtFloat thetamax,
 *                  RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadSphereV( PRIB_INSTANCE rib ) 
{ 
   RtFloat    *p = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 4, &p ); 
   CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 4,
                             /* vertex  */ 4,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibSphereV[] = { F, F, F, F, V }; */
   ((PRiSphereV)*rib->ritable[kRIB_SPHERE])( p[0],p[1],p[2],p[3],
                                            n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( p );     
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (p)
     _RibFree( p );     
   return rc;
}


/* RtVoid RiSubdivisionMeshV( RtToken scheme, RtInt nfaces, 
 *                         RtInt nvertices[], RtInt vertices[],
 *		           RtInt ntags, RtToken tags[],
 *		           RtInt nargs[], RtInt intargs[], RtFloat floatargs[],
 *		           RtInt n, RtToken tokens[], RtPointer parms[] );
 */
STATIC int RibReadSubdivisionMeshV( PRIB_INSTANCE rib ) 
{ 
   RtToken    scheme = NULL;
   RtInt      nfaces;
   RtInt      *nvertices = NULL; 
   RtInt      *vertices = NULL;
   RtInt      ntags;
   RtToken    *tags = NULL;
   RtInt      *nargs = NULL;
   RtInt      *intargs = NULL;
   RtFloat    *floatargs = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   i, v, l, ia, fa, c;
   auto int   rc;

   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &scheme );  CheckForError(rc);

   /* When reading from a RIB file the value nfaces is not present.  
    * The value of nfaces is determined by the size of nvertices[].
    */
   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_INTEGER, 
                              (int*)&nfaces, (void**)&nvertices );  
                              CheckForError(rc);

   v = 0;
   for ( i=0; i < nfaces; i++ )
   {
      v += nvertices[i];
   }
   rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED,
                            v, &vertices );  CheckForError(rc);
   l = vertices[0];
   for ( i=0; i < v; i++ )
   {
      if ( vertices[i] > l )
        l = vertices[i];
   }
   l++;

   /* The arrays after vertices[] can be absent in a SubdivisionMesh 
    *    statement.  When they are missing the number of tags is zero
    *    and also the parameter list immediately follows
    *    with atleast the "P" data.  
    * Assuming a valid SubdivisionMesh with "P" data, check for the quote
    *    that starts the token name in the parameter list.  If there
    *    is a quote, then there are no tags with matching argument data.
    */
   if ( !RibIsaStringNext(rib) )
   {
      /* When reading from a RIB file the value ntags is not present.  
       * The value of ntags is determined by the size of tags[].
       */
      rc = RibReadArrayAndLength( rib, 
				  kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_STRING, 
				  (int*)&ntags, (void**)&tags );  
                                  CheckForError(rc);
				  
      rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED,
				2*ntags, &nargs );  
                                CheckForError(rc);

      /* Pixar uses RIB examples that included zero valued arrays of
       *    length one for intargs and floatargs when nargs was [0 0].  
       *    Also catrib 3.8 allows intargs and floatargs to have any
       *    number of values excede nargs without complaint.
       * So use RibReadArrayAndLength() for intargs and floatargs
       *    instead of RibReadIntegerArray() and RibReadFloatingPointArray().
       */
      ia = 0;
      fa = 0;
      for ( i=0; i < 2*ntags; i+=2 )
      {
	 ia += nargs[i];
	 fa += nargs[i+1];
      }      

      if ( ia )
      {
	 rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED,
				   ia, &intargs );  CheckForError(rc);
      }
      else
      {
	 /* Clear out white spaces and any RI or string definitions. */
	 c = RibHandlePrefix( rib );
	 if (c==EOF)
	    goto Error;
	 RibUngetChar( rib, c );

	 /* Read junk arrays if there. */
	 if (c=='[')
	 {
	    rc = RibReadArrayAndLength( rib, 
					kRIB_BRACKETS_REQUIRED 
					| kRIB_ARRAY_INTEGER, 
					NULL, NULL );  CheckForError(rc);
	 }
      }

      if ( ia )
      {
	 rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED,
					 fa, &floatargs );  CheckForError(rc);
      }
      else
      {
	 /* Clear out white spaces and any RI or string definitions. */
	 c = RibHandlePrefix( rib );
	 if (c==EOF)
	    goto Error;
	 RibUngetChar( rib, c );

	 /* Read junk arrays if there. */
	 if (c=='[')
	 {
	    rc = RibReadArrayAndLength( rib, 
					kRIB_BRACKETS_REQUIRED 
					| kRIB_ARRAY_FLOAT, 
					NULL, NULL );  CheckForError(rc);
	 }
      }

   }
   else
   {
      ntags = 0;
   }

   rc = RibReadParameterList( rib, kRIB_REQUIRE_P,
                             /* uniform */ nfaces,
                             /* varying */ l, 
                             /* vertex  */ l,
                             &n, &tokens, &parms );  
                             CheckForError(rc);

   /* scRibSubdivisionMeshV[] = { T, I, Ia, Ia, I, Ta, Ia, Ia, Fa, V }; */
   ((PRiSubdivisionMeshV)*rib->ritable[kRIB_SUBDIVISIONMESH])( scheme, nfaces, 
						nvertices, vertices,
						ntags, tags,
						nargs, intargs, floatargs,
						n, tokens, parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      if (ntags)
      {
	 _RibFree(floatargs);
	 _RibFree(intargs);
	 _RibFree(nargs);
	 _RibFree(tags);
      }
      _RibFree(vertices);
      _RibFree(nvertices);
      _RibFree(scheme);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (floatargs)
      _RibFree(floatargs);
   if (intargs)
      _RibFree(intargs);
   if (nargs)
      _RibFree(nargs);
   if (tags)
      _RibFree(tags);
   if (vertices)
      _RibFree(vertices);
   if (nvertices)
      _RibFree(nvertices);
   if (scheme)
      _RibFree(scheme);
   
   return rc;
}


/* RtVoid RiSurfaceV( RtToken name, 
 *                   RtInt n, RtToken tokens[], RtPointer parms[] )
 *
 */
STATIC int RibReadSurfaceV( PRIB_INSTANCE rib ) 
{ 
   RtToken    name = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;

   rc = RibReadString( rib, kRIB_BRACKETS_NONE, &name );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 1,
                             /* vertex  */ 1,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibSurfaceV[] = { T, V }; */
   ((PRiSurfaceV)*rib->ritable[kRIB_SURFACE])( name, n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(name);
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (name)
     _RibFree(name);
   return rc;
}


/* RtVoid RiTextureCoordinates( RtFloat s1, RtFloat t1, 
 *                             RtFloat s2, RtFloat t2,
 *                             RtFloat s3, RtFloat t3, 
 *                             RtFloat s4, RtFloat t4 )
 */
STATIC int RibReadTextureCoordinates( PRIB_INSTANCE rib ) 
{ 
   RtFloat    *p;
   auto int   rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 8,
                                  &p );  CheckForError(rc);
   /* scRibTextureCoordinates[] = { F, F, F, F, F, F, F, F, N }; */ 
   ((PRiTextureCoordinates)*rib->ritable[kRIB_TEXTURECOORDINATES])( 
                                    p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7] );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );     
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc;
}


/* RtVoid RiTorusV( RtFloat majorradius, RtFloat minorradius, 
 *                 RtFloat phimin, RtFloat phimax, RtFloat thetamax, 
 *                 RtInt n, RtToken tokens[], RtPointer parms[] )
 */
STATIC int RibReadTorusV( PRIB_INSTANCE rib ) 
{ 
   RtFloat    *p = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto int   rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 5,
                                  &p );  CheckForError(rc);
   rc = RibReadParameterList( rib, kRIB_REQUIRE_NOTHING,
                             /* uniform */ 1, 
                             /* varying */ 4,
                             /* vertex  */ 4,
                             &n, &tokens, &parms );  CheckForError(rc);
   /* scRibTorusV[] = { F, F, F, F, F, V }; */
   ((PRiTorusV)*rib->ritable[kRIB_TORUS])( p[0],p[1],p[2],p[3],p[4], 
                                          n, tokens, parms );

   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree( p );     
   }

   return kRIB_OK; 

 Error:
   RibFreeVectorData( rib, n, tokens, parms );
   if (p)
     _RibFree(p);
   return rc;
}


/* RtVoid RiTransform( RtMatrix transform )
 *
 *
 */
STATIC int RibReadTransform( PRIB_INSTANCE rib ) 
{ 
   void      *p;
   auto int  rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  sizeof(RtMatrix)/sizeof(RtFloat), 
                                  (RtFloat**)&p );  CheckForError(rc);

   /* scRibTransform[] = { M, N }; */
   ((PRiTransform)*rib->ritable[kRIB_TRANSFORM])( (RtFloat(*)[4])p );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc;
}


/* RtVoid RiTransformBegin( void )
 *
 *
 */
STATIC int RibReadTransformBegin( PRIB_INSTANCE rib ) 
{ 
   /* scRibTransformBegin[] = { N }; */
   ((PRiTransformBegin)*rib->ritable[kRIB_TRANSFORMBEGIN])();

   return kRIB_OK; 
}


/* RtVoid RiTransformEnd( void )
 *
 *
 */
STATIC int RibReadTransformEnd( PRIB_INSTANCE rib ) 
{ 
   /* scRibTransformEnd[] = { N }; */
   ((PRiTransformEnd)*rib->ritable[kRIB_TRANSFORMEND])();

   return kRIB_OK; 
}


/* RtVoid RiTranslate( RtFloat dx, RtFloat dy, RtFloat dz )
 *
 *
 */
STATIC int RibReadTranslate( PRIB_INSTANCE rib ) 
{ 
   RtFloat   *p;
   auto int  rc;


   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 3, &p ); 
   CheckForError(rc);
   /* scRibTranslate[] = { F, F, F, N }; */
   ((PRiTranslate)*rib->ritable[kRIB_TRANSLATE])( p[0],p[1],p[2] );

   if ( RibShouldFreeData(rib) )
   {
      _RibFree( p );     
   }

   return kRIB_OK; 

 Error:
   /* Nothing would have been allocated for p if an error occurred. */
   return rc;
}


/* RtVoid RiTrimCurve( RtInt nloops, RtInt ncurves[], RtInt order[], 
 *                    RtFloat knot[], RtFloat min[], RtFloat max[], 
 *                    RtInt n[], RtFloat u[], RtFloat v[], RtFloat w[] )
 *
 * Refer to [UPST90] pages 249 to 250 for information on the sizes 
 *    of the arrays used in RiTrimCurve().
 */
STATIC int RibReadTrimCurve( PRIB_INSTANCE rib ) 
{
   int       C; /* number of curves */
   int       nknots; /* number of knots */
   int       ncvs; /* number of control vertices */
   int       N; /* number of control vertices */
   RtInt     nloops;
   RtInt     *ncurves = NULL;
   RtInt     *order = NULL;
   RtFloat   *knot = NULL;
   RtFloat   *min = NULL;
   RtFloat   *max = NULL;
   RtInt     *n = NULL; 
   RtFloat   *u = NULL; 
   RtFloat   *v = NULL; 
   RtFloat   *w = NULL;
   auto int  rc;
   auto int  i;
                

   /* When reading from a RIB file the value nloops is not present.  
    * The value of nloops is determined by the size of ncurves.
    */
   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_INTEGER, 
                              (int*)&nloops, (void**)&ncurves );  
                              CheckForError(rc);
   C = 0;
   for ( i=0; i<nloops; i++ )
   {
      C += ncurves[i];
   }
   rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED, 
                            C, &order );  CheckForError(rc);
   rc = RibReadArrayAndLength( rib, 
                              kRIB_BRACKETS_REQUIRED | kRIB_ARRAY_FLOAT, 
                              (int*)&nknots, (void**)&knot );  
                              CheckForError(rc);
   ncvs = nknots;
   for ( i=0; i<C; i++ )
   {
      ncvs -= order[i];
   }
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  C, &min );  CheckForError(rc);
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  C, &max );  CheckForError(rc);
   rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED, 
                            C, &n );  CheckForError(rc);
   N = 0;
   for ( i=0; i<C; i++ )
   {
      N += n[i];
   }
   if (ncvs<=0 || N<=0 || N!=ncvs )
   {
      rib->error.type = kRIB_ERRTYPE_TRIMCURVE_NCVS;
      rib->error.expected.sys_int = ncvs;
      rib->error.unexpected.user_int = N;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */         
   }
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  ncvs, &u );  CheckForError(rc);
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  ncvs, &v );  CheckForError(rc);
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  ncvs, &w );  CheckForError(rc);

   /* scRibTrimCurve[] = { I, Ia, Ia, Fa, Fa, Fa, Ia, Fa, Fa, Fa, N }; */
   ((PRiTrimCurve)*rib->ritable[kRIB_TRIMCURVE])( nloops, ncurves,
                                                order, knot, min, max,
                                                n, u, v, w );
   if ( RibShouldFreeData(rib) )
   {
      _RibFree(w);
      _RibFree(v); 
      _RibFree(u); 
      _RibFree(n); 
      _RibFree(max);
      _RibFree(min);
      _RibFree(knot);
      _RibFree(order);
      _RibFree(ncurves);
   }

   return kRIB_OK; 

 Error:
   if (w)
     _RibFree(w);
   if (v)
     _RibFree(v); 
   if (u)
     _RibFree(u); 
   if (n)
     _RibFree(n); 
   if (max)
     _RibFree(max);
   if (min)
     _RibFree(min);
   if (knot)
     _RibFree(knot);
   if (order)
     _RibFree(order);
   if (ncurves)
     _RibFree(ncurves);
   
   return rc;
}


/* RiWorldBegin( void )
 *
 */
STATIC int RibReadWorldBegin( PRIB_INSTANCE rib ) 
{ 
   if ( RibGetStatus(rib) & kRIB_STATUS_WORLDBLOCK )
   {
      /* A WorldBegin was given inside a WorldBegin/End block, so give
       *    an error message and then try to just read through the rest
       *    of the statement.
       */
      rib->error.type = kRIB_ERRTYPE_BAD_WORLDBEGIN;
      RibSetError( rib, RIE_NESTING, RIE_ERROR, &(rib->error) );

      /* Continue reading even though an error was given. */
   }
   /* Mark that reader has entered a WorldBegin/End block. */
   RibSetStatus( rib, kRIB_STATUS_WORLDBLOCK, kRIB_STATUS_WORLDBLOCK );

   /* scRibWorldBegin[] = { N }; */
   ((PRiWorldBegin)*rib->ritable[kRIB_WORLDBEGIN])();

   return kRIB_OK; 
}


/* RiWorldEnd( void )
 *
 */
STATIC int RibReadWorldEnd( PRIB_INSTANCE rib ) 
{ 
   if ( !(RibGetStatus(rib) & kRIB_STATUS_WORLDBLOCK) )
   {
      /* A WorldEnd was given without a matching WorldBegin, so give
       *    an error message and then try to just read through the rest
       *    of the statement.
       */
      rib->error.type = kRIB_ERRTYPE_BAD_WORLDEND;
      RibSetError( rib, RIE_NESTING, RIE_ERROR, &(rib->error) );

      /* Continue reading even though an error was given. */
   }
   /* Mark that reader has left a FrameBegin/End block. */
   RibSetStatus( rib, kRIB_STATUS_WORLDBLOCK, kRIB_STATUS_CLEAR );

   /* scRibWorldEnd[] = { N }; */
   ((PRiWorldEnd)*rib->ritable[kRIB_WORLDEND])();

   return kRIB_OK; 
}


STATIC int RibRead_Version( PRIB_INSTANCE rib )
{
   float  version;


   RibReadFloatingPoint( (PRIB_INSTANCE)rib, &version );
   if ( version > kRIB_SUPPORTED_VERSION )
   {
      rib->error.type = kRIB_ERRTYPE_VERSION_TOO_NEW;
      rib->error.var = NULL;
      rib->error.expected.sys_float = kRIB_SUPPORTED_VERSION;
      rib->error.unexpected.user_float = version;
      RibSetError( rib, RIE_VERSION, RIE_ERROR, &(rib->error) );
   }

   return kRIB_OK;
}


int RibReadVer35Option( RIB_HANDLE hrib ) 
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;

   if ( rib )
   {
      gRibReadTable[kRIB_NUPATCH] = RibReadVer35NuPatchV;
      rib->options |= kRIB_OPTION_NO_INLINE;
      return kRIB_OK;
   }

   /* else return error */
   return kRIB_ERRRC_INT;
}


int RibReadVer36Option( RIB_HANDLE hrib ) 
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;

   if ( rib )
   {
      gRibReadTable[kRIB_NUPATCH] = RibReadNuPatchV;
      rib->options |= kRIB_OPTION_NO_INLINE;
      return kRIB_OK;
   }

   /* else return error */
   return kRIB_ERRRC_INT;
}


int RibReadVer37Option( RIB_HANDLE hrib ) 
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;

   if ( rib )
   {
      rib->options |= kRIB_OPTION_NO_INLINE;
      return kRIB_OK;
   }

   /* else return error */
   return kRIB_ERRRC_INT;
}


STATIC int RibReadVer35NuPatchV( PRIB_INSTANCE rib ) 
{ 
   RtInt      nu, nv, uorder, vorder;
   RtFloat    umin, umax, vmin, vmax;
   RtFloat    *uknot = NULL;
   RtFloat    *vknot = NULL;
   RtInt      n = 0;
   RtToken    *tokens;
   RtPointer  *parms;
   auto RtInt  i;
   auto int    rc;
                

   rc = RibReadInteger( rib, &nu );  CheckForError(rc);
   rc = RibReadInteger( rib, &uorder );  CheckForError(rc);

   i = uorder+nu;
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  i, &uknot );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &umin );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &umax );  CheckForError(rc);
   rc = RibReadInteger( rib, &nv );  CheckForError(rc);
   rc = RibReadInteger( rib, &vorder );  CheckForError(rc);

   i = vorder+nv;
   rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  i, &vknot );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &vmin );  CheckForError(rc);
   rc = RibReadFloatingPoint( rib, &vmax );  CheckForError(rc);

   rc = RibReadParameterList( rib, kRIB_REQUIRE_P_OR_PW,
                             /* uniform */ 1, 
                             /* varying */ 4,
                             /* vertex  */ 4,
                             &n, &tokens, &parms );  CheckForError(rc);

   /* scRibNuPatchV[] = { I, I, Fa, F, F, I, I, Fa, F, F, V }; */
   ((PRiNuPatchV)*rib->ritable[kRIB_NUPATCH])(
       nu,uorder,uknot,umin,umax,nv,vorder,vknot,vmin,vmax,n,tokens,parms );
   if ( RibShouldFreeData(rib) )
   {
      RibFreeVectorData( rib, n, tokens, parms );
      _RibFree(uknot);
      _RibFree(vknot);
   }

   return kRIB_OK; 

 Error:
    RibFreeVectorData( rib, n, tokens, parms );
   if (uknot)
     _RibFree(uknot);
   if (vknot)
     _RibFree(vknot);
   return rc;
}


int RibFreeStepStack( RIB_HANDLE hrib )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   auto PRIB_UVSTEPS   p, pp;


   p = rib->stepstack;
   while (p)
   {
      pp = p;
      p = p->next;
      _RibFree(pp);
   }  
   rib->stepstack = NULL;    

   if ( rib )
   {
      p = rib->stepstack;
      while (p)
      {
	 pp = p;
	 p = p->next;
	 _RibFree(pp);
      }  
      rib->stepstack = NULL;    
      
      return kRIB_OK;
   }

   /* else return error */
   return kRIB_ERRRC_INT;
}


int RibFreeInLineList( RIB_HANDLE hrib )
{
   auto PRIB_INSTANCE    rib = (PRIB_INSTANCE)hrib;
   auto PRIB_INLINEDECL  p, pp;


   p = rib->inlinelist;
   while (p)
   {
      pp = p;
      p = p->next;
      _RibFree(pp);
   }  
   rib->inlinelist = NULL;    

   if ( rib )
   {
      p = rib->inlinelist;
      while (p)
      {
	 pp = p;
	 p = p->next;
	 _RibFree(pp);
      }  
      rib->inlinelist = NULL;    
      
      return kRIB_OK;
   }

   /* else return error */
   return kRIB_ERRRC_INT;
}

