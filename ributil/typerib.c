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
 *          Copyright (c) 1995-1999 Thomas E. Burge.  
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
 * FILE:  typerib.c
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *  
 *     History:
 *      01-08-98  Added comment about double ##RenderMan hints.
 *      03-15-98  Fixed problem with multiple ##RenderMan hints.  Refer
 *                to client library sribw/write.c for fix made 03-15-98.
 *      04-26-98  Added -r flag and readarchives variable to TypeRIB().
 *      10-17-99  Sometime back had added table size to RibOpen() and
 *                now changed the constant from kRIB_RILEVEL98 to be
 *                kRIB_LAST_RI.
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
#define  _RIPRIV_FUNC_TYPES 
#include <string.h>
#include <ributil.h>

#define DEBUG 0

int main(int argc, char **argv); 
void PrintHelp( void );
void PrintError( char *file );
int TypeRIB( char *input, char* output, 
	     RtBoolean binary, RtBoolean readarchives );


int main(int argc, char **argv)
{
   char       *outputfilename = NULL;
   char       *fout = RI_NULL;
   RtBoolean  binary = RI_FALSE;
   RtBoolean  readarchives = RI_FALSE;
   int        i,n;

   
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
	 if ( (i+1<argc) && argv[i][1]=='o' && argv[i][2]=='\0' )
	 {
	    i++;
	    if ( fout == RI_NULL )
	    {
	       outputfilename = argv[i];
	       fout = argv[i];
	    }
	    else
	    {
	       printf( "Error: Option -o specified more than once.\n" );
	       goto CommandLineError;
	    }
	 }
	 else if (!strcmp(&argv[i][1],"binary"))
	 {
	    binary = RI_TRUE;
	 }
	 else if (!strcmp(&argv[i][1],"r"))
	 {
	    readarchives = RI_TRUE;
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
   
   /* Check that none of the input file names matches the output file name. */
   if ( i < argc && outputfilename )
   {
      n = i;
      while ( n < argc )
      {
	 if (!strcmp(argv[n],outputfilename))
	 {
	    fprintf( stderr, 
		    "Output filename \"%s\" matches an input filename.\n\n",
		    argv[n] );
	    goto CommandLineError;
	 }
	 n++;
      }
   }

   if ( i < argc )
   {
      while ( i < argc )
      {
	 TypeRIB( argv[i], fout, binary, readarchives );
	 i++;
      }     
   }
   else
   {
      TypeRIB( argv[i], fout, binary, readarchives );
   }

#ifdef RIB_MEMORY_TESTS
   RibMemoryTestResults( gPMemTest );
#endif
   
   return 0;
   
 CommandLineError:
   PrintHelp();
   return 1;
}


void PrintHelp( void )
{
  printf( 
"typerib [-o file] [-binary] [-r] [filename . . .]\n"                      \
"   [-o file]          Output file name.  If not given, stdout is used.\n" \
"   [-binary]          Print RIB in encoded format.\n"                     \
"   [-r]               Recursively expand ReadArchive statements.\n"       \
"   [filename . . .]   If no file names are given then typerib will\n"     \
"                      use standard input.\n" );
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}


int TypeRIB( char *input, char* output, 
	     RtBoolean binary, RtBoolean readarchives  )
{
   static RtToken    format[] = { "format" };
   static RtString   binarystr = "binary";
   static RtString*  binaryformat[] = { &binarystr };
   RIB_HANDLE  rib;
   int  err = kRIB_OK;

   rib = RibOpen( input, kRIB_LAST_RI, gRibRITable );
   if (!rib)
   {
      PrintError( input );
      return 1;
   }
   if ( readarchives )
   {
      RibSetOptions( rib, 
		     kRIB_OPTION_READ_SUBFILES, kRIB_OPTION_READ_SUBFILES );
   }
   if ( output )
   {
      if (RibBegin( rib, output ))
      {
	 PrintError( output );
	 return 1;	
      }
   }
   if (binary)
   {
      ((PRiOptionV)*gRibRITable[kRIB_OPTION])("rib", 1, format, 
					      (RtPointer*)binaryformat );
   }
#if DEBUG
   RibSetStatus( rib, kRIB_STATUS_ENCODEDDATA, kRIB_STATUS_ENCODEDDATA );
#endif
   if (RibRead(rib))
   {
      err = 1;
   }
   RibClose(rib);

   return err;
}
