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
 * FILE:  pixinfo.c
 *
 * DESCRIPTION:  Utility to print description of Alias/Wavefront pix files.
 *   
 *
 *    Contains:
 * 
 *    References:
 *
 */
#include <stdio.h>

int pixinfo( char *filename );
int main(int argc, char **argv);

int main(int argc, char **argv) 
{
   register int  i;

   if ( argc < 2 || (argc > 1 && argv[1][0]=='-') )
   {
      /* User tried to specify an option, so just print help text. */
      printf( "pixinfo pix_filename . . .\n"                          \
      "   pix_filename . . .  List of Alias/Wavefront pix files to read.\n" );
      return 1;
   }

   i = 1;
   if ( i < argc )
   {
      while ( i < argc )
        pixinfo( argv[i++] );
   }
   
   return 0;
}

int pixinfo( char *filename )
{
   FILE          *pix;
   char          b[5];
   unsigned int  xres, yres;
   int           bitsperpel;


   pix = fopen( filename, "rb" );
   if (!pix) 
   {
      fprintf( stderr, "Can't read %s\n", filename );
      return 1;
   }

   printf( "File: %s\n", filename );

   /* Read xres. */
   if ( fread( b, 1, 2, pix ) < 2 )
     goto Error;
   xres = (b[0]<<8) + b[1];

   /* Read yres. */
   if ( fread( b, 1, 2, pix ) < 2 )
     goto Error;
   yres = (b[0]<<8) + b[1];

   /* Read through unused fields. 
    * These fields are supposed to be unused, but the yoffset always seems
    *    to be set to yres-1. 
    */
   if ( fread( b, 1, 4, pix ) < 4 )
     goto Error;

   /* Read bits per pixel. */
   if ( fread( b, 1, 2, pix ) < 2 )
     goto Error;
   bitsperpel = (b[0]<<8) + b[1];

   printf( "  Image Width: %u\n  Image Length: %u\n  Bits/Pixel: %d\n", 
          xres, yres, bitsperpel );

   return 0;

 Error:
   fprintf( stderr, "Error reading %s\n", filename );
   return 1;
}
