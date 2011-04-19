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
 * FILE:  tiff2fp.c
 *
 * DESCRIPTION:  Test application.
 *   
 *
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.h"
#include "wtiff.h"


void PrintHelp( void );
void PrintError( char *file );
int ConvertToTiff( FILE *fp, char *outputfilename, int xres, int yres );
int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   FILE     *fp;
   char     *outputfilename = NULL;
   int      xres = 0;
   int      yres = 0;
   int      i,n;


   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( (i+1<argc) && argv[i][1]=='o' && argv[i][2]=='\0' )
         {
            i++;
            if ( !outputfilename )
            {
               outputfilename = argv[i];
            }
            else
            {
               printf( "Error: Option -o specified more than once.\n" );
               PrintHelp();
               return 1;
            }
         }
         else if (!strcmp(&argv[i][1],"x"))
         {
            if (xres)
            {
               printf( "Error:  X resolution specified more than once.\n" );
               PrintHelp();
               return 1;
            }
            i++;
            xres = atoi(argv[i]);
            if (!xres)
            {
               printf( "Error:  " \
                      "Expected non-zero integer value for X resolution.\n" );
               goto CommandLineError;
            }
         }
         else if (!strcmp(&argv[i][1],"y"))
         {
            if (yres)
            {
               printf( "Error:  Y resolution specified more than once.\n" );
               goto CommandLineError;
            }
            i++;
            yres = atoi(argv[i]);
            if (!yres)
            {
               printf( "Error:  " \
                      "Expected non-zero integer value for Y resolution.\n" );
               goto CommandLineError;
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

   if ( i < argc && !outputfilename )
   {
      outputfilename = argv[i++];
   }
   else if ( i >= argc && !outputfilename )
   {
      fprintf( stderr, "Error:  An output filename is required.\n" );
      goto CommandLineError;
   }

   /* Check that the resolution was given. */
   if ( !(xres && yres) )
   {
      fprintf( stderr, "Resolution of TIFF required,\n" );
      goto CommandLineError;
   }
   
   /* Check that none of the input file names matches the output file name. */
   if ( i < argc )
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
         fp = fopen(argv[i],"rb");
         if (!fp)
         {
            PrintError( argv[i] );
            return 1;
         }
         ConvertToTiff( fp, outputfilename, xres, yres );
         fclose(fp);
         i++;
      }     
   }
   else
   {
      fp = stdin;
      ConvertToTiff( fp, outputfilename, xres, yres );
   }
   
   return 0;
   
 CommandLineError:
   PrintHelp();
   return 1;
}


void PrintHelp( void )
{
  printf( 
"fp2tiff -x X -y Y tiff_filename [fp_inputfile1 ...]\n"                      \
"   -x X                  Resulting TIFF file is X pixels across.\n"         \
"   -y Y                  Resulting TIFF file is Y pixels high.\n"           \
"   tiff_filename         TIFF file to write to.\n"                          \
"   [fp_inputfile1 ...]   Input files that contain ASCII written floating\n" \
"                         point values.  If no names are given then stdin\n" \
"                         is used.\n" );
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}


int ConvertToTiff( FILE *fp, char *outputfilename, int xres, int yres )
{
   PBITMAP  pBmp;
   float    *p,*pp;


   pBmp = CreateBitmap( xres, yres, 0, 0, 1, BITMAP_IEEE, 32 );
   if(!pBmp)
   {
      printf( "Memory allocation error.\n" );
      return NULL;
   }

   pp = p = (float*)pBmp->pbits;
   pp += xres * yres;
   while ( p < pp )
   {
      if (fscanf( fp, "%g  ", p++ ) < 1)
        return 1;
   }

   WriteTiff( pBmp, outputfilename );

   DestroyBitmap( pBmp );

   return 0;
}
