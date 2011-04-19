/* $RCSfile: version.c,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:07 $
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
 * FILE:  version.c
 *
 * DESCRIPTION:
 *   
 *    Contains:  
 *               unsigned int RibGetLibraryVersion( void );
 *               char        *RibGetLibraryVersionName( void ); * 
 *     History:
 *
 *    References:
 *          [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, 
 *                    Richmond, CA, pp. 160-165, September 1989.  
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_VERSION
#include <stdio.h>
#include <stdlib.h>
#include <ripriv.h>
#include <ribrdr.h>


/* These functions are Needed for a cross platform method of version 
 *    checking a library compiled and linked as a DSO, CFM or DLL.
 *
 * It'd  be nice if they would all have choosen one name for these 
 *    things.
 */


unsigned int RibGetLibraryVersion( void )
{
   return (unsigned int)kRIB_AFFINE_VERSIONINT;
}


char *RibGetLibraryVersionName( void )
{
   return kRIB_AFFINE_VERSIONSTR;
}
