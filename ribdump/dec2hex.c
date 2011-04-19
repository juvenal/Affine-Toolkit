/* $RCSfile: dec2hex.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 07:15:30 $
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
 * FILE:  dec2hex.c
 *
 * DESCRIPTION:  Prints decimal numbers out as a series of ASCII 
 *               hex numbers.
 *   
 *    Contains:
 * 
 *    References:
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


void PrintHelp( void );
void PrintError( char *file );
int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   FILE  *fout;
   int   i, d, column;


   fout = stdout;
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( argv[i][1] && !isdigit(argv[i][1]) )
         {
            if ( (i+1<argc) 
                && argv[i][1]=='o' && argv[i][2]=='\0' )
            {
               i++;
               if ( fout == stdout )
               {
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
      }
      else
      {
         break;
      }
      i++;
   }
   
   column = 0;
   if ( i < argc )
   {
      do
      {
         sscanf( argv[i], "%d", &d );
         fprintf( fout, "%x  ", d );
         column++;
         i++;
         if (column > 7) 
         {
            fputc( '\n', fout );
            column = 0;
         }
         }
      while ( i < argc );
   }
   else
   {
      while ( fscanf( stdin, "%d", &d ) == 1 )
      {
         fprintf( fout, "%x  ", d );
         column++;
         if (column > 7) 
         {
            fputc( '\n', fout );
            column = 0;
         }
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
  printf( 
"dec2hex [-o file] [dec#] . . .\n"                                        \
"   [-o file]      Output file name.  If not given, stdout is used.\n"    \
"   [dec#]         Decimal number to be represented as a hex number.\n"   \
"                  If no file names are given then dec2hex will use\n"    \
"                  standard input.\n" );
  return;
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}
