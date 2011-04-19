/* 
 * Copyright (c) 1998 Thomas E. Burge.  All rights reserved.  
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
 * FILE:  iff2iff.c
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
#include "bitmap.h"
#include "riff.h"
#include "wiff.h"


void PrintHelp( void );
int main(int argc, char **argv);


void PrintHelp( void )
{
   printf( "iff2iff [-d] [-8] iff_filename1 iff_filename2\n"                 \
	   "   -d                  Do not compress data (RLE is default).\n" \
	   "   -8                  Convert 16 bit per sample IFF to 8.\n"    \
	   "   iff_filename1       Maya IFF file to read from.\n"            \
	   "   iff_filename2       Maya IFF file to write to.\n" );
}


int main(int argc, char **argv) 
{
   PBITMAP       iff;
   int           rle = 1;
   int           i, s, t;
   unsigned int  flags = 0;


   if ( argc < 3 )
   {
      PrintHelp();
      return 1;
   }

   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
	 if ( argv[i][1]=='8' && argv[i][2]=='\0' )
	 {
	    flags = READIFF_CONVERT16TO8;
	 }
	 else if ( argv[i][1]=='d' && argv[i][2]=='\0' )
	 {
	    rle = 0;
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
   s = i;
   t = i+1;

   /* Check that the input file name doesn't match the output file name. */
   if (!strcmp(argv[s],argv[t]))
   {
      fprintf( stderr, "Filenames can not refer to the same file: \"%s\".\n", 
              argv[s] );
      return 1;            
   }

   iff = ReadIff( argv[s], flags );
   
   if (!iff)
   {
      fprintf( stderr, "Can't read %s\n", argv[s] );
      return 1;
   }

   if (WriteIff( iff, argv[t], rle ))
   {
      fprintf( stderr, "Can't write file: \"%s\"\n", argv[t] );
      DestroyBitmap( iff );
      return 1;
   }

   DestroyBitmap( iff );

   return 0;
}
