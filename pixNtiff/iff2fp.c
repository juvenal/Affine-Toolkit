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
 * FILE:  iff2fp.c
 *
 * DESCRIPTION:  Prints the floating point values in a Maya 
 *               IFF Depth Map file.
 *
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>
#include "bitmap.h"
#include "riff.h"


void PrintHelp( void );
int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   PBITMAP  iff;
   float    *p,*pp;
   FILE     *fout = stdout;
   char     *outputfilename = NULL;
   int      i,column;

   
   if (argc!=2 && argc!=4)
   {
      PrintHelp();
      return 1;            
   }

   if ( argv[1][0]=='-' && argv[1][1]=='o' && argv[1][2]=='\0' )
   {
      if (argc!=4)
      {
         PrintHelp();
         return 1;            
      }
      outputfilename = argv[2];
      fout = fopen( outputfilename, "w" );
      if (!fout)
      {
         fprintf( stderr, "Could not open file: %s\n", outputfilename );
         return 1;
      }
      i = 3;
   }
   else
   {
      if ( argc != 2 || argv[1][0] == '-' )
      {
         PrintHelp();
         return 1;            
      }
      i = 1;
   }
   
   /* Check that the input file name doesn't match the output file name. */
   if (outputfilename && !strcmp(argv[i],outputfilename))
   {
      fprintf( stderr, "Filenames can not refer to the same file: \"%s\".\n", 
              outputfilename );
      PrintHelp();
      return 1;            
   }

   iff = ReadIff( argv[i], 0 );
   if (!iff)
   {
      fprintf( stderr, "Can't read %s\n", argv[i] );
      return 1;
   }
   iff = OrientBitmap( iff, BITMAP_TOPLEFT );

   column = 0;
   pp = p = (float*)iff->pbits;
   pp += iff->xres * iff->yres;
   while ( p < pp )
   {
      fprintf( fout, "%g  ", *p++ );
      column++;
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
   

   DestroyBitmap( iff );

   return 0;
}


void PrintHelp( void )
{
      printf( "iff2fp [-o outputfilename] iff_filename\n"                    \
             "   fp_filename       ASCII floating point file to write to.\n" \
             "                     If not given, stdout is used.\n"          \
             "   iff_filename      IFF file to read from.\n");
}
