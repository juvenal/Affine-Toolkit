/* 
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
 * FILE:  objmod.c
 *
 * NOTE TO POTENTIAL USERS:
 * 
 *       I only needed this code once and that was it.  The surfaces I wanted
 *       to move from RIB format to WaveFront OBJ format were non-periodic
 *       NuPatch surfaces created by Alias PA 7.0.1.  This code does not 
 *       handle converting periodic surfaces to obj format.  
 *
 *       This code is also not very well tested by any means, but I found it
 *       useful once upon a time.  Like ribtree.c, I'm including this code
 *       incase someone is interested -- that's it.
 *
 * DESCRIPTION:  Takes nonperiodic NuPatch surfaces defined in world space
 *               and converts them into WaveFront OBJ format.  
 *   
 *       TO-DO:  Make this into a real tool.  This code is just for someone
 *               who is interested.  It wouldn't take much to make this into
 *               a really useful tool.  
 *
 *    Contains:
 * 
 *    History:
 *        ?-?-96  Created.  
 *      10-17-99  Sometime back had added table size to RibOpen() and
 *                now changed the constant from kRIB_RILEVEL98 to be
 *                kRIB_LAST_RI.
 *
 *    References:
 *          [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, 
 *                    Richmond, CA, September 1989.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ributil.h>
#include <ribnop.h>


enum {
   PPWTBL_P,
   PPWTBL_PW,
   PPWTBL_LAST
};


void PrintHelp( void );
void PrintError( char *file );
int WriteSurf( char *file );
RtVoid AttributeBegin( void );
RtVoid AttributeV( RtToken name,
		  RtInt n, RtToken tokens[], RtPointer parms[] );
RtVoid AttributeEnd( void );
RtVoid NuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
		RtFloat umin, RtFloat umax, 
		RtInt nv, RtInt vorder, RtFloat vknot[],
		RtFloat vmin, RtFloat vmax,
		RtInt n, RtToken tokens[], RtPointer parms[] );
int main(int argc, char **argv);

/* 
 * The following table was created originally with the following command:
 *
 *            tokentbl -s ppw.asc Ppw
 */
char Ppw[] = {
    0,  1 , 'P',
    2,  2 ,'\0',      PPWTBL_P,
    0,  3 ,'w','\0',  PPWTBL_PW
};


/*
 *  Globals that are shared among callback functions.
 */
RIB_HANDLE  rib;
char *outputfilename = NULL;
FILE  *fp = NULL;
int presentlevel=0;
int NameMatchLevel=0;
char *nupatchname = NULL;
int nupatchcount = 0;


int main(int argc, char **argv) 
{
   int   i, n;
   char  *p;
   
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
	 if ( (i<argc+1) && argv[i][1]=='o' && argv[i][2]=='\0' )
	 {
	    i++;
	    outputfilename = argv[i];
	    if (strlen(outputfilename)>4)
	    {
	       p = outputfilename + strlen(outputfilename)-4;
	    }
	    else
	    {
	       p = NULL;
	    }
	    if ( !p || (p && strcmp( p, ".obj" )) )
	    {
	       printf("Warning:  " \
		      "Alias requires that WaveFront obj files have an\n"\
		      "          .obj extention.\n" );
	    }
	 }
	 else
	 {
	    goto CommandLineError;
	 }
      }
      else
      {
	 break;
      }
      i++;
   }

   /* Setup RIB table for filtering RIB files. */
   gRibNopRITable[kRIB_ATTRIBUTE] = (PRIB_RIPROC)AttributeV;
   gRibNopRITable[kRIB_ATTRIBUTEBEGIN] = (PRIB_RIPROC)AttributeBegin;
   gRibNopRITable[kRIB_ATTRIBUTEEND] = (PRIB_RIPROC)AttributeEnd;
   gRibNopRITable[kRIB_NUPATCH] = (PRIB_RIPROC)NuPatchV;

   if ( i <argc )
   {
      n = 0;
      do 
      {
	 if ( !n )
	 {
	    n = WriteSurf( argv[i] );
	 }
	 i++;
      } while ( i < argc );
   }
   else
   {
      WriteSurf( argv[i] );
   }

   return 0;
   
 CommandLineError:
   PrintHelp();
   return 1;
}


void PrintHelp( void )
{
  printf( 
	 "ribobjmod [-o output] [rib_file . . .]\n\n"                        \
         "   Takes nonperiodic NuPatch surfaces defined in world space\n"    \
         "   and converts them into WaveFront OBJ format.\n\n"               \
	 "   [rib_file . . .]   If no file names are given then ribobjmod\n"\
	 "                      will use standard input.\n" );
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}


int WriteSurf( char *file )
{
   if ( outputfilename )
   {
      fp = fopen(outputfilename,"w");
      if (!fp)
	PrintError(outputfilename);
   }
   else
   {
      fp = stdout;
   }
   rib = RibOpen( file, kRIB_LAST_RI, gRibNopRITable );
   if (!rib)
   {
      PrintError( file );
      exit(1);
   }
   if (RibRead(rib))
   {
      return 0;
   }
   RibClose(rib);
   fclose(fp);

   return 0;
}


RtVoid NuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], 
		RtFloat umin, RtFloat umax, 
		RtInt nv, RtInt vorder, RtFloat vknot[],
		RtFloat vmin, RtFloat vmax,
		RtInt n, RtToken tokens[], RtPointer parms[] )
{
   RtPointer  tokensfound[PPWTBL_LAST];
   RtBoolean  PW;
   RtFloat  *p = NULL;
   int  i;
   

   RibGetUserParameters( Ppw, PPWTBL_LAST, n, tokens, parms, tokensfound );
   if ( tokensfound[PPWTBL_P] )
   {
      PW = RI_FALSE;
      p = (RtFloat*)tokensfound[PPWTBL_P];
   }
   else if ( tokensfound[PPWTBL_PW] )
   {
      PW = RI_TRUE;
      p = (RtFloat*)tokensfound[PPWTBL_PW];
   }

   if (nupatchname)
     fprintf( fp, "g %s\n",nupatchname);
   else
     fprintf( fp, "g Obj_NuPatch#%d\n",nupatchcount );

   if (PW)
   {
      for (i=0;i<nu*nv;i++)
      {
	 fprintf( fp, "v %g %g %g\n", p[0]/p[3], p[1]/p[3], p[2]/p[3] );
	 p+=4;
      }
   }
   else
   {
      for (i=0;i<nu*nv;i++)
      {
	 fprintf( fp, "v %g %g %g\n", p[0], p[1], p[2] );
	 p+=3;
      }
   }
   /* TO-DO:  Use "cstype rat bspline" for Pw. */
   fprintf( fp, "g\ncstype bspline\ndeg %d %d\n", uorder-1, vorder-1 );
   if (nupatchname)
     fprintf( fp, "g %s\n",nupatchname);
   else
     fprintf( fp, "g Obj_NuPatch#%d\n",nupatchcount );
   fprintf( fp, "surf %g %g %g %g\\\n", umin, umax, vmin, vmax );
   for ( i=1; i<=nu*nv; i++ )
   {
      if ( !(i%28) )
	fprintf( fp, "\\\n" );
      fprintf( fp, "%d ", i );
   }
   fprintf( fp, "\nparm u " );
   for ( i=0; i < uorder+nu; i++ )
     fprintf( fp, "%g ", uknot[i] );
   fprintf( fp, "\nparm v " );
   for ( i=0; i < vorder+nv; i++ )
     fprintf( fp, "%g ", vknot[i] );
   fprintf( fp, "\nend\ng\n" );
}


RtVoid AttributeBegin( void )
{
   presentlevel++;
}


RtVoid AttributeV( RtToken name,
		  RtInt n, RtToken tokens[], RtPointer parms[] )
{
   int   i;


   if ( !strcmp(name,"identity"))
   {
      for ( i=0; i<n; i++)
      {
	 if ( !strcmp( tokens[i], "name") )
	 {
	    if (nupatchname)
	      _RibFree(nupatchname);
	    nupatchname = (char*)_RibMalloc( strlen(*(char**)parms[i]) + 1 );
	    if (!nupatchname)
	      return;
	    NameMatchLevel = presentlevel;
	    strcpy( nupatchname, *(char**)parms[i] );
	 }
      }
   }
}


RtVoid AttributeEnd( void )
{
   presentlevel--;
   if ( nupatchname && presentlevel < NameMatchLevel )
   {
      _RibFree(nupatchname);
      nupatchname = NULL;
      NameMatchLevel = 0;
   }
}
