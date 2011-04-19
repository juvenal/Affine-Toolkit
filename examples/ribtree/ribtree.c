/* 
 * Copyright (c) 1993-1998 Thomas E. Burge.  All rights reserved.
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
 * FILE:  ribtree.c
 *
 * DESCRIPTION:  Old program that creates very simplistic, toy-like trees 
 *               up to very large trees that look more like a pile of leaves
 *               with a stick under'em.
 *
 *               Provided here only because I remembered it was a cute program
 *               and plus it creates large RIB files that are good for testing
 *               a set of RIB libraries.
 *
 *        NOTE:  Ten iterations creates a 48mb RIB file -- so don't get too
 *               carried away with the number of iterations.  Six to eight
 *               iterations seems to create the better trees -- but this 
 *               varies with the hard coded numbers in the program.    
 *
 *    History: 
 *     11-05-96  The program was written before I had a RIB writer library,
 *               So printf()'s were used to create the RIB files.  Have now
 *               changed over to the Ri calls.  
 *
 *    Contains:
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ripriv.h>


/* Some C compilers have stdlib.h with TRUE and FALSE already defined. */
#ifndef TRUE
#define TRUE     1
#endif
#ifndef FALSE
#define FALSE    0
#endif
#define DEG2RAD  (3.1415927/180)

typedef int BOOLEAN;
typedef int *PBOOLEAN;

typedef struct _TREENODE {
        RtFloat           twist;
        RtFloat           angle;
        RtFloat           totalangle;
        RtFloat           length;
        RtFloat           radius;
        BOOLEAN           leaf;
        int               level;
        struct _TREENODE  *sibling;
        struct _TREENODE  *child;
        struct _TREENODE  *parent;
        struct _TREENODE  *next;
} TREENODE;
typedef TREENODE *PTREENODE;

int depth;
int procedural = 0;


int main(int argc, char **argv);
int GrowTree( PTREENODE pTreeRoot );
int PrintTreeNode( PTREENODE pTreeRoot, int angle );


int GrowTree( PTREENODE pTreeRoot )
{
   register PTREENODE  p;
   PTREENODE  pt;  /* ptr to a new tree node */
   PTREENODE  pn;  
   float  rn;      /* random number of branches */
   int    i;

   p = pTreeRoot;
   do
   {
      pn = p->next;
      p->length *= 1.1;
      p->radius *= 1.1;
      
      if (p->leaf)
      {
         rn = rand() % 4 + 2;
         p->leaf = FALSE;
           for (i=0;i<rn;i++)
           {
              pt = (PTREENODE)malloc( sizeof(TREENODE) );
              if (!pt)
              {
                 printf( "MEMORY ERROR\n" );
                 return 1;
              }
              pt->twist = (p->child 
                           ? rand() % (int)(180/rn) + 90 + p->child->twist 
                           : rand() %  359 );
              pt->angle = rand() % 35 + (p->level > 1 ? 35 : 30);
              pt->length = p->length * 0.66;
              pt->radius = p->radius * 0.44;
              pt->leaf  = TRUE;
              pt->level = p->level+1;
              pt->totalangle = pt->angle + p->angle;
              
              pt->sibling = p->child;
              pt->parent = p;
              pt->child = NULL;
              if (!p->child)
              {
                 pt->next = p->next;
              }
              else
              {
                 pt->next = pt->sibling;
              }
              
              p->child = pt;
              p->next = pt;
           }
      }
      p = pn;
   } while ( p );
   
   return 0;
}


int PrintTreeNode( PTREENODE pTreeRoot, int angle )
{
   register PTREENODE p = pTreeRoot;
   int i,depth = 0;
   RtColor brown = { 165.0/255.0, 42.0/255.0, 42.0/255.0 };
   RtColor green = { 0.0, 1.0, 0.5 };

   (void)angle;

   do
   {
      for ( ; depth < p->level; depth++ )
      {
         RiTransformBegin(); /* printf( "TransformBegin\n" ); */
      }
      
      if ( p->parent )
      {         
         /*
          * printf( "Rotate %g 0 0 1\n", p->twist );
          * printf( "Translate %g %g %g\n", 0.0, 0.0, p->parent->length );
          * printf( "Rotate %g 1 0 0\n", p->angle );                     
          */
         RiRotate( p->twist, 0.0, 0.0, 1.0 );
         RiTranslate( 0.0, 0.0, p->parent->length );
         RiRotate( p->angle, 1.0, 0.0, 0.0 );
      }
      
      if ( p->leaf)
      {
         /* 
          * printf( "Surface \"plastic\"\n" );
          * printf( "Color   [0 1 0.5]\n" );
          * printf( "Disk 0 1.5 360\n" ); 
          */ /* TO-DO: use angle for leaf's pos */

         RiSurface( "plastic", RI_NULL );
         RiColor( green );
         RiDisk( 0.0, 1.5, 360, RI_NULL );
      }
      else
      {
         /*
          * printf( "Surface \"plastic\"\n" );
          * printf( "Color [%g %g %g]\n", 165./255, 42./255., 42./255. );
          * printf( "Cylinder %g 0 %g 360\n", p->radius, p->length );
          */
         RiSurface( "plastic", RI_NULL );
         RiColor( brown );
         RiCylinder( p->radius, 0, p->length, 360, RI_NULL );
      }
      
      if (p->next)
      {
         for ( ; depth >= p->next->level; depth-- )
         {
            /* printf( "TransformEnd\n" ); */
            RiTransformEnd();
         }
      }
      p = p->next;
      
   } while ( p );
   for (i=0; i<depth; i++ )
   {
      /* printf( "TransformEnd\n" ); */
      RiTransformEnd();
   }
   
   return 0;
}


int main(int argc, char **argv) 
{
   TREENODE  root;
   int i,iterations;
   float  fov = 40.0;
   float  distantlight = 0.5;
   float  ambientlight = 0.1;
   
   root.twist    = 0.0;
   root.angle    = 0.0;
   root.length   = 12.0;
   root.radius   = 1.3;
   root.leaf     = TRUE;
   root.sibling  = NULL;
   root.child    = NULL;
   root.parent   = NULL;
   root.next     = NULL;
   root.level    = 1;

   if ( argc > 1 && (iterations=atoi(argv[1])) > 0)
   {
      /* 
       * printf( "Display \"a.tif\" \"framebuffer\" \"rgba\"\n" );
       * printf( "Format 300 300 1\nClipping 0.001 1000\n" );
       * printf( "Projection \"perspective\" \"fov\" [ 40 ]\n" );
       * printf( "Translate 0 0 80\n" );
       * printf( "WorldBegin\n" );
       * printf( "LightSource \"distantlight\" 1 \"intensity\" [0.5 ]\n" );
       * printf( "LightSource \"ambientlight\" 2 \"intensity\" [0.1 ]\n" );
       * printf( "Translate 0 -20 0\n" ); 
       * printf( "Rotate -90 1 0 0\n" );
       * printf( "Rotate 50 0 0 1\n" );
       */
      RiBegin( RI_NULL );
      RiDisplay( "a.tif", "framebuffer", "rgb", RI_NULL );
      RiFormat( 300., 300., 1. );
      RiClipping( 0.001, 1000. );
      RiProjection( "perspective", "fov", &fov, RI_NULL );
      RiTranslate( 0.0, 0.0, 80.0 );
      RiWorldBegin();
      RiLightSource( "distantlight", "intensity", &distantlight, RI_NULL );
      RiLightSource( "ambientlight", "intensity", &ambientlight, RI_NULL );
      RiTranslate( 0.0, -20.0, 0.0 ); /* move tree down into scene */
      RiRotate( -90.0, 1.0, 0.0, 0.0 );
      RiRotate( 50.0, 0.0, 0.0, 1.0 );

      srand( 234543 );
      
      for (i=0;i<iterations;i++)
        GrowTree( &root );
      
      PrintTreeNode( &root, 0 );   
      
      /* printf( "WorldEnd\n" ); */
      RiWorldEnd();

      if (procedural)
      {
         RiArchiveRecord(RI_COMMENT,"\377");
      }

      RiEnd();
   }
   else
   {
      printf( "ribtree #OfIterations\n"                                       \
"   #OfIterations  How many levels of iterations performed to make a tree.\n" \
"                  Note:  10 iterations and up make huge RIB files.  For\n"   \
"                  example \"ribtree 10\" outputs 48mb of data.\n" );
   }

   return 0;
}
