# $RCSfile: Makefile.unix,v $  $Revision: 1.3 $ $Date: 2000/02/27 07:19:24 $
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
# PROGRAMS/LIBRARIES:  libribrdr.a libsribrdr.a libribtable.a libriberror.a
#
#
#
TOP=..

#Put all the compiler and linker flags in one place:
include ../config.mak

TARGETPATH= ../lib
INCPATHS= -I. -I../include ${ZINC_PATH}
LIBPATHS= -L../lib ${ZLIB_PATH}
LIBS=
HEADERS=

RDRLIBS= ../lib/libribhash.a ../lib/libribrdr.a ../lib/libsribw.a 
RDRHEADERS= ../include/ribhash.h ../include/ripriv.h ../include/ribrdr.h

RDROBJS= readrib.o streamio.o paraml.o seterror.o vcache.o \
	 readfunc.o hintfunc.o ribfunc.o declare.o ribtbl.o hinttbl.o
TABLEOBJS= ritable.o
ERROROBJS= error.o

all: ../lib/libribrdr.a ../lib/libsribrdr.a ../lib/libribtable.a \
     ../lib/libriberror.a strmtest errtest readtest hinttest

../lib/libribrdr.a: ${RDROBJS} ${TABLEOBJS} ${ERROROBJS} ../lib/libribmsg.a
	${AR} ../lib/libribrdr.a ${RDROBJS} ${TABLEOBJS} \
		${ERROROBJS} ../ribhash/errormsg.o

../lib/libsribrdr.a: ${RDROBJS}
	${AR} ../lib/libsribrdr.a ${RDROBJS}

../lib/libribtable.a: ${TABLEOBJS}
	${AR} ../lib/libribtable.a ${TABLEOBJS}

../lib/libriberror.a: ${ERROROBJS}
	${AR} ../lib/libriberror.a ${ERROROBJS}

../lib/libribhash.a: ${RDRHEADERS}
	cd ../ribhash; make -f Makefile; cd ../readrib

../lib/libribmsg.a: ${RDRHEADERS}
	cd ../ribhash; make -f Makefile; cd ../readrib

../lib/libsribw.a: ${RDRHEADERS}
	cd ../sribw; make -f Makefile; cd ../readrib

clean:
	-rm strmtest.o errtest.o readtest.o hinttest.o \
	${RDROBJS} ${TABLEOBJS} ${ERROROBJS} \
	../lib/libribrdr.a ../lib/libsribrdr.a ../lib/libribtable.a \
	../lib/libriberror.a strmtest errtest readtest hinttest

strmtest: strmtest.o ${RDRLIBS} ${RDRHEADERS}
	${CC} -o strmtest strmtest.o ${LFLAGS32} ${LIBPATHS} \
		-lribrdr -lribhash ${ZLIB}

readtest: readtest.o ${RDRLIBS} ${RDRHEADERS}
	${CC} -o readtest readtest.o ${LFLAGS32} ${LIBPATHS} \
		-lribrdr -lsribhash -lsribw ${ZLIB}

hinttest: hinttest.o ${RDRLIBS} ${RDRHEADERS}
	${CC} -o hinttest hinttest.o ${LFLAGS32} ${LIBPATHS} \
		-lribrdr -lsribhash -lsribw ${ZLIB}

errtest: errtest.o ../lib/libriberror.a ../lib/libribmsg.a
	${CC} -o errtest errtest.o ${LFLAGS32} ${LIBPATHS} -lriberror -lribmsg

${RDROBJS}: ${RDRHEADERS}

${TABLEOBJS}: ${RDRHEADERS}

ribfunc.o: ribfunc.h ${RDRHEADERS}
