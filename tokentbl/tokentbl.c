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
 * FILE:  tokentbl.c
 *
 * DESCRIPTION:  
 *    
 *    Quick and dirty table generator for the functions:
 *       RibReadFileForToken( RIB_HANDLE rib, char *table ),
 *       RibReadStringForToken( char *s, char *table, char **tokenend ), and
 *       RibGetUserParameters( char *table, int ntable,
 *                            RtInt n, RtToken tokens[], RtPointer parms[], 
 *                            RtPointer output[] ) functions.
 *
 *    Originally I wasn't planning on publishing this utility, but I started
 *    using it too much in the toolkit.  Some folks might find such 
 *    functions as RibGetUserParameters() to be useful and the table
 *    parameter refers to the output created by this program.  
 *
 *    I have tried to give this code a good once over, but it still is rather
 *    ugly.  It really is just a quick tool, so don't stress test it with
 *    wild test cases.  And please do not find bugs with this thing, as that
 *    would mean that I would have to figure it out again and I have long
 *    since forgotten most of it.
 *
 *    USING THIS TOOL
 *    ---------------
 *    This tool creates a table that can be used for finding tokens in a
 *    string or file reasonably quickly.  It is not a great lexer, but it's
 *    simplicity and ability to to have code that does error checking
 *    as data is read prompted it's use in the toolkit.  
 *
 *    You first start by listing a set of tokens you want a function such
 *    as RibReadStringForToken() to identify such as the following list in
 *    the file fruits.asc:
 *
 *                      Apples
 *                      Oranges
 *                      Peaches
 *                      Pears
 *
 *    These didn't have to be listed alphabetically, but they do have to
 *    be listed one string per line and with strings grouped together
 *    that have the same prefixing substring.
 *
 *    Inshort Pear and Peaches have to be listed together because of the
 *    commond string "Pea" they each start with.  They could be listed
 *    as Peaches followed by Pears or Pears followed by Peaches.
 *
 *    Using the command "tokentbl -s fruits.asc Fruits" the following 
 *    table is created:
 *               char Fruits[] = {
 *                   8,  8 ,'A','p','p','l','e','s','\0',  0,
 *                   9,  9 ,'O','r','a','n','g','e','s','\0',  1,
 *                   0,  3 ,'P', 'e', 'a',
 *                   6,  6 ,'c','h','e','s','\0',  2,
 *                   0,  4 ,'r','s','\0',  3,
 *               };
 *
 *    The table basically describes a tree structure.  The first number
 *    of each line gives an offset to branch from the same node.  The
 *    second number on each line gives the starting offset to a child 
 *    node.    
 *
 *    In the above example, the first token "Apple" is given the ID 
 *    number 0 and the other tokens follow in ascending order: 
 *    Oranges(1), Pears(2) and Peaches(3).
 *
 *    Without the "-s" option the items in fruits.asc would be listed in
 *    separate tables depending on the first letter.  Using the command 
 *    "tokentbl fruits.asc Fruits" the following tables were created:
 * 
 *               char Fruits_A[] = {
 *                   0,  7 ,'p','p','l','e','s','\0',  0,
 *               };
 *               char Fruits_O[] = {
 *                   0,  8 ,'r','a','n','g','e','s','\0',  1,
 *               };
 *               
 *               char Fruits_P[] = {
 *                   0,  2 , 'e', 'a',
 *                   6,  6 ,'c','h','e','s','\0',  2,
 *                   0,  4 ,'r','s','\0',  3,
 *               };
 *               char *Fruits_table[] = { Fruits_A, Fruits_O, Fruits_P };
 *    
 *    The above output is useful for large sets of tokens.  Refer to 
 *    affine/readrib/ribread.c and affine/readrib/ribtbl.c for an example 
 *    of how these types of tables can be used.
 *
 *    WHAT DO THE NUMBERS MEAN?
 *    -------------------------
 *    We'll refer to the last two tables entries from the first example:
 *
 *                   0,  3 ,'P', 'e', 'a',
 *                   6,  6 ,'c','h','e','s','\0',  2,
 *                   0,  4 ,'r','s','\0',  3,
 *
 *    Each line has the format of starting with two numbers followed by a 
 *    list of characters and optional ID number if the last character 
 *    is a NULL.  The first number is the offset from the first letter 
 *    needed to move onto the next possible matching prefix.  The second 
 *    number is the offset from the first character to the next possible 
 *    continuation of the string after the rest of the characters on the
 *    line have been matched.  
 *
 *    In the above example the common prefix ("Pea") to Pears and Peaches has
 *    been separated from the two strings and placed on the first line.  The
 *    number 0 indicates no other possible prefixes are given.  If the string
 *    being examined does not start with 'P', 'e' and then an 'a' the number 0
 *    indicates no match is possible.   The second number of the first line
 *    is a three.  Since it is not zero, there are continuations of the 
 *    string "Pea" that must be examined before a match is found.  A non-zero 
 *    second value indicates that no ID number will be given on this line.  
 *
 *    The 3 gives the offset from the first character 'P' to the next line.
 *    The next line's first number 6 indicates the number of characters to 
 *    the next possible substring.  When the second line is reached, the 
 *    letters 'P', 'e' and 'a' have been found to match the first three
 *    characters of the string being examined.  Once "Pea" has been checked 
 *    the 6 starting the second line indictes that more than one possible
 *    substring can be matched.  
 *
 *
 *    Contains:
 *  
 *    History:
 *       12-11-97  Tiny comment made in above description to match html
 *                 file.
 *
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>

#define MAXBUFFER 2

int main(int argc, char **argv);
int ReadLine( FILE *fp, int n, char *s );
int SetUpTree( char t[255][255], int BaseIndex, int UpTo, int InToString, 
	      char *Output );
int PrintTree( char *T, int i );


int main(int argc, char **argv) 
{
   static char  var[255];
   static int   vars;
   static char  s[255];
   static char  d[255];
   static char  t[255][255];
   FILE  *fp;
   int   NameIndex = 0;
   int   BaseIndex = 0;
   char  *TableName;
   int   StandAlone = 0;
   int   i = 1;
   char  c;

   if ( argc < 3 || argc > 4)
   {
      printf( "tabletbl [-s] file tablename\n" );
      return 1;
   }

   if ( !strcmp("-s",argv[1]) )
   {
      StandAlone = 1;
      i++;
   }

   TableName = argv[i+1];

   fp = fopen(argv[i],"r");

   if (!fp)
   {
      printf( "Can't open %s\n", argv[i] );
      return 1;
   }

   if (EOF==ReadLine(fp,255,s))
   {
      return 1;
   }
   c = s[0];	
   vars = 0;

   if (!StandAlone)
   {   
      do
      {
	 if ( c != s[0] )
	 {	 
	    printf( "char %s_%c[] = {\n", TableName, (c!=' ' ? c : '_') );
	    SetUpTree( t, BaseIndex, NameIndex, 1, d );
	    PrintTree( d, 0 );
	    printf( "};\n" );
	    BaseIndex = NameIndex;
	    var[vars] = ( c!=' ' ? c : '_' );
	    vars++;
	    c = s[0];
	 }
	 strcpy( &t[NameIndex][0], s ); 
	 NameIndex++;
      } while (EOF!=ReadLine(fp,255,s));
      printf( "\nchar %s_%c[] = {\n", TableName, (c!=' ' ? c : '_') );
      SetUpTree( t, BaseIndex, NameIndex, 1, d );
      PrintTree( d, 0 );
      printf( "};\n" );
      var[vars] = (c!=' ' ? c : '_');
      vars++;
      
      printf( "char *%s_table[] = { ", TableName );
      for (i=0; i<(vars-1); i++ )
	printf( "%s_%c, ", TableName, var[i] );
      printf( "%s_%c };\n", TableName, var[i] );
   }
   else
   {   
      BaseIndex = NameIndex;
      var[vars] = c;
      vars++;
      c = s[0];

      do
      {
	 strcpy( &t[NameIndex][0], s ); 
	 NameIndex++;
      } while (EOF!=ReadLine(fp,255,s));

      printf( "\nchar %s[] = {\n", TableName );
      SetUpTree( t, BaseIndex, NameIndex, 0, d );
      PrintTree( d, 0 );
      printf( "};\n" );
      var[vars] = c;
      vars++;
   }

   fclose(fp);
   
   return 0;
}


int ReadLine( FILE *fp, int n, char *s )
{
   static char buffer[MAXBUFFER]; 
   static char *b=NULL;
   static char *l;
   char *p,*pl;
   int  i;

   if (!fp)
   {
      b = NULL;
      return 0;
   }

   if (!b)
     l = (char*)-1;

   p = s;
   pl = s + n;
   while ( p!=pl )
   {
      if ( b==l || !b )
      {
	 i = (int)fread( buffer, 1, MAXBUFFER, fp ) ;

	 if ( i<=0 )
	   return EOF;

	 b = &buffer[0];
	 l = &buffer[i];
      }
      if ( *b=='\n' )
      {
	 b++;
	 break;
      }
      
      if  ( *b >= ' ' && *b <= '~' )
      {
	 *p++ = *b++;
      }
      else
      {
	 fprintf( stderr,   "ERROR:  Illegal character %x.\n", *b++ );
	 if (p!=pl)
	 {
	    *p = '\0';
	   fprintf( stderr, "        Read in \"%s\"\n", s );
	 }
      }
   }
   if (p == pl)
     return EOF;
   
   *p = '\0';

   return (int)(size_t)(pl-p);
}


int SetUpTree( char t[255][255], int BaseIndex, int UpToIndex, int InToString, 
	      char *Output )
{
   int   Length = 0;
   char  *p, *p1, *p2;
   int   i,k,l,g;

   if (BaseIndex>=UpToIndex)
     return 0;

   p = p1 = &t[BaseIndex][InToString];

   if (BaseIndex+1==UpToIndex)
   {
      Length = (int)strlen( p ) + 1;
      Output[0] = 0;
      Output[1] = Length + 1;
      memcpy( &Output[2], p, Length );
      Output[Length+2] = BaseIndex;
      return Length+3;
   }

   p2 = &t[BaseIndex+1][InToString];			

   while ( *p1 == *p2 )
   {
      Length++; 
      if ('\0' == *p1)
	break;
      p1++; p2++;
   }
   if (0==Length)
   {
      Length = (int)strlen( p ) + 1;
      Output[0] = Length + 3;
      Output[1] = Length + 1;
      memcpy( &Output[2], p, Length );
      Output[Length+2] = BaseIndex;
      l = SetUpTree( t, BaseIndex+1, UpToIndex, InToString, 
		    &Output[Length+3] );

      return Length+3+l;
   }

   for (g=0,k=1; k<=Length; k++ )
   {
      for ( i=BaseIndex+2; i<UpToIndex; i++ )
      {
	 if ( strncmp(p, &t[i][InToString],k) )
	   break;
      }
      if ( i < g )
	 break;
      else
	 g=i;
   }
   i = g;
   Length = k-1;
   Output[1] = Length;

   memcpy( &Output[2], p, Length );  

   l = SetUpTree( t, BaseIndex, i, InToString+Length, &Output[Length+2] );

   g = SetUpTree( t, i, UpToIndex, InToString, &Output[Length+2+l] );
   if ( g )
     Output[0] = Length+2+l;
   else
     Output[0] = 0;

   return Length+2+l+g;
}


int PrintTree( char *T, int i )
{
   int k = 0;

   printf( "   %2d, %2d ", (int)(T[i]-2<0?0:T[i]-2), (int)T[i+1] );

   for (k=i+2; k<(i+T[i+1]); k++)
     printf( ",'%c'", T[k] );

   if  ( T[k] >= ' ' && T[k] <= '~' )
   {
      if (1!=T[i+1])
	printf( ", '%c'", T[k++] );  
      printf( ", '%c',\n", T[k] );  

      PrintTree( T, k+1 );
   }
   else if (0==T[k])
   {
      printf( ",'\\0', %2d,\n", (int)T[k+1] );
   }

   if (T[i] > 0)
     PrintTree( T, i+T[i] );

   return 0;
}
