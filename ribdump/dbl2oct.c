/* $RCSfile: dbl2oct.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 07:15:30 $
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
 * FILE:  dbl2oct.c
 *
 * DESCRIPTION:  Prints double precision floating point numbers as a series 
 *               of ASCII octal numbers representing a RIB encoded double.  
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
int PrintDouble( double dbl );
int main(int argc, char **argv);

/* globals */
FILE  *fout;


int main(int argc, char **argv) 
{
   char    *outputfilename = NULL;
   double  dbl;
   int     i,n;


   fout = stdout;
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( (i+1<argc) && argv[i][1]=='o' && argv[i][2]=='\0' )
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
         else if ( isdigit(argv[i][1]) || argv[i][1]=='.' )
         {
            break;
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
         dbl = strtod( argv[i], (char**)NULL );
         PrintDouble( dbl );
         i++;
      }
   }
   else
   {
      while ( fscanf( stdin, "%lg", &dbl) == 1 )
        PrintDouble( dbl );
   }
   
   return 0;
}


void PrintHelp( void )
{
      printf( 
"dbl2oct [-o file] [double . . .]\n"                                     \
"   [-o file]        Output file name.  If not given, stdout is used.\n" \
"   [double . . .]   A number to be represented as a IEEE double-\n"     \
"                    precision number.  If no doubles are given\n"       \
"                    then dbl2oct will use standard input.\n" );
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}


int PrintDouble( double dbl )
{
   unsigned char  *v; /* value of double accessed as bytes*/
   int   i;

   /* Write out a double as a series of ASCII octal numbers. */
   /* 0245 | <eight bytes> */
   
   /* NOTE:      Encoded doubles don't appear to be used by 
    *            anybody.  Storing data as a double doesn't get 
    *            you much right now since it is going to be 
    *            converted into a 32-bit float anyway (RtFloat is of 
    *            type float as of prman 3.6).
    */
   v = (unsigned char*)(void*)&dbl;
      
   fprintf( fout, "245  " );
#ifdef LITTLE_ENDIAN         
   for ( i=(sizeof(double)-1); i > -1; i-- )
#else
   for ( i=0; i < sizeof(double); i++ )
#endif        
   {
      fprintf( fout, "%03o  ", v[i] );
   }
   fputc( '\n', fout );

   return 0;
}
