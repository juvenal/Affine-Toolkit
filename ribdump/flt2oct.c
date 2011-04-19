/* $RCSfile: flt2oct.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 07:15:30 $
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
 * FILE:  flt2oct.c
 *
 * DESCRIPTION:  Prints foating point numbers as a series of  
 *               ASCII octal numbers representing a RIB encoded float  
 *               or integer/fixed-point.  
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
#include <math.h>
#include <float.h>


void PrintHelp( void );
void PrintError( char *file );
int PrintFloat( float flt, int ieee );
int GetIntegerFixedPoint( float flt, unsigned char *v );
int WriteOutIntegerFixedPoint( unsigned char *v );
int WriteOutIEEEFloat( float flt );
int main(int argc, char **argv);

/* globals */
FILE  *fout;


int main(int argc, char **argv) 
{
   int     ieee = 0;
   float   flt;
   int     i;


   fout = stdout;
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( (i+1<argc) 
             && argv[i][1]=='o' && argv[i][2]=='\0' )
         {
            i++;
            if ( fout == stdout )
            {
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
         else if ( !strcmp(&argv[i][1],"IEEE") 
                  || !strcmp(&argv[i][1],"ieee")
                  || !strcmp(&argv[i][1],"i") )
         {
            ieee = 1;
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
   
   if ( i < argc )
   {
      while ( i < argc )
      {
         flt = atof( argv[i] );
         PrintFloat( flt, ieee );
         i++;
      }
   }
   else
   {
      while ( scanf("%g", &flt) == 1 )
        PrintFloat( flt, ieee );
   }

   return 0;
}


void PrintHelp( void )
{
  printf( 
"flt2oct [-o file] [-IEEE|-ieee|-i] [float . . .]\n"                        \
"   [-o file]         Output file name.  If not given, stdout is used.\n"   \
"   [-IEEE|-ieee|-i]  Use only IEEE encoding.\n"                            \
"   [float . . .]     A number to be represented as an IEEE single-\n"      \
"                     precision or fixed point number.  If no floats are\n" \
"                     given then flt2oct will use standard input.\n" );
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}


int PrintFloat( float flt, int ieee )
{
   /* The array v is the value of flt represented as a series of 
    *    encoded bytes. 
    */
   unsigned char  v[5]; 

   if ( !ieee && !GetIntegerFixedPoint( flt, v ))
   {
      WriteOutIntegerFixedPoint( v );
   }
   else
   {
      WriteOutIEEEFloat( flt );
   }

   fputc( '\n', fout );

   return 0;
}


int GetIntegerFixedPoint( float flt, unsigned char *v )
{
   int  exponentbase2;
   int  fraction;
   int  tmp;
   register int  d,w;

   /* Convert float to integer/fixed-point format. */
   /* 0200 + (d*4) + w | <value> */

   tmp = *((unsigned int *)(void *)&flt);

   exponentbase2 = (tmp & 0x7F800000) >> 23; /* Biased exponent. */

   /* Check the biased exponent to see if the number is too large 
    *    or too small to be represented in integer/fixed-point.
    */
   if (exponentbase2 > 141 || exponentbase2 < 122)
     return 1;/* The 1 signals that IEEE Single Prec. format should be used.*/

   exponentbase2 -= 127; /* Get unbiased exponent */

   fraction = (tmp & 0x007fffff) | 0x00800000; /* OR in the implied 1.0 */

   /* Handle sign bit. */
   if ( tmp & 0x80000000 ) /* Check sign bit. */
      fraction = ~fraction + 1;
     
   w = exponentbase2;
   if ( exponentbase2 < 0 )
   {
      w = (w+9) % 8;
   }
   else
   {
      w = (w+1) % 8;
   }

   if ( exponentbase2 < 0 )
   {
      d = (exponentbase2==-1 ? -1 : 0);
   }
   else
   {
      d = -( ((exponentbase2+1) >> 3) + 1 );
   }

   fraction = fraction << w;

   if ( fraction & 0x80000000 )
   {
      tmp = 0xffffffff;
   }
   else
   {
      tmp = 0;
   }

   w = 0;
   while ( (fraction && d < 3) || (d<0) )
   {
      w++;
      d++;

      v[w] = ( fraction & 0xff000000 ) >> 24;

      tmp = tmp<<8 | v[w];

      fraction = fraction << 8;

      /* If it takes four bytes, use IEEE single precision float. */
      if (w>3)
        return 1;
   }
      
   v[0] = 0200 + d*4 + (w-1);

   return 0;
}


int WriteOutIntegerFixedPoint( unsigned char *v )
{
   register int  i,w1;
   
   /* Get w+1. */
   w1 = ( v[0]&0x0003 ) + 2;
   for ( i=0; i < w1; i++ )
     fprintf( fout, "%03o  ", v[i] );

   return 0;
}


int WriteOutIEEEFloat( float flt )
{
   int  tmp;


   /* Write out a float as a series of ASCII octal numbers. */
   /* 0244 | <four bytes> */   

   tmp = *((unsigned int *)(void *)&flt);

   fprintf( fout, "244  %03o  %03o  %03o  %03o  ", 
          (tmp>>24)&0x000000ff, (tmp>>16)&0x000000ff,
          (tmp>>8)&0x000000ff,  tmp&0x000000ff );

   return 0;
}
