# $RCSfile: Makefile.unix,v $  $Revision: 1.2 $ $Date: 2000/02/29 07:35:00 $
# 
# Copyright (c) 2000 Thomas E. Burge.  All rights reserved.
# 
# Affine (R) is a registered trademark of Thomas E. Burge
#
# THIS SOFTWARE IS DISTRIBUTED "AS-IS" WITHOUT WARRANTY OF ANY KIND
# AND WITHOUT ANY GUARANTEE OF MERCHANTABILITY OR FITNESS FOR A 
# PARTICULAR PURPOSE.  
#
# In no event shall Thomas E. Burge be liable for any indirect or
# consequential damages or loss of data resulting from use or performance 
# of this software.
# 
# Permission is granted to include compiled versions of this code in
# noncommercially sold software provided the following copyrights and
# notices appear in all software and any related documentation:
#
#                The Affine (R) Libraries and Tools are 
#    Copyright (c) 1995, 1996, 1997, 1998, 1999, 2000 Thomas E. Burge.
#                          All rights reserved.
#        Affine (R) is a registered trademark of Thomas E. Burge.
#
# Also refer to any additional requirements presently set by Pixar 
# in regards to the RenderMan (R) Interface Procedures and Protocol.
#
# Those wishing to distribute this software commercially and those wishing 
# to redistribute the source code must get written permission from the 
# author, Thomas E. Burge.  
#
# Basically for now, I would like folks to get the source code directly 
# from me rather than to have a bunch of different versions circulating
# about.
#
# 
# Affine Toolkit
#
# FILE:  Makefile for Unix systems.  Refer to ../config.mak for 
#        compiler flags that may differ between flavors of Unix.
#
# PROGRAMS/LIBRARIES:  pix2tiff tiff2pix tif2tif pixinfo 
#                      tga2tiff tiff2tga tga2tga tgainfo 
#                      tiff2X z2tiff tiff2fp fp2tiff normfptiff
#                      tiffedge tifftile tiffinvert tifforient
#                      iffinfo iff2iff iff2tiff tiff2iff 
#                      iff2fp busybits showbits cininfo
#
#
#
.SUFFIXES: .x .tif .c .o

TOP=..

#Put all the compiler and linker flags in one place:
include ../config.mak

TARGETPATH=../bin
INCPATHS= -I. -I../include ${TIFF_INCPATH}
LIBS=${TIFF_LIBPATH} ${TIFFLIB}

HEADERS=

.tif.x:
	../bin/tiff2X $< $*

all: ${TARGETPATH}/pix2tiff ${TARGETPATH}/tiff2pix ${TARGETPATH}/tif2tif    \
	${TARGETPATH}/pixinfo ${TARGETPATH}/tiff2X ${TARGETPATH}/tgainfo    \
	${TARGETPATH}/tga2tiff ${TARGETPATH}/tiff2tga ${TARGETPATH}/tga2tga \
	${TARGETPATH}/showbits ${TARGETPATH}/z2tiff ${TARGETPATH}/tiff2fp   \
	${TARGETPATH}/fp2tiff  ${TARGETPATH}/normfptiff                     \
	${TARGETPATH}/tiffedge ${TARGETPATH}/tifftile                       \
	${TARGETPATH}/tiffinvert ${TARGETPATH}/tifforient                   \
	${TARGETPATH}/iffinfo ${TARGETPATH}/iff2iff                         \
	${TARGETPATH}/iff2tiff ${TARGETPATH}/tiff2iff ${TARGETPATH}/iff2fp  \
	${TARGETPATH}/busybits ${TARGETPATH}/cininfo 

${TARGETPATH}/pix2tiff: pix2tiff.o rpix.o wtiff.o bitmap.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/pix2tiff \
		pix2tiff.o rpix.o wtiff.o bitmap.o ${LIBS} 

${TARGETPATH}/tiff2pix: tiff2pix.o rtiff.o wpix.o bitmap.o orient.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tiff2pix \
		tiff2pix.o rtiff.o wpix.o bitmap.o orient.o ${LIBS} 

${TARGETPATH}/tif2tif: tif2tif.o rtiff.o wtiff.o bitmap.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tif2tif \
		tif2tif.o rtiff.o wtiff.o bitmap.o ${LIBS} 

${TARGETPATH}/tifftile: tifftile.o rtiff.o wtiff.o bitmap.o orient.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tifftile \
		tifftile.o rtiff.o wtiff.o bitmap.o orient.o ${LIBS} 

${TARGETPATH}/tiffinvert: tiffinvert.o rtiff.o wtiff.o bitmap.o orient.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tiffinvert \
		tiffinvert.o rtiff.o wtiff.o bitmap.o orient.o ${LIBS} 

${TARGETPATH}/tifforient: tifforient.o rtiff.o wtiff.o bitmap.o orient.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tifforient \
		tifforient.o rtiff.o wtiff.o bitmap.o orient.o ${LIBS} 

${TARGETPATH}/iff2iff: iff2iff.o riff.o wiff.o bitmap.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/iff2iff \
		iff2iff.o riff.o wiff.o bitmap.o 

${TARGETPATH}/iff2tiff: iff2tiff.o riff.o wtiff.o orient.o bitmap.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/iff2tiff \
		iff2tiff.o riff.o wtiff.o orient.o bitmap.o ${LIBS} 

${TARGETPATH}/tiff2iff: tiff2iff.o rtiff.o wiff.o orient.o bitmap.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tiff2iff \
		tiff2iff.o rtiff.o wiff.o orient.o bitmap.o ${LIBS} 

${TARGETPATH}/pixinfo: pixinfo.o 
	${CC} ${LFLAGS32} -o ${TARGETPATH}/pixinfo pixinfo.o 

${TARGETPATH}/cininfo: cininfo.o 
	${CC} ${LFLAGS32} -o ${TARGETPATH}/cininfo cininfo.o 

${TARGETPATH}/iffinfo: iffinfo.o 
	${CC} ${LFLAGS32} -o ${TARGETPATH}/iffinfo iffinfo.o 

${TARGETPATH}/showbits: showbits.o rpix.o rtiff.o rtga.o riff.o \
		bitmap.o orient.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/showbits                \
		showbits.o rpix.o rtiff.o rtga.o riff.o bitmap.o orient.o \
		${LIBS} ${OGL_LIBPATH} ${OGLXLIB}

${TARGETPATH}/tiff2X: tiff2X.o rtiff.o wX.o bitmap.o orient.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tiff2X               \
		tiff2X.o rtiff.o wX.o bitmap.o orient.o ${LIBS} 

${TARGETPATH}/tgainfo: tgainfo.o 
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tgainfo tgainfo.o 

${TARGETPATH}/tga2tiff: tga2tiff.o rtga.o wtiff.o bitmap.o orient.o 
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tga2tiff \
		tga2tiff.o rtga.o wtiff.o bitmap.o orient.o ${LIBS} 

${TARGETPATH}/tiff2tga: tiff2tga.o rtiff.o wtga.o bitmap.o orient.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tiff2tga \
		tiff2tga.o rtiff.o wtga.o bitmap.o orient.o ${LIBS} 

${TARGETPATH}/tga2tga: tga2tga.o rtga.o wtga.o bitmapm.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tga2tga \
		tga2tga.o rtga.o wtga.o bitmapm.o

${TARGETPATH}/z2tiff: z2tiff.o rz.o wtiff.o bitmap.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/z2tiff \
		z2tiff.o rz.o wtiff.o bitmap.o ${LIBS} 

${TARGETPATH}/tiff2fp: tiff2fp.o rtiff.o bitmap.o orient.o 
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tiff2fp \
		tiff2fp.o rtiff.o bitmap.o orient.o ${LIBS} 

${TARGETPATH}/iff2fp: iff2fp.o riff.o bitmap.o orient.o 
	${CC} ${LFLAGS32} -o ${TARGETPATH}/iff2fp \
		iff2fp.o riff.o bitmap.o orient.o ${LIBS} 

${TARGETPATH}/fp2tiff: fp2tiff.o wtiff.o bitmap.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/fp2tiff \
		fp2tiff.o wtiff.o bitmap.o ${LIBS} 

${TARGETPATH}/normfptiff: normfptiff.o rtiff.o wtiff.o bitmap.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/normfptiff \
		normfptiff.o rtiff.o wtiff.o bitmap.o ${LIBS} 

${TARGETPATH}/tiffedge: tiffedge.o rtiff.o wtiff.o bitmap.o orient.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/tiffedge \
		tiffedge.o rtiff.o wtiff.o bitmap.o orient.o ${LIBS} -lm

${TARGETPATH}/busybits: busybits.o wtiff.o bitmap.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/busybits \
		busybits.o wtiff.o bitmap.o ${LIBS}

clean:
	-rm ${TARGETPATH}/pix2tiff ${TARGETPATH}/tiff2pix                    \
	${TARGETPATH}/tif2tif                                                \
	${TARGETPATH}/z2tiff ${TARGETPATH}/tiff2fp ${TARGETPATH}/fp2tiff     \
	${TARGETPATH}/normfptiff ${TARGETPATH}/tiffedge                      \
	${TARGETPATH}/pixinfo ${TARGETPATH}/tiff2X ${TARGETPATH}/tgainfo     \
	${TARGETPATH}/tga2tiff ${TARGETPATH}/tiff2tga ${TARGETPATH}/tga2tga  \
	${TARGETPATH}/showbits ${TARGETPATH}/tifftile                        \
	${TARGETPATH}/tiffinvert ${TARGETPATH}/tifforient                    \
	${TARGETPATH}/iffinfo ${TARGETPATH}/iff2iff                          \
	${TARGETPATH}/iff2tiff ${TARGETPATH}/tiff2iff ${TARGETPATH}/iff2fp   \
	${TARGETPATH}/busybits ${TARGETPATH}/cininfo                         \
	bitmapm.o bitmap.o wtiff.o rtiff.o wpix.o rpix.o wtga.o              \
	rtga.o pix2tiff.o tiff2pix.o tif2tif.o tga2tga.o tgainfo.o           \
	pixinfo.o   tga2tiff.o  tiff2tga.o rz.o z2tiff.o tiff2fp.o           \
	wX.o tiff2X.o showbits.o fp2tiff.o normfptiff.o tiffedge.o           \
	tifftile.o tiffinvert.o tifforient.o iffinfo.o orient.o              \
	iff2iff.o iff2tiff.o tiff2iff.o iff2fp.o busybits.o                  \
        wiff.o riff.o cininfo.o

# bitmapm.o is compiled with LIBTIFF not defined.  Refer to COPTS set above.
bitmapm.o: bitmap.c bitmap.h
	${CC} ${CFLAGS} ${CFLAGS32} -o bitmapm.o -c bitmap.c

bitmap.o: bitmap.c bitmap.h

wtiff.o: wtiff.c bitmap.h wtiff.h

rtiff.o: rtiff.c bitmap.h rtiff.h

wpix.o: wpix.c bitmap.h wpix.h

rpix.o: rpix.c bitmap.h rpix.h

wtga.o: wtga.c bitmap.h wtga.h

rtga.o: rtga.c bitmap.h rtga.h

pix2tiff.o: pix2tiff.c bitmap.h rpix.h wtiff.h

tiff2pix.o: tiff2pix.c bitmap.h rtiff.h wpix.h

tif2tif.o: tif2tif.c bitmap.h rtiff.h wtiff.h

tga2tga.o: tga2tga.c bitmap.h rtga.h wtga.h

affine.x: ../bin/tiff2X affine.tif 
	../bin/tiff2X affine.tif affine.x

showbits.o: showbits.c bitmap.h rpix.h rtiff.h rtga.h affine.x

wX.o: bitmap.h wX.h

tiff2X.o: tiff2X.c bitmap.h rtiff.h wX.h

z2tiff.o: z2tiff.c bitmap.h wtiff.h

tiff2fp.o: tiff2fp.c bitmap.h rtiff.h

iff2fp.o: iff2fp.c bitmap.h riff.h

fp2tiff.o: fp2tiff.c bitmap.h wtiff.h

normfptiff.o: normfptiff.c bitmap.h rtiff.h wtiff.h

orient.o: orient.c bitmap.h

wiff.o: wiff.c bitmap.h wiff.h

riff.o: riff.c bitmap.h riff.h

iff2iff.o: riff.c bitmap.h riff.h wiff.c wiff.h

iff2tiff.o: riff.c bitmap.h riff.h wtiff.c wtiff.h

tiff2iff.o: rtiff.c bitmap.h rtiff.h wiff.c wiff.h

cininfo.o: cininfo.c cineon.h bitmap.h
