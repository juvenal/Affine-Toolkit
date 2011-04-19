/* 
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
 * FILE:  trimstore.c
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *  
 *       To-Do:   Add support for trimcurve sense attribute by also outputing
 *                Attribute "trimcurve" "sense" "outside" or "inside".  Right
 *                now the attribute is saved, but nothing is done with it.
 *                
 * 
 *    References:
 *        [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, Richmond, CA, 
 *                  September 1989.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_TRIMSTORE_FILE
#define  _RIPRIV_FUNC_TYPES 
#include <stdlib.h>
#include <string.h>
#include <ributil.h>
#include <ribtrimstore.h>

/* 
 *  Globals used only if an ritable is given. 
 */
static PTRIMSTORE   gTrimStore = NULL;



PTRIMSTORE TrimStoreInitRITable( PRIB_RITABLE ritable, RIB_UINT32 flags )
{
   PTRIMSTORE  p = NULL;

   
   p = (PTRIMSTORE)_RibMalloc( sizeof(TRIMSTORE) );
   if (!p)
     return NULL;
   
   p->rib = NULL;
   p->flags = flags;
   p->presentlevel = 0;
   p->senseoutside = RI_FALSE;
   p->last = NULL;
   p->first = NULL;

   if ( ritable )
   {
      gTrimStore = p;

      p->ricalls[TS_ATTRIBUTE] = ritable[kRIB_ATTRIBUTE];
      p->ricalls[TS_ATTRIBUTEBEGIN] = ritable[kRIB_ATTRIBUTEBEGIN];
      p->ricalls[TS_ATTRIBUTEEND] = ritable[kRIB_ATTRIBUTEEND];
      p->ricalls[TS_BEGIN] = ritable[kRIB_BEGIN];
      p->ricalls[TS_END] = ritable[kRIB_END];
      p->ricalls[TS_FRAMEBEGIN] = ritable[kRIB_FRAMEBEGIN];
      p->ricalls[TS_FRAMEEND] = ritable[kRIB_FRAMEEND];
      p->ricalls[TS_TRIMCURVE] = ritable[kRIB_TRIMCURVE];
      p->ricalls[TS_WORLDBEGIN] = ritable[kRIB_WORLDBEGIN];
      p->ricalls[TS_WORLDEND] = ritable[kRIB_WORLDEND];

      ritable[kRIB_ATTRIBUTE] = (PRIB_RIPROC)TsAttributeV;
      ritable[kRIB_ATTRIBUTEBEGIN] = (PRIB_RIPROC)TsAttributeBegin;
      ritable[kRIB_ATTRIBUTEEND] = (PRIB_RIPROC)TsAttributeEnd;
      ritable[kRIB_BEGIN] = (PRIB_RIPROC)TsBegin;
      ritable[kRIB_END] = (PRIB_RIPROC)TsEnd;
      ritable[kRIB_FRAMEBEGIN] = (PRIB_RIPROC)TsFrameBegin;
      ritable[kRIB_FRAMEEND] = (PRIB_RIPROC)TsFrameEnd;
      ritable[kRIB_TRIMCURVE] = (PRIB_RIPROC)TsTrimCurve;
      ritable[kRIB_WORLDBEGIN] = (PRIB_RIPROC)TsWorldBegin;
      ritable[kRIB_WORLDEND] = (PRIB_RIPROC)TsWorldEnd;
   }
   else
   {
      p->flags |= TRIMSTORE_DONTFREEDATA;
   }

   return p;
}


int TrimStoreUse( PTRIMSTORE p )
{
   if (p)
   {
      gTrimStore = p;

      return 0;
   }

   return 1;
}


int TrimStoreUseRIB( PTRIMSTORE p, RIB_HANDLE rib )
{
   if (p)
   {
      gTrimStore->rib = rib;

      return 0;
   }

   return 1;
}


PTRIMSTORE TrimStoreInitRIB( RIB_HANDLE rib, RIB_UINT32 flags )
{
   PTRIMSTORE  p;


   p = TrimStoreInitRITable( ((PRIB_INSTANCE)rib)->ritable, flags );
   gTrimStore->rib = rib;

   return p;
}


int TrimStoreRestoreRITable( PTRIMSTORE p, PRIB_RITABLE ritable )
{
   if (p && ritable && gTrimStore)
   {
      ritable[kRIB_ATTRIBUTE] = p->ricalls[TS_ATTRIBUTE];
      ritable[kRIB_ATTRIBUTEBEGIN] = p->ricalls[TS_ATTRIBUTEBEGIN];
      ritable[kRIB_ATTRIBUTEEND] = p->ricalls[TS_ATTRIBUTEEND];
      ritable[kRIB_BEGIN] = p->ricalls[TS_BEGIN];
      ritable[kRIB_END] = p->ricalls[TS_END];
      ritable[kRIB_FRAMEBEGIN] = p->ricalls[TS_FRAMEBEGIN];
      ritable[kRIB_FRAMEEND] = p->ricalls[TS_FRAMEEND];
      ritable[kRIB_TRIMCURVE] = p->ricalls[TS_TRIMCURVE];
      ritable[kRIB_WORLDBEGIN] = p->ricalls[TS_WORLDBEGIN];
      ritable[kRIB_WORLDEND] = p->ricalls[TS_WORLDEND];

      return kRIB_OK;
   }

   return kRIB_ERRRC_INT;
}


int TrimStoreSetFlags( PTRIMSTORE p, RIB_UINT32 mask, RIB_UINT32 flags )
{
   if (p)
   {
      p->flags = (p->flags & ~mask) | (flags & mask);

      return kRIB_OK;
   }
   
   return kRIB_ERRRC_INT; /* Error */
}


RIB_UINT32 TrimStoreGetFlags( PTRIMSTORE p )
{
   if (p)
   {
      return p->flags;
   }
   
   return 0;
}


RtBoolean  TrimStoreIsaTrimmedSurface( PTRIMSTORE p )
{
   /* If there are any trim curves in the list that could be applied to
    *    a NURB surface, then the surface is a trimmed surface.
    */
   if (p && p->first)
   {
      return RI_TRUE;
   }

   return RI_FALSE;
}


PTRIMCURVE TrimStoreGetFirstCurve( PTRIMSTORE p )
{
   if (p)
   {
      return p->first;
   }

   return NULL;
}


PTRIMCURVE TrimStoreGetLastCurve( PTRIMSTORE p )
{
   if (p)
   {
      return p->last;
   }

   return NULL;
}


int TrimStoreBeginBlock( PTRIMSTORE p )
{
   if (p)
   {
      p->presentlevel++;
      return 0;
   }

   return 1;
}


PTRIMCURVE TrimStoreCurves( PTRIMSTORE p, 
			   RtInt nloops, 
			   RtInt ncurves[], RtInt order[], RtFloat knot[],
			   RtFloat min[], RtFloat max[], RtInt n[],
			   RtFloat u[], RtFloat v[], RtFloat w[] )
{   
   auto PTRIMCURVE  t;


   /* 
    *  When making changes to TrimStoreCurves() also make them to 
    *     TsTrimCurve() below. 
    */
   if (!p)
     return NULL; /* Error */

   t = (PTRIMCURVE)_RibMalloc(sizeof(TRIMCURVE));
   if (!t)
     return NULL; /* Error */

   t->nloops=nloops; 
   t->ncurves=ncurves; 
   t->order=order; 
   t->knot=knot;
   t->min=min; 
   t->max=max; 
   t->n=n;
   t->u=u; 
   t->v=v; 
   t->w=w;
   t->level = p->presentlevel;
   t->reverse = p->senseoutside;
   t->applied = -1;
   t->prev = p->last;
   p->last = t;
   t->next=NULL;
   if (!p->first)
   {
      p->first = t;
   }

   return t;
}


int TrimStoreApplyCurves( PTRIMSTORE p )
{
   auto PTRIMCURVE  t;


   if (!p || !p->ricalls[TS_TRIMCURVE])
     return 1;

   t = p->last;
   if ( p->flags & TRIMSTORE_APPLYALL )
   {
      while( t )
      {
	 ((PRiTrimCurve)*p->ricalls[TS_TRIMCURVE])( t->nloops, t->ncurves,
						   t->order, t->knot, 
						   t->min, t->max,
						   t->n, t->u, t->v, t->w );
	 t->applied = p->presentlevel;
	 t = t->prev;
      }      
   }
   else
   {
      /* Apply all unapplied curves on the stack. */
      while( t && t->applied<0 )
      {
	 ((PRiTrimCurve)*p->ricalls[TS_TRIMCURVE])( t->nloops, t->ncurves,
						   t->order, t->knot, 
						   t->min, t->max,
						   t->n, t->u, t->v, t->w );
	 t->applied = p->presentlevel;
	 t = t->prev;
      }
   }

   return 0;
}


int TrimStoreEndBlock( PTRIMSTORE p )
{
   auto PTRIMCURVE  t;
   auto PTRIMCURVE  tt;


   if (!p)
     return 1;
   
   /* Remove items from stack that are of the present level. */
   t = p->last;
   while( t && (t->level >= p->presentlevel) )
   {
      p->last = t->prev;

      if ( !(p->flags & TRIMSTORE_DONTFREEDATA) )
      {
	 _RibFree( t->ncurves );
	 _RibFree( t->order );
	 _RibFree( t->knot );
	 _RibFree( t->min );
	 _RibFree( t->max );
	 _RibFree( t->n );
	 _RibFree( t->u );
	 _RibFree( t->v );
	 _RibFree( t->w );
      }
      tt = t;
      _RibFree( t );

      t = tt->prev;
   }
   p->presentlevel--;
   if (p->last==NULL)
   {
      p->first = NULL;
      /* Reset senseoutside to default. */
      p->senseoutside = RI_FALSE;
   }
   else
   {
      /* Reset senseoutside to last setting. */
      p->senseoutside = p->last->reverse;

      /* Reset the level at which the curves were applied. */
      t = p->last;
      while( t && (t->applied > t->level) )
      {
	 t->applied = -1;
	 t = t->prev;
      }
   }

   return 0;
}


int TrimStoreDestroy( PTRIMSTORE p )
{
   if (!p)
     return 1;

   p->flags = 0;
   p->presentlevel = -1;
   TrimStoreEndBlock( p );
   _RibFree( p );
   gTrimStore = NULL;

   return 0;
}


RtVoid TsAttributeBegin( void )
{
   TrimStoreBeginBlock( gTrimStore );

   ((PRiAttributeBegin)*gTrimStore->ricalls[TS_ATTRIBUTEBEGIN])();
}


RtVoid TsAttributeEnd( void )
{
   TrimStoreEndBlock( gTrimStore );

   ((PRiAttributeEnd)*gTrimStore->ricalls[TS_ATTRIBUTEEND])();
}


/* Store the fact that the following attribute was active:
 *
 *     Attribute "trimcurve" "sense" "outside"
 *
 */
RtVoid TsAttributeV( RtToken name,
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   int  i;


   if (!gTrimStore)
     return;

   if (!strcmp(name,"trimcurve"))
   {
      for ( i=0; i<n; i++ )
      {
	 if (!strcmp(tokens[i],"sense"))
	 {
	    if (!strcmp((char*)parms[i],"outside"))
	    {
	       gTrimStore->senseoutside = RI_TRUE;
	    }
	    else if (!strcmp((char*)parms[i],"inside"))
	    {
	       gTrimStore->senseoutside = RI_FALSE;
	    }
	    break;
	 }
      }
   }

   if (gTrimStore->ricalls[TS_ATTRIBUTE])
   {
      ((PRiAttributeV)*gTrimStore->ricalls[TS_ATTRIBUTE])( name, 
							  n, tokens, parms );
   }
}


RtVoid TsBegin( RtToken name )
{
   TrimStoreBeginBlock( gTrimStore );

   ((PRiBegin)*gTrimStore->ricalls[TS_BEGIN])( name );
}


RtVoid TsEnd( void )
{
   TrimStoreEndBlock( gTrimStore );

   ((PRiEnd)*gTrimStore->ricalls[TS_END])();

   gTrimStore = NULL;
}


RtVoid TsFrameBegin( RtInt frame )
{
   TrimStoreBeginBlock( gTrimStore );

   ((PRiFrameBegin)*gTrimStore->ricalls[TS_FRAMEBEGIN])( frame );
}


RtVoid TsFrameEnd( void )
{
   TrimStoreEndBlock( gTrimStore );

   ((PRiFrameEnd)*gTrimStore->ricalls[TS_FRAMEEND])();
}


/*
 *  Refer to ../include/trimcurve.h for TsTrimCurve().
 */
RtVoid TsTrimCurve( RtInt nloops, 
		   RtInt ncurves[], RtInt order[], RtFloat knot[],
		   RtFloat min[], RtFloat max[], RtInt n[],
		   RtFloat u[], RtFloat v[], RtFloat w[] )
{
   auto PTRIMCURVE  t;


   /* Don't push all this back onto the stack again just to call
    *   TrimStoreCurves().  Instead make a copy of the code.
    */
   if (!gTrimStore)
     return; /* Error */

   t = (PTRIMCURVE)_RibMalloc(sizeof(TRIMCURVE));
   if (!t)
     return; /* Error */

   t->nloops=nloops; 
   t->ncurves=ncurves; 
   t->order=order; 
   t->knot=knot;
   t->min=min; 
   t->max=max; 
   t->n=n;
   t->u=u; 
   t->v=v; 
   t->w=w;
   t->level = gTrimStore->presentlevel;
   t->reverse = gTrimStore->senseoutside;
   t->applied = -1;
   t->prev = gTrimStore->last;
   gTrimStore->last = t;
   t->next=NULL;
   if (!gTrimStore->first)
   {
      gTrimStore->first = t;
   }

   if (gTrimStore->rib)
     RibKeepData( gTrimStore->rib );

   ((PRiTrimCurve)*gTrimStore->ricalls[TS_TRIMCURVE])( nloops, 
		   ncurves, order, knot, min, max, n, u, v, w );
}


RtVoid TsWorldBegin( void )
{
   TrimStoreBeginBlock( gTrimStore );

   ((PRiWorldBegin)*gTrimStore->ricalls[TS_WORLDBEGIN])();
}


RtVoid TsWorldEnd( void )
{
   TrimStoreEndBlock( gTrimStore );

   ((PRiWorldEnd)*gTrimStore->ricalls[TS_WORLDEND])();
}
