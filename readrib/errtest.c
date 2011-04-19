/* $RCSfile: errtest.c,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:07 $
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
 * FILE:  errtest.c
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
#include <ribrdr.h>


int main(int argc, char **argv)
{
   RIB_INSTANCE  rib;
   RIB_HASHATOM  var;
   RIB_ERROR     error;

   rib.filename = "george";
   rib.linecount = 9;
   rib.byteoffset = 127;
   rib.status = kRIB_STATUS_ENCODEDDATA;
   error.type = kRIB_ERRTYPE_UNDEFINED_LIGHT;
   error.var = NULL;
   error.expected.sys_int = 215;
   error.unexpected.user_int = 128;

   printf( "A horrible looking error message should appear:\n" );
   if (RibDefaultErrorHandler( (RIB_HANDLE)&rib, 
                              RIE_BADHANDLE, RIE_ERROR, &error ) )
     printf( "ERROR from RibDefaultErrorHandler\n" );
   else
     printf( "Horrible error message test passed\n\n\n" );

   rib.filename = "bob";
   rib.linecount = 10;
   rib.byteoffset = 134;
   rib.status = 0L;
   var.code = kRIB_VARYINGCLASS | kRIB_POINTTYPE;
   var.data = "my varying variable";
   error.type = kRIB_ERRTYPE_NUMBER_OF_VALUES;
   error.var = &var;
   error.expected.sys_int = 12;
   error.unexpected.user_int = 1;

   printf( "A horrible looking error message should appear again:\n" );
   if (RibDefaultErrorHandler( (RIB_HANDLE)&rib, 
                              RIE_CONSISTENCY, RIE_ERROR, &error ) )
     printf( "ERROR from RibDefaultErrorHandler\n" );
   else
     printf( "Horrible error message test passed\n" );   

   return 0;
}


