/* $RCSfile: readtest.c,v $  $Revision: 1.5 $ $Date: 2000/02/27 07:34:00 $
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
 * FILE:  readtest.c
 *
 * DESCRIPTION:  Private tests for the RIB Reader Library's basic 
 *               readfunc.c calls -- inshort, more gravity tests.
 *   
 *    Uses:      
 *       The files in affine/readrib/tests are needed for this program.
 *
 *    Contains:
 *       int main( void );
 *
 *     History:
 *      10-17-99  Added test for kRIB_RILEVEL* and kRIB_HINTLEVEL*.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ripriv.h>
#include <ribrdr.h>


#define GRAVITY_TESTS


char defref_bfile[]="./tests/defref.bin";
char read_nfile[]="./tests/numbers.asc";
char read_ifile[]="./tests/integers.asc";
char read_zfile[]="./tests/zeros.asc";
char read_sfile[]="./tests/strings.asc";
char read_uli[]="./tests/uli.asc";
char read_ulf[]="./tests/ulf.asc";
char read_uls[]="./tests/uls.asc";
char read_afile[]="./tests/read.asc";
char read_bfile[]="./tests/read.bin";
char read_s1file[]="./tests/strings.bin";
char read_commentEOF[]="./tests/commentEOF.asc";
char read_int1[]="./tests/int1.asc";
char read_int1EOF[]="./tests/int1EOF.asc";
char read_flt1[]="./tests/flt1.asc";
char read_flt1EOF[]="./tests/flt1EOF.asc";
char read_minus[]="./tests/minus.asc";
char read_flt1E[]="./tests/flt1E.asc";
char read_str1[]="./tests/str1.asc";
char read_strEOF[]="./tests/strEOF.asc";
char read_int3[]="./tests/int3.asc";
char read_flt3[]="./tests/flt3.asc";
char read_int3B[]="./tests/int3B.asc";
char read_flt3B[]="./tests/flt3B.asc";

char read_int3brackets[]="./tests/int3brackets.asc";
char read_flt3brackets[]="./tests/flt3brackets.asc";


int ExpectedAnError = RI_TRUE;
int PresentLine;
int ExpectedCode;
size_t ExpectedByteOffset;

int main(int argc, char **argv);

int FloatIntStrTestErrorHandler( RIB_HANDLE rib, int code, int severity, 
				 PRIB_ERROR error );


int FloatIntStrTestErrorHandler( RIB_HANDLE rib, int code, int severity, 
				 PRIB_ERROR error )
{
   (void)severity; (void)error;


   if (!ExpectedAnError)
   {
      printf( "ERROR:  Got an unexpected error message.  " \
	      "(see readtest.c line %d)\n", PresentLine );
      exit(1);
   }

   if ( code != ExpectedCode )
   {
      printf( "ERROR:  code(%d) != ExpectedCode(%d) " \
	      "(see readtest.c line %d)\n", code, ExpectedCode,
	      PresentLine );
      exit(1);
   }
   if ( RibGetByteOffset(rib) != ExpectedByteOffset )
   {
      printf( "ERROR:  ByteOffset(%u) != ExpectedByteOffset(%u) " \
	      "(see readtest.c line %d)\n", 
	      (unsigned int)RibGetByteOffset(rib), 
	      (unsigned int)ExpectedByteOffset, PresentLine );
      exit(1);
   }

   return 0;
}


int main(int argc, char **argv) 
{
   RIB_HANDLE     rib;
   PRIB_HASHATOM  a;
   RtInt     *t;
   RtFloat   *p,f,*pf;
   RtString  *r;
   int  i,c,*pi;
   char *s,**ss;
   int error = 0;


   if ( argc > 1 && !strcmp(argv[1],"-") )
   {
      printf( "readtest [-q]\n" 
             "   [-q]       Quick read test only.\n" );
      return 1;
   }

#define _TABLE_LENGTH
#ifdef TABLE_LENGTH
   printf( "#define kRIB_RILEVEL99=%d\n",kRIB_RILEVEL99);
   printf( "enum kRIB_LAST_RI=%d\n",kRIB_LAST_RI);

   printf( "#define kRIB_HINTLEVEL99=%d\n",kRIB_HINTLEVEL99);
   printf( "enum kRIB_LAST_HINT=%d\n",kRIB_LAST_HINT);
   return 0;
#endif

#ifdef GRAVITY_TESTS
   printf( "# Gravity Tests For readfunc.c\n" );
   printf( "# ----------------------------\n" );

   if (argc>1 && !strcmp(argv[1],"-q") )
   {
      rib = RibOpen( read_nfile, 0, NULL );
      if (!rib)
      {
         printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_nfile );
         exit(1);
      }
      
      p = NULL;
      if ( RibReadFloatingPointArray( rib, kRIB_BRACKETS_NONE, 
                                     492, (RtFloat**)&p ) || !p )
      {
         printf( "Error:  RibReadFloatingPointArray()\n");
      }
      else
      {
         error = RI_FALSE;
         for( i=0; i<492; i++)
         {
            if ( p[i] != (RtFloat)1.23 )
            {
               printf("ERROR: p[%d]=%g ", i, p[i] );
               error = RI_TRUE;
            }
         }
         if (error)
           printf("\n");
      }
      _RibFree(p);
      RibClose(rib);
      
      rib = RibOpen( read_zfile, 0, NULL );
      if (!rib)
      {
         printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_ifile );
         exit(1);
      }
      
      ((PRIB_INSTANCE)rib)->status |= kRIB_STATUS_ENCODEDDATA;
      for ( i=0; i<15; i++ )
      {
         if ( RibReadFloatingPoint( rib, &f ) )
         {
            printf( "Error:  RibReadFloatingPoint()\n" );
            printf( "Error:  Zero value read incorrectly at position: %d\n", 
                   i+1 );
         }
         else
         {
            if ( f != 0.0 )
            {
               printf( "Error:  Zero value read incorrectly: "\
                      "position: %d value: %f\n", i+1, f );
            }
         }
      }
      RibClose(rib);
      
      
      /* 
       *   Test RibReadString() and RibReadStringArray() calls. 
       *
       */
      rib = RibOpen( read_sfile, 0, NULL );
      if (!rib)
      {
         printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_ifile );
         exit(1);
      }
      ((PRIB_INSTANCE)rib)->status |= kRIB_STATUS_ENCODEDDATA;
      if ( RibReadString( rib, kRIB_BRACKETS_NONE, &s ) )
      {
         printf( "Error:  RibReadstring(rib, kRIB_BRACKETS_NONE, &s)\n" );
      }
      else
      {
         if ( strcmp( s, "string" ) )
         {
            printf( "Error:  Reading \"string\" as \"%s\".\n", s );
         }
      }
      if ( RibReadStringArray( rib, kRIB_BRACKETS_REQUIRED, 1, &r ) )
      {
         printf( "Error:  " \
                "RibReadstring(rib, kRIB_BRACKETS_REQUIRED, 1, &r)\n" );
      }
      else
      {
         if ( strcmp( r[0], "string" ) )
         {
            printf( "Error:  Reading \"string\" as \"%s\".\n", r[0] );
      }
      }
      if ( RibReadString( rib, kRIB_STRING_RTSTRING, 
                         (RtString*)&ss ) )
      {
         printf( "Error:  "\
                "RibReadstring(rib, kRIB_STRING_RTSTRING, (RtString*)&ss)\n" );
      }
      else
      {
         if ( strcmp( *ss, "string" ) )
         {
            printf( "Error:  Reading \"string\" as \"%s\".\n", *ss );
         }
      }
      if ( RibReadStringArray( rib, kRIB_BRACKETS_REQUIRED, 
                              5, (RtString**)&ss ) )
      {
         printf( "Error:  RibReadstringArray()\n" );
      }
      else
      {
         for ( i=0; i<5; i++ )
         {
            s = ss[i];
            if ( ('1'+i)!=s[0] || '\0'!=s[1] )
            {
               printf("s[0]:%c s[1]:%c\n",s[0],s[1]);
               printf( "Error:  Reading strings: \"%c\" was read as \"%s\".\n", 
                      '1'+i, s );
            }
         }
      }
      RibClose(rib);

      /* Test binary encoded strings. */
      rib = RibOpen( read_s1file, 0, NULL );
      if (!rib)
      {
         printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_ifile );
         exit(1);
      }
      if ( RibReadString( rib, kRIB_BRACKETS_NONE, &s ) )
      {
         printf( "Error:  RibReadstring(rib, kRIB_BRACKETS_NONE, &s)\n" );
      }
      else
      {
         if ( strcmp( s, "string" ) )
         {
            printf( "Error:  Reading \"string\" as \"%s\".\n", s );
         }
      }
      if ( RibReadStringArray( rib, kRIB_BRACKETS_REQUIRED, 1, &r ) )
      {
         printf( "Error:  "\
                "RibReadstring(rib, kRIB_BRACKETS_REQUIRED, 1, &r)\n" );
      }
      else
      {
         if ( strcmp( r[0], "string" ) )
         {
            printf( "Error:  Reading \"string\" as \"%s\".\n", r[0] );
         }
      }
      
      if ( RibReadString( rib, kRIB_BRACKETS_NONE | kRIB_STRING_RTSTRING, 
                         (RtString*)&ss ) )
      {
         printf( "Error:  "\
                "RibReadstring(rib, kRIB_STRING_RTSTRING, (RtString*)&ss)\n" );
      }
      else
      {
         if ( strcmp( *ss, "string" ) )
         {
            printf( "Error:  Reading \"string\" as \"%s\".\n", *ss );
         }
      }
      
      if ( RibReadStringArray( rib, kRIB_BRACKETS_REQUIRED, 
                              5, (RtString**)&ss ) )
      {
         printf( "Error:  RibReadstringArray()\n" );
      }
      else
      {
         for ( i=0; i<5; i++ )
         {
            s = ss[i];
            if ( ('1'+i)!=s[0] || '\0'!=s[1] )
            {
               printf("s[0]:%c s[1]:%c\n",s[0],s[1]);
               printf( "Error:  "\
                      "Reading strings: \"%c\" was read as \"%s\".\n", 
                      '1'+i, s );
            }
         }
      }
      
      if ( RibReadString( rib, kRIB_BRACKETS_NONE, &s ) )
      {
         printf( "Error:  sBinary string defintion and reference\n" );
      }
      else
      {
         if ( strcmp( s, "refstring" ) )
      {
         printf( "Error:  Reading \"refstring\" as \"%s\".\n", s );
      }
      }
      RibClose(rib);
      
      rib = RibOpen( read_ifile, 0,NULL );
      if (!rib)
      {
         printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_ifile );
         exit(1);
      }
      p = NULL;
      if ( RibReadIntegerArray( rib, kRIB_BRACKETS_NONE, 
                               492, (RtInt**)&t ) || !t )
      {
         printf( "Error:  RibReadIntegerArray()\n");
      }
      else
      {
         error = RI_FALSE;
         for( i=0; i<492; i++)
         {
            if ( t[i] != (RtInt)123 )
            {
               printf(" Error: t[%d]=%d ", i, t[i] );
               error = RI_TRUE;
            }
         }
         if (error)
           printf("\n");
      }
      _RibFree(t);
      RibClose(rib);
      
      rib = RibOpen( read_ulf, 0,NULL );
      RibSetStatus( rib, kRIB_STATUS_ENCODEDDATA, kRIB_STATUS_ENCODEDDATA );
      if (!rib)
      {
         printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_ulf );
         exit(1);
      }
      c = 0;
      t = NULL;
      if ( RibReadArrayAndLength( rib, 
                                 kRIB_BRACKETS_REQUIRED|kRIB_ARRAY_FLOAT, 
                                 &c, (void**)&p ) || !p || 492!=c )
      {
         printf( "Error:  RibReadArrayAndLength(kRIB_ARRAY_FLOAT)\n");
      }
      else
      {
         error = RI_FALSE;
         for( i=0; i<492; i++)
         {
            if ( p[i] != (RtFloat)1.23 )
            {
               printf(" Error: p[%d]=%g ",i, p[i] );
               error = RI_TRUE;
            }
         }
         if (error)
           printf("\n");
      }
      _RibFree(p);
      RibClose(rib);
      

      rib = RibOpen( read_uli, 0,NULL );
      if (!rib)
      {
         printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_uli );
         exit(1);
      }
      c = 0;
      t = NULL;
      if ( RibReadArrayAndLength( rib, 
                                 kRIB_BRACKETS_REQUIRED|kRIB_ARRAY_INTEGER, 
                                 &c, (void**)&t ) || !t || 492!=c )
      {
         printf( "Error:  RibReadArrayAndLength(kRIB_ARRAY_INTEGER)\n");
      }
      else
      {
         error = RI_FALSE;
         for( i=0; i<492; i++)
         {
            if ( t[i] != (RtInt)123 )
              printf(" Error: t[%d]=%d ",i,t[i]);
            error = RI_TRUE;
         }
         if (error)
           printf("\n");
      }
      _RibFree(t);
      RibClose(rib);
      
      rib = RibOpen( defref_bfile, kRIB_LAST_RI, gRibRITable );
      if (!rib)
      {
         printf( "ERROR:  RibOpen(\"%s\",kRIB_LAST_RI, gRibRITable) failed.\n",
		 defref_bfile );
         exit(1);
      }

      /* Unknown string array length test. */
      rib = RibOpen( read_uls, 0,NULL );
      RibSetStatus( rib, kRIB_STATUS_ENCODEDDATA, kRIB_STATUS_ENCODEDDATA );
      if (!rib)
      {
         printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_uls );
         exit(1);
      }
      c = 0;
      r = NULL;
      if ( RibReadArrayAndLength( rib, 
                                 kRIB_BRACKETS_REQUIRED|kRIB_ARRAY_STRING, 
                                 &c, (void**)&r ) || !r || 4!=c )
      {
         printf( "Error:  RibReadArrayAndLength(kRIB_ARRAY_STRING)\n");
	 printf( "  c:%d r:%p\n",c,r);
         exit(1);
      }
      else
      {
         error = RI_FALSE;
         for( i=0; i<4; i++)
         {
	    if (strcmp( r[i], "abcdefghijklmnopqrstuvwxyz" ))
            {
               printf(" Error: r[%d]=\"%s\" ",i, r[i] );
               error = RI_TRUE;
            }
         }
         if (error)
           printf("\n");
      }
      _RibFree(r);
      RibClose(rib);
      /* Unknown string array length test. */

      /* defref_bfile is used for the following input:
       *
       * % ribdump tests/defref.bin 
       * 315  000  226    G    e    o    r    g   # <defstr 0 "Georg
       *   e  317  000                            # e"> <str 0>
       */
      s = NULL;
      c = RibHandlePrefix(rib);
      RibUngetChar( rib, c );
      a = RibFindMatch(((PRIB_INSTANCE)rib)->phashtable, 
                       kRIB_HASH_STRINGTOKEN, 0, NULL);
      if (!a)
      {
         printf("Error:  No match in hash table.\n");
         return 1;
      }
      if (strcmp("George",(char*)a->with.extdata))
      {
         printf("Error:  "\
                "Bad string retrieved:\"%s\".\n",(char*)a->with.extdata);
      }
      if ( RibReadString(rib,0,&s) )
      {
         printf( "Error:  RibReadString()\n");
      }
      RibClose(rib);
   }

   rib = RibOpen( read_afile, kRIB_LAST_RI, gRibRITable );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",kRIB_LAST_RI, gRibRITable) failed.\n", 
	      read_afile );
      exit(1);
   }

   if ( (PRIB_SUBDIVFUNC)RiProcDelayedReadArchive
       != (PRIB_SUBDIVFUNC)
       (((PRIB_INSTANCE)rib)->ritable[kRIB_PROCDELAYEDREADARCHIVE]) )
   {
      printf( "ERROR:  RiProcDelayedReadArchive != " \
             "((PRIB_INSTANCE)rib)->ritable[kRIB_PROCDELAYEDREADARCHIVE]\n" );
   }

   if ( RibResetBuffer(rib) )
   {
      printf( "ERROR:  RibResetBuffer( rib ) failed.\n" );
      exit(1);
   }

   if ( RibGetArcRecHandler(rib) != ((PRIB_INSTANCE)rib)->arcrechandler )
     printf( "Error: RibGetArcRecHandler(hrib)\n");

   i=RibGetRIFromString( "AreaLightSource" );
   if (i != kRIB_AREALIGHTSOURCE )
   {
      printf( "Error: RibGetRIFromString(\"AreaLightSource\")!="\
             "kRIB_AREALIGHTSOURCE: returned:%d\n", i );
   }

   i=RibGetRIFromString( "Sphere" );
   if (i != kRIB_SPHERE )
   {
      printf( "Error: RibGetRIFromString(\"Sphere\")!="\
             "kRIB_SPHERE: returned:%d\n", i );
   }

   if (RibGetRIFromString("MakeBump")!=kRIB_MAKEBUMP)
   {
      printf( "Error: RibGetRIFromString( \"MakeBump\" )!=kRIB_MAKEBUMP\n");
   }

   ((PRIB_INSTANCE)rib)->status |= kRIB_STATUS_ENCODEDDATA;

   if (RibRead(rib))
   {
     printf( "Error: RibRead(rib)\n");
   }

   RibClose(rib);

   if (argc>1 && !strcmp(argv[1],"-q") )
   {
      /* Skip the rest of the tests. */
      return 0;
   }


   printf( "\n#Binary Encoded RIB Input\n" );
   printf(   "#------------------------\n" );
   rib = RibOpen( read_bfile, kRIB_LAST_RI, gRibRITable );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",kRIB_LAST_RI, gRibRITable) failed.\n", 
	      read_afile );
      exit(1);
   }
   if (RibRead(rib))
   {
     printf( "Error: RibRead(rib)\n");
   }
   RibClose(rib);


   /* tests/commentEOF.asc
    *
    *      % ribdump tests/commentEOF.asc
    *        #    c    o    m    m    e    n    t   # #comment
    */
   rib = RibOpen( read_commentEOF, 0, NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_commentEOF );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_FALSE;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   RibSaveToBuffer( rib, RI_TRUE );
   RibReadArchiveRecord( rib );
   s = RibCreateStringFromBuffer( rib, 0 );
   if (!s || !strcmp(s,"comment"))
   {
      printf( "ERROR:  readtest.c:%d\n",__LINE__ );
      exit(1);
   }
   RibClose(rib);
#endif

   printf( "\n#Float, Integer and String tests\n" );
   printf(   "#-------------------------------\n" );

   /* tests/int1.asc
    *
    *      % ribdump tests/int1.asc
    *        1    N    U    M    B    E    R  040   # 1NUMBER 
    *        O    N    E   \n                       # ONE\n
    */
   rib = RibOpen( read_int1, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_int1 );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   i = 1234;
   RibReadInteger( rib, &i );   
   if ( i != 1 )
      printf( "ERROR:  i==%d see readtest.c (line %d)\n",i,__LINE__ );
   if ( RibGetByteOffset(rib)!=1 )
    {
      printf( "ERROR:  BytesOffset should be one readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 2; /* TO-DO ITEM: Should probably be 1 not 2. */
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   i = 9999;
   RibReadInteger( rib, &i );   
   if ( i!=9999 )
      printf("ERROR:   RibReadInteger() wrote data when it shouldn't "\
	     "have.  Refer to readtest.c:%d\n",__LINE__);
   RibClose(rib);
   ExpectedAnError = RI_FALSE;

   /* tests/int1EOF.asc
    *
    *      % ribdump tests/int1EOF.asc
    *        1                                      # 1
    */
   rib = RibOpen( read_int1EOF, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_int1EOF );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
   {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_FALSE;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   i = 1234;
   RibReadInteger( rib, &i );   
   if ( i != 1 )
      printf( "ERROR:  readtest.c:%d\n",__LINE__ );
   RibClose(rib);

   /* tests/flt1.asc
    *
    *      % ribdump tests/flt1.asc
    *        1    .    2    3    4    F    L    O   # 1.234FLO
    *        A    T   \n                            # AT\n
    */
   rib = RibOpen( read_flt1, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_flt1 );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   RibReadFloatingPoint( rib, &f );   
   if ( f != (float)1.234 )
      printf( "ERROR:  return float %g!=1.234 readtest.c:%d\n",
	      f, __LINE__ );
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 6; /* TO-DO ITEM: Should be 5 not 6. */
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   f = (float)99.99;
   RibReadFloatingPoint( rib, &f );   
   if ( f!=(float)99.99 )
      printf("ERROR:   RibReadFloatingPoint() wrote data when it shouldn't "\
	     "have.  Refer to readtest.c:%d\n",__LINE__);
   RibClose(rib);
   ExpectedAnError = RI_FALSE;


   /* tests/flt1EOF.asc
    *
    *      % ribdump tests/flt1EOF.asc
    *        1    .    2    3    4                  # 1.234
    */
   rib = RibOpen( read_flt1EOF, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_flt1EOF );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_FALSE;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   RibReadFloatingPoint( rib, &f );   
   if ( f != (float)1.234 )
      printf( "ERROR:  readtest.c:%d\n",__LINE__ );
   RibClose(rib);

   /* tests/minus.asc
    *
    *      % ribdump tests/minus.asc
    *        -   \n                                 # -\n
    */
   rib = RibOpen( read_minus, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_minus );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 3; /* TO-DO ITEM: Should be 1 not 3. */
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   i = 123;
   RibReadInteger( rib, &i );   
   if (i!=123)
      printf("ERROR:   RibReadInteger() wrote data when it shouldn't "\
	     "have.  Refer to readtest.c:%d\n",__LINE__);      
   RibClose(rib);
   ExpectedAnError = RI_FALSE;


   /* tests/minus.asc
    *
    *      % ribdump tests/minus.asc
    *        -   \n                                 # -\n
    */
   rib = RibOpen( read_minus, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_minus );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 2;/* TO-DO ITEM: Should be 1 not 2. */
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   f = (float)123.4;
   RibReadFloatingPoint( rib, &f );   
   if (f!=(float)123.4)
      printf("ERROR:  RibReadFloatingPoint():%.17g wrote data when it "\
	     "shouldn't have.  Refer to readtest.c:%d\n",f,__LINE__);      
   RibClose(rib);
   ExpectedAnError = RI_FALSE;


   /* tests/flt1E.asc
    *
    *      % ribdump tests/flt1E.asc
    *        1    .    2    3    4    E   \n        # 1.234E\n
    */
   rib = RibOpen( read_flt1E, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_flt1E );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 6;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   f = (float)9.87;
   RibReadFloatingPoint( rib, &f );   
   if ( f != (float)1.234 )
      printf( "ERROR:  f=%g readtest.c:%d\n",f,__LINE__ );
   RibClose(rib);
   ExpectedAnError = RI_FALSE;

   /* tests/str1.asc
    *
    *      % ribdump tests/str1.asc
    *        "    s    t    r    i    n    g    1   # "string1
    *        "                                      # "
    */
   rib = RibOpen( read_str1, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_str1 );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_FALSE;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   RibReadString( rib, kRIB_BRACKETS_NONE, &s );  
   if (!s || strcmp(s,"string1"))
   {
      printf( "ERROR:  String set to \"%s\" readtest.c:%d\n",s,__LINE__ );
      exit(1);
   }
   RibClose(rib);


   /* tests/strEOF.asc
    *
    *      % ribdump tests/strEOF.asc
    *        "    s    t    r    i    n    g    1   # "string1
    */
   rib = RibOpen( read_strEOF, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_strEOF );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
   {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 8;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   s = read_strEOF;
   RibReadString( rib, kRIB_BRACKETS_NONE, &s );  
   if ( s!=read_strEOF && s!=NULL ) /* TO-DO ITEM: For now take NULL too. */
   {
      printf("ERROR:   RibReadString() wrote data when it shouldn't "\
	     "have (\"%s\").  Refer to readtest.c:%d\n",s,__LINE__);      
      exit(1);
   }
   RibClose(rib);
   ExpectedAnError = RI_FALSE;


   /* tests/int3.asc
    *
    *      % ribdump tests/int3.asc
    *        1  040    2  040    3   \n             # 1 2 3\n
    */
   rib = RibOpen( read_int3, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_int3 );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   RibReadIntegerArray( rib, kRIB_BRACKETS_NONE, 3, &pi );
   if ( !pi || pi[0]!=1 || pi[1]!=2 || pi[2]!=3 )
   {
      if (pi)
	 printf( "ERROR:   RibReadIntegerArray()={ %d, %d, %d } ",
		 pi[0],pi[1],pi[2] );
      else
	 printf("ERROR:   RibReadIntegerArray() ");

      printf( "Refer to readtest.c:%d\n",__LINE__);      

      exit(1);
   }
   ExpectedAnError = RI_FALSE;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   RibClose(rib);


   /* tests/int3.asc
    *
    *      % ribdump tests/int3.asc
    *        1  040    2  040    3   \n             # 1 2 3\n
    */
   rib = RibOpen( read_int3, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_int3 );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   RibReadIntegerArray( rib, kRIB_BRACKETS_NONE, 3, &pi );
   if ( !pi || pi[0]!=1 || pi[1]!=2 || pi[2]!=3 )
   {
      if (pi)
	 printf( "ERROR:   RibReadIntegerArray()={ %d, %d, %d } ",
		 pi[0],pi[1],pi[2] );
      else
	 printf("ERROR:   RibReadIntegerArray() ");

      printf( "Refer to readtest.c:%d\n",__LINE__);      

      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 6;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   pi = (int*)0xA0A0A0A;
   RibReadIntegerArray( rib, kRIB_BRACKETS_NONE, 4, &pi );

   /* TO-DO ITEM: For now take NULL too. */
   if ( pi != (int*)0xA0A0A0A && pi!=NULL ) 
   {
      printf( "ERROR:  pi==%p!=(int*)0xA0A0A0A readtest.c:%d\n",pi,__LINE__);
      exit(1);
   }
   RibClose(rib);
   ExpectedAnError = RI_FALSE;


   /* tests/flt3.asc
    *
    *      % ribdump tests/flt3.asc
    *        1    .    2    3    4  040    2    .   # 1.234 2.
    *        3    4    5  040    3    .    4    5   # 345 3.45
    *        6   \n                                 # 6\n
    */
   rib = RibOpen( read_flt3, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_flt3 );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   RibReadFloatingPointArray( rib, kRIB_BRACKETS_NONE, 3, &pf );
   if ( !pf 
	|| pf[0]!=(float)1.234 || pf[1]!=(float)2.345 || pf[2]!=(float)3.456 )
   {
      if (pf)
	 printf( "ERROR:   RibReadFloatingPointArray()={ %g, %g, %g } ",
		 pf[0],pf[1],pf[2] );
      else
	 printf("ERROR:   RibReadFloatingPointArray() ");

      printf( "Refer to readtest.c:%d\n",__LINE__);      

      exit(1);
   }
   ExpectedAnError = RI_FALSE;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   RibClose(rib);


   /* tests/flt3.asc
    *
    *      % ribdump tests/flt3.asc
    *        1    .    2    3    4  040    2    .   # 1.234 2.
    *        3    4    5  040    3    .    4    5   # 345 3.45
    *        6   \n                                 # 6\n
    */
   rib = RibOpen( read_flt3, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_flt3 );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 18;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   pf = (float*)0xA0A0A0A;
   RibReadFloatingPointArray( rib, kRIB_BRACKETS_NONE, 4, &pf );

   /* TO-DO ITEM: For now take NULL too. */
   if ( pf != (float*)0xA0A0A0A && pf!=NULL )
   {
      printf( "ERROR:  pf(%p)!= (float*)0xA0A0A0A readtest.c:%d\n",
	      pf,__LINE__);
      exit(1);
   }
   RibClose(rib);
   ExpectedAnError = RI_FALSE;

   /* tests/int3B.asc
    *
    *      % ribdump tests/flt3B.asc
    *        1    .    2    3    4  040    2    .   # 1.234 2.
    *        3    4    5  040    B    L    O    C   # 345 BLOC
    *        K   \n                                 # K\n
    *      % ribdump -offset 4 tests/int3B.asc 
    *        B    L    O    C    K   \n             # BLOCK\n
    */
   rib = RibOpen( read_int3B, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_int3B );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 5; /* TO-DO ITEM: Should be 4 not 5. */
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   pi = (int*)0xA0A0A0A;
   RibReadIntegerArray( rib, kRIB_BRACKETS_CLOSE, 2, &pi );
   /* TO-DO ITEM: For now take NULL too. */
   if ( pi != (int*)0xA0A0A0A && pi!=NULL )
   {
      printf( "ERROR:  pi(%p)!= (int*)0xA0A0A0A readtest.c:%d\n",
	      pi,__LINE__);
      exit(1);
   }
   RibClose(rib);
   ExpectedAnError = RI_FALSE;


   /* tests/flt3B.asc
    *
    *      % ribdump tests/flt3B.asc
    *        1    .    2    3    4  040    2    .   # 1.234 2.
    *        3    4    5  040    B    L    O    C   # 345 BLOC
    *        K   \n                                 # K\n
    *      % ribdump -offset 12 tests/flt3B.asc 
    *        B    L    O    C    K   \n             # BLOCK\n
    */
   rib = RibOpen( read_flt3B, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_flt3B );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 13; /* TO-DO ITEM: Should be 12 not 13. */
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   pf = (float*)0xA0A0A0A;
   RibReadFloatingPointArray( rib, kRIB_BRACKETS_CLOSE, 2, &pf );
   /* TO-DO ITEM: For now take NULL too. */
   if ( pf != (float*)0xA0A0A0A && pf!=NULL )
   {
      printf( "ERROR:  pf(%p)!= (float*)0xA0A0A0A readtest.c:%d\n",
	      pf,__LINE__);
      exit(1);
   }
   RibClose(rib);
   ExpectedAnError = RI_FALSE;


   /* tests/int3brackets.asc
    *
    *      % ribdump tests/int3brackets.asc
    *        [    1  040    2  040    3    ]   \n   # [1 2 3]\n
    */
   rib = RibOpen( read_int3brackets, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_int3brackets );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 7;/* Read one after the ']'. */
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   pi = (int*)0xA0A0A0A;
   RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED, 4, &pi );
   /* TO-DO ITEM: For now take NULL too. */
   if ( pi != (int*)0xA0A0A0A && pi!=NULL )
   {
      printf( "ERROR:  pi(%p)!= (int*)0xA0A0A0A readtest.c:%d\n",
	      pi,__LINE__);
      exit(1);
   }
   RibClose(rib);
   ExpectedAnError = RI_FALSE;

   /* tests/flt3brackets.asc
    *
    *      % ribdump tests/flt3brackets.asc
    *        [    1    .    2  040    2    .    3   # [1.2 2.3
    *      040    3    .    4    ]                  #  3.4]
    */
   rib = RibOpen( read_flt3brackets, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_flt3brackets );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 13;/* Read one after the ']'. */
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   pf = (float*)0xA0A0A0A;
   RibReadFloatingPointArray( rib, kRIB_BRACKETS_REQUIRED, 4, &pf );
   /* TO-DO ITEM: For now take NULL too. */
   if ( pf != (float*)0xA0A0A0A && pf!=NULL )
   {
      printf( "ERROR:  pf(%p)!= (float*)0xA0A0A0A readtest.c:%d\n",
	      pf,__LINE__);
      exit(1);
   }
   RibClose(rib);
   ExpectedAnError = RI_FALSE;


   /* tests/int1.asc
    *
    *      % ribdump tests/int1.asc
    *        1    N    U    M    B    E    R  040   # 1NUMBER 
    *        O    N    E   \n                       # ONE\n
    */
   rib = RibOpen( read_int1, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_int1 );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 1;
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   pi = (int*)0xA0A0A0A;
   RibReadIntegerArray( rib, kRIB_BRACKETS_REQUIRED, 4, &pi );
   /* TO-DO ITEM: For now take NULL too. */
   if ( pi != (int*)0xA0A0A0A && pi!=NULL )
   {
      printf( "ERROR:  pi(%p)!= (int*)0xA0A0A0A readtest.c:%d\n",
	      pi,__LINE__);
      printf("     RibGetByteOffset(rib):%lu\n",RibGetByteOffset(rib));
      exit(1);
   }
   RibClose(rib);
   ExpectedAnError = RI_FALSE;


   /* tests/flt1.asc
    *
    *      % ribdump tests/flt1.asc
    *        1    .    2    3    4    F    L    O   # 1.234FLO
    *        A    T   \n                            # AT\n
    */
   rib = RibOpen( read_flt1, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", read_flt1 );
      exit(1);
   }
   if ( RibGetByteOffset(rib)!=0 )
    {
      printf( "ERROR:  BytesOffset should be zero readtest.c:%d\n",__LINE__);
      exit(1);
   }
   ExpectedAnError = RI_TRUE;
   ExpectedCode = RIE_SYNTAX;
   ExpectedByteOffset = 6; /*TO-DO ITEM: The 6 should be a 5. */
   PresentLine = __LINE__;
   RibSetErrorHandler( rib, FloatIntStrTestErrorHandler );
   pf = (float*)0xA0A0A0A;
   RibReadFloatingPointArray( rib, kRIB_BRACKETS_CLOSE, 4, &pf );
   /* TO-DO ITEM: For now take NULL too. */
   if ( pf != (float*)0xA0A0A0A && pf!=NULL )
   {
      printf( "ERROR:  pf(%p)!= (float*)0xA0A0A0A readtest.c:%d\n",
	      pf,__LINE__);
      exit(1);
   }
   RibClose(rib);
   ExpectedAnError = RI_FALSE;

   printf( "\n#Completed Float, Integer and String tests\n" );
   printf(   "#-----------------------------------------\n" );

   return 0;
}
