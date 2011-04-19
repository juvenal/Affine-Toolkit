/* $RCSfile: oct2bin.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 07:15:30 $
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
 * FILE:  oct2bin.c
 *
 * DESCRIPTION:  Utility to convert octal ASCII printed numbers into a binary
 *               format.
 *   
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>


int ReadOctalNumbers( FILE *fp );
void PrintHelp( void );
void PrintError( char *file );
int main(int argc, char **argv);


/* globals */
FILE  *fout;


int main(int argc, char **argv) 
{
   char  *outputfilename = NULL;
   FILE  *fp;
   int   i,n;


   fout = stdout;
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( (i+1<argc) 
             && argv[i][1]=='o' && argv[i][2]=='\0' )
         {
            i++;
            if ( fout == stdout )
            {
               outputfilename = argv[i];
               fout = fopen( argv[i], "w" );
            }
            else
            {
               printf( "Error: Option -o specified more than once.\n" );
               PrintHelp();
               return 1;
            }
            if (!fout)
            {
               PrintError( argv[i] );
               return 1;
            }
         }
         else
         {
            PrintHelp();
            return 1;
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
            PrintHelp();
            return 1;
         }
         n++;
      }
   }

   if ( i < argc )
     while ( i < argc )
     {
        fp = fopen(argv[i],"r");
        
        if (!fp)
        {
           PrintError( argv[i] );
           return 1;
        }
        ReadOctalNumbers( fp );
        fclose( fp );
        i++;
     }
   else
   {
      fp = stdin;
      ReadOctalNumbers( fp );
   }
   
   return 0;
}


int ReadOctalNumbers( FILE *fp )
{
   int d;
   
   while ( fscanf(fp,"%o",&d) == 1 )
   {
      fputc( (char)d, fout );
   }
   return 0;
}


void PrintHelp( void )
{
   printf( 
"oct2bin [-o file] [filename . . .]\n"                                     \
"   [-o file]          Output file name.  If not given, stdout is used.\n" \
"   [filename . . .]   If no file names are given then oct2bin\n"          \
"                      will use standard input.\n" );
   return;
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}
