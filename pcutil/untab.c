/* 
 * Copyright (c) 1996, 1997, 1998 Thomas E. Burge.  All rights reserved.  
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
 * FILE:  untab.c
 *
 * DESCRIPTION:  Utility to remove tabs and replace with spaces.  
 *   
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void PrintHelp( void );
void PrintError( char *file );
int untab( FILE *fpin, FILE *fpout, int nspaces );
int main( int argc, char **argv );


/* Strings */
static char ErrorOutputAndEdit[] = 
"Error: Option -o and -edit both specified.\n";


int main( int argc, char **argv ) 
{
   FILE      *fpin;
   FILE      *fpout;
   char      tmpfilename[FILENAME_MAX+1];
   char      *outputfilename = NULL;
   int       nspaces=8;
   int       edit = 0;
   auto int  i,j;

   
   fpout = stdout;
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( (i+1<argc) 
             && argv[i][1]=='o' && argv[i][2]=='\0' )
         {
            i++;
            if (edit)
            {
               fprintf( stderr, ErrorOutputAndEdit );
               PrintHelp();
               return 1;       
            }
            if ( fpout == stdout )
            {
               fpout = fopen( argv[i], "wb" );
               outputfilename = argv[i];
            }
            else
            {
               fprintf( stderr, 
                       "Error: Option -o specified more than once.\n" );
               PrintHelp();
               return 1;
            }
            if (!fpout)
            {
               PrintError( argv[i] );
               return 1;
            }
         }
         else if (!strcmp(&argv[i][1],"edit"))
         {
            if (fpout!=stdout)
            {
               fprintf( stderr, ErrorOutputAndEdit );
               PrintHelp();
               return 1;       
            }            
            edit = 1;
         }
         else if (!strcmp(&argv[i][1],"t"))
         {
            i++;
            if ( i < argc )
            {
               nspaces = atoi(argv[i]);
            }
            else
            {
               fprintf( stderr, "Need number of spaces.\n" );
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

   /* Check that none of the input file names matches the output file name.
    *
    * Was doing this in the while loop that opened and worked on each file,
    *   but an error like this indicates a mistake on the command line and
    *   an input file would have already been written over by the time
    *   the error was found.  
    * So do a special loop just to check.
    */
   if ( i < argc && outputfilename )
   {
      j = i;
      while ( j < argc )
      {
         if (!strcmp(argv[j],outputfilename))
         {
            fprintf( stderr, 
                    "Output filename %s matches an input filename.\n",
                    argv[j] );
            PrintHelp();
            return 1;            
         }
         j++;
      }
   }

   if ( i < argc )
   {
      while ( i < argc )
      {
         if (edit)
         {
            strcpy( tmpfilename, argv[i] );
            j = strlen( argv[i] );
            if (j >= FILENAME_MAX )
            {
               fprintf( stderr, "Error: File name too long: %s\n", argv[i] );
               return 1;
            }
            tmpfilename[j] = '~';
            tmpfilename[j+1] = '\0';
            if ( rename(argv[i], tmpfilename) )
            {
               fprintf( stderr, "Error: Could not create back-up file: %s\n", 
                      argv[i] );
               return 1;
            }              
            fpin = fopen(tmpfilename,"rb");
            if (!fpin)
            {
               PrintError( tmpfilename );
               return 1;
            }
            fpout = fopen(argv[i],"wb");
            if (!fpout)
            {
               PrintError( argv[i] );
               return 1;
            }
            untab(fpin,fpout,nspaces);
            fclose( fpout );
         }
         else
         {
            fpin = fopen(argv[i],"rb");
            if (!fpin)
            {
               PrintError( argv[i] );
               return 1;
            }
            untab(fpin,fpout,nspaces);         
         }
         fclose(fpin);
         i++;
      }     
   }
   else
   {
      untab( stdin, fpout, nspaces );
   }

   return 0;
}


int untab( FILE *fpin, FILE *fpout, int nspaces )
{
   static char spaces[25];
   int   c;


   memset(spaces,' ',nspaces);
   spaces[nspaces]='\0';
   while ( EOF != (c=fgetc(fpin)))
   {
      if ('\t' == c)
      {
         fprintf( fpout, "%s", spaces );
      }
      else
      {
         fputc( c, fpout );
      }
   }

   return 0;
}


void PrintHelp( void )
{
   fprintf( stderr,
           "untab [-t n] [-o outputfile|-edit] [filename . . .]\n"         \
           "   [-t n]            Replace each tab with n number of\n"      \
           "                     spaces.  Default is 8.  Maximum is 25.\n" \
           "   [filename . . .]  If no file names are given then\n"        \
           "                     untab will use standard input.\n"         \
           "   [-o outputfile    File to write to.\n"                      \
           "                     If no output file name is given then\n"   \
           "                     untab will use standard output.\n"        \
           "     | -edit]        Create a temporary file of the form\n"    \
           "                     filename~ and edit the given file.\n\n" );
   fprintf( stderr,
           "   Note:  Don't use this on UNIX Makefiles.\n" );
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}
