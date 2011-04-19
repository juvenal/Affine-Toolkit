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
 * FILE:  tiffedge.c
 *
 * DESCRIPTION:  Do edge detection and if given more than one TIFF file
 *               combine them.
 *   
 *
 *    Contains:
 * 
 *    References:
 *          [ARVO91]   Arvo, James, Graphics GEMS II, Noise Thresholding
 *                     Edge Images by John Schlag, AP Professional, 1991,
 *                     pp. 105-106.
 *          [BAXE94]   Baxes, Gregory A., Digital Image Processing: 
 *                     Principles and Applications, John Wiley & Sons, Inc.,
 *                     1994, p. 87.
 *          [BURD97]   Burdick, Howard E., Digital Imaging: Theory and
 *                     Applications, McGraw-Hill, 1997, pp. 95-102.
 *
 *    Note:
 *       If you want to see how the threshold works, try tiffedge with
 *       atest.tif.  The gradients in that image will give you an idea
 *       how tiffedge works.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bitmap.h"
#include "rtiff.h"
#include "wtiff.h"


void PrintHelp( void );
PBITMAP DetectEdges( int n, char *filenames[], float threshold );
int Sobel8( PBITMAP original, PBITMAP target, float threshold );
int SobelIEEEFP( PBITMAP original, PBITMAP target, float threshold );
int Overlay( PBITMAP source, PBITMAP target );
int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   char     *outputfilename = NULL;
   PBITMAP  pBmp;
   int      n;
   int      i = 1;
   float    threshold = 1.0;


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
               fprintf( stderr, 
                       "Error: Option -o specified more than once.\n" );
               goto CommandLineError;
            }
         }
         else if ( (i+1<argc) && argv[i][1]=='t' && argv[i][2]=='\0' )
         {
            i++;
            if ( i < argc )
            {
               threshold = atof( argv[i] );
               if ( threshold > 1.0 || threshold < 0.0 )
               {
                  fprintf( stderr, 
                          "Error:  Value from 0.0 to 1.0 requireed.\n" );
                  goto CommandLineError;
               }
            }
            else
            {
               fprintf( stderr, 
                       "Error:  Option -t requires a numeric value.\n" );
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

   if ( !outputfilename )
   {
      fprintf( stderr, "Error:  Missing needed filenames.\n" );
      goto CommandLineError;
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
            goto CommandLineError;
         }
         n++;
      }
   }

   pBmp = DetectEdges( argc-i, &argv[i], threshold );
   if (!pBmp)
   {
      fprintf( stderr, "Error:  DetectEdges()\n" );
      return 1;
   }

   if ( WriteTiff( pBmp, outputfilename ))
   {
      fprintf( stderr, "Error:  Can't write file: \"%s\"\n", outputfilename );
      return 1;
   }

   DestroyBitmap( pBmp );

   return 0;

 CommandLineError:
   PrintHelp();
   return 1;
}


void PrintHelp( void )
{
   printf( 
"tiffedge [-t threshold] -o tiff_output tiff_input1 [tiff_input2 ...]\n" \
"   -t threshold     Threshold value from 0.0 to 1.0 indicating edge.\n" \
"                    sensitivity.  Default is 1.0\n"                     \
"   -o tiff_output   TIFF file to write to.\n"                           \
"   tiff_input1...   TIFF files to read from.\n" );
}


/* To-Do:  Add code that erodes and dilates the edges to an ink width. */
PBITMAP DetectEdges( int n, char *filenames[], float threshold )
{
   static char MemError[] = "Memory allocation error.\n";
   PBITMAP  tiff;
   PBITMAP  rgba[4];
   PBITMAP  tmp[4];
   PBITMAP  pBmp = NULL;
   unsigned char  *p, *pp, *last;   
   int  channels;
   int  i;


   while (n--)
   {
      tiff = ReadTiff( filenames[n], READTIFF_ALLOW8ONLY );
      if (!tiff)
      {
         fprintf( stderr, "Can't read file: \"%s\"\n", filenames[n] );
         return NULL;
      }

      tiff = OrientBitmap( tiff, BITMAP_TOPLEFT );

      if ( !pBmp )
      {
         /* Create target image. */
	 pBmp = CreateBitmap( tiff->xres, tiff->yres, 0, 0, 
			      1, BITMAP_UINT8, 8 );
	 if (!pBmp)
         {
	    printf( MemError );
            return NULL;
         }
         memset( pBmp->pbits, 0, pBmp->xres*pBmp->yres*sizeof(char) );
      }

      channels = tiff->nsamples;
      switch (channels)
      {
       case 4:
       case 3:
       case 1:
	 break;
       default:
         printf("Number of channels not supported: %d\n",(int)tiff->nsamples);
         return NULL;
      }

      if ( tiff->sampleformat!=BITMAP_IEEE )
      {
         /* Seperate the channels for now. */
         for ( i=0; i<channels; i++ )
         {
            /* Create a greyscale bitmap for the channel. */
	    rgba[i] = CreateBitmap( tiff->xres, tiff->yres, 0, 0, 
				    1, BITMAP_UINT8, 8 );
            if (!rgba[i])
            {
               printf( MemError );
               return NULL;
            }
	    tmp[i] = CreateBitmap( tiff->xres, tiff->yres, 0, 0, 
				   1, BITMAP_UINT8, 8 );
            if (!tmp[i])
            {
               printf( MemError );
               return NULL;
            }
            /* Copy channel to greyscale bitmap. */
            p = rgba[i]->pbits;
            pp = (tiff->pbits) + i;
            last = tiff->pbits + channels*(tiff->xres*tiff->yres);

            while ( pp < last )
            {
               *p = *pp;
               p++;
               pp += channels;
            }
         }
         
         /* Process each channel. */
         for ( i=0; i<channels; i++ )
         {
            if (Sobel8( rgba[i], tmp[i], threshold ))
            {
               printf("Error:  Sobel8()\n");
               return NULL;
            }
         }
         
         /* Overlay channels together into one image. */
         for ( i=0; i<channels; i++ )
         {
            if (i==4)
              break;
            
            if (Overlay( tmp[i], pBmp ))
            {
               printf("Error:  Overlay\n");
               return NULL;
            }
            
            DestroyBitmap( tmp[i] );
            DestroyBitmap( rgba[i] );
         }
      }
      else
      {
         /* Create target image. */
	 tmp[0] = CreateBitmap( tiff->xres, tiff->yres, 0, 0, 
				1, BITMAP_UINT8, 8 );
         if (!tmp[0])
         {
            printf( MemError );
            return NULL;
         }
         
         /* Process IEEE Floating Point values. */
         if ( SobelIEEEFP( tiff, tmp[0], threshold ) )
         {
            printf("Error:  SobelIEEEFP()\n");
            return NULL;
         }
         
         /* Overlay channels together into one image. */
         if ( Overlay( tmp[0], pBmp ) )
         {
            printf("Error:  Overlay\n");
            return NULL;
         }
            
         DestroyBitmap( tmp[0] );
      }
   }

   return pBmp;
}


#define P(a,b)  ((int)(*(p + (a) + ((b)*(original->xres)))))
int Sobel8( PBITMAP original, PBITMAP target, float threshold )
{
   int    x, y;
   int    Ix, Iy;
   int    pixel;
   int    t;
   unsigned char  *p, *pp;


   if ( !original 
       || !target
       || (original->xres != target->xres) 
       || (original->yres != target->yres)
       || (original->nsamples!=1)
       || (original->bitspersample!=8)
       || (target->nsamples!=1)
       || (target->bitspersample!=8) )
   {
      return 1; /* Error */
   }

   /* Clear target image. */
   memset( target->pbits, 0, target->xres*target->yres );

   p = original->pbits;
   pp = target->pbits;
   
   t = threshold*255;

   /* Skip first row. */
   p  += original->xres;
   pp += original->xres;

   for ( y = 1; y < original->yres - 1; y++ )
   {
      /* Skip first pixel in the row. */
      p++;
      pp++;

      /* Go through the row of pixels except for the first and last. */
      for ( x = 1; x < original->xres - 1; x++ )
      {
         /* Sobel:
          *                 | -1  0  1 |          |  1  2  1 |
          *         Ix(p) = | -2  0  2 |  Ix(p) = |  0  0  0 |
          *                 | -1  0  1 |          | -1 -2 -1 |
          *
          *         E(p) = sqrt(Ix*Ix + Iy*Iy)
          */
         Ix = -1*P(-1,-1) + /* 0*P( 0,-1)+ */ 1*P(+1,-1) + 
              -2*P(-1, 0) + /* 0*P( 0, 0)+ */ 2*P(+1, 0) + 
              -1*P(-1,+1) + /* 0*P( 0,+1)+ */ 1*P(+1,+1);

         Iy =  1*P(-1,-1) +  2*P( 0,-1) +  1*P(+1,-1) + 
            /* 0*P(-1, 0) +  0*P( 0, 0) +  0*P(+1, 0) + */
              -1*P(-1,+1) + -2*P( 0,+1) + -1*P(+1,+1);

         /* Calculate Sobel gradient. */
         pixel = sqrt( Ix*Ix + Iy*Iy );
         
         /* Kernel (or mask) weight is zero for Sobel, so don't
          *    divide pixel by a sum of the numbers in the mask.
          */

         /* Clamp pixel value from 0 to 255, inclusively. */
         if ( pixel > 255 )
         {
            pixel = 255;
         }
         else if ( pixel < t )
         {
            pixel = 0;
         }

         /* Save new pixel value. */
         *pp = pixel;

         /* Move onto next victum pixel. */
         p++;
         pp++;
      }
      /* Skip last pixel in the row. */
      p++;
      pp++;
   }

   return 0;
}


#undef P
#define P(a,b)  ((float)(*(p + (a) + ((b)*(original->xres)))))
int SobelIEEEFP( PBITMAP original, PBITMAP target, float threshold )
{
   int    x, y;
   float  Ix, Iy;
   float  pixel;
   float  *p;
   unsigned char  *pp;


   if ( !original 
       || !target
       || (original->xres != target->xres) 
       || (original->yres != target->yres)
       || (original->sampleformat!=BITMAP_IEEE)
       || (original->nsamples!=1)
       || (target->bitspersample!=32) )
   {
      return 1; /* Error */
   }

   /* Clear target image. */
   memset( target->pbits, 0, target->xres*target->yres );

   p = (float*)original->pbits;
   pp = (unsigned char*)target->pbits;

   /* Skip first row. */
   p  += original->xres;
   pp += original->xres;

   for ( y = 1; y < original->yres - 1; y++ )
   {
      /* Skip first pixel in the row. */
      p++;
      pp++;

      /* Go through the row of pixels except for the first and last. */
      for ( x = 1; x < original->xres - 1; x++ )
      {
         /* Sobel:
          *                 | -1  0  1 |          |  1  2  1 |
          *         Ix(p) = | -2  0  2 |  Ix(p) = |  0  0  0 |
          *                 | -1  0  1 |          | -1 -2 -1 |
          *
          *         E(p) = sqrt(Ix*Ix + Iy*Iy)
          */
         Ix = -1.0*P(-1,-1) + /* 0.0*P( 0,-1)+ */ 1.0*P(+1,-1) + 
              -2.0*P(-1, 0) + /* 0.0*P( 0, 0)+ */ 2.0*P(+1, 0) + 
              -1.0*P(-1,+1) + /* 0.0*P( 0,+1)+ */ 1.0*P(+1,+1);

         Iy =  1.0*P(-1,-1) +  2.0*P( 0,-1) +  1.0*P(+1,-1) + 
            /* 0.0*P(-1, 0) +  0.0*P( 0, 0) +  0.0*P(+1, 0) + */
              -1.0*P(-1,+1) + -2.0*P( 0,+1) + -1.0*P(+1,+1);

         /* Calculate Sobel gradient. */
         pixel = sqrt( Ix*Ix + Iy*Iy );
         
         /* Kernel (or mask) weight is zero for Sobel, so don't
          *    divide pixel by a sum of the numbers in the mask.
          */

         /* Clamp pixel value from 0.0 to 1.0, inclusively. */
         if ( pixel > 1.0 )
         {
            pixel = 1.0;
         }
         else if ( pixel < threshold )
         {
            pixel = 0;
         }

         /* Save new pixel value scaled from 0 to 255. */
         *pp = 255*pixel;

         /* Move onto next victum pixel. */
         p++;
         pp++;
      }
      /* Skip last pixel in the row. */
      p++;
      pp++;
   }

   return 0;
}


int Overlay( PBITMAP source, PBITMAP target )
{
   register unsigned char  *s,*ss,*t;


   if ( !source 
       || !target
       || (source->xres != target->xres) 
       || (source->yres != target->yres) 
       || (source->sampleformat!=BITMAP_UINT8)
       || (target->sampleformat!=BITMAP_UINT8) )
   {
      printf( "ERROR:  overlay().\n" );
      return 1;
   }

   ss = s = source->pbits;
   t = target->pbits;
   ss += source->xres*source->yres;
   while ( s < ss )
   {
      if ( *t < *s )
      {
         *t = *s;
      }
      t++;
      s++;
   }

   return 0;
}
