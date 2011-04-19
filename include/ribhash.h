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
 * FILE:  ribhash.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          Structures used for the hash portion of RIB Reader library.
 *
 *    History:
 *          07-10-97  Added class "constant" and type "matrix".
 *          03-28-98  Added RibFreeBasisMatrix(). 
 *          06-07-99  Added RibGetTokenName().
 *          10-05-99  Added gRibSearchArchiveTable.
 *          02-26-00  Added RibDumpHashTable().
 *
 *
 *    References:
 *          [PIXA89]  Pixar Animation Studios, The RenderMan Interface, 
 *                    Version 3.1, Richmond, CA, September 1989.
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#ifndef __RIBHASH_HEADER__
#define __RIBHASH_HEADER__

#ifndef _RIPRIV_TYPES 
#define _RIPRIV_TYPES  
#endif
#ifndef RI_HEADER
#include <ripriv.h>
#endif


#define _RIB_RTLONGINT 0
#define _RIB_RTDOUBLE  0

typedef int RIB_INT32;
typedef int *PRIB_INT32;
typedef unsigned int RIB_UINT32;
typedef unsigned int *PRIB_UINT32;

/* Constants for hashatom.type */
#define kRIB_HASH_TYPEMASK  0x00ffffff
#define kRIB_HASH_FLAGMASK  0xff000000
enum {
   kRIB_HASH_UNKNOWN,
   kRIB_HASH_RIREQUESTCODE,
   kRIB_HASH_LIGHTHANDLE,
   kRIB_HASH_OBJECTHANDLE,
   kRIB_HASH_STRINGTOKEN,
   kRIB_HASH_STRING,
   kRIB_HASH_VARIABLE,
   kRIB_HASH_COORDSYS,
   kRIB_HASH_LAST,
   kRIB_HASH_USER = 1024
};
#define kRIB_HASH_FREEDATA     0x01000000
#define kRIB_HASH_FREEEXTDATA  0x02000000


enum {
   kRIB_UNKNOWNCLASS  = 0x00000000,
   kRIB_UNIFORMCLASS  = 0x00010000,
   kRIB_VARYINGCLASS  = 0x00020000,
   kRIB_VERTEXCLASS   = 0x00030000,
   kRIB_CONSTANTCLASS = 0x00040000,
   kRIB_LASTCLASS     = 0x00050000
};
#define /* int */ RibClassEnumToInt( /* int */ class ) ((class)>>16)
#define /* int */ RibIntToClassEnum( /* int */ class ) \
           ( class > 0 && class < kRIB_LASTCLASS ? ((class)<<16) : 0 )

/* Don't change the order of the kRIB_*TYPEs without changing the tables
 *   in affine/readrib/ribfunc.c.
 */
enum {
   kRIB_UNKNOWNTYPE, 
   kRIB_FLOATTYPE, 
   kRIB_STRINGTYPE,
   kRIB_POINTTYPE, 
   kRIB_COLORTYPE, 
   kRIB_NORMALTYPE,
   kRIB_VECTORTYPE,
   kRIB_HPOINTTYPE,
   kRIB_INTTYPE,   
   kRIB_MATRIXTYPE,  
   kRIB_FLOATPAIRTYPE,
   kRIB_INTPAIRTYPE,  
   kRIB_STANDALONETYPE,
   kRIB_LASTTYPE
};
#define kRIB_SYS_DECLARED     0x80000000
#define kRIB_ARRAY_DECLARED   0x40000000
#define kRIB_INLINE_DECLARED  0x20000000
#define kRIB_USER1_DECLARED   0x08000000
#define kRIB_USER2_DECLARED   0x04000000
#define kRIB_CLASS_MASK       0x00ff0000
#define kRIB_TYPE_MASK        0x0000ffff
#define kRIB_CLASSTYPE_MASK   (kRIB_CLASS_MASK | kRIB_TYPE_MASK)


typedef struct _RIB_HASHATOM {
   RIB_UINT32    type;
   unsigned int  code;
   void          *data;
   union  {
      unsigned int  ripID;
      void          *extdata;
      unsigned int  n;
   } with;
   struct _RIB_HASHATOM  *next;
} RIB_HASHATOM;
typedef RIB_HASHATOM  *PRIB_HASHATOM;


/* Default dimensions for hash table. */
#define kRIB_HASH_TABLESIZE    197
#define kRIB_HASH_SECONDSIZE   23
typedef struct _RIB_HASHTABLE {
   int            modnumber;  /* table size (prime number) */
   int            mod2number;    
   PRIB_HASHATOM  conflict;
   PRIB_HASHATOM  apAtoms[1][kRIB_HASH_SECONDSIZE];  
} RIB_HASHTABLE;
typedef RIB_HASHTABLE  *PRIB_HASHTABLE;
typedef PRIB_HASHTABLE  RIB_HASHHND;

typedef int (*PRIB_QCLASSTYPE)( char *variable );

/*
 *   Return Codes:
 *
 *      Functions with an int return code that have a return code scheme like
 *         fgetc() use kRIB_ERRRC_STREAMIO for errors and EOF for end of file.
 *         These functions do not print any error messages by calling an 
 *         error handler.
 *
 *      Functions with an int return code that return a boolean value 
 *         for success or failure use kRIB_OK for success and kRIB_ERRRC_INT
 *         for failure.  If these functions read from a file and encounter
 *         an unexpected end of file, they call an error handler and return
 *         EOF.
 *
 *      Functions with a pointer as a return code either return a pointer
 *         for success or kRIB_ERRRC_PTR for an error.  These functions
 *         are for setting up structures and do not read from files, so 
 *         an EOF return code is not needed.
 *
 *      Functions returning an index into an array, return a zero based
 *         index or kRIB_ERRRC_INDEX (a negative value) for an error.
 */
#if (EOF!=-2)
#define kRIB_ERRRC_STREAMIO  ((int)-2)
#else
#error EOF conflicts with kRIB_ERRRC_STREAMIO (change the -2 to -3(?))
#endif
#define kRIB_ERRRC_INDEX     ((int)-1)
#define kRIB_ERRRC_INT       ((int)1)
#define kRIB_ERRRC_PTR       NULL
#define kRIB_OK              ((int)0)
#define kRIB_ERRRC_SIGNAL    ((int)-3)
#define kRIB_ERRRC_USER1     ((int)-4)

/*   
 *   Hash Table Functions 
 */
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#ifndef __FILE_HASH
extern 
#endif
  RIB_HASHHND RibCreateHashTable( int size );

#ifndef __FILE_HASH
extern 
#endif
int 
   RibDestroyHashTable( RIB_HASHHND hash ),
   RibDestroyHashTableKeepData( RIB_HASHHND hash );


#if defined(RIB_MEMORY_TESTS)

#ifndef __FILE_HASH
extern 
#endif
  void *RibMalloc( char *file, unsigned int line, unsigned int size );


#ifndef __FILE_HASH
extern 
#endif
  void *RibRealloc( char *file, unsigned int line, void *p, 
                   unsigned int size );

#ifndef __FILE_HASH
extern 
#endif
  void RibFree( char *file, unsigned int line, void *p );

#define _RibMalloc(a)     RibMalloc(__FILE__,__LINE__,(a))
#define _RibRealloc(a,b)  RibRealloc(__FILE__,__LINE__,(a),(b))
#define _RibFree(a)       RibFree(__FILE__,__LINE__,(a))

#else

#ifndef __FILE_HASH
extern 
#endif
  void *RibMalloc( unsigned int size );

#ifndef __FILE_HASH
extern 
#endif
  void *RibRealloc( void *p, unsigned int size );

#ifndef __FILE_HASH
extern 
#endif
  void RibFree( void *p );

#ifdef __RIBMEMORYCALLS__
#define _RibMalloc(a)     RibMalloc(a)
#define _RibRealloc(a,b)  RibRealloc((a),(b))
#define _RibFree(a)       RibFree(a)
#else
#if !defined(malloc) 
#include <stdlib.h>
#endif
#define _RibMalloc(a)     malloc(a)
#define _RibRealloc(a,b)  realloc((a),(b))
#define _RibFree(a)       free(a)
#endif

#endif

#ifndef __RIB_FILE_HASH
extern 
#endif
  int RibFreeBasisMatrix( RtBasis p );


#ifndef __RIB_FILE_HASH
extern 
#endif
PRIB_HASHATOM 
  RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, ... ),
  RibFindMatch( RIB_HASHHND hash, RIB_UINT32 type, int code, void *data ),
  RibFindNextMatch( RIB_HASHHND hash, PRIB_HASHATOM atom ),
  RibGetConflictingHashAtom( RIB_HASHHND hash );


#ifndef __RIB_FILE_HASH
extern 
#endif
int
  RibSetDefinedRIRequest( PRIB_HASHATOM a, int token, int ripID ),
  RibSetDefinedString( PRIB_HASHATOM a, int token, char *s ),
  RibSetStringAtom( PRIB_HASHATOM a, int token, char *s ),
  RibSetDefinedLightHandle( PRIB_HASHATOM a, 
                           int sequence, RtLightHandle lh ),
  RibSetDefinedObjectHandle( PRIB_HASHATOM a, 
                            int sequence, RtObjectHandle oh ),
  RibSetHashExtendedData( PRIB_HASHATOM a, void *t ),
  RibGetDefinedRIRequest( PRIB_HASHATOM a );


#ifndef __RIB_FILE_HASH
extern 
#endif
char *RibGetDefinedString( PRIB_HASHATOM atom );


#ifndef __RIB_FILE_HASH
extern 
#endif
char *RibGetStringAtom( PRIB_HASHATOM a );


#ifndef __RIB_FILE_HASH
extern 
#endif
RtLightHandle RibGetDefinedLightHandle( PRIB_HASHATOM atom );

#ifndef __RIB_FILE_HASH
extern 
#endif
RtObjectHandle RibGetDefinedObjectHandle( PRIB_HASHATOM atom );

#ifndef __RIB_FILE_HASH
extern 
#endif
void* RibGetHashExtendedData( PRIB_HASHATOM atom );

#ifndef __RIB_FILE_HASH
extern 
#endif
int 
   RibRemoveItem( RIB_HASHHND hash, PRIB_HASHATOM atom ),
   RibHashValueFromString( RIB_HASHHND hash, char* p ),
   RibHashValueFromStringN( RIB_HASHHND hash, char *p, int n );


#ifndef __RIB_FILE_HASH
extern 
#endif
RIB_UINT32
   RibQueryClassType( RIB_HASHHND hash, char *variable, RIB_UINT32 *n );


enum {
   kRIB_BEZIER_BASIS,
   kRIB_SPLINE_BASIS,
   kRIB_CATMULLROM_BASIS,
   kRIB_HERMITE_BASIS,
   kRIB_POWER_BASIS,
   kRIB_LAST_BASIS
};
#ifndef __RIB_FILE_BASISTBL
extern RtFloat *RiStdBasisTbl[5];
#endif


#ifndef __RIB_FILE_STRING
extern  
#endif
int 
   RibReadStringForToken( char *s, char *table, char **tokenend ),
   RibGetUserParameters( char *table, int ntable,
                         RtInt n, RtToken tokens[], RtPointer parms[], 
                         RtPointer output[] );


#ifndef __RIB_FILE_STRING
extern  
#endif
char *RibGetTokenName( char *name );


#ifndef __RIB_FILE_ERRORMSG
extern  
#endif
  char *RibGetErrorMsg( int code );


enum {
   kRIB_CPARAMS_NULL,
   kRIB_CPARAMS_VECTORFORMAT,
   kRIB_CPARAMS_TOKEN,
   kRIB_CPARAMS_POINTER,
   kRIB_CPARAMS_INT,
   kRIB_CPARAMS_FLOAT,
   kRIB_CPARAMS_MATRIX,
   kRIB_CPARAMS_BOUND,
   kRIB_CPARAMS_BASIS,
   kRIB_CPARAMS_COLOR,
   kRIB_CPARAMS_POINT,
   kRIB_CPARAMS_LIGHTHANDLE,
   kRIB_CPARAMS_OBJECTHANDLE,
   kRIB_CPARAMS_BOOLEAN,
   kRIB_CPARAMS_CHAR_POINTER,
   kRIB_CPARAMS_ERRORHANDLER,
   kRIB_CPARAMS_FUNC,
   kRIB_CPARAMS_FILTERFUNC,
   kRIB_CPARAMS_TOKEN_ARRAY,
   kRIB_CPARAMS_POINTER_ARRAY,
   kRIB_CPARAMS_INT_ARRAY,
   kRIB_CPARAMS_FLOAT_ARRAY,
   kRIB_CPARAMS_POINT_ARRAY,
   kRIB_CPARAMS_VARIABLE_ARG,
   kRIB_LAST_CPARAMS
};

#ifndef __RIB_FILE_CPARAMS
extern char *gRibCParams[];
#endif

#ifndef __RIB_FILE_TABLES
extern char gRibClassTable[];
extern char gRibTypeTable[];
extern char gRibPatchTable[];
extern char gRibCurvesTable[];
extern char gRibPeriodicTable[];
extern char gRibBasisTable[];
extern char gRibFilterTable[];
extern char gRibShadingTypeTable[];
extern char gRibErrorHandlerTable[];
extern char gRibOrientationsTable[];
extern char gRibProceduralTable[];
extern char gRibSolidOpsTable[];
extern char gRibRecordsTable[];
extern char gRibFormatTable[];
extern char gRibCoordSysTable[];
extern char gRibSearchArchiveTable[];
#endif

#if defined(RIB_MEMORY_TESTS)
typedef struct _MEM_LIST {
   char          *file;
   unsigned int  line;
   unsigned int  size;
   void          *p;
   struct _MEM_LIST  *next;
} MEM_LIST;
typedef MEM_LIST  *PMEM_LIST;

typedef struct _MEM_TEST {
   unsigned int  nmalloc;
   unsigned int  nfree;
   unsigned int  nstepovers;
   unsigned int  badfreecalls;
   int           stepoverallowed;
   PMEM_LIST     memlist;
   PMEM_LIST     badfreelist;
   PMEM_LIST     stepoverlist;
} MEM_TEST;
typedef MEM_TEST  *PMEM_TEST;

#ifndef __FILE_HASH
extern PMEM_TEST  gPMemTest;
#endif

#ifndef __FILE_HASH
extern 
#endif
PMEM_TEST RibMemoryTestInit( void );

#ifndef __FILE_HASH
extern 
#endif
int RibMemoryTestResults( PMEM_TEST pmemtest );

#ifndef __FILE_HASH
extern 
#endif
int RibMemoryTestStepOverAllowed( void );

#ifndef __FILE_HASH
extern 
#endif
int RibMemoryTestMalloc( char *file, unsigned int line,
                        void *p, unsigned int size );

#ifndef __FILE_HASH
extern 
#endif
int RibMemoryTestFree( char *file, unsigned int line, void *p );


#ifndef __FILE_HASH
extern 
#endif
void RibDumpHashTable( RIB_HASHHND hash );


#endif
/* endif for defined(RIB_MEMORY_TESTS). */

#ifndef __RIB_FILE_NAMES
extern char *gRibNames[];
#endif


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#define RibIsaWhiteSpace(a) \
                ( (a) == ' ' || (a) == '\n' || (a) == '\t' || (a) == '\r' )
#define RibIsaBinaryChar(a) (0200 & (char)(a))
/* Characters '\"', '#', '[' and ']' delimit syntatic enities.  The following
 *   test just checks all but ']' since it would be handled by a function 
 *   that reads a string or an array.
 */
#define RibIsaDelimitor(a) ( (a)=='[' || (a)=='#' || a=='\"' )

#endif 

