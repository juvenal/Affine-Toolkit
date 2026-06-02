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
 * FILE:  iff2tiff.c
 *
 * DESCRIPTION:  Test application.
 *
 *
 *    Contains:
 *
 *    References:
 *
 */
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "bitmap.h"
#include "riff.h"
#include "wtiff.h"

void PrintVersion(const char *toolname);
void PrintHelp(const char *toolname);
int main(int argc, char **argv);

void PrintVersion(const char *toolname) {
    printf("%s version %s\n", toolname, AFFINE_VERSION);
    printf(RAT_COPYRIGHT_STATEMENT);
    printf(RENDERMAN_COPYRIGHT_STATEMENT);
}

void PrintHelp(const char *toolname) {
    printf("%s\n", toolname);
    printf(RAT_COPYRIGHT_STATEMENT);
    printf(RENDERMAN_COPYRIGHT_STATEMENT);
    printf("\nUsage: %s [-8] iff_filename tiff_filename\n"
           "   -8                  Convert 16 bit IFF to 8 bit TIFF.\n"
           "   iff_filename        IFF file to read from.\n"
           "   tiff_filename       TIFF file to write to.\n",
           toolname);
}

int main(int argc, char **argv) {
    PBITMAP iff;
    int s, t;
    unsigned int flags;
    const char *toolname = "iff2tiff";

    if (argc > 1) {
        if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")) {
            PrintVersion(toolname);
            return 0;
        }
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            PrintHelp(toolname);
            return 0;
        }
    }

    if (argc < 3) {
        PrintHelp("iff2tiff");
        return 1;
    }
    if (argv[1][0] == '-') {
        if (argv[1][1] == '8' && argv[1][2] == '\0') {
            flags = READIFF_CONVERT16TO8;
        }
        else {
            PrintHelp(toolname);
            return 1;
        }
        s = 2;
        t = 3;
    }
    else {
        flags = 0;
        s = 1;
        t = 2;
    }

    /* Check that the input file name doesn't match the output file name. */
    if (!strcmp(argv[s], argv[t])) {
        fprintf(stderr, "Filenames can not refer to the same file: \"%s\".\n",
                argv[s]);
        return 1;
    }

    iff = ReadIff(argv[s], flags);

    if (!iff) {
        fprintf(stderr, "Can't read %s\n", argv[s]);
        return 1;
    }

    iff = OrientBitmap(iff, BITMAP_TOPLEFT);

    if (WriteTiff(iff, argv[t])) {
        fprintf(stderr, "Can't write file: \"%s\"\n", argv[t]);
        DestroyBitmap(iff);
        return 1;
    }

    DestroyBitmap(iff);

    return 0;
}
