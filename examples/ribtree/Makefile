# $RCSfile: Makefile.unix,v $  $Revision: 1.3 $ $Date: 2000/02/28 08:27:03 $
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
# PROGRAMS/LIBRARIES:  ribtree
#
#
#
TOP = ../..

#Put all the compiler and linker flags in one place:
include ${TOP}/config.mak

TARGETPATH = ../../bin
INCPATHS = -I. -I../../include
LIBPATHS = -L../../lib
LIBS = -lsribw
HEADERS =

all: ${TARGETPATH}/ribtree

clean:
	-rm ${TARGETPATH}/ribtree ribtree.o

${TARGETPATH}/ribtree: ribtree.o
	${CC} ${LFLAGS32} -o ${TARGETPATH}/ribtree ribtree.o \
		${LIBPATHS} ${LIBS} 
