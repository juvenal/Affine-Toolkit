# $RCSfile: Makefile.unix,v $  $Revision: 1.2 $ $Date: 2000/03/01 08:32:50 $
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
# PROGRAMS/LIBRARIES:  libribhash.a  
#
#
#
TOP=..

#Put all the compiler and linker flags in one place:
include ../config.mak

TARGETPATH= ../lib
INCPATHS= -I. -I../include ${ZINC_PATH}
LIBPATHS= -L../lib
LIBS=
HEADERS=

all: ../lib/libribhash.a ../lib/libsribhash.a ../lib/libribconst.a \
     ../lib/libribmsg.a  ../lib/libribcparm.a  \
     ../lib/libribtables.a  ../bin/isaprime \
     hashtest strtest cnsttest cptest

../lib/libribhash.a: hash.o constant.o basistbl.o string.o errormsg.o \
            cparams.o names.o tables.o
	${AR} ../lib/libribhash.a \
            hash.o constant.o basistbl.o string.o errormsg.o cparams.o \
            names.o tables.o

../lib/libsribhash.a: hash.o basistbl.o string.o errormsg.o \
            cparams.o names.o tables.o
	${AR} ../lib/libsribhash.a \
            hash.o basistbl.o string.o errormsg.o cparams.o \
            names.o tables.o

../lib/libribconst.a: constant.o
	${AR} ../lib/libribconst.a constant.o

../lib/libribtables.a: tables.o
	${AR} ../lib/libribtables.a tables.o

../lib/libribmsg.a: errormsg.o
	${AR} ../lib/libribmsg.a errormsg.o

../lib/libribcparm.a: cparams.o names.o
	${AR} ../lib/libribcparm.a cparams.o names.o

clean:
	-rm hashtest strtest cnsttest cptest ../bin/isaprime             \
	   hashtest.o strtest.o cnsttest.o cptest.o tables.o             \
	   hash.o constant.o string.o errormsg.o cparams.o               \
           names.o basistbl.o                                            \
	   ../lib/libribhash.a ../lib/libsribhash.a ../lib/libribconst.a \
	   ../lib/libribmsg.a  ../lib/libribcparm.a ../lib/libribtables.a

hash.o: hash.c hash.h ../include/ribhash.h ../include/ripriv.h 

#Note that atof is not an ansi call and the above use of a -ansi
#   flag can get in the way of compiling the isaprime.c file.
../bin/isaprime: isaprime.c
	cc ${LFLAGS32} -o ../bin/isaprime isaprime.c -lm

hashtest: hashtest.o ../lib/libribhash.a          
	${CC} ${LFLAGS32} -o hashtest hashtest.o ${LIBPATHS} -lribhash

strtest: strtest.o string.o tables.o
	${CC} ${LFLAGS32} -o strtest strtest.o string.o tables.o ${LIBPATHS}

cptest: cptest.o ../lib/libribcparm.a          
	${CC} ${LFLAGS32} -o cptest cptest.o ${LIBPATHS} -lribcparm

cnsttest: cnsttest.o          
	${CC} ${LFLAGS32} -o cnsttest cnsttest.o basistbl.o \
		${LIBPATHS} -lribconst

hashtest.o: hashtest.c ../include/ribhash.h ../include/ripriv.h  

string.o: string.c ../include/ribhash.h ../include/ripriv.h  

tables.o: tables.c ../include/ribtables.h
