/* $RCSfile: hash.c,v $  $Revision: 1.9 $ $Date: 2000/02/28 08:10:14 $
 *
 * Copyright (c) 1995, 1996, 1997, 1998, 1999, 2000 Thomas E. Burge.  
 *                   All rights reserved.
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
 *     Copyright (c) 1995, 1996, 1997, 1998, 1999, 2000 Thomas E. Burge.
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
 * FILE:  hash.c
 *
 * DESCRIPTION:  Hash functions.
 *   
 *    Contains:
 * 
 *    References:
 *          [GRITZ95] Gritz, Larry I., Blue Moon Rendering Tools: User 
 *                    Guide Version 2.3, Silver Spring, MD, Febuary 27,
 *                    1995.
 *          [GRITZ97] Gritz, Larry I., Blue Moon Rendering Tools -- 
 *                    Summary of recent changes [3.4 to 3.5], Silver 
 *                    Spring, MD, January 1997.
 *          [MTOR00]  MTOR RIB output (possible PRMan 3.9 addition.
 *          [PIXA89]  Pixar Animation Studios, The RenderMan Interface, 
 *                    Version 3.1, Richmond, CA, September 1989.
 *          [PIXA90]  Pixar Animation Studios, Application Note #9: 
 *                    PhotoRealistic RenderMan: OPTIONS AND ATTRIBUTES, 
 *                    Richmond, CA, May 1990.
 *          [PIXA93a] Pixar Animation Studios, PhotoRealistic RenderMan 
 *                    User's Manual: Unix Version, Richmond, CA, January 
 *                    1993.
 *          [PIXA93b] Pixar Animation Studios, Vector RenderMan 3.5 User's
 *                    Manual: Unix Version, Richmond, CA, January 1993.
 *          [PIXA96]  Pixar Animation Studios, Application Note #7: 
 *                    Reserved RenderMan Parameters Names, Richmond, CA,
 *                    March 1996.
 *          [PIXA98a] Pixar Animation Studios, cineon.4.html,
 *                    Reserved RenderMan Parameters Names, Richmond, CA,
 *                    1998.
 *          [PIXA98b] Pixar Animation Studios, user.html,
 *                    Reserved RenderMan Parameters Names, Richmond, CA,
 *                    1998.
 *
 *    History:
 *          02-??-97  Updated to include the BMRT 3.5 additions.
 *          03-28-97  (Last minute clean-ups)
 *                    Added a TO-DO list of little known items that should 
 *                    be added to the hash table.  These items were listed
 *                    in a HTML file from Pixar.  While searching for some
 *                    of these items, a vectrman option and attribute
 *                    that were overlooked were added.
 *          06-10-97  Added memory tests to check for memory leaks.
 *                    Use a -DRIB_MEMORY_TESTS option in Makefiles.
 *          07-10-97  Added variable declarations for 3.7 Curves and Points
 *                    support.  Added class "constant" and type "matrix".
 *          03-28-98  Added RibFreeBasisMatrix(). 
 *          12-30-98  Fixed ifdef TEST that excluded needed code for
 *                    handling already defined variable names.
 *          05-30-99  Added setpoints for cineon driver Display option added
 *                    in PRMan 3.8.
 *          10-11-99  Changed struct _variables classNtype to be unsigned.
 *                    This cleaned up some warnings about values being
 *                    out of range.  
 *          10-17-99  Added server searchpath token names to hash table.
 *          02-15-00  Fixed gridsize and merge declarations [EVES00].
 *          02-26-00  Added RibDumpHashTable().
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 *
 */
#define __RIB_FILE_HASH
#define _RIPRIV_TYPES
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ripriv.h>
#include <ribhash.h>
#include "hash.h"


static void RibInitHashTable( RIB_HASHHND hash );


#define RibHashValueFromInt( /* RIB_HASHHND */ hashhandle,  \
                             /* int key */ k )              \
   ((k) % ((hashhandle)->modnumber))

#define RibHash2ValueFromInt( /* RIB_HASHHND */ hashhandle, \
                             /* int key */ k )              \
   ((k) % ((hashhandle)->mod2number))


/* 
 *  Globals only for testing for memory leaks.
 *
 */
#if defined(RIB_MEMORY_TESTS)
PMEM_TEST  gPMemTest = NULL;
#endif

int RibHashValueFromString( RIB_HASHHND hash, char *p )
{
   auto int hashvalue;

 
   hashvalue = *p;
   while ( *p )
   {
      hashvalue = (( hashvalue * 32) + (*p)) % (hash->modnumber);
      p++;
   }

   return hashvalue;
}


int RibHashValueFromStringN( RIB_HASHHND hash, char *p, int n )
{
   auto int  hashvalue;

 
   hashvalue = *p;
   while ( *p && n )
   {
      hashvalue = (( hashvalue * 32) + (*p)) % (hash->modnumber);
      p++;
      n--;
   }
   
   return hashvalue;
}


/* The Affine libraries use macros _RibMalloc(), _RibRealloc() and _RibFree()
 *   to call the stdlib calls inline rather than go through these calls.
 */


#ifndef RIB_MEMORY_TESTS

void *RibMalloc( unsigned int size )
{
   return malloc( size );
}

void *RibRealloc( void *p, unsigned int size )
{
   return realloc( p, size );
}

void RibFree( void *p )
{
   free( p );
   return;
}

#else


void *RibMalloc( char *file, unsigned int line, unsigned int size )
{
   void *p;


   if (!gPMemTest)
   {
      gPMemTest = RibMemoryTestInit();
   }

   p = malloc( size );

   if (p)
   {
      RibMemoryTestMalloc( file, line, p, size );
   }

   return p;
}


void *RibRealloc( char *file, unsigned int line, void *p, unsigned int size )
{
   void  *pp;
   PMEM_LIST  l;


   if (!gPMemTest)
   {
      fprintf( stderr, "ERROR: realloc called before malloc: %s(%u)\n",
              file, line );      
   }

   pp = p;

   p = realloc( p, size );

   if ( p!=pp )
   {
      /* If realloc() is given a NULL pointer it just acts like malloc(). */
      if (pp)
      {
         RibMemoryTestFree( file, line, pp );
      }
      RibMemoryTestMalloc( file, line, p, size );      
   }
   else
   {
      l = gPMemTest->memlist;
      while ( l )
      {
         if (l->p == p)
         {
            l->size = size;
            break;
         }
         l = l->next;
      }      
   }

   return p;
}


void RibFree( char *file, unsigned int line, void *p )
{
   if (!gPMemTest)
   {
      fprintf( stderr, "ERROR: free called before malloc: %s(%u)\n",
              file, line );
   }
   RibMemoryTestFree( file, line, p );
   free( p );
   return;
}


PMEM_TEST RibMemoryTestInit( void )
{
   PMEM_TEST  p;


   p = malloc( sizeof(MEM_TEST) );

   if (!p)
     return NULL;

   p->nfree = 0;
   p->nmalloc = 0;
   p->nstepovers = 0;
   p->badfreecalls = 0;
   p->memlist = NULL;
   p->badfreelist = NULL;
   p->stepoverlist = NULL;
   p->stepoverallowed = 0;

   return p;
}


int RibMemoryTestStepOverAllowed( void )
{
   if (!gPMemTest)
   {
      fprintf( stderr, "ERROR: StepOverAllowed called before malloc.\n" );
      return 1;
   }
   gPMemTest->stepoverallowed = 1;

   return 0;
}


int RibMemoryTestResults( PMEM_TEST p )
{
   static char border[] = "----------------------------------------------\n";
   PMEM_LIST  l;


   if (!p)
     return 1;

   fprintf( stderr, border );
   fprintf( stderr, "Valid _RibMalloc() calls: %u\n", p->nmalloc );
   fprintf( stderr, "Valid _RibFree() calls: %u\n", p->nfree );
   fprintf( stderr, "Step overs during _RibFree(): %u\n", p->nstepovers );
   if (gPMemTest->stepoverlist)
   {
      l = gPMemTest->stepoverlist;
      while ( l )
      {
         fprintf( stderr, "   %s(%u)", l->file, l->line );
         l = l->next;
      }      
      fputc('\n',stderr);
   }
   fprintf( stderr, "Invalid _RibFree() calls: %u\n", p->badfreecalls );
   if (gPMemTest->badfreelist)
   {
      l = gPMemTest->badfreelist;
      while ( l )
      {
         fprintf( stderr, "   %s(%u)", l->file, l->line );
         l = l->next;
      }      
      fputc('\n',stderr);
   }
   fprintf( stderr, "Memory leaks found: %u\n", p->nmalloc - p->nfree );
   if (gPMemTest->memlist)
   {
      l = gPMemTest->memlist;
      while ( l )
      {
         fprintf( stderr, "   %s(%u):%p", l->file, l->line, l->p );
         l = l->next;
      }      
      fputc('\n',stderr);
   }
   fprintf( stderr, border );

   l = gPMemTest->memlist;
   while ( l )
   {
      if (l->file)
        free(l->file);
      free(l);
      l = l->next;
   }      
   l = gPMemTest->badfreelist;
   while ( l )
   {
      if (l->file)
        free(l->file);
      free(l);
      l = l->next;
   }      
   l = gPMemTest->stepoverlist;
   while ( l )
   {
      if (l->file)
        free(l->file);
      free(l);
      l = l->next;
   }      

   return 0;
}


int RibMemoryTestMalloc( char *file, unsigned int line, 
                        void *p, unsigned int size )
{
   PMEM_LIST  l;


   if (gPMemTest && p)
   {
      l = malloc( sizeof(MEM_LIST) );
      if (l)
      {
         l->file = malloc( strlen(file)+1 );
         if (l->file)
         {
            l->line = line;
            l->size = size;
            l->p = p;
            strcpy( l->file, file );
            l->next = gPMemTest->memlist;
            gPMemTest->memlist = l;
            gPMemTest->nmalloc++;

            return 0;
         }
         else
         {
            free(l);
         }
      }
   }
   return 1;
}


int RibMemoryTestFree( char *file, unsigned int line, void *p )
{
   PMEM_LIST  l,ll,lll;
   int first = 1;


   if ( gPMemTest && p )
   {
      ll = NULL;
      l = gPMemTest->memlist;
      while ( l )
      {
         if (l->p == p)
         {
            if (ll)
            {
               ll->next = l->next;
            }
            else
            {
               gPMemTest->memlist = l->next;
            }
            gPMemTest->nfree++;
            free(l);
            return 0;
         }
         ll = l;
         l = l->next;

         if (first)
         {
            if (!gPMemTest->stepoverallowed)
            {
               lll = malloc( sizeof(MEM_LIST) );
               if (lll)
               {
                  lll->file = malloc( strlen(file)+1 );
                  if (lll->file)
                  {
                     lll->line = line;
                     lll->size = 0;
                     lll->p = p;
                     strcpy( lll->file, file );
                     lll->next = gPMemTest->stepoverlist;
                     gPMemTest->stepoverlist = lll;
                     gPMemTest->nstepovers++;
                  }
                  else
                  {
                     free(lll);
                  }
               }
            }
            else
            {
               gPMemTest->stepoverallowed = 0;
            }
            first = 0;
         }
         else
         {
            gPMemTest->stepoverallowed = 0;
         }
      }
   }
   else
   {
      l = malloc( sizeof(MEM_LIST) );
      if (l)
      {
         l->file = malloc( strlen(file)+1 );
         if (l->file)
         {
            l->line = line;
            l->size = 0;
            l->p = p;
            strcpy( l->file, file );
            l->next = gPMemTest->badfreelist;
            gPMemTest->badfreelist = l;
            gPMemTest->badfreecalls++;
         }
         else
         {
            free(l);
         }
      }
   
      return 1;
   }

   return 0;
}


void RibDumpHashTable( RIB_HASHHND hash )
{
   PRIB_HASHATOM p;
   int i,j;
   
   printf("\nhashtable structure:%p  hash data:%p\n",hash,hash->apAtoms);
   for ( i=0; i<hash->modnumber; i++ )
   {
      for ( j=0; j<hash->mod2number; j++ )
      {
	 if ( hash->apAtoms[i][j] )
	 {
	    p = hash->apAtoms[i][j];
	    while (p)
	    {
	       printf( "[%d][%d] p->code=%x \"%s\"\n", 
		       i, j, p->code, (char*)p->data );
	       p=p->next;
	    }
	 }
      }
   }
   printf("\n");

   return;
}

#endif


int RibFreeBasisMatrix( RtBasis p )
{
   if ( p
        && p != RiBezierBasis 
        && p != RiBSplineBasis
        && p != RiCatmullRomBasis
        && p != RiHermiteBasis
        && p != RiPowerBasis )
   {
      _RibFree( p );
   }

   return 0;
}


RIB_HASHHND RibCreateHashTable( int size )
{
   register PRIB_HASHTABLE  ptable;
   register size_t          i;
   
   i = sizeof(PRIB_HASHATOM)*(size*kRIB_HASH_SECONDSIZE);
   ptable = (PRIB_HASHTABLE)(void *)_RibMalloc( i + sizeof(RIB_HASHTABLE));
   ptable->modnumber  = size;
   ptable->mod2number = kRIB_HASH_SECONDSIZE;
   ptable->conflict = NULL;
   memset( ptable->apAtoms, 0, i );

   RibInitHashTable( (RIB_HASHHND)ptable );

   return ptable;
}


PRIB_HASHATOM RibHash( RIB_HASHHND hash, int type, int code, char *s, 
                      PRIB_HASHATOM **pparentnext )
{
   int  h1, h2;
   PRIB_HASHATOM  p, *pp;


   type &= kRIB_HASH_TYPEMASK;
   if ( s && type < kRIB_HASH_LAST )
   {
      h1 = RibHashValueFromString( hash, s );
      h2 = RibHash2ValueFromInt( hash, 
                                ( s[0] ? (s[1] ? (s[0]<<6)+s[1] : s[1]) : 0 ));
   }
   else
   {
      h1 = RibHashValueFromInt( hash, code );
      h2 = RibHash2ValueFromInt( hash, code );
   }

#ifdef TEST
   if (s)
     printf( "\"%s\" h1: %d, h2: %d\n", s, h1, h2 );
   else
     printf( "'\\0' h1: %d, h2: %d\n", h1, h2 );
#endif

   pp = &(hash->apAtoms[h1][h2]); 
   p  = hash->apAtoms[h1][h2]; 

#ifdef TEST
   printf( "p:%p\n",p);
   printf( "pp:%p\n",pp);
#endif 
   
   if (type)
   {
      while ( p )
      {
         if ( (p->type&kRIB_HASH_TYPEMASK) == type )
         {
            if (s)
            {
               if ( strcmp( (char *)p->data, s ) == 0 )
                 break;
            }
            else 
            {
               if (p->code == code)
                 break;
            }
         }
         pp = &(p->next);
         p = p->next;
      }
   }
   
   if (pparentnext)
     *pparentnext = pp;
   
   return p;
}


/*
 * ((type & kRIB_HASH_TYPEMASK) == (kRIB_HASH_RIREQUESTCODE)
 *   RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, int requestcode, 
 *              int ripID );
 *
 * ((type & kRIB_HASH_TYPEMASK) == kRIB_HASH_VARIABLE) 
 *   && !(classNtype & kRIB_ARRAY_DECLARED)  -- Notice the negation '!'.
 *   RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, int classNtype,
 *               char *variablename );
 *
 * ((type & kRIB_HASH_TYPEMASK) == kRIB_HASH_VARIABLE) 
 *   && (classNtype & kRIB_ARRAY_DECLARED)
 *   RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, int classNtype,
 *               char *variablename, int ntype );
 *
 * ((type & kRIB_HASH_TYPEMASK) == kRIB_HASH_STRINGTOKEN)
 *   RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, int code, char *string );
 *
 * ((type & kRIB_HASH_TYPEMASK) == kRIB_HASH_STRING)
 *   RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, int code, char *string );
 *
 * ((type & kRIB_HASH_TYPEMASK) == kRIB_HASH_LIGHTHANDLE) 
 *   or (type & kRIB_HASH_TYPEMASK == kRIB_HASH_OBJECTHANDLE)
 *   RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, int handle, 
 *              void *extended_data );
 *
 * ((type & kRIB_HASH_TYPEMASK) == kRIB_HASH_COORDSYS)
 *   RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, char *name, 
 *              void *extended_data );
 *
 * ((type & kRIB_HASH_TYPEMASK) >= kRIB_HASH_USER)
 *   RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, int code, 
 *               void *data, void *extended_data );
 */
PRIB_HASHATOM RibAddItem( RIB_HASHHND hash, RIB_UINT32 type, ... )
{
   va_list        arg;
   RIB_UINT32     mtype;
   int            code = 0;
   int            ripIDorN = 0;
   void           *data = NULL;
   void           *extdata = NULL;
   PRIB_HASHATOM  p, *pp;


   va_start( arg, type );
   mtype = type & kRIB_HASH_TYPEMASK;
   switch ( mtype )
   {
    case kRIB_HASH_RIREQUESTCODE:
      code = (int)va_arg( arg, int );
      ripIDorN = (int)va_arg( arg, int );
#ifdef TEST
      printf( "RibAddItem: RIREQUESTCODE, %d\n", ripIDorN );
#endif
      break;
    case kRIB_HASH_STRINGTOKEN:
      code = (int)va_arg( arg, int );
      extdata = (void*)va_arg( arg, char* );
#ifdef TEST
      printf( "RibAddItem: STRINGTOKEN, %s\n", extdata );
#endif
      break;
    case kRIB_HASH_STRING:
      code = (int)va_arg( arg, int ); /* code */
      data = (void*)va_arg( arg, char* );
#ifdef TEST
      printf( "RibAddItem: STRING, %s\n", data );
#endif
      break;
    case kRIB_HASH_VARIABLE:
      code = (int)va_arg( arg, int ); /* classNtype */
      data = (void*)va_arg( arg, char* );
      if (code & kRIB_ARRAY_DECLARED)
      {
         ripIDorN = (int)va_arg( arg, int ); /* n of type */      
      }
      else
      {
         ripIDorN = 1;
      }
#ifdef TEST
      printf( "RibAddItem: VARIABLE, %s\n", data );
#endif
      break;
    case kRIB_HASH_LIGHTHANDLE:
    case kRIB_HASH_OBJECTHANDLE:
      code = (int)va_arg( arg, int );
      extdata = (void*)va_arg( arg, void* );
#ifdef TEST
      printf( "RibAddItem:  %sHANDLE, %d\n", 
             (mtype==kRIB_HASH_LIGHTHANDLE ? "LIGHT" : "OBJECT"), code );
#endif
      break;
    case kRIB_HASH_COORDSYS:
      data = (void*)va_arg( arg, char* );
      extdata = (void*)va_arg( arg, void* );
#ifdef TEST
      printf( "RibAddItem: COORDINATESYSTEM, data: %s\n", data );
#endif
      break;
    default:
      if ( mtype >= kRIB_HASH_USER )
      {
         code = (int)va_arg( arg, int );
         data = (void*)va_arg( arg, void* );
         extdata = (void*)va_arg( arg, void* );
#ifdef TEST
         printf( "RibAddItem: USER, type:%d\n",mtype);
#endif
      }
   }
   
   p = RibHash(  hash, mtype, code, (char*)data, &pp );
   
   if (!p)
   {
      p = (PRIB_HASHATOM)(void *)_RibMalloc(sizeof(RIB_HASHATOM));
      p->type = type;
      p->code = code;
      p->data = data; 

      switch ( mtype )
      {
       case kRIB_HASH_RIREQUESTCODE:
         p->with.ripID = ripIDorN;
         break;
       case kRIB_HASH_VARIABLE:
        p->with.n = ripIDorN;
         break;
       default:
        p->with.extdata = extdata; 
      }
#ifdef TEST      
      printf("created %p\n",p);
      printf("type: %08x\n", type );
      printf("code: %08x\n", code );
#endif

      p->next = *pp;
      
#ifdef TEST
      printf("next is %p\n",*pp);
#endif
      
      *pp = p;
   }
   else
   {
#ifdef TEST
      printf("found item: %d\n", p->code );
#endif
      hash->conflict = p;
      p = NULL;
   }
   
   va_end(arg);
   
   return p;
}


PRIB_HASHATOM RibGetConflictingHashAtom( RIB_HASHHND hash )
{
   auto PRIB_HASHATOM  p;

   p = hash->conflict;
   if (p)
     hash->conflict = NULL;

   return p;
}


int RibRemoveItem( RIB_HASHHND hash, PRIB_HASHATOM patom )
{
   PRIB_HASHATOM  p, *pp;

   p = RibHash(  hash, patom->type, patom->code, (char*)patom->data, &pp );

   if (!p)
     return kRIB_ERRRC_INT; /* return error */

   *pp = p->next;
   if (p->type & kRIB_HASH_FREEEXTDATA)
     _RibFree(p->with.extdata);
   if (p->type & kRIB_HASH_FREEDATA)
     _RibFree(p->data);
   _RibFree(p);
   
   return kRIB_OK;
}


PRIB_HASHATOM RibFindMatch( RIB_HASHHND hash, RIB_UINT32 type, 
                           int code, void *data )
{
   PRIB_HASHATOM  p;

   p = RibHash(  hash, type, code, (char*)data, NULL );

   return p;
}


PRIB_HASHATOM RibFindNextMatch( RIB_HASHHND hash, PRIB_HASHATOM patom )
{
   register PRIB_HASHATOM  p = patom;
   auto RIB_UINT32  type;
   auto int  code;


   if (hash && p)
   {
      p  = patom->next;
      type = p->type & kRIB_HASH_TYPEMASK;
      code = p->code;
      while ( p )
      {
         if ( (p->type & kRIB_HASH_TYPEMASK) == type  && p->code == code )
           break;
         p = p->next;
      }
   }
   else
     return kRIB_ERRRC_PTR;
   
   return p;
}


int RibDestroyHashTable( RIB_HASHHND hash )
{
   register int  i,j;
   register PRIB_HASHATOM  p, pp;

   
   for ( i=0; i < hash->modnumber; i++ )
   {
      for ( j=0; j < hash->mod2number; j++ )
      {
         if ( hash->apAtoms[i][j] )
         {
            p = hash->apAtoms[i][j];
            while (p)
            {
               pp = p->next;
               if (p->type & kRIB_HASH_FREEEXTDATA)
               {
                  #ifdef RIB_MEMORY_TESTS
                  RibMemoryTestStepOverAllowed();
                  #endif
                  _RibFree(p->with.extdata);
               }
               if (p->type & kRIB_HASH_FREEDATA)
               {
                  #ifdef RIB_MEMORY_TESTS
                  RibMemoryTestStepOverAllowed();
                  #endif
                  _RibFree(p->data);
               }
               #ifdef RIB_MEMORY_TESTS
               RibMemoryTestStepOverAllowed();
               #endif
               _RibFree(p);
               p=pp;
            }
         }
      }
   }
   _RibFree( hash );
   
   return kRIB_OK;
}


int RibDestroyHashTableKeepData( RIB_HASHHND hash )
{
   register int  i,j;
   register PRIB_HASHATOM  p, pp;

   
   for ( i=0; i < hash->modnumber; i++ )
   {
      for ( j=0; j < hash->mod2number; j++ )
      {
         if ( hash->apAtoms[i][j] )
         {
            p = hash->apAtoms[i][j];
            while (p)
            {
               pp = p->next;
               _RibFree(p);
               p=pp;
            }
         }
      }
   }
   _RibFree( hash );
   
   return kRIB_OK;
}


RIB_UINT32 RibQueryClassType( RIB_HASHHND hash, char *variable, 
                             RIB_UINT32 *n )
{
   register PRIB_HASHTABLE  h = (RIB_HASHHND)hash;
   register PRIB_HASHATOM  p;

   
   if (!h)
     return (kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE);
        
   p = RibFindMatch( (RIB_HASHHND)h, kRIB_HASH_VARIABLE, 0, variable );
   
   if (!p)
     return (kRIB_UNKNOWNCLASS | kRIB_UNKNOWNTYPE);

   if ( n )
   {
      *n = p->with.n;
   }

   return (p->code & kRIB_CLASSTYPE_MASK);
}


int RibSetDefinedRIRequest( PRIB_HASHATOM a, int token, int ripID )
{
   a->code = token;
   a->with.ripID = ripID;

   return kRIB_OK;
}


int RibGetDefinedRIRequest( PRIB_HASHATOM a )
{
   if (a && ((a->type & kRIB_HASH_TYPEMASK) == kRIB_HASH_RIREQUESTCODE) )
     return (int)(a->with.ripID);
   else
     return kRIB_ERRRC_INDEX;
}


int RibSetDefinedString( PRIB_HASHATOM a, int token, char *s )
{
   a->code = token;
   a->with.extdata = s;

   return kRIB_OK;
}


char *RibGetDefinedString( PRIB_HASHATOM a )
{
   if (a && ((a->type & kRIB_HASH_TYPEMASK) == kRIB_HASH_STRINGTOKEN) )
     return (char*)a->with.extdata;
   else
     return kRIB_ERRRC_PTR;
}


int RibSetStringAtom( PRIB_HASHATOM a, int token, char *s )
{
   a->code = token;
   a->data = s;

   return kRIB_OK;
}


char *RibGetStringAtom( PRIB_HASHATOM a )
{
   if (a && ((a->type & kRIB_HASH_TYPEMASK) == kRIB_HASH_STRING) )
     return (char*)a->data;
   else
     return kRIB_ERRRC_PTR;
}


int RibSetDefinedLightHandle( PRIB_HASHATOM a, 
                             int sequence, RtLightHandle lh )
{
   a->code = sequence;
   a->with.extdata = (void*)lh;

   return kRIB_OK;
}


RtLightHandle RibGetDefinedLightHandle( PRIB_HASHATOM a )
{
   if (a && ((a->type & kRIB_HASH_TYPEMASK) == kRIB_HASH_LIGHTHANDLE) )
     return (RtLightHandle)a->with.extdata;
   else
     return kRIB_ERRRC_PTR;
}


int RibSetDefinedObjectHandle( PRIB_HASHATOM a, 
                             int sequence, RtObjectHandle oh )
{
   a->code = sequence;
   a->with.extdata = (void*)oh;

   return kRIB_OK;
}


RtObjectHandle RibGetDefinedObjectHandle( PRIB_HASHATOM a )
{
   if (a && ((a->type & kRIB_HASH_TYPEMASK) == kRIB_HASH_OBJECTHANDLE) )
     return (RtObjectHandle)a->with.extdata;
   else
     return kRIB_ERRRC_PTR;
}


int RibSetHashExtendedData( PRIB_HASHATOM a, void *t )
{
   a->with.extdata = t;

   return kRIB_OK;
}

void* RibGetHashExtendedData( PRIB_HASHATOM a )
{
   if (a)
     return a->with.extdata;
   else
     return kRIB_ERRRC_PTR;
}


/* 
 *  Macros for RibInitHashTable() -- added only for readability:
 */
#define CONSTANT    kRIB_CONSTANTCLASS
#define UNIFORM     kRIB_UNIFORMCLASS
#define VERTEX      kRIB_VERTEXCLASS
#define VARYING     kRIB_VARYINGCLASS
#define COLOR       kRIB_COLORTYPE
#define POINT       kRIB_POINTTYPE
#define HPOINT      kRIB_HPOINTTYPE
#define NORMAL      kRIB_NORMALTYPE
#define STRING      kRIB_STRINGTYPE
#define INT         kRIB_INTTYPE
#define FLOAT       kRIB_FLOATTYPE
#define FLOATPAIR   kRIB_FLOATPAIRTYPE
#define INTPAIR     kRIB_INTPAIRTYPE
#define STANDALONE  kRIB_STANDALONETYPE

/* NOTE:
 *   Items defined with kRIB_SYS_DECLARED are those listed in Application
 *   note #7 as names that are strongly recommended not to be redefined
 *   in a RIB file.  If redefined with a Declare statement, libribrdr.a will
 *   print a warning.  Refer to kRIB_ERRTYPE_SYSTEM_PARAM.
 */
/* The flags kRIB_HASH_FREEDATA and kRIB_HASH_FREEEXTDATA 
 *    are not included because the strings are not stored
 *    in malloc'ed memory.
 */
#define  ADDITEM( ItemName, ItemClass )                           \
                 RibAddItem( hash, kRIB_HASH_VARIABLE,            \
                            kRIB_SYS_DECLARED | (ItemClass), (ItemName) )

/* 03-28-97 TO-DO:
 *
 *   Add the following to the hash table.  They were list in "Application 
 *   Note #7: Reserved RenderMan Parameters Names" (First I need to look 
 *   around a bit and find something published that states what these guys 
 *   are for.  Some are kind'a obvious, but hey...):
 *
 *          algorithm
 *          bucketstride
 *          dynamic
 *          endofframe
 *          epsilon1, epsilon2
 *          gridarea 
 *          interleave
 *          nearhither
 *          print
 *          subwindow
 *          trimdeviation
 *
 *          chanlist
 *          pattern
 *          pvn
 *          gridarea
 *          vnmag
 *          vtags
 *          vdmag
 *
 *   Haven't found anything published saying what these are for.  
 *   But by experimenting with some random Declare statements, rendrib 2.3.5
 *   will give warnings about "endofframe" and "interleave" using the
 *   following RIB file (prman 3.6c gives no warnings):
 *          
 *            Display "a.tif" "framebuffer" "rgba" 
 *            Format 256 256 1
 *            WorldBegin 
 * 
 *            Declare "endofframe" "string"
 *            Declare "interleave" "integer"
 *
 *            WorldEnd 
 *
 *   The following warnings are printed by rendrib (BMRT 3.5) using the
 *   above RIB file:
 *
 *       WARNING: unpub2.rib (5): Parameters inconsistent
 *           --> "endofframe" (uniform integer) redeclared as uniform string
 *       WARNING: unpub2.rib (6): Parameters inconsistent
 *           --> "interleave" (uniform string) redeclared as uniform integer
 *
 */
static void RibInitHashTable( RIB_HASHHND hash )
{
   static struct _variables {
      RtToken       variable;
      unsigned int  classNtype;
   } variables[] = {

   /* 
    *   The following was added for the RiOption call. 
    *
    */

   /* The "rib" options of Pixar's prman: */
    { "format", UNIFORM | STRING },

   /* The "limits" options of Pixar's prman: */
    { "bucketsize", UNIFORM | INTPAIR | kRIB_SYS_DECLARED },
    { "gridsize", UNIFORM | INT | kRIB_SYS_DECLARED },
    { "texturememory", UNIFORM | INT | kRIB_SYS_DECLARED },
    { "zthreshold", UNIFORM | POINT },
    { "extremedisplacement", UNIFORM | INT },
    { "eyesplits", UNIFORM | INT },

   /* The "limits" options of BMRT's rendrib (2.3.5): */ 
   /* Already defined above: { "texturememory", UNIFORM | INT }, */
    { "geommemory", UNIFORM | INT },

   /* The "shadow" options of RenderMan Artist Tools' prman: */
    { "bias0", UNIFORM | FLOAT | kRIB_SYS_DECLARED },
    { "bias1", UNIFORM | FLOAT | kRIB_SYS_DECLARED },  
    { "bias", UNIFORM | FLOAT | kRIB_SYS_DECLARED },  

   /* The "searchpath" options of RenderMan Artist Tools' prman: */
    { "shader", UNIFORM | STRING | kRIB_SYS_DECLARED },
    { "texture", UNIFORM | STRING | kRIB_SYS_DECLARED },
    { "vfxmaster", UNIFORM | STRING },
    { "vfxinstance", UNIFORM | STRING },
    { "archive", UNIFORM | STRING }, /* Added for PRMan 3.8 */
    { "resource", UNIFORM | STRING }, /* Missing item added. */
    { "display", UNIFORM | STRING }, 
    { "servershader", UNIFORM | STRING | kRIB_SYS_DECLARED },
    { "servertexture", UNIFORM | STRING | kRIB_SYS_DECLARED },
    { "servervfxmaster", UNIFORM | STRING },
    { "servervfxinstance", UNIFORM | STRING },
    { "serverarchive", UNIFORM | STRING },
    { "serverresource", UNIFORM | STRING },
    { "serverdisplay", UNIFORM | STRING }, 

   /* The "render" options of BMRT's rendrib: */ 
    { "minsamples", UNIFORM | INT },
    { "maxsamples", UNIFORM | INT },
    { "max_raylevel", UNIFORM | INT },
    { "branch_ratio", UNIFORM | INT },
    { "max_branch_ratio", UNIFORM | INT },
    { "minshadowbias", UNIFORM | FLOAT },

   /* The "radiosity" options of BMRT's rendrib: */
    { "steps", UNIFORM | INT },
    { "minpatchsamples", UNIFORM | INT },

   /* The "runtime" options of BMRT's rendrib: */
    { "verbosity", UNIFORM | STRING },

   /* The "searchpath" options of BMRT's rendrib: */ 
    { "include", UNIFORM | STRING },

   /* The "display" options of Pixar's vectrman: */
    { "refresh",   UNIFORM | INT },  /* See [PIXA93b]. */
    { "fullcolor", UNIFORM | INT },  /* See [PIXA93b]. */
    { "setpoints", UNIFORM | INTPAIR },  /* See [PIXA98]. */
    
   /* The "texture" options of PRMan: */
    { "enable gaussian", UNIFORM | FLOAT },
    { "enable lerp", UNIFORM | FLOAT },

   /* 
    *  The following was added for the RiAttribute call.               
    *
    */

   /* The "identifier" attributes of Pixar's prman: */ 
    { "name", UNIFORM | STRING | kRIB_SYS_DECLARED },

   /* Added because Alias Sketch! uses it and the Pixar tools don't
    *    have any problems with it.  
    * So one more for the "identifier" attributes of Pixar's prman: 
    */ 
    { "shadinggroup", UNIFORM | STRING | kRIB_SYS_DECLARED },


   /* The "dice" attributes of Pixar's prman: */ 
    { "binary", UNIFORM | INT | kRIB_SYS_DECLARED },

   /* The "displacementbound" attributes of Pixar's prman: */ 
    { "coordinatesystem", UNIFORM | STRING },
    { "sphere", UNIFORM | FLOAT },
    /*{ "space", UNIFORM | STRING },*//*Note: In 3.8 docs but not prman 3.8.*/

   /* The "trimcurve" attributes of Pixar's prman (also BMRT 2.3.5): */ 
    { "sense", UNIFORM | STRING },

   /* The "radiosity" attributes of BMRT's rendrib: */
    { "averagecolor", UNIFORM | COLOR },
    { "emissioncolor", UNIFORM | COLOR },
    { "patchsize", UNIFORM | INT },
    { "elemsize", UNIFORM | INT },
    { "minsize", UNIFORM | INT },
    { "zonal", UNIFORM | STRING },
    { "casts_shadows", UNIFORM | STRING },
    /* The variable "specularcolor is defined with RI_SPECULARCOLOR below. */

   /* The "render" attributes of BMRT's rendrib: */ 
    { "patch_maxlevel", UNIFORM | INT },
    { "patch_minlevel", UNIFORM | INT }, /* 2.3.5 */
    { "patch_multiplier", UNIFORM | INT }, /* 2.3.6? */
    { "truedisplacement", UNIFORM | INT }, /* 2.3.5 */

   /* The "light" attributes of BMRT's rendrib. */
    { "shadows", UNIFORM | STRING },
    { "cache", UNIFORM | STRING },
    { "nsamples", UNIFORM | INT }, /* BMRT 2.3.5 */

   /* The "divisions" attributes of Pixar's vectrman: */
    { "udivisions", UNIFORM | INT }, /* See [PIXA93b]. */
    { "vdivisions", UNIFORM | INT }, /* See [PIXA93b]. */


   /* 
    *   The following were added for the RiDisplay call. 
    *
    */

   /* A required option of RiDisplay() is "origin". */
    { "origin", UNIFORM | INTPAIR | kRIB_SYS_DECLARED },

   /* More Display options of Pixar's prman: */
    /* See Application Note #9 and section 4.1.3 of [PIXA89]. */
    { "merge", UNIFORM | INT | kRIB_SYS_DECLARED }, 
    { "compression",    UNIFORM | STRING },   /* See [PIXA93a]. */
    { "resolution",     UNIFORM | INTPAIR },  /* See [PIXA93a]. */
    { "resolutionunit", UNIFORM | STRING },   /* See [PIXA93a]. */
    { "dspyParams", UNIFORM | STRING },   /* See [MTOR00]. */


   /* 
    *   The following was added for the RiHider call. 
    *
    */
   /* Hider's "hidden" options: */
    { "jitter", UNIFORM | INT | kRIB_SYS_DECLARED },
    { "pdisc", UNIFORM | INT },


   /* 
    *   The following was added for the RiProjection call. 
    *
    */
    { "fov", UNIFORM | FLOAT | kRIB_SYS_DECLARED },

   /* 
    *   The following was added for the RiGeometricApproximation call. 
    *
    */
    { "flatness", UNIFORM | FLOAT | kRIB_SYS_DECLARED },
    { "motionfactor", UNIFORM | FLOAT },


   /* 
    *   The following were added for the RiCurves and RiPoints calls that
    *      are new in PRMan 3.7.
    *
    */
    { "width", VARYING | FLOAT | kRIB_SYS_DECLARED },
    { "constantwidth", CONSTANT | FLOAT | kRIB_SYS_DECLARED },

   /* 
    *   The following were added for the RiPoints call that is new in 
    *      PRMan 3.7.   (HTML files described both "width" and "radius".  
    *      The "radius" variable doesn't seem to affect the particle sizes,
    *      but catrib and prman seem to have radius declared.
    */
    { "radius", VARYING | FLOAT | kRIB_SYS_DECLARED },

   };
   int  i;


   ADDITEM( RI_INTENSITY, UNIFORM | FLOAT );
   ADDITEM( RI_LIGHTCOLOR, UNIFORM | COLOR );
   ADDITEM( RI_FROM, UNIFORM | POINT );
   ADDITEM( RI_TO, UNIFORM | POINT );
   ADDITEM( RI_CONEANGLE, UNIFORM | FLOAT );
   ADDITEM( RI_CONEDELTAANGLE, UNIFORM | FLOAT );
   ADDITEM( RI_BEAMDISTRIBUTION, UNIFORM | FLOAT );
   
   ADDITEM( RI_KA, UNIFORM | FLOAT );
   ADDITEM( RI_KD, UNIFORM | FLOAT );
   ADDITEM( RI_KS, UNIFORM | FLOAT );
   ADDITEM( RI_ROUGHNESS, UNIFORM | FLOAT );
   ADDITEM( RI_KR, UNIFORM | FLOAT );
   ADDITEM( RI_TEXTURENAME, UNIFORM | STRING );
   ADDITEM( RI_SPECULARCOLOR, UNIFORM | COLOR );
      
   ADDITEM( RI_DISTANCE, UNIFORM | FLOAT );
   ADDITEM( RI_MINDISTANCE, UNIFORM | FLOAT );
   ADDITEM( RI_MAXDISTANCE, UNIFORM | FLOAT );
   ADDITEM( RI_BACKGROUND, UNIFORM | COLOR );
   ADDITEM( RI_AMPLITUDE, UNIFORM | FLOAT );
   
   ADDITEM( RI_P, VERTEX | POINT );
   ADDITEM( RI_PZ, VERTEX | FLOAT );
   ADDITEM( RI_PW, VERTEX | HPOINT );
   
   ADDITEM( RI_N, VARYING | NORMAL );
   ADDITEM( RI_NP, UNIFORM | NORMAL );
   ADDITEM( RI_CS, VARYING | COLOR );
   ADDITEM( RI_OS, VARYING | COLOR );
   ADDITEM( RI_S, VARYING | FLOAT );
   ADDITEM( RI_T, VARYING | FLOAT );
   ADDITEM( RI_ST, VARYING | FLOATPAIR );
   
   /* The following was added for the RiDisplay call. */
   ADDITEM( RI_ORIGIN, INTPAIR );
   
   for ( i=0; i<sizeof(variables)/sizeof(struct _variables); i++ )
   {
      /* The flags kRIB_HASH_FREEDATA and kRIB_HASH_FREEEXTDATA 
       *    are not included because the strings are not stored
       *    in malloc'ed memory.
       */
      RibAddItem( hash, kRIB_HASH_VARIABLE,      
                 kRIB_SYS_DECLARED | variables[i].classNtype, 
                 variables[i].variable );
   }

   return;
}


