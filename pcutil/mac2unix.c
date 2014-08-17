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
 * FILE:  mac2unix.c
 *
 * DESCRIPTION:  Utility to convert ASCII files from Macs to follow the
 *               end-of-line conventions of UNIX ASCII files.  
 *   
 *
 *    Contains:
 * 
 *    References:
 *
 */

#include <stdio.h>

int mac2unix( void );
int main( int argc, char **argv );


FILE *fpin;
FILE *fpout;

int main(int argc, char **argv) {
    int i;

    if (argc > 1 && argv[1][0]=='-') {
        if (argv[1][1]=='o' && argv[1][2]=='\0') {
            fpout = fopen(argv[2], "wb");
            if (!fpout) {
                printf("Can't open %s\n", argv[2]);
                return 1;
            }
            i = 3;
        }
        else {
            printf( "mac2unix [-o outputfile] files . . .\n"                     \
                    "   [filename . . .]   If no file names are given then\n"     \
                    "                      mac2unix will use standard input.\n"   \
                    "   [-o outputfile]    UNIX file to write to.\n"              \
                    "                      If no output file name is given then\n"\
                    "                      mac2unix will use standard output.\n" );
            return 1;
        }
    }
    else {
        fpout = stdout;
        i = 1;
    }
    if (i < argc) {
        while (i < argc) {
            fpin = fopen(argv[i],"rb");
            if (!fpin) {
                printf("Can't open %s\n", argv[i]);
                return 1;
            }
        }
        mac2unix();
        fclose(fpin);
        i++;
    }     
    else {
        fpin = stdin;
        mac2unix();
    }
    return 0;
}

int mac2unix(void) {
    int c;

    while (EOF != (c=fgetc(fpin))) {
        if (13 == c) {
            fputc(10, fpout);
        }
        else {
            fputc(c, fpout);
        }
    }
    return 0;
}
