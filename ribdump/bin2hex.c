/* $RCSfile: bin2hex.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 07:15:30 $
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
 * FILE:  bin2hex.c
 *
 * DESCRIPTION:  Utility to print out a hex ASCII form of binary files.
 *   
 *    Contains:
 * 
 *    References:
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int ReadFile( FILE *fp, unsigned int offset );
void PrintHelp( void );
void PrintError( char *file );
int main(int argc, char **argv);

/* globals */
FILE  *fout;


int main(int argc, char **argv) 
{
   char  *outputfilename = NULL;
   FILE  *fp;
   int   i, n, offset;

   
   offset = 0;
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
         else if (!strcmp(&argv[i][1],"offset"))
         {
            i++;
            if ( i < argc )
            {
               offset = atoi(argv[i]);
            }
            else
            {
               printf( "Need offset value.\n" );
                   PrintHelp();
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
   {
      while ( i < argc )
      {
         fp = fopen(argv[i],"rb");
         if (!fp)
         {
            PrintError( argv[i] );
            return 1;
         }
         ReadFile( fp, offset );
         fclose(fp);
         i++;
      }     
   }
   else
   {
      fp = stdin;
      ReadFile( fp, offset );
   }
   
   return 0;
}


int ReadFile( FILE *fp, unsigned int offset )
{
   int   c, i, column;

   column = i = 0;
   while ( EOF!=(c=getc(fp)) )
   {
      if ( i++ >= offset )
      {
         fprintf( fout, "%02x  ", c );
         column++;
      }
      if (column > 7) 
      {
        fputc( '\n', fout );
        column = 0;
      }
   }
   if (column)
     {
       fputc( '\n', fout );
     }

   return 0;
}


void PrintHelp( void )
{
   printf( "bin2hex [-o file] [-offset] [filename . . .]\n"                \
"   [-o file]          Output file name.  If not given, stdout is used.\n" \
"   [-offset n]        Print octal values starting at zero based\n"        \
"                      offset n.\n"                                        \
"   [filename . . .]   If no file names are given then bin2hex\n"          \
"                      will use standard input.\n" );
   return;
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}

