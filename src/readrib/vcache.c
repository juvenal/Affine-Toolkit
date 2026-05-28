/* $RCSfile: vcache.c,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:07 $
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
 * FILE:  vcache.c
 *
 *        Refer to affine/ribhash/hash.c for RibMalloc(), RibRealloc() and
 *        RibFree().  Macros _RibMalloc, _RibRealloc and _RibFree are defined
 *        in affine/include/ribhash.c.
 *
 * DESCRIPTION:  Library that reads both ASCII and binary RIB files.
 *   
 *    Contains:
 * 
 *     History:
 *      03-28-98  Added a comment in RibFreeVectorCache() about handling
 *                syntax errors that might have occured in a while
 *                reading a parameter list.
 * 
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_VCACHE
#define  _RIPRIV_TYPES
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ripriv.h>
#include <ribrdr.h>


int RibVectorCacheInit( RIB_HANDLE hrib, RtInt n )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if ((!rib) || (n<1))
     return kRIB_ERRRC_INT;

   rib->vectorcache_n = 0;
   rib->vectorcache_nused = 0;
   rib->tokencache = NULL;
   rib->paramcache = NULL;
   rib->typecache  = NULL;      
   rib->ncache     = NULL;
   /* Use RibCacheVector to set up an intial buffer for a vector
    *   form of the parameter list.  The function was intended to 
    *   be called to grow the buffer if needed and otherwise simply
    *   add to the number of items used.  
    */
   if ( RibCacheVector(rib, n, NULL, NULL) )
     return kRIB_ERRRC_INT;

   /* Reset the number of used items to zero. */
   rib->vectorcache_nused = 0;
   
   return kRIB_OK;
}


int RibCacheVector( RIB_HANDLE hrib, 
                   RtInt tn, RtToken **tokens, RtPointer **params )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   size_t   size;
   size_t   n = tn;


   if (!rib)
     return kRIB_ERRRC_INT;

   if (tn==0)
   {
      rib->vectorcache_nused = 0;
   }
   else if (tn<0)
   {
      if (rib->vectorcache_nused >= tn)
      {
         rib->vectorcache_nused += tn;
      }
      else
      {
         return kRIB_ERRRC_INT;
      }
   }
   else
   {
      size = rib->vectorcache_n - rib->vectorcache_nused;
      rib->vectorcache_nused += tn;

      if ( size < n )
      {
         n -= size;
         size = n%kRIB_VECTORCACHEINCR;

         /* Round n up to the nearest multiple of kRIB_VECTORCACHEINCR. */
         if (size)
           n = (n-size) + kRIB_VECTORCACHEINCR;
         rib->vectorcache_n += n;

         size = n * sizeof(void*);
         rib->tokencache = _RibRealloc( rib->tokencache, size );
         if ( !rib->tokencache )
           goto Error;
         rib->paramcache = _RibRealloc( rib->paramcache, size );
         if ( !rib->paramcache )
           goto Error;
         
         size = n * sizeof(RIB_UINT32);
         rib->typecache = _RibRealloc( rib->typecache, size );

         if ( !rib->typecache )
           goto Error;
         rib->ncache = _RibRealloc( rib->ncache, size );

         if ( !rib->ncache )
           goto Error;
      }
   }

   if (tokens)
     *tokens = (RtToken*)rib->tokencache;
   if (params)
     *params = (RtPointer*)rib->paramcache;
   

   return kRIB_OK;         

 Error:
   rib->vectorcache_nused = 0;
   rib->vectorcache_n = 0;
   if ( rib->tokencache )
     _RibFree( rib->tokencache );
   if ( rib->paramcache )
     _RibFree( rib->paramcache );
   if ( rib->typecache )
     _RibFree( rib->typecache );
   if ( rib->ncache )
     _RibFree( rib->ncache );
   if (tokens)
     *tokens = NULL;
   if (params)
     *params = NULL;

   return kRIB_ERRRC_INT;
}


int RibCopyVectorCache( RIB_HANDLE hrib, 
                   RtInt nparams, RtToken **tokens, RtPointer **params )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   size_t              tsize; /* Total size to allocate. */
   unsigned int  nsize; /* N items to copy. */
        auto RtToken *t = NULL;
        auto RtPointer *p = NULL;


   if (!rib)
     return kRIB_ERRRC_INT;

   if ( nparams > rib->vectorcache_nused )
   {
      nsize = rib->vectorcache_nused;
   }
   else
   {
      nsize = nparams;
   }
   
   if ( nsize > 0 )
   {
      if (tokens)
      {
         tsize = nsize * sizeof(RtToken);
         t = (RtToken*)_RibMalloc( tsize );
         if ( !t )
           goto Error;
         memcpy( t, rib->tokencache, tsize );
         *tokens = t;
      }
      if (params)
      {
         tsize = nsize * sizeof(RtPointer);
         p = (RtPointer*)_RibMalloc( tsize );
         if ( !p )
           goto Error;
         memcpy( p, rib->paramcache, tsize );
         *params = p;
      }
   }
   
   return kRIB_OK;         
   
 Error:
   if ( t )
     _RibFree( t );
   if ( p )
     _RibFree( p );

   return kRIB_ERRRC_INT;
}


int RibFreeVectorCache( RIB_HANDLE hrib )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (rib)
   {
      if (rib->tokencache)
        _RibFree(rib->tokencache);
      if (rib->paramcache)
        _RibFree(rib->paramcache);
      if (rib->typecache)
        _RibFree(rib->typecache);
      if (rib->ncache)
        _RibFree(rib->ncache);
      rib->vectorcache_nused = 0;
      rib->vectorcache_n = 0;
   }
   
   return kRIB_OK;
}


int RibFreeVectorData( RIB_HANDLE hrib, 
                      RtInt nparms, RtToken *tokens, RtPointer *parms )
{
   register PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   auto int         n;
   auto PRtToken    pTokens;
   auto PRtPointer  pParams;


   if (rib)
   {
      if ( nparms && tokens && parms )
      {
         n = nparms;
         pTokens = tokens;
         pParams = parms;         
      }
      else
      {
         n = rib->vectorcache_nused;
         pTokens = (PRtToken)rib->tokencache;
         pParams = (PRtPointer)rib->paramcache;
      }
      while ( n-- )
      {
#ifdef RIB_MEMORY_TESTS
         RibMemoryTestStepOverAllowed();
#endif
         _RibFree(*pTokens++);
#ifdef RIB_MEMORY_TESTS
         RibMemoryTestStepOverAllowed();
#endif
         /* 03-28-98:
          * Sometimes an error occurs and tokens[] is given a valid pointer
          *    to a string, but parms was assigned a NULL pointer because
          *    no memory was allocated for the bad data.
          * The code that is adding data to the vector cache needs to
          *    handle removing incomplete data.  If incomplete data is
          *    present the following _RibFree() could cause a trap with some
          *    compilers.  If it doesn't trap and if RIB_MEMORY_TESTS is
          *    defined, the following call will be listed in the 
          *    "Invalid _RibFree() calls" list printed by RibSetStatus().
          * A program such as typerib has to call RibSetStatus() to get
          *    the status report on memory calls and the libraries need to
          *    have been compiled with RIB_MEMORY_TESTS defined.
          * Basically if the following call is an invalid call to free(),
          *    fix the code that setup the vector cache data.  Odds are this
          *    code does not need to be changed.
          */
         _RibFree(*pParams++);
      }
      rib->vectorcache_nused = 0;
      
      /* Remove in-line parameter declarations from hash table. */
      RibUndoInLineList( rib );
   }
   
   return kRIB_OK;
}


int RibGetVectorCache( RIB_HANDLE hrib, 
                   RtInt *n, RtToken **tokens, RtPointer **params )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (!rib)
     return kRIB_ERRRC_INT;

   if (n)
   {
      *n = rib->vectorcache_nused;
   }
   if ( tokens )
   {
      *tokens = (RtToken*)rib->tokencache;
   }
   if ( params )
   {
      *params = (RtPointer*)rib->paramcache;
   }

   return kRIB_OK;
}


int RibGetVectorCacheInfo( RIB_HANDLE hrib, RtInt *n, RtInt *nvertices,
                          unsigned int **types, unsigned int **sizes )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;


   if (!rib)
     return kRIB_ERRRC_INT;

   if (n)
   {
      *n = rib->vectorcache_nused;
   }
   if (nvertices)
   {
      *nvertices = rib->nvertices;
   }
   if ( types )
   {
      *types = (unsigned int *)rib->typecache;
   }
   if ( sizes )
   {
      *sizes = (unsigned int *)rib->ncache;
   }

   return kRIB_OK;
}
