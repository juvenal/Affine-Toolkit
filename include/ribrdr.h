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
 * FILE:  ribrdr.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          Structures used for the RIB Reader library.
 *
 *    References:
 *          [PIXA89]  Pixar Animation Studios, The RenderMan Interface, 
 *                    Version 3.1, Richmond, CA, September 1989.
 *          [UPST90]  Upstill, Steve, The RenderMan Companion: A Programmer's
 *                    Guide to Realistic Computer Graphics, Addison Wesley, 
 *                    1990, pp. 56-58.
 *
 *    History:
 *         ?-?-96  Created.  Need to look at archives for date.
 *       07-11-97  Added error code for bad curve type given.
 *       07-14-97  Added gzip support with a few cleanup 
 *                    items left to-do.
 *       12-15-97  Incremented version number.
 *       01-03-98  Incremented version number.
 *       01-05-98  Added an error message for ASCII integers followed by
 *                 a decimal point.  Before the next RibRead* function would
 *                 would print a rather cryptic message.
 *       01-07-98  Added kRIB_ERRTYPE_BAD_BASISUSTEP and 
 *                 kRIB_ERRTYPE_BAD_BASISUSTEP error codes. 
 *       01-08-98  Added arcreclist and arcreclistend to RIB_INSTANCE to fix 
 *                 "comment migration" problem.
 *                 Also added a status flag kRIB_STATUS_PARAML and
 *                 proto-type RibArcRecPlayBack().
 *       03-28-98  Added RibFreeArcRecPlayBack().
 *       03-30-98  Added kRIB_OPTION_INIT_RI_FIRST option to handle client
 *                 libraries that require their RiBegin() functions to be
 *                 called to initialize their RI_ values.
 *       04-04-98  Added proper Attribute block scoping for Basis statements.
 *                 Refer to "4.3. RIB Client Library Known Problems and Bugs" 
 *                 in PRMan 3.7 HTML documentation which states that the RIB
 *                 Client Library ignores scope of Basis statements.  
 *       04-05-98  Incremented version number.
 *       04-06-98  Added kRIB_STATUS_ARRAY so RibReadInteger() and 
 *                 RibReadFloatingPoint() could avoid printing a syntax error
 *                 before their corresponding RibRead...Array() functions
 *                 could.
 *       04-22-98  Added savedoptions.ncolorsamples to RIB_INSTANCE because
 *                 the number of color samples is an option that is to be
 *                 saved when FrameBegin is encountered and restored at a
 *                 matching FrameEnd.  Refer to [PIXA89] page 16 which
 *                 states the FrameEnd retores all options and refer to 
 *                 [UPST90] page 57 which lists ColorSamples as an option.
 *       04-25-98  Added kRIB_STATUS_FRAMEBLOCK, kRIB_STATUS_WORLDBLOCK,
 *                 kRIB_ERRTYPE_BAD_FRAMEBEGIN, kRIB_ERRTYPE_BAD_FRAMEEND, 
 *                 kRIB_ERRTYPE_BAD_WORLDBEGIN, kRIB_ERRTYPE_BAD_WORLDEND,
 *                 and kRIB_ERRTYPE_BAD_COLORSAMPLES.  All these flags
 *                 were added to detect and handle errors when ColorSamples 
 *                 statements are included in Frame and World blocks.
 *                 Refer to [UPST90] page 57 listing ColorSamples as an
 *                 option which have certain limits on valid context for
 *                 being called.
 *                 Added the calls RibGetLastNUniform() RibSetLastNUniform(),
 *                 RibGetLastNVarying(), RibSetLastNVarying(), 
 *                 RibGetLastNVertex(), RibSetLastNVertex() and
 *                 RibGetParameterTypeInfo().  These calls allow functions
 *                 to query the number of values that appear in a parameter
 *                 array rather than calculate the number itself.
 *       10-12-98  Added kRIB_SUBDIVISIONMESH.
 *       12-31-98  Added inline declaration support.
 *       04-27-99  Added kRIB_NUCURVES and kRIB_IMPLICIT.
 *       06-05-99  Changed RibGetTypeClassNames() to RibGetClassTypeNames()
 *                 and also the returned strings "float pair" and "int pair"
 *                 to strings "float[2]" and "int[2]" respectively.  These
 *                 strings can be plugged directly into a Declaration 
 *                 statement making them more useful.  Added a macro to
 *                 ribrdr.h to cover for the missing RibGetTypeClassNames()
 *                 function.
 *       07-15-99  Incremented version number.
 *       10-05-99  Added RibFindFile() and archivesearchpath fields 
 *                 to PRIB_INSTANCE.
 *       10-17-99  Changed hint table Set/Get functions to match those
 *                 for the Ri tables.  Added rib->hintlevel, kRIB_RILEVEL99,
 *                 and kRIB_HINTLEVEL99.
 *       02-24-00  Added RibNuCurvesV and RibImplicitV to allow linking with 
 *                 client libraries such as librib.a which do not have the
 *                 Ri functions.  Also RibGeometricRepresentation.
 *    
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 */
#ifndef __RIBRDR_HEADER__
#define __RIBRDR_HEADER__

#ifndef FILE
#include <stdio.h>
#endif

#ifndef NULL
#include <stdlib.h>
#endif

#ifndef __RIBHASH_HEADER__
#include <ribhash.h>
#endif

#if defined(RIB_ZLIB) && !defined(ZLIB_VERSION)
#include <zlib.h>
#endif

#define kRIB_SUPPORTED_VERSION    3.03
#define kRIB_AFFINE_VERSIONINT    0x3C120000
#define kRIB_AFFINE_VERSIONSTR    "3C12.0"

enum RIB_RICALLS {
   kRIB_AREALIGHTSOURCE,
   kRIB_ATMOSPHERE,
   kRIB_ATTRIBUTE,
   kRIB_ATTRIBUTEBEGIN,
   kRIB_ATTRIBUTEEND,
   kRIB_BASIS,
   kRIB_BOUND,
   kRIB_CLIPPING,
   kRIB_COLOR,
   kRIB_COLORSAMPLES,
   kRIB_CONCATTRANSFORM,
   kRIB_CONE,
   kRIB_COORDINATESYSTEM,
   kRIB_COORDSYSTRANSFORM,
   kRIB_CROPWINDOW,
   kRIB_CURVES,
   kRIB_CYLINDER,
   kRIB_DECLARE,
   kRIB_DEFORMATION,
   kRIB_DEPTHOFFIELD,
   kRIB_DETAIL,
   kRIB_DETAILRANGE,
   kRIB_DISK,
   kRIB_DISPLACEMENT,
   kRIB_DISPLAY,
   kRIB_ERRORHANDLER,
   kRIB_EXPOSURE,
   kRIB_EXTERIOR,
   kRIB_FORMAT,
   kRIB_FRAMEASPECTRATIO,
   kRIB_FRAMEBEGIN,
   kRIB_FRAMEEND,
   kRIB_GENERALPOLYGON,
   kRIB_GEOMETRICAPPROXIMATION,
   kRIB_GEOMETRICREPRESENTATION, /* bmrt */
   kRIB_GEOMETRY,
   kRIB_HIDER,
   kRIB_HYPERBOLOID,
   kRIB_IDENTITY,
   kRIB_ILLUMINATE,
   kRIB_IMAGER,
   kRIB_IMPLICIT, /* PrMan 3.8 */
   kRIB_INTERIOR,
   kRIB_LIGHTSOURCE,
   kRIB_MAKEBUMP,
   kRIB_MAKECUBEFACEENVIRONMENT,
   kRIB_MAKELATLONGENVIRONMENT,
   kRIB_MAKESHADOW,
   kRIB_MAKETEXTURE,
   kRIB_MATTE,
   kRIB_MOTIONBEGIN,
   kRIB_MOTIONEND,
   kRIB_NUCURVES, /* RenderDotC */
   kRIB_NUPATCH,
   kRIB_OBJECTBEGIN,
   kRIB_OBJECTEND,
   kRIB_OBJECTINSTANCE,
   kRIB_OPACITY,
   kRIB_OPTION,
   kRIB_ORIENTATION,
   kRIB_PARABOLOID,
   kRIB_PATCH,
   kRIB_PATCHMESH,
   kRIB_PERSPECTIVE,
   /* kRIB_PIXELFIDELITY, *//* Replaced with RiPixelVariance.*/
   kRIB_PIXELFILTER,
   kRIB_PIXELSAMPLES,
   kRIB_PIXELVARIANCE,
   kRIB_POINTS,
   kRIB_POINTSGENERALPOLYGONS,
   kRIB_POINTSPOLYGONS,
   kRIB_POLYGON,
   kRIB_PROCEDURAL,
   kRIB_PROJECTION,
   kRIB_QUANTIZE,
   kRIB_READARCHIVE, /* 3.6c */
   kRIB_RELATIVEDETAIL,
   kRIB_RESOURCE,
   kRIB_REVERSEORIENTATION,
   kRIB_ROTATE,
   kRIB_SCALE,
   kRIB_SCREENWINDOW,
   kRIB_SHADINGINTERPOLATION,
   kRIB_SHADINGRATE,
   kRIB_SHUTTER,
   kRIB_SIDES,
   kRIB_SKEW,
   kRIB_SOLIDBEGIN,
   kRIB_SOLIDEND,
   kRIB_SPHERE,
   kRIB_SUBDIVISIONMESH,
   kRIB_SURFACE,
   kRIB_TEXTURECOORDINATES,
   kRIB_TORUS,
   kRIB_TRANSFORM,
   kRIB_TRANSFORMBEGIN,
   kRIB_TRANSFORMEND,
   kRIB_TRANSLATE,
   kRIB_TRIMCURVE,
   kRIB_WORLDBEGIN,
   kRIB_WORLDEND,

   /* Don't use kRIB_VERSION (added an extra underscore in case there
    *    ever is an RiVersion() call added to the standard.
    * The RiVersion() call would then get the kRIB_VERSION macro.
    */
   kRIB__VERSION, /* see RtVoid Rib_Version( RtFloat version ); */

   /* The following calls have no RIB bindings: */
   kRIB_BEGIN,
   kRIB_END,
   kRIB_ARCHIVERECORD, /* NOTE: # and ## are kind'a RIB bindings for this. */
   /*kRIB_PROCEDURAL, (3.7 changed) Procedural RIB binding has been added.*/
   kRIB_TRANSFORMPOINTS,
   /* Error Ri Calls */
   kRIB_ERRORABORT,
   kRIB_ERRORIGNORE,
   kRIB_ERRORPRINT,
   /* Pixel Filter Ri Calls */
   kRIB_BOXFILTER,
   kRIB_CATMULLROMFILTER,
   kRIB_GAUSSIANFILTER,
   kRIB_SINCFILTER,
   kRIB_TRIANGLEFILTER,
   kRIB_PROCDELAYEDREADARCHIVE,
   kRIB_PROCDYNAMICLOAD,
   kRIB_PROCRUNPROGRAM,
   kRIB_LAST_RI
};
/* Last call with an actual RIB binding. */
#define kRIB_LAST_RIB (kRIB_ARCHIVERECORD+1)

/* The number of Ri function pointers in the Ri Table as of 99 release. */
#define kRIB_RILEVEL99  116

enum {
   kRIB_CAMERAORIENTATION_HINT,  /* ##CameraOrientation */
   kRIB_CAPABILITIESNEEDED_HINT, /* ##CapabilitiesNeeded */
   kRIB_CREATIONDATE_HINT,       /* ##CreationDate */
   kRIB_CREATOR_HINT,            /* ##Creator */
   kRIB_FOR_HINT,                /* ##For */
   kRIB_FRAMES_HINT,             /* ##Frames */
   kRIB_INCLUDE_HINT,            /* ##Include */   
   kRIB_RENDERMAN_HINT,          /* ##RenderMan */
   kRIB_SCENE_HINT,              /* ##Scene */
   kRIB_SHADERS_HINT,            /* ##Shaders */
   kRIB_TEXTURES_HINT,           /* ##Textures */
   kRIB_LAST_HINT
};

/* The number of hint function pointers in the Hint Table as of 99 release. */
#define kRIB_HINTLEVEL99  11

/*   
 *   Error Handling
 */
enum {
   kRIB_ERRTYPE_NONE,
   kRIB_ERRTYPE_MISSING_P,
   kRIB_ERRTYPE_MISSING_P_AND_PZ,
   kRIB_ERRTYPE_MISSING_P_AND_PW,

   kRIB_ERRTYPE_UNDEFINED_TOKEN,  /* user_int has binary token character. */
   kRIB_ERRTYPE_UNDEFINED_LIGHT,  /* user_int has the bad handle. */
   kRIB_ERRTYPE_UNDEFINED_OBJECT, /* user_int has the bad handle. */
   kRIB_ERRTYPE_UNDEFINED_RI_CODE,/* user_int has the bad RI code. */
   kRIB_ERRTYPE_UNDEFINED_REQUEST,/* user_string has the bad RI request. */
   kRIB_ERRTYPE_UNDEFINED_STRING, /* user_string points to var string. */ 

   kRIB_ERRTYPE_OPENING_FAILED,  /* Couldn't open the file. */
   kRIB_ERRTYPE_UNEXPECTED_EOF,
   kRIB_ERRTYPE_SYNTAX, /* Syntax error, but will try to continue. */
   kRIB_ERRTYPE_NUMBER_OF_VALUES,/* refer to sys_int and user_int. */
   kRIB_ERRTYPE_OPEN_BRACKET, /* Missing open bracket for array. */
   kRIB_ERRTYPE_CLOSE_BRACKET, /* Missing close bracket for array. */
   kRIB_ERRTYPE_BAD_ENCODED_INT, 
   kRIB_ERRTYPE_BAD_ENCODED_FLOAT, 
   /*   The following kRIB_ERRTYPE_BAD_* codes require user_string to point 
    *   to the offending string. 
    */
   kRIB_ERRTYPE_BAD_DECLARATION,
   kRIB_ERRTYPE_BAD_CLASSNAME,
   kRIB_ERRTYPE_BAD_TYPENAME,
   kRIB_ERRTYPE_BAD_RIREQUEST, 
   kRIB_ERRTYPE_BAD_PATCHTYPE,
   kRIB_ERRTYPE_BAD_CURVETYPE,
   kRIB_ERRTYPE_BAD_PROCEDURALTYPE,
   kRIB_ERRTYPE_BAD_BASISNAME,
   kRIB_ERRTYPE_BAD_BASISUSTEP,
   kRIB_ERRTYPE_BAD_BASISVSTEP,
   kRIB_ERRTYPE_BAD_WRAPMODE,
   kRIB_ERRTYPE_BAD_FILTERNAME,
   kRIB_ERRTYPE_BAD_ERRORHANDLER,
   kRIB_ERRTYPE_BAD_FRAMEBEGIN,
   kRIB_ERRTYPE_BAD_FRAMEEND,
   kRIB_ERRTYPE_BAD_WORLDBEGIN,
   kRIB_ERRTYPE_BAD_WORLDEND,
   kRIB_ERRTYPE_BAD_COLORSAMPLES,

   /* kRIB_ERRTYPE_SYSTEM_PARAM:
    *    RIB_ERROR.var points to RIB_HASHATOM associated with given parameter
    *                  before any modifications are made.
    *    RIB_ERROR.unexpected.user_int is the new class and type code.
    */
   kRIB_ERRTYPE_SYSTEM_PARAM,
   kRIB_ERRTYPE_UNDECLARED_PARAM, /* user_string points to param name. */ 

   /* kRIB_ERRTYPE_INCONSISTENT_PARAM:
    *    RIB_ERROR.var points to RIB_HASHATOM associated with given parameter.
    */
   kRIB_ERRTYPE_INCONSISTENT_PARAM,
   kRIB_ERRTYPE_ENCODED_ARRAY_SIZE,
   kRIB_ERRTYPE_FLOAT_ARRAY_SIZE,
   kRIB_ERRTYPE_INTEGER_ARRAY_SIZE,
   kRIB_ERRTYPE_STRING_ARRAY_SIZE,
   kRIB_ERRTYPE_REDEF_SYS_VAR,  /* A var such as "P" is being redeclared. */
   kRIB_ERRTYPE_VERSION_TOO_NEW,/* sys_float and user_float have the ver #s */
   kRIB_ERRTYPE_N_COLORSAMPLES, /* The n in RiColorSamples was <= 0. */
   kRIB_ERRTYPE_ADDING_TO_HASH, /* Couldn't add to hash table (mem error?).*/
   kRIB_ERRTYPE_DECIMATEDINT, /* Decimal point followed an ASCII int. */
   kRIB_ERRTYPE_TRIMCURVE_NCVS,
   kRIB_ERRTYPE_UNSUPPORTED_RIB,
   kRIB_ERRTYPE_UNKNOWN_INTERNAL,
   kRIB_ERRTYPE_LAST,
   kRIB_ERRTYPE_USER = 1024
};
#define kRIB_ERRTYPE_FIRST_MISSING  kRIB_ERRTYPE_MISSING_P
#define kRIB_ERRTYPE_LAST_MISSING   kRIB_ERRTYPE_MISSING_P_AND_PW
#define kRIB_ERRTYPE_FIRST_UNDEF    kRIB_ERRTYPE_UNDEFINED_TOKEN
#define kRIB_ERRTYPE_LAST_UNDEF     kRIB_ERRTYPE_UNDEFINED_STRING
#define kRIB_ERRTYPE_FIRST_MISC     kRIB_ERRTYPE_OPENING_FAILED
#define kRIB_ERRTYPE_LAST_MISC      (kRIB_ERRTYPE_LAST-1)


typedef struct _RIB_ERROR {
   int  type;
   PRIB_HASHATOM  var;
   union {
      int    sys_int;
      float  sys_float;
      char   *sys_string;
   } expected;
   union {
      int    user_int;
      float  user_float;
      char   *user_string;
      void   *user_data;
   } unexpected;
} RIB_ERROR;
typedef RIB_ERROR  *PRIB_ERROR;

/* Increment vector cache by a minimum number.  A size of 30 seemed
 *   to be a good minimum size to incrment the size of the cache.
 */
#define kRIB_VECTORCACHEINCR  30


/* Make buffer size evenly divisible by the size of a RtInt. */
#define kRIB_BUFFER_MAX_B_SIZE  (sizeof(RtInt)*64)
typedef struct _RIB_BUFFERITEM {
   char                    b[kRIB_BUFFER_MAX_B_SIZE];
   struct _RIB_BUFFERITEM  *next;
   struct _RIB_BUFFERITEM  *prev;
} RIB_BUFFERITEM;
typedef RIB_BUFFERITEM  *PRIB_BUFFERITEM;

typedef struct _RIB_BUFFER {
   char             *pchar;     /* points to byte for next char    */
   size_t           buflength;
   PRIB_BUFFERITEM  pbufhere;  /* present buf struct              */
   PRIB_BUFFERITEM  pbufavail; /* available bufferitem structures */
   PRIB_BUFFERITEM  plist;
} RIB_BUFFER;
typedef RIB_BUFFER  *PRIB_BUFFER;

typedef struct _RIB_ARCREC {
   char                *type;
   char                *record;
   struct _RIB_ARCREC  *next;
} RIB_ARCREC;
typedef RIB_ARCREC  *PRIB_ARCREC;

typedef struct _RIB_UVSTEPS {
   RtInt                ustep;
   RtInt                vstep;
   RtInt                blocklevel;
   struct _RIB_UVSTEPS  *next;
} RIB_UVSTEPS;
typedef RIB_UVSTEPS  *PRIB_UVSTEPS;


typedef struct _RIB_INLINEDECL {
   unsigned int            code;
   unsigned int            n;
   PRIB_HASHATOM           atom;
   struct _RIB_INLINEDECL  *next;
} RIB_INLINEDECL;
typedef RIB_INLINEDECL  *PRIB_INLINEDECL;


/* Options are saved and then restored when dealing with FrameBegin/End. 
 *    For code that only deals with with syntax, ncolorsamples is the
 *    the only option that needs its state saved and restored when dealing
 *    with frames.  But here is a structure to place anymore additions in.
 */
typedef struct _RIB_SAVEDOPTIONS {
   int                    ncolorsamples;
} RIB_SAVEDOPTIONS;
typedef RIB_SAVEDOPTIONS  *PRIB_SAVEDOPTIONS;

typedef void         *RIB_HANDLE;  /* rib */
typedef void*        (*PRIB_RIPROC)();
typedef RtVoid       (*PRIB_SUBDIVFUNC)(RtPointer, RtFloat);
typedef RtVoid       (*PRIB_FREEFUNC)(RtPointer);
typedef PRIB_RIPROC  *PRIB_RITABLE;

typedef int (*PRIB_ARCRECFILTERPROC)( RIB_HANDLE rib );
typedef PRIB_ARCRECFILTERPROC  *PRIB_HINTTABLE;
typedef int (*PRIB_ERRORFILTERPROC)( RIB_HANDLE rib, int code, int severity,
                                    PRIB_ERROR error );

/* Set rib->status to zero using kRIB_STATUS_CLEAR. */
#define  kRIB_STATUS_CLEAR         0x00000000
#define  kRIB_STATUS_CLEARMASK     0xffffffff

/* Indicates the RiBegin() call was made for the RIB file.  RibBegin() 
 *    checks this to make certain RiBegin() calls are not made more 
 *    than once.
 *
 * Note:  The differences in the prefixes of RibBegin() and RiBegin().
 *    RibBegin() is function in the RIB Reader library.  RiBegin() is 
 *    the standard RenderMan Interface call.
 */
#define  kRIB_STATUS_BEGUN         0x00000001

/* Mark file as having a subfile open because a ReadArchive statement or 
 *    ##Include hint (if support was added) was encountered in the RIB file.
 */
#define  kRIB_STATUS_SUBFILEOPEN   0x00000002

/* Mark file as using a buffer, hash table and caches that are from another
 *    RIB instance and should not be deleted when RibClose() is called. 
 */
#define  kRIB_STATUS_USINGSHARED   0x00000004

/* Mark RIB file as using a hash table not created during RibOpen(). */
#define  kRIB_STATUS_EXTERNHASHTBL 0x00000008

/* Mark file as being temporarily closed so a file descriptor could be
 *    be made available. 
 */
#define  kRIB_STATUS_CLOSED        0x00000010

/* Mark file as using the binary encoded format. */
#define  kRIB_STATUS_ENCODEDDATA   0x00000020

/* Have each character read by RibGetChar() stored into a buffer.  The
 *    buffer can then be used to create strings or arrays.
 */
#define  kRIB_STATUS_SAVETOBUFFER  0x00000040

/* After the RIB reader has called a Ri...() call do not free the data 
 *    represented by the pointers given in the Ri...() call's C parameter
 *    list.  Refer to RibKeepData() and RibShouldFreeData().
 */
#define  kRIB_STATUS_KEEP_DATA     0x00000080

/* The RiEnd() call was made for the RIB file.  RibEnd() checks this
 *    to make certain RiEnd() calls are not made more than once.
 *
 * Note:  The differences in the prefixes of RibEnd() and RiEnd().
 *    RibEnd() is function in the RIB Reader library.  RiEnd() is 
 *    the standard RenderMan Interface call.
 */
#define  kRIB_STATUS_ENDED         0x00000100

/* Zip compressed file is being read. */
#define  kRIB_STATUS_COMPRESSED    0x00000200

/* RIB is being read from stdin. */
#define  kRIB_STATUS_STDIN         0x00000400

/* Presently reading a parameter list. */
#define  kRIB_STATUS_PARAML        0x00000800

/* Presently reading an integer, float or string array. */
#define  kRIB_STATUS_ARRAY         0x00001000

/* Presently in a FrameBegin/End block, so error on finding ColorSamples. */
#define  kRIB_STATUS_FRAMEBLOCK    0x00002000

/* Presently in a WorldBegin/End block, so error on finding ColorSamples. */
#define  kRIB_STATUS_WORLDBLOCK    0x00004000

#define  kRIB_OPTION_READ_SUBFILES   0x00000001
#define  kRIB_OPTION_ERRMSG_VERBOSE  0x00000002

/* Initialize hash table after client library's RiBegin() is called. */
#define  kRIB_OPTION_INIT_RI_FIRST   0x00000004

/* Do not allow in-line parameter declarations. */
#define  kRIB_OPTION_NO_INLINE       0x00000008

/* Create extra Declare's for each in-line parameter declaration. */
#define  kRIB_OPTION_EXPAND_INLINE   0x00000010

/* An assumed max file length, extra checks are done to expand buffer
 * sizes if needed.
 */
#define kRIB_MAXFILELENGTH  1024

/* TO-DO:  Change code so the structure RIB_INSTANCE size doesn't 
 *         change depending on the flag RIB_ZLIB.
 */
typedef struct _RIB_INSTANCE {
   RIB_UINT32             status;
   RIB_UINT32             options;
#ifdef RIB_ZLIB
   void                   *fp;
   int                    ungetc;
   size_t                 fpoffset;   
#else
   FILE                   *fp;
#endif
   char                   *filename;
   struct _RIB_INSTANCE   *parent;
   size_t                 linecount;
   size_t                 byteoffset;
   int                    ncolorsamples;
   RIB_SAVEDOPTIONS       savedoptions;
   int                    ustep;
   int                    vstep;
   PRIB_UVSTEPS           stepstack;
   int                    blocklevel;
   int                    lastsequence;
   PRIB_BUFFER            pbuf;      /* list of buf structs */
   PRIB_HASHTABLE         phashtable;
   int                    rilevel;
   PRIB_RITABLE           ritable;
   int                    hintlevel;
   PRIB_HINTTABLE         hinttable;
   PRIB_ARCRECFILTERPROC  arcrechandler;
   PRIB_ARCREC            arcreclist;
   PRIB_ARCREC            arcreclistend;
   PRIB_ERRORFILTERPROC   errorhandler;
   RIB_ERROR              error;
   int                    errorcode;
   int                    errorseverity;
   int                    lastnuniform;
   int                    lastnvarying;
   int                    lastnvertex;
   unsigned int           vectorcache_n;    
   unsigned int           vectorcache_nused;
   unsigned int           nvertices;
   void                   *tokencache;
   void                   *paramcache;
   void                   *typecache;
   void                   *ncache;
   PRIB_INLINEDECL        inlinelist; /* In-line parameter declarations */
   size_t                 archivelmaxpath;
   size_t                 archivelnamebuffer;
   char                   *archivenamebuffer;
   char                   *archivesearchpath;
} RIB_INSTANCE;
typedef RIB_INSTANCE  *PRIB_INSTANCE;

typedef int (*PRIB_READPROC)( PRIB_INSTANCE );

typedef struct _RIB_RTSTRING {
   RtString  pstr;
   char      str[1];
} RIB_RTSTRING;
typedef RIB_RTSTRING  *PRIB_RTSTRING;

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


/* 
 *   Prototypes 
 *
 */

#ifndef __RIB_FILE_VERSION
extern  
#endif
unsigned int RibGetLibraryVersion( void );

#ifndef __RIB_FILE_VERSION
extern  
#endif
char *RibGetLibraryVersionName( void );

#ifndef  __RIB_FILE_RIBTBL
extern
#endif
int RibGetRIFromString( char *s );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
unsigned int RibFindFile( PRIB_INSTANCE prib, char *filename );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
RIB_HANDLE RibOpen( char *filename, int ri_level, PRIB_RITABLE table );

/* The following two calls are internal and are exported for testing. */
#ifndef __RIB_FILE_STREAMIO
extern  
#endif
int 
   RibTempClose( PRIB_INSTANCE prib ),
   RibReopen( PRIB_INSTANCE prib );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
RIB_HANDLE RibOpenWithHashTable( char *filename, PRIB_HASHTABLE hashtable );

/* The following is for recursive opening such as for ##include hints. */
#ifndef __RIB_FILE_STREAMIO
extern  
#endif
RIB_HANDLE RibOpenSubfile( RIB_HANDLE rib, char *filename );

#ifndef __RIB_FILE_READRIB
extern  
#endif
int RibRead( RIB_HANDLE rib );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
int 
   RibClose( RIB_HANDLE rib ),
   RibCloseFile( RIB_HANDLE rib ),
   RibSetMaximumFiles( RIB_HANDLE rib, int n ),
   RibSetStatus( RIB_HANDLE hrib, RIB_UINT32 mask, RIB_UINT32 flags ),
   RibSetOptions( RIB_HANDLE hrib, RIB_UINT32 mask, RIB_UINT32 flags ),
   RibSetUVStep( RIB_HANDLE hrib, RtInt ustep, RtInt vstep ),
   RibGetUVStep( RIB_HANDLE hrib, RtInt *ustep, RtInt *vstep ),
   RibBegin( RIB_HANDLE hrib, RtToken name ),
   RibEnd( RIB_HANDLE hrib ),
   RibOption( RIB_HANDLE hrib, RtToken name, RtToken token, RtPointer param );


#ifndef __RIB_FILE_STREAMIO
extern  
#endif
RIB_UINT32 
   RibGetStatus( RIB_HANDLE hrib ),
   RibGetOptions( RIB_HANDLE hrib );

#define /* int */ RibGetMaximumFiles( /* RIB_HANDLE */ rib ) \
                          ( (rib) ? gRibMaxFiles : 0 ) 

#define /* int */ RibGetNumberOfSubfiles( /* RIB_HANDLE */ rib ) \
                          ( (rib) ? gRibNSubfiles : 0 ) 

#define /* int */ RibGetNumberOfOpenSubfiles( /* RIB_HANDLE */ rib ) \
                          ( (rib) ? gRibNOpenSubfiles : 0 ) 

#define /* int */ RibGetRITableSize( /* RIB_HANDLE */ rib )  \
			     ( (rib) ? ((PRIB_INSTANCE)(rib))->rilevel : 0 )

#define /* PRIB_RITABLE */ RibGetRITable( /* RIB_HANDLE */ rib ) \
                          ( (rib) ? ((PRIB_INSTANCE)(rib))->ritable : NULL ) 

#define /*int*/ RibSetRITable( /*RIB_HANDLE*/ rib,       \
			       /*int*/ ri_level,         \
			       /*PRIB_RITABLE*/ table )  \
     ( (rib)&&(table)                                    \
      ? ((((PRIB_INSTANCE)(rib))->ritable=(table)),      \
	 (((PRIB_INSTANCE)(rib))->rilevel=(ri_level)),   \
	 NULL)                                           \
      : 1 /* Error */ ) 

#define RibGetHintTableSize( /* RIB_HANDLE */ rib )  \
			     ( (rib) ? ((PRIB_INSTANCE)(rib))->hintlevel : 0 )

#define /* PRIB_HINTTABLE */ RibGetHintTable( /* RIB_HANDLE */ rib ) \
     ( (rib) ? ((PRIB_INSTANCE)(rib))->hinttable : NULL ) 

#define /* int */ RibSetHintTable( /* RIB_HANDLE */ rib,        \
				   /*int*/ hint_level,          \
                                  /* PRIB_HINTTABLE */ table )  \
     ( (rib)&&(table)                                           \
      ? ((((PRIB_INSTANCE)(rib))->hinttable=(table)),           \
	 (((PRIB_INSTANCE)(rib))->hintlevel=(hint_level)),      \
	 NULL)                                                  \
      : 1 /* Error */ ) 

#define /* RtBoolean */ RibShouldFreeData( /* RIB_HANDLE */ rib )          \
     ( (rib)                                                               \
      && (((PRIB_INSTANCE)(rib))->status & kRIB_STATUS_KEEP_DATA)          \
      ? (((PRIB_INSTANCE)(rib))->status &= ~kRIB_STATUS_KEEP_DATA),RI_FALSE\
      : RI_TRUE )

/* Same as RibShouldFreeData(), but does not remove the kRIB_STATUS_KEEP_DATA
 *    from the rib->status.
 */
#define /* RtBoolean */ RibQueryKeepData( /* RIB_HANDLE */ rib )           \
     ( (rib)                                                               \
      && (((PRIB_INSTANCE)(rib))->status & kRIB_STATUS_KEEP_DATA)          \
      ? RI_FALSE : RI_TRUE )

#define /* int */ RibKeepData( /* RIB_HANDLE */ rib ) \
     ( (rib) ? ((PRIB_INSTANCE)(rib))->status |= kRIB_STATUS_KEEP_DATA,0 : 1 )

#define /* PRIB_HINTTABLE */ RibGetArcRecHandler( /* RIB_HANDLE */ rib ) \
     ( (rib) ? ((PRIB_INSTANCE)(rib))->arcrechandler : NULL ) 

#define /* int */ RibGetNColorSamples( /* RIB_HANDLE */ rib ) \
     ( (rib) ? (((PRIB_INSTANCE)(rib))->ncolorsamples) : 0 ) 

#define /* int */ RibSetNColorSamples( /* RIB_HANDLE */ rib, /*int*/n ) \
     ((rib) ? (((((PRIB_INSTANCE)(rib))->ncolorsamples)=(n)),0)       \
            : kRIB_ERRRC_INT) 

#define /* int */ RibGetLastSequence( /* RIB_HANDLE */ rib ) \
     ( (rib) ? (((PRIB_INSTANCE)(rib))->lastsequence) : 0 ) 

#define /* int */ RibSetLastSequence( /* RIB_HANDLE */ rib, /*int*/n )  \
     ((rib) ? (((PRIB_INSTANCE)(rib))->lastsequence)=(n),0 \
            : kRIB_ERRRC_INT) 

#define /* int */ RibGetLastNUniform( /* RIB_HANDLE */ rib ) \
     ( (rib) ? (((PRIB_INSTANCE)(rib))->lastuniform) : 0 ) 

#define /* int */ RibSetLastNUniform( /* RIB_HANDLE */ rib, /*int*/n )  \
     ((rib) ? (((PRIB_INSTANCE)(rib))->lastuniform)=(n),0 \
            : kRIB_ERRRC_INT) 

#define /* int */ RibGetLastNVarying( /* RIB_HANDLE */ rib ) \
     ( (rib) ? (((PRIB_INSTANCE)(rib))->lastvarying) : 0 ) 

#define /* int */ RibSetLastNVarying( /* RIB_HANDLE */ rib, /*int*/n )  \
     ((rib) ? (((PRIB_INSTANCE)(rib))->lastvarying)=(n),0 \
            : kRIB_ERRRC_INT) 

#define /* int */ RibGetLastNVertex( /* RIB_HANDLE */ rib ) \
     ( (rib) ? (((PRIB_INSTANCE)(rib))->lastvertex) : 0 ) 

#define /* int */ RibSetLastNVertex( /* RIB_HANDLE */ rib, /*int*/n )  \
     ((rib) ? (((PRIB_INSTANCE)(rib))->lastvertex)=(n),0 \
            : kRIB_ERRRC_INT) 

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
PRIB_BUFFER RibCreateBuffer( RIB_HANDLE rib );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
PRIB_BUFFERITEM RibGetBufferItem( PRIB_BUFFER pBuffer );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
int
   RibResetBuffer( RIB_HANDLE rib ),
   /* RibSaveToBuffer calls RibResetBuffer() and sets some flags. */
   RibSaveToBuffer( RIB_HANDLE rib, int flag );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
char *RibCreateStringFromBuffer( RIB_HANDLE rib, RIB_UINT32 options );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
int RibCreateStringsFromBuffer( RIB_HANDLE hrib, RtInt n, RtString **r );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
RtInt *RibCreateIntegerArrayFromBuffer( RIB_HANDLE rib, RIB_UINT32 options );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
RtFloat *RibCreateFloatArrayFromBuffer( RIB_HANDLE rib, RIB_UINT32 options );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
int RibDestroyBuffer( PRIB_BUFFER pbuf );

#ifndef __RIB_FILE_STREAMIO
extern  
#endif
int
   RibGetChar( RIB_HANDLE rib ),
   RibPutChar( RIB_HANDLE  rib, int c ),
   RibUngetChar( RIB_HANDLE rib, int c ),
   RibBufferRead( RIB_HANDLE rib, char *buffer, unsigned int size ),
   RibGetLastChar( RIB_HANDLE hrib ),
   RibIgnoreLastChar( RIB_HANDLE  rib );

#define /* unsigned int */ RibGetFileName( /* RIB_HANDLE */ rib )   \
          ( (rib) ? (((PRIB_INSTANCE)(rib))->filename) : 0L ) 

#define /* RIB_HASHHND */ RibGetHashHandle( /* RIB_HANDLE */ rib )   \
          ( (rib) ? ((PRIB_INSTANCE)(rib))->phashtable : NULL ) 

#define /* unsigned int */ RibGetLineCount( /* RIB_HANDLE */ rib )   \
          ( (rib) ? ((PRIB_INSTANCE)(rib))->linecount : 0L ) 
#define /* int */ RibIncrLineCount( /* RIB_HANDLE */ rib )    \
          ( (rib) ? ((PRIB_INSTANCE)(rib))->linecount++,0 : 1 ) 

#define /* size_t */ RibGetByteOffset( /* RIB_HANDLE */ rib )  \
          ( (rib) ? ((PRIB_INSTANCE)(rib))->byteoffset : 0L ) 
#define /* int */ RibIncrByteOffset( /* RIB_HANDLE */ rib )    \
          ( (rib) ? ((PRIB_INSTANCE)(rib))->byteoffset++,0 : 1 ) 

#define /* size_t */ RibGetBufferLength( /* RIB_HANDLE */ rib )  \
         ( ((rib)&&((PRIB_INSTANCE)(rib))->pbuf)                 \
          ? ((PRIB_INSTANCE)(rib))->pbuf->buflength : 0L ) 

#define /* int */ RibIncrBufferLength( /* RIB_HANDLE */ rib )    \
         ( ((rib)&&((PRIB_INSTANCE)(rib))->pbuf)                 \
          ? ((PRIB_INSTANCE)(rib))->pbuf->buflength++,0 : 1 ) 


#ifndef __RIB_FILE_RIBFUNC
extern  
#endif
int
   RibFreeStepStack( RIB_HANDLE hrib ),
   RibFreeInLineList( RIB_HANDLE hrib );


/*
 *  Pre PRMan 3.6 Compatibility Option
 */
#ifndef __RIB_FILE_RIBFUNC
extern  
#endif
int
   RibReadVer35Option( RIB_HANDLE hrib ),
   RibReadVer36Option( RIB_HANDLE hrib ),
   RibReadVer37Option( RIB_HANDLE hrib );


/*
 *   Error Message Handling Functions
 */
#ifndef __RIB_FILE_SETERROR
extern  
#endif
int 
   RibSetErrorHandler( RIB_HANDLE  rib, PRIB_ERRORFILTERPROC p ),
   RibSetError( RIB_HANDLE rib, int code, int severity, PRIB_ERROR error );

#define /* PRIB_ERRORFILTERPROC */ RibGetErrorHandler(/*RIB_HANDLE*/rib) \
          ( (rib) ? ((PRIB_INSTANCE)(rib))->errorhandler : 0L )

#ifndef __RIB_FILE_SETERROR
extern  
#endif
PRIB_ERROR 
   RibGetError( RIB_HANDLE rib );


/* Use RibClearError(rib) to clear a present error message from a
 *   RIB_ERRORFILTERPROC().  Don't use RibSetError( rib, 0, 0, NULL ).
 */
#ifndef __RIB_FILE_SETERROR
extern  
#endif
int RibClearError( RIB_HANDLE rib );

#define /* int */ RibGetLastErrorCode( /* RIB_HANDLE */ rib )      \
          ( (rib) ? ((PRIB_INSTANCE)(rib))->errorcode : 0L ) 
#define /* int */ RibGetLastErrorSeverity( /* RIB_HANDLE */ rib )  \
          ( (rib) ? ((PRIB_INSTANCE)(rib))->errorseverity : 0L ) 

/*
 *   Read Functions for Basic Elements
 */

/* Flags for RibRead...Array() functions. */
#define kRIB_BRACKETS_MASK    0x00000007
enum {
   kRIB_BRACKETS_NONE       = 0x00000000,
   kRIB_BRACKETS_OPTIONAL   = 0x00000001,
   kRIB_BRACKETS_PARAML     = 0x00000002,
   kRIB_BRACKETS_REQUIRED   = 0x00000003,
   kRIB_BRACKETS_CLOSE      = 0x00000004,
   kRIB_STRING_ENCODED      = 0x00000008,
   kRIB_STRING_RTSTRING     = 0x00000010,
   kRIB_STRING_RTARRAY      = 0x00000020,
   kRIB_ARRAY_ENCODED       = 0x00000040,
   kRIB_ARRAY_INTEGER       = 0x00000080,
   kRIB_ARRAY_FLOAT         = 0x00000100,
   kRIB_ARRAY_STRING        = 0x00000200
};


typedef int (*PReadArrayFunc)( RIB_HANDLE hrib, RIB_UINT32 options, 
                              int numberof, void*p );

#ifndef __RIB_FILE_READFUNC
extern 
#endif
int
   RibReadInteger( RIB_HANDLE rib, RtInt *i ),
   RibReadFloatingPoint( RIB_HANDLE rib, RtFloat *flt ),
   RibReadString( RIB_HANDLE rib, RIB_UINT32 options, RtString *p ),
   RibReadStringArray( RIB_HANDLE hrib, RIB_UINT32 options, 
                      int numberof, RtString **array),
   RibReadIntegerArray( RIB_HANDLE hrib, RIB_UINT32 options, 
                       int numberof, RtInt **array),
   RibReadFloatingPointArray( RIB_HANDLE rib, RIB_UINT32 options, 
                             int numberof, RtFloat **array ),
   RibReadRIBCallParameters( RIB_HANDLE rib,
                            char *cparams, /* Refer to ribhash/cparams.c. */ 
                            int asizes[], void *params ),
   RibReadArrayAndLength( RIB_HANDLE hrib, RIB_UINT32 options, 
                         int *numberof, void **pp ),
   RibReadFileForToken( RIB_HANDLE rib, char *table ),
   RibReadThroughUndeclaredData( RIB_HANDLE hrib );


#ifndef __RIB_FILE_READFUNC
extern 
#endif
RtBoolean 
   RibIsaNumberNext( RIB_HANDLE hrib ),
   RibIsaStringNext( RIB_HANDLE hrib );


/*
 *   Handle "version #.##" in a RIB file by just ignoring it.
 *      An entry in the ritable is provided in case this default
 *      behavior needs to be changed.
 */
#ifndef __RIB_FILE_READFUNC
extern 
#endif
RtVoid Rib_version( RtFloat version );


/* Three items can appear inbetween the values that make up the parameters 
 *    to an RI call (except in an encoded array of floats): an archive 
 *    record ( # or ## ), a binary code marking a string definition or
 *    a binary code marking the begining of a RI request definition.
 *
 * Call RibHandlePrefix() inbetween items in a RIB file to handle these little
 *    interruptions in reading an RI call's parameters and user data.  
 */
#ifndef __RIB_FILE_READFUNC
extern
#endif
int
   RibHandlePrefix( RIB_HANDLE rib ),
   RibReadDefiningBinCode( RIB_HANDLE rib, int alreadyreadin ),
   RibReadRIRequestToken( RIB_HANDLE hrib );


/* 
 * Functions for handling hints, comments and user written data structures.
 *
 */
#ifndef __RIB_FILE_READFUNC
extern
#endif
int 
   RibReadArchiveRecord( RIB_HANDLE rib ),
   RibHandleArchiveRecord( RIB_HANDLE rib );


#ifndef __RIB_FILE_HINTFUNC
extern
#endif
int
   RibSetArchiveRecordHandler( RIB_HANDLE rib, PRIB_ARCRECFILTERPROC p ),
   RibIgnoreArchiveRecords( RIB_HANDLE rib ),
   RibDefaultArchiveRecordHandler( RIB_HANDLE rib ),
   RibDefaultHintHandler( RIB_HANDLE rib ),
   RibArcRecStore( RIB_HANDLE hrib ),
   RibArcRecPlayBack( RIB_HANDLE hrib ),
   RibFreeArcRecPlayBack( RIB_HANDLE hrib );



#ifndef __RIB_FILE_VCACHE
extern
#endif
int
   RibVectorCacheInit( RIB_HANDLE hrib, RtInt n ),
   RibCacheVector( RIB_HANDLE rib, 
                  RtInt n, RtToken **paTokens, RtPointer **paParams ),
   RibCopyVectorCache( RIB_HANDLE rib, 
                      RtInt n, RtToken **paTokens, RtPointer **paParams ),
   RibFreeVectorCache( RIB_HANDLE rib ),
   RibFreeVectorData( RIB_HANDLE hrib, 
                     RtInt nparms, RtToken *tokens, RtPointer *parms ),
   RibGetVectorCache( RIB_HANDLE rib, 
                     RtInt *n, RtToken **tokens, RtPointer **params ),
   RibGetVectorCacheInfo( RIB_HANDLE rib, RtInt *n, RtInt *nvertices, 
                         unsigned int **types, unsigned int **sizes );


/* Flags for required parameters of RibReadParameterList(). */
/* Refer to affine/readrib/paraml.c before changing the following: */
enum {
   kRIB_REQUIRE_NOTHING,
   kRIB_REQUIRE_P,
   kRIB_REQUIRE_P_OR_PZ,
   kRIB_REQUIRE_P_OR_PW
};
#define kRIB_PARAML_UNKNOWN_SIZE  -1

#ifndef __RIB_FILE_PARAML
extern 
#endif
int
   RibReadParameterList( RIB_HANDLE rib, int require,
                        int uniform, int varying, int vertex,
                        RtInt *pn, RtToken **paTokens, RtPointer **paParams ),
   RibGetParameterTypeInfo( RIB_HANDLE rib, 
			    int *uniform, int *varying, int *vertex ),
   RibErrorNumberOfValues( RIB_HANDLE hrib, RtToken parameterID, 
                           RtInt expected_nitems, RtInt received_nitems );


#ifndef __RIB_FILE_DECLARE
extern  
#endif
PRIB_HASHATOM 
   RibDeclare( RIB_HANDLE hrib, char *name, char *declaration ),
   RibHandleInLineDeclaration( RIB_HANDLE hrib, char **pname );


#ifndef __RIB_FILE_DECLARE
extern  
#endif
int RibUndoInLineList( RIB_HANDLE hrib );

#define RibGetTypeClassNames( code, c, t ) \
        RibGetClassTypeNames( (code), (c), (t) )

#ifndef __RIB_FILE_ERROR
extern  
#endif
int 
   RibGetClassTypeNames( unsigned int code, char **c, char **t ),
   RibDefaultErrorHandler( RIB_HANDLE hrib, int code, int severity, 
                           PRIB_ERROR error );

/* OK.  A real C implementation would use a 255 long table with flags. 
 * I'll fix this and make it efficient later.
 */
#define RibIsaLetter(l) \
                (( (l) >= 'a' && (l) <= 'z' ) || ( (l) >= 'A' && (l) <= 'Z' ))
#define RibIsaStdASCIIChar(a) \
                ((a) >= 32 && (a) <= 126 && (a) != '\"' && (a) != '\\' )
#define RibIsanEncodedDefinition(c) ((c) >= 0314 && (c) <= 0316)
#define RibIsanEncodedRIRequest(c)  ((c)==0246)
#endif 


#ifndef __RIB_FILE_RIBFUNC
extern  
#endif
RtVoid
   RibNuCurvesV( RtInt ncurves, RtInt nvertices[], RtInt order[],
		RtFloat knot[], RtFloat min[], RtFloat max[], 
		RtInt n, RtToken tokens[], RtPointer parms[] ),
   RibImplicitV( RtInt a, RtInt b[], RtInt c, RtFloat d[],
		   RtInt e, RtFloat f[], RtInt g, RtFloat h[],
		   RtInt n, RtToken tokens[], RtPointer parms[] ),
   RibGeometricRepresentation( RtToken type );


/* 
 *   Globals
 *
 */
#ifndef __RIB_FILE_STREAMIO
extern int gRibMaxFiles;
extern int gRibNSubfiles;
extern int gRibNOpenSubfiles;
#endif

#ifndef __RIB_FILE_RITABLE
extern PRIB_RIPROC gRibRITable[];
#endif

#ifndef __RIB_FILE_READFUNC
extern PRIB_ARCRECFILTERPROC gRibHintTable[];
#endif

#ifndef  __RIB_FILE_RIBFUNC
extern PRIB_READPROC gRibReadTable[];
#endif

#ifndef  __RIB_FILE_RIBTBL
extern char *gRibRITokenTables[]; 
#endif

#ifndef  __RIB_FILE_HINTTBL
extern char gRibHintTokenTable[]; 
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
