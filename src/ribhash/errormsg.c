/* $RCSfile: errormsg.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 06:30:52 $
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
 * FILE:  errormsg.c
 *
 * DESCRIPTION:  Standard RIB error messages.
 *   
 *    Contains:
 * 
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_ERRORMSG
#include <stdio.h>
#include <stdlib.h>
#include <ripriv.h>
#include <ribhash.h>


char *RibGetErrorMsg( int code )
{
   auto char *p;

   switch (code)
   {
    case RIE_NOMEM:
      p = "Out of memory";
      break;
    case RIE_SYSTEM:
      p = "Miscellaneous system error";
      break;
    case RIE_NOFILE:
      p = "File nonexistent";
      break;
    case RIE_BADFILE:
      p = "Bad file format";
      break;
    case RIE_VERSION:
      p = "File version mismatch";
      break;
    case RIE_INCAPABLE:
      p = "Optional RI feature";
      break;
    case RIE_UNIMPLEMENT:
      p = "Unimplemented feature";
      break;
    case RIE_LIMIT:
      p = "Arbitrary program limit";
      break;
    case RIE_BUG:
      p = "Probably a bug in renderer";
      break;
    case RIE_NOTSTARTED:
      p = "RiBegin not called";
      break;
    case RIE_NESTING:
      p = "Bad begin-end nesting";
      break;
    case RIE_NOTOPTIONS:
      p = "Invalid state for options";
      break;
    case RIE_NOTATTRIBS:
      p = "Invalid state for attributes";
      break;
    case RIE_NOTPRIMS:
      p = "Invalid state for primitives";
      break;
    case RIE_ILLSTATE:
      p = "Other invalid state";
      break;
    case RIE_BADMOTION:
      p = "Badly formed motion block";
      break;
    case RIE_BADSOLID:
      p = "Badly formed solid block";
      break;
    case RIE_BADTOKEN:
      p = "Invalid token for request";
      break;
    case RIE_RANGE:
      p = "Parameter out of range";
      break;
    case RIE_CONSISTENCY:
      p = "Parameters inconsistent";
      break;
    case RIE_BADHANDLE:
      p = "Bad object or light handle";
      break;
    case RIE_NOSHADER:
      p = "Can't load requested shader";
      break;
    case RIE_MISSINGDATA:
      p = "Required parameters not provided";
      break;
    case RIE_SYNTAX:
      /* Spec had the following message: "Declare type syntax error."  
       *   But when printed out by the default error handler, this looked
       *   like an error in a RIB file's Declare statement.  
       */
      p = "Syntax error"; 
      break;
    case RIE_MATH:
      p = "Zero-divide, noninvertable matrix, or someother math error";
      break;
    default:
      return NULL;
   }

   return p;
}
