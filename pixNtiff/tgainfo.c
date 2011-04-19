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
 * FILE:  tgainfo.c
 *
 * DESCRIPTION:  Utility to print description of tga files.
 *               Additional information provided by Version 2.0 of the 
 *               TGA format are not printed.
 *
 *    Contains:
 *          int tgainfo( char *filename );
 *          int main(int argc, char **argv);
 *
 * 
 *    References:
 *          [BROW95]  Brown, C. Wayne and Shepherd, Barry J., Graphics File
 *                    Formats: Reference and Guide, Manning Publications 
 *                    Co., 1995, pp. 434-439.
 *          [KAYD95]  Kay, David C. and Levine, John R., Graphics File 
 *                    Formats: 2nd Edition, Windcrest/McGraw-Hill, 1995, 
 *                    p. 156.
 *     
 *          [MURR96]  Murray, James D. and VanRyper, William, Graphics
 *                    File Formats: Second Edition, O'Reilly & Associates, 
 *                    Inc., 1996, pp. 860-879.
 *          [TRUE91]  Truevision Inc.,  Truevision TGA: File Format 
 *                    Specification, Version 2.0, 1991.  
 *
 */
#include <stdio.h>
#include "tga.h"


int tgainfo( char *filename );
int main(int argc, char **argv);

int main(int argc, char **argv) 
{
   register int  i;

   if ( argc < 2 || (argc > 1 && argv[1][0]=='-') )
   {
      /* User tried to specify an option, so just print help text. */
      printf( "tgainfo tga_filename . . .\n"                             \
          "   tga_filename . . .  TGA format files to read.\n" );
      return 1;
   }

   i = 1;
   if ( i < argc )
   {
      while ( i < argc )
        tgainfo( argv[i++] );
   }
   
   return 0;
}


int tgainfo( char *filename )
{
   FILE          *tga;
   unsigned int  xres, yres, xorigin, yorigin;
   int           bitsperpel;
   TGAHEADER     tgaheader;
   register int  i;
   register char *s;

   tga = fopen( filename, "rb" );
   if (!tga) 
   {
      fprintf( stderr, "Can't read %s\n", filename );
      return 1;
   }

   printf( "File: %s\n", filename );

   /* Read TGA header. */
   if ( fread( (char*)&tgaheader, 1, sizeof(TGAHEADER), tga ) 
       < sizeof(TGAHEADER) )
     goto Error;
   
   /* TGA uses little-endian. */
   xres = tgaheader.width2<<8  | tgaheader.width1;
   yres = tgaheader.height2<<8 | tgaheader.height1;
   
   xorigin = tgaheader.xorigin2<<8 | tgaheader.xorigin1;
   yorigin = tgaheader.yorigin2<<8 | tgaheader.yorigin1;

   /* Allow only 16, 24 and 32 bits per pixel. */
   bitsperpel = tgaheader.pixeldepth;
   
   /* Get the image type. */
   switch (tgaheader.imagetype)
   {
    case 10: /* RLE Truecolor */
      s = "RLE Truecolor";
      break;
    case 2: /* Truecolor */
      s = "Truecolor (no RLE encoding)";
      break;
    case 0: /* No image in file */
      s = "No image in file";
      break;
    case 1: /* Colormapped */
      s = "Color mapped (no RLE encoding)";
      break;
    case 3: /* Monochrome */
      s = "Monochrome (no RLE encoding)";
      break;
    case 9: /* RLE Colormapped */
      s = "RLE Colormapped";
      break;
    case 11: /* RLE Monchrome */
      s = "RLE Monchrome";
      break;
    default:
      s = "Unknown image type";
   }
   printf( "  Image type: %s\n", s );

   printf( "  Image Width: %u\n  Image Length: %u\n  Bits/Pixel: %d\n" \
          "  X-origin: %u\n  Y-origin: %u\n",
          xres, yres, bitsperpel, xorigin, yorigin );
   
   printf( "  Palette: " );
   /* Color maps can be present even when the image doesn't need it. */
   if (tgaheader.clrmaptype==1)
   {
      printf("%d bytes\n", 
             tgaheader.clrmaplen2<<8 | tgaheader.clrmaplen1 );
   }
   else
   {
      printf("Not Given\n" );
   }

   i = (0x30 & tgaheader.origin_alphabits) >> 4;
   switch (i)
   {
    case 0:
      s = "bottom left";
      break;
    case 1:
      s = "bottom right";
      break;
    case 2:
      s = "top left";
      break;
    case 3:
      s = "top right";
      break;
   }
   printf( "  Origin: %s\n", s );

   i = 0x0f & tgaheader.origin_alphabits;
   printf( "  Alpha bits: %d\n", i );

   return 0;

 Error:
   fprintf( stderr, "Error reading %s\n", filename );
   return 1;
}
