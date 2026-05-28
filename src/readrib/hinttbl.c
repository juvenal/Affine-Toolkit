/* $RCSfile: hinttbl.c,v $  $Revision: 1.1 $ $Date: 1999/06/12 08:30:07 $
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
 * FILE:  hinttbl.c
 *
 * DESCRIPTION:  Table for ReadHint().  Created originally with the 
 *               following line command:
 *
 *                     tokentbl -s hints.asc RibHintsTable
 *   
 *    Contains:  
 *               gRibHintTokenTable[]
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
#define __RIB_FILE_HINTTBL

char gRibHintTokenTable[] = {
   66,  1 , 'C',
   40,  1 , 'a',
   17, 17 ,'m','e','r','a',
           'O','r','i','e','n','t','a','t','i','o','n','\0',  0,
    0, 18 ,'p','a','b','i','l','i','t','i','e','s',
           'N','e','e','d','e','d','\0',  1,
    0,  4 ,'r','e', 'a', 't',
    9,  9 ,'i','o','n','D','a','t','e','\0',  2,
    0,  4 ,'o','r','\0',  3,
   16,  1 , 'F',
    4,  4 ,'o','r','\0',  4,
    0,  7 ,'r','a','m','e','s','\0',  5,
    9,  9 ,'I','n','c','l','u','d','e','\0',  6,
   11, 11 ,'R','e','n','d','e','r','M','a','n','\0',  7,
   19,  1 , 'S',
    6,  6 ,'c','e','n','e','\0',  8,
    0,  8 ,'h','a','d','e','r','s','\0',  9,
    0, 10 ,'T','e','x','t','u','r','e','s','\0', 10
};
