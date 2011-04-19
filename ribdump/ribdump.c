/* $RCSfile: ribdump.c,v $  $Revision: 1.2 $ $Date: 1999/06/12 07:15:30 $
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
 * FILE:  ribdump.c
 *
 * DESCRIPTION:  Utility to print out the characters in a binary/ASCII 
 *               RIB file as a series of ASCII printed octal numbers.
 *   
 *    Contains:
 * 
 *     History:
 *       ?-?-95  Created tool to deal with debugging binary RIB code.
 *     03-04-98  Added history section and removed cut-paste error that
 *               added a reference about a page in a William Pratt book on 
 *               on signal filters -- not something typically needed for
 *               parsing a binary RIB file.  Also fixed problem where 
 *               "-offset" was being ignored when input was taken from 
 *               stdin.
 *     05-02-98  Found an extra space being printed with following test:
 *                % echo 0 0 0 0 0 0 0 360 0 0 1 0 0 0 0 0 | oct2bin | ribdump
 *                 \0   \0   \0   \0   \0   \0   \0  360   # ********
 *                 \0   \0  001   \0   \0   \0   \0   \0   #  ********
 *                %
 *               TO-DO: Not a real problem, but should see where extra 
 *                      space is from.
 *
 *    References:
 *          [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, 
 *                    Richmond, CA, September 1989.
 *          [UPST89]  Upstill, Steve, The Renderman Companion: A 
 *                    Programmer's Guide to Realistic Computer Graphics,
 *                    Addison Wesley, 1989. 
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_COLUMN 8
void PrintHelp( void );
void PrintError( char *file );
int PrintRIB( void );                    
int PrintCommentColumn( void );
int PrintString( FILE *fp );        
int PrintCharacter( char c );  
int PrintEncodedFloatingPoint( FILE *fp, int size);         
int main(int argc, char **argv);

/* globals */
int   column;
char  s[80];
char  *p;
int   OctalOnly;
FILE  *fp;
FILE  *fout;


int main(int argc, char **argv) 
{
   char  *outputfilename = NULL;
   int   offset = 0;
   int   i,n;

   
   OctalOnly = 0;
   fout = stdout;
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if ( (i+1<argc) && argv[i][1]=='o' && argv[i][2]=='\0' )
         {
            i++;
            if ( fout == stdout )
            {
               outputfilename = argv[i];
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
         else if (!strcmp(&argv[i][1],"octal"))
         {
            OctalOnly = 1;
         }
         else if (!strcmp(&argv[i][1],"offset"))
         {
            i++;
            if ( i < argc )
            {
               offset = atoi(argv[i]);
            }
            else
            {
               printf( "Need offset value.\n" );
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
   
   /* Check that none of the input file names matches the output file name. 
    *    This is just a braindead check to prevent a common command line
    *    typo.
    */
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

   if ( i < argc )
   {
      while ( i < argc )
      {
         fp = fopen(argv[i],"rb");
         if (!fp)
         {
            PrintError( argv[i] );
            return 1;
         }
         n = offset;
         while ( n && EOF!=getc(fp) )
         {
            n--;
         }
         PrintRIB();
         fclose(fp);
         i++;
      }     
   }
   else
   {
      fp = stdin;
      n = offset;
      while ( n && EOF!=getc(fp) ) /*03-04-98*/
      {
         n--;
      }
      PrintRIB();
   }
   
   return 0;
   
 CommandLineError:
   PrintHelp();
   return 1;
}


void PrintHelp( void )
{
  printf( 
"ribdump [-o file] [-octal] [filename . . .]\n"                            \
"   [-o file]          Output file name.  If not given, stdout is used.\n" \
"   [-octal]           Print octal values only (no comments).\n"           \
"   [-offset n]        Print octal values starting at zero based\n"        \
"                      offset n.\n"                                        \
"   [filename . . .]   If no file names are given then ribdump will\n"     \
"                      use standard input.\n" );
}


void PrintError( char *file )
{
   printf( "Error:  Couldn't open file: %s\n", file );
}


int PrintRIB( void )
{
#define ASCII  0
#define BIN    1
   int    lastchar=ASCII;
   int    c,i,l,w;
   int           tmp; 
   unsigned  int utmp;
   float  flt;
   
   column = 0;
   p = &s[0];
   *p = '\0';
   if (!OctalOnly) 
   {
      while ( EOF!=(c=getc(fp)) )
      {
         /* Handle binary RIB characters. */
         if (  0200 & (char)c ) /* Bit 0x80 (0200 octal) indicates binary. */
         {
            lastchar=BIN;
            /* Handle definitions of an encoded RI request. */
            /* 0314 | <code> | <string> */
            if ( 0314 == c ) 
            {            
               /* Add a space if not at the start of the comment column. */
               if (p!=s)
               {
                  *p++ = ' '; 
                  *p = '\0';
               }
               /* Print the 0314. */ 
               p += sprintf( p, "<defreq " );
               fprintf( fout, "%03o  ", c );
               column++; 
               PrintCommentColumn();        
               
               /* Get <code> character. */
               c=getc(fp); 
               
               if (EOF == c)
                 goto EndOfFile;
               
               /* Print the <code>. */   
               p += sprintf( p, "%#3o ", c );
               fprintf( fout, "%03o  ", c );
               column++;          
               PrintCommentColumn();
               /* Print the string followed by a '>'. */
               if (PrintString(fp))
                 goto EndOfFile;      
               *p++ = '>'; 
               *p = '\0';
               PrintCommentColumn();                                         
            }         
            /* Handle definitions of a string. */
            /* 0315 + w | <token> | <string>, w=(0 or 1) */
            else if (( 0315 == c ) || ( 0316 == c))
            {
               /* Add a space if not at the start of the comment column. */
               if (p!=s)
               {
                  *p++ = ' '; 
                  *p = '\0';
               }
               p += sprintf( p, "<defstr " );
               fprintf( fout, "%03o  ", c );
               column++; 
               PrintCommentColumn();           
               
               /* Read token value */
               if ( 0316 == c )
               {
                  c=getc(fp); 
                  if (EOF == c)
                    goto EndOfFile;
                  
                  fprintf( fout, "%03o  ", c );
                  column++; 
                  PrintCommentColumn();           
                  tmp = c << 8;
               }
               else 
               {            
                  tmp = 0;
               }    
               c=getc(fp); 
               if (EOF == c)
                 goto EndOfFile;
               
               fprintf( fout, "%03o  ", c );
               column++; 
               PrintCommentColumn();           
               
               if (EOF == c)
                 goto EndOfFile;
               
               tmp |= c;
               
               p += sprintf( p, "%d ", tmp );
               
               /* Print the string followed by a '>'. */
               if (PrintString(fp))
                 goto EndOfFile;        
               *p++ = '>'; 
               *p = '\0';
               PrintCommentColumn();           
            }
            /* Handle encoded strings. */
            /* 0220 + w | <string> or 0240+l | <length> | <string> */
            else if (( c >= 0220 ) && ( c <= 0243 ))
            {
               ungetc(c,fp);
               if (PrintString(fp))
                 goto EndOfFile;     
               PrintCommentColumn();           
            }
            /* Handle referencing defined string. */
            /* 0317 + w | <token>, w=(0 or 1) */
            else if (( c >= 0317 ) && ( c <= 0320 ))
            {
               w = c - 0317 + 1;
               
               /* Add a space if not at the start of the comment column. */
               if (p!=s)
               {
                  *p++ = ' '; 
                  *p = '\0';
               }
               p += sprintf( p, "<str " );
               fprintf( fout, "%03o  ", c );
               column++; 
               PrintCommentColumn();           
               
               /* Read all w number of bytes. */
               tmp = 0;
               for ( i=0; i < w; i++ )
               {
                  c = getc(fp); 
                  if (EOF == c)
                    goto EndOfFile;
                  fprintf( fout, "%03o  ", c );
                  column++; 
                  PrintCommentColumn();           
                  
                  tmp = tmp << 8;
                  tmp |= c;
               }
               p += sprintf( p, "%d>", tmp );
            }
            /* Handle referencing defined RI request. */
            /* 0246 | <code> */
            else if ( 0246 == c )
            {
               /* Add a space if not at the start of the comment column. */
               if (p!=s)
               {
                  *p++ = ' '; 
                  *p = '\0';
               }
               p += sprintf( p, "<req " );
               fprintf( fout, "%03o  ", c );
               column++; 
               PrintCommentColumn();           
               
               /* Read <code>. */
               c = getc(fp); 
               if (EOF == c)
                 goto EndOfFile;
               fprintf( fout, "%03o  ", c );
               p += sprintf( p, "%#o>", c );
               column++; 
               PrintCommentColumn();           
            }
            /* Handle integers and fixed-point numbers. */
            /* 0200 + (d*4) + w | <value> */
            else if (( c >= 0200 ) && ( c <= 0217 ))
            {
               int d;
               
               w = (int)(0x00000003 & c) + 1;
               d = (0x0000000C & c) >> 2;
               
               fprintf( fout, "%03o  ", c );
               column++; 
               PrintCommentColumn();           
               
               /* Read all bytes into tmp. */
               tmp = 0;
               for ( i=0; i < w; i++ )
               {
                  /* Check if at end of line. */
                  PrintCommentColumn();           
                  c = getc(fp); 
                  if (EOF == c)
                    goto EndOfFile;
                  fprintf( fout, "%03o  ", c );
                  column++; 
                  tmp = tmp << 8;
                  tmp |= (0xff)&c;
               }
               i = w*8;
               if  ( tmp & ( 1 << (i-1) ))
                 tmp |= 0xffffffff << i;
               
               /* Add a space if not at the start of the comment column. */
               if (p!=s)
               {
                  *p++ = ' '; 
                  *p = '\0';
               }
               /* If you don't do the following check for d being zero, 
                *    a large integer such as 99999999 prints as 1e+08 
                *    instead of 99999999.  The utility catrib prints 
                *    these large numbers out, so I'll add this to ribdump.
                */
               if ( d ) 
               {
                  flt = (double)tmp / (double)( 1 << (d * 8) );        
                  p += sprintf( p, "%g", flt );
               }
               else 
               {
                  /* If d==0 then there is nothing to the right of 
                   *    the decimal and the value is just an integer.
                   * So get the extra accuracy of using %d and not %g.
                   */
                  p += sprintf( p, "%d", tmp );
               }
               PrintCommentColumn();           
            }
            /* Handle floats. */
            /* 0244 | <four bytes> */
            else if ( 0244 == c )
            {
               /* Add a space if not at the start of the comment column. */
               if (p!=s)
               {
                  *p++ = ' '; 
                  *p = '\0';
               }
               fprintf( fout, "%03o  ", c );
               column++; 
               PrintCommentColumn();           
               if (PrintEncodedFloatingPoint( fp , sizeof(float) ))
                 goto EndOfFile;
               PrintCommentColumn();           
            }         
            /* Handle doubles. */
            /* 0245 | <eight bytes> */
            
            /* NOTE:      Encoded doubles don't appear to be used by 
             *            anybody.  (But this was tested once with a 
             *            hacked binary rib file.)
             */
            else if ( 0245 == c )
            {
               /* Storing data as a double doesn't get you much right 
                *    now since it is going to be stored as a 32-bit float
                *    anyway (RtFloat is of type float as of prman 3.6).
                */
               /* Add a space if not at the start of the comment column. */
               if (p!=s)
               {
                  *p++ = ' '; 
                  *p = '\0';
               }
               fprintf( fout, "%03o  ", c );
               column++; 
               PrintCommentColumn();           
               if (PrintEncodedFloatingPoint( fp, sizeof(double) ))
                 goto EndOfFile;
               PrintCommentColumn();           
            }         
            
            /* Handle array of floats. */
            /* 0310 + l | <length> | <array of floats> */
            else if (( c >= 0310 ) && ( c <= 0313 ))
            {
               l = c - 0310 + 1;
               
               fprintf( fout, "%03o  ", c );
               column++; 
               PrintCommentColumn();           
               
               /* Read length value */
               /* Read all l number of bytes. */
               utmp = 0;
               for ( w=0; w < l; w++ )
               {
                  c = getc(fp); 
                  if (EOF == c)
                    return 1;
                  fprintf( fout, "%03o  ", c );
                  column++; 
                  PrintCommentColumn();           
                  
                  utmp = utmp << 8;
                  utmp |= 0xff&c;
               }
               
               /* Read all <length> number of floats. */
               p += sprintf( p, " [" );
               
               for ( i=0; i < utmp; i++ )
               {
                  if (i && p!=s)
                  {
                     *p++ = ' '; 
                     *p = '\0';
                  }
                  /* Assuming all the floats in the array are encoded. */
                  if (PrintEncodedFloatingPoint( fp , sizeof(float) ))
                    goto EndOfFile;
               }
               p += sprintf( p, "]" );
               PrintCommentColumn();
            }
            else
            {            
               p += sprintf( p, "*" );
               fprintf( fout, "%3o  ", c );
               column++;
            }
         }
         else /* Handle ASCII RIB characters. */
         {
            if (lastchar!=ASCII)
            {
               /* Last character printed in the comment column
                *   was for encoded value.  So add a space before
                *   printing an ASCII character in the comment 
                *   column.
                */
               lastchar=ASCII;
               *p++=' ';
               *p = '\0';
            }
            PrintCharacter(c);
         }
         PrintCommentColumn();
      } /* end while */
   }
   else /* Print OctalOnly. */
   {
      while ( EOF!=(c=getc(fp)) )
      {        
         fprintf( fout, "%03o  ", c );
         column++;
         if (column >= MAX_COLUMN)
         {
            column = 0;
            fprintf( fout,"\n");
         }
      }
      if (column)
        fprintf( fout,"\n");
      return 0;
   }
   
 EndOfFile:
   if (column)
   {
      for (i=0; i< 5*(MAX_COLUMN - column); i++)
        fputc( ' ', fout );
      fprintf( fout, " # %s\n", s );
   }

   return 0;
}


int PrintCommentColumn( void )
{
   if (column >= MAX_COLUMN) 
   {
      if (!OctalOnly)
         fprintf( fout, " # %s\n", s );
      p = s; /* &s[0] */
      *p = '\0';
      column = 0;
   }
   return 0;
}


int PrintString( FILE *fp )
{
   int c,l,w,tmp;
   unsigned  int utmp;


   /* Handle a string (ASCII and encoded). */

   c=getc(fp); 
   
   if (EOF == c)
     return 1;

   fprintf( fout, "%03o  ", c );
   column++;

   /* Start with a quote. */
   *p++ = '\"'; 
   *p = '\0';

   PrintCommentColumn();           

   /* Handle short strings of 15 characters or less. */
   if (( c >= 0220 ) && ( c <= 0237 ))
   /* 0220 + w | <ASCII string>, w=[0..15] */
   {
      w = c - 0220;
      for ( l=w; l && !PrintCommentColumn(); l-- )
      {
         c=getc(fp); 
         if (EOF == c)
           return 1;
         PrintCharacter( c );
      }
      *p++ = '\"'; 
      *p = '\0';
      
      PrintCommentColumn();           
   }
   /* Handle long strings greater than 15 characters. */
   else if (( c >= 0240 ) && ( c <= 0243 ))
   /* 0240 + l | <length> | <ASCII string>, l=[0..3] */
   {
      /* Assign l be the number of bytes needed to specify 
       *   the length of the string.
       */
      l = c - 0240 + 1;
      
      /* Read all l number of bytes. */
      utmp = 0;
      for ( w=0; w < l; w++ )
      {
         c = getc(fp); 
         if (EOF == c)
           return 1;
         fprintf( fout, "%03o  ", c );
         column++; 
         PrintCommentColumn();           
         
         utmp = utmp << 8;
         utmp |= 0xff&c;
      }
      
      for ( w=0; w < utmp && !PrintCommentColumn(); w++ )
      {
         c=getc(fp); 
         
         if (EOF == c)
           return 1;
         
         PrintCharacter( c );
      }
      *p++ = '\"'; 
      *p = '\0';
      PrintCommentColumn();           
   }
   else if ('\"' == c)
   {
      do
      {
         tmp=c;
         c = getc(fp); 
         if (EOF==c)
           return 1;
         PrintCharacter( c );
         PrintCommentColumn();           
      }
      /* Read until we run into an ending quote
       *   that doesn't have a preceding '\' 
       *   before it.
       */
      while (c!='\"' && '\\' != tmp);
   }
   
   return 0;
}


int PrintCharacter( char c )
{
   switch (c)
   {
    case '\0':
      p += sprintf( p, "*" );
      fprintf( fout, " \\0  " );
      column++;
      break;
    case '\n':
      p += sprintf( p, "\\n" );
      fprintf( fout, " \\n  " );
      column++;
      break;
    case '\r':
      p += sprintf( p, "\\r" );
      fprintf( fout, " \\r  " );
      column++;
      break;
    case '\b':
      p += sprintf( p, "\\b" );
      fprintf( fout, " \\b  " );
      column++;
      break;
    case '\t':
      p += sprintf( p, "\\t" );
      fprintf( fout, " \\t  " );
      column++;
      break;
    case '\f':
      p += sprintf( p, "\\f" );
      fprintf( fout, " \\f  " );
      column++;
      break;
    case '\\':
      p += sprintf( p, "\\" );
      fprintf( fout, "  \\  " );
      column++;
      break;
    case '\"':
      p += sprintf( p, "\"" );
      fprintf( fout, "  \"  " );
      column++;
      break;
    case ' ':
      p += sprintf( p, " " );
      fprintf( fout, "%03o  ", c );
      column++;
      break;
    default:
      if ((c >= '!') && (c <= '~'))
      {
         *p++ = (char)c; *p = '\0';
         fprintf( fout, "  %c  ", (char)c );
         column++;
      }
      else
      {
         *p++ = '*'; *p = '\0';
         fprintf( fout, "%03o  ", c );
         column++;
      }
   }

   return 0;
}


int PrintEncodedFloatingPoint( FILE *fp, int size )
{
   char *v;
   double dbl;
   int  i,c,tmp;


   if (size==4)
   {
      /* Read float. */
      tmp = 0;
      for ( i=0; i < sizeof(float) && !PrintCommentColumn(); i++ )
      {
         c = getc(fp); 
         if (EOF == c)
           return 1;
         fprintf( fout, "%03o  ", c );
         column++; 
         tmp = tmp << 8;
         tmp |= (0xff)&c;
      }

      dbl = *( (float*)(void*)&tmp );
   }
   else
   {
      v = (char*)(void*)&dbl;
#ifdef LITTLE_ENDIAN         
      for ( i=(size-1); i > -1 && !PrintCommentColumn(); i-- )
#else
      for ( i=0; i < size && !PrintCommentColumn(); i++ )
#endif        
      {
         c = getc(fp); 
         if (EOF == c)
           return 1;
         fprintf( fout, "%03o  ", c );
         column++; 
         v[i] = (char)c;
      }
   }

   p += sprintf( p, "%g", dbl );

   return 0;
}

