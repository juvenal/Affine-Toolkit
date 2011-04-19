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
 * FILE:  tiffinvert.c
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
#include "rtiff.h"
#include "wtiff.h"


void PrintHelp( void );
void PrintError( char *file );
int Orient( char *filename, int setflags, int orientation );
int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   int  orientation = 0;
   int  setflags = 0;
   int  i;

   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( argv[i][1]!='\0' && argv[i][2]=='\0' )
         {
	    switch (argv[i][1]) {
	     case 'k':
	       setflags = 1;
	       break;
	     case '0':
	     case '1':
	     case '2':
	     case '3':
	     case '4':
	     case '5':
	     case '6':
	     case '7':
	       orientation = argv[i][1] - '0';
	       break;
	     default:
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
 
   if (i==argc)
   {
      PrintHelp();
      return 1;
   }

   while ( i < argc )
   {
      Orient( argv[i], setflags, orientation );
      i++;
   }     
   
   return 0;
}


void PrintHelp( void )
{
   printf( 
"tifforient [-k] [-0|-1|-2|-3|-4|-5|-6|-7|-8] tiff_filename1 ...\n"  \
"  [-k]  Keep the image the same, just set the orientation flags.\n" \
" [-0    BITMAP_TOPLEFT,  -- Top is row 0.     Left is column 0.\n"  \
"  -1    BITMAP_TOPRIGHT, -- Top is row 0.     Right is column 0.\n" \
"  -2    BITMAP_BOTRIGHT, -- Bottom is row 0.  Right is column 0.\n" \
"  -3    BITMAP_BOTLEFT,  -- Bottom is row 0.  Left is column 0.\n"  \
"  -4    BITMAP_LEFTTOP,  -- Left is row 0.    Top is column 0.\n"   \
"  -5    BITMAP_RIGHTTOP, -- Right is row 0.   Top is column 0.\n"   \
"  -6    BITMAP_RIGHTBOT, -- Right is row 0.   Bottom is column 0.\n"\
"  -7]   BITMAP_LEFTBOT,  -- Left is row 0.    Bottom is column 0.\n"\
"By default -0 is assumed.\n"\
"  tiff_filename1 ...   TIFF file(s) to reorient.\n" );
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}


int Orient( char *filename, int setflags, int orientation )
{
   PBITMAP        tiff;


   tiff = ReadTiff( filename, 0 );
   
   if ( !tiff )
   {
      fprintf( stderr, "Can't read %s\n", filename );
      return 1;
   }
   
   if ( tiff->sampleformat!=BITMAP_UINT8 
	&& tiff->sampleformat!=BITMAP_UINT16
	&& tiff->sampleformat!=BITMAP_IEEE )
   {
      fprintf( stderr, 
            "TIFF file %s is not a \"r\", \"rgb\", \"rgba\" or IEEE image\n", 
            filename );
      goto Error;
   }
   
   if (setflags)
   {
      tiff->orientation = orientation;
   }
   else
   {
      tiff = OrientBitmap( tiff, orientation );
   }

   if (WriteTiff( tiff, filename ))
   {
      fprintf( stderr, "Can't write file: \"%s\"\n", filename );
      goto Error;
   }
   
   DestroyBitmap( tiff );
   return 0;

 Error:
   DestroyBitmap( tiff );
   return 1;
}


