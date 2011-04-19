/* $RCSfile: ribtbl.c,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:07 $
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
 * FILE:  ribtbl.c
 *
 * DESCRIPTION:  Table for ReadFileForToken().  Created originally with the 
 *               following line command:
 *
 *                          tokentbl ribcalls.asc RibRI
 *               Note:  The program tokentbl leaves an extra comma just
 *                      before "};" that has to be deleted by hand for now.
 *
 *    Contains:  
 *               RibRI_A[], RibRI_B[], RibRI_C[], RibRI_D[], RibRI_E[], 
 *               RibRI_F[], RibRI_G[], RibRI_H[], RibRI_I[], RibRI_L[], 
 *               RibRI_M[], RibRI_N[], RibRI_O[], RibRI_P[], RibRI_Q[], 
 *               RibRI_R[], RibRI_S[], RibRI_T[], RibRI_W[], RibRI_v[], 
 *               and gRibRITokenTables[]
 *
 *     History:  
 *      12-01-96  Changed the order of the RibRI_A and RibRI_C tables to be 
 *                a little more optimized in the order the tokens are searched.
 *       7-11-97  Added reading functions for new PRMan 3.7 Curves and 
 *                Points primitives.  Added new Procedural RIB binding.
 *      10-12-98  Added SubdivsionMesh.
 *      12-05-98  Added GeometricRepresentation and Resource.
 *      04-27-99  Added NuCurves and Implicit.
 * 
 *
 *    References:
 *          [PIXA89]  Pixar Animation Studios, The RenderMan Interface, 
 *                    Version 3.1, Richmond, CA, September 1989.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define __RIB_FILE_RIBTBL
#define  _RIPRIV_TYPES
#include <ripriv.h>
#include <ribrdr.h>

/* Since AreaLightSource and Atmosphere are not as common as Attribute calls,
 *   the order in which the tokens were searched was changed.  The RI index
 *   into the RI table remains the same.
 * 
 * Using "tokentbl aribcall.asc RibRI" and changing the indexes manually the 
 *   following table data was created.  The orginal table was commented
 *   out.
 */
char RibRI_A[] = {
   42,  1 , 't',
   27,  7 ,'t','r','i','b','u', 't', 'e',
    2,  2 ,'\0',                                  2, /* 0, */
    7,  7 ,'B','e','g','i','n','\0',              3, /* 1, */
    0,  5 ,'E','n','d','\0',                      4, /* 2, */
    0, 10 ,'m','o','s','p','h','e','r','e','\0',  1, /* 3, */
    0, 16 ,'r','e','a','L','i','g','h','t',
           'S','o','u','r','c','e','\0',          0 /* 4 */
/* Original data follows:
 *
 *   16, 16 ,'r','e','a','L','i','g','h','t','S','o','u','r','c','e','\0',  0,
 *    0,  1 , 't',
 *   10, 10 ,'m','o','s','p','h','e','r','e','\0',  1,
 *    0,  7 ,'t','r','i','b','u', 't', 'e',
 *    2,  2 ,'\0',  2,
 *    7,  7 ,'B','e','g','i','n','\0',  3,
 *    0,  5 ,'E','n','d','\0',  4
 */
};

char RibRI_B[] = {
    6,  6 ,'a','s','i','s','\0',  5,
    0,  6 ,'o','u','n','d','\0',  6
};

/* Color gets called more than Clipping, so the order of the search was
 *   changed based on the table from "tokentbl cribcall.asc RibRI".
 */
char RibRI_C[] = {
   81,  1 , 'o',
   18,  3 ,'l', 'o', 'r',
    2,  2 ,'\0',                                                  8, /*0,*/
    0,  9 ,'S','a','m','p','l','e','s','\0',                      9, /*1,*/
   22,  1 , 'n',
   14, 14 ,'c','a','t','T','r','a','n','s','f','o','r','m','\0',  10,/*2,*/
    0,  3 ,'e','\0',                                              11,/*3,*/
    0,  3 ,'o', 'r', 'd',
   13, 13 ,'i','n','a','t','e','S','y','s','t','e','m','\0',      12,/*4,*/
    0, 14 ,'S','y','s','T','r','a','n','s','f','o','r','m','\0',  13,/*5,*/
    7,  7 ,'u','r','v','e','s','\0',                              15,/*6,*/
    9,  9 ,'y','l','i','n','d','e','r','\0',                      16,/*7,*/
   11, 11 ,'r','o','p','W','i','n','d','o','w','\0',              14,/*8,*/
    0,  9 ,'l','i','p','p','i','n','g','\0',                      7  /*9 */
/* Original table from "tokentbl ribcalls.asc RibRI" is shown below:
 *
 *    9,  9 ,'l','i','p','p','i','n','g','\0',  7,
 *   81,  1 , 'o',
 *   18,  3 ,'l', 'o', 'r',
 *    2,  2 ,'\0',  8,
 *    0,  9 ,'S','a','m','p','l','e','s','\0',  9,
 *   22,  1 , 'n',
 *   14, 14 ,'c','a','t','T','r','a','n','s','f','o','r','m','\0', 10,
 *    0,  3 ,'e','\0', 11,
 *    0,  3 ,'o', 'r', 'd',
 *   13, 13 ,'i','n','a','t','e','S','y','s','t','e','m','\0', 12,
 *    0, 14 ,'S','y','s','T','r','a','n','s','f','o','r','m','\0', 13,
 *   11, 11 ,'r','o','p','W','i','n','d','o','w','\0', 14,
 *    7,  7 ,'u','r','v','e','s','\0', 15,
 *    0,  9 ,'y','l','i','n','d','e','r','\0', 16,
 */
};
char RibRI_D[] = {
   56,  1 , 'e',
    7,  7 ,'c','l','a','r','e','\0', 17,
   11, 11 ,'f','o','r','m','a','t','i','o','n','\0', 18,
   12, 12 ,'p','t','h','O','f','F','i','e','l','d','\0', 19,
    0,  4 ,'t','a', 'i', 'l',
    2,  2 ,'\0', 20,
    0,  7 ,'R','a','n','g','e','\0', 21,
    0,  2 , 'i', 's',
    3,  3 ,'k','\0', 22,
    0,  3 ,'p', 'l', 'a',
    8,  8 ,'c','e','m','e','n','t','\0', 23,
    0,  3 ,'y','\0', 24
};
char RibRI_E[] = {
   13, 13 ,'r','r','o','r','H','a','n','d','l','e','r','\0', 25,
    0,  1 , 'x',
    8,  8 ,'p','o','s','u','r','e','\0', 26,
    0,  8 ,'t','e','r','i','o','r','\0', 27
};
char RibRI_F[] = {
    7,  7 ,'o','r','m','a','t','\0', 28,
    0,  4 ,'r','a', 'm', 'e',
   13, 13 ,'A','s','p','e','c','t','R','a','t','i','o','\0', 29,
    7,  7 ,'B','e','g','i','n','\0', 30,
    0,  5 ,'E','n','d','\0', 31
};
char RibRI_G[] = {
    0,  1 , 'e',
   14, 14 ,'n','e','r','a','l','P','o','l','y','g','o','n','\0', 32,
    0,  5 ,'o','m','e', 't', 'r',
   37,  2 , 'i', 'c',
   15, 15 ,'A','p','p','r','o','x','i','m','a','t','i','o','n','\0', 33,
    0, 16 ,'R','e','p','r','e','s','e','n','t','a','t','i','o','n','\0', 34,
    0,  3 ,'y','\0', 35
};
char RibRI_H[] = {
    6,  6 ,'i','d','e','r','\0', 36,
    0, 12 ,'y','p','e','r','b','o','l','o','i','d','\0', 37
};
char RibRI_I[] = {
    9,  9 ,'d','e','n','t','i','t','y','\0', 38,
   11, 11 ,'l','l','u','m','i','n','a','t','e','\0', 39,
   19,  1 , 'm',
    6,  6 ,'a','g','e','r','\0', 40,
    0,  8 ,'p','l','i','c','i','t','\0', 41,
    0,  9 ,'n','t','e','r','i','o','r','\0', 42
};
char RibRI_L[] = {
    0, 12 ,'i','g','h','t','S','o','u','r','c','e','\0', 43
};
char RibRI_M[] = {
   86,  1 , 'a',
   76,  2 , 'k', 'e',
    6,  6 ,'B','u','m','p','\0', 44,
   21, 21 ,'C','u','b','e','F','a','c','e',
           'E','n','v','i','r','o','n','m','e','n','t','\0', 45,
   20, 20 ,'L','a','t','L','o','n','g',
           'E','n','v','i','r','o','n','m','e','n','t','\0', 46,
    8,  8 ,'S','h','a','d','o','w','\0', 47,
    0,  9 ,'T','e','x','t','u','r','e','\0', 48,
    0,  5 ,'t','t','e','\0', 49,
    0,  5 ,'o','t','i', 'o', 'n',
    7,  7 ,'B','e','g','i','n','\0', 50,
    0,  5 ,'E','n','d','\0', 51
};
char RibRI_N[] = {
    0,  1 , 'u',
    8,  8 ,'C','u','r','v','e','s','\0', 52,
    0,  7 ,'P','a','t','c','h','\0', 53
};
char RibRI_O[] = {
   33,  5 ,'b','j','e', 'c', 't',
    7,  7 ,'B','e','g','i','n','\0', 54,
    5,  5 ,'E','n','d','\0', 55,
    0, 10 ,'I','n','s','t','a','n','c','e','\0', 56,
   18,  1 , 'p',
    7,  7 ,'a','c','i','t','y','\0', 57,
    0,  6 ,'t','i','o','n','\0', 58,
    0, 12 ,'r','i','e','n','t','a','t','i','o','n','\0', 59
};
char RibRI_P[] = {
   30,  1 , 'a',
   10, 10 ,'r','a','b','o','l','o','i','d','\0', 60,
    0,  3 ,'t', 'c', 'h',
    2,  2 ,'\0', 61,
    0,  6 ,'M','e','s','h','\0', 62,
   12, 12 ,'e','r','s','p','e','c','t','i','v','e','\0', 63,
   37,  4 ,'i','x', 'e', 'l',
    8,  8 ,'F','i','l','t','e','r','\0', 64,
    9,  9 ,'S','a','m','p','l','e','s','\0', 65,
    0, 10 ,'V','a','r','i','a','n','c','e','\0', 66,
   51,  1 , 'o',
   39,  4 ,'i','n', 't', 's',
    2,  2 ,'\0', 67,
   17, 17 ,'G','e','n','e','r','a','l',
           'P','o','l','y','g','o','n','s','\0', 68,
    0, 10 ,'P','o','l','y','g','o','n','s','\0', 69,
    0,  7 ,'l','y','g','o','n','\0', 70,
    0,  2 , 'r', 'o',
    9,  9 ,'c','e','d','u','r','a','l','\0', 71,
    0,  9 ,'j','e','c','t','i','o','n','\0', 72
};
char RibRI_Q[] = {
    0,  9 ,'u','a','n','t','i','z','e','\0', 73
};
char RibRI_R[] = {
   60,  1 , 'e',
   11, 11 ,'a','d','A','r','c','h','i','v','e','\0', 74,
   14, 14 ,'l','a','t','i','v','e','D','e','t','a','i','l','\0', 75,
    8,  8 ,'s','o','u','r','c','e','\0', 76,
    0, 18 ,'v','e','r','s','e',
           'O','r','i','e','n','t','a','t','i','o','n','\0', 77,
    0,  7 ,'o','t','a','t','e','\0', 78
};
char RibRI_S[] = {
   22,  1 , 'c',
    5,  5 ,'a','l','e','\0', 79,
    0, 12 ,'r','e','e','n','W','i','n','d','o','w','\0', 80,
   42,  1 , 'h',
   30,  5 ,'a','d','i', 'n', 'g',
   15, 15 ,'I','n','t','e','r','p','o','l','a','t','i','o','n','\0', 81,
    0,  6 ,'R','a','t','e','\0', 82,
    0,  7 ,'u','t','t','e','r','\0', 83,
    6,  6 ,'i','d','e','s','\0', 84,
    5,  5 ,'k','e','w','\0', 85,
   20,  4 ,'o','l', 'i', 'd',
    7,  7 ,'B','e','g','i','n','\0', 86,
    0,  5 ,'E','n','d','\0', 87,
    7,  7 ,'p','h','e','r','e','\0', 88,
    0,  1 , 'u',
   15, 15 ,'b','d','i','v','i','s','i','o','n','M','e','s','h','\0', 89,
    0,  7 ,'r','f','a','c','e','\0', 90
};
char RibRI_T[] = {
   19, 19 ,'e','x','t','u','r','e',
           'C','o','o','r','d','i','n','a','t','e','s','\0', 91,
    6,  6 ,'o','r','u','s','\0', 92,
    0,  1 , 'r',
   37,  3 ,'a', 'n', 's',
   24,  4 ,'f','o', 'r', 'm',
    2,  2 ,'\0', 93,
    7,  7 ,'B','e','g','i','n','\0', 94,
    0,  5 ,'E','n','d','\0', 95,
    0,  6 ,'l','a','t','e','\0', 96,
    0,  9 ,'i','m','C','u','r','v','e','\0', 97
};
char RibRI_W[] = {
    0,  4 ,'o','r', 'l', 'd',
    7,  7 ,'B','e','g','i','n','\0', 98,
    0,  5 ,'E','n','d','\0', 99
};

char RibRI_v[] = {
    0,  8 ,'e','r','s','i','o','n','\0', 100
};


char *gRibRITokenTables[] = { 
   RibRI_A, RibRI_B, RibRI_C, RibRI_D, RibRI_E, RibRI_F, 
   RibRI_G, RibRI_H, RibRI_I,  /*J*/0,  /*K*/0, RibRI_L, 
   RibRI_M, RibRI_N, RibRI_O, RibRI_P, RibRI_Q, RibRI_R, 
   RibRI_S, RibRI_T,  /*U*/0,  /*V*/0, RibRI_W, RibRI_v
};


int RibGetRIFromString( char *s )
{
   char      *p;
   auto int  c;
   auto int  index = kRIB_ERRRC_INDEX;


   c = s[0];
   /*  Spec has the statements starting with capital letters,
    *    but Pixar's programs do take RIB statements starting
    *    with lower case letters, if the statement's name does
    *    not have any capital letters in the middle of the name
    *    given in the spec.  (That's my latest theory anyway).
    */
   /* Map first letter to an index into gRibRITokenTables[]. */
   if ( c >= 'a' )
   {
      if ( c == 'v' )
      {
         c = 'X'; /* Map "version" RIB statement to letter X. */
      }
      else
      {
         /* Ok, I was doing the following until I started finding
          *   the above to be true.  So I'm just going to declare
          *   all RIB statements that start with lower case letters
          *   to be syntax errors.
          */
         /* c -= ('a' - 'A'); *//* Make first letters uppercase. */
         return kRIB_ERRRC_INDEX;;
      }
   }
   c -= 'A';
   if ( c < 0 || c > ('X'-'A') || !gRibRITokenTables[c] )
   {
      return kRIB_ERRRC_INDEX;
   }
   index = RibReadStringForToken( s+1, gRibRITokenTables[c], &p );
   if ( *p != '\0' )
   {
      index = kRIB_ERRRC_INDEX;
   }
   return index;
}
