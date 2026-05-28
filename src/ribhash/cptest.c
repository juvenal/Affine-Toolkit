/* $RCSfile: cptest.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 06:30:52 $
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
 * FILE:  cptest.c
 *
 * DESCRIPTION:  Gravity check for cparams.o.
 *   
 *    Contains:
 * 
 *    References:
 *
 */
#define _RIPRIV_TYPES
#include <stdio.h>
#include <ribrdr.h>


int main(int argc, char **argv)
{
   int   i,c;
   char  *p;


   for ( i=0; i < kRIB_LAST_RI; i++ )
   {
      p = gRibCParams[i];
      c=0;
      if ( i != kRIB__VERSION )
      {
         while (p[c]!=kRIB_CPARAMS_NULL && p[c]!=kRIB_CPARAMS_VECTORFORMAT)
           c++;
         printf( "Ri%s%s( ", gRibNames[i], 
                (p[c]==kRIB_CPARAMS_VECTORFORMAT ? "V" : "" ) );
         c = 0;
      }
      else
      {
         printf( "%s( ", gRibNames[i] );
      }
      for(;;)
      {
         c++;
         switch (*p)
         {
          case kRIB_CPARAMS_VECTORFORMAT:
            printf( "RtInt n, RtToken tokens[], RtPointer pointers[]" );
            break;
          case kRIB_CPARAMS_TOKEN:
            printf( "RtToken" );
            break;
          case kRIB_CPARAMS_POINTER:
            printf( "RtPointer" );
            break;
          case kRIB_CPARAMS_INT:
            printf( "RtInt" );
            break;
          case kRIB_CPARAMS_FLOAT:
            printf( "RtFloat" );
            break;
          case kRIB_CPARAMS_MATRIX:
            printf( "RtMatrix" );
            break;
          case kRIB_CPARAMS_BOUND:
            printf( "RtBound" );
            break;
          case kRIB_CPARAMS_BASIS:
            printf( "RtBasis" );
            break;
          case kRIB_CPARAMS_COLOR:
            printf( "RtColor" );
            break;
          case kRIB_CPARAMS_POINT:
            printf( "RtPoint" );
            break;
          case kRIB_CPARAMS_LIGHTHANDLE:
            printf( "RtLighthandle" );
            break;
          case kRIB_CPARAMS_OBJECTHANDLE:
            printf( "RtObjecthandle" );
            break;
          case kRIB_CPARAMS_BOOLEAN:
            printf( "RtBoolean" );
            break;
          case kRIB_CPARAMS_CHAR_POINTER:
            printf( "char*" );
            break;
          case kRIB_CPARAMS_ERRORHANDLER:
            printf( "RtErrorhandler" );
            break;
          case kRIB_CPARAMS_FUNC:
            printf( "function pointer" );
            break;
          case kRIB_CPARAMS_FILTERFUNC:
            printf( "filterfunc" );
            break;
          case kRIB_CPARAMS_TOKEN_ARRAY:
            printf( "RtToken[]" );
            break;
          case kRIB_CPARAMS_POINTER_ARRAY:
            printf( "RtPointer[]" );
            break;
          case kRIB_CPARAMS_INT_ARRAY:
            printf( "RtInt[]" );
            break;
          case kRIB_CPARAMS_FLOAT_ARRAY:
            printf( "RtFloat[]" );
            break;
          case kRIB_CPARAMS_POINT_ARRAY:
            printf( "RtPoint[]" );
            break;
          case kRIB_CPARAMS_VARIABLE_ARG:
            printf( "va_arg" );
            break;
          default:
            c--;
         }
         if ((*p==kRIB_CPARAMS_NULL || *p==kRIB_CPARAMS_VECTORFORMAT) && c)
         {
            printf( " );\n" );
            break;
         }
         else if (!c)
         {
            printf( "void );\n" );
            break;
         }
         p++;
         if (*p)
         {
            printf(", ");
         }
      }
   }

   return 0;
}
