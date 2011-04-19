/* $RCSfile: strtest.c,v $  $Revision: 1.3 $ $Date: 1999/10/12 02:27:32 $
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
 * FILE:  strtest.c
 *
 * DESCRIPTION:  Gravity check.
 *   
 *    Contains:
 * 
 *    History:
 *          10-11-99   Some C libs have printf()'s that can't take a NULL 
 *                     pointer for a string, so added extra code to handle
 *                     this.
 *
 *    References:
 *
 */
#define _RIPRIV_TYPES
#include <string.h>
#include <stdio.h>
#include <ripriv.h>
#include <ribrdr.h>


/* Table of tokens. */

char *tokens[] = {
   "abc",
   "abcd",
   "bdefghij",
   "defghijk",
   "lmnop",
   "qrstuv",
   "  abc"
};

char *validtokens[] = {
   "abcd ",
   "qrstuv "
};

int validindices[] = { 1, 5 };

char *nontokens[] = {
   " abcd",
   "aBc",
   "bbdefghij",
   "defghi",
   "defghij",
   "lmnop12",
   "qrst_uv"
};
 

char table[] = {
   12,  3 ,'a', 'b', 'c',
    2,  2 ,'\0',  0,
    0,  3 ,'d','\0',  1,
   10, 10 ,'b','d','e','f','g','h','i','j','\0',  2,
   10, 10 ,'d','e','f','g','h','i','j','k','\0',  3,
    7,  7 ,'l','m','n','o','p','\0',  4,
    8,  8 ,'q','r','s','t','u','v','\0',  5,
    0,  7 ,' ',' ','a','b','c','\0',  6,
};


char vartable[] = {
   15,  1 , 'P',
   2,  2 ,'\0',  0,
   3,  3 ,'w','\0',  1,
   0,  3 ,'z','\0',  2,
   10,  1 , 'N',
   2,  2 ,'\0',  3,
   0,  3 ,'p','\0',  4,
   3,  3 ,'s','\0',  5,
   0,  3 ,'t','\0',  6,
};
RtToken vartablelist[] = { "P","Pw","Pz","N","Np","s","t" };
RtPointer varoutput[sizeof(vartablelist)/sizeof(RtToken)];
RtToken vartokens[]      = { "P", "st", "N", "Np", "s" };
RtPointer varparms[]     = { 
   "P's data", "st's data", "N's data", "Np's data", "s's data" };
RtPointer expectedoutput[sizeof(vartablelist)/sizeof(RtToken)];


int main(int argc, char **argv);


int main(int argc, char **argv)
{
   int    i,index;
   char *p,*pp;

   for ( i=0; i < sizeof(tokens)/sizeof(char*); i++ )
   {
      index = RibReadStringForToken( tokens[i], table, NULL );
      if ( index < 0 )
      {
         printf( "Error: No match found for \"%s\"\n", tokens[i] );
         printf( "       Index returned: %d\n", index );
      }
      else
      {
         if ( i!=index )
           printf( "Error:  Wrong match: \"%s\" matched token \"%s\"\n",
                  tokens[i], tokens[index] );
         else
           printf( "Test Passed: Found token \"%s\"\n", tokens[i] );
      }
   }
   putchar( '\n' );

   for ( i=0; i < sizeof(validtokens)/sizeof(char*); i++ )
   {
      index = RibReadStringForToken( validtokens[i], table, NULL );
      if ( index < 0 )
      {
         printf( "Error: No match found for \"%s\"\n", validtokens[i] );
         printf( "       Index returned: %d\n", index );
      }
      else
      {
         if ( index != validindices[i] )
         {
            printf( "Error:  Wrong match: \"%s\" matched token \"%s\"\n",
                   validtokens[i], tokens[index] );
            printf( "i: %d, index: %d\n", i, index );
         }
         else
         {
            printf( "Test Passed: Found token \"%s\"\n", validtokens[i] );
         }
      }
   }
   putchar( '\n' );

   for ( i=0; i < sizeof(nontokens)/sizeof(char*); i++ )
   {
      index = RibReadStringForToken( nontokens[i], table, NULL );
      if ( index < 0 )
      {
         printf( "Test Passed: No match found for \"%s\"\n", nontokens[i] );
      }
      else
      {
         if ( index < sizeof(nontokens)/sizeof(char*) )
           printf( "Error:  Wrong match: \"%s\" matched token \"%s\"\n",
                  nontokens[i], tokens[index] );
         else
           printf( "Error:  Wrong match: index: %d i: %d testtoken: \"%s\"\n", 
                  index, i, nontokens[i] );
      }
   }

   p = "abc";
   index = RibReadStringForToken( p, table, &pp );
   if ( index==0 && pp==p+strlen(p) )
   {
      printf( "Test Passed: RibReadStringForToken( \"abc\", table, &pp )\n" );
   }
   else
   {
      printf( "Error with RibReadStringForToken( \"abc\", table, &pp )\n" );
   }
   p = "abc ";
   index = RibReadStringForToken( p, table, &pp );
   if ( index==0 && pp==p+strlen("abc") )
   {
      printf( "Test Passed: RibReadStringForToken( \"abc \", table, &pp )\n" );
   }
   else
   {
      printf( "Error with RibReadStringForToken( \"abc \", table, &pp )\n" );
   }
   p = "abc\n";
   index = RibReadStringForToken( p, table, &pp );
   if ( index==0 && pp==p+strlen("abc") )
   {
      printf( "Test Passed: RibReadStringForToken(\"abc\\n \",table, &pp)\n" );
   }
   else
   {
      printf( "Error with RibReadStringForToken( \"abc\\n\", table, &pp )\n" );
   }



   printf( "\nRibGetUserParameters Tests\n" );

   expectedoutput[0] = varparms[0];  /*P */ 
   expectedoutput[1] = NULL;         /*Pw*/ 
   expectedoutput[2] = NULL;         /*Pz*/ 
   expectedoutput[3] = varparms[2];  /*N */ 
   expectedoutput[4] = varparms[3];  /*Np*/ 
   expectedoutput[5] = varparms[4];  /*s */
   expectedoutput[6] = NULL;         /*t */

   if ( RibGetUserParameters( vartable, sizeof(vartablelist)/sizeof(RtToken),
                             sizeof(varparms)/sizeof(RtPointer), 
                             vartokens, varparms, varoutput ) )
   {
      printf("Error from RibGetUserParameters\n" );
   }
   else
   {
      for ( i=0; i < sizeof(vartablelist)/sizeof(RtPointer); i++ )
      {
         if ( varoutput[i]!=expectedoutput[i] )
         {
	    /* Some C libs have printf()'s that can't take a NULL pointer
	     *    for a string.
	     */
	    if ( varoutput[i]==NULL )
	       varoutput[i] = "(nil)";
	    if ( expectedoutput[i]==NULL )
	       expectedoutput[i] = "(nil)";
            printf("Error with output from RibGetUserParameters\n" \
                   "   Token %s, varoutput[%d]: %s, expectedoutput[%d]: %s\n",
                   vartablelist[i], i, varoutput[i], i, expectedoutput[i] );
         }
         else
         {
	    /* Some C libs have printf()'s that can't take a NULL pointer
	     *    for a string.
	     */
	    if ( varoutput[i]==NULL )
	       varoutput[i] = "(nil)";
	    if ( expectedoutput[i]==NULL )
	       expectedoutput[i] = "(nil)";
            printf( "Test Passed:  " \
                   "Token: %s, varoutput[%d]: %s, expectedoutput[%d]: %s\n",
                   vartablelist[i], i, varoutput[i], i, expectedoutput[i] );
         }
      }
   }

   return 0;
}



