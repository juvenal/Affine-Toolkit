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
 * FILE:  normfptiff.c
 *
 * DESCRIPTION:  Takes an IEEE floating point TIFF file and normalizes
 *               the floating points range to be from 0 to 1.  PRMan
 *               clamps IEEE floating point texture maps to this range.
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
int NormalizeFPData( char *fin, char *fout, 
                    int ZeroMin, int ZeroOutInf, float Infinity,
                    int Verbose,
                    float Minimum, float Maximum );
int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   char   *outputfilename = NULL;
   char   *inputfilename = NULL;
   int    ZeroMin = 0;
   int    ZeroOutInf = 0;
   int    Verbose = 0;
   float  Minimum = (float)-1.0e38;
   float  Maximum = (float)1.0e38;
   float  Infinity = (float)1.0e38;
   int    i = 1;


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
         else if (!strcmp(&argv[i][1],"ZeroMin"))
         {
            ZeroMin = 1;
         }
         else if (!strcmp(&argv[i][1],"ZeroOutInf"))
         {
            ZeroOutInf = 1;
         }
         else if (!strcmp(&argv[i][1],"Verbose") 
                  || !strcmp(&argv[i][1],"v") )
         {
            Verbose = 1;
         }
         else if (!strcmp(&argv[i][1],"Minimum")
                  || !strcmp(&argv[i][1],"min"))
         {
            i++;
            if ( i>=argc || sscanf( argv[i], "%g", &Minimum )<1 )
            {
               fprintf(stderr,"Missing minimum clipping value.\n");
               goto CommandLineError;
            }
         }
         else if (!strcmp(&argv[i][1],"Infinity"))
         {
            i++;
            if ( i>=argc || sscanf( argv[i], "%g", &Infinity )<1 )
            {
               fprintf(stderr,"Missing infinity and beyond value.\n");
               goto CommandLineError;
            }
         }
         else if ( !strcmp(&argv[i][1],"Maximum")
                  || !strcmp(&argv[i][1],"max"))
         {
            i++;
            if ( i>=argc || sscanf( argv[i], "%g", &Maximum )<1 )
            {
               fprintf(stderr,"Missing maximum clipping value.\n");
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
   
   if ( i < argc && !inputfilename )
   {
      inputfilename = argv[i++];
   }
   else if ( i >= argc && !inputfilename )
   {
      fprintf( stderr, "Error:  An input filename is required.\n" );
      goto CommandLineError;
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
   
   /* Check that the input file name doesn't match the output file name. */
   if (!strcmp(inputfilename,outputfilename))
   {
      fprintf( stderr, "Filenames can not refer to the same file: \"%s\".\n", 
              inputfilename );
      return 1;            
   }
   
   NormalizeFPData( inputfilename, outputfilename,
                   ZeroMin, ZeroOutInf, Infinity,
                   Verbose, Minimum, Maximum );
   
   return 0;
   
 CommandLineError:
   PrintHelp();
   return 1;
}


void PrintHelp( void )
{
   printf( 
"normfptiff [-ZeroMin] [-ZeroOutInf] [-Infinity n] [-Verbose|-v]\n"          \
"           [-Minimum n|min n] [-Maximum n|max n]\n"                         \
"           tiff_input tiff_output\n"                                        \
"   [-ZeroMin]      Translate data range so min is zero.  This affects \n"   \
"                   the mapping of the data's values to the range 0 to 1.\n" \
"                   The default is to map 0 to 0 and map max to 1.\n"        \
"   [-ZeroOutInf]   Take all values at Inf and set them to zero.\n"          \
"   [-Infinity n]   Default Inf is 1.0e38.  Z files appear to have Inf\n"    \
"                   at 1.0E30.\n"                                            \
"   [-Verbose|-v]   Print min, max values found.\n"                          \
"   [-Minimum n     Clip data to n if it is less than or equal to n.\n"      \
"   | -min n]       Default min value is -1.0e38\n"                          \
"   [-Maximum n     Clip data to n if it is greater than or equal to n.\n"   \
"   | -max n]       Default min value is 1.0e38\n"                           \
"   tiff_input      IEEE floating point TIFF file to read from.\n"           \
"   tiff_output     IEEE floating point TIFF file to write to.\n" );
}


int NormalizeFPData( char *fin, char *fout, 
                    int ZeroMin, int ZeroOutInf, float Infinity,
                    int Verbose,
                    float Minimum, float Maximum )
{
   PBITMAP  tiff;
   float    max = Minimum;
   float    min = Maximum;
   float    maxfound;
   float    minfound;
   register float  *p,*pp;


   tiff = ReadTiff( fin, 0 );
   
   if (!tiff)
   {
      fprintf( stderr, "Can't read %s\n", fin );
      return 1;
   }
   if ( tiff->sampleformat != BITMAP_IEEE )
   {
      fprintf( stderr, "TIFF file %s is not in IEEE format.\n", fin );
      return 1;
   }

   /* Find min and max. */
   pp = p = (float*)tiff->pbits;
   pp += tiff->xres * tiff->yres;
   while ( p < pp )
   {
      if ( ZeroOutInf && *p >= Infinity )
      {
         *p++ = Infinity;
         continue;
      }
      if ( *p < Minimum )
      {
         *p = Minimum;
      }
      if ( *p < min )
      {
         min = *p;
      }
      if ( *p > Maximum )
      {
         *p = Maximum;
      }
      if ( *p > max )
      {
         max = *p;
      }
      p++;
   }

   /* For verbose option. */
   minfound = min;
   maxfound = max;

   /* If option set, translate data's range down so zero is minimum. */
   if ( ZeroMin )
   {
      p = (float*)tiff->pbits;
      while ( p < pp )
      {
         *p -= min;
         p++;
      }
      max -= min;
      min = 0.0;
   }

   /* Scale the data from the range min to max to the range 0 to 1. */
   p = (float*)tiff->pbits;
   while ( p < pp )
   {
      if ( ZeroOutInf && *p == Infinity )
      {
         *p++ = 0.0;
         continue;
      }
      *p = *p / max;
      p++;
   }

   /* Write out the data in the form of a TIFF with IEEE floats for data. */
   WriteTiff( tiff, fout );
   if (WriteTiff( tiff, fout ))
   {
      fprintf( stderr, "Can't write %s\n", fout );
      return 1;
   }

   if (Verbose)
   {
      printf( "min found: %g\n", minfound );
      printf( "max found: %g\n", maxfound );
      printf( "min: %g\n", min );
      printf( "max: %g\n", max );
   }

   DestroyBitmap( tiff );

   return 0;
}
