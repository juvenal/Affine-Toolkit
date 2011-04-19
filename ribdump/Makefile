# $RCSfile: Makefile.unix,v $  $Revision: 1.1 $ $Date: 2000/02/28 04:04:52 $
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
# PROGRAMS/LIBRARIES:  ribdump, oct2bin, bin2oct, hex2bin, bin2hex,
#                      flt2oct, dbl2oct, dec2hex, hex2dec.
#
#
#
TOP=..

#Put all the compiler and linker flags in one place:
include ../config.mak

TARGETPATH= ../bin
INCPATHS= -I. -I../include
LIBS=
HEADERS=

all:    ${TARGETPATH}/ribdump                       \
	${TARGETPATH}/bin2oct ${TARGETPATH}/bin2hex \
	${TARGETPATH}/hex2bin ${TARGETPATH}/oct2bin \
	${TARGETPATH}/dbl2oct ${TARGETPATH}/flt2oct \
	${TARGETPATH}/hex2dec ${TARGETPATH}/dec2hex \
	${TARGETPATH}/str2oct dbltest.rib flttest.rib

${TARGETPATH}/ribdump: ribdump.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/ribdump ribdump.o ${LIBS} 

${TARGETPATH}/oct2bin: oct2bin.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/oct2bin oct2bin.o ${LIBS} 

${TARGETPATH}/bin2oct: bin2oct.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/bin2oct bin2oct.o ${LIBS} 

${TARGETPATH}/bin2hex: bin2hex.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/bin2hex bin2hex.o ${LIBS} 

${TARGETPATH}/hex2bin: hex2bin.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/hex2bin hex2bin.o ${LIBS} 

${TARGETPATH}/dbl2oct: dbl2oct.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/dbl2oct dbl2oct.o ${LIBS}

${TARGETPATH}/flt2oct: flt2oct.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/flt2oct flt2oct.o ${LIBS}

${TARGETPATH}/hex2dec: hex2dec.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/hex2dec hex2dec.o ${LIBS}

${TARGETPATH}/dec2hex: dec2hex.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/dec2hex dec2hex.o ${LIBS}

${TARGETPATH}/str2oct: str2oct.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/str2oct str2oct.o ${LIBS}

clean:
	rm ${TARGETPATH}/ribdump                                     \
	${TARGETPATH}/bin2oct ${TARGETPATH}/bin2hex                  \
	${TARGETPATH}/hex2bin ${TARGETPATH}/oct2bin                  \
	${TARGETPATH}/dbl2oct ${TARGETPATH}/flt2oct                  \
	${TARGETPATH}/hex2dec ${TARGETPATH}/dec2hex                  \
	${TARGETPATH}/str2oct                                        \
	dbltest.rib flttest.rib dbltest.oct flttest.oct              \
	ribdump.o oct2bin.o bin2oct.o bin2hex.o hex2bin.o dbl2oct.o  \
	flt2oct.o hex2dec.o dec2hex.o str2oct.o	

dbltest.rib: dbltest.oct

dbltest.oct: ${TARGETPATH}/dbl2oct ${TARGETPATH}/oct2bin header.oct
	cat header.oct > dbltest.oct
	${TARGETPATH}/dbl2oct 1.234567890 >> dbltest.oct
	${TARGETPATH}/dbl2oct 12.34567890 >> dbltest.oct
	${TARGETPATH}/dbl2oct 123.4567890 >> dbltest.oct
	${TARGETPATH}/dbl2oct 1234.567890 >> dbltest.oct
	${TARGETPATH}/dbl2oct 12345.67890 >> dbltest.oct
	${TARGETPATH}/dbl2oct 123456.7890 >> dbltest.oct
	${TARGETPATH}/dbl2oct -1.23456789 >> dbltest.oct
	${TARGETPATH}/oct2bin dbltest.oct > dbltest.rib

flttest.rib: flttest.oct

flttest.oct: ${TARGETPATH}/flt2oct ${TARGETPATH}/oct2bin header.oct Makefile
	cat header.oct > flttest.oct
	${TARGETPATH}/flt2oct 1.234567890 >> flttest.oct
	${TARGETPATH}/flt2oct 12.34567890 >> flttest.oct
	${TARGETPATH}/flt2oct 123.4567890 >> flttest.oct
	${TARGETPATH}/flt2oct 1234.567890 >> flttest.oct
	${TARGETPATH}/flt2oct 12345.67890 >> flttest.oct
	${TARGETPATH}/flt2oct 123456.7890 >> flttest.oct
	${TARGETPATH}/flt2oct -1.23456789 >> flttest.oct
	cat header.oct    >> flttest.oct
	${TARGETPATH}/flt2oct 0.1       >> flttest.oct
	${TARGETPATH}/flt2oct -0.1      >> flttest.oct
	${TARGETPATH}/flt2oct 0.999985  >> flttest.oct
	${TARGETPATH}/flt2oct 0.032768  >> flttest.oct
	${TARGETPATH}/flt2oct -32761    >> flttest.oct
	${TARGETPATH}/flt2oct 234       >> flttest.oct
	${TARGETPATH}/flt2oct -0.32768  >> flttest.oct
	${TARGETPATH}/oct2bin flttest.oct > flttest.rib

