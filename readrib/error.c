/* $RCSfile: error.c,v $  $Revision: 1.3 $ $Date: 1999/10/16 09:15:19 $
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
 * FILE:  error.c
 *
 * DESCRIPTION:  Error message printing.
 *   
 *    Contains:
 * 
 *     History:
 *       01-05-98  Added an error message for ASCII integers followed by
 *                 a decimal point.  Before the next RibRead* function would
 *                 would print a rather cryptic message.
 *       01-07-98  Added kRIB_ERRTYPE_BAD_BASISUSTEP and 
 *                 kRIB_ERRTYPE_BAD_BASISUSTEP error messages. 
 *       12-30-98  Fixed kRIB_ERRTYPE_SYSTEM_PARAM print out of old
 *                 delcaration.  Was printing "type class" not "class type".
 *                 Also set s=NULL so an extra printf() was avoided.
 *       06-05-99  Changed RibGetClassTypeNames() to RibGetClassTypeNames()
 *                 and also the returned strings "float pair" and "int pair"
 *                 to strings "float[2]" and "int[2]" respectively.  These
 *                 strings can be plugged directly into a Declaration 
 *                 statement making them more useful.  Added a macro to
 *                 ribrdr.h to cover for the missing RibGetClassTypeNames()
 *                 function.
 *       10-14-99  Moved commenting out of calling kRIB_ERRTYPE_SYSTEM_PARAM
 *                 from declare.c to error.c.  Now someone can setup an
 *                 error handler if they want to set redeclaration errors
 *                 printed.  Refer to flag SYSTEM_DECLARED_WARNING below.
 *       10-15-99  Expected and  unexpected values were swapped
 *                 in kRIB_ERRTYPE_VERSION_TOO_NEW message.  A testcase
 *                 affine/readrib/tests/vererr.bin was added.
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
#define __RIB_FILE_ERROR
#include <stdio.h>
#include <stdlib.h>
#include <ripriv.h>
#include <ribrdr.h>


/* Do not free the pointers assigned to c and t.  The pointers returned
 *   are to static strings.
 */
int RibGetClassTypeNames( unsigned int code, char **c, char **t )
{

   if (c)
   {
      switch( code & kRIB_TYPE_MASK )
      {
       case kRIB_FLOATTYPE:
         *t = "float";
         break;
       case kRIB_STRINGTYPE:
         *t = "string";
         break;
       case kRIB_POINTTYPE:
         *t = "point";
         break;
       case kRIB_COLORTYPE:
         *t = "color";
         break;
       case kRIB_NORMALTYPE:
         *t = "normal";
         break;
       case kRIB_VECTORTYPE:
         *t = "vector";
         break;
       case kRIB_HPOINTTYPE:
         *t = "hpoint";
         break;
       case kRIB_INTTYPE:
         *t = "int";
         break;
       case kRIB_FLOATPAIRTYPE:
         *t = "float[2]";
         break;
       case kRIB_INTPAIRTYPE:
         *t = "integer[2]";
         break;
       case kRIB_STANDALONETYPE:
         *t = "standalone";
         break;
       default:
         *t = "unknown";
      }
   }
   if (t)
   {
      switch ( code & kRIB_CLASS_MASK )
      {
       case kRIB_VERTEXCLASS:
         *c = "vertex";
         break;
       case kRIB_VARYINGCLASS:
         *c = "varying";
         break;
       case kRIB_UNIFORMCLASS:
       default:
         *c = "uniform";
      }
   }

   return kRIB_OK;
}


int RibDefaultErrorHandler( RIB_HANDLE hrib, int code, int severity, 
                           PRIB_ERROR error )
{
   static char *severities[] =
   {
      "INFO", "WARNING", "ERROR", "SEVERE"
   };
   static char  *nullstr = "";
   char  *s,*t,*v;
#ifdef SYSTEM_DECLARED_WARNING
   char  *c1,*c2;
#endif
   auto PRIB_HASHATOM  var;
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   register int  i;


   if ( severity < 0 && severity > (sizeof(severity)/sizeof(char*)) )
     return 1;

#ifndef SYSTEM_DECLARED_WARNING
   /* If redeclaration warning, just return without printing anything
    * given something like the following testcase:
    *    echo 'Declare "P" "uniform string"' | typerib
    */
   if (error->type == kRIB_ERRTYPE_SYSTEM_PARAM)
      return 0;
#endif

   s = RibGetErrorMsg( code );

   if ( s )
   {
      if ( rib )
      {
         if (rib->status & kRIB_STATUS_ENCODEDDATA)
         {
            fprintf( stderr, "%s: \"%s\", line %u: byte offset %u: %s.\n",
                    severities[severity], rib->filename, rib->linecount, 
                    rib->byteoffset-1, s );
         }
         else
         {
            fprintf( stderr, "%s: \"%s\", line %u: %s.\n",
                    severities[severity], rib->filename, rib->linecount, s );
         }
      }
      else
      {
         fprintf( stderr, "%s: %s\n", severities[severity], s );
      }

      if ( error != NULL 
          && error->type < kRIB_ERRTYPE_LAST 
          && (rib->options & kRIB_OPTION_ERRMSG_VERBOSE) )
      {
         i = error->type;
         if ( i>=kRIB_ERRTYPE_FIRST_MISSING && i<=kRIB_ERRTYPE_LAST_MISSING)
         {
            switch(i) 
            {
             case kRIB_ERRTYPE_MISSING_P:
               s = "\"P\"";
               break;
             case kRIB_ERRTYPE_MISSING_P_AND_PZ:
               s = "\"P\" or \"Pz\"";
               break;
             case kRIB_ERRTYPE_MISSING_P_AND_PW:
               s = "\"P\" or \"Pw\"";
               break;
            }
            fprintf( stderr, "    Required %s is missing.\n", s );
         }
         else if ( i>=kRIB_ERRTYPE_FIRST_UNDEF && i<=kRIB_ERRTYPE_LAST_UNDEF )
         {
            t = NULL;
            switch(i)
            {
             case kRIB_ERRTYPE_UNDEFINED_TOKEN:
               s = "binary token";
               break;
             case kRIB_ERRTYPE_UNDEFINED_LIGHT:
               s = "light handle";
               break;
             case kRIB_ERRTYPE_UNDEFINED_OBJECT:
               s = "object handle";
               break;
             case kRIB_ERRTYPE_UNDEFINED_RI_CODE:
               s = "RI request code";
               break;
             case kRIB_ERRTYPE_UNDEFINED_REQUEST:
               s = "RI request"; t = error->unexpected.user_string;
               break;
             case kRIB_ERRTYPE_UNDEFINED_STRING:
               s = "string reference"; 
               break;
            }
            if (!t)
            {
               fprintf( stderr, "    Invalid/undefined %s: %d\n", 
                       s, error->unexpected.user_int );
            }
            else
            {
               fprintf( stderr, "    Undefined %s: %s\n", s, t );
            }
         }
         else if ( kRIB_ERRTYPE_NUMBER_OF_VALUES == i )
         {
            /*  kRIB_ERRTYPE_NUMBER_OF_VALUES:
             *   
             *    error->var points to a hash atom that identifies the 
             *               parameter ID (token) that was read.
             *    error->expected.sys_int is the number of values that
             *               should have been given.  The number measures
             *               number of floats or integers, not points or
             *               hpoints.
             *               If the number of values was unknown at the time
             *               of reading the data, set error->expected.sys_int
             *               kRIB_PARAML_UNKNOWN_SIZE.
             *    error->unexpected.user_int is the number of floats or
             *               integers actually found in the RIB file.
             */
            v = nullstr;
            s = NULL;
            t = NULL;
            var = error->var;
            if (var)
            {
               v = (char*)var->data;
               RibGetClassTypeNames( var->code, &s, &t );
            }
            if ( v && s && t )
            {
               fprintf( stderr, 
                       "    Wrong number of values for \"%s\" (%s %s).\n",
                       v, t, s );
            }
            else
            {
               fprintf( stderr, "    Wrong number of values.  \n" );
            }
            if ( error->expected.sys_int==kRIB_PARAML_UNKNOWN_SIZE )
            {
               fprintf( stderr, "    Found %d value(s).\n",
                       error->unexpected.user_int );
            }
            else
            {
               fprintf( stderr, "    Expected %d value(s) found %d.\n",
                       error->expected.sys_int, error->unexpected.user_int );
            }

         }
         else if ( i>=kRIB_ERRTYPE_FIRST_MISC && i<=kRIB_ERRTYPE_LAST_MISC )
         {
            s = NULL;
            switch (i)
            {
             case kRIB_ERRTYPE_OPENING_FAILED:
               fprintf( stderr, "    Could not open file \"%s\".\n",
                        error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_SYNTAX:
               s = "Syntax Error.  Skipping to next comment, "\
                   "encoded definition or RIB call.";
               break;
             case kRIB_ERRTYPE_OPEN_BRACKET: 
               /* Missing open bracket for array. */
               s = "Expected '[' to mark start of array.";
               break;
             case kRIB_ERRTYPE_CLOSE_BRACKET:
               /* Missing close bracket for array. */
               s = "Expected ']' to mark end of array.";
               break;
             case kRIB_ERRTYPE_BAD_ENCODED_INT:
               s = "Bad encoded integer.";
               break;
             case kRIB_ERRTYPE_BAD_ENCODED_FLOAT:
               s = "Bad encoded float.";
               break;
             case kRIB_ERRTYPE_BAD_DECLARATION:
               fprintf( stderr, 
                       "    Bad declaration for: \"%s\" as \"%s\".\n", 
                       error->expected.sys_string,
                       error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_CLASSNAME:
               fprintf( stderr, 
                       "    Bad declaration: Unknown class for: "  \
                       "\"%s\" declared as \"%s\".\n", 
                       error->expected.sys_string,
                       error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_TYPENAME:
               fprintf( stderr, 
                       "    Bad declaration: Unknown type for: "  \
                       "\"%s\" declared as \"%s\".\n", 
                       error->expected.sys_string,
                       error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_RIREQUEST:
               fprintf( stderr, "    Unknown request \"%s\".\n",
                       error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_PATCHTYPE:
               fprintf( stderr, "    Bad patch type: \"%s\".  Expected " \
                       "\"bilinear\" or \"bicubic\".\n", 
                       error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_CURVETYPE:
               fprintf( stderr, "    Bad curve type: \"%s\".  Expected " \
                       "\"linear\" or \"cubic\".\n", 
                       error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_PROCEDURALTYPE:
               fprintf( stderr, "    Bad procedural type: \"%s\".  Expected " \
                       "\"DelayedReadArchive\", \"DynamicLoad\", or "         \
                       "\"RunProgram\".\n", 
                       error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_BASISNAME:
               fprintf( stderr, "    Bad basis name: \"%s\".\n", 
                        error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_BASISUSTEP:
             case kRIB_ERRTYPE_BAD_BASISVSTEP:
               fprintf( stderr, "    Bad basis %cstep value: %d.\n", 
                        (i==kRIB_ERRTYPE_BAD_BASISUSTEP ? 'u' : 'v'),
                        error->unexpected.user_int );
               break;
             case kRIB_ERRTYPE_BAD_WRAPMODE:
               fprintf( stderr, "    Bad wrapping mode: \"%s\".  Expected " \
                       "\"periodic\" or \"nonperiodic\".\n", 
                       error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_FILTERNAME:
               fprintf( stderr, "    Bad filter name: \"%s\".\n", 
                               error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_ERRORHANDLER:
               fprintf( stderr, "    Bad error handler name: \"%s\".\n", 
                               error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_BAD_FRAMEBEGIN:
               s = "Bad FrameBegin statement given.";
               break;
             case kRIB_ERRTYPE_BAD_FRAMEEND:
               s = "Bad FrameEnd statement given.";
               break;
             case kRIB_ERRTYPE_BAD_WORLDBEGIN:
               s = "Bad WorldBegin statement given.";
               break;
             case kRIB_ERRTYPE_BAD_WORLDEND:
               s = "Bad WorldEnd statement given.";
               break;
	     case kRIB_ERRTYPE_BAD_COLORSAMPLES:
               s = "ColorSamples given inside a Frame or World block.";
	       break;
             case kRIB_ERRTYPE_SYSTEM_PARAM:
#ifdef SYSTEM_DECLARED_WARNING
               if (!error->var)
                 return kRIB_ERRRC_INT;
               RibGetClassTypeNames( error->var->code, &s, &t );
               RibGetClassTypeNames( error->unexpected.user_int, &c1, &c2 );
               fprintf( stderr, "    System declared parameter \"%s\" "   \
                       "previously declared \"%s %s\" is redeclared as "  \
                       "\"%s %s\".\n", (char*)error->var->data, s, t, c1, c2 );
#endif
	       s = NULL;
               break;
             case kRIB_ERRTYPE_UNDECLARED_PARAM:
               fprintf( stderr, "    User parameter \"%s\" is of unknown " \
                       "type.\n", error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_INCONSISTENT_PARAM:
               if (!error->var)
                 return kRIB_ERRRC_INT;
               RibGetClassTypeNames( error->var->code, &s, &t );
               fprintf( stderr, "    User parameter \"%s\" is inconsistent" \
                       "with expected type \"%s %s\".\n", 
                       (char*)error->var->data, s, t );
               break;
             case kRIB_ERRTYPE_UNEXPECTED_EOF:
               s = "Unexpected end of file.";
               break;
             case kRIB_ERRTYPE_ENCODED_ARRAY_SIZE:
               fprintf( stderr, 
                       "    Encoded array of floats is the wrong size.\n" \
                       "    Expected %d float(s), but given %d.\n",
                       error->expected.sys_int, error->unexpected.user_int );
               break;
             case kRIB_ERRTYPE_FLOAT_ARRAY_SIZE:
               fprintf( stderr, 
                       "    Array of floats is the wrong size.\n" \
                       "    Expected %d float(s), but given %d.\n",
                       error->expected.sys_int, error->unexpected.user_int );
               break;
             case kRIB_ERRTYPE_INTEGER_ARRAY_SIZE:
               fprintf( stderr, 
                       "    Array of integers is the wrong size.\n" \
                       "    Expected %d integer(s), but given %d.\n",
                       error->expected.sys_int, error->unexpected.user_int );
               break;
             case kRIB_ERRTYPE_STRING_ARRAY_SIZE:
               fprintf( stderr, 
                       "    Array of strings is the wrong size.\n" \
                       "    Expected %d string(s), but given %d.\n",
                       error->expected.sys_int, error->unexpected.user_int );
               break;
             case kRIB_ERRTYPE_TRIMCURVE_NCVS:
               fprintf( stderr, 
                       "    Given number of control points is wrong.\n" \
                       "    Expected %d, but given %d.\n",
                       error->expected.sys_int, error->unexpected.user_int );
               break;
             case kRIB_ERRTYPE_REDEF_SYS_VAR:
               fprintf( stderr, "    Redefining variable %s.\n", 
                       error->expected.sys_string );
               break;
             case kRIB_ERRTYPE_VERSION_TOO_NEW:
	       /* Testcase:
		*   %echo 166 145 162 163 151 157 156 212 003 007 377 043 141
                * 142 040 | oct2bin | typerib
		* Refer to affine/readrib/tests/vererr.bin.
		*/
               fprintf( stderr, 
                       "    Version %g was specified and is newer than "\
                       "library's version level of %g.\n",
                       error->unexpected.user_float,
                       error->expected.sys_float );
               break;
             case kRIB_ERRTYPE_N_COLORSAMPLES:
               fprintf( stderr, 
                       "    Bad integer value %d was given for the number " \
                       "of color samples.\n", error->unexpected.user_int );
               break;
             case kRIB_ERRTYPE_ADDING_TO_HASH:
               s = "Couldn't add item to hash table.  Maybe out of memory.";
               break;
             case kRIB_ERRTYPE_DECIMATEDINT:
               s = "Found decimal point following an integer value.";
               break;
             case kRIB_ERRTYPE_UNSUPPORTED_RIB:
               fprintf( stderr, 
			"Unsupported RIB statement (%s) encounterd.",
			error->unexpected.user_string );
               break;
             case kRIB_ERRTYPE_UNKNOWN_INTERNAL:
               s = "Unknown internal error occurred.";
               break;
            }
            if (s)
            {
               fprintf( stderr, "    %s\n", s );
            }
         }
      }
   }
   else
   {
      return kRIB_ERRRC_INT;
   }

   return kRIB_OK;
}
