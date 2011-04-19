/* $RCSfile: streamio.c,v $  $Revision: 1.11 $ $Date: 2000/08/20 02:27:30 $
 *
 * Copyright (c) 1995-1999 Thomas E. Burge.  All rights reserved.
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
 *                Copyright (c) 1995-1999 Thomas E. Burge.  
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
 * FILE:  streamio.c
 *
 * DESCRIPTION:  Library that reads both ASCII and binary RIB files.
 *   
 *    Contains:
 *        All basic file I/O and buffering functions.
 *
 *    History:
 *       01-08-98  Added rib->arcreclist and rib->arcreclistend 
 *                 initialization for "comment migration" fix.  
 *                 See comment in paraml.c.
 *       03-28-98  Added RibFreeArcRecPlayBack() to RibClose() to handle
 *                 possible memory leaks that might happen if a RIB is
 *                 suddenly closed before being entirely read-in first.
 *                 Under normal usage no comments would be stored for
 *                 playback.
 *       03-30-98  Added kRIB_OPTION_INIT_RI_FIRST option to handle client
 *                 libraries that require their RiBegin() functions to be
 *                 called to initialize their RI_ values.
 *       04-22-98  Added savedoptions.ncolorsamples to RIB_INSTANCE because
 *                 the number of color samples is an option that is to be
 *                 saved when FrameBegin is encountered and restored at a
 *                 matching FrameEnd.
 *       04-25-98  Added code to set the values rib->lastnuniform, 
 *                 rib->lastnvarying rib->lastnvertex.
 *       04-26-98  Added code to handle matching begin/end blocking done
 *                 in a RIB's subfile.  RiReadArchive() is documented as 
 *                 acting like an inline function, so statements that begin 
 *                 a Frame, World, or Attribute block could -- and with an 
 *                 very odd RIB structure -- have their matching end block 
 *                 statements inside a file read by RiReadArchive().  Note 
 *                 that this matching scheme would cause problems with tools 
 *                 such as catrib and typerib that normally just read a RIB 
 *                 and do not expand named subfiles.
 *       06-09-99  Added support for rib->rilevel.  This is for DSO support.
 *       10-05-99  Added RibFindFile() and archivesearchpath to PRIB_INSTANCE.
 *       10-10-99  Changed a variable to be unsigned char since some compilers
 *                 get confused on this and assign octal value 37777777612
 *                 instead of 212 and 37777777656 instead of octal 256 in
 *                 following testcase:
 *                   %echo 166 145 162 163 151 157 156 212 003 007 256 043 141
 *                   142 040 | oct2bin | typerib
 *                 The above should print "version 3.03" followed by a 
 *                 comment "#ab"
 *       10-17-99  Added rib->hintlevel.
 *       02-24-00  Moved RibClose()'s call to RibEnd() so that RibEnd()
 *                 was not called if the RIB being closed was a ReadArchiveV
 *                 subfile.  [EVES00]
 *       02-28-00  Fixed RibBegin() problem which caused rib->phashtable 
 *                 to be over written in subfiles when using the 
 *                 kRIB_OPTION_INIT_RI_FIRST flag.  The flag is used
 *                 when linking to Pixar's libprman.a or librib.a.  
 *       02-28-00  Updated code to use zib-1.1.3 so that the modified
 *                 zlib could be dropped.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_STREAMIO
#define _RIPRIV_TYPES
#define _RIPRIV_FUNC_TYPES 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ripriv.h>
#include <ribrdr.h>
#if defined(RIB_ZLIB)
#include <unistd.h>
#include <zlib.h>
#endif
/* unix specific */
#ifndef WINDOWS_NT
#include <sys/types.h>
#include <sys/stat.h>
#endif

#define  PARAM_CHECKING 0

/* 
 *   Local Prototypes 
 *
 */
static PRIB_INSTANCE RibOpenWith( char *filename, PRIB_INSTANCE prib, 
                                 PRIB_INSTANCE parent, 
                                 PRIB_HASHTABLE hashtable );
static int RibIncrementPchar( register PRIB_INSTANCE rib );


/* 
 *   Globals
 *
 */
int  gRibMaxFiles      = 10; /* Number of recursive file openings allowed. */
int  gRibNSubfiles     =  0; /* Number of subfiles. */
int  gRibNOpenSubfiles =  0; /* Number of presently opened subfiles. */


/* RibFindFile -- Used to find archive files on a user given searchpath 
 *    option: Option "searchpath" "archive" ["s"].
 * Refer to ribfunc.c's RibReadOptionV() for how archivesearchpath, 
 *    archivenamebuffer archivelmaxpath and archivelnamebuffer are set.
 */
unsigned int RibFindFile( PRIB_INSTANCE prib, char *filename )
{
#ifndef WINDOWS_NT
   struct stat  statbuf;
   size_t       lfilename = 0;
   size_t       lpath = 0;
   auto char    *p, *b;


   if ( !prib || !filename )
      return 0;

   lfilename = strlen( filename ) + 1; 

   /* Make certain pathfilename scratch pad is big enough.
    *    The buffer needs to be big enough for the filename and the longest 
    *    path from the path list.  Also add 1 for a '/' to separate the
    *    path and filename.  Already added another 1 above for the 
    *    terminating '\0'.
    */
   if ( prib->archivelnamebuffer == 0 
	|| prib->archivelnamebuffer - prib->archivelmaxpath < lfilename + 1 )
   {
      if ( prib->archivenamebuffer )
      {
	 _RibFree(prib->archivenamebuffer);
      }
      prib->archivelnamebuffer
	 = prib->archivelmaxpath + lfilename + kRIB_MAXFILELENGTH;
      prib->archivenamebuffer = (char*)_RibMalloc(prib->archivelnamebuffer);
      if ( !prib->archivenamebuffer )
      {
	 prib->archivelnamebuffer = 0;
	 return 0;
      }
   }

   p = prib->archivesearchpath;
   do
   {
      b = prib->archivenamebuffer;
      lpath = 0;
      while (*p!='\0')
      {
	 if (*p==':')
	 {
	    p++;
	    break;
	 }
	 *b++ = *p++;
	 lpath++;
      }
      *b++ = '/';
      memcpy( b, filename, lfilename );

      if ( !stat( prib->archivenamebuffer, &statbuf )
	   && !(statbuf.st_mode & S_IFDIR) )
	 return lfilename + lpath;
   } while (*p!='\0');

   return 0;
#else
   (void)prib;  (void)filename;  

   return 0;
#endif
}


static PRIB_INSTANCE RibOpenWith( char *filename, 
                                 PRIB_INSTANCE prib,
                                 PRIB_INSTANCE parent, 
                                 PRIB_HASHTABLE hashtable )
{
   auto  PRIB_INSTANCE  rib = NULL;
   auto  PRIB_BUFFER    pbuf;
   void  *pv;
#if defined(RIB_ZLIB)
   gzFile  *fp;
#else
   FILE  *fp;
#endif
   size_t  lfilename = 0;
   auto PRIB_INSTANCE  p;
   

   /* Handle Option "searchpath" "archive" ["s"].  The default is ["."]. */
   /* When prib->archivesearchpath==NULL, it means use default ('.'). */
   if (prib && prib->archivesearchpath)
   {
      /* If no prib to base searchpath on, then default to "." which
       *    basically means filename as is.  So only do something if 
       *    prib!=NULL.
       */
      /* Don't bother if searchpath is only the current directory ('.'). */
      if (prib->archivesearchpath[0]!='.' || prib->archivesearchpath[0]!='\n' )
      {
	 lfilename = RibFindFile( prib, filename );

	 /* RibFindFile() will create prib->archivenamebuffer if it didn't
	  *    exist.
	  */
	 if (lfilename)
	    filename = prib->archivenamebuffer;
      }
   }

   if ( filename )
   {
      if ( parent && (gRibNOpenSubfiles == gRibMaxFiles) )
      {
         /* Too many files are open.  Need to temporarily close one. */
         p = parent;
         while ( p->parent && !(p->parent->status & kRIB_STATUS_CLOSED) )
         {
            p = p->parent;
         }
         if ( RibTempClose(p) )
           goto Error;
      }
      
#if defined(RIB_ZLIB)
      fp = gzopen( filename, "rb" );      
#else
      fp = fopen( filename, "rb" );
#endif
      if (!fp)
        goto Error;
      if ( parent )
      {
         gRibNOpenSubfiles++;
         gRibNSubfiles++;
      }
      /* If filename length not set yet, find its length now. */
      if (!lfilename)
	 lfilename = strlen( filename ) + 1; 
   }
   else
   {
#if defined(RIB_ZLIB)
      fp = gzdopen( dup(STDIN_FILENO), "rb" ); 
#else
      fp = stdin;
#endif
      lfilename = 0;
   }
   
   pv = _RibMalloc( lfilename + sizeof(RIB_INSTANCE) );
   if (!pv)
     goto Error;
   rib = (PRIB_INSTANCE)pv;
   
   /* Setup RIB file info. */
   rib->fp = fp;

   /* Clear status flags. */
   rib->status = kRIB_STATUS_CLEAR;

#if defined(RIB_ZLIB)
   /* Assume Affine Z library. */
   /* This is where one would set the kRIB_STATUS_COMPRESSED flag,
    *   but this would still require a nonstandard zlib call and since
    *   gzseek and gztell are now in zlib, this one last feature does
    *   seem worth keeping a modified zlib around anymore.  This feature
    *   which I don't think anyone used is dropped.  02-28-00
    */
   /*
     if (gzIsaGZippedFile(fp))
     {
        rib->status |= kRIB_STATUS_COMPRESSED;
     }   
    */
#endif

   if (filename)
   {
      rib->filename = (char*)((unsigned long int)pv + sizeof(RIB_INSTANCE) );
      memcpy( rib->filename, filename, lfilename );
   }
   else
   {
      rib->filename = "<stdin>";
      rib->status |= kRIB_STATUS_STDIN;
   }

   if (prib)
   {
      /* Get access to buffer. */
      pbuf = prib->pbuf;
      rib->pbuf = pbuf;
      if (!pbuf)
        goto Error;
      
      /* Setup hash table. */
      if (hashtable)
      {
         rib->phashtable = hashtable;
      }
      else
      {
         rib->phashtable = prib->phashtable;
      }
      
      /* Setup default error handler. */
      rib->errorhandler = prib->errorhandler;
      
      /* Setup default archive record handler. */
      rib->arcrechandler = prib->arcrechandler;
      
      /* Setup default hint handler. */
      rib->hintlevel = prib->hintlevel;
      rib->hinttable = prib->hinttable;
      
      /* Mark the new rib struct as using an existing buffer. */
      rib->status |= kRIB_STATUS_USINGSHARED;
      
      /* Give the new rib struct the same options. */
      rib->options = prib->options;
      
      /* Setup table of RI vector functions. */
      rib->rilevel = prib->rilevel;
      rib->ritable = prib->ritable;

      /* Setup default ustep and vstep values for PatchMesh. */
      rib->ustep = prib->ustep;
      rib->vstep = prib->vstep;

      /* RiReadArchive() is documented as acting like an inline function,
       *    so statements that begin a Frame, World, or Attribute block 
       *    could -- and with an very odd RIB structure -- have their
       *    matching end block statements inside a file read by 
       *    RiReadArchive().  Note that this matching scheme would cause
       *    problems with tools such as catrib and typerib that normally
       *    just read a RIB and do not expand named subfiles.
       * The following values will be set by based on the other RIB file
       *    (typically a parent) and then during RibClose() changes made
       *    to them will by carried back to the parent.
       *
       * Share begin/end block information and the matching step and 
       *    color sample states.
       */
      rib->stepstack = prib->stepstack;
      rib->blocklevel = prib->blocklevel;
      rib->status |= (prib->status 
		      & (kRIB_STATUS_FRAMEBLOCK | kRIB_STATUS_WORLDBLOCK));
      rib->ncolorsamples = prib->ncolorsamples; 
      rib->savedoptions.ncolorsamples = prib->savedoptions.ncolorsamples; 

      /* Share cached inline-declarations. */
      rib->inlinelist = prib->inlinelist;

      /* Share vector call scratch pad. */
      rib->vectorcache_n = prib->vectorcache_n;
      rib->vectorcache_nused = prib->vectorcache_nused;
      rib->tokencache = prib->tokencache;
      rib->paramcache = prib->paramcache;
      rib->typecache  = prib->typecache;
      rib->ncache     = prib->ncache;

      /* Setup archive search path and scratchpad. */
      rib->archivelmaxpath = prib->archivelmaxpath;
      rib->archivelnamebuffer = prib->archivelnamebuffer;
      rib->archivenamebuffer = prib->archivenamebuffer;
      rib->archivesearchpath = prib->archivesearchpath;
   }
   else
   {
      /* Setup buffer. */
      pbuf = RibCreateBuffer( rib );
      if (!pbuf)
        goto Error;
      rib->pbuf = pbuf;
      
      /* Give the new rib struct with default options. 
       *    Set options here so, kRIB_OPTION_INIT_RI_FIRST can be 
       *    ORed in.
       */
      rib->options = kRIB_OPTION_ERRMSG_VERBOSE;

      /* Setup hash table. */
      if (hashtable)
      {
         rib->phashtable = hashtable;
      }
      else
      {
         /* Some client libraries do not initialize their RI_ values
          *    until after RiBegin() is called.  To handle this the 
          *    kRIB_OPTION_INIT_RI_FIRST option was added.
	  */
	 if ( rib->options & kRIB_OPTION_INIT_RI_FIRST )
	 {
            rib->phashtable = NULL;
	 }
	 else if ( RI_P == NULL )
         {
	    /* As a safty check assume that the RI_ values will have an 
	     *    initial value of zero if they have not been initialized
	     *    yet.  
	     */
            rib->options |= kRIB_OPTION_INIT_RI_FIRST;
	    rib->phashtable = NULL;
         }
         else
         {
	    /* Create a hash table and access the RI_ strings -- inshort
	     *    assume the RI_ strings such as RI_P are valid pointers
	     *    to NULL terminated strings.
	     */
            rib->phashtable = RibCreateHashTable( kRIB_HASH_TABLESIZE ); 
            if (!rib->phashtable)
               goto Error;
         }
      }
      
      /* Setup default error handler. */
      rib->errorhandler = RibDefaultErrorHandler;
      
      /* Setup default archive record handler. */
      rib->arcrechandler = RibDefaultArchiveRecordHandler;
      
      /* Setup default archive record handler. */
      rib->hintlevel = 0;
      rib->hinttable = NULL;
      
      /* Setup table of RI vector functions. */
      rib->rilevel = 0;
      rib->ritable = NULL;

      /* Setup default ustep and vstep values for PatchMesh. 
       * RiBezierBasis has a step of 3.  Refer to RiBasis().
       */
      rib->ustep = 3; 
      rib->vstep = 3;

      rib->stepstack = NULL;
      rib->blocklevel = 0;

      rib->ncolorsamples = 3; 
      rib->savedoptions.ncolorsamples = 3; 

      rib->inlinelist = NULL;
   
      /* Setup vector call scratch pad. */
      if ( RibVectorCacheInit(rib, 30) )
        goto Error;

      /* Setup archive search path and scratchpad. */
      rib->archivelmaxpath = 0;
      rib->archivelnamebuffer = 0;
      rib->archivenamebuffer = NULL;
      rib->archivesearchpath = NULL;
   }
   rib->linecount     = 1; /* Atleast on the first line. */
   rib->byteoffset    = 0; 
   rib->nvertices     = 0;
   rib->lastsequence  = -1;
   rib->lastnuniform  = 0;
   rib->lastnvarying  = 0;
   rib->lastnvertex   = 0;
   rib->errorcode     = RIE_NOERROR;
   rib->errorseverity = 0;
   rib->error.type    = kRIB_ERRTYPE_NONE;
   rib->arcreclist = NULL;
   rib->arcreclistend = NULL;
#if defined(RIB_ZLIB)
   rib->ungetc = -1;
   rib->fpoffset    = 0; 
#endif
   
   /* Set parent RIB file. */
   p = rib->parent = parent;
   if ( p )
   {
      p->status |= kRIB_STATUS_SUBFILEOPEN;
   }
   
   return rib;

 Error:

   if (rib)
   {
      _RibFree(rib);
      if (rib->tokencache)
        _RibFree(rib->tokencache);
      if (rib->paramcache)
        _RibFree(rib->paramcache);
      if (rib->typecache)
        _RibFree(rib->typecache);
      if (rib->ncache)
        _RibFree(rib->ncache);
   }

   return kRIB_ERRRC_PTR; /* Error */  
}


RIB_HANDLE RibOpen( char *filename, int rilevel, PRIB_RITABLE table )
{
   PRIB_INSTANCE  prib;


   /* Incase the number of supported RI calls is way too low or
    *    maybe mixed up with the version number kRIB_AFFINE_VERSIONINT,
    *    return an error.
    */
   if ( table
	&& (rilevel<kRIB_LAST_RI || rilevel >= 0x3C100000) )
   {
      /* The rilevel value has to be reasonable. */
      return kRIB_ERRRC_PTR;
   }

   /* Use stdin if filename is NULL. */
   prib = RibOpenWith( filename, NULL, NULL, NULL );
   if ( prib && table )
   {
      RibSetRITable( prib, rilevel, table );
   }

   return (RIB_HANDLE)prib;
}


RIB_HANDLE RibOpenSubfile( RIB_HANDLE hrib, char *filename )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   auto PRIB_INSTANCE  prib;
   

   if (rib && filename)
   {
      prib = RibOpenWith( filename, rib, rib, NULL );
      if (prib)
      {
         /* Mark as having been started so RiBegin() isn't called. */
         prib->status |= kRIB_STATUS_BEGUN;
      }

      return (RIB_HANDLE)prib;
   }
   
   return kRIB_ERRRC_PTR; /* Error */
}


RIB_HANDLE RibOpenWithHashTable( char *filename, PRIB_HASHTABLE hashtable )
{
   auto PRIB_INSTANCE  prib;


   /* Use stdin if filename is NULL. */
   if (hashtable)
   {
      prib = RibOpenWith( filename, NULL, NULL, hashtable );

      /* Mark as using an external hash table that shouldn't be
       *    deleted or freed when RIB file is closed.
       */
      prib->status |= kRIB_STATUS_EXTERNHASHTBL;

      return (RIB_HANDLE)prib;
   }
   
   return kRIB_ERRRC_PTR; /* Error */
}


int RibTempClose( PRIB_INSTANCE prib )
{
   register PRIB_INSTANCE  rib = prib;


   
   if ( rib->status & kRIB_STATUS_STDIN )
      return kRIB_OK;

   rib->status |= kRIB_STATUS_CLOSED;
#if defined(RIB_ZLIB)
 {
    auto gzFile  *fp;
    
    /* Assume Affine Z library. */
    fp = rib->fp;

    rib->fpoffset = gzseek(fp, 0L, SEEK_CUR);/*gztell(fp);*/
    if (gzclose( fp ) )
      return kRIB_ERRRC_INT;
 }
#else
   if (fclose( rib->fp ) )
   {
      return kRIB_ERRRC_INT;
   }
   rib->fp = NULL;
#endif
   gRibNOpenSubfiles--;
   
   return kRIB_OK;
}


int RibReopen( PRIB_INSTANCE prib )
{
   register PRIB_INSTANCE  rib = prib;


   if ( gRibNOpenSubfiles <= gRibMaxFiles )
   {
#if defined(RIB_ZLIB)
      gzFile  *fp;

      fp = gzopen( rib->filename, "rb" );
      if ( !fp )
        return kRIB_ERRRC_INT;

      gzseek( fp, (long)rib->fpoffset, SEEK_SET );
      gRibNOpenSubfiles++;
      rib->status &= ~kRIB_STATUS_CLOSED;
      return kRIB_OK;
#else
      rib->fp = fopen( rib->filename, "rb" );
      if ( rib->fp )
      {
         gRibNOpenSubfiles++;
         rib->status &= ~kRIB_STATUS_CLOSED;
         fseek( rib->fp, (long)rib->byteoffset, SEEK_SET );
         if ( feof((FILE*)rib->fp) )
         {
            clearerr((FILE*)rib->fp);
         }

         return kRIB_OK;
      }
#endif
   }

   return kRIB_ERRRC_INT;
}


/* To prevent multiple RiBegin() calls reaching the output 
 *    library hooking out the ritable, use RibBegin() if an output 
 *    file must be specified.  
 *
 *    RibRead() will call RibBegin() and RibClose() will call
 *    a matching RibEnd().  A flag is set that prevents multiple 
 *    calls reaching the output library.  
 *    If you call RibBegin() a matching RibEnd() call is optional.
 */
int RibBegin( RIB_HANDLE hrib, RtToken name )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (!rib)
     return kRIB_ERRRC_INT;

   if ( rib->ritable 
       && !(rib->status & kRIB_STATUS_BEGUN) 
       && rib->ritable[kRIB_BEGIN] )
   {
      ((PRiBegin)*rib->ritable[kRIB_BEGIN])( name );
      rib->status |= kRIB_STATUS_BEGUN;
   }

   /* Some client libraries do not initialize their RI_ values
    *    until after RiBegin() is called.  To handle this the 
    *    kRIB_OPTION_INIT_RI_FIRST option was added.
    */
   if ( rib->options & kRIB_OPTION_INIT_RI_FIRST && !rib->phashtable )
   {
      rib->phashtable = RibCreateHashTable( kRIB_HASH_TABLESIZE ); 
      if (!rib->phashtable)
         return kRIB_ERRRC_INT;
   }

   return kRIB_OK;
}


int RibEnd( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (!rib)
     return kRIB_ERRRC_INT;

   if ( rib->ritable 
       && !(rib->status & kRIB_STATUS_ENDED)
       && rib->ritable[kRIB_END] )
   {
      ((PRiEnd)*rib->ritable[kRIB_END])();
      rib->status |= kRIB_STATUS_ENDED;
   }

   return kRIB_OK;
}


int RibOption( RIB_HANDLE hrib, RtToken name, RtToken token, RtPointer param )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   RtPointer  rtstr;

   if (!rib)
     return kRIB_ERRRC_INT;

   if ( rib->ritable 
       && rib->ritable[kRIB_OPTION] )
   {
      rtstr = (RtPointer)&param;
      ((PRiOptionV)*rib->ritable[kRIB_OPTION])( name, 1, &token, &rtstr );
   }

   return kRIB_OK;
}


int RibSetStatus( RIB_HANDLE hrib, RIB_UINT32 mask, RIB_UINT32 flags )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (rib)
   {
      rib->status = (rib->status & ~mask) | (flags & mask);
      return kRIB_OK;
   }
   
   return kRIB_ERRRC_INT; /* Error */
}


RIB_UINT32 RibGetStatus( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (rib)
   {
      return rib->status;
   }
   
   return 0;
}


int RibSetOptions( RIB_HANDLE hrib, RIB_UINT32 mask, RIB_UINT32 flags )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (rib)
   {
      rib->options = (rib->options & ~mask) | (flags & mask);
   }
   
   return kRIB_ERRRC_INT; /* Error */
}


RIB_UINT32 RibGetOptions( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (rib)
   {
      return rib->options;
   }
   
   return 0;
}


int RibSetUVStep( RIB_HANDLE hrib, RtInt ustep, RtInt vstep )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (rib)
   {
      rib->ustep = ustep;
      rib->vstep = vstep;
      return kRIB_OK;
   }
   
   return kRIB_ERRRC_INT;
}


int RibGetUVStep( RIB_HANDLE hrib, RtInt *ustep, RtInt *vstep )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (rib)
   {
      if (ustep)
      {
         *ustep = rib->ustep;
      }
      if (vstep)
      {
         *vstep = rib->vstep;
      }
      return kRIB_OK;
   }
   
   return kRIB_ERRRC_INT;
}


int RibSetMaximumFiles( RIB_HANDLE rib, int n )
{
   if ( rib && n < 0 && n > FOPEN_MAX )
     return kRIB_ERRRC_INT;
   
   if ( gRibNOpenSubfiles < n )
   {
      gRibMaxFiles = n;
   }
   else if ( gRibMaxFiles != n )
   {
      /* Can't go below the number of already opened subfiles. */
      return kRIB_ERRRC_INT;
   }
   
   return kRIB_OK;
}


int RibIncrementPchar( register PRIB_INSTANCE rib )
{
   register PRIB_BUFFER      pbuf;
   register PRIB_BUFFERITEM  p;
   register char *s;


   if ( !(rib && (pbuf=rib->pbuf)) )
     return kRIB_ERRRC_INT;
   
   pbuf=rib->pbuf;
   s = ++(pbuf->pchar);

   /* If p is one char beyond the array, set it to the beginning
    *   of the next buffer item structure.
    */
   if (s >= &(pbuf->pbufhere->b[kRIB_BUFFER_MAX_B_SIZE]))
   {
      p = pbuf->pbufhere;
      if (p->next)
      {
         pbuf->pbufhere = p->next;
      }
      else
      {
         p = RibGetBufferItem(pbuf);
         if (!p)
           return kRIB_ERRRC_INT; /* Error */
         p->next = NULL;
         p->prev = pbuf->pbufhere;
         pbuf->pbufhere->next = p;
         pbuf->pbufhere = p;
      }
      pbuf->pchar = &(pbuf->pbufhere->b[0]);
   }

   return kRIB_OK;
}


int RibGetChar( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   register PRIB_BUFFER    pbuf;
   int  c;


   if ( rib )
   {
#if defined(RIB_ZLIB)
      if ( rib->ungetc < 0 )
      {
         unsigned char  b;

         c = gzread( (gzFile)rib->fp, &b, 1 );
         if (c!=1)
         {
            if (c==0)
              return EOF;
            else
              return kRIB_ERRRC_STREAMIO; /* Error */
         }
         c = b;
      }
      else
      {
         c = rib->ungetc;
         rib->ungetc = -1;
      }
#else
      c = fgetc( (FILE*)rib->fp );
      if (c==EOF)
	 return EOF;	 
#endif

      if ( (rib->status & kRIB_STATUS_SAVETOBUFFER)
          && (pbuf=rib->pbuf)!=NULL )
      {
         *(pbuf->pchar) = c;
         pbuf->buflength++;
         if ( RibIncrementPchar(rib) )
           return kRIB_ERRRC_STREAMIO; /* Error */
      }
      rib->byteoffset++;
      if ( '\n'==c )
      {
         rib->linecount++;
      }

      return c;
   }

   return kRIB_ERRRC_STREAMIO; /* Error */
}


int RibPutChar( RIB_HANDLE hrib, int c )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   register PRIB_BUFFER    pbuf;


   if ( rib && (pbuf=rib->pbuf)!=NULL )
   {
      *(pbuf->pchar) = c;
      pbuf->buflength++;
      if (RibIncrementPchar( rib ))
         return kRIB_ERRRC_STREAMIO; /* Error */
      return c;
   }
   
   return kRIB_ERRRC_STREAMIO; /* Error */
}


int RibUngetChar( RIB_HANDLE hrib, int c )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   register PRIB_BUFFER      pbuf;
   register PRIB_BUFFERITEM  p;


   if ( rib )
   {
      if ( (rib->status & kRIB_STATUS_SAVETOBUFFER) 
          && (pbuf=rib->pbuf)!=NULL )
      {
         /* Check if the rib->pbuf->pchar isn't at the very beginning 
          *    of a RIB_BUFFER struct, because decrementing the
          *    pointer could take the pointer into la-la land.
          */
         p = pbuf->pbufhere;
         if ( pbuf->pchar != p->b )
         {
            pbuf->buflength--;
            pbuf->pchar--;
         }
         else if ( p->prev )
         {
            pbuf->buflength--;
            pbuf->pchar = &(p->prev->b[kRIB_BUFFER_MAX_B_SIZE - 1]);
            pbuf->pbufhere = p->prev;
         }
         else
         {
            return kRIB_ERRRC_INT; /* Error */
         }
      }

#if defined(RIB_ZLIB)
      rib->ungetc = c;
#else
      ungetc( c, (FILE*)rib->fp );
#endif

      rib->byteoffset--;
      if ( '\n'==c )
      {
         rib->linecount--;
      }
      return kRIB_OK;
   }

   return kRIB_ERRRC_INT; /* Error */
}


int RibBufferRead( RIB_HANDLE hrib, char *buffer, unsigned int size )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   auto int            n;


   if ( rib && rib->pbuf!=NULL )
   {
#if defined(RIB_ZLIB)
      n = gzread( (gzFile)rib->fp, buffer, size );
      if (n<0)
        return kRIB_ERRRC_STREAMIO; /* Error */
#else
      n = fread( buffer, 1, size, (FILE*)rib->fp );
#endif
      rib->byteoffset += n;

      /* Returns the number of bytes read. */
      return n;
   }

   return kRIB_ERRRC_INT; /* Error */
}


int RibIgnoreLastChar( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE    rib = (PRIB_INSTANCE)hrib;
   register PRIB_BUFFER      pbuf;
   register PRIB_BUFFERITEM  p;


   if ( rib 
       && (pbuf=rib->pbuf)!=NULL 
       && (rib->status&kRIB_STATUS_SAVETOBUFFER) )
   {
      /* Check if the rib->pbuf->pchar isn't at the very beginning 
       *    of a RIB_BUFFER struct, because decrementing the
       *    pointer could take the pointer into la-la land.
       */
      p = pbuf->pbufhere;
      if ( pbuf->pchar != p->b )
      {
         pbuf->buflength--;
         pbuf->pchar--;
      }
      else if ( p->prev )
      {
         pbuf->buflength--;
         pbuf->pchar = &(p->prev->b[kRIB_BUFFER_MAX_B_SIZE - 1]);
         pbuf->pbufhere = p->prev;
      }

      return kRIB_OK;
   }

   return kRIB_ERRRC_INT; /* Error */
}


int RibGetLastChar( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE    rib = (PRIB_INSTANCE)hrib;
   register PRIB_BUFFER      pbuf;
   register PRIB_BUFFERITEM  p;


   if ( rib 
       && (pbuf=rib->pbuf)!=NULL 
       && !(rib->status&kRIB_STATUS_SAVETOBUFFER) )
   {
      /* Check if the rib->pbuf->pchar isn't at the very beginning 
       *    of a RIB_BUFFER struct, because decrementing the
       *    pointer could take the pointer into la-la land.
       */
      p = pbuf->pbufhere;
      if ( pbuf->pchar != p->b )
      {
         return *(pbuf->pchar - 1);
      }
      else if ( p->prev )
      {
         return (p->prev->b[kRIB_BUFFER_MAX_B_SIZE - 1]);
      }
   }

   return kRIB_ERRRC_STREAMIO; /* Error */
}


char *RibCreateStringFromBuffer( RIB_HANDLE hrib, RIB_UINT32 options )
{
   auto PRIB_INSTANCE        rib = (PRIB_INSTANCE)hrib;
   auto PRIB_BUFFER          pbuf;
   auto PRIB_BUFFERITEM      p;
   char                      *s;
   RIB_RTSTRING              *rts;
   register char             *ss;
   register size_t           l;


   if ( !(rib && rib->pbuf ) )
     return kRIB_ERRRC_PTR; /* Error */

   pbuf = rib->pbuf;
   p = pbuf->plist;
   l = pbuf->buflength+1; /* Add 1 for a null that terminates the string. */

   if ( options & kRIB_STRING_RTSTRING )
   {
      rts = (RIB_RTSTRING*)_RibMalloc( sizeof(RtString) + l );
      if ( !rts )
        return kRIB_ERRRC_PTR;
      rts->pstr = rts->str;
      s = (char*)rts->str;
   }
   else
   {
      s = (char*)_RibMalloc( l );
      if ( !s )
        return kRIB_ERRRC_PTR;
      rts = (RIB_RTSTRING*)s;
   }

   l--; /* Don't count the final '\0' that is appended to the string. */
   if (s)
   {
      ss = s;
      while (p)
      {
         /* If an item follows on the list (p->next is nonzero), p isn't 
          *   the last one and a simple memcpy of a full buffer size
          *   is done. 
          */
         if (p->next)
         {
            memcpy( ss, p->b, sizeof(char)*kRIB_BUFFER_MAX_B_SIZE );
            l  -= kRIB_BUFFER_MAX_B_SIZE;
            ss += kRIB_BUFFER_MAX_B_SIZE;
         }
         /* Last item may not be a full buffer and requires a memcpy of
          *   a smaller size.
          */
         else 
         {
            memcpy( ss, p->b, l );
            ss += l;
         }
         p = p->next;
      }
      *ss = '\0';
   }

   return (char*)rts;
}


int RibCreateStringsFromBuffer( RIB_HANDLE hrib, RtInt n, RtString **r )
{
   auto PRIB_INSTANCE    rib = (PRIB_INSTANCE)hrib;
   auto PRIB_BUFFER      pbuf;
   auto PRIB_BUFFERITEM  p;
   RtString              *ar;
   int                   i;
   register char         *s;
   register size_t       l;

   
   if ( !(rib && rib->pbuf && r) )
     return kRIB_ERRRC_PTR; /* Error */

   pbuf = rib->pbuf;
   p = pbuf->plist;
   l = pbuf->buflength; 

   /* NULLs were already added into the buffer to terminate each string. */
   ar = (RtString*)_RibMalloc( sizeof(RtString)*n + l );
   if ( !ar )
     return kRIB_ERRRC_PTR;

   /* Block of memory ar has n char*'s followed by n NULL terminated 
    *    strings.  Use ar to access the array of RtString's (the char*'s)
    *    and use s to access the buffer of characters making-up the n NULL
    *    terminated strings.  The RtString's in ar[0...n-1] point into the 
    *    buffer s.
    */

   /* Have s and ss start at the char after ar[n-1]. */
   s = (char*)&ar[n];

   /* OpenStep seems to be missing memccpy().  So I'll first copy
    *   the buffer into one block of memory and then setup the string 
    *   pointers.
    *
    * To-Do: If numberof is actually less then the number of strings that
    *        that the buffers l number of characters represents, the
    *        the extra number of characters are also copied and waste
    *        space.  I'll get back to this later.  For now this code doesn't
    *        cause any errors with typerib tests.
    */
   while (p)
   {
      /* If an item follows on the list (p->next is nonzero), p isn't 
       *   the last one and a simple memcpy of a full buffer size
       *   is done. 
       */
      if (p->next)
      {
         memcpy( s, p->b, kRIB_BUFFER_MAX_B_SIZE );
         l -= kRIB_BUFFER_MAX_B_SIZE;
         s += kRIB_BUFFER_MAX_B_SIZE;
      }
      /* Last item may not be a full buffer and requires a memcpy of
       *   a smaller size.
       */
      else 
      {
         memcpy( s, p->b, l );
         s += l;
      }
      p = p->next;
   }

   /* Setup string pointers. */
   i = 0;
   s = (char*)&ar[n];
   do
   {
      ar[i] = s;
      while (*s)
      {
         s++;
      }
      s++; /* Skip NULL. */
      i++;
   } while ( i < n );

   /* Already checked above that r is not NULL. */
   *r = &ar[0];

   return kRIB_OK;
}


RtInt *RibCreateIntegerArrayFromBuffer( RIB_HANDLE hrib, RIB_UINT32 options )
{
   auto PRIB_INSTANCE     rib = (PRIB_INSTANCE)hrib;
   auto PRIB_BUFFER       pbuf;
   auto PRIB_BUFFERITEM   p;
   char                   *s;
   register char          *ss;
   register size_t        l;

   (void)options;

   if ( !(rib && rib->pbuf) )
     return kRIB_ERRRC_PTR; /* Error */

   pbuf = rib->pbuf;
   p = pbuf->plist;
   l = pbuf->buflength;

   s = (char*)_RibMalloc( l );
   if ( !s )
     return kRIB_ERRRC_PTR;

   if (s)
   {
      ss = s;
      while (p)
      {
         if (p->next)
         {
            memcpy( ss, p->b, sizeof(char)*kRIB_BUFFER_MAX_B_SIZE );
            l  -= kRIB_BUFFER_MAX_B_SIZE;
            ss += kRIB_BUFFER_MAX_B_SIZE;
         }
         else
         {
            memcpy( ss, p->b, l );
            ss += l;
         }
         p = p->next;
      }
   }

   return (RtInt*)s;
}


RtFloat *RibCreateFloatArrayFromBuffer( RIB_HANDLE hrib, RIB_UINT32 options )
{
   auto PRIB_INSTANCE     rib = (PRIB_INSTANCE)hrib;
   auto PRIB_BUFFER       pbuf;
   auto PRIB_BUFFERITEM   p;
   char                   *s;
   register char          *ss;
   register size_t        l;

   (void)options;

   if ( !(rib && rib->pbuf) )
     return kRIB_ERRRC_PTR; /* Error */

   pbuf = rib->pbuf;
   p = pbuf->plist;
   l = pbuf->buflength;

   s = (char*)_RibMalloc( l );
   if ( !s )
     return kRIB_ERRRC_PTR;

   if (s)
   {
      ss = s;
      while (p)
      {
         if (p->next)
         {
            memcpy( ss, p->b, sizeof(char)*kRIB_BUFFER_MAX_B_SIZE );
            l  -= kRIB_BUFFER_MAX_B_SIZE;
            ss += kRIB_BUFFER_MAX_B_SIZE;
         }
         else
         {
            memcpy( ss, p->b, l );
            ss += l;
         }
         p = p->next;
      }
   }

   return (RtFloat*)s;
}


int RibClose( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   register PRIB_INSTANCE  p;
   int  rc = kRIB_OK;

   
   if ( rib && !(rib->status&kRIB_STATUS_SUBFILEOPEN))
   {
      /* Clean out any stored comments and hints that were about
       *    to be played back when RibClose() was suddenly called.
       */
      RibFreeArcRecPlayBack( (RIB_HANDLE)rib );

      /* Don't destroy the stepstack, buffer, hash table, inline 
       *    declarations list and caches if they are in use by 
       *    another RIB_INSTANCE structure. 
       */
      if ( !(rib->status & kRIB_STATUS_USINGSHARED) )
      {
	 /* Call any RiEnd() functions hooked into the Ri Table. */
	 RibEnd( (RIB_HANDLE)rib );

	 /* Clean out any stored RIB_UVSTEP structures. */
	 if ( RibFreeStepStack( (RIB_HANDLE)rib ) )
         {
            rc = kRIB_ERRRC_INT; /* Error */
         }
	 if ( RibFreeInLineList( (RIB_HANDLE)rib ) )
         {
            rc = kRIB_ERRRC_INT; /* Error */
         }
         if ( rib->tokencache )
         {
            #ifdef RIB_MEMORY_TESTS
            RibMemoryTestStepOverAllowed();
            #endif
            _RibFree( rib->tokencache );
         }
         if ( rib->paramcache )
         {
            #ifdef RIB_MEMORY_TESTS
            RibMemoryTestStepOverAllowed();
            #endif
            _RibFree( rib->paramcache );
         }
         if ( rib->typecache )
         {
            #ifdef RIB_MEMORY_TESTS
            RibMemoryTestStepOverAllowed();
            #endif
            _RibFree( rib->typecache );
         }
         if ( rib->ncache )
         {
            #ifdef RIB_MEMORY_TESTS
            RibMemoryTestStepOverAllowed();
            #endif
            _RibFree( rib->ncache );
         }

	 /* Destroy archive search path and scratchpad. */
	 if ( rib->archivenamebuffer )
	 {
            #ifdef RIB_MEMORY_TESTS
            RibMemoryTestStepOverAllowed();
            #endif
            _RibFree( rib->archivenamebuffer );
	 }
	 if ( rib->archivesearchpath )
	 {
            #ifdef RIB_MEMORY_TESTS
            RibMemoryTestStepOverAllowed();
            #endif
            _RibFree( rib->archivesearchpath );
	 }

         if ( RibDestroyBuffer(rib->pbuf) )
         {
            rc = kRIB_ERRRC_INT; /* Error */
         }
         /* Don't destroy hash table if it was created by someone else. */
         if ( !(rib->status & kRIB_STATUS_EXTERNHASHTBL) )
         {
            if ( RibDestroyHashTable((RIB_HASHHND)(rib->phashtable)) )
            {
               rc = kRIB_ERRRC_INT; /* Error */
            }
         }
#ifdef LATER
	 if ( !(rib->status & kRIB_STATUS_EXTERNRITBL) )
	 {
	    if (  rib->ritable )
	    {
	       _RibFree( rib->ritable );
	    }
	    else
	    {
               rc = kRIB_ERRRC_INT; /* Error */
	    }
	 }
#endif
      }
      else if ( rib->parent )
      {
	 /* RiReadArchive() is documented as acting like an inline function,
	  *    so statements that begin a Frame, World, or Attribute block 
	  *    could -- and with an very odd RIB structure -- have their
	  *    matching end block statements inside a file read by 
	  *    RiReadArchive().  Note that this matching scheme would cause
	  *    problems with tools such as catrib and typerib that normally
	  *    just read a RIB and do not expand named subfiles.
	  *
	  * Carry changes made by the child RIB back to the parent.
	  */
	 p = rib->parent;	 

	 /* Copy uv step info and Attribute blocking. */
	 p->blocklevel = rib->blocklevel;
	 p->stepstack = rib->stepstack;
	 p->inlinelist = rib->inlinelist;

	 /* Clear the two bits for Frame and World blocking. */
	 p->status &= ~(kRIB_STATUS_FRAMEBLOCK | kRIB_STATUS_WORLDBLOCK);

	 /* Copy the childs status for Frame and World blocking to parent. */
	 p->status |= (rib->status 
		       & (kRIB_STATUS_FRAMEBLOCK | kRIB_STATUS_WORLDBLOCK));

	 p->ncolorsamples = rib->ncolorsamples; 
	 p->savedoptions.ncolorsamples = rib->savedoptions.ncolorsamples; 
      }

#if defined(RIB_ZLIB)
      if ( gzclose( (gzFile)rib->fp ) )
        rc = kRIB_ERRRC_INT; /* Error */
#else
      if ( !(rib->status & kRIB_STATUS_STDIN) )
      {
         if ( fclose( rib->fp) )
         {
            rc = kRIB_ERRRC_INT; /* Error */
         }
      }
#endif
      if ( rib->parent )
      {
         gRibNSubfiles--;
         if ( gRibNOpenSubfiles <= gRibMaxFiles )
         {
            gRibNOpenSubfiles--;
         }
         p = rib->parent;
         while ( p && !(p->status & kRIB_STATUS_CLOSED) )
         {
            p = p->parent;
         }
         if ( p && p->status & kRIB_STATUS_CLOSED )
         {
            if ( RibReopen( p ) )
            {
               rc = kRIB_ERRRC_INT; /* Error */
            }
         }
         rib->parent->status &= ~kRIB_STATUS_SUBFILEOPEN;
      }
      _RibFree( rib );

      return rc;
   }
   
   return kRIB_ERRRC_INT; /* Error */
}


/* Close the RIB file, but don't destroy any of the RIB structures
 *    and don't invalidate the RIB handle by freeing it.  
 */
int RibCloseFile( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   
   if ( rib 
       && !(rib->status & kRIB_STATUS_STDIN)
       && !(rib->status&kRIB_STATUS_SUBFILEOPEN) 
       && !fclose((FILE*)rib->fp) )
   {
      rib->fp = NULL;
      if ( rib->parent )
      {
         gRibNSubfiles--;
         if ( gRibNOpenSubfiles <= gRibMaxFiles )
         {
            gRibNOpenSubfiles--;
         }
         if ( rib->parent->status & kRIB_STATUS_CLOSED )
         {
            if ( RibReopen(rib->parent) )
              return kRIB_ERRRC_INT; /* Error */
         }
         rib->parent->status &= ~kRIB_STATUS_SUBFILEOPEN;
      }

      return kRIB_OK;
   }
   
   return kRIB_ERRRC_INT; /* Error */
}


PRIB_BUFFERITEM RibGetBufferItem( PRIB_BUFFER pbuffer )
{
   register PRIB_BUFFER      pbuf = pbuffer;
   register PRIB_BUFFERITEM  p;

   if (pbuf)
   {
      if (pbuf->pbufavail)
      {
         p = pbuf->pbufavail;
         pbuf->pbufavail = p->next;
      }
      else
      {
         p = (PRIB_BUFFERITEM)_RibMalloc( sizeof(RIB_BUFFERITEM) );
      }
      
      if (p)
      {
         p->next = NULL;
         p->prev = NULL;
      }
      
      return p;
   }
   
   return kRIB_ERRRC_PTR; /* Error */
}


PRIB_BUFFER RibCreateBuffer( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE    rib = (PRIB_INSTANCE)hrib;
   register PRIB_BUFFER      pbuf;
   register PRIB_BUFFERITEM  plist;

   if (rib)
   {
      pbuf = (PRIB_BUFFER)_RibMalloc( sizeof(RIB_BUFFER) );
      pbuf->pbufavail = NULL;
      pbuf->pbufhere  = NULL;
      pbuf->plist     = NULL;
      pbuf->pchar     = NULL;
      if (!pbuf)
        return kRIB_ERRRC_PTR; /* Error */
      plist = RibGetBufferItem( pbuf );
      if (!plist)
      {
         _RibFree (pbuf);
         return NULL;
      }
      pbuf->pbufhere  = plist;
      pbuf->plist     = plist;
      pbuf->pchar     = plist->b;

      plist->next = NULL;
      plist->prev = NULL;

      return pbuf;
   }
   
   return kRIB_ERRRC_PTR; /* Error */
}


int RibDestroyBuffer( PRIB_BUFFER pbuf )
{
   register PRIB_BUFFERITEM  p,pp;


   if (pbuf)
   {
      p = pbuf->plist;
      while (p)
      {
         pp = p->next;
#ifdef RIB_MEMORY_TESTS
         RibMemoryTestStepOverAllowed();
#endif
         _RibFree(p);

         p = pp;
      }

      p = pbuf->pbufavail;
      while (p)
      {
         pp = p->next;

#ifdef RIB_MEMORY_TESTS
         RibMemoryTestStepOverAllowed();
#endif
         _RibFree(p);

         p = pp;
      }

#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
      _RibFree(pbuf);

      return kRIB_OK;
   }

   return kRIB_ERRRC_INT; /* Error */
}


int RibResetBuffer( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE    rib = (PRIB_INSTANCE)hrib;
   register PRIB_BUFFER      pbuf;
   register PRIB_BUFFERITEM  p,pp;


   if ( !(rib && (pbuf=rib->pbuf)) )
     return kRIB_ERRRC_INT; /* Error */

   p = pbuf->plist;
   if (p)
   {
      /* Move extra buf structures over to the "available" list. */
      pp = p->next;
      if (pp)
      {
         while (pp->next)
         {
            pp = pp->next;
         }
         pp->next = pbuf->pbufavail;
         pbuf->pbufavail = p->next;
         p->next = NULL;
      }
      pbuf->pbufhere = p;
      pbuf->pchar = &(p->b[0]);
      *(pbuf->pchar) = '\0';
      pbuf->buflength = 0;

      return kRIB_OK;
   }

   return kRIB_ERRRC_INT; /* Error */   
}


int RibSaveToBuffer( RIB_HANDLE hrib, int flag )
{
   register PRIB_INSTANCE    rib = (PRIB_INSTANCE)hrib;


   if ( RibResetBuffer( (RIB_HANDLE)rib ) )
     return kRIB_ERRRC_INT;

   if ( flag )
   {
      rib->status |= kRIB_STATUS_SAVETOBUFFER;
   }
   else
   {
      rib->status &= ~kRIB_STATUS_SAVETOBUFFER;
   }

   return kRIB_OK;
}
