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
 * FILE:  attrstore.c
 *
 * DESCRIPTION:  
 *   
 *    Contains:
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
#define __RIB_ATTRSTORE_FILE
#define  _RIPRIV_FUNC_TYPES 
#include <stdlib.h>
#include <string.h>
#include <ributil.h>
#include <ribattrstore.h>

/* 
 *  Globals used only if an ritable is given. 
 */
static PATTR   gAttr = NULL;



PATTR AttrInitRITable( PRIB_RITABLE ritable, RIB_UINT32 flags )
{
   PATTR  p = NULL;

   
   p = (PATTR)_RibMalloc( sizeof(ATTR) );
   if (!p)
     return NULL;
   
   p->flags = flags;
   p->presentlevel = 0;
   p->last = NULL;
   p->first = NULL;

   if ( ritable )
   {
      gAttr = p;

      p->ricalls[ATTR_ATTRIBUTE] = ritable[kRIB_ATTRIBUTE];
      p->ricalls[ATTR_ATTRIBUTEBEGIN] = ritable[kRIB_ATTRIBUTEBEGIN];
      p->ricalls[ATTR_ATTRIBUTEEND] = ritable[kRIB_ATTRIBUTEEND];
      p->ricalls[ATTR_BEGIN] = ritable[kRIB_BEGIN];
      p->ricalls[ATTR_END] = ritable[kRIB_END];
      p->ricalls[ATTR_FRAMEBEGIN] = ritable[kRIB_FRAMEBEGIN];
      p->ricalls[ATTR_FRAMEEND] = ritable[kRIB_FRAMEEND];
      p->ricalls[ATTR_WORLDBEGIN] = ritable[kRIB_WORLDBEGIN];
      p->ricalls[ATTR_WORLDEND] = ritable[kRIB_WORLDEND];

      ritable[kRIB_ATTRIBUTE] = (PRIB_RIPROC)AttrAttributeV;
      ritable[kRIB_ATTRIBUTEBEGIN] = (PRIB_RIPROC)AttrAttributeBegin;
      ritable[kRIB_ATTRIBUTEEND] = (PRIB_RIPROC)AttrAttributeEnd;
      ritable[kRIB_BEGIN] = (PRIB_RIPROC)AttrBegin;
      ritable[kRIB_END] = (PRIB_RIPROC)AttrEnd;
      ritable[kRIB_FRAMEBEGIN] = (PRIB_RIPROC)AttrFrameBegin;
      ritable[kRIB_FRAMEEND] = (PRIB_RIPROC)AttrFrameEnd;
      ritable[kRIB_WORLDBEGIN] = (PRIB_RIPROC)AttrWorldBegin;
      ritable[kRIB_WORLDEND] = (PRIB_RIPROC)AttrWorldEnd;
   }
   else
   {
      p->flags |= ATTR_DONTFREEDATA;
      memset( p->ricalls, 0, sizeof(PRIB_RIPROC)*ATTR_LAST );
   }

   return p;
}


int AttrUseRIB( PATTR p, RIB_HANDLE rib )
{
   (void)p; (void)rib;

   return 0;
}


int AttrUse( PATTR p )
{
   if (p)
   {
      gAttr = p;

      return 0;
   }

   return 1;
}


PATTR AttrInitRIB( RIB_HANDLE rib, RIB_UINT32 flags )
{
   PATTR  p;


   p = AttrInitRITable( ((PRIB_INSTANCE)rib)->ritable, flags );

   return p;
}


int AttrRestoreRITable( PATTR p, PRIB_RITABLE ritable )
{
   if (p && ritable && gAttr)
   {
      ritable[kRIB_ATTRIBUTE] = p->ricalls[ATTR_ATTRIBUTE];
      ritable[kRIB_ATTRIBUTEBEGIN] = p->ricalls[ATTR_ATTRIBUTEBEGIN];
      ritable[kRIB_ATTRIBUTEEND] = p->ricalls[ATTR_ATTRIBUTEEND];
      ritable[kRIB_BEGIN] = p->ricalls[ATTR_BEGIN];
      ritable[kRIB_END] = p->ricalls[ATTR_END];
      ritable[kRIB_FRAMEBEGIN] = p->ricalls[ATTR_FRAMEBEGIN];
      ritable[kRIB_FRAMEEND] = p->ricalls[ATTR_FRAMEEND];
      ritable[kRIB_WORLDBEGIN] = p->ricalls[ATTR_WORLDBEGIN];
      ritable[kRIB_WORLDEND] = p->ricalls[ATTR_WORLDEND];

      return kRIB_OK;
   }

   return kRIB_ERRRC_INT;
}


int AttrSetFlags( PATTR p, RIB_UINT32 mask, RIB_UINT32 flags )
{
   if (p)
   {
      p->flags = (p->flags & ~mask) | (flags & mask);

      return kRIB_OK;
   }
   
   return kRIB_ERRRC_INT; /* Error */
}


RIB_UINT32 AttrGetFlags( PATTR p )
{
   if (p)
   {
      return p->flags;
   }
   
   return 0;
}


PATTRITEM AttrGetFirstAttribute( PATTR p )
{
   if (p)
   {
      return p->first;
   }

   return NULL;
}


PATTRITEM AttrGetLastAttribute( PATTR p )
{
   if (p)
   {
      return p->last;
   }

   return NULL;
}


int AttrBeginBlock( PATTR p )
{
   if (p)
   {
      p->presentlevel++;
      return 0;
   }

   return 1;
}


int AttrEndBlock( PATTR p )
{
   auto PATTRITEM  a;
   auto PATTRITEM  aa;


   if (!p)
     return 1;
   
   /* Remove items from stack that are of the present level. */
   a = p->last;
   while( a && (a->level >= p->presentlevel) )
   {
      p->last = a->prev;

      if ( !(p->flags & ATTR_DONTFREEDATA) )
      {
	 _RibFree( a->setting );
	 _RibFree( a->property );
	 _RibFree( a->name );
      }
      aa = a;
      _RibFree( a );
      a = aa->prev;
   }
   p->presentlevel--;
   if (p->last==NULL)
   {
      p->first = NULL;
   }

   return 0;
}


int AttrDestroy( PATTR p )
{
   if (!p)
     return 1;

   p->flags = 0;
   p->presentlevel = -1;
   AttrEndBlock( p );
   _RibFree( p );
   gAttr = NULL;

   return 0;
}


RtVoid AttrAttributeBegin( void )
{
   AttrBeginBlock( gAttr );

   ((PRiAttributeBegin)*gAttr->ricalls[ATTR_ATTRIBUTEBEGIN])();
}


RtVoid AttrAttributeEnd( void )
{
   AttrEndBlock( gAttr );

   ((PRiAttributeEnd)*gAttr->ricalls[ATTR_ATTRIBUTEEND])();
}


RtVoid AttrAttributeV( RtToken name,
		    RtInt n, RtToken tokens[], RtPointer parms[] )
{
   PATTRITEM  p;
   int  i;


   if (!gAttr)
     return;

   for ( i=0; i<n; i++ )
   {
      p = (PATTRITEM)_RibMalloc(sizeof(ATTRITEM));
      if (!p)
	return;

      p->level = gAttr->presentlevel;

      p->name = (char*)_RibMalloc(strlen(name)+1);
      if(!p->name)
	return;
      strcpy( p->name, name );

      p->property = (char*)_RibMalloc(strlen(tokens[i])+1);
      if(!p->property)
      {
	 _RibFree(p->name);
	 return;
      }
      strcpy( p->property, tokens[i] );

      p->setting = (char*)_RibMalloc(strlen(parms[i])+1);
      if(!p->setting)
      {
	 _RibFree(p->property);
	 _RibFree(p->name);
	 return;
      }
      strcpy( (char*)p->setting, parms[i] );

      p->prev = gAttr->last;
      p->next = NULL;
      gAttr->last = p;
      if (!gAttr->first)
      {
	 gAttr->first = p;
      }
   }
   if (gAttr->ricalls[ATTR_ATTRIBUTE])
   {
      ((PRiAttributeV)*gAttr->ricalls[ATTR_ATTRIBUTE])( name, 
						       n, tokens, parms );
   }

   return;
}


void *AttrQueryAttribute( PATTR p, char *name, char *property )
{
   auto PATTRITEM  a;


   a = p->last;
   while( a )
   {
      if (!strcmp(a->name, name) && !strcmp(a->property, property) )
      {
	 return a->setting;
      }
      a = a->prev;
   }

   return NULL;
}


RtVoid AttrBegin( RtToken name )
{
   AttrBeginBlock( gAttr );

   ((PRiBegin)*gAttr->ricalls[ATTR_BEGIN])( name );
}


RtVoid AttrEnd( void )
{
   AttrEndBlock( gAttr );

   ((PRiEnd)*gAttr->ricalls[ATTR_END])();

   gAttr = NULL;
}


RtVoid AttrFrameBegin( RtInt frame )
{
   AttrBeginBlock( gAttr );

   ((PRiFrameBegin)*gAttr->ricalls[ATTR_FRAMEBEGIN])( frame );
}


RtVoid AttrFrameEnd( void )
{
   AttrEndBlock( gAttr );

   ((PRiFrameEnd)*gAttr->ricalls[ATTR_FRAMEEND])();
}


RtVoid AttrWorldBegin( void )
{
   AttrBeginBlock( gAttr );

   ((PRiWorldBegin)*gAttr->ricalls[ATTR_WORLDBEGIN])();
}


RtVoid AttrWorldEnd( void )
{
   AttrEndBlock( gAttr );

   ((PRiWorldEnd)*gAttr->ricalls[ATTR_WORLDEND])();
}
