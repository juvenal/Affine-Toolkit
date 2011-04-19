/* $RCSfile: tables.c,v $  $Revision: 1.4 $ $Date: 2000/02/15 17:11:02 $
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
 * FILE:  tables.c
 *
 * DESCRIPTION:  
 *   
 *     History:
 *       7-10-97  Added support for new PRMan 3.7 Points and Curve primitives.
 *      10-05-99  Added gRibSearchArchiveTable.
 *      02-15-00  Added declaration type "int" to types.asc [EVES00].
 *
 * 
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_TABLES
#include <ribtables.h>


/* The following table was created originally with the following command:
 *
 *            tokentbl -s classes.asc gRibClassTable
 *
 *
 * NOTE:  The order of class names in classes.asc and the following
 *        table can not change without modification to RibDeclare() in
 *        affine/readrib/hash.c.
 */
char gRibClassTable[] = {
    9,  9 ,'u','n','i','f','o','r','m','\0',      kRIBTABLES_UNIFORMCLASS,
   20,  1 , 'v',
    8,  8 ,'a','r','y','i','n','g','\0',          kRIBTABLES_VARYINGCLASS,
    0,  7 ,'e','r','t','e','x','\0',              kRIBTABLES_VERTEXCLASS,
    0, 10 ,'c','o','n','s','t','a','n','t','\0',  kRIBTABLES_CONSTANTCLASS
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s types.asc gRibTypeTable
 *
 * NOTE:  The order of tpye names in types.asc and the following
 *        table can not change without modification to RibDeclare() in
 *        readrib/declare.c.
 */
char gRibTypeTable[] = {
    7,  7 ,'f','l','o','a','t','\0',          kRIBTABLES_FLOATTYPE,
    8,  8 ,'s','t','r','i','n','g','\0',      kRIBTABLES_STRINGTYPE,
    7,  7 ,'p','o','i','n','t','\0',          kRIBTABLES_POINTTYPE,
    7,  7 ,'c','o','l','o','r','\0',          kRIBTABLES_COLORTYPE,
    8,  8 ,'n','o','r','m','a','l','\0',      kRIBTABLES_NORMALTYPE,
    8,  8 ,'v','e','c','t','o','r','\0',      kRIBTABLES_VECTORTYPE,
    8,  8 ,'h','p','o','i','n','t','\0',      kRIBTABLES_HPOINTTYPE,
   15,  3 ,'i', 'n', 't',
    2,  2 ,'\0',                              kRIBTABLES_INTEGERTYPE,
    0,  6 ,'e','g','e','r','\0',              kRIBTABLES_INTEGERTYPE,
    0,  8 ,'m','a','t','r','i','x','\0',      kRIBTABLES_MATRIXTYPE
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s patches.asc gRibPatchTable
 *
 * NOTE:  The order of patch types in patches.asc and the following
 *        table can not change without modification to RibReadPatch() in
 *        affine/readrib/ribfunc.c.
 */
char gRibPatchTable[] = {
    0,  2 , 'b', 'i',
    8,  8 ,'l','i','n','e','a','r','\0',  kRIBTABLES_BICUBIC,
    0,  7 ,'c','u','b','i','c','\0',      kRIBTABLES_BILINEAR
};


/* 7-10-97 PRMan 3.7 Support
 *
 * The following table was created originally with the following command:
 *
 *            tokentbl -s curves.asc gRibCurvesTable
 *
 * NOTE:  The order of patch types in curves.asc and the following
 *        table can not change without modification to RibReadCurves() in
 *        affine/readrib/ribfunc.c.
 */
char gRibCurvesTable[] = {
    8,  8 ,'l','i','n','e','a','r','\0',  kRIBTABLES_LINEAR,
    0,  7 ,'c','u','b','i','c','\0',      kRIBTABLES_CUBIC
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s periodic.asc gRibPeriodicTable
 *
 * NOTE:  The order of items in periodic.asc and the following
 *        table can not change without modification to RibReadPatchMesh() in
 *        affine/readrib/ribfunc.c.
 */
char gRibPeriodicTable[] = {
   10, 10 ,'p','e','r','i','o','d','i','c','\0',      kRIBTABLES_PERIODIC,
    0, 13 ,'n','o','n',
               'p','e','r','i','o','d','i','c','\0',  kRIBTABLES_NONPERIODIC
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s bases.asc gRibBasisTable
 *
 * NOTE:  The order of items in bases.asc and the following
 *        table can not change without modification to RibReadBasis() in
 *        affine/readrib/ribfunc.c.
 */
char gRibBasisTable[] = {
   21,  1 , 'b',
    7,  7 ,'e','z','i','e','r','\0',          kRIBTABLES_BEZIERBASIS,
    0,  9 ,'-','s','p','l','i','n','e','\0',  kRIBTABLES_BSPLINEBASIS,
   13, 13 ,'c','a','t','m','u','l','l',
                       '-','r','o','m','\0',  kRIBTABLES_CATMULLROMBASIS,
    9,  9 ,'h','e','r','m','i','t','e','\0',  kRIBTABLES_HERMITEBASIS,
    0,  7 ,'p','o','w','e','r','\0',          kRIBTABLES_POWERBASIS
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s filters.asc gRibFilterTable
 *
 * NOTE:  The order of items in filters.asc and the following
 *        table can not change without modification to functions in
 *        affine/readrib/ribfunc.c.
 */
char gRibFilterTable[] = {
    5,  5 ,'b','o','x','\0',                      kRIBTABLES_BOXFILTER,
   13, 13 ,'c','a','t','m','u','l','l',
                     '-','r','o','m','\0',        kRIBTABLES_CATMULLROMFILTER, 
   10, 10 ,'g','a','u','s','s','i','a','n','\0',  kRIBTABLES_GAUSSIANFILTER,
    6,  6 ,'s','i','n','c','\0',                  kRIBTABLES_SINCFILTER,
    0, 10 ,'t','r','i','a','n','g','l','e','\0',  kRIBTABLES_TRIANGLEFILTER
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s shading.asc gRibShadingTypeTable
 *
 */
char gRibShadingTypeTable[] = {
   10, 10 ,'c','o','n','s','t','a','n','t','\0',  kRIBTABLES_CONSTANT,
    0,  8 ,'s','m','o','o','t','h','\0',          kRIBTABLES_SMOOTH
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s errorhnd.asc gRibErrorHandlerTable
 *
 * NOTE:  The order of items in bases.asc and the following
 *        table can not change without modification to RibReadErrorHandler() 
 *        in affine/readrib/readfunc.c.
 */
char gRibErrorHandlerTable[] = {
    8,  8 ,'i','g','n','o','r','e','\0',  kRIBTABLES_IGNORE,
    7,  7 ,'p','r','i','n','t','\0',      kRIBTABLES_PRINT,
    0,  7 ,'a','b','o','r','t','\0',      kRIBTABLES_ABORT
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s orient.asc gRibOrientationsTable
 *
 */
char gRibOrientationsTable[] = {
    4,  4 ,'l','h','\0',                      kRIBTABLES_LHORIENT,
    4,  4 ,'r','h','\0',                      kRIBTABLES_RHORIENT,
    9,  9 ,'o','u','t','s','i','d','e','\0',  kRIBTABLES_OUTSIDEORIENT,
    0,  8 ,'i','n','s','i','d','e','\0',      kRIBTABLES_INSIDEORIENT
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s procedural.asc gRibProceduralTable
 *
 */
char gRibProceduralTable[] = {
   36,  1 , 'D',
   19, 19 ,'e','l','a','y','e','d',
           'R','e','a','d','A','r','c','h','i','v','e','\0',  
                                                kRIBTABLES_DELAYEDREADARCHIVE,
    0, 12 ,'y','n','a','m','i','c','L','o','a','d','\0',  
                                                kRIBTABLES_DYNAMICLOAD,
    0, 12 ,'R','u','n','P','r','o','g','r','a','m','\0',
                                                kRIBTABLES_RUNPROGRAM
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s solidops.asc gRibSolidOpsTable
 *
 */
char gRibSolidOpsTable[] = {
   11, 11 ,'p','r','i','m','i','t','i','v','e','\0',              
                                                kRIBTABLES_PRIMITIVE,
   14, 14 ,'i','n','t','e','r','s','e','c','t','i','o','n','\0',
                                                kRIBTABLES_INTERSECTION,
    7,  7 ,'u','n','i','o','n','\0',            
                                                kRIBTABLES_UNION,
    0, 12 ,'d','i','f','f','e','r','e','n','c','e','\0',
                                                kRIBTABLES_DIFFERENCE
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s records.asc gRibRecordsTable
 *
 */
char gRibRecordsTable[] = {
    9,  9 ,'c','o','m','m','e','n','t','\0',          kRIBTABLES_COMMENT,
    0, 11 ,'s','t','r','u','c','t','u','r','e','\0',  kRIBTABLES_STRUCTURE
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s format.asc gRibFormatTable
 *
 */
char gRibFormatTable[] = {
   11, 11 ,'R','I','S','p','e','c','3','.','1','\0',     
                                               kRIBTABLES_RISPEC31FORMAT,
   12, 12 ,'N','u','P','a','t','c','h',
           '3','.','6','\0',                   kRIBTABLES_NUPATCH36FORMAT,
    8,  8 ,'i','n','d','e','n','t','\0',       kRIBTABLES_INDENTFORMAT,
    6,  6 ,'l','o','n','g','\0',               kRIBTABLES_LONGFORMAT,
    0,  7 ,'s','h','o','r','t','\0',           kRIBTABLES_SHORTFORMAT
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s coordsys.asc gRibCorodSysTable
 *
 */
char gRibCoordSysTable[] = {
    8,  8 ,'c','a','m','e','r','a','\0',  kRIBTABLES_CAMERA_CS,
    8,  8 ,'r','a','s','t','e','r','\0',  kRIBTABLES_RASTER_CS,
    8,  8 ,'o','b','j','e','c','t','\0',  kRIBTABLES_OBJECT_CS,
    7,  7 ,'w','o','r','l','d','\0',      kRIBTABLES_WORLD_CS,
    8,  8 ,'s','c','r','e','e','n','\0',  kRIBTABLES_SCREEN_CS,
    0,  5 ,'N','D','C','\0',              kRIBTABLES_NDC_CS,
};


/* The following table was created originally with the following command:
 *
 *            tokentbl -s archive.asc gRibSearchArchiveTable
 *
 */
char gRibSearchArchiveTable[] = {
    0,  9 ,'a','r','c','h','i','v','e','\0',  0,
};
