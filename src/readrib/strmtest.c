/* $RCSfile: strmtest.c,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:07 $
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
 * FILE:  strmtest.c
 *
 * DESCRIPTION:  Private tests for the RIB Reader Library basic 
 *               streamio.c calls -- inshort, more gravity tests.
 *   
 *    Uses:      
 *       The files in affine/readrib/tests are needed for this program.
 *
 *    Contains:
 *       int main( void );
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


char abc_file[]="./tests/abc.asc";
char sb_file[]="./tests/sb.asc";
char gz123_file[]="./tests/123.asc.gz";


int main( void );

int main( void )
{
   RIB_HANDLE   rib;
   int  c,count,i,j,max,n,x;
   size_t  u;
   char *s,*ss;
   char b[255];
   char digits[]="0123456789abc";
   RIB_HANDLE   r[sizeof(digits)/sizeof(char)];
   RtString  *ar;


   printf( "Gravity Tests For streamio.c\n" );
   printf( "----------------------------\n" );

   rib = RibOpen( abc_file, 0, NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", abc_file );
      exit(1);
   }
   else
   {
      printf( "Called:  RibOpen(\"%s\",0,NULL).\n", abc_file );
   }

   if ( RibResetBuffer(rib) )
   {
      printf( "ERROR:  RibResetBuffer( rib ) failed.\n" );
      exit(1);
   }
   else
   {
      printf( "Called:  RibResetBuffer( rib ).\n" );
   }

   if (   RibSaveToBuffer( rib, 1 )
       || !(((PRIB_INSTANCE)rib)->status & kRIB_STATUS_SAVETOBUFFER) )
   {
      printf( "ERROR:  RibSaveToBuffer( rib, 1 ) failed.\n" );
      exit(1);
   }
   else
   {
      printf( "Called:  RibSaveToBuffer( rib, 1 ).\n" );
      if (   RibSaveToBuffer( rib, 0 )
          || (((PRIB_INSTANCE)rib)->status & kRIB_STATUS_SAVETOBUFFER) )
      {
         printf( "ERROR:  RibSaveToBuffer( rib, 0 ) failed.\n" );
         exit(1);
      }
      else
      {
         printf( "Called:  RibSaveToBuffer( rib, 0 ).\n" );
      }     
   }     
   ((PRIB_INSTANCE)rib)->status |= kRIB_STATUS_SAVETOBUFFER;

   for ( i = 'a'; i <= 'z'; i++ )
   {
      if ( 'd'==i )
      {
         c = RibGetChar(rib);
         if ( c != i ) 
           printf( "ERROR: '%c' expected value.  '%c' actual value.\n",i,c );
         c = RibUngetChar( rib, c );
         if ( c ) 
           printf( "ERROR: Calling RibUngetChar( rib, '%c' ).\n", (char)c );
      }
      c = RibGetChar(rib);
      if ( c != i ) 
        printf( "ERROR: '%c' expected value.  '%c' actual value.\n",i,c );
      else
        putchar( c );
   }
   putchar( '\n' );

   s = RibCreateStringFromBuffer( rib, 0 );   
   if (s && !strcmp(s,"abcdefghijklmnopqrstuvwxyz"))
     printf( "%s\n", s );
   else
     printf( "ERROR: \"abcdefghijklmnopqrstuvwxyz\" expected string.  " \
            "\"%s\" actual string\n", s );

   printf( "Buffer length: %lu\n", RibGetBufferLength(rib) );

   printf( "status: %08x\n", ((PRIB_INSTANCE)rib)->status );

   if (!RibIgnoreLastChar( rib ))
     printf( "RibIgnoreLastChar() called.\n" );
   else
     printf( "ERROR from RibIgnoreLastChar().\n" );

#ifdef RIB_MEMORY_TESTS
   _RibFree( s );
#else
   RibFree( s );
#endif

   printf( "Buffer length: %lu\n", RibGetBufferLength(rib) );

   s = RibCreateStringFromBuffer( rib, 0 );   
   if (strcmp(s,"abcdefghijklmnopqrstuvwxy"))
     printf( "ERROR: \"abcdefghijklmnopqrstuvwxy\" expected string " \
            "(no 'z').  \"%s\" actual string\n", s );
   else
     printf( "%s\n", s );
#ifdef RIB_MEMORY_TESTS
   _RibFree( s );
#else
   RibFree( s );
#endif

   RibSaveToBuffer( rib, RI_FALSE );
   if ((u = RibGetBufferLength(rib)) != 0)
      printf("ERROR:  Buffer Length!=0 (buflen==%u)\n",u );
#ifdef RIB_MEMORY_TESTS
   ss = (char*)_RibMalloc( 1234 + 1 );
#else
   ss = (char*)RibMalloc( 1234 + 1 );
#endif
   if (!ss)
     printf( "ERROR:  NULL pointer from RibMalloc().\n");
   else
   {
      ss[1234] = '\0';
      for (i=0; i<1234; i++)
      {
         RibPutChar( rib, i%26 + 'A' );
         ss[i] = i%26 + 'A';
      }
      if ((u = RibGetBufferLength(rib)) != 1234)
        printf("ERROR:  Buffer Length!=1234 (buflen==%u)\n",u );
      else
      {
         s = RibCreateStringFromBuffer( rib, 0 );
         if (!s)
           printf("ERROR:  NULL pointer from RibCreateStringFromBuffer().\n" );
         else
         {
            if (strcmp(s,ss))
              printf( "ERROR:  Large strings don't create correctly.\n" );
         }
         printf( "Large string: %s\n", s );
      }
   }
   if (s)
   {
#ifdef RIB_MEMORY_TESTS
      _RibFree( s );
#else
      RibFree( s );
#endif
   }
   if (ss)
   {
#ifdef RIB_MEMORY_TESTS
      _RibFree( ss );
#else
      RibFree( ss );
#endif
   }
   
   RibSaveToBuffer( rib, RI_FALSE );
   ss = (char*)_RibMalloc( 1234 * sizeof(RtInt) );
   if (!ss)
     printf( "ERROR:  NULL pointer from RibMalloc(1234 * sizeof(RTInt)).\n");
   else
   {
      RtInt I = 123;
      RtInt *pI = NULL;

      for (i=0; i<1234; i++)
      {
         for (c=0; c<sizeof(RtInt); c++ )
         {
            RibPutChar( rib, ((char*)&I)[c] );
            ss[i*sizeof(RtInt)+c] = ((char*)&I)[c];
         }
      }
      u = RibGetBufferLength(rib);
      if ( u != 1234*sizeof(RtInt) )
      {
         printf( "ERROR:  Buffer Length!=1234*sizeof(RtInt) (buflen==%u)\n",
                 u );
      }
      else
      {
         pI = RibCreateIntegerArrayFromBuffer( rib, 0 );
         if (!pI)
         {
            printf("ERROR:  " \
                   "NULL pointer from RibCreateIntegerArrayFromBuffer().\n" );
         }
         else
         {
            if (memcmp(pI,ss,1234*sizeof(RtInt)))
            {
               printf( "ERROR:  Large int array didn't create correctly.\n" );
               for (i=0; i<1234; i++)
               {
                  printf( "%03d ", pI[i] );
               }
               printf( "\n" );
            }
         }
      }
      if (pI)
        _RibFree(pI);
   }
   if (ss)
     _RibFree(ss);


   RibSaveToBuffer( rib, RI_FALSE );
   ss = (char*)_RibMalloc( 1234 * sizeof(RtFloat) );
   if (!ss)
     printf( "ERROR:  NULL pointer from RibMalloc(1234 * sizeof(RTFloat)).\n");
   else
   {
      RtFloat F = 123.45;
      RtFloat *pF = NULL;

      for (i=0; i<1234; i++)
      {
         for (c=0; c<sizeof(RtFloat); c++ )
         {
            RibPutChar( rib, ((char*)&F)[c] );
            ss[i*sizeof(RtFloat)+c] = ((char*)&F)[c];
         }
      }
      u = RibGetBufferLength(rib);
      if ( u != 1234*sizeof(RtFloat))
      {
         printf("ERROR:  " \
                "Buffer Length!=1234*sizeof(RtFloat) (buflen==%u)\n",u );
      }
      else
      {
         pF = RibCreateFloatArrayFromBuffer( rib, 0 );
         if (!pF)
         {
            printf("ERROR:  " \
                   "NULL pointer from RibCreateFloatArrayFromBuffer().\n" );
         }
         else
         {
            if (memcmp(pF,ss,1234*sizeof(RtFloat)))
            {
               printf( "ERROR:  "\
                      "Large float array wasn't created correctly.\n" );
               for (i=0; i<1234; i++)
               {
                  printf( "%g ", pF[i] );
               }
               printf( "\n" );
            }
         }
      }
      if (pF)
        _RibFree(pF);
   }
   if (ss)
     _RibFree(ss);


   /*
    * Test opening subfiles.
    *
    */
   printf( "\nTest opening/tempclosing a compressed files.\n" );
   printf(   "--------------------------------------------\n" );

#ifdef RIB_ZLIB
   rib = RibOpen( gz123_file, 0,NULL );
   if ( !rib )
     printf("Error: RibOpen(%s,0,NULL)\n", gz123_file );
   c = RibGetChar( rib );
   if (c!='1')
     printf("Error: First char in %s not '1'\n", gz123_file );
   if ( RibTempClose((PRIB_INSTANCE)rib) )
     printf("Error: RibTempClose(%s)\n", gz123_file );
   if ( RibReopen((PRIB_INSTANCE)rib) )
     printf("Error: RibReopen(%s)\n", gz123_file );
   c = RibGetChar( rib );
   if (c!='2')
     printf("Error: Second char in %s was '%c' not '2'"\
            "(possible offset problem)\n", gz123_file, c );
#endif
      
   printf( "\nTest opening subfiles.\n" );
   printf(   "----------------------\n" );

   max = RibGetMaximumFiles( rib );
   n = RibGetNumberOfOpenSubfiles( rib );
   if ( n != 0 )
     printf( "ERROR:  RibGetNumberOfOpenSubfiles(rib)==%d.  Expected 0.\n",n);
   n = RibGetNumberOfSubfiles( rib ); 
   if ( n != 0 )
     printf( "ERROR:  RibGetNumberOfSubfiles(rib)==%d.  Expected 0.\n",n);
   max = 5;
   if ( RibSetMaximumFiles(rib, max) )
   {
      printf( "ERROR:  RibSetMaximumFiles(rib, %d).\n",max );
   }
   else
   {
      printf( "Called:  RibSetMaximumFiles(rib, %d).\n",max );
      i = RibGetMaximumFiles( rib );
      if ( max != i )
        printf( "ERROR:  max != RibGetMaximumFiles( rib ):%d.\n",i );
   }

   if ( RibClose(rib) )
   {
      printf( "ERROR:  RibClose( rib ).\n" );
   }
   else
   {
      printf( "Called:  RibClose( rib ).\n" );
   }

   strcpy( b, "./tests/#.asc" );
   s = strrchr( b, '#' );
   count = max+7;
   for ( i=0; i<count; i++ )
   {
      *s = digits[i];
      if (!i)
        r[i] = RibOpen( b, 0,NULL );
      else
        r[i] = RibOpenSubfile( r[i-1], b );

      if (i)
      {
         if (!r[i])
         {
            printf( "ERROR:  RibOpenSubfile( r[%d], \"%s\" ) failed.\n",i,b);
         }
         else
         {
            printf( "Called:  RibOpenSubfile( r[%d],\"%s\" ).\n",i,b);
         }
      }
      else
      {
         if (!r[i])
         {
            printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n",b );
         }
         else
         {
            printf( "Called:  RibOpen(\"%s\",0,NULL).\n",b );
         }
      }

      x=RibGetNumberOfSubfiles(r[i]);
      if ( i != x )
      {
         printf( "ERROR:  " \
                "RibGetNumberOfSubfiles(r[%d]) returned %d expected %d.\n",
                i, x, i );
      }
      else
      {
         printf( "Called:  " \
                "RibGetNumberOfSubfiles(r[%d]) returned %d.\n",i,x);
      }
        
      x=RibGetNumberOfOpenSubfiles(r[i]);
      if (( i<max && i!=x ) || ( i>=max && x!=max))
      {
         printf( "ERROR:  " \
                "RibGetNumberOfOpenSubfiles(r[%d]) returned %d expected %d.\n",
                i, x, ( x>max||i>max ? max : i) );
      }
      else
      {
         printf( "Called:  " \
                "RibGetNumberOfOpenSubfiles(r[%d]) returned %d.\n",i,x);
      }
      if ( gRibNOpenSubfiles!=x )
      {
         printf( "ERROR:  " \
                "RibGetNumberOfOpenSubfiles(r[%d])!=gRibNSubfiles (%d!=%d).\n",
                i, x, gRibNOpenSubfiles );
      }
   }
   printf( "\nClose all subfiles.\n" );
   printf(   "-------------------\n" );
   count--;
   i=count;
   for (;;)
   {
      if ( ((PRIB_INSTANCE)r[i])->status&kRIB_STATUS_CLOSED )
      {
         printf( "ERROR:  r[%d] is closed (kRIB_STATUS_CLOSED set)\n", i );
      }
      if ( ((PRIB_INSTANCE)r[i])->status & kRIB_STATUS_CLOSED )
      {
         printf( "ERROR:  r[%d] is closed\n", i );
      }
      if ( i > max && 
          !(((PRIB_INSTANCE)r[i-max-1])->status&kRIB_STATUS_CLOSED) )
      {
         printf( "ERROR:  r[%d] is opened and max==%d and r[%d]->status " \
                "should marked closed.\n", i, max, i-max-1 );
      }
      c = RibGetChar( r[i] );
      if (i==5)
        exit(0);
      if ( c != digits[i] )
      {
         printf( "ERROR:  Have the wrong file.  RibGetChar returned %c.  " \
                "Expected %c.\n", (char)c, digits[i] );
      }
      else
      {
         printf( "Called:  RibGetChar(r[%d])==%c, ",i,c);
         if ( !(((PRIB_INSTANCE)r[i])->status&kRIB_STATUS_CLOSED))
         {
            printf( "r[%d] is open.\n", i );
         }
         else
         {
            printf( "ERROR:  r[%d] is closed.\n", i );
         }
      }
      if ( RibClose(r[i]) )
      {
         printf( "ERROR:  RibClose( r[%d]=%p ).\n", i, r[i] );
      }
      else
      {
         printf( "Called:  RibClose(r[%d]).  r[%d] is closed\n", i, i );
      }
      i--;
      if ( i>=0 )
      {
         x=RibGetNumberOfSubfiles(r[i]);
         if ( i!=x )
         {
            printf( "ERROR:  " \
                   "RibGetNumberOfSubfiles(r[%d]) returned %d " \
                   "expected %d.\n", i, x, i );
         }
         x=RibGetNumberOfOpenSubfiles(r[i]);
         if (( i>=max && x!=max ) || ( i<max && x != i ))
         {
            printf( "ERROR:  " \
                   "RibGetNumberOfOpenSubfiles(r[%d]) returned %d " \
                   "expected %d.\n", i, x, (i>max?max:i) );
         }
         else
         {
            printf( "Called:  " \
                   "RibGetNumberOfOpenSubfiles(r[%d]) returned %d.\n",i,x);
         }
      }
      else
      {
         break;
      }
   }

   /* 
    *  Test ReadCreateStringsFromBuffer().
    *
    */
   rib = RibOpen( sb_file, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", sb_file );
      exit(1);
   }
   RibSaveToBuffer( rib, RI_TRUE );
   while ( EOF!=(c=RibGetChar(rib)) )
   {
      if ( c=='0' )
      {
         RibIgnoreLastChar( rib );
         RibPutChar( rib, '\0' );
      }
      else if ( c=='\r' || c=='\n' )
      {
         RibIgnoreLastChar( rib );
      }
   }
   if ( RibCreateStringsFromBuffer( rib, 12, &ar ) )
   {
      printf( "ERROR:  RibCreateStringsFromBuffer().\n" );
   }
   else
   {
      for ( i=0; i<12 ; i++ )
      {
         s = ar[i];
         for ( j=0; j<i; j++ )
         {
            if ( memcmp( "123456789-", s, 10 ) )
            {
               printf("ERROR:  String from RibCreateStringsFromBuffer() "\
                      "not correct: %s\n", ar[i] );
            }
            s += 10;
         }
         if ( strcmp( "123456789", s ) )
         {
            printf("ERROR:  End of string from RibCreateStringsFromBuffer() "\
                   "not correct: %s\n", ar[i] );
         }
      }
      _RibFree( ar );
   }
   RibClose( rib );



   /* 
    *  Test rib->byteoffset.
    *
    */
   rib = RibOpen( sb_file, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", sb_file );
      exit(1);
   }
   for ( i=0; i<9; i++ )
   {
      c = RibGetChar(rib);
      if ( c!='1'+i )
      {
         printf( "ERROR:  Reading char: %c.\n",c);
      }
   }
   RibClose( rib );


   /* 
    * Test RibTempClose() and RibReopen() calls.
    *
    */
   rib = RibOpen( sb_file, 0,NULL );
   if (!rib)
   {
      printf( "ERROR:  RibOpen(\"%s\",0,NULL) failed.\n", sb_file );
      exit(1);
   }
   for ( i=0; i<5; i++ )
   {
      c = RibGetChar(rib);
      if ( c!='1'+i )
      {
         printf( "ERROR:  Reading char: %c.\n",c);
      }
   }
   RibTempClose( (PRIB_INSTANCE)rib );
   if ( RibReopen((PRIB_INSTANCE)rib) )
   {
      printf( "ERROR:  RibReopen() failed.\n" );
      exit(1);
   }
   for ( i=5; i<9; i++ )
   {
      c = RibGetChar(rib);
      if ( c!='1'+i )
      {
         printf( "ERROR:  Reading char: %c.\n",c);
      }
   }   
   RibClose( rib );

   return 0;
}
