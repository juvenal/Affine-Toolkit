/* $RCSfile: readfunc.c,v $  $Revision: 1.6 $ $Date: 1999/10/18 01:47:06 $
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
 * FILE:  readfunc.c
 *
 * DESCRIPTION:  Library that reads both ASCII and binary RIB files.
 *   
 *    Contains:
 * 
 *     History:
 *      12-05-96  Added checking for w < d in encoded floats 
 *                0200 + (d*4) + w | <value>
 *       6-19-97  Arrays of strings were not supported as of 3.6.  I don't
 *                have 3.7 yet, but a posting by Larry Gritz indicates that 
 *                PRMan 3.7 supports atleast arrays of strings when there 
 *                are multiple uniform values across a piece of geometry.  
 *                Added support for string arrays.
 *       9-18-97  At SIGGRAPH '97 one of the TD's from Sony's Image group who
 *                worked on Contact mentioned that the opening almost silent 
 *                scene showing stars and then galaxies involved some very 
 *                large numbers that the Pixar tools would occasionally would 
 *                write out without using exponential notation.  Problems 
 *                would result when reading these printed out ASCII values.  
 *                The Pixar tools appear to read such large numbers into a 
 *                32-bit int and a problem results when a number n is larger 
 *                than the maximum integer that can be stored in a 32-bit 
 *                number: INT32_MAX < n < MAXFLOAT.  When looking at the IRIX
 *                headers 2147483647 < n < ((float)3.40282346638528860e+38).
 *                Also considered UINT32_MAX (4294967295u).
 * 
 *                To test if the above problem occurs and if it occurs with
 *                the Affine Toolkit use affine/readrib/tests/contact.asc:
 *                     % cat contact.asc
 *                     Clipping 0 4294967295
 *                     Clipping 0 2147483647
 *                     Clipping 0 3000000000
 *                     Clipping 0 3000000000.123
 *                     % catrib contact.asc [note: catrib version 3.7]
 *                     ##RenderMan RIB
 *                     version 3.03
 *                     Clipping 0 -1
 *                     Clipping 0 2.14748e+09
 *                     Clipping 0 -1.29497e+09
 *                     Clipping 0 3e+09
 *                     % typerib contact.asc [note: typerib version 3c11-0001]
 *                     ##RenderMan RIB-Structure 1.0
 *                     Clipping 0 4.29497e+09
 *                     Clipping 0 2.14748e+09
 *                     Clipping 0 3e+09
 *                     Clipping 0 3e+09
 *                Unless someone finds a problem with the contact.asc test,
 *                the Affine tools luckily do not seem to have the problem.
 *      10-09-97  Fixed hidden octal number typo with RibIsaStringNext().
 *      01-05-98  Added an error message for ASCII integers followed by
 *                a decimal point.  Before the next RibRead* function would
 *                would print a rather cryptic message.
 *      01-06-98  Fixed kRIB_CPARAMS_BASIS code in switch() statement of
 *                RibReadRIBCallParameters().  Basis statements were causing
 *                a syntax error;
 *      03-04-98  Added note about backslashes and fixed path-file name
 *                for contact.asc.
 *      03-15-98  Allow zero length arrays.  See comment below in 
 *                ReadFloatingPointArray() by searching for "3-15-98".
 *      03-22-98  Fixed return codes and error checking for Read...Array()
 *                functions.
 *      03-24-98  Changed how escape character sequences such as '\n' and
 *                '\r' are read by RibReadString().  Also ifdef'ed some
 *                some code for NT.
 *      04-06-98  Added code to avoid ungetting a character that caused
 *                a syntax error before the error message was printed.
 *                Before the added code, the byteoffset given was off by
 *                one when reading integer and floating point arrays that
 *                were smaller than expected.
 *                Added kRIB_STATUS_ARRAY so RibReadInteger() and 
 *                RibReadFloatingPoint() could avoid printing a syntax error
 *                before their corresponding RibRead...Array() functions
 *                could.
 *      06-09-99  Fixed error return code from RibReadRIRequestToken() to
 *                be kRIB_ERRRC_INDEX when rib is NULL.  It was 
 *                kRIB_ERRRC_INT which could be taken to be an index value 
 *                of 1.  Fixed bug in RibReadArrayAndLength() returning
 *                wrong pointer when given kRIB_ARRAY_STRING.  
 *      10-12-99  Changed RIB_LITTLE_ENDIAN flag to use LITTLE_ENDIAN.
 *                Checked back in lost fix for EOF terminated comments
 *                losing their last character.  Some how fix missing from
 *                database.  Refer to RibReadArchiveRecord().
 *      10-17-99  Allow EOF terminated integers and floats.
 *
 *    
 *      WINDOWS NT AND ESCAPE CHARACTER SEQUENCES IN STRINGS (03-04-98):  
 *
 *           WindozeNT uses backslash for subdirectory separators 
 *           instead of slashes like Unix uses.  The RenderMan
 *           Standard has problems with NT path-file names for texture 
 *           files being stored in strings.  NT does allow UNIX-like
 *           path-file names with forward slashes and also would allow
 *           double backslashes to represent a single '\' (this is how
 *           the RI Spec would have it done), but you know users of NT 
 *           are going to expect the normal NT path-file names to work
 *           and will not use "C:\\bob\\george.rib" or 
 *           "C:/bob/george.rib".  So as a quick fix to this problem
 *           I'll take the approach that others seem to have taken and
 *           that is to drop support for the escape character sequences
 *           in strings when compiling under NT.  
 *
 *           Look for the ifdef flag WINDOWS_NT in this file.
 *
 *
 *    References:
 *          [GRITZ97] Gritz, Larry, "Re: texture names per mesh face?",
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
#define __RIB_FILE_READFUNC
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <ripriv.h>
#include <ribrdr.h>

#define RIB_MAX_SIGC     8
#define RIB_MAX_EXPSIGC  4

#define RIB_MIN(a,b)  ((a)<(b) ? (a) : (b))
#define RIB_MAX(a,b)  ((a)>(b) ? (a) : (b))


int gRibErrorHandlers[] = { 
   kRIB_ERRORIGNORE, /* RiErrorIgnore */
   kRIB_ERRORABORT,  /* RiErrorPrint */
   kRIB_ERRORPRINT   /* RiErrorAbort */
};
   

int gRibFilters[] = { 
   kRIB_BOXFILTER,        /* RiBoxFilter */
   kRIB_CATMULLROMFILTER, /* RiCatmullRomFilter */
   kRIB_GAUSSIANFILTER,   /* RiGaussianFilter */
   kRIB_SINCFILTER,       /* RiSincFilter */
   kRIB_TRIANGLEFILTER    /* RiTriangleFilter */
};


int RibHandleEncodedEscChar( RIB_HANDLE rib, char c );
#ifdef REPLACE_ESCAPE_CHARACTERS
int RibHandleASCIIEscChar( RIB_HANDLE rib );
#endif

/* Reads through an archive record from the characters following the '#'
 *   to the first encountered '\n'.
 *
 * This function can be used to skip over archive records or store them
 *   into the buffer depending on the flag used in the last call to 
 *   RibSaveToBuffer().
 */
int RibReadArchiveRecord( RIB_HANDLE rib )
{
   register int  c;


   do
   {
      c = RibGetChar( rib );
   } while ( c!=EOF && c!='\n' );

   /* Remove '\n' character from buffer.
    *
    * If RibIgnoreLastChar() returns an error, it's because the input 
    *    isn't being saved to a buffer and is just being discarded.  For 
    *    this case just ignore the error return code.
    */
   /* Fix bug where EOF terminated comment or hint lost its last character:
    * The 'd' in 'abcd' was missing when the following was done:
    *     % echo 043 141 142 143 144 | oct2bin | typerib
    *     ##RenderMan RIB-Structure 1.0
    *     #abcd
    *
    */
   if (c!=EOF)
      RibIgnoreLastChar( rib );

   /* Don't return an error for running into an EOF. */
   return kRIB_OK;
}


int RibHandleArchiveRecord( RIB_HANDLE hrib )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   register int  rc;

   if ( rib->arcrechandler )
   {
      RibSaveToBuffer( rib, RI_TRUE );
      /* Initially rib->arcrechandler is assigned the function 
       *    RibDefaultArchiveRecordHandler() in hintfunc.c as the handler
       *    for comments and hints.
       */
      rc = (*(rib->arcrechandler))((RIB_HANDLE)rib);
      RibSaveToBuffer( rib, RI_FALSE );
   }
   else
   {
      RibSaveToBuffer( rib, RI_FALSE );
      rc = RibReadArchiveRecord( rib );
   }

   return rc;
}


int RibHandlePrefix( RIB_HANDLE rib )                
{
   auto int  archive_record = 0;
   auto int  c;


   c = RibGetChar( rib );
   if (EOF==c)
     return c;

   if ( c == '#' )
   {
      archive_record = 1;
   }

   while ( RibIsanEncodedDefinition(c) 
          || RibIsaWhiteSpace(c) 
          || archive_record )
   {                                                         
      if ( archive_record )
      {
         if ( RibHandleArchiveRecord(rib) )
           return kRIB_ERRRC_STREAMIO; /* Error */
         archive_record = 0;
      }
      else if ( RibIsanEncodedDefinition(c) )                             
      {            
         c = RibReadDefiningBinCode( rib, c );               
         if (c)                                              
           return kRIB_ERRRC_STREAMIO; /* Error */
      }                        

      c = RibGetChar( rib ); 
      if (EOF==c)            
        return c;

      if ( c == '#' )
      {
         archive_record = 1;
      }
   }

   return c;
}


int RibHandleEncodedEscChar( RIB_HANDLE rib, char alreadyreadin )
{
   static struct esctable {
      char ascii;
      char bin;
   } t[] = { 
      { 'n', '\n' }, /* 0 */
      { 'r', '\r' }, /* 1 */
      { 't', '\t' }, /* 2 */
      { 'b', '\b' }, /* 3 */
      { 'f', '\f' }, /* 4 */
      { '\\','\\' }, /* 5 */
      { '\"','\"' }  /* 6 */
   };
   register unsigned int  i,u;
   register int  c = alreadyreadin;


   /* Internal call don't do checking. */
   /*
    *  if (!rib)
    *     return kRIB_ERRRC_INT;
    */ 

   for (i=0; i < sizeof(t)/sizeof(struct esctable); i++)
   {
      if (c == t[i].bin)
      {
         RibIgnoreLastChar( rib );
         RibPutChar( rib, '\\' );
         RibPutChar( rib, t[i].ascii );
         
         return kRIB_OK;
        }
   }
    
   /* Assume that the character c was given to this function
    *    because it was not a standard printable ASCII character
    *    and since it is not in the table checked in the above
    *    if statment, it must need to be printed in octal 
    *    format \ddd.
    */
   RibPutChar( rib, '\\' );
   u = c & 0x00ff;
   for ( i=0; i<3; i++ )
   {
      c = 0x01C0 & u; /* 0x01c0 == 0000 0001 1100 0000 binary */
      c = c >> 6;
      u = u << 3;
      RibPutChar( rib, '0' + c );
   }
   
   return kRIB_OK;
}


#ifdef REPLACE_ESCAPE_CHARACTERS
int RibHandleASCIIEscChar( RIB_HANDLE rib )
{
   static struct esctable {
      char ascii;
      char bin;
   } t[] = { 
      { 'n', '\n' }, 
      { 'r', '\r' },
      { 't', '\t' },
      { 'b', '\b' },
      { 'f', '\f' }
   };
   register int  c,i;
   register unsigned int u;
   

   /* Internal call don't do checking. */
   /*
    *  if (!rib)
    *     return kRIB_ERRRC_INT; 
    */ 

   /* Get rid of the slash that caused this function to be called. */
   RibIgnoreLastChar( rib );

   c = RibGetChar( rib );
   if (EOF==c)
     goto Error;   

   for (i=0; i <sizeof(t)/sizeof(struct esctable); i++)
   {
      if (c == t[i].ascii)
      {
         RibIgnoreLastChar( rib );
         RibPutChar( rib, t[i].bin );
         return kRIB_OK;
      }
   }

   switch (c)
   {
    case '\\': /* backslash */
    case '\"': /* double quote */
      /* The character we want is already there, just return. */
      break;
    case '\n': /* continuation to next line was marked. */
      /* Simply ignore '\n'. */
      RibIgnoreLastChar( rib );
      break;
    default:
      /* Handle octal character code. */
      RibIgnoreLastChar( rib );
      if ( c >= '0' && c <= '8' )
      {
         u = c - '0';   
         for (i=0; i < 3 && ( c >= '0' && c < '8' ); i++ )
         {
            c -= '0';
            RibIgnoreLastChar( rib );   
            u = u << 3 | c;         

            c = RibGetChar( rib );
            if (EOF==c)
              goto Error;
         }
         if ( !( c >= '0' && c < '8' ) )
         {
            RibUngetChar( rib, c );
         }
         RibPutChar( rib, u );
      }
      else 
      {
         /* No escape character sequence found, so ignore the slash
          *   mark that caused this function to be called, but print
          *   the following character.
          */
         RibPutChar( rib, c );
      }
      break;
   }
   return kRIB_OK;

 Error:
   
   return (c==EOF ? EOF : kRIB_ERRRC_INT); /* Error */
} 
#endif


/* The following are options for reading arrays and strings:
 *
 * (options & kRIB_BRACKETS_MASK)==kRIB_BRACKETS_NONE
 *    The value being read is not surrounded by square brackets.  
 *    RIB statements do not allow brackets around strings except for
 *    data in a parameterlist.
 *
 *    For example, the statement:
 *
 *             Surface "paintedplastic" "texturename" "grid2.tex"
 *
 *    can not have brackets around "paintedplastic" without causing a
 *    syntax error (such as catrib's P79014 RIB syntax error).
 *
 *    Tokens can not have brackets.  For example, in the statement:
 *
 *             Surface "paintedplastic" "texturename" "grid2.tex"
 *
 *    the string "texturename" is the token part of a token and 
 *    parameter pair and can not have brackets around it.
 * 
 *    The following causes a syntax error (such as catrib's P79014 
 *    RIB syntax error):
 *
 *             Surface "paintedplastic" ["texturename"] "grid2.tex"
 *
 * (options & kRIB_BRACKETS_MASK)==kRIB_BRACKETS_OPTIONAL
 *    The following calls have optional brackets around the values given
 *    them: Bound, Color, Cone, CropWindow, Cylinder, Detail, DetailRange, 
 *    Disk, Hyperboloid, Opacity, Paraboloid, ScreenWindow, Sphere, 
 *    TextureCoordinates, and Torus.  These calls can have the values given
 *    them in the form of an encoded array of IEEE floats.
 *
 *    The following are valid RIB statements (Note that the '[' character
 *    can also be adjacent to the RIB request such as "Color[1 1 1]".):
 *
 *           Color 1 1 1
 *           Color[1 1 1]
 *           CropWindow 0.0 0.0 1 1.0 
 *           CropWindow [0 0 1 1 ]
 *
 *    Skew is mentioned in the spec [PIXA89] as allowing brackets, but catrib
 *    doesn't appear to like it.  I'm tempted to write it off as a 
 *    cut-paste typo in the spec.  The suspected typo is also in HTML 3.6 
 *    version of the RenderMan Interface Specification.
 *
 * (options & kRIB_BRACKETS_MASK)==kRIB_BRACKETS_PARAML
 *    The parameter "grid2.tex" in the above example can have optional
 *    brackets.  The following statements are both valid (note the last
 *    string "grid2.tex"):
 *
 *             Surface "paintedplastic" "texturename" "grid2.tex"
 *             Surface "paintedplastic" "texturename" ["grid2.tex"]
 *
 *    The brackets are also optional for parameters that are either a single 
 *    integer or a single floating point value.  The following statements
 *    are both valid (notice the format given the last "1" on each line):
 *
 *            LightSource "distantlight" 1 "intensity" 1
 *            LightSource "distantlight" 1 "intensity" [1]
 *
 *    The parameter data in a parameter lists can have optional brackets
 *    only when the data is a single string, float or integer.  Use
 *    kRIB_BRACKETS_PARAML to get this behavior when reading an array or
 *    string.
 *
 * (options & kRIB_BRACKETS_MASK)==kRIB_BRACKETS_REQUIRED
 *    In parameter lists arrays of more than one floating point value or 
 *    integer value require brackets.  The following statement is valid 
 *    syntactically:
 *
 *        Transform [1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16]
 *    
 *    Without the brackets in the above array there would be a syntax
 *    error.  An encoded array of IEEE floats acts like a bracketed array
 *    of floats.
 *
 * (options & kRIB_BRACKETS_MASK)==kRIB_BRACKETS_CLOSE
 *    Used if an open bracket was already read and a closing square
 *    bracket should be found.
 *
 * options & kRIB_STRING_ENCODED
 *    Accept only encoded strings.  This is used for reading a definition 
 *    for a RI request where strings in the normal ASCII format delimited
 *    by beginning and ending quotes are not allowed.
 *    
 *    The spec [PIXA89] says strings in ASCII form are used for defining 
 *    a RI request, but this doesn't seem to work.  Only encoded strings 
 *    or a reference to a defined string seems to work with catrib 3.6c.
 *
 * options & kRIB_STRING_RTARRAY
 *    Makes RibReadString() accumulate strings in rib->pbuf.  This allows
 *    one block of memory to be allocated for all of the strings and 
 *    an array of pointers to them..
 *
 * options & kRIB_STRING_RTSTRING
 *    RibReadString() will return a char** (pointer to a pointer to a char
 *    array) instead of a normal char*.  This is for strings placed on the
 *    parameter list.
 *
 *    When passing a string in a parameter list, a simple char* is not
 *    used.  Instead a pointer to a char* is used.  Inshort, if "texture"
 *    is declared and initialized with the following C code:
 *              char *texture = "texturename";
 *    a call using "texture" in a parameter list would require &texture
 *    to be used not just texture such as the following example:
 *              RiSurface( "Bob", "texturename", &texture, RI_NULL );
 *    The above call is correct and the following call would core dump 
 *    with the RIB writer libraries available:
 *              RiSurface( "Bob", "texturename", texture, RI_NULL );
 *  
 *    This extra dereferencing appears to be setup this way because 
 *    RtString is treated as a type and a parameter list contains 
 *    RtPointer's to RtFloat's, RtInt's and also RtString's.  For strings
 *    this turns into a pointer to a pointer to a block of characters.
 *
 *    The RIB Reader library counts on a function listed in the RI Table
 *    being able to call RibFree() for any pointers to data that it is 
 *    given.  (As a side note:  If a RI Table function wants to keep some
 *    of the data it is given, but release other pieces, then it needs to
 *    call RibKeepData().  This prevents the RIB Reader from freeing all of
 *    the data after the RI Table function returns, but it also means that
 *    the RI Table function must call RibFree() for the pieces of data it
 *    wishes to discard -- because the RIB Reader won't be freeing anything.)
 *
 *    To get around the pointer to pointer issue and allow RI Table functions
 *    to call RibFree() for all pointers including parameter list strings,
 *    the RIB Reader library allocates space for the string and space for a
 *    pointer.  These two allocated areas are really allocated with one call
 *    to malloc() and the RtString pointer is placed in the first few bytes
 *    of the memory area.  
 *
 * options & kRIB_ARRAY_ENCODED
 *    Set RibReadFloatingPointArray() to only accept an encoded array
 *    of floating point values.
 *
 * options & kRIB_ARRAY_INTEGER
 *    Set RibReadArrayAndLength() to only accept integers in the array.
 *
 * options & kRIB_ARRAY_FLOAT
 *    Set RibReadArrayAndLength() to only accept floating point values in 
 *    the array.  
 *
 * options & kRIB_ARRAY_STRING
 *    Set RibReadArrayAndLength() to only accept strings in the array.  
 *
 */


int RibReadString( RIB_HANDLE hrib, RIB_UINT32 options, RtString *pp )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   PRIB_HASHATOM       a;
   RtBoolean           createarray = RI_FALSE;
   char                *s;
   auto int            c,t,w;
   auto size_t         l;
   auto unsigned int   utmp;
   

   if (!rib)
     return kRIB_ERRRC_INT; /* Error */

   /* Handle white spaces and any RI request or string definitions. */
   c = RibHandlePrefix( rib );
   if (EOF==c)
     goto Error;

   /* Set true if storing strings up in the buffer to form an array. */
   createarray = options & kRIB_STRING_RTARRAY;

   if ( RibIsaBinaryChar(c) )
   {
      rib->status |= kRIB_STATUS_ENCODEDDATA;

      /* Handle reference to a defined string. */
      if ( 0317==c || 0320==c )
        /* 0317 + w | <token>, w=[0..1] */
      {
         t = 0;
         if ( 0320==c )
         {
            t = RibGetChar( rib );
            if (t==EOF)
            {
               c = t; /* Error label expects c to be set to last char read. */
               goto Error;
            }
            t = t<<8;
         }
         c = RibGetChar( rib );
         if (EOF==c)
           goto Error;

         /* Use t to assemble the token which is an integer value. */
         t = t | (0xff & c);

         a = RibFindMatch( rib->phashtable, kRIB_HASH_STRINGTOKEN, t, NULL );
         if (!a)
         {
            rib->error.type = kRIB_ERRTYPE_UNDEFINED_STRING;
            rib->error.unexpected.user_int = t;
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );

            return kRIB_ERRRC_INT; /* error */
         }

         /* If no way to return a string because pp is NULL, then there
          *    no need allocating the memory for it, so simply return.
          * 
          * Do this check after looking the string up in the hash table,
          *    so an undefined string error can occur if needed.
          */
         if ( !pp && !createarray )
         {
            return kRIB_OK;
         }

         s = RibGetDefinedString( a );
         l = strlen( s ) + 1; /* Include the terminating null. */

         /* Make a copy of string even though it already exists in memory.
          *    A copy is made because otherwise the code that frees a string
          *    would have to check if a string pointer is defined in the
          *    hash table or just a normal string.
          */ 
         if ( createarray )
         {
            /* Copy string to buffer. */
            while (l)
            {
               RibPutChar(rib,*s++);
               l--;
            }
         }
         else if ( options & kRIB_STRING_RTSTRING )
         {
            RIB_RTSTRING  *ss;
            
            ss = (RIB_RTSTRING*)_RibMalloc( sizeof(RtString) + l );
            if (!ss)
              return kRIB_ERRRC_INT;
            
            /* Set the first few bytes to be a RtString pointer so that
             * it points to the soon to be adjacent string.
             */
            ss->pstr = &ss->str[0];
            
            /* Copy string to be after the RtString pointer. */
            memcpy( ss->str, s, l );
            *pp = (char*)ss;
         }
         else
         {
            *pp = (char*)_RibMalloc( l );
            if (!*pp)
              return kRIB_ERRRC_INT;
            /* Copy string. */
            memcpy( *pp, s, l );

            RibSaveToBuffer( rib, RI_FALSE );
            
            return kRIB_OK;
         }
      }
      /* Handle short strings of 15 characters or less. */
      else if (( c >= 0220 ) && ( c <= 0237 ))
        /* 0220 + w | <ASCII string>, w=[0..15] */
      {
         w = c - 0220;
         
         if ( createarray )
         {
            RibSetStatus( rib, kRIB_STATUS_SAVETOBUFFER, 
                         kRIB_STATUS_SAVETOBUFFER );
         }
         else if (pp)
         {
            RibSaveToBuffer( rib, RI_TRUE );
         }
         for ( l=0; l < w; l++ )
         {
            c = RibGetChar( rib ); 
            if (EOF == c)
              goto Error;
            
#ifndef WINDOWS_NT
            if (!RibIsaStdASCIIChar(c))
              RibHandleEncodedEscChar( rib, c );           
#endif
         }
         if ( createarray )
         {
            RibSetStatus( rib, kRIB_STATUS_SAVETOBUFFER, 0 );
         }
      }
      /* Handle long strings greater than 15 characters. */
      else if (( c >= 0240 ) && ( c <= 0243 ))
        /* 0240 + l | <length> | <ASCII string>, l=[0..3] */
      {
         /* Assign l be the number of bytes needed to specify 
          *   the length of the string.
          */
         l = c - 0240 + 1;

         /* Read all l number of bytes. */
         utmp = 0;
         for ( w=0; w < l; w++ )
         {
            c = RibGetChar( rib );
            if (EOF==c)
              goto Error;
            
            utmp = utmp << 8;
            utmp |= 0xff&c;
         }
         
         if ( createarray )
         {
            RibSetStatus( rib, kRIB_STATUS_SAVETOBUFFER, 
                         kRIB_STATUS_SAVETOBUFFER );
         }
         else if (pp)
         {
            RibSaveToBuffer( rib, RI_TRUE );
         }
         for ( w=0; w < utmp; w++ )
         {
            c = RibGetChar( rib ); 
            if (EOF == c)
              goto Error;
            
#ifndef WINDOWS_NT
            if (!RibIsaStdASCIIChar(c))
              RibHandleEncodedEscChar( rib, c );           
#endif
         }
         if ( createarray )
         {
            RibSetStatus( rib, kRIB_STATUS_SAVETOBUFFER, 0 );
         }
      }
      else
      {
         rib->error.type = kRIB_ERRTYPE_UNDEFINED_TOKEN;
         rib->error.unexpected.user_int = c;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         RibSaveToBuffer( rib, RI_FALSE );

         return kRIB_ERRRC_INT; /* error */
      }
   }
   /* Handle an ASCII string. */
   else if ( '\"'==c )
   {
      if ( pp && !createarray )
      {
         RibSaveToBuffer( rib, RI_TRUE );
      }

      /* Read until we run into an ending quote that doesn't have a 
       *   preceding '\' before it.  When there is a '\' character
       *   RibHandleASCIIEscChar() will handle the character that 
       *   follows, which prevents a string with a quote following
       *   a slash from tricking the "do {} while (no quote char)" 
       *   into ending prematurely.
       */
      /* 3-24-98:
       *  PRMan's catrib program reads strings given in ASCII form 
       *  with special escape sequences such as '\n' and expands them 
       *  to a single binary character as shown below:
       *
       *      % echo 'Surface "abc\nabc"'  | catrib
       *      ##RenderMan RIB
       *      version 3.03
       *      Surface "abc
       *      abc" 
       *
       *  The Affine Toolkit's typerib program also did this.  Refer to 
       *  the functions RibHandleASCIIEscChar() and RibReadString() in 
       *  affine/readrib/readfunc.c.  
       *
       *      % echo 'Surface "abc\nabc"' | typerib
       *      ##RenderMan RIB-Structure 1.0
       *      Surface "abc
       *      abc" 
       *
       *  This replacing of the '\' character sequences can cause a 
       *  loss of data so I changed to prevent problems as shown in the 
       *  following example where an escape character is exchanged for 
       *  a NULL and begins to mark the middle of a string as its end:
       *
       *      % echo 'Surface "abc\0abc"'  | catrib
       *      ##RenderMan RIB
       *      version 3.03
       *      Surface "abc" 
       *
       *  Note that typerib did this too.  By setting the flag
       *  REPLACE_ESCAPE_CHARACTERS when compiling, this behavior
       *  can be included in the Affine Libraries if desired.
       */
      /* Have two do-while loops.  The first stores up data as it is
       *    read and the second just throws the data away because
       *    the data is being read over and ignored. 
       *
       * TO-DO:  Combine some of the code a bit and figure-out what
       *         to do with WindowsNT's use of backslashes in 
       *         path-file names.
       */
      if ( (createarray && !RibSetStatus( rib, kRIB_STATUS_SAVETOBUFFER, 
                                         kRIB_STATUS_SAVETOBUFFER )) 
          || pp ) 
        /* The given order of terms above is required, so that RibSetStatus()
         *    is given a chance to be called.
         */
      {
#ifdef REPLACE_ESCAPE_CHARACTERS
         do
         {
            c = RibGetChar( rib ); 
            if (EOF == c)
              goto Error;

            if ('\\' == c)
            {
               c = RibHandleASCIIEscChar( rib );
               if (EOF==c)
                 goto Error;
            }
         } while ( '\"'!=c );
#else
         /* See above comments from 3-24-98. */
         do
         {
            c = RibGetChar( rib ); 
            if (EOF == c)
              goto Error;

            /* For WindowsNT just ignored the escape character portion
             *    of the RenderMan Standard when reading an ASCII string.
             *    Binary strings will have special characters expanded
             *    to ASCII form.  
             */
#ifndef WINDOWS_NT
            if ('\\' == c)
            {
               c = RibGetChar( rib ); 
               if (EOF == c)
                 goto Error;
               c = 0; /* Set c to something else besides a possible '\"'. */
            }
#endif
         } while ( '\"'!=c );
#endif
         /* Ignore the ending quote character. */
         RibIgnoreLastChar( rib );

         /* If an array is being created, stop saving input
          *   to buffer so that the RibReadStringArray() that
          *   called RibReadString() can read through the
          *   white spaces upto the next beginning string 
          *   quote without all the junk being stored in
          *   the buffer with the actual string data.
          */
         if ( createarray )
         {
            /* Set the flag directly, because RibSaveToBuffer(rib,RI_FALSE)
             *   would reset the buffer and delete the string data stored
             *   there.
             */
            RibSetStatus( rib, kRIB_STATUS_SAVETOBUFFER, 
                          0 /* Clear kRIB_STATUS_SAVETOBUFFER flag */ );
         }         
      }
      else
      {
         do
         {
            c = RibGetChar( rib ); 
            if (EOF == c)
              goto Error;
            
            /* For WindowsNT just ignored the escape character portion
             *    of the RenderMan Standard when reading an ASCII string.
             *    Binary strings will have special characters expanded
             *    to ASCII form.  
             */
#ifndef WINDOWS_NT
            if ('\\' == c)
            {
               c = RibGetChar( rib ); 
               if (EOF == c)
                 goto Error;
               c = 0; /* Set c to something else besides a possible '\"'. */
            }
#endif
         } while ( c!='\"' );
      }
   }
   else
   {
      /* If the array is cut short in size, don't just print some
       *    syntax error.  Instead print a more useful array size error.
       * Return kRIB_ERRRC_SIGNAL to signal RibReadStringArray() that a 
       *    square closing bracket was found.
       */
      if ( ']'==c && createarray )
      {
         RibSaveToBuffer( rib, RI_FALSE );
         if (pp)
         {
            *pp = NULL;
         }

         /* Special signal for array creation. */
         return kRIB_ERRRC_SIGNAL;
      }

      /* Unexpected character found. */
      goto Error;
   }

   /* If assembling strings in the RIB buffer don't malloc any
    *    memory to copy the string into and don't reset the buffer
    *    with RibSaveToBuffer().  
    * Add a NULL to terminate the string and separate it from the next
    *    string that could be read in.  Then return and skip the rest 
    *    of this function's code.
    */
   if ( createarray )
   {
      RibPutChar( rib, '\0' );
      
      /* Don't call RibSaveToBuffer().  It will remove everything
       *   that has been stored in the way of strings in the array
       *   being assembled.
       */
      return kRIB_OK;
   }

   /* Copy buffer to an allocated memory area and return address. */
   if (pp)
   {
      *pp = RibCreateStringFromBuffer( rib, options );
      if (!*pp)
      {
         RibSaveToBuffer( rib, RI_FALSE );

         return kRIB_ERRRC_INT;
      }
   }
   RibSaveToBuffer( rib, RI_FALSE );

   return kRIB_OK;

 Error:

   RibSaveToBuffer( rib, RI_FALSE );
   if (pp)
   {
      *pp = NULL;
   }
   if ( EOF==c )
   {
      rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   }
   else
   {
      rib->error.type = kRIB_ERRTYPE_SYNTAX;
      c = kRIB_ERRRC_INT; /* Error */
   }
   RibSetError( rib, RIE_SYNTAX, (EOF==c ? RIE_SEVERE : RIE_ERROR), 
                &(rib->error) );

   return c; /* Error */
}


/* RibReadStringArray()
 *
 *    The only options that are used are for the kRIB_BRACKETS_* flags. 
 *
 */
int RibReadStringArray( RIB_HANDLE hrib, unsigned int options, 
                       int numberof, RtString **pp )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   RtBoolean      brackets;
   int            rc = 0;
   auto int       c;
   auto int       i;
   unsigned int   utmp;
   RtString       *p;


   if (!rib)
     return kRIB_ERRRC_INT; /* Error */

   /* Handle white spaces and any RI requests or string definitions. */
   c = RibHandlePrefix( rib );
   if (EOF==c)
     goto EOFError;

   /* Handle initial square bracket if present. */
   utmp = kRIB_BRACKETS_MASK & options;
   if ( '['==c )
   {
      /* Check that one of the kRIB_BRACKETS_* flag bits were set
       *    except kRIB_BRACKETS_CLOSE, because kRIB_BRACKETS_CLOSE
       *    is the only nonzero kRIB_BRACKETS_* flag that makes an 
       *    opening bracket '[' illegal.  Note that kRIB_BRACKETS_NONE
       *    is a zero value that would not show up in an AND operation,
       *    so kRIB_BRACKETS_CLOSE is the only flag bit to check for.
       */
      if ( utmp && (kRIB_BRACKETS_CLOSE!=utmp) )
      {
         brackets = RI_TRUE;
      }
      else
      {
         /* kRIB_BRACKETS_CLOSE is set and a '[' character was found. */
         rib->error.type = kRIB_ERRTYPE_SYNTAX;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         return kRIB_ERRRC_INT; /* Error */
      }

      /* Handle white spaces and any RI requests or string definitions. */
      c = RibHandlePrefix( rib );
      if (c==EOF)
          goto EOFError;
   }
   else
   {
      /* No opening '[' bracket was found, so now check if this is
       *    an error by looking at the kRIB_BRACKETS_* flag bits. 
       */
      if ( kRIB_BRACKETS_REQUIRED==utmp 
           || (kRIB_BRACKETS_PARAML==utmp && numberof > 1) )
      {
         rib->error.type = kRIB_ERRTYPE_OPEN_BRACKET;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         
         return kRIB_ERRRC_INT; /* error */
      }
      else if ( kRIB_BRACKETS_CLOSE==utmp )
      {
         brackets = RI_TRUE;
      }
      else
      {
         brackets = RI_FALSE;
      }
   }

   /* Handle string data. */
   if ( numberof > 0 )
   {
      if (pp)
      {
         utmp = kRIB_STRING_RTARRAY;
      }
      else
      {
         utmp = 0;
      }
      /* Put starting character of string back (quote or binary char). */
      RibUngetChar( rib, c );

      /* RibReadString() with kRIB_STRING_RTARRAY will set status
       *    flags directly to achieve the effect of calling 
       *    RibSaveToBuffer( rib, RI_TRUE ).  
       */
      for (i=0; i<numberof; i++)
      {
         c = RibReadString( rib, utmp, NULL );
         if (c)
         {
            if (EOF==c)
            {
	       rc = c; /* Set rc to return code from RibReadString(). */
	       goto Error; /* Error reported by RibReadString(). */
            }
            else
            {
               /* The return code from RibReadString() could be
		*    kRIB_ERRRC_SIGNAL which signals that a square 
		*    closing bracket was found.
		* Get out of the present for() loop and let the code that
		*    follows handle the error code c.
		*/
               break;
            }
         }
      }
      if ( i<numberof )
      {        
         /* Check for special return code indicating a ']' was found early. */
         if ( kRIB_ERRRC_SIGNAL == c )
         {
            rib->error.type = kRIB_ERRTYPE_STRING_ARRAY_SIZE;
            rib->error.expected.sys_int = numberof;
            rib->error.unexpected.user_int = i;
            
            /* Complain about size of array. */
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         }
         rc = kRIB_ERRRC_INT; /* Set a normal return code in rc. */
         goto Error;
      }
   }
   else
   {
      /* See comment about 3-15-98 change.  Search for "3-15-98". */
      if ( numberof < 0 )
      {
         rc = kRIB_ERRRC_INT; /* Set the return code in rc. */
         goto Error; /* RibSaveToBuffer() clean-up needed. */
      }
   }   

   if (brackets)
   {
      /* Handle white spaces and any RI request or string definitions. */
      c = RibHandlePrefix( rib );
      if ( EOF==c )
        goto EOFError;

      /* Handle extra strings that appear in the array making the array
       *    exceed the expected size.
       */
      if ( ']'!=c )
      {
         if ( ('\"'==c) /* ASCII string. */
             || (c>=0220 && c<=0243)  /* Encoded string. */
             || (c==0317) ) /* Interpolate encoded string. */
         {
            RibUngetChar( rib, c );
            i = numberof;
            if (pp)
            {
               utmp = kRIB_STRING_RTARRAY;
            }
            else
            {
               utmp = 0;
            }
            do
            {
               c = RibReadString( rib, utmp, NULL );
               if (c)
	       {
		  rc = c; /* Error code from RibReadString(). */
		  goto Error; /* Error reported by RibReadString(). */
	       }
               
               /* Found an extra string.  Include it in the array length. */
               i++;
            }
            while ( RibIsaStringNext(rib) );

	    /* Try to make the byte offset accurate.  If there is a ']'
	     *    character present, it was the ']' to cause the syntax 
	     *    error.
	     */
	    c = RibGetChar( rib );
	    if (EOF==c)
	       goto EOFError;            
	    if ( ']'!=c )
	       RibUngetChar( rib, c );

            rib->error.type = kRIB_ERRTYPE_STRING_ARRAY_SIZE;
            rib->error.expected.sys_int = numberof;
            rib->error.unexpected.user_int = i;
            
            /* Complain about size of array. */
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
            
            /* There is still an error condition, so set a return code. */
            rc = kRIB_ERRRC_INT; /* Error */
            
            /* Allow for a missing square bracket error, so continue to 
             *   the following if ( ']'!=c ) statement. 
             */
         }
         if ( ']'!=c )
         {
            rib->error.type = kRIB_ERRTYPE_CLOSE_BRACKET;
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
            rc = kRIB_ERRRC_INT; /* Set the return code in c. */
         }
         /* A few lines before if( ']'!=c ){} given above, rc could have
          *    given an error code.  The following "if (rc)" allows 
          *    that code use of the if( ']'!=c ){} code and still return 
          *    an error condition.
          */
         if (rc)
	 {
            /* The value rc is already set to the error code. */
            goto Error;
	 }
      }
   }

   if (pp)
   {
      if (RibCreateStringsFromBuffer( rib, numberof, &p ))
      {
         rc = kRIB_ERRRC_INT; /* Set the return code in rc. */
         goto Error;
      }
      *pp = p;
   }
   RibSaveToBuffer( rib, RI_FALSE );
   
   return kRIB_OK;

 EOFError:
   rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   RibSetError( rib, RIE_SYNTAX, RIE_SEVERE, &(rib->error) );
        rc = EOF;

 Error:

   RibSaveToBuffer( rib, RI_FALSE );
   if (pp)
   {
      *pp = NULL;
   }

   return rc;
}


int RibReadFloatingPoint( RIB_HANDLE hrib, float *pflt )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   char    fltbuf[RIB_MAX_SIGC+RIB_MAX_EXPSIGC+4]; 
   /* The above "+4" is for possible '+/-' 'E' '+/-' '\0' characters. */  
   int     c,tmp;
   int     d,w;
   float   flt;
   register int  i=0;


   if (!rib)
     return kRIB_ERRRC_INT; /* Error */

   /* Clear out white spaces and any RI or string definitions. */
   c = RibHandlePrefix( rib );
   if (EOF==c)
     goto Error;

   if (RibIsaBinaryChar(c))
   {
      rib->status |= kRIB_STATUS_ENCODEDDATA;

      /* Handle integers and fixed-point numbers. */
      /* 0200 + (d*4) + w | <value> */

      /* NOTE:  Certain encoded tokens in the 0200 to 0217 range
       *        can not form a valid number because w < d.
       *        Since the value w is the total number of bytes and the 
       *        value d is the number of bytes to the right of the 
       *        decimal, an encoded token such as 0214 where w=1 and d=3
       *        can not be valid.
       * (DONE 12-05-96)  TO-DO:  Add checking for w < d.  
       */
      if (( c >= 0200 ) && ( c <= 0217 ))
      {
         w = (int)(0x00000003 & c) + 1;
         d = (0x0000000C & c) >> 2;

         /* Return error if w < d.  (DONE 12-05-96) */
         if ( w < d )
         {
            rib->error.type = kRIB_ERRTYPE_BAD_ENCODED_FLOAT;
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
            
            return kRIB_ERRRC_INT; /* Error */
         }
         
         /* Read all bytes to the left of the decimal. */
         tmp = 0;
         for ( i=0; i < w; i++ )
         {
            c = RibGetChar( rib ); 
            if (EOF == c)
              goto Error;
            tmp = tmp << 8;
            tmp |= (0xff)&c;
         }
         i = w*8;
         if  ( tmp & ( 1 << (i-1) ))
           tmp |= 0xffffffff << i;

         flt = (double)tmp / (double)( 1 << (d * 8) );        

         if (pflt)
           *pflt = flt;
         return kRIB_OK;
      }
      /* Handle IEEE floats. */
      /* 0244 | <four bytes> */
      else if ( 0244 == c )
      {
         /* Read float. */
         tmp = 0;
         for ( i=0; i < sizeof(float); i++ )
         {
            c = RibGetChar( rib ); 
            if (EOF == c)
              goto Error;
            tmp = tmp << 8;
            tmp |= (0xff)&c;
         }
         flt = *( (float*)(void*)&tmp );

         if (pflt)
           *pflt = flt;
         return kRIB_OK;
      }         
      
      /* Handle doubles. */
      /* 0245 | <eight bytes> */
      
      /* NOTE:  Encoded doubles don't appear to be used by anybody.  
       *        But the following code was tested once with a hacked 
       *        binary rib file.  Refer to the test file:
       *        affine/ribdump/dbltest.rib.  Use ribdump, typerib,
       *        or Pixar's catrib to view the file since it is in
       *        binary encoded form.
       */
      else if ( 0245 == c )
      {
         char *v; /* value of float accessed as bytes*/
         double dbl;

         
         v = (char*)(void*)&dbl;
         
         /* Read double. */
#ifdef LITTLE_ENDIAN         
         for ( i=(sizeof(double)-1); i > -1; i-- )
#else
         for ( i=0; i < sizeof(double); i++ )
#endif        
         {
            c = RibGetChar( rib ); 
            if (EOF == c)
              goto Error;
            v[i] = (char)c;
         }
         
         /* Storing data as a double doesn't get you much right 
          *    now since it is going to be stored as a 32-bit float
          *    anyway.  RtFloat is of type float as of prman 3.6c.
          */
         if (pflt)
           *pflt = dbl;
         return kRIB_OK;
      }
   }
   else /* ASCII float */
   {
      int  sigc;
      int  expsigc;
      int  exponent;


      sigc = 0;
      exponent=0;
      d = 0;
      i = 0;
      fltbuf[0]='\0';
      tmp = RI_FALSE; /* True if a leading zero is found. */

      /* Handle initial +/- signs. */
      if ('-'==c || '+'==c)
      {
         fltbuf[i++]=c;
         c = RibGetChar( rib );
         if (EOF==c)
           goto Error;
      }

      /* Handle digits left of decimal point. */
      while (c >= '0' && c <= '9')
      {
         /* Handle initial zeros. */
         if (sigc || '0'!=c)
         {
            /* Handle significant figures. */
            if (sigc < RIB_MAX_SIGC)
            {
               fltbuf[i++]=c;
               sigc++;
            }
            else
            {
               exponent++;
            }
         }
         else
         {
            tmp = RI_TRUE; /* First digit was a zero. */
         }
         c = RibGetChar( rib );
	 /* Don't check for EOF here because it could be a EOF terminated
	  *    floating point value.
	  * if (EOF==c)
          *     goto Error;
	  */
      }

      /* Handle digits after decimal point. */
      if ('.'==c)
      {
         c = RibGetChar( rib );
	 /* Don't check for EOF here because it could be a EOF terminated
	  *    floating point value.
	  * if (EOF==c)
          *     goto Error;
	  */
         
         while (c >='0' && c <= '9')
         {
            /* Handle initial zeros. */
            if (sigc || '0'!=c)
            {
               /* Handle significant figure */
               if (sigc < RIB_MAX_SIGC)
               {
                  fltbuf[i++]=c;
                  sigc++;
               }
               else
                 exponent++;
            }
            d++; /* Count the number of digits after decimal point. */

            c = RibGetChar( rib );
	    /* Don't check for EOF here because it could be a EOF terminated
	     *    floating point value.
	     * if (EOF==c)
	     *     goto Error;
	     */
         }
      }

      /* Handle exponent. */
      w = 0;
      /* Check if there was a zero or significant figure and there is now
       *    a letter 'E' or 'e'.  This check indicates the start of a valid 
       *    exponent to read.
       */
      if ( (sigc||tmp) && ('E'==c || 'e'==c))
      {
         int expsign = 0;

         c = RibGetChar( rib );
         if (EOF==c)
           goto Error;

         /* Handle initial +/- signs. */
         if ( (expsign='-'==c) || '+'==c)
         {
            /*fltbuf[i++]=c;*/
            c = RibGetChar( rib );
            if (EOF==c)
              goto Error;
         }

         expsigc = 0;
         while ( c >= '0' && c <= '9' )
         {
            /* Handle initial zeros. */
            if (expsigc || '0'!=c)
            {
               /* Handle significant figure */
               if ( expsigc < RIB_MAX_EXPSIGC )
               {
                  w = w*10 + (c-'0');
                  expsigc++;
               }
            }
            c = RibGetChar( rib );
	    /* Don't check for EOF here because it could be a EOF terminated
	     *    floating point value.
	     * if (EOF==c)
	     *     goto Error;
	     */
         }
         if (!RibIsaWhiteSpace(c))
           RibUngetChar( rib, c );

         if (expsign)
           w *= -1;
      } /* if ('E'==c ... */
      else
      {
	 c = RibUngetChar( rib, c );
	 if (c)
	   goto Error;
      }
      exponent += w-d;

      if (sigc)
      {
         exponent = RIB_MAX( RIB_MIN( exponent, FLT_MAX_10_EXP + d), 
                            FLT_MIN_10_EXP - d); 

         fltbuf[i++] = 'e';
         if (exponent < 0)
         {
            fltbuf[i++] = '-';
            exponent *= -1;
         }
         fltbuf[i++] = (int)(exponent / 10) + '0';
         fltbuf[i++] = (int)(exponent % 10) + '0';
         fltbuf[i] = '\0';

         if (sigc>0 && pflt)
           *pflt = atof( fltbuf );
         return kRIB_OK;
      }
      else if ( tmp || d )
      {
        /* A zero digit to left or right of decimal point was found, but 
         *    no nonzero digits were found (except maybe in the exponent). 
         *    the value returned is to be zero.
         */
         if (pflt)
           *pflt = 0.0;
         return kRIB_OK;
      }

      if ( RibGetStatus(rib) & kRIB_STATUS_ARRAY )
      {
	 /* Don't print a syntax error message, let 
	  *    RibReadFloatingPointArray() do that.
	  */
	 return kRIB_ERRRC_INT;
      }

      c = 0; /* Fall through to Error, no float found. */

   } /* end ASCII format */
   
 Error:

   /* Add extra code so that byte offset points to where the error
    *    really occured.  If c is not EOF, then unget was called with 
    *    the character that really caused the syntax error.  A call
    *    to unget was made to see if the next part in the parser would
    *    know what to do with it.  If this code is reached then the
    *    next part in the parser didn't know.
    * To get the correct byte offset without a bunch of extra code calling
    *    RibGetChar() and RibUngetChar(), play with the byteoffset counter.
    *    This could have side effects if somebody is getting fancy with an
    *    error handler.
    */
   if ( EOF==c )
   {
      rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   }
   else
   {
      /* See above comment. */
      rib->byteoffset++;
      rib->error.type = kRIB_ERRTYPE_SYNTAX;
      c = kRIB_ERRRC_INT; /* Error */
   }
   RibSetError( rib, RIE_SYNTAX, (EOF==c ? RIE_SEVERE : RIE_ERROR), 
                &(rib->error) );
   if ( EOF!=c )
   {
      /* Restore the byteoffset counter to what it really is. */
      rib->byteoffset--;
   }

   return c; /* Error */
}


/* Macro for RibReadFloatingPointArray(). */
#define CheckForEOFError(a)  if(EOF==a) goto EOFErrorMsg;

/* RibReadFloatingPointArray 
 *  
 *   Reads an array of floats with length numberof.  A pointer for
 *   the array is returned using the pointer pp.  If pp is not set,
 *   an array is read from the RIB file and discarded.
 *
 *   If numberof is -1 and options has the kRIB_ARRAY_ENCODED set, the 
 *   length given in the encoded array is used for numberof and also as 
 *   the return code.  In this case kRIB_ERRRC_STREAMIO and EOF are the 
 *   return codes for errors.
 */
int RibReadFloatingPointArray( RIB_HANDLE hrib, RIB_UINT32 options, 
                              int numberof, float **pp )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   int                 tmp;
   RtBoolean           brackets;
   int                 rc = 0;
   int                 unget = -1; /* Don't unget a char before error msg. */
   auto int            c,i,w;
   auto unsigned int   l;
   auto RIB_UINT32     utmp;
   auto float          *p = NULL;


   if (!rib)
   {
      /* See above comment about numberof being -1. */
      if ( numberof==-1 )
         return kRIB_ERRRC_STREAMIO;
      else
         return kRIB_ERRRC_INT; /* Error */
   }

   /* Clear out white spaces and any RI or string definitions. */
   c = RibHandlePrefix( rib );
   CheckForEOFError(c);

   /* Mark that an array is being read so RibReadFloatingPoint() doesn't print
    *    syntax errors that RibReadFloatingPointArray() can give more detailed
    *    error messages on. 
    */
   RibSetStatus( rib, kRIB_STATUS_ARRAY, kRIB_STATUS_ARRAY );

   /* Handle array of IEEE single-precision floats. */
   /* 0310 + l | <length> | <array of floats> */
   if (( c >= 0310 ) && ( c <= 0313 ))
   {
      rib->status |= kRIB_STATUS_ENCODEDDATA;

      /* An encoded array can not have brackets around it without
       *    causing a syntax error.
       */

      l = c - 0310 + 1;
              
      /* Read length value by reading all l number of bytes. */
      utmp = 0;
      for ( w=0; w < l; w++ )
      {
         c = RibGetChar( rib );
         CheckForEOFError(c);
                 
         utmp = utmp << 8;
         utmp |= 0xff & c;
      }

      if ( utmp != (RIB_UINT32)numberof )
      {
         if ( numberof==-1 )
         {
            numberof = utmp;
         }
         else
         {
            rib->error.type = kRIB_ERRTYPE_ENCODED_ARRAY_SIZE;
            rib->error.expected.sys_int = numberof;
            rib->error.unexpected.user_int = utmp;

            /* Complain about size of array. */
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );

            /* Keep reading on.  Don't return from function yet. */
         }
      }

      /* Use l for the length of the array in sizeof(RtFloat) units. */
      if ( (int)utmp > numberof )
      {
         /* Don't want to write beyond allocated memory. */
         l = numberof;
      }
      else
      {
         l = (int)utmp;
      }
      if (pp)
      {
         p = (float*)_RibMalloc( l * sizeof(RtFloat) );
         if (!p)
         {
            rc = kRIB_ERRRC_INT;
            goto Error;
         }

         /* Read all <length> number of floats. */      
         for ( w=0; w < l; w++ )
         {
            /* Assume all the floats in the array are IEEE single-precision. */
            /* Read float. */
            tmp = 0;
            for ( i=0; i < sizeof(float); i++ )
            {
               c = RibGetChar( rib ); 
               CheckForEOFError(c);

               tmp = tmp << 8;
               tmp |= (0xff)&c;
            }
            p[w] = *( (float*)(void*)&tmp );
         } /* for (i=0; ... */
      }
      else
      {
         /* Read all <length> number of floats. */      
         for ( w=0; w < l; w++ )
         {
            /* Assume all the floats in the array are IEEE single-precision. */
            /* Read float. */
            for ( i=0; i < sizeof(float); i++ )
            {
               c = RibGetChar( rib ); 
               CheckForEOFError(c);
            }
         } /* for (i=0; ... */
      }

      /* If sizes didn't match, read through extra trailing stuff. */
      if (utmp > numberof)
      {
         for ( i=numberof; i < utmp; i++ )
         {
            c = RibGetChar( rib ); 
            CheckForEOFError(c);
         }
      }

      /* The return code has a special meaning when kRIB_ARRAY_ENCODED set. */
      if ( options & kRIB_ARRAY_ENCODED )
      {
         if (pp)
         {
            *pp = p;
         }

	 RibSetStatus( rib, kRIB_STATUS_ARRAY, 0 );

         return numberof;
      }
   }
   /* Else handle ASCII array of floats and arrays that are of 
    *    individually encoded floats. 
    */
   else
   {            
      /* Handle initial square bracket if present. */
      utmp = kRIB_BRACKETS_MASK & options;
      if ( '['==c )
      {
         /* Check that one of the kRIB_BRACKETS_* flag bits were set
          *    except kRIB_BRACKETS_CLOSE, because kRIB_BRACKETS_CLOSE
          *    is the only nonzero kRIB_BRACKETS_* flag that makes an 
          *    opening bracket '[' illegal.  Note that kRIB_BRACKETS_NONE
          *    is a zero value that would not show up in an AND operation,
          *    so kRIB_BRACKETS_CLOSE is the only flag bit to check for.
          */
         if ( utmp && (kRIB_BRACKETS_CLOSE!=utmp) )
         {
            brackets = RI_TRUE;
         }
         else
         {
            /* kRIB_BRACKETS_CLOSE is set and a '[' character was found. */
            goto SyntaxErrorMsg;
         }
      }
      else
      {
         /* No opening '[' bracket was found, so now check if this is
          *    an error by looking at the kRIB_BRACKETS_* flag bits. 
          */
         if ( kRIB_BRACKETS_REQUIRED==utmp 
             || (kRIB_BRACKETS_PARAML==utmp && numberof > 1) )
         {
            rib->error.type = kRIB_ERRTYPE_OPEN_BRACKET;
            rc = kRIB_ERRRC_INT; /* error */
            goto ErrorMsg;
         }
         else if ( kRIB_BRACKETS_CLOSE==utmp )
         {
            brackets = RI_TRUE;
         }
         else
         {
            brackets = RI_FALSE;
         }
         /* The character is probably a part of the data, so return it 
          *    so the next RibRead...() function can look at it. 
          */
         c = RibUngetChar( rib, c );  if (c) return c;
      }

      /* Handle numerical data. */
      if ( numberof > 0 )
      {
         if (pp)
         {
            p = (RtFloat*)_RibMalloc( numberof * sizeof(RtFloat) );
            if (!p)
            {
               rc = kRIB_ERRRC_INT;
               goto Error;
            }

            for (i=0; i<numberof; i++)
            {
               c = RibReadFloatingPoint( rib, &p[i] );
               if (c)
               {
                  /* Error reported by RibReadFloatingPoint(),
                   *    but check if a closing square bracket was found 
		   *    and give an error message.
                   */
		  rc = c; /* Set rc to RibReadFloatingPoint()'s error code. */
		  c = i; /* Assign c the number of values found. */
		  goto MissingValuesError;
               }
            }
         }
         else
         {
            for (i=0; i<numberof; i++)
            {
               c = RibReadFloatingPoint( rib, NULL );
               if (c)
               {
                  /* Error reported by RibReadFloatingPoint() 
                   *    and there is no memory to free, but check if a 
		   *    closing square bracket was found and give an
		   *    error message.
                   */
		  rc = c; /* Set rc to RibReadFloatingPoint()'s error code. */
		  c = i; /* Assign c the number of values found. */
		  goto MissingValuesError;
               }
            }
         }
      }
      else
      {
         /* 3-15-98:
          *
          * Allow zero length arrays because catrib and typerib can
          *    actually print such things out and the output from these
          *    programs should not cause a syntax error when reentered
          *    back into one of them.
          *
          * Example:
          *
          *    % echo 'NuPatch   0 1 [0] 0 0 0 1 [0] 0 0 "Pw"  []' | typerib
          *    ##RenderMan RIB-Structure 1.0
          *    NuPatch 0 1 [0 ] 0 0 0 1 [0 ] 0 0 "Pw" []
          * 
          * The following if() was added to strings and integer arrays as 
          *    well.   
          */
         if ( numberof < 0 )
	 {
	    rc = kRIB_ERRRC_INT;
	    goto Error;
	 }
      }

      /* The number of expected floats was read, but there might be more
       *    floats that follow before the closing square bracket.  Remember
       *    to free the pointer p if the array is determined to be invalid
       *    because of extra floats.
       */
      if (brackets)
      {
         /* Handle white spaces and any RI request or string definitions. */
         c = RibHandlePrefix( rib );
         CheckForEOFError(c);

         if ( ']'!=c )
         {
            if (isdigit(c)||'+'==c||'-'==c||'.'==c /*ASCII float or integer.*/
                 || (c>=0200 && c<=0217) /* Encoded float or integer, */
                 || 0244==c              /* Encoded IEEE float. */ 
                 || 0245==c )            /* Encoded IEEE double. */ 
            {
               RibUngetChar( rib, c );
               i = numberof;
               do
               {
                  c = RibReadFloatingPoint( rib, NULL );
                  if (c)
                  {
                     /* Error reported by RibReadFloatingPoint() already. 
                      *    So no message, just a return code and some
                      *    clean-up handle at Error label.
                      */
                     rc = c; /* Error code from RibReadFloatingPoint(). */
                     goto Error;
                  }
                  /* Found an extra float.  Keep track of how many extra by 
                   *    storing the total in i.
                   */
                  i++;
               }
               while ( RibIsaNumberNext(rib) );

	       /* Try to make the byte offset accurate.  If there is a ']'
		*    character present, it was the ']' to cause the syntax 
		*    error.
		*/
	       c = RibGetChar( rib );
	       if (EOF==c)
		  goto EOFErrorMsg;
	       if ( ']'!=c )
		  RibUngetChar( rib, c );

               rib->error.type = kRIB_ERRTYPE_FLOAT_ARRAY_SIZE;
               rib->error.expected.sys_int = numberof; /* Expected length. */
               rib->error.unexpected.user_int = i; /* Longer array found. */
               
               /* Complain about size of array. */
               RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );

               /* There is still an error condition, so set a return code. */
               rc = kRIB_ERRRC_INT; /* Error */

               /* Allow for a missing square bracket error, so continue to 
                *   the following if ( ']'!=c ) statement. 
                */
            }
            if ( ']'!=c )
            {
               rib->error.type = kRIB_ERRTYPE_CLOSE_BRACKET;
               rc = kRIB_ERRRC_INT; /* Error */
               goto ErrorMsg;
            }
            /* A few lines before if( ']'!=c ){} given above, rc could have
             *    given an error code.  The following "if (rc)" allows 
             *    that code use of the if( ']'!=c ){} code and still return 
             *    an error condition.
             */
            if (rc)
               goto Error;
         }
      }
   }

   if (pp)
   {
      *pp = p;
   }

   RibSetStatus( rib, kRIB_STATUS_ARRAY, 0 );

   return kRIB_OK;

 MissingValuesError:
   /* Reaching here means that rc is an error code from a RibRead...()
    *    function, c is the number of values found, numberof is greater
    *    that c, and numberof is the number of expected values in the
    *    array.
    */
   if ( rc!=EOF )
   {
      if ( ']'== RibGetChar(rib) && brackets )
      {
	 rib->error.type = kRIB_ERRTYPE_FLOAT_ARRAY_SIZE;
	 rib->error.expected.sys_int = numberof;
	 rib->error.unexpected.user_int = c;

	 goto ErrorMsg;      
      }
      else
      {
	 unget = c;
	 goto SyntaxErrorMsg;
      }
   }
   /* Else fall through to EOF file error. */

 EOFErrorMsg:
   /* Jump here when EOF encountered. */
   rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   rc = EOF;
   /* Skip over SyntaxErrorMsg code. */
   goto ErrorMsg;

 SyntaxErrorMsg:
   rib->error.type = kRIB_ERRTYPE_SYNTAX;
   if ( options & kRIB_ARRAY_ENCODED )
      rc = kRIB_ERRRC_STREAMIO; /* Error */
   else
      rc = kRIB_ERRRC_INT; /* Error */

 ErrorMsg:
   RibSetError( rib, RIE_SYNTAX, (EOF==rc ? RIE_SEVERE : RIE_ERROR), 
                &(rib->error) );

 Error:
   if (unget>=0)
   {
      RibUngetChar( rib, unget );
   }
   if (p)
   {
      _RibFree( p );
   }
   if (pp)
   {
      *pp = NULL;
   }
   RibSetStatus( rib, kRIB_STATUS_ARRAY, 0 );

   return rc;
}
#undef CheckForEOFError

/* When reading an integer it can only be encoded as a fixed point
 *    number with d as zero or as an ASCII integer with no decimal
 *    point.  Integers and arrays of integers can not be encoded
 *    as floats (0244 or 0245) or as arrays of floats (0310 to 0313).
 */
int RibReadInteger( RIB_HANDLE hrib, RtInt *pint )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   char      intbuf[RIB_MAX_SIGC+3]; 
   int       c,tmp;
   int       w;
   int       sigc;
   auto int  i;


   if (!rib)
     return kRIB_ERRRC_INT; /* Error */

   /* Clear out white spaces and any RI or string definitions. */
   c = RibHandlePrefix( rib );
   if (c==EOF)
     goto Error;

   if (RibIsaBinaryChar(c))
   {
      /* Handle integers and fixed-point numbers. */
      /* 0200 + (d*4) + w | <value> */

      /* See note below about tokens in the 0200 to 0217 range. */
      if (( c >= 0200 ) && ( c <= 0217 ))
      {
         rib->status |= kRIB_STATUS_ENCODEDDATA;
         w = (int)(0x00000003 & c) + 1;

         /* Return error if d > 0. */
         if ( (0x0000000C & c) >> 2 )
         {
            rib->error.type = kRIB_ERRTYPE_BAD_ENCODED_INT;
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
            
            return kRIB_ERRRC_INT; /* Error */
         }
         
         /* Read all bytes to the left of the decimal. */
         tmp = 0;
         for ( i=0; i < w; i++ )
         {
            c = RibGetChar( rib ); 
            if (EOF == c)
              goto Error;
            tmp = tmp << 8;
            tmp |= (0xff)&c;
         }
         i = w*8;
         if  ( tmp & ( 1 << (i-1) ))
           tmp |= 0xffffffff << i;

         if (pint)
           *pint = tmp;
      }
   }
   else /* ASCII integer */
   {
      sigc = 0;
      intbuf[0]='\0';
      i = 0;
      /* Handle initial +/- signs. */
      if ('-'==c || '+'==c)
      {
         intbuf[i++]=c;
         c = RibGetChar( rib );
         if (EOF==c)
           goto Error;
      }

      /* Handle digits. */
      while (c >= '0' && c <= '9')
      {
         /* The catrib (3.6c) utility allows initial zeros, but not a 
          *    following decimal even if it is 256. or 256.0.  
          *    The value 0256 is allowed.
          */
         /* Handle initial zeros. */
         if ( sigc || '0'!=c )
         {
            /* Handle significant figures. */
            if (sigc < RIB_MAX_SIGC)
            {
               intbuf[i++]=c;
               sigc++;
            }
         }
         else
         {
            /* Right now the following is true: (sigc==0 && c=='0'). */
            intbuf[0]='0';
         }
         c = RibGetChar( rib );
	 /* Don't check for EOF here because it could be an EOF terminated
	  *    valid integer value.
	  * if (EOF==c)
          *     goto Error;
	  */
      }
      /* Allow for a stand alone zero. */
      if ('0'==intbuf[0])
      {
         sigc++;
      }

      /* Any non-digit character ends reading an integer:
       *       % echo 'Sides 1Sides 0' | catrib
       *       ##RenderMan RIB
       *       version 3.03
       *       Sides 1
       *       Sides 0
       * 
       * If it is an integer, there can not be a decimal point. 
       *
       * If there is a decimal point, let the next RibRead* function
       *    stumble into it and declare a syntax error.
       *    This method seems to be what catrib does.
       *
       *       % echo 'Sides 1.' | catrib
       *       P79014 RIB syntax error (at line 1 in stdin, last token .
       *
       *       % echo 'Sides 1.0' | catrib
       *       P79014 RIB syntax error (at line 1 in stdin, last token .
       *
       * NOTE: 01-03-98 
       *   The above technique of letting the next RibRead* function
       *       handle a decimal point following an ASCII integer creates
       *       cryptic messages such as in the following example:
       * 
       *       % echo 'Hider "hidden" "jitter" [0.001]' | typerib
       *       ##RenderMan RIB-Structure 1.0
       *       ERROR: "<stdin>", line 1: Syntax error.
       *           Expected ']' to mark end of array.
       *
       *   Fix this by adding a new error message and then continue
       *       reading.  If digits follow the decimal point then the
       *       next RibRead* function can give its cryptic message, but
       *       atleast the user can see where the real problem is.
       *
       *   Now the following output is seen:
       *
       *       % echo 'Hider "hidden" "jitter" [0.001]' | typerib
       *       ##RenderMan RIB-Structure 1.0
       *       ERROR: "<stdin>", line 1: Syntax error.
       *           Found decimal point following an integer value.
       */
      if ( '.'==c )
      {         
         rib->error.type = kRIB_ERRTYPE_DECIMATEDINT; /* Sorry for the pun. */
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         return EOF; /* error */
      }
      else if (!RibIsaWhiteSpace(c))
      {
         RibUngetChar( rib, c );
      }

      /* If no significant figures counted then last c is the error code
       *    given to Error.  
       */
      if (!sigc)
      {
	 if ( ']'==c && RibGetStatus(rib) & kRIB_STATUS_ARRAY )
	 {
	    /* Don't print a syntax error message, let 
	     *    RibReadIntegerArray() do that.
	     */
	    return kRIB_ERRRC_INT;
	 }
	 /* Check for error and if needed move return code into c. */
	 else 
	 {
	    /* If c is just a character, Error will turn it into 
	     *    a kRIB_ERRRC_INT.
	     */
	    goto Error;
	 }
      }
      intbuf[i] = '\0';
      if (sigc>0 && pint)
        *pint = atoi( intbuf );
   }

   return kRIB_OK;

 Error:

   /* Add extra code so that byte offset points to where the error
    *    really occured.  If c is not EOF, then unget was called with 
    *    the character that really caused the syntax error.  A call
    *    to unget was made to see if the next part in the parser would
    *    know what to do with it.  If this code is reached then the
    *    next part in the parser didn't know.
    * To get the correct byte offset without a bunch of extra code calling
    *    RibGetChar() and RibUngetChar(), play with the byteoffset counter.
    *    This could have side effects if somebody is getting fancy with an
    *    error handler.
    */
   if ( EOF==c )
   {
      rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   }
   else
   {
      /* See above comment. */
      rib->byteoffset++;
      rib->error.type = kRIB_ERRTYPE_SYNTAX;
      c = kRIB_ERRRC_INT; /* Error */
   }
   RibSetError( rib, RIE_SYNTAX, (EOF==c ? RIE_SEVERE : RIE_ERROR), 
                &(rib->error) );
   if ( EOF!=c )
   {
      /* Restore the byteoffset counter to what it really is. */
      rib->byteoffset--;
   }

   return c; /* Error */
}


/* Macro for RibReadIntegerArray(). */
#define CheckForEOFError(a)  if(EOF==a) { rc=EOF; goto EOFErrorMsg; }

int RibReadIntegerArray( RIB_HANDLE hrib, unsigned int options, 
                        int numberof, RtInt **pp )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   RtBoolean           brackets;
   int                 rc = 0;
   int                 unget = -1; /* Don't unget a char before error msg. */
   auto int            c,i;
   auto RtInt          *p = NULL;


   if (!rib)
     return kRIB_ERRRC_INT; /* Error */

   /* Clear out white spaces and any RI or string definitions. */
   c = RibHandlePrefix( rib );
   CheckForEOFError(c);

   /* Mark that an array is being read so RibReadInteger() doesn't print
    *    syntax errors that RibReadIntegerArray() can give more detailed
    *    error messages on. 
    */
   RibSetStatus( rib, kRIB_STATUS_ARRAY, kRIB_STATUS_ARRAY );

   /* Handle initial square bracket if present. */
   i = kRIB_BRACKETS_MASK & options;
   if ( '['==c )
   {
      /* Check that one of the kRIB_BRACKETS_* flag bits were set
       *    except kRIB_BRACKETS_CLOSE, because kRIB_BRACKETS_CLOSE
       *    is the only nonzero kRIB_BRACKETS_* flag that makes an 
       *    opening bracket '[' illegal.  Note that kRIB_BRACKETS_NONE
       *    is a zero value that would not show up in an AND operation,
       *    so kRIB_BRACKETS_CLOSE is the only flag bit to check for.
       */
      if ( i && kRIB_BRACKETS_CLOSE!=i )
      {
         brackets = RI_TRUE;
      }
      else
      {
         /* kRIB_BRACKETS_CLOSE is set and a '[' character was found. */
         goto SyntaxErrorMsg;
      }
   }
   else
   {
      /* No opening '[' bracket was found, so now check if this is
       *    an error by looking at the kRIB_BRACKETS_* flag bits. 
       */
      if ( kRIB_BRACKETS_REQUIRED==i 
          || (kRIB_BRACKETS_PARAML==i && numberof > 1) )
      {
         rib->error.type = kRIB_ERRTYPE_OPEN_BRACKET;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         
         rc = kRIB_ERRRC_INT;
	 goto Error;
      }
      else if ( kRIB_BRACKETS_CLOSE==i )
      {
         brackets = RI_TRUE;
      }
      else
      {
         brackets = RI_FALSE;
      }
      /* The character is probably a part of the data, so return it 
       *    so the next RibRead...() function can look at it. 
       */
      c = RibUngetChar( rib, c );  if (c) return c;
   }

   /* Handle numerical data. */
   if ( numberof > 0 )
   {
      if (pp)
      {
         p = (RtInt*)_RibMalloc( numberof * sizeof(RtInt) );
         if (!p)
         {
            rc = kRIB_ERRRC_INT;
            goto Error;
         }
         
         for (i=0; i<numberof; i++)
         {
            c = RibReadInteger( rib, &p[i] );
            if (c)
            {
               /* Error reported by RibReadInteger(), 
		*    but check if a closing square bracket was found 
		*    and give an useful error message.
		*/
	       rc = c; /* Set rc to RibReadInteger()'s error code. */
	       c = i; /* Assign c the number of values found. */
	       goto MissingValuesError;
            }
         }
      }
      else
      {
         for (i=0; i<numberof; i++)
         {
            c = RibReadInteger( rib, NULL );
            if (c)
            {
               /* Error was reported by RibReadInteger() and
                *    there is no memory to free, but check if a 
		*    closing square bracket was found and give an
		*    error message.
		*/
	       rc = c; /* Set rc to RibReadInteger()'s error code. */
	       c = i; /* Assign c the number of values found. */
	       goto MissingValuesError;
            }
         }
      }
   }
   else
   {
      /* See comment about 3-15-98 change.  Search for "3-15-98". */
      if ( numberof < 0 )
      {
         rc = kRIB_ERRRC_INT;
	 goto Error;
      }
   }
   if (brackets)
   {
      /* Handle white spaces and any RI request or string definitions. */
      c = RibHandlePrefix( rib );
      CheckForEOFError(c);
      
      if ( ']'!=c )
      {
         if ( isdigit(c) || '+'==c || '-'==c /* ASCII integer. */
             || (c>=0200 && c<=0217)) /* Encoded integer, */
         {
            RibUngetChar( rib, c );
            i = numberof;
            do
            {
               c = RibReadInteger( rib, NULL );
               if (c)
               {
                  /* Error reported by RibReadInteger() already. 
                   *    So no message, just a return code and some
                   *    clean-up handle at Error label.
                   */
                  rc = c; /* Error code from RibReadInteger(). */
                  goto Error;
               }

               /* Found an extra integer.  Include it in the array length. */
               i++;
            }
            while ( RibIsaNumberNext(rib) );
            
	    /* Try to make the byte offset accurate.  If there is a ']'
	     *    character present, it was the ']' to cause the syntax 
	     *    error.
	     */
	    c = RibGetChar( rib );
	    if (EOF==c)
	       goto EOFErrorMsg;
	    if ( ']'!=c )
	       RibUngetChar( rib, c );

            rib->error.type = kRIB_ERRTYPE_INTEGER_ARRAY_SIZE;
            rib->error.expected.sys_int = numberof;
            rib->error.unexpected.user_int = i;
            
            /* Complain about size of array. */
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
            
            /* There is still an error condition, so set a return code. */
            rc = kRIB_ERRRC_INT; /* Error */
            
            /* Allow for a missing square bracket error, so continue to 
             *   the following if ( ']'!=c ) statement. 
             */
         }
         if ( ']'!=c )
         {
            rib->error.type = kRIB_ERRTYPE_CLOSE_BRACKET;
            rc = kRIB_ERRRC_INT; /* Error */
            goto ErrorMsg;
         }
         /* A few lines before if( ']'!=c ){} given above, rc could have
          *    given an error code.  The following "if (rc)" allows 
          *    that code use of the if( ']'!=c ){} code and still return 
          *    an error condition.
          */
         if (rc)
            goto Error;
      }
   }
   
   if (pp)
   {
      *pp = p;
   }

   RibSetStatus( rib, kRIB_STATUS_ARRAY, 0 );
   
   return kRIB_OK;

 MissingValuesError:
   /* Reaching here means that rc is an error code from a RibRead...()
    *    function, c is the number of values found, numberof is greater
    *    that c, and numberof is the number of expected values in the
    *    array.
    */
   if ( rc!=EOF )
   {
      if ( ']'== RibGetChar(rib) && brackets )
      {
	 rib->error.type = kRIB_ERRTYPE_FLOAT_ARRAY_SIZE;
	 rib->error.expected.sys_int = numberof;
	 rib->error.unexpected.user_int = c;

	 goto ErrorMsg;      
      }
      else
      {
	 unget = c;
	 goto SyntaxErrorMsg;
      }
   }
   /* Else fall through to EOF file error. */

 EOFErrorMsg:
   /* Jump here when EOF encountered. */
   rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   rc = EOF;
   /* Skip over ErrorMsg code. */
   goto ErrorMsg;

 SyntaxErrorMsg:
   rib->error.type = kRIB_ERRTYPE_SYNTAX;
   rc = kRIB_ERRRC_INT; /* Error */

 ErrorMsg:
   RibSetError( rib, RIE_SYNTAX, (EOF==rc ? RIE_SEVERE : RIE_ERROR), 
                &(rib->error) );

 Error:
   if (unget>=0)
   {
      RibUngetChar( rib, unget );
   }
   if (p)
   {
      _RibFree( p );
   }
   if (pp)
   {
      *pp = NULL;
   }
   RibSetStatus( rib, kRIB_STATUS_ARRAY, 0 );

   return rc;
}
#undef CheckForEOFError


int RibReadArrayAndLength( RIB_HANDLE hrib, RIB_UINT32 options, 
                        int *pnumberof, void **pp )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   RtBoolean       brackets = RI_FALSE;
   RtFloat         f;
   RtInt           i;
   RtString        *s;
   auto void       *p = NULL;
   auto int        c, n;


   if (!rib)
     return kRIB_ERRRC_INT; /* Error */

   /* Reset buffer and do not save characters from RibGetChar(). */
   RibSaveToBuffer( rib, RI_FALSE );

   /* The calls RiGeneralPolygon, RiPointsPolygons, RiGeneralPolygons,
    *    RiTrimCurve are missing their first parameter when converted to 
    *    the RIB bindings GeneralPolygon, PointsPolygons, GeneralPolygons,
    *    and TrimCurve respectively.  This first missing parameter (called 
    *    either nloops or npolys) is an integer with a value equal to the 
    *    number of items in the first non-missing parameter, which is 
    *    always an array.  The first non-missing parameter is always the 
    *    Ri call's second parameter.
    *
    * To read an array of unknown length, first determine if it is an encoded 
    *    array or a list of values.  If it is encoded then the size is known 
    *    allowing memory to be be allocated and the values read into the 
    *    allocated memory.  If the array is a list of values, each value is 
    *    read and then stored as a float or integer into a buffer.  The 
    *    flags in parameter "options" determine if the values are stored as 
    *    floats or integers.  If a closing square bracket is read in, the 
    *    array has been read and the size of the buffer determines the amount 
    *    of memory needed for the array.  Memory is then allocated and the 
    *    buffer is copied into the allocated memory.  
    *
    * Refer to RiPolygon() which unlike the calls mentioned above requires
    *    that the first missing parameter be determined by an array of 
    *    floats given in the parameter list.
    */

   /* Clear out white spaces and any RI or string definitions. */
   c = RibHandlePrefix( rib );
   if (c==EOF)
     goto Error;

   if ( (options&kRIB_BRACKETS_MASK)==kRIB_BRACKETS_CLOSE )
   {
      brackets = RI_TRUE;
      /* Unget c so that the next RibRead...() call can read
       *    it as a part of a value.
       */
      RibUngetChar( rib, c );
   }
   /* Check for the start of an array by looking for an opening square
    *   bracket or the beginning of an encoded array of IEEE floats.
    */
   else if ('['==c )
   {
      brackets = RI_TRUE;
      /* Clear out white spaces and any RI or string definitions that
       * can follow an opening bracket. 
       */
      c = RibHandlePrefix( rib );
      if (c==EOF)
        goto Error;

      /* Unget c so that the next RibRead...() call can read
       *    it as a part of a value.
       */
      RibUngetChar( rib, c );
   }
   else 
   {
      /* Unget c so that the next RibRead...() call can read
       *    it as a part of a value.
       */
      RibUngetChar( rib, c );
      brackets = RI_FALSE;

      if ( c>=0310 && c<=0313 )
      {
         /* An array of integers is not allowed to be encoded as an
          *   array of floats.  The catrib 3.6c program gives an error
          *   for this case.
          */
         if ( options & kRIB_ARRAY_INTEGER )
           return kRIB_ERRRC_INT;
         
         /* An encoded array acts like an ASCII array surrounded by
          *   square brackets.
          */
         options &= ~kRIB_BRACKETS_MASK;
         options |= kRIB_BRACKETS_NONE;
         
         /* The encoded array has a length included in the encoding.  
          *    Call RibReadFloatingPointArray() to read the encoded array.
          *    A minus one is a special indicator to the function.
          */
         c = RibReadFloatingPointArray( rib, kRIB_ARRAY_ENCODED, -1, 
                                       (RtFloat**)pp );
         if ( c > 0 )
         {
            if (pnumberof)
              *pnumberof = c;
            return kRIB_OK;
         }
         else
         {
            goto Error;
         }
      }
   }

   /* Handle array of ASCII or encoded values. */
   if ( (brackets && (kRIB_BRACKETS_NONE==(options&kRIB_BRACKETS_MASK)))
       || (!brackets&&(kRIB_BRACKETS_REQUIRED==(options&kRIB_BRACKETS_MASK))))
   {
      rib->error.type = kRIB_ERRTYPE_OPEN_BRACKET;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      
      return kRIB_ERRRC_INT; /* error */
   }
   if ( options & kRIB_ARRAY_INTEGER )
   {
      if (brackets)
      {
         n = 0;
         for (;;)
         {
            /* Clear out white spaces and any RI or string definitions. */
            c = RibHandlePrefix( rib );
            if (c==EOF)
              goto Error;

            if ( c!=']' )
            {
               RibUngetChar( rib, c );
               c = RibReadInteger( rib, &i );
               if (c)
                 return c; /* Error already reported by RibReadInteger(). */
               RibPutChar( rib, ((char*)&i)[0] );
               RibPutChar( rib, ((char*)&i)[1] );
               RibPutChar( rib, ((char*)&i)[2] );
               RibPutChar( rib, ((char*)&i)[3] );
#if _RIB_RTLONGINT
               RibPutChar( rib, ((char*)&i)[4] );
               RibPutChar( rib, ((char*)&i)[5] );
               RibPutChar( rib, ((char*)&i)[6] );
               RibPutChar( rib, ((char*)&i)[7] );
#endif
               n++;
            }
            else
            {
               break;
            }
         }
         if (pnumberof)
         {
            *pnumberof = n;
         }
         if (pp)
         {
            p = (void*)RibCreateIntegerArrayFromBuffer( rib, 0 );
            if (!p)
              return kRIB_ERRRC_INT;
            *pp = (RtInt*)p;
         }
      }
      else
      {
         /* If there is no bracket then there is only one integer to read. */
         c = RibReadInteger( rib, &i );
         if (c)
           goto Error;
         
         if (pnumberof)
         {
            *pnumberof = 1;
         }
         if (pp)
         {
            p = (void*)_RibMalloc( sizeof(RtInt) );
            if (!p)
              return kRIB_ERRRC_PTR;
            *((RtInt*)p) = i;
            *pp = (RtInt*)p;
         }
      }
   }
   else if ( options & kRIB_ARRAY_FLOAT )
   {
      if (brackets)
      {
         n = 0;
         for (;;)
         {
            /* Clear out white spaces and any RI or string definitions. */
            c = RibHandlePrefix( rib );
            if (c==EOF) 
              goto Error;
            if ( c!=']' )
            {
               RibUngetChar( rib, c );
               c = RibReadFloatingPoint( rib, &f );
               if (c)
                 return c; /* Error reported by RibReadFloatingPoint(). */

               RibPutChar( rib, ((char*)&f)[0] );
               RibPutChar( rib, ((char*)&f)[1] );
               RibPutChar( rib, ((char*)&f)[2] );
               RibPutChar( rib, ((char*)&f)[3] );
#if _RIB_RTDOUBLE==1
               RibPutChar( rib, ((char*)&f)[4] );
               RibPutChar( rib, ((char*)&f)[5] );
               RibPutChar( rib, ((char*)&f)[6] );
               RibPutChar( rib, ((char*)&f)[7] );
#endif
                  n++;
            }
            else
            {
               break;
            }
         }
         if (pnumberof)
         {
            *pnumberof = n;
         }
         if (pp)
         {
            p = (void*)RibCreateFloatArrayFromBuffer( rib, 0 );
            if (!p)
              return kRIB_ERRRC_INT;
            *pp = (RtFloat*)p;
         }
      }
      else
      {
         /* If there is no bracket then there is only one float to read. */
         c = RibReadFloatingPoint( rib, &f );
         if (c)
           goto Error;
         
            if (pnumberof)
            {
               *pnumberof = 1;
            }
         if (pp)
         {
            p = (void*)_RibMalloc( sizeof(RtFloat) );
            if (!p)
              return kRIB_ERRRC_PTR;
            *((RtFloat*)p) = f;
            *pp = (RtFloat*)p;
         }
      }
   }
   /* Pixar's 3.5 and 3.6 tools didn't like string arrays, but version 3.7
    *    fixes problems with uniform string arrays.  
    */
   else if ( options & kRIB_ARRAY_STRING )
   {
      if (brackets)
      {
         n = 0;
         for (;;)
         {
            /* Clear out white spaces and any RI or string definitions. */
            c = RibHandlePrefix( rib );
            if (c==EOF) 
              goto Error;
            if ( c!=']' )
            {
               RibUngetChar( rib, c );
               c = RibReadString( rib, (pp?kRIB_STRING_RTARRAY:0), NULL );
               if (c)
                 return c; /* Error reported by RibReadString(). */
               n++;
            }
            else
            {
               break;
            }
         }
         if (pnumberof)
         {
            *pnumberof = n;
         }
         if (pp)
         {
            if (RibCreateStringsFromBuffer( rib, n, &s ))
            {
               return kRIB_ERRRC_INT;
            }
            if (!s)
	    {
	       return kRIB_ERRRC_INT;
	    }
            *pp = (RtString*)s;
         }
      }
      else
      {
         /* If there is no bracket then there is only one string to read. */
         c = RibReadString( rib, kRIB_STRING_RTSTRING, (RtString*)pp );
         if (c)
           goto Error;
         if (pnumberof)
         {
            *pnumberof = 1;
         }
      }      
   }
   else
   {
      /* Error in way RibReadArrayAndLength() was called. */
      return kRIB_ERRRC_INT; 
   }


   return kRIB_OK;

 Error:

   if ( EOF==c )
   {
      rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   }
   else
   {
      rib->error.type = kRIB_ERRTYPE_SYNTAX;
      c = kRIB_ERRRC_INT; /* Error */
   }
   RibSetError( rib, RIE_SYNTAX, (EOF==c ? RIE_SEVERE : RIE_ERROR), 
                &(rib->error) );

   return c; /* Error */
}


int RibReadDefiningBinCode( RIB_HANDLE hrib, int alreadyreadin )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   PRIB_HASHATOM  a;
   register int   c;
   int            token = 0;
   char           *s;
   auto int       i;


   if (!rib)
     return kRIB_ERRRC_INT; /* Error in way RibReadDefiningBinCode called */
   c = alreadyreadin;

   /* Handle definitions of an encoded RI request. */
   /* 0314 | <code> | <string> */
   if ( 0314 == c ) 
   {
      rib->status |= kRIB_STATUS_ENCODEDDATA;

      c = RibGetChar( rib );
      if (EOF==c)
        goto Error;

      token = c;

      if (RibReadString( rib, kRIB_STRING_ENCODED, &s ))
        return kRIB_ERRRC_INT; /* Error */

      /* Is version treated as a valid RI statement? (see below)
       *
       * Let "version #.##" also get defined as a RI request. 
       * 
       * % str2oct version
       * 227  166  145  162  163  151  157  156  
       *
       * % str2oct '3.03'
       * 224  063  056  060  063  
       *
       * % echo '0314 003 227 166 145 162 163 151 157 156  \ 
       *         0246 003 063  056  060  063 012' | oct2bin | ribdump
       *   314  003  227    v    e    r    s    i   # <defreq  03 "versi
       *     o    n  246  003    3    .    0    3   # on"> <req 03> 3.03
       *    \n                                      # \n
       *
       * The above rib input being displayed by ribdump is also acceptable
       *   to Pixar's catrib utility.
       * If you try to change the 'v' to an upper case 'V', catrib will
       *   print:
       *
       *     P01001 Unregistered request: "Version" (at line 1 in stdin)
       *
       * So it appears catrib actually accepts defining a "version" request.
       */
      
      /* Get index into RI Table based on the string given. */
      i = RibGetRIFromString( s );

      /* If i is zero or greater it represents a valid index into the RI
       *   table.  Otherwise the string s is not recognized, so give an error.
       */
      if ( i < 0 )
      {
         rib->error.type = kRIB_ERRTYPE_BAD_RIREQUEST;
         rib->error.unexpected.user_string = s;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         _RibFree( s );

         return kRIB_ERRRC_INT;
      }

      /* The index into the RI Table is known now, so dispose of the 
       * string "s".
       */
      _RibFree( s );

      if ( !RibAddItem( RibGetHashHandle(rib), 
                       kRIB_HASH_RIREQUESTCODE, token, i ) )
      {
         a = RibGetConflictingHashAtom( rib->phashtable );
         if (!a)
         {
            /* There is no already existing atom to get in the way, 
             *    so there must have been a problem adding to the hash table.
             */
            rib->error.type = kRIB_ERRTYPE_ADDING_TO_HASH;
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
            return kRIB_ERRRC_INT; /* Error */   
         }
         /* Handle existing RI request definition stored in hash table. */
         RibSetDefinedRIRequest( a, token, i );
      }

      return kRIB_OK;
   }
   /* Handle definitions of a string. */
   /* 0315 + w | <token> | <string>, w=(0 or 1) */
   else if ( 0316 == c )
   {
      /* Read token value */
      c = RibGetChar( rib );
      if (EOF==c)
      {
         rib->status |= kRIB_STATUS_ENCODEDDATA;
         goto Error;
      }

      token = c << 8;
   }
   else if ( 0315 != c )
   {            
      /* It could only be 0315 or 0316.  It wasn't either so give an error. */
      return kRIB_ERRRC_INT; /* Error in way RibReadDefiningBinCode called */
   }    
   rib->status |= kRIB_STATUS_ENCODEDDATA;

   c = RibGetChar( rib );
   if (EOF==c)
     goto Error;               

   token |= c; /* The variable token now equals the token value. */

   if (RibReadString( rib, kRIB_BRACKETS_NONE, &s ))
     return kRIB_ERRRC_INT; /* Error */

   if (!RibAddItem( RibGetHashHandle(rib), 
                   kRIB_HASH_STRINGTOKEN | kRIB_HASH_FREEEXTDATA, token, s ))
   {
      a = RibGetConflictingHashAtom( rib->phashtable );
      if (!a)
      {
         /* If there is no already existing atom to get in the way, 
          *    so there must have been a problem adding to the hash table.
          */
         rib->error.type = kRIB_ERRTYPE_ADDING_TO_HASH;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
         return kRIB_ERRRC_INT; /* Error */   
      }
      /* Handle existing string definition stored in hash table. */
      RibSetDefinedString( a, token, s );
   }

   return kRIB_OK;

 Error:

   rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   RibSetError( rib, RIE_SYNTAX, RIE_SEVERE, &(rib->error) );
   return EOF; /* Error */   
}


/* RibReadRIRequestToken returns an RI Table index after reading
 *   an expected one byte token that identifies an entry in the
 *   hash table.  The character 0246 has already been read in.
 */
int RibReadRIRequestToken( RIB_HANDLE hrib )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   PRIB_HASHATOM  a;
   register int  c;

   
   if (!rib)
     return kRIB_ERRRC_INDEX; 

   rib->status |= kRIB_STATUS_ENCODEDDATA;

   /* Handle definitions of an encoded RI request. */
   /* 0246 | <code> */
   /* The 0246 was already read in.  This function only handles the <code>. */

   c = RibGetChar( rib );
   if (EOF==c)
     goto Error;
   
   a = RibFindMatch( rib->phashtable, kRIB_HASH_RIREQUESTCODE, c, NULL );
   if (!a)
   {
      rib->error.type = kRIB_ERRTYPE_UNDEFINED_RI_CODE;
      rib->error.unexpected.user_int = c;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INDEX; /* Error */   
   }

   /* Optimize out extra function call and checking.  Just use the data 
    *    structure directly.
    */
   /*Replaced with code below: "return RibGetDefinedRIRequest( a );". */
   return a->with.ripID;

 Error:

   rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   RibSetError( rib, RIE_SYNTAX, RIE_SEVERE, &(rib->error) );
   return EOF; /* Error */   
}


/* RibReadThroughUndeclaredData()
 *   This makes an attempt at error recovery and tries to skip over an 
 *   undeclared parameter ID's data.  
 */
int RibReadThroughUndeclaredData( RIB_HANDLE hrib )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   RIB_UINT32  options;
   auto int c;


   /* Handle white spaces and any RI request or string definitions. */
   c = RibHandlePrefix( rib );
   if (EOF==c)
     goto Error;

   /* Handle data of unknown type by looking at the first character. */
   if ( '['==c )
   {
      c = RibHandlePrefix( rib );  
      if (EOF==c) 
        goto Error;
      options = kRIB_BRACKETS_CLOSE;
   }
   else
   {
      options = kRIB_BRACKETS_NONE;
   }

   if ( isdigit(c) || '+'==c || '-'==c || '.'==c /* ASCII float or integer. */
       || (c>=0200 && c<=0217) /* Encoded float or integer, */
       || (c>=0310 && c<=0313) /* Encoded array of IEEE floats. */
       || 0244==c              /* Encoded IEEE float. */ 
       || 0245==c )            /* Encoded IEEE double. */ 
   {
      c = RibUngetChar( rib, c ); if (c) goto Error;
      options |= kRIB_ARRAY_FLOAT;
      c = RibReadArrayAndLength( rib, options, NULL, NULL );
      if (c) 
        goto Error;
   }
   else if ( '\"'==c || 0317==c || (c>=0220 && c<=0243) )
   {
      c = RibUngetChar( rib, c ); if (c) goto Error;
      options |= kRIB_ARRAY_STRING;
      c = RibReadArrayAndLength( rib, options, NULL, NULL );
      if (c) 
        goto Error;
   }
   else if ( !options ) /* No error if there wasn't a '['. */
   {
      /* Assume that c marks the beginning of a RIB statement. 
       * If it isn't, some other RibRead...() function will find that out.  
       * For now return the character as unread so that the next RibRead...()
       * call can look at it.
       */
      c = RibUngetChar( rib, c );  
      if (c) 
        goto Error;
   }
   else
   {
      /* There was an open square bracket with no identifiable data 
       * following it.  Give a syntax error.
       */
      rib->error.type = kRIB_ERRTYPE_SYNTAX;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      
      return kRIB_ERRRC_INT;
   }
   return kRIB_OK;
   
 Error:

   return c;
}


/* RibIsaNumberNext()
 *
 *   Return RI_TRUE if a singleinteger or float follows.
 */
RtBoolean RibIsaNumberNext( RIB_HANDLE hrib )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   auto int c, rc;


   /* Handle white spaces and any RI request or string definitions. */
   c = RibHandlePrefix( rib );

   if ( isdigit(c) || '+'==c || '-'==c || '.'==c /* ASCII float or integer. */
       || (c>=0200 && c<=0217) /* Encoded float or integer, */
       || 0244==c              /* Encoded IEEE float. */ 
       || 0245==c )            /* Encoded IEEE double. */ 
   {
      rc = RI_TRUE;
   }
   else
   {
      rc = RI_FALSE;
   }
   RibUngetChar( rib, c );

   return rc;
}


/* RibIsaStringNext()
 *
 *   Return RI_TRUE if a string follows.
 */
RtBoolean RibIsaStringNext( RIB_HANDLE hrib )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   auto int c, rc;


   /* Handle white spaces and any RI request or string definitions. */
   c = RibHandlePrefix( rib );

   if ( '\"'==c || 0317==c || (c>=0220 && c<=0243) )
   {
      rc = RI_TRUE;
   }
   else
   {
      rc = RI_FALSE;
   }
   RibUngetChar( rib, c );

   return rc;
}


#define CheckForError(rc)  if(rc) goto Error
int RibReadRIBCallParameters( RIB_HANDLE hrib,
                       char *cparams, /* Refer to affine/ribhash/cparams.c. */ 
                       int asizes[], 
                       void *params )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   char       *p;
   char       *s1;
   char       *s2;
   auto int   a;
   auto char  *c;
   auto int   rc; /* return code */


   a = 0;
   c = cparams;
   p = (char*)params;
   while ( *c!=kRIB_CPARAMS_NULL && *c!=kRIB_CPARAMS_VECTORFORMAT )
   {
      switch (*c)
      {
       case kRIB_CPARAMS_TOKEN:
       case kRIB_CPARAMS_CHAR_POINTER:
         rc = RibReadString( rib, kRIB_BRACKETS_NONE, (char**)p );
         CheckForError(rc);
         p += sizeof(char*);
         break;
       case kRIB_CPARAMS_INT:
       case kRIB_CPARAMS_LIGHTHANDLE:
       case kRIB_CPARAMS_OBJECTHANDLE:
       case kRIB_CPARAMS_BOOLEAN:
         rc = RibReadInteger( rib, (RtInt*)p );
         p += sizeof(RtInt);
         break;
       case kRIB_CPARAMS_FLOAT:
         rc = RibReadFloatingPoint( rib, (RtFloat*)p );
         CheckForError(rc);
         p += sizeof(RtFloat);
         break;
       case kRIB_CPARAMS_BASIS:
         rc = RibHandlePrefix( rib );
         /* Don't change the following if() to 'CheckForError(rc);'
          *    because CheckForError() only looks for a nonzero value
          *    not EOF.
          */
         if (EOF==rc)
            goto Error;
         if ( rc=='\"' || rc == 0317 || rc == 0320 
             || (( rc >= 0220 ) && ( rc <= 0243 )) )
         {
            rc = RibUngetChar( rib, rc );  CheckForError(rc);
            rc = RibReadString( rib, kRIB_BRACKETS_NONE, 
                               (char**)&s1 );  CheckForError(rc);
            rc = RibReadStringForToken( s1, gRibBasisTable, (char**)&s2 );
            if ( rc<0 || *s2 )
            {
               rib->error.type = kRIB_ERRTYPE_BAD_BASISNAME;
               rib->error.unexpected.user_string = s1;
               RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
               _RibFree( s1 );
               
               return kRIB_ERRRC_INT; /* Error */   
            }
            else
            {
               auto RtFloat **f = (RtFloat**)p;
               
               *f = (RtFloat*)RiStdBasisTbl[rc];
            }
            _RibFree( s1 );

            p += sizeof(RtFloat*);

            break;
         }
         rc = RibUngetChar( rib, rc );
         CheckForError(rc);
         /* else fall through and read data as a simple matrix */
       case kRIB_CPARAMS_MATRIX:
         rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                        16, (RtFloat**)p );
         CheckForError(rc);
         p += sizeof(RtFloat*);
         break;
       case kRIB_CPARAMS_BOUND:
         rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL, 
                                        6, (RtFloat**)p );
         CheckForError(rc);
         p += sizeof(RtFloat*);
         break;
       case kRIB_CPARAMS_COLOR:
         rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_OPTIONAL,
                                        RibGetNColorSamples(rib),
                                        (RtFloat**)p );
         CheckForError(rc);
         p += sizeof(RtFloat*);
         break;
       case kRIB_CPARAMS_POINT:
         rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED,
                                        3, (RtFloat**)p );
         CheckForError(rc);
         p += sizeof(RtFloat*);
         break;
       case kRIB_CPARAMS_INT_ARRAY:
         rc = RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED, 
                                  asizes[a++], (RtInt**)p );
         CheckForError(rc);
         p += sizeof(RtInt*);
         break;
       case kRIB_CPARAMS_FLOAT_ARRAY:
         rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 
                                        asizes[a++], (RtFloat**)p );
         CheckForError(rc);
         p += sizeof(RtFloat*);
         break;
       case kRIB_CPARAMS_POINT_ARRAY:
         rc = RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED,
                                        3*asizes[a++], (RtFloat**)p );
         CheckForError(rc);
         p += sizeof(RtFloat*);
         break;
       case kRIB_CPARAMS_ERRORHANDLER:
         rc = RibReadString( rib, kRIB_BRACKETS_NONE, (char**)&s1 );
         CheckForError(rc);
         rc = RibReadStringForToken( s1, gRibErrorHandlerTable, 
                                    (char**)&s2 );
         if ( rc<0 || *s2 )
         {
            rib->error.type = kRIB_ERRTYPE_BAD_ERRORHANDLER;
            rib->error.unexpected.user_string = s1;
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
            _RibFree( s1 );
            
            return kRIB_ERRRC_INT; /* Error */   
         }
         else
         {
            /* The table ritable has function pointers for the three main 
             * error handlers: RiErrorIgnore, RiErrorPrint and RiErrorAbort.
             */
            auto RtErrorHandler *h = (RtErrorHandler*)p;
               
            *h = (RtErrorHandler)rib->ritable[gRibErrorHandlers[rc]];
         }
         _RibFree( s1 );
         p += sizeof(void*);
         break;
       case kRIB_CPARAMS_FILTERFUNC:
         rc = RibReadString( rib, kRIB_BRACKETS_NONE, (char**)&s1 );
         CheckForError(rc);
         rc = RibReadStringForToken( s1, gRibFilterTable, (char**)&s2 );
         if ( rc<0 || *s2 )
         {
            rib->error.type = kRIB_ERRTYPE_BAD_FILTERNAME;
            rib->error.unexpected.user_string = s1;
            RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
            _RibFree( s1 );
            
            return kRIB_ERRRC_INT; /* Error */   
         }
         else
         {
            /* The table ritable has function pointers for the five main 
             * pixel filters: RiBoxFilter, RiTriangleFilter, 
             * RiCatmullRomFilter, RiGaussianFilter and RiSincFilter.
             */
            auto RtFilterFunc  *f = (RtFilterFunc*)p;

            *f = (RtFilterFunc)rib->ritable[gRibFilters[rc]];
         }
         _RibFree( s1 );
         p += sizeof(void*);
         break;
         /* Not supported for reading: case kRIB_CPARAMS_VARIABLE_ARG:   */
         /* Not supported for reading: case kRIB_CPARAMS_POINTER:        */
         /* Not supported for reading: case kRIB_CPARAMS_TOKEN_ARRAY:    */
         /* Not supported for reading: case kRIB_CPARAMS_POINTER_ARRAY:  */
         /* Not supported for reading: case kRIB_CPARAMS_FUNC:           */
         /* Note: Nothing has an hpoint as a c parameter.  Hpoints are   */
         /*       only in parameter lists -- as of prman 3.6c.           */
       default:
         rc=1;
      }

      c++;
   }
   
   return kRIB_OK;

 Error:
   if ( EOF==rc )
   {
      rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
      RibSetError( rib, RIE_SYNTAX, RIE_SEVERE, &(rib->error) );
      return EOF; /* error */
   }
   else
   {
      rib->error.type = kRIB_ERRTYPE_SYNTAX;
      RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      return kRIB_ERRRC_INT; /* Error */
   }   
}


/* Rib_version()
 *    Default method for handling version #.## statments that are read
 *    in a RIB file.
 */
RtVoid Rib_version( RtFloat version )
{
   /* If you want to actually do something with this info,
    *    hook out the fuction at index kRIB__VERSION in the
    *    RI Table associated with the RIB file being read.
    */
   
   (void)version; /* Just ignore it. */
   
   return;
}


int RibReadFileForToken( RIB_HANDLE rib, char *table )
{
   int   i, n, l;
   char *t = table;
   int  c = 0;


   if ( rib && table )
   {
      c = RibGetChar( rib );
      for(;;)
      {
         n = *t++;
         l = *t++;

         for ( i=0; i < l; i++ )
         {        
            if ( c != *t )
            {
               if ( !*t && (  RibIsaBinaryChar(c) || RibIsaWhiteSpace(c)
                           || RibIsaDelimitor(c)) )
               {
                  t++;
                  RibUngetChar( rib, c );
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
               goto Error;
            }
            else
            {
               if (EOF==c)
                 goto Error;   
               c = RibGetChar( rib );
               t++;
            }
         } /* for (i=...) */
      } /* for (;;) */
   }
   
 Error:
   return (c==EOF ? EOF : kRIB_ERRRC_INDEX); /* Error */   
}
