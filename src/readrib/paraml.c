/* $RCSfile: paraml.c,v $  $Revision: 1.2 $ $Date: 1999/06/13 05:16:58 $
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
 * FILE:  paraml.c
 *
 * DESCRIPTION:  Read the parameter list of a RIB statement.
 *   
 *    Contains:
 * 
 *     History:
 *       6-19-97  Arrays of strings were not supported as of 3.6.  I don't
 *                have 3.7 yet, but a posting by Larry Gritz indicates that 
 *                PRMan 3.7 supports atleast arrays of strings when there 
 *                are multiple uniform values across a piece of geometry.  
 *                Added support for string arrays.
 *       7-10-97  Finally got 3.7 today.  Added class "constant" and 
 *                type "matrix".  Added support for new Points primitive.
 *      12-12-97  Comment grammar checked.
 *      01-07-98  Added comment about kRIB_STANDALONETYPE.
 *      01-08-98  Added fix for "comment migration" problem.
 *      03-04-98  Fixed dangling comment written below concerning fix added
 *                on 01-08-98.  Changed comments about Points and Polygon
 *                concerning the reading of parameter list data.
 *      03-15-98  Change to support zero length arrays in parameter list.
 *                See comment below in readfunc.c's ReadFloatingPointArray() 
 *                by searching for "3-15-98".
 *      03-28-98  Both rib->typecache[n] and rib->ncache[n] are assigned
 *                values even when type != kRIB_PARAML_UNKNOWN_SIZE.
 *                Fixed bugs with bad parameter data causing memory leaks.
 *      04-25-98  Added code to set the values rib->lastnuniform, 
 *                rib->lastnvarying and rib->lastnvertex, so functions in the
 *                Ri table can query for the number of values to expect for
 *                a parameter rather than waste time recalculating it.
 *      06-11-99  Hopefully fixed inline declarations and clean up if an
 *                undeclared token in a param list is encountered.
 *
 *    References:
 *          [GRITZ97] Gritz, Larry, Re: texture names per mesh face?,
 *                    comp.graphics.rendering.renderman, June 6, 1997.
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
#define __RIB_FILE_PARAML
#define _RIPRIV_TYPES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ripriv.h>
#include <ribrdr.h>


#define  CheckForError(rc)  if (rc) goto Error
#define  CheckForErrorAndCleanUp(rc)  if (rc) goto ErrorCleanUp


/* RequiredTable was originally created with the following line command:
 *   
 *       tokentbl -s required.asc RequiredTable
 *
 */
static char RequiredTable[] = {
   0,  1 , 'P',
   2,  2 ,'\0',  0,
   3,  3 ,'w','\0',  1,
   0,  3 ,'z','\0',  2
};
enum {
   REQUIRE_P,
   REQUIRE_PW,
   REQUIRE_PZ
};


int RibReadParameterList( RIB_HANDLE hrib, int require,
                         int uniform, int varying, int vertex,
                         RtInt *pn, RtToken **paTokens, RtPointer **paParams ) 
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   PRtToken        pTokens = NULL;
   PRtPointer      pParams = NULL;
   char            *s, *name;
   PRIB_HASHATOM   p;
   int             n=0;
   int             ntype;
   int             type;
   PReadArrayFunc  pf;
   auto int        c, rc;


   if (!rib)
     return kRIB_ERRRC_INT;

   /*NOTE:   -- FIXED 01-08-98 --
    *
    *   Version 3.6 of catrib and its predecessors suffered from 
    *   "comment migration" where a comment located in the middle 
    *   or end of a parameter list would be printed by catrib just 
    *   before the RIB statement that has the parameter list. 
    *   The Affine RIB reader suffers from "comment migration"
    *   as well, but since the Pixar tools did too, I thought -- OK.
    *   But now catrib 3.7 has fixed/altered how it deals with
    *   this problem, I'll now look into changing this "feature" 
    *   soon.  
    *   Little note to myself: catrib 3.7 migrates comments in the
    *   middle of an array to being at the end of the entire RIB
    *   statement and its parameter list.  Catrib 3.6
    *   didn't migrate things to the end, but to the beginning.
    *   Also note that there really is no way to fix migration
    *   of comments in the middle of a RIB statement because that
    *   would require the Ri calls to allow a description of 
    *   a set of comments and where to place them.  Basically
    *   RiArchiveRecord statement can be called before or after
    *   a Ri call, but not in the middle.
    */
   /* 01-08-98 -- Added fix for "comment migration". 
    *
    *   Just before reading a parameter list set a flag to tell the
    *   archive record handler to store up comments and hints
    *   into a linked list of strings.   Once it is known that an entire 
    *   parameter list has been read and the only thing that follows is 
    *   another RIB statement or EOF, then play back the stored up 
    *   comments and hints to the function pointer 
    *   rib->ritable[kRIB_ARCHIVERECORD].
    */
   RibSetStatus( rib, kRIB_STATUS_PARAML, kRIB_STATUS_PARAML );
   for (;;)
   {
      /* Read possible comments, RI and string definitions before the 
       *    first token of the parameter list.  
       */
      c = RibHandlePrefix( rib );

      /* If the character c returned by RibHandlePrefix() does not mark the
       *   beginning of a string then there is no token and no more items in 
       *   the parameter list.
       * Either way, unget the read-in character c so the next RibRead...() 
       *   function can have a look at it. 
       */
      if (c!=EOF)
      {
         rc = RibUngetChar( rib, c );  CheckForError(rc);
      }
      if ( !('\"'==c || 0317==c || (c>=220 && c<=0243)) )
      {
         /* See above comments on "comment migration. */
	 /* Change present state to no longer mark reading a param list. */
         RibSetStatus( rib, kRIB_STATUS_PARAML, 0 );
         break;
      }
      
      /* Read the token name (parameter ID) such as "P", "Cs" or 
       *    "texturename". 
       */
      rc = RibReadString( rib, kRIB_BRACKETS_NONE, &s );  CheckForError(rc);

      /* Remember s for a comparison after RibHandleInLineDeclaration(). */
      name = s;

      /* Check for inline declaration.  Remember s for a comparison later. */
      p = RibHandleInLineDeclaration( rib, &name );

      /* If p not NULL then the name string pointer would have been
       *    changed, so an inline declaration was found in string s
       *    and name points to a copy of the name that is after the 
       *    inline declaration in s.  The pointer name will point
       *    to a string in a hash table or remain equal to s which it
       *    was set to above before RibHandleInLineDeclaration() was 
       *    called.
       */
      if ( rib->options & kRIB_OPTION_EXPAND_INLINE && p )
      {
	 int   l;
	 char  *c;

	 /* Change the data pointer to be a copy of name. */
	 l = strlen( name ) + 1;
	 c = (char*)_RibMalloc( l );
	 if (!c)
	 {
	    /* Free token declaration/name read by RibReadString() above. */
	    _RibFree(s);

	    /* Note that at this point pTokens[n] and pParams[n] do
	     *   not point to anything and the arrays pTokens and 
	     *   pParams may not even have n elements in them at this
	     *   point.  Doing a goto ErrorCleanUp might cause a problem
	     *   when it tries to free the data pointed to by pTokens[n] 
	     *   and pParams[n].
	     * So skip ErrorCleanUp and goto ErrorCleanUpInlineList.
	     */
	    goto ErrorCleanUpInlineList;
	 }
	 strcpy( c, name );
	 _RibFree(s);
	 s = c;
      }

      /* No in-line declaration given, check hash table. */
      if (!p)
      {
	 /* Find the token in the hash table to see how it was declared. */
	 p = RibFindMatch( (RIB_HASHHND)rib->phashtable, kRIB_HASH_VARIABLE, 
			   kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, name );
      }

      if (p)
      {
         /* The token was found in the hash table so reserve space in a 
          *    buffer for a pointer to the token and a pointer to its 
          *    soon to be read data.
          */
         rc = RibCacheVector( rib, 1, &pTokens, &pParams ); 
         
         /* If the return code indicates an error, then nothing was allocated
          *    so don't do any of the error clean up.  Just goto Error. 
          */
         CheckForError(rc); 
         
         /* The variable name being dealt with is the token s. */
         pTokens[n] = (RtToken)s;
         
         /* Set pParams[n] to NULL to prevent any error handling code from
          *   trying to free an older call's string or array.  Inshort there
          *   might be junk in this buffer from a previous call so don't
          *   accidentally use it.
          */
         pParams[n] = NULL; 

         /* The type is referred to frequently so store it in "type".  The
          *    type identifies a token's declaration as being a string, 
          *    point, hpoint, float, integer, and a few others listed
          *    in a switch(type) statement a page or two below.
          */
         type = kRIB_TYPE_MASK & p->code;

         /* A token can be declared as a "uniform string".  The Pixar 
          *    catrib (3.6c) utility seems to atleast try to read in files 
          *    with tokens declared as "varying string", "vertex string"
          *    and different combinations of "string[n]" where n is an 
          *    integer.  These declarations and especially the data that
          *    has arrays of strings cause slight problems with the
          *    Pixar utilities (version 3.5 and 3.6c), so I'll take that 
          *    as a hint and just read single strings and report errors
          *    with the more adventurous string declarations.  
          *
          * 6-19-97:  I don't have the 3.7 update yet, but a posting by LG
          *      states that geometry with multiple uniform values 
          *      and tokens declared as "uniform strings" work now.
          *      So now the possibility of string arrays needs to be 
          *      handled in paraml.c and readfunc.c.
          */
         /*REMOVED 6-19-97 see above comment.
          *       
          ** If the token is a string, skip the rest of this function's code 
          **    and simply read the incoming data as a string.
          **
          * if ( kRIB_STRINGTYPE==type )
          * {
          *    rc = RibReadString( rib, 
          *                     kRIB_BRACKETS_OPTIONAL | kRIB_STRING_RTSTRING, 
          *                     &pParams[n] ); CheckForErrorAndCleanUp(rc);
          *    n++;
          *    continue;
          * }         
          *REMOVED 6-19-97 see above comment.
          */
         
         /* A token in the parameter list is declared with a class and a type.
          *    The classes are "varying", "vertex" and "uniform" with uniform 
          *    as a default.  Each geometric primitive can specify how much 
          *    data is needed for each class.  Uniform doesn't always mean 
          *    one value is needed.  A bilinear patch takes only one uniform 
          *    value, but a PatchMesh of several bilinear patches takes a 
          *    uniform value for each patch.
          */
         switch ( kRIB_CLASS_MASK & p->code ) 
         {
          case kRIB_VARYINGCLASS:
            ntype = varying;
            break;
          case kRIB_VERTEXCLASS:
            ntype = vertex;
            break;
          case kRIB_CONSTANTCLASS:
            ntype = 1;
            break;
            /* Let default handle kRIB_UNIFORMCLASS. */
          default:
            ntype = uniform;
         }
         
         
         /* NOTE:  Points also acts like Polygon when reading positional
          *        data.  So Polygon is no longer alone.  7-10-97
          *
          * Polygon and Points are unique from the other RIB bindings in the
          *    way their parameter lists are read.  The other bindings have 
          *    known sizes for their vertex and varying parameters, but 
          *    Polygon and Points require a RIB reader to figure it out by 
          *    itself.  
          *
          * Other bindings such as GeneralPolygon have arrays that the RIB
          *    reader has to read without knowing the size ahead of time, but
          *    these arrays are not in the parameter list.  
          *
          * Experiments with Pixar's catrib (3.6c) were done to see what is 
          *    acceptable behavior.  The following is a RIB file that  
          *    serves only as a test and will not render:
          *
          *  %cat polygon.rib
          *  Polygon "Cs" [ 1 2 3   ]
          *  "P" [ 11 22 33   44 55 66   77 88 99]
          *  "Os" [ 111 222 333]
          * 
          *  %catrib polygon.rib
          *  ##RenderMan RIB
          *  version 3.03
          *  Polygon "Cs" [1 2 3 11 22 33 44 55 66]
          *     "P" [11 22 33 44 55 66 77 88 99]
          *     "Os" [111 222 333 0 0 0 0 0 0]
          *
          * (Note: The above output has been modified from its original  
          *         version.  It has been formatted to fit your screen.)
          *
          * From the above example and one or two other experiments, it 
          *    appears that catrib reads the data for Cs, determines the size
          *    after the array is read into a buffer somewhere, allocates 
          *    memory for three floats, and copies the floats from the buffer
          *    into the allocated area.  When reading "P" it does the same 
          *    thing, but as according to the RenderMan spec it also 
          *    remembers the array size as the number of vertices for the 
          *    polygon.  Notice that even though the correct number of 
          *    is known after reading "P", catrib continues to handle each
          *    array as if they were of unknown length.  No error is given for
          *    "Os" not having enough floats in its array.
          *
          * When printing out the above Polygon statement, catrib has the 
          *    number of vertices and uses that number as the number of varying
          *    points Cs is expected to have.  Cs was wrongly given only
          *    three floats in the above example (as a test), it should have 
          *    been given nine floats.  Since the array was allocated with the
          *    smaller and incorrect size, the catrib program appears to 
          *    meander its way into P's allocated memory area. 
          *
          * Inshort, when reading a Polygon statement, the size of each array
          *    of data is determined on the fly even after the positional 
          *    array "P" has been read in.
          */
	 /* Note: ntype has been set to one of the given stack parameters 
	  *    vertex, varying, uniform or 1.
	  */
         if ( ntype == kRIB_PARAML_UNKNOWN_SIZE )
         {
            /* Assign c the flags to give RibReadArrayAndLength(). */
            switch (type)
            {
             case kRIB_INTTYPE:
             case kRIB_INTPAIRTYPE:
               c = kRIB_BRACKETS_PARAML | kRIB_ARRAY_INTEGER;
               break;
             case kRIB_FLOATTYPE:
             case kRIB_NORMALTYPE:
             case kRIB_POINTTYPE:
             case kRIB_VECTORTYPE:
             case kRIB_HPOINTTYPE:
             case kRIB_COLORTYPE:
             case kRIB_FLOATPAIRTYPE:
             case kRIB_MATRIXTYPE:
               c = kRIB_BRACKETS_PARAML | kRIB_ARRAY_FLOAT;
               break;
             case kRIB_STRINGTYPE:
               c = kRIB_BRACKETS_PARAML | kRIB_ARRAY_STRING;
               break;
             case kRIB_STANDALONETYPE:
               /* Store class, type and length of array into vector cache so
                * RibGetVectorCacheInfo() can be used to retrieve this data.
                */
                ((PRIB_UINT32)rib->typecache)[n] = p->code;
                ((PRIB_UINT32)rib->ncache)[n] = 0;
                
                /* Skip over the code that will try to read parameter data. */
                goto NextToken;
             default:
               rc = kRIB_ERRRC_INT;
               goto ErrorCleanUp;
            }

            /* Handle arrays of unknown size. */
            rc = RibReadArrayAndLength( rib, c,
                                       &ntype, (void**)&pParams[n] );
            CheckForErrorAndCleanUp(rc);

            /* The value ntype indicates the number of values
             *    in the array, not the number of the particular type 
             *    such as points or hpoints.
             */
            /* Convert ntype to be the number of "type" items. */
            switch (type)
            {
             case kRIB_INTTYPE:
             case kRIB_FLOATTYPE:
             case kRIB_STRINGTYPE:
               /* Leave ntype alone. */
               c = 1;
               break;
             case kRIB_NORMALTYPE:
             case kRIB_POINTTYPE:
             case kRIB_VECTORTYPE:
               c = 3;
               break;
             case kRIB_HPOINTTYPE:
               c = 4;
               break;
             case kRIB_COLORTYPE:
               c = rib->ncolorsamples;
               break;
             case kRIB_FLOATPAIRTYPE:
             case kRIB_INTPAIRTYPE:
               c = 2;
               break;
             case kRIB_MATRIXTYPE:
               c = 16;
               break;
             default:
               rc = kRIB_ERRRC_INT;
               goto ErrorCleanUp;
            }
            /* Refer to code after the following if statement to read about 
             *    array sizes.  Search for kRIB_ARRAY_DECLARED.
             */
            if (kRIB_ARRAY_DECLARED & p->code)
            {
               c = c * p->with.n;
            }
            
            /* If the number of values is not divisible by the the number
             *    of values associated with the type, then it has to be 
             *    an error.  Inshort 5 floats can not be possibly be an
             *    array of points which has an atomic size of 3 floats.
             */
            if ( ntype % c)
            {
               rib->error.type = kRIB_ERRTYPE_NUMBER_OF_VALUES;
               rib->error.expected.sys_int = kRIB_PARAML_UNKNOWN_SIZE;
               rib->error.unexpected.user_int = ntype;
               rib->error.var = p;
               RibSetError( rib, RIE_CONSISTENCY, RIE_ERROR, &(rib->error) );

               /* Free the bad data and remove it from the cache. 
                *    The pointers pTokens and pParams are assumed
                *    to be valid pointers since to get here, 
                *    RibCacheVector() returned no error when asked
                *    to assign values to pTokens and pParams.
                */
               if (pTokens[n])
                  _RibFree( pTokens[n] );
               if (pParams[n])
                  _RibFree( pParams[n] );
               RibCacheVector( rib, -1, NULL, NULL );
               
               /* Don't increment n. */
               continue;
            }
            else
            {
               ntype = ntype / c;
            }
            
            /* Don't just skip over the rest of the loop until the "require"
             *    variable can be cleared after pTokens[] is checked for 
             *    any needed "P", "Pw" or "Pz" data.
             */
            /* Save the fact that the array was originally of unknown size. */
            type = kRIB_PARAML_UNKNOWN_SIZE;
         } /* end of if ( ntype == kRIB_PARAML_UNKNOWN_SIZE ) */
         
         /* The variable ntype is now set with the length of the data,
          *    regardless if the array length was known up front or not.
          *
          * So now see if any requirements have been met and while checking
          *    for "P", "Pw" and "Pz" we might as well note the number of
          *    vertices there were.  See RibReadPolygon() in ribfunc.c
          *
          * Some calls such as NuPatch require a position array to be in the
          *    parameter list.  NuPatch requires that either a "P" or a "Pw"
          *    be in the list.  The variable required is set with flags to
          *    indicate either "P", "Pw" or "Pz" is required.  Multiple flags
          *    set indicates that any one of the chosen items is acceptable.
          * If multiple required parameters appear in a RIB statement, prman
          *    3.6c appears to ignore the entire RIB statement and BMRT's 
          *    rendrib 2.3.5 appears to choose the first one.
          * The Affine RIB Reader will accept the RIB statement with all
          *    its parameters and print only an error if no required 
          *    parameters are given.
          */
         rc = RibReadStringForToken( (char*)pTokens[n], 
                                     RequiredTable, NULL );
         
         /* If rc is zero or greater then it represents an index into the
          *    table RequiredTable (see table above -- at beginning of code).
          */
         if ( rc >= 0 )
         {
            switch( rc ) 
            {
             case REQUIRE_P:
               /* All the requirements have "P" as an option, so just
                *   check if something is required at all rather than 
                *   go through each type of requirement.
                */
               if ( require != kRIB_REQUIRE_NOTHING )
               {
                  /* Mark the requirements as satisfied by clearing it. */
                  require = kRIB_REQUIRE_NOTHING;
               }
               break;
             case REQUIRE_PW:
               if ( require == kRIB_REQUIRE_P_OR_PW )
               {
                  /* Mark the requirements as satisfied by clearing it. */
                  require = kRIB_REQUIRE_NOTHING;
               }
               break;
             case REQUIRE_PZ:
               if ( require == kRIB_REQUIRE_P_OR_PZ )
               {
                  /* Mark the requirements as satisfied by clearing it. */
                  require = kRIB_REQUIRE_NOTHING;
               }
               break;
             default:
               rc = kRIB_ERRRC_INT;
               goto ErrorCleanUp;
            }
            /* Store the number of vertices.  This is needed for arrays of
             *    unknown length.
             */
            rib->nvertices = ntype;
         }
         
         /* Store class, type and length of array into vector cache so
          * RibGetVectorCacheInfo() can be used to retrieve this data.
          */
         ((PRIB_UINT32)rib->typecache)[n] = p->code;
         ((PRIB_UINT32)rib->ncache)[n] = ntype;

         /* If the type was originally of unknown length, then code listed 
          *    above has already read in the data to determine the length.
          *    Skip the rest of the loop's code and increment the index 
          *    variable "n".
          */
         if ( type == kRIB_PARAML_UNKNOWN_SIZE )
         {
            /* 03-28-98 -  Moved assignment of rib->typecache[n] and
             *             rib->ncache[n] from here to just before the
             *             "if (type==kRIB_PARAML_UNKNOWN_SIZE)".  
             */
            n++;
            continue;
         }
         
         /* A Declare statement's class and type string can also include
          *    a square bracketed integer following the type name.  There
          *    can be no whitespace between the type name and the opening
          *    square bracket.  
          *
          * % echo 'Declare "Bob" "vertex float[2]" Patch "bilinear"   \
          *  "P" [-1 1 0 1 1 0 -1 -1 0 1 -1 0] "Bob" [1 2 3 4 5 6 7 8]'|catrib
          * ##RenderMan RIB
          * version 3.03
          * Declare "Bob" "vertex float[2]"
          * Patch "bilinear" "P" [-1 1 0 1 1 0 -1 -1 0 1 -1 0]  \
          *       "Bob" [1 2 3 4 5 6 7 8]
          *
          * % echo 'Declare "Bob" "vertex float [2]" Patch "bilinear" \
          *  "P" [-1 1 0 1 1 0 -1 -1 0 1 -1 0] "Bob" [1 2 3 4 5 6 7 8]'|catrib
          * A27001 RiDeclare: syntax error for: Bob 
          * A08008 RIBParameterList: Unknown parameter identifier: Bob
          * ##RenderMan RIB
          * version 3.03
          * Patch "bilinear" "P" [-1 1 0 1 1 0 -1 -1 0 1 -1 0] 
          *
          * Note that in the last example "Bob" was dropped from the parameter
          *    list because of the syntax error.  If a valid token parameter
          *    had followed '"Bob" [1 2 3 4 5 6 7 8]', Bob would have been
          *    printed.  Bob and its data were not printed because the 
          *    Declaration for Bob had failed and when catrib was reading
          *    the Patch's parameter list, Bob was an undeclared token and
          *    was ignored.
          *
          * The following code takes into account any array sizing that was
          *    given in a declaration.
          */
         if (kRIB_ARRAY_DECLARED & p->code)
         {
            ntype = ntype * p->with.n;
         }
         
         /* A token in the parameter list is declared with a class and a type.
          *    The types are integer, float, string, point, color, hpoint
          *    and normal.  The types normal and hpoint were added in version
          *    3.6c of the RenderMan Artist Tools.  The type hpoint allows 
          *    each item to be of 4 floats and the type normal is the same as
          *    point except in the manner that they are transformed.
          *
          * The following code will determine the type and then read in the 
          *    right number of floats or integers.  
          */
         /* Assume that data is an array of floats -- since that's most 
          *    common.  Only a few calls have arrays of integers and even
          *    less cases for strings.
          */
         pf = (PReadArrayFunc)RibReadFloatingPointArray;
         switch ( type )
         {
          case kRIB_FLOATTYPE:
            /* Leave ntype and pf alone. */
            break;
          case kRIB_NORMALTYPE:
          case kRIB_POINTTYPE:
          case kRIB_VECTORTYPE:
            ntype = 3 * ntype;
            break;
          case kRIB_HPOINTTYPE:
            ntype = 4 * ntype;
            break;
          case kRIB_COLORTYPE:
            ntype = rib->ncolorsamples * ntype;
            break;
          case kRIB_MATRIXTYPE:
            ntype = ntype * 16;
            break;
          case kRIB_STRINGTYPE: /* Refer to 6-19-97 comments. */
            pf = (PReadArrayFunc)RibReadStringArray;
            break;
          case kRIB_FLOATPAIRTYPE:
            ntype = 2 * ntype;
            break;
          case kRIB_INTPAIRTYPE:
            ntype = ntype * 2;
            /* Fall through */
          case kRIB_INTTYPE:
            pf = (PReadArrayFunc)RibReadIntegerArray;
            break;
          case kRIB_STANDALONETYPE:
            /* 3-15-98:
             *    Use to set ntype to zero here, but now goto a label just
             *    after code that handles reading an array that typically
             *    exists at this point in the RIB file.
             */
            goto NextToken;
          default:
            rc = kRIB_ERRRC_INT;
            goto ErrorCleanUp;
         }
         
         /* The Pixar utility catrib 3.6c will read a declaration of 
          *   "<classname> <type>[0]".  
          * 
          * Example RIB file:
          *
          * Declare "Bob" "varying float[0]"
          * Patch "bilinear" "P" [-1 1 0 1 1 0 -1 -1 0 1 -1 0] "Bob" [1 2 3 4]
          *
          * Output from catrib 3.6c:
          * ##RenderMan RIB
          * version 3.03
          * Declare "Bob" "varying float[0]"
          * Patch "bilinear" "P" [-1 1 0 1 1 0 -1 -1 0 1 -1 0]"Bob" []
          *
          * If the above output is read back into catrib 3.6c, a syntax error 
          * results:
          *    P79014 RIB syntax error (at line 4 in stdin, last token .
          *
          * Other combos such as no data following "Bob" cause syntax errors
          * too.  The behavior of the RIB Reader library is to give an
          * error for a token declared with a zero length array.
          *
          * Unique to the Affine Libraries is an internal variable type of 
          * kRIB_STANDALONETYPE that is not accessible with the standard
          * Ri calls.  
          *
          * A variable declared with kRIB_STANDALONETYPE allows the variable
          * to be in a parameter list with no paired parameter data.  In
          * vector form the token[i] pointing to the stand alone token name
          * would have a NULL pointer in parms[i].
          *
          * 3-15-98:
          * The kRIB_STANDALONETYPE is available because the Affine RIB 
          * Reader was designed to read RIB files and also RIB-like files.
          * PRMan has introduced additional Ri calls and to allow the Affine
          * libraries to easily expand with additions introduced by PRMan 
          * the stand-alone type seemed (a couple years ago anyway) to be 
          * something to provide developers.  Note that it really doesn't add
          * much code (basically a simple jump), so why not include it.
          */
         rc = ((PReadArrayFunc)*pf)( (RIB_HANDLE)rib, 
                                     kRIB_BRACKETS_PARAML, 
                                     ntype, (RtFloat**)&pParams[n] );
         if (rc)
         {
            /* An error from a RibRead...Array() function means that
             *    no array was allocated or it was freed.  So no need
             *    to worry about freeing pParams[n] just pTokens[n]. 
             */
            _RibFree( pTokens[n] );
            
            /* Remove the allocated space off from the vector cache. */
            RibCacheVector( rib, -1, NULL, NULL );
            
            /* Skip over the rest of the for(;;) loop code and
             *    skip the code that increments n. 
             */
            continue; 
         }
      NextToken:
         /* A new token has been successfully read-in, so
          *    count it and move onto the next possible
          *    token and parameter data.
          */
         n++;
      }
      else /* (!p) */
      {
         /* Undeclared token error. */
         rib->error.type = kRIB_ERRTYPE_UNDECLARED_PARAM;
         rib->error.unexpected.user_string = s;
         RibSetError( rib, RIE_BADTOKEN, RIE_ERROR, &(rib->error) );
         _RibFree( s );
         
         /* Don't just return error, first try to read through to the
          *   next token.
          */
         rc = RibReadThroughUndeclaredData( rib );

	 /* Note that at this point pTokens[n] and pParams[n] do
	  *   not point to anything and the arrays pTokens and 
	  *   pParams may not even have n elements in them at this
	  *   point.  Doing a goto ErrorCleanUp might cause a problem
	  *   when it tries to free the data pointed to by pTokens[n] 
	  *   and pParams[n].
	  * So skip ErrorCleanUp and goto ErrorCleanUpInlineList.
	  */
	  goto ErrorCleanUpInlineList;
      }
   } /* End of for (;;)... */
   
   /* The parameter list has been read in.  Now see that all the requirements
    *    have been met.  
    */
   if (require)
   {
      switch ( require )
      {
       case kRIB_REQUIRE_P:
         rib->error.type = kRIB_ERRTYPE_MISSING_P;         
         break;
       case kRIB_REQUIRE_P_OR_PZ:
         rib->error.type = kRIB_ERRTYPE_MISSING_P_AND_PZ;
         break;
       case kRIB_REQUIRE_P_OR_PW:
         rib->error.type = kRIB_ERRTYPE_MISSING_P_AND_PW;
         break;
       default:
         /* Invalid option given to RibReadParameterList(). */
         rc = kRIB_ERRRC_INT;
         goto ErrorCleanUp;
      }
      /* 3-15-98: 
       *   Tried the following change, but changed it back.  I'll leave
       *   the following comment here to point this out to others so if they
       *   prefer the warning status given to the message then they could 
       *   change it.
       *   ------------------------------------------------------------------
       *   Tried changing the RibSetError() call below from an error 
       *   (RIE_ERROR) to a warning (RIE_WARNING) because the following
       *   NuPatch statement gives a warning not and error:
       *
       *      NuPatch 0 1 [0] 0 0  0 1 [0] 0 0 
       *  
       *   The following is the warning vectrman gives: 
       *      V70001 RiNuPatchV called without a P or Pw parameter. (WARNING)
       *
       *   Note that a NuPatch with no positional data when given to 
       *   vectrman 3.7f causes the following warning:
       *     % vectrman n1.rib 
       *     V70001 RiNuPatchV called without a P or Pw parameter. (WARNING)
       *
       *   Note that a NuPatch with no positional data when given to 
       *   PRMan 3.7f causes the following warning:
       *     % prman n.rib
       *     R56001 Primitive "<unnamed>" won't split at camera plane (WARNING)
       *   
       *   Given to BMRT 2.3.6beta you get the following output:
       *     % rendrib n.rib
       *     ERROR: n.rib (17): Required parameters not provided
       *         --> P/Pw/Pz parameter needed.
       *     Rendering "a.tif" ...     
       *     Done computing image.   
       *
       *   The above commmand-line examples refer to the following RIB:
       *     % cat n.rib 
       *     Display "a.tif" "framebuffer" "rgba" 
       *     Format 256 256 1
       *     Projection "perspective" 
       *     Translate 0 0 1
       *     WorldBegin 
       *     LightSource "ambientlight" 2 "intensity" [0.3]
       *     Surface "paintedplastic" "texturename" "grid2.tex"
       *     AttributeBegin
       *     NuPatch   4 4 [-3 -2 -1 0 1 2 3 4] 0 1
       *          4 4 [-3 -2 -1 0 1 2 3 4] 0 1
       *     #"Pw" 
       *     #[
       *     #-1    1 0  1    -0.5    1 0 1    0.5    1 0 1     1    1  0  1
       *     #-1  0.5 0  1    -0.5  0.5 0 1    0.5  0.5 0 1     1  0.5  0  1
       *     #-1 -0.5 0  1    -0.5 -0.5 0 1    0.5 -0.5 0 1     1 -0.5  0  1
       *     #-1   -1 0  1    -0.5   -1 0 1    0.5   -1 0 1     1   -1  0  1
       *     #] 
       *     AttributeEnd
       *     WorldEnd 
       *   
       *   Notice that the "Pw" array was commented out.      
       */
      RibSetError( rib, RIE_MISSINGDATA, RIE_ERROR, &(rib->error) );

      /* This is more of a semantic error rather syntactic error, so don't 
       *   cause this statement to entirely fail by returning 
       *   kRIB_ERRRC_INT.  Set pointers to the arrays and return no error.
       *   What ever library is hooked into the RI Table will have to deal 
       *   with the fact that the needed data is missing.
       */

      /* Set nvertices to zero since the requirement wasn't met. */
      rib->nvertices = 0;
   } /* End of if (require). */

   /* Assign pointers to data that was requested. */
   if (pn)
   {
      *pn = n;
   }
   if (paTokens)
   {
      *paTokens = pTokens;
   }
   if (paParams)
   {
      *paParams = pParams;
   }

   /* Assign the number of uniform, varying and vertex values, so
    *    that code in an Ri table function can query these values
    *    without recalculating them.  Reading functions such as 
    *    RibReadPointsV() and RibReadPolygonV() have to set these
    *    values themselves, since figuring out the number of varying
    *    vertex values is figured out while reading the parameter list.
    */
   rib->lastnuniform = uniform;
   rib->lastnvarying = varying;
   rib->lastnvertex = vertex;

   /* At this point the status flag kRIB_STATUS_PARAML has been 
    *    removed from the status by code in the for(;;) loop located
    *    almost at the beginning of this function.  Inshort
    *    it doesn't need to be cleared here.
    */

   /* See above comments on "comment migration".
    *
    * When there is no error kRIB_STATUS_PARAML is cleared before 
    *    exiting the for(;;) loop in the code above. 
    */
   return kRIB_OK;

 ErrorCleanUp:

   /* Free data pointed to by nth pointers in vector cache. */
   if (pTokens && pTokens[n])
     _RibFree( pTokens[n] );
   if (pParams && pParams[n])
     _RibFree( pParams[n] );

   /* Decrease the size of the vector cache by 1. */
   RibCacheVector( rib, -1, NULL, NULL );

 ErrorCleanUpInlineList:
   /* Clear out the inline declared token names. */
   RibFreeInLineList( rib );

   /* Return pointers to data that was requested and
    *    assembled together before the error occurred.
    */
   if (pn)
   {
      *pn = n;
   }
   if (paTokens)
   {
      *paTokens = pTokens;
   }
   if (paParams)
   {
      *paParams = pParams;
   }

 Error:
   /* See above comments on "comment migration.
    *
    * When there is no error kRIB_STATUS_PARAML is cleared before 
    *    exiting the for(;;) loop in the code above. 
    */
   RibSetStatus( rib, kRIB_STATUS_PARAML, 0 /* Clear flag. */ );

   return rc;
}


int RibErrorNumberOfValues( RIB_HANDLE hrib, RtToken parameterID, 
                           RtInt expected_nitems, RtInt received_nitems )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   PRIB_HASHATOM  a;
   int  c;


   a = RibFindMatch( (RIB_HASHHND)rib->phashtable, kRIB_HASH_VARIABLE, 
                    kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE, (char*)parameterID );

   if (!a)
   {
      return kRIB_ERRRC_INT; /* Error */   
   }

   switch ( kRIB_TYPE_MASK & a->code )
   {
    case kRIB_INTTYPE:
    case kRIB_FLOATTYPE:
      /* Leave ntype alone. */
      c = 1;
      break;
    case kRIB_NORMALTYPE:
    case kRIB_POINTTYPE:
    case kRIB_VECTORTYPE:
      c = 3;
      break;
    case kRIB_HPOINTTYPE:
      c = 4;
      break;
    case kRIB_COLORTYPE:
      c = rib->ncolorsamples;
      break;
    case kRIB_FLOATPAIRTYPE:
    case kRIB_INTPAIRTYPE:
      c = 2;
      break;
    case kRIB_MATRIXTYPE:
      c = 16;
      break;
    default:
      return kRIB_ERRRC_INT;
   }
   expected_nitems *= c;
   received_nitems *= c;

   rib->error.type = kRIB_ERRTYPE_NUMBER_OF_VALUES;
   rib->error.expected.sys_int = expected_nitems;
   rib->error.unexpected.user_int = received_nitems;
   rib->error.var = a;
   RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );

   return kRIB_OK;
}


int RibGetParameterTypeInfo( RIB_HANDLE hrib, 
			    int *uniform, int *varying, int *vertex )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (!rib)
      return kRIB_ERRRC_INT;

   if (uniform)
      *uniform = rib->lastnuniform;

   if (varying)
      *varying = rib->lastnvarying;

   if (vertex)
      *vertex = rib->lastnvertex;

   return kRIB_OK;  
}
