# @(#)Makefile 1.2 - 99/06/11 00:16:39
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
# FILE:  Makefile for the Affine Toolkit
#
# PROGRAMS/LIBRARIES:  
#
#	Utilities and libraries in affine/bin affine/lib.
#
#

all:
	@echo Type \"make setupsgi32\" - for SGI MIPS4000/8000 32 bits
	@echo      \"make setuplinux\" - for GNU/Linux
	@echo      \"make setupsun\"   - for SunOS or Solaris
	@echo to copy over the needed makefiles.
	@echo To customize how the code is built edit the file called
	@echo config.mak.
	@echo Type \"make toolkit\" to build the affine toolkit.

toolkit:
	-cd ribdump;  make ; cd ..
	-cd ribhash;  make ; cd ..
	-cd sribw;    make ; cd ..
	-cd readrib;  make ; cd ..
	-cd tokentbl; make ; cd ..
	-cd ributil;  make ; cd ..
	-cd pixNtiff; make ; cd ..
	-cd pcutil;   make ; cd ..
	-cd examples/ribtree;  make ; cd ../..
	-cd examples/closequad;  make ; cd ../..

clean:
	-cd ribdump;  make -f Makefile clean; cd ..
	-cd ribhash;  make -f Makefile clean; cd ..
	-cd sribw;    make -f Makefile clean; cd ..
	-cd readrib;  make -f Makefile clean; cd ..
	-cd tokentbl; make -f Makefile clean; cd ..
	-cd ributil;  make -f Makefile clean; cd ..
	-cd pixNtiff; make -f Makefile clean; cd ..
	-cd pcutil;   make -f Makefile clean; cd ..
	-cd examples/ribtree;  make -f Makefile clean; cd ../..
	-cd examples/closequad;  make -f Makefile clean; cd ../..

setupsgi32:
	cp config.mak.sgi32 config.mak
	chmod u+w config.mak
	cp ribdump/Makefile.unix ribdump/Makefile
	cp ribhash/Makefile.unix ribhash/Makefile
	cp sribw/Makefile.unix sribw/Makefile
	cp readrib/Makefile.unix readrib/Makefile
	cp tokentbl/Makefile.unix tokentbl/Makefile
	cp ributil/Makefile.unix ributil/Makefile
	cp pixNtiff/Makefile.unix pixNtiff/Makefile
	cp pcutil/Makefile.unix pcutil/Makefile
	cp examples/ribtree/Makefile.unix examples/ribtree/Makefile
	cp examples/closequad/Makefile.unix examples/closequad/Makefile
	chmod u+w  ribdump/Makefile
	chmod u+w  ribhash/Makefile
	chmod u+w  sribw/Makefile
	chmod u+w  readrib/Makefile
	chmod u+w  tokentbl/Makefile
	chmod u+w  ributil/Makefile
	chmod u+w  pixNtiff/Makefile
	chmod u+w  pcutil/Makefile
	chmod u+w  examples/ribtree/Makefile
	chmod u+w  examples/closequad/Makefile

setuplinux:
	cp config.mak.linux config.mak
	chmod u+w config.mak
	cp ribdump/Makefile.unix ribdump/Makefile
	cp ribhash/Makefile.unix ribhash/Makefile
	cp sribw/Makefile.unix sribw/Makefile
	cp readrib/Makefile.unix readrib/Makefile
	cp tokentbl/Makefile.unix tokentbl/Makefile
	cp ributil/Makefile.unix ributil/Makefile
	cp pixNtiff/Makefile.unix pixNtiff/Makefile
	cp pcutil/Makefile.unix pcutil/Makefile
	cp examples/ribtree/Makefile.unix examples/ribtree/Makefile
	cp examples/closequad/Makefile.unix examples/closequad/Makefile
	chmod u+w  ribdump/Makefile
	chmod u+w  ribhash/Makefile
	chmod u+w  sribw/Makefile
	chmod u+w  readrib/Makefile
	chmod u+w  tokentbl/Makefile
	chmod u+w  ributil/Makefile
	chmod u+w  pixNtiff/Makefile
	chmod u+w  pcutil/Makefile
	chmod u+w  examples/ribtree/Makefile
	chmod u+w  examples/closequad/Makefile

setupsun:
	cp config.mak.sun config.mak
	chmod u+w config.mak
	cp ribdump/Makefile.unix ribdump/Makefile
	cp ribhash/Makefile.unix ribhash/Makefile
	cp sribw/Makefile.unix sribw/Makefile
	cp readrib/Makefile.unix readrib/Makefile
	cp tokentbl/Makefile.unix tokentbl/Makefile
	cp ributil/Makefile.unix ributil/Makefile
	cp pixNtiff/Makefile.unix pixNtiff/Makefile
	cp pcutil/Makefile.unix pcutil/Makefile
	cp examples/ribtree/Makefile.unix examples/ribtree/Makefile
	cp examples/closequad/Makefile.unix examples/closequad/Makefile
	chmod u+w  ribdump/Makefile
	chmod u+w  ribhash/Makefile
	chmod u+w  sribw/Makefile
	chmod u+w  readrib/Makefile
	chmod u+w  tokentbl/Makefile
	chmod u+w  ributil/Makefile
	chmod u+w  pixNtiff/Makefile
	chmod u+w  pcutil/Makefile
	chmod u+w  examples/ribtree/Makefile
	chmod u+w  examples/closequad/Makefile

