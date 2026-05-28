/* $RCSfile: hintfunc.c,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:07 $
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
 * FILE:  hintfunc.c
 *
 * DESCRIPTION:  Library that reads both ASCII and binary RIB files.
 *   
 *    Contains:
 *
 *     History:
 *       1-03-98  Fixed possible hint handling bugs and created test
 *                cases to test code paths.
 *      01-08-98  Added fix "comment migration" problem.
 *      03-15-98  Fixed memory leak with playback of archive records.
 *      04-04-98  Changed the way kRIB_ARCHIVERECORD function in the RI table
 *                are called.  Instead of a format="%s" followed by a string,
 *                it is now just format pointing to the string.
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
#define __RIB_FILE_HINTFUNC
#define  _RIPRIV_TYPES
#define  _RIPRIV_FUNC_TYPES 
#include <stdio.h>
#include <ripriv.h>
#include <ribrdr.h>

/* 
 *   Static Globals
 *
 */
static char *types[] = { "comment", "structure" };
#define COMMENT_TYPE    0
#define HINT_TYPE       1
#define STRUCTURE_TYPE  1


/* 
 *   Globals
 *
 */
PRIB_ARCRECFILTERPROC gRibHintTable[] = {
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##CameraOrientation */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##CapabilitiesNeeded */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##CreationDate */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##Creator */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##For */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##Frames */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##Include */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##RenderMan */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##Scene */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler,/* ##Shaders */
   (PRIB_ARCRECFILTERPROC)RibDefaultHintHandler /* ##Textures */
};
/* Was thinking of adding Alias's "# Surface" as a hint, but Power Animator
 * 7.5.X's AliasToRenderman is using the "name" attribute.  So the problem
 * is fixed.
 */


int RibDefaultHintHandler( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   register int   rc;
   register char  *s;


   rc = RibReadArchiveRecord(rib);
   if ( rc )
      return rc;

   /* The flag kRIB_STATUS_SAVETOBUFFER will be set because 
    *    RibHandleArchiveRecord() before calling the archive
    *    handler associated with rib, which when a hint table
    *    is registered is the only place RibDefaultHintHandler() 
    *    can be called.
    */
   s = RibCreateStringFromBuffer( rib, 0 );
   if (!s)
      return kRIB_ERRRC_INT;
   
   /* Using types[] means that the function being called can not free this
    *    string pointer.  Basically this is another exception to the rule
    *    that all pointers given to the RI functions are up for grabs.
    */
   ((PRiArchiveRecord)*rib->ritable[kRIB_ARCHIVERECORD])(types[HINT_TYPE], s);
   if ( RibShouldFreeData(rib) )
   {
      _RibFree( s );
   }

   return kRIB_OK;
}


/* When RibDefaultArchiveRecordHandler() is called the first
 * '#' of a comment/hint line has already been read-in.  If
 * the next character is another '#' then line is a hint.
 */
int RibDefaultArchiveRecordHandler( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   char      *t;
   char      *s;
   auto int  index;
   register int  c;


   if (!rib && !rib->ritable)
     return kRIB_ERRRC_INT;

   c = RibGetChar( rib );
   if (EOF==c)
     goto Error;

   if ( c=='#' && rib->hinttable && !(rib->status & kRIB_STATUS_PARAML) )
   {
      RibIgnoreLastChar(rib);
      t = types[HINT_TYPE];

      /* Determine which hint it is. */
      index = RibReadFileForToken( rib, gRibHintTokenTable );

      if ( index >= 0 && index < kRIB_LAST_HINT )
      {
         if ( (*(rib->hinttable[index]))((RIB_HANDLE)rib) )
         {
            return kRIB_ERRRC_INT;
         }
         return kRIB_OK;
      }
      else
      {
         /* Unget last character incase it was a '\n'. */
         RibUngetChar( rib, c );
         c = RibReadArchiveRecord( rib );
         if (c)
           return c;
      }
   }
   else
   {
      if (c=='#')
      {
         RibIgnoreLastChar(rib);
         t = types[HINT_TYPE];
      }
      else
      {
         /* Unget last character incase it was a '\n'. */
         RibUngetChar( rib, c );
         t = types[COMMENT_TYPE];
      }

      /* Read remainder of archive record. */
      if ( RibReadArchiveRecord(rib) )
        return kRIB_ERRRC_INT; /* Error already reported. */
   }

   s = RibCreateStringFromBuffer( rib, 0 );

   if (!s)
     return kRIB_ERRRC_INT;

   /* To prevent "comment migration" (refer to paraml.c) store
    *    archive records in a list if status flag kRIB_STATUS_PARAML 
    *    is set.  RibArcRecPlayBack() will playback the stored list
    *    of comments and hints.
    */
   if (rib->status & kRIB_STATUS_PARAML)  
   {
      PRIB_ARCREC  p;


      p = (PRIB_ARCREC)_RibMalloc( sizeof(RIB_ARCREC) );
      if (!p)
      {
         _RibFree( s );
         return kRIB_ERRRC_INT; /* Error */   
      }
      p->type = t;
      p->record = s;
      p->next = NULL;
      if (!rib->arcreclist)
         rib->arcreclist = p;
      if (rib->arcreclistend)
         rib->arcreclistend->next = p;
      rib->arcreclistend = p;
      return kRIB_OK;      
   }

   /* NOTE:  Treating the archive record as a string has the limitation
    *        that it maybe terminated before the actual end of the string
    *        if a NULL is encountered.  But this seems to be the expected
    *        behavior.
    *
    *        Below is an encoded string printed as ASCII octal numbers.
    *
    *              > str2oct #abcd
    *              225  43  141  142  143  144  
    *         
    *        Using ribdump, the decoding of the string can be shown:
    *
    *              >str2oct #abcd | oct2bin | ribdump
    *              225    #    a    b    c    d             # "#abcd"
    *
    *        Dropping the prefix 225, the remaining numbers are octal 
    *        numbers representing the ASCII codes of the letters in 
    *        the string "#abcd".  Using these octal numbers the string 
    *        can be printed as a comment and seen with catrib:
    *
    *              >echo 43 141 142 143 144 | oct2bin | catrib
    *              ##RenderMan RIB
    *              #abcd
    *
    *        The "##RenderMan RIB" is something catrib likes to add.
    *        Now add a '\0' in the middle of the comment right after 
    *        the 143 octal number:
    *        
    *              >echo 43 141 142 143 0 144 | oct2bin | catrib
    *              ##RenderMan RIB
    *              #abc
    *
    *        Notice that the 'd' (144) is missing because the NULL 
    *        terminated the printing of the string.  Also notice
    *        that catrib didn't print a syntax error complaining
    *        about the extra 'd' (144).  Inshort the 'd' was read
    *        as a part of the comment, but was lost only during
    *        the printing out.  (Two separate libraries doing something
    *        similiar, but in different ways.)
    *
    *        Basically don't put binary characters in your 
    *        comments and structures.  
    * 
    */
   ((PRiArchiveRecord)*rib->ritable[kRIB_ARCHIVERECORD])( t, s );   
   if ( RibShouldFreeData(rib) )
   {
      _RibFree( s );
   }

   return kRIB_OK;

 Error:

   /* Handle syntax error. */
   rib->error.type = kRIB_ERRTYPE_UNEXPECTED_EOF;
   RibSetError( rib, RIE_SYNTAX, RIE_SEVERE, &(rib->error) );
   return EOF; /* Error */   
}


int RibSetArchiveRecordHandler( RIB_HANDLE hrib, PRIB_ARCRECFILTERPROC p )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (rib)
   {
      rib->arcrechandler = p;
   }
   else
   {
      return 1;
   }
   
   return kRIB_OK;
}


int RibIgnoreArchiveRecords( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;

   if (rib)
   {
      rib->arcrechandler = NULL;
   }
   else
   {
      return 1;
   }
   
   return kRIB_OK;
}


int RibArcRecPlayBack( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   PRIB_ARCREC  p,pp;


   if (!rib && !rib->ritable)
     return kRIB_ERRRC_INT;

   p = rib->arcreclist;
   while (p)
   {
      ((PRiArchiveRecord)*rib->ritable[kRIB_ARCHIVERECORD])( p->type,
                                                             p->record );
      if ( RibShouldFreeData(rib) )
      {
         #ifdef RIB_MEMORY_TESTS
         RibMemoryTestStepOverAllowed();
         #endif
         _RibFree( p->record );
      }
      pp = p;
      p = pp->next;
      #ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
      #endif
      _RibFree( pp );
   }
   rib->arcreclist = NULL;
   rib->arcreclistend = NULL;

   return kRIB_OK;
}


/* This function should only get called when a RIB has been suddenly closed
 *    by a program before it was entirely read-in.  RibClose() calls this
 *    function just in case.
 */
int RibFreeArcRecPlayBack( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   PRIB_ARCREC  p,pp;


   if (!rib)
     return kRIB_ERRRC_INT;

   p = rib->arcreclist;
   while (p)
   {
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
      _RibFree( p->record );
      pp = p;
      p = pp->next;
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
      _RibFree( pp );
   }
   rib->arcreclist = NULL;
   rib->arcreclistend = NULL;
   
   return kRIB_OK;
}
