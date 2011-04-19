/* $RCSfile: str2oct.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 07:15:30 $
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
 * FILE:  str2oct.c
 *
 * DESCRIPTION:  Prints a series of octal numbers in ASCII form 
 *               representing RIB encoded strings.  
 *   
 *    Contains:
 * 
 *    References:
 *          [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, 
 *                    Richmond, CA, pp. 160-165, September 1989.  
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void PrintHelp( void );
void PrintError( char *file );
char *HandleASCIIEscChar( char *s );
unsigned int PrepareString( char *s );
int PrintOctalNumbers( unsigned int l, char *s );
int PrintString( char *s );
int PrintStringDefAndRef( char *s, int token );
int main(int argc, char **argv);


/* globals */
FILE  *fout;
int   column;

int main(int argc, char **argv) 
{
   int  i,o;

   if (argc < 2)
   {
      PrintHelp();
      return 1;
   }

   column = 0;
   fout = stdout;
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( i+1<argc )
         {
            if ( argv[i][1]=='s' )
            {
               if ( argv[i][2]=='\0' )
               {
                  i++;
                  sscanf( argv[i], "%o", &o );
               }
               else if ( argv[i][2]>='0' && argv[i][2]<'8' )
               {
                  sscanf( &argv[i][2], "%o", &o );
               }
               else
               {
                  PrintHelp();
                  return 1;                  
               }
               i++;
               if ( PrintStringDefAndRef( argv[i], o ) )
               {
                  PrintHelp();
                  return 1;
               }
               if (column)
               {
                  column = 0;
                  fputc( '\n', fout );
               }
            }
            else if ( argv[i][1]=='o' && argv[i][2]=='\0' )
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
            else
            {
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
         if ( PrintString( argv[i] ) )
         {
            PrintHelp();
            return 1;
         }
         if (column)
         {
            column = 0;
            fputc( '\n', fout );
         }
      }
      i++;
   }
   
   return 0;
}


void PrintHelp( void )
{
   printf( 
"str2oct [-o file] {\"string\"|-s### \"string\"} . . .\n"                   \
"   [-o file]     Output file name.  If not given, stdout is used.\n"       \
"   string        An ASCII string to encode.  If you need to start the\n"   \
"                 string with a '-' which normally marks a command line\n"  \
"                 option, simply place a '\\' in front.  For example the\n" \
"                 to encode the string \"-supposedstring\" type\n"          \
"                            str2oct \"\\-supposedstring\"\n"               \
"                 The RenderMan Spec states that a '\\' not marking the\n"  \
"                 characters 'n', 'r', 't', 'b', 'f', '\\', \", 0-7, or\n"  \
"                 a newline character is simply ignored.\n"                 \
"   -s### string  Use a definition and reference encoding with ### as\n"    \
"                 the octal string token.\n" ); 
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}


char *HandleASCIIEscChar( char *s )
{
   static struct esctable {
      char ascii;
      char bin;
   } t[] = { 
      { 'n', '\n' }, 
      { 'r', '\r' },
      { 't', '\t' },
      { 'b', '\b' },
      { 'f', '\f' }
   };
   auto unsigned int  u;
   register int  c,i;
   register char  *p;

   
   p = s;
   
   /* Skip over the back-slash that caused this function to be called. */
   p++;
   
   c = *p;
   
   for (i=0; i <sizeof(t)/sizeof(struct esctable); i++)
   {
      if ( c == t[i].ascii )
      {
         /* Replace character following back-slash with encoded character. */
         *p = t[i].bin; 
         return p;
      }
   }
   
   switch (c)
   {
    case '\\': /* backslash */
    case '\"': /* double quote */
      /* The character we want is already there. */
      break;
    case '\n': /* continuation to next line was marked. */
      /* Simply ignore '\n'. */
      p++; /* Skip over the '\n'. */
      break;
    default:
      /* Handle octal character code. */
      if ( c >= '0' && c <= '7' )
      {
         /* The catrib utility when given "\0065" will include the '5'.
          * It appears that if the digit in an ASCII octal number following
          * a backslash is a zero then the octal number is allowed to have
          * upto four digits not just three.
          */
         u = c - '0';
         for ( i=(u==0 ? -1 : 0); i < 2; i++ )
         {
            c = p[1]; /* Look ahead one character. */
            if ( c >= '0' && c <= '7' )
            {
               p++;
               c -= '0';
               u = (u << 3) | c;         
            }
            else
            {
               break;
            }
         }
         /* Replace string's first char with binary version of octal number. */
         *p = (char)u; 
      }
      /* else
       * {
       *  // No escape character sequence found, so ignore the slash
       *  //   mark that caused this function to be called.
       *  //
       * }
       */
      break;
   }

   return p;
} 


unsigned int PrepareString( char *s )
{
   register char  *p, *pp;
   register unsigned int   l = 0;
   
   l = 0;
   p = s;
   while (*p)
   {
      if (*p=='\\')
      {
         pp = p;
         p = HandleASCIIEscChar( p );
         *pp++=*p++;
         l++;
         while (*p)
         {
            if (*p=='\\')
            {
               p = HandleASCIIEscChar( p );
            }
            *pp++=*p++;
            l++;
         }
         break;
      }
      l++;
      p++;
   }
   
   return l;
}


int PrintOctalNumbers( unsigned int l, char *s )
{
   auto char  *p;
   auto unsigned int  n;

   n = l;
   p = s;
   while ( n-- )
   {
      fprintf( fout, "%03o  ", *p++ );
      column++;
      if (column > 7) 
      {
         fputc( '\n', fout );
         column = 0;
      }
   }

   return 0;
}


int PrintString( char *s )
{
   static char b[6];
   int  n;
   unsigned int  length;
   auto unsigned int  l;
   auto unsigned int  m;


   length = PrepareString( s );

   if ( length < 16 ) 
   {
      b[0] = 0220 + length;
      n = 1;
   }
   else
   {
      if (length & 0xff000000)
      {
         l = 3;
      }
      else if (length & 0x00ff0000)
      {
         l = 2;
      }
      else if (length & 0x0000ff00)
      {
         l = 1;
      }
      else
      {
         l = 0;
      }
      n = 1;
      b[0] = 0240 + l;

      /* l=3 --> shift mask right  0 bits
       * l=2 --> shift mask right  8 bits
       * l=1 --> shift mask right 16 bits
       * l=0 --> shift mask right 24 bits
       */
      l++; /* Switch from being zero based to minimum of one. */
      n+=l;
      m = length;
      while (l)
      {
         b[l] = m  & 0x000000ff;
         m = (m >> 8);
         l--;
      }
   }
   PrintOctalNumbers( n, b );
   PrintOctalNumbers( length, s );

   return 0;
}


int PrintStringDefAndRef( char *s, int token )
{
   static char   b[3];
   int   n;


   if ( token <= 0 )
     return 1;

   if ( token & 0xff00 )
   {
      b[0] = 0316;
      b[1] = (token & 0xff00) >> 8;
      b[2] = token & 0x00ff;
      n = 3;
   }
   else
   {
      b[0] = 0315;
      b[1] = token & 0x00ff;
      n = 2;
   }
   PrintOctalNumbers( n, b ); /* Define string. */

   PrintString( s ); 

   b[0] = b[0] + 2; /* 0315 -> 0317 and 0316 -> 0320 */
   PrintOctalNumbers( n, b ); /* Reference string. */

   return 0;
}
