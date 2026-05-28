/* $RCSfile: string.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 06:30:52 $
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
 * FILE:  string.c
 *
 * DESCRIPTION:  Token ID function (lexing).
 *   
 *    Contains:
 * 
 *    References:
 *          [PIXA89]  Pixar Animation Studios, The RenderMan Interface, 
 *                    Version 3.1, Richmond, CA, September 1989.
 *
 *    History:
 *          01-04-98  Fixed bug in RibGetUserParameters() that left
 *                    unitialized parts of the output array.
 *                    
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 *
 */
#define __RIB_FILE_STRING
#define _RIPRIV_TYPES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ripriv.h>
#include <ribhash.h>


int RibReadStringForToken( char *s, char *table, char **pp )
{
   auto int   i, n, l;
   auto char  *t = table;
   auto int   c;
   auto char  *p = s;


   c = *p;
   for(;;)
   {
      n = *t++;
      l = *t++;

      for ( i=0; i < l; i++ )
      {        
         if ( c != *t || !*t )
         {
            if ( !*t && (RibIsaWhiteSpace(c) || !c 
                         || RibIsaBinaryChar(c) || RibIsaDelimitor(c)) )
            {
               t++;
               if (pp)
                 *pp = p;
               return *t;
            }
            else if ( !i ) 
            {
               if ( n )
               { 
                  t += n;
                  break;
               }
            }
            goto NotFound;
         }
         else
         {
            t++;
            p++;
            c = *p;
         }
      } /* for (i=...) */
   } /* for (;;) */

 NotFound:

   return kRIB_ERRRC_INDEX; /* Not found */
}


char *RibGetTokenName( char *name )
{
   int   i;
   char  *p = name;


   /* Get class. */
   i = RibReadStringForToken( p, gRibClassTable, &p );
   if (i<0)
   {
      /* Give-up and go back to the beginning. */
      return name;
   }

   /* If i was set to a class then there should be whitespace. */
   if ( i!=0 && !isspace(*p) )
   {
      return kRIB_ERRRC_PTR; 
   }

   while( isspace(*p) )
     p++;

   /* Get type. */
   i = RibReadStringForToken( p, gRibTypeTable, &p );
   if (i<0)
   {
      return kRIB_ERRRC_PTR; 
   }

   /* Get array depth.  Default to 1. */
   if ( *p == '[' )
   {
      p++;
      i = 0;

      while ( *p >= '0' && *p <= '9' && i < 5 )
      {
         p++;
         i++;
      }

      if ( *p != ']'  )
        return kRIB_ERRRC_PTR; /* error bad array number given */

      p++;
   }

   while( isspace(*p) )
     p++;

   return p;
}

int RibGetUserParameters( char *table, /* token table */
                         int ntable, /* number of tokens in parse table */
                         RtInt n, /* n from vector format of user data */
                         RtToken tokens[], /* tokens vector format */
                         RtPointer parms[], /* parameters vector format */
                         RtPointer output[] ) /* list of parameters found */
{
   register int  i,index, initNull;
   char          *token;
   
   initNull = 0;
   for ( i=0; i<n; i++ )
   {
      token = RibGetTokenName( tokens[i] );
      index = RibReadStringForToken( token, table, NULL );
      if (index >= 0)
      {
         while ( initNull < index )
         {
            output[initNull++] = NULL;
         }
         output[index] = parms[i];
         if ( index == initNull )
            initNull++;
      }
   }
   while ( initNull < ntable )
   {
      output[initNull++] = NULL;
   }

   return kRIB_OK;
}


