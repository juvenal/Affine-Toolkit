/* $RCSfile: seterror.c,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:07 $
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
 * FILE:  seterror.c
 *
 * DESCRIPTION:  Library that reads both ASCII and binary RIB files.
 *   
 *    Contains:
 * 
 *        ?-?-96  Created.  Need to look back over archives for actual date.
 *      12-14-97  Added comment in RibSetError().
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_SETERROR
#include <string.h>
#include <ripriv.h>
#include <ribrdr.h>


int RibSetErrorHandler( RIB_HANDLE hrib, PRIB_ERRORFILTERPROC p )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;

   if (rib)
   {
      rib->errorhandler = p;
   }
   else
   {
      return kRIB_ERRRC_INT;
   }

   return kRIB_OK;
}


int RibSetError( RIB_HANDLE hrib, int code, int severity, PRIB_ERROR error )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if ( rib && code )
   {
      /* If given error structure is not the scratch pad error structure
       * provided in the rib structure, then copy the data to the 
       * scratch pad error structure.
       */
      if ( error != &rib->error )
      {
         memcpy( &rib->error, error, sizeof(RIB_ERROR));
      }
      if ( rib->errorhandler
	   && !(*(rib->errorhandler))( (RIB_HANDLE)rib, code, severity, 
				       &rib->error ) )
      {
         rib->errorcode = code;
         rib->errorseverity = severity;
      }
   }
   else
   {
      return kRIB_ERRRC_INT;
   }

   return kRIB_OK;
}


PRIB_ERROR RibGetError( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if ( rib )
   {
      return &rib->error;
   }
   else
   {
      return kRIB_ERRRC_PTR;
   }
}


/* Don't use RibSetError( rib, NULL, NULL, NULL ) to clear a present
 *   error message from an *RIB_ERRORFILTERPROC().  Use RibClearError(rib).
 */
int RibClearError( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if ( rib )
   {
      rib->errorcode     = RIE_NOERROR;
      rib->errorseverity = RIE_INFO;
      rib->error.type    = kRIB_ERRTYPE_NONE;
   }
   else
   {
      return kRIB_ERRRC_INT;
   }

   return kRIB_OK;
}

