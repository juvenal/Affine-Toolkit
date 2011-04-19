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
 * DESCRIPTION:  Prints the floating point values in an IEEE Floating 
 *               Point TIFF file.
 *
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>
#include "bitmap.h"
#include "rtiff.h"


void PrintHelp( void );
int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   PBITMAP         tiff;
   float           *f,*fl;
   unsigned char   *uc,*ucl;
   unsigned short  *us,*usl;
   FILE            *fout = stdout;
   char            *outputfilename = NULL;
   unsigned int    i,column;

   
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

   tiff = ReadTiff( argv[i], 0 );
   if (!tiff)
   {
      fprintf( stderr, "Can't read %s\n", argv[i] );
      goto Error;
   }
   if ( tiff->nsamples != 1 )
   {
      printf("\"%s\" must be a grayscale image.\n",argv[i]);
      goto Error;
   }
   tiff = OrientBitmap( tiff, BITMAP_TOPLEFT );

   if ( tiff->sampleformat == BITMAP_IEEE )
   {
      column = 0;
      f = (float*)tiff->pbits;
      fl = &f[tiff->xres * tiff->yres];
      while ( f < fl )
      {
	 fprintf( fout, "%g  ", *f++ );
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
   } else if ( tiff->sampleformat == BITMAP_UINT8 )
   {
      column = 0;
      uc = (unsigned char*)tiff->pbits;
      ucl = &uc[tiff->xres * tiff->yres];
      while ( uc < ucl )
      {
	 fprintf( fout, "%d  ", *uc++ );
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
   } else if ( tiff->sampleformat == BITMAP_UINT16 )
   {
      column = 0;
      us = (unsigned short*)tiff->pbits;
      usl = &us[tiff->xres * tiff->yres];
      while ( us < usl )
      {
	 fprintf( fout, "%d  ", *us++ );
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
   }
   else
   {
      printf("\"%s\" must be a grayscale 8, 16, or 32 bit image.\n",argv[i]);
   }

   DestroyBitmap( tiff );
   return 0;

Error:
   DestroyBitmap( tiff );
   return 1;
}


void PrintHelp( void )
{
      printf( "tiff2fp [-o outputfilename] tiff_filename\n"                  \
             "   fp_filename       ASCII floating point file to write to.\n" \
             "                     If not given, stdout is used.\n"          \
             "   tiff_filename     TIFF file to read from.\n");
}
