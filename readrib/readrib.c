/* $RCSfile: readrib.c,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:07 $
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
 * FILE:  readrib.c
 *
 * DESCRIPTION:  Library that reads both ASCII and binary RIB files.
 *   
 *    Contains:
 *       int RibReadRib( RIB_HANDLE rib );
 *
 *     History:
 *      01-08-98  Added fix for "comment migration" problem.
 *      04-04-98  Changed conditional for first letter of a statement name.
 *      06-09-99  Added support for rib->rilevel.  This is for DSO support.
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
#define __RIB_FILE_READRIB
#include <stdio.h>
#include <ripriv.h>
#include <ribrdr.h>


int RibRead( RIB_HANDLE hrib )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   int  GotLostInRib=0;
   int  c;


   if (!rib || !rib->ritable)
     return kRIB_ERRRC_INT; /* Error */

   RibBegin( rib, RI_NULL );

   while ( EOF!=(c=RibGetChar( rib )) )
   {
      /* See "comment migration" comments in paraml.c. */
      if (rib->arcreclist)
         RibArcRecPlayBack( rib );

      /* Check top bit (0x80 bitmask, 0200 in octal).
       *   If set then c is a binary RIB character. 
       */
      /* Handle binary RIB character. */
      if ( 0200 & c ) 
      {
         if (RibIsanEncodedDefinition(c))
         {
            /* Handle string definition or RI request definition. */
            c = RibReadDefiningBinCode( rib, c );
            if ( c )
               goto Error;
         }
         else if (RibIsanEncodedRIRequest(c))
         {
            /* Handle RI request. */
            c = RibReadRIRequestToken( rib );
            if ( c < 0 )
	       goto Error;

	    /* If the Affine Libraries are set up as dynamically linked
	     *    and a tokenname is newer than a tool can take then
	     *    do not return an error.  Instead somehow skip over it
	     *    maybe by calling the reading function and have later
	     *    reading functions each check rilevel or maybe declare
	     *    parser lost in RIB till the next RIB statement.
	     */
            if ( c > rib->rilevel )
	    {
	       /* Maybe an old tool should give a syntax error. */
	       GotLostInRib=1;
	       rib->error.type = kRIB_ERRTYPE_UNSUPPORTED_RIB;
	       rib->error.unexpected.user_string = gRibNames[c];
	       RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
	       c = kRIB_ERRTYPE_SYNTAX;
	       goto Error;
	    }

            /* Handle RI binding. */
            c = (*gRibReadTable[c])( rib );
            if ( c )
               goto Error;
         }
         else
         {
            /* Handle invalid encoded byte. */
            c = kRIB_ERRTYPE_SYNTAX;
            goto ErrorMsg;
         }
      }
      /* Handle ASCII RIB character. */
      else
      {
         /* Handle any white spaces. */
         if ( RibIsaWhiteSpace(c) )
         {
            continue;
         }

         /* Handle archive record (# and ##). */
         if ( '#' == c )
         {
            c = RibHandleArchiveRecord( rib );
            if ( c )
               goto Error;
         }
         /* Handle RenderMan Interface Bytestream binding. */
         else
         {
            /* Map first letter to an index into gRibRITokenTables[]. */
            if ( c >= 'a' )
            {
               if ( 'v' == c )
               {
                  /* Map "version" RIB statement to letter X.  No RIB
                   *   statements start with a capital 'X' and since
                   *   "version" is the only one that starts with a lower
                   *   case letter, the 'X' position in the table seemed
                   *   as good as any place to hook in the "version"
                   *   statement.
                   */
                  c = 'X';
               }
               else
               {
                  /*  Spec has the statements starting with capital letters,
                   *    but Pixar's programs do take RIB statements starting
                   *    with lower case letters if the statement's name does
                   *    not have any capital letters in the middle of the name
                   *    given in the spec.  (That's my latest theory anyway).
                   */
                  /* Ok, I was doing the following until I started finding
                   *   the above to be true.  So I'm just going to declare
                   *   all RIB statements that start with lower case letters
                   *   to be syntax errors.
                   */
                  /* c -= ('a' - 'A'); *//* Make first letters uppercase. */
                  c = kRIB_ERRTYPE_SYNTAX;
                  goto ErrorMsg;
               }
            }
            else if ( 'X' == c )
            {
               c = kRIB_ERRTYPE_SYNTAX;
               goto ErrorMsg;
            }

	    /* Make c from A to Z to 0 to 25. */
	    c -= 'A';

            /* RI call's first character has been read.  Now read the rest. */
            if ( c>=('A'-'A') && c<=('X'-'A') && gRibRITokenTables[c] )
            {
               RibSaveToBuffer( rib, RI_FALSE );
               c = RibReadFileForToken( rib, gRibRITokenTables[c] );

               /* Check for invalid token. */
               if ( c < 0 )
               {
                  /* To keep the byte offset correct, print the error 
                   *    message now, then unget the last character.
                   */
                  GotLostInRib=1;
                  rib->error.type = kRIB_ERRTYPE_SYNTAX;
                  RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
                  c = kRIB_ERRTYPE_SYNTAX;
                  goto Error;
               }
               else
               {
		  if ( c > rib->rilevel )
		  {
		     /* Maybe an old tool should give a syntax error. */
		     GotLostInRib=1;
		     rib->error.type = kRIB_ERRTYPE_UNSUPPORTED_RIB;
		     rib->error.unexpected.user_string = gRibNames[c];
		     RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
		     c = kRIB_ERRTYPE_SYNTAX;
		     goto Error;
		  }

                  /* Handle RI binding. */
                  c = (*gRibReadTable[c])( rib );
                  if ( c )
                    goto Error;
		  rib->status &= ~kRIB_STATUS_KEEP_DATA;
                  if ( rib->status & kRIB_STATUS_ENDED )
                  {
                     return 0;
                  }
               }
            }
            else
            {
               /* Handle syntax error. */
               c = kRIB_ERRTYPE_SYNTAX;
               goto ErrorMsg;
            }
         }
      }
      if (GotLostInRib)
      {
         GotLostInRib=0;
      }
      continue;

      
    Error:
      if ( EOF==c)
      {
         break;
      }
      if (!GotLostInRib)
      {
         GotLostInRib=1;
      }
      continue;

    ErrorMsg:
      if ( EOF==c )
      {
         rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
         RibSetError( rib, RIE_SYNTAX, RIE_SEVERE, &(rib->error) );
         break;
      }
      else
      {
         /* Avoid printing more errors after the first syntax error. 
          * Try to find the next valid RIB statement, comment or encoded
          *    definition.  This appears to be the behavior in catrib 3.6c.
          */
         if (!GotLostInRib)
         {
            GotLostInRib=1;
            rib->error.type = kRIB_ERRTYPE_SYNTAX;
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         }
      }
   }
   /* See "comment migration" comments in paraml.c. */
   if (rib->arcreclist)
      RibArcRecPlayBack( rib );

   return 0;
}

