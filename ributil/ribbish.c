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
 * FILE:  ribbish.c
 *
 * DESCRIPTION:  Utility to print out a RIB file while ignoring a given set 
 *               of RIB statements listed by the user.
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
 *        [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, Richmond, CA, 
 *                  September 1989.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 * 
 */
#define  _RIPRIV_FUNC_TYPES 
#include <string.h>
#include <ributil.h>
#include <ribnop.h>


int main(int argc, char **argv); 
void PrintHelp( void );
void PrintError( char *file );
int PrintRIB( char *input, char* output, RtBoolean binary );


int main(int argc, char **argv)
{
   char       *fout = RI_NULL;
   char       *outputfilename = NULL;
   RtBoolean  binary = RI_FALSE;
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
               printf( "Error: Option -o specified more than once.\n\n" );
	       goto CommandLineError;
            }
         }
         else if (!strcmp(&argv[i][1],"binary"))
         {
            binary = RI_TRUE;
         }
	 else if ( i+1==argc && argv[i][1]=='\0' )
	 {
	    /* User gave a single dash, so print help. */
	    goto CommandLineError;
	 }
	 /* Have the following string comparison as the last step. */
         else 
         {
	    n = RibGetRIFromString( &argv[i][1] );
	    if ( n < 0 )
	    {
               printf( "Error:  Unknown RIB statement or option: %s.\n\n",
		      argv[i] );
	       goto CommandLineError;
	    }
	    else
	    {
	       /* Disable Ri call being printed by hooking in RiNop call. */
	       gRibRITable[n] = gRibNopRITable[n];
	    }
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

   /* Handle the remaining arguments as filenames. */
   if ( i < argc )
   {
      while ( i < argc )
      {
         PrintRIB( argv[i], fout, binary );
         i++;
      }     
   }
   else
   {
      PrintRIB( argv[i], fout, binary );
   }
   
   return 0;
   
 CommandLineError:
   PrintHelp();
   return 1;
}


void PrintHelp( void )
{
  fprintf( stderr, 
"ribbish [-o file] [-binary] [-RIBcall1 [-RIBcall2] ...] [filename ...]\n\n"  \

" Utility to print out a RIB file without a given set of RIB statements\n"    \
" listed by the user.\n\n"                                                    \

" [-o file]                    Output file name.  If not given, stdout is\n"  \
"                              used.\n"                                       \
" [-binary]                    Print output in encoded format.\n"             \
" [-RIBcall1 [-RIBcall2] ...]  Name of RIB statement(s) to remove.\n"         \
" [filename ...]               If no file names are given then ribbish\n"     \
"                              will use standard input.\n\n" );
  fprintf( stderr, 
" Note:  Many modelers include extra RIB calls that are not needed or\n"   \
"        that represent settings that were too annoying in the GUI to\n"     \
"        actually set and change their default values.  Such extra and\n"  \
"        unwanted RIB code is \"ribbish\" that can be removed by the\n"    \
"        ribbish utility.\n" );
  fprintf( stderr, 
" Example:\n"\
"        The following copies myold.rib to mynew.rib except for the\n"\
"        TextureCoordinates and Color RIB statements:\n\n" \
"             ribbish -o mynew.rib -TextureCoordinates -Color myold.rib\n\n" );

/* Reasons for writing ribbish: 
 *
 *    For AliasToRenderman 7.X RIB files, the TextureCoordinates statements 
 *    are unwanted ribbish.  Alias seems to use some odd set of values
 *    for TextureCoordinates that I don't quite see the usefulness in.
 *
 *    Programs such as Amapi 2.03 (NT) and 2.05 (Mac), I found myself not
 *    using the program to set the color for the surfaces.  The NT version 
 *    runs only in 8-bit color depth anyway.  The end result was a bunch
 *    of Color RIB statements for varying shades of purple and green.  
 *    Basically the Color statements turned into ribbish that got in the 
 *    way of such shaders as paintedplastic.  
 *
 */
}


void PrintError( char *file )
{
   fprintf( stderr, "Error:  Couldn't open file: %s\n", file );
}


int PrintRIB( char *input, char* output, RtBoolean binary )
{
   static RtToken    format[] = { "format" };
   static RtString   binarystr = "binary";
   static RtString*  binaryformat[] = { &binarystr };
   RIB_HANDLE  rib;


   rib = RibOpen( input, kRIB_LAST_RI, gRibRITable );
   if (!rib)
   {
      PrintError( input );
      return 1;
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
      ((PRiOptionV)*gRibRITable[kRIB_OPTION])( "rib", 1, format,
					      (RtPointer*)binaryformat );
   }
   if (RibRead(rib))
   {
      RibClose(rib);
      return 1;
   }
   RibClose(rib);

   return 0;
}
