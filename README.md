
[![Build Status](https://travis-ci.org/juvenal/Affine-Toolkit.svg?branch=master)](https://travis-ci.org/juvenal/Affine-Toolkit)

Copyright (c) 1995, 1996, 1997, 1998 Thomas E. Burge.  All rights reserved.
    
    Affine (R) is a registered trademark of Thomas E. Burge
   
    THIS SOFTWARE IS DISTRIBUTED "AS-IS" WITHOUT WARRANTY OF ANY KIND
    AND WITHOUT ANY GUARANTEE OF MERCHANTABILITY OR FITNESS FOR A 
    PARTICULAR PURPOSE.  
   
    In no event shall Thomas E. Burge be liable for any indirect or
    consequential damages or loss of data resulting from use or performance 
    of this software.
    
    Permission is granted to include compiled versions of this code in
    noncommercially sold software provided the following copyrights and
    notices appear in all software and any related documentation:
   
                    The Affine (R) Libraries and Tools are 
              Copyright (c) 1995, 1996, 1997, 1998 Thomas E. Burge.
                             All rights reserved.
            Affine (R) is a registered trademark of Thomas E. Burge.
   
    Also refer to any additional requirements presently required by Pixar 
    in regards to the RenderMan (R) Interface Procedures and Protocol.
   
    Those wishing to distribute this software commercially and those wishing 
    to redistribute the source code must get written permission from the 
    author, Thomas E. Burge.  
   
    Basically for now, I would like folks to get the source code directly 
    from me rather than to have a bunch of different versions circulating
    about.
   
   
    Affine Toolkit (Release 4)
    --------------------------

    The Affine Toolkit is based on only published materials regarding the
    RenderMan standard and the general subject of computer graphics.  I 
    developed the code without reference to any proprietary code from 
    Pixar -- inshort this code is mine and is the result of some 
    research, e-mails and also experimentation with the Pixar and BMRT 
    tools such as prman, rendrib and catrib.

    The Affine Toolkit is published to provide some useful tools to folks
    using the RenderMan standard.  Even those with a full-blown setup that
    includes Pixar's RenderMan Artist Tools and the inexpensive Alias 
    AutoStudio package should find the toolkit to be useful.  Those with a 
    slightly cheaper setup such as a system running Amapi, Sketch! or other
    PC/Mac based applications that export RIBs should also find the tools 
    to be extremely helpful.

    Feed back on the toolkit is appreciated.  I will check e-mail at the
    Affine domain name probably on a weekly basis depending on work.  If 
    you have found a bug or have a suggestion, send e-mail to 
    teb@affine.com.  Please do not send "junk e-mail" to this account.  

    Hopefully this work will provide some answers to those developing 
    code to export RIB files.  It would be great to see more modeling 
    packages support the RenderMan standard and export good valid RIB 
    files.  Also hopefully, some folks will write some cool utilities 
    that the rest of us can use.

    I have tried to give credit to where credit is due.  You'll find that
    each C file has a header that lists books, articles and e-mails that 
    were referred to or provided inspiration in writing the code.  Many
    books give a huge list of books and articles, the ones listed in the
    C code are the actual references that were used not just an endless
    list.  One or two books are out of print, but are accessible through 
    inter-library loans.  You should still be able to assemble a good 
    reference library based on the references listed.

    Please follow the copyright rules given in the license file included
    in the Affine Toolkit.  


    What's New in this Release
    --------------------------
   
        Documentation changes and bug fixes.
        

    List of the Affine Toolkit's Tools and Libraries
    ------------------------------------------------

    The toolkit includes the following tools and libraries:

       bin2hex     fp2tiff     normfptiff  ribfixer    tga2tiff    tiff2tga
       bin2oct     hex2bin     oct2bin     ribobjmod   tgainfo     tiffedge
       dbl2oct     hex2dec     pix2tiff    ribtree     tif2tif     tokentbl
       dec2hex     ibm2unix    pixinfo     showbits    tiff2X      typerib
       flt2oct     isaprime    ribbish     str2oct     tiff2fp     untab
       mac2unix    ribdump     tga2tga     tiff2pix    z2tiff

       libribhash.a       libribtable.a      libsribrdr.a
       libribattrstore.a  libribmsg.a        libribtables.a     
       libribconst.a      libribnop.a        libribtrimstore.a
       libribcparm.a      libribrdr.a        libributil.a
       libriberror.a      libribstr.a        libsribhash.a
       libsribw.a

    Beta Version
    ------------

    This is a beta version of the Affine Toolkit version 3C-11 
    release 4 for April 7, 1998.  Version 3C-12 will be the first 
    GM version.  (The version numbers might be recognizable to 
    those animation history buffs out there.)

    I don't plan on any major changes in the basic library APIs.  
    Suggestions are welcome, but any tools written using this version
    of the toolkit should not require any radical changes if any for 
    the next release.  


    Installing and Building the Toolkit (Version 3C-11 Beta Release 4)
    ------------------------------------------------------------------

    For now the Affine Toolkit includes makefiles for SGI and OpenStep.  
    This is a beta version and support for other systems will be added 
    in a later release.  

    To install the Affine Toolkit, unpack the tar file with the command: 
    "tar xvf affine9804.tar".  This will create a subdirectory called 
    "./affine9804/".  If the subdirectory already exists, tar will write
    over files with names matching the tar file's contents.  So be careful.

    Create a link to ./affine9804/ with the name ./affine using the 
    following command:

                  ln -s affine9804 affine

    If you wish to take advantage of PRMan 3.7's new feature of reading
    gzipped RIB files, you will also need to download the zlib package.
    I had to add few lines of code to gzio.c, so you'll want the version
    of zlib modified for the Affine Toolkit.  Install the zlib package
    in ./zlib and at the same level the Toolkit at /Affine.  

    If you do not wish to use the zlib library, comment out the -DRIBZLIB
    option and the ZLIB macro in the affine/readrib/Makefile.

    The tools in affine/pixNtiff/ rely on the existence of libtiff.a on 
    your system.  You can get this library from SGI's web pages.  Place
    this library in /usr/lib or /usr/local/lib.  You can also put the
    library in ./libtiff/libtiff/ if ./ is the same directory affine/ is in.
    The redundant libtiff/ has to do with the way the libtiff tar file has
    been packaged.  You can untar libtiff and setup a link based on the 
    following example:

                  ln -s tiff-v3.4 libtiff

    Change the directory to affine/.  If you have an SGI system type the
    commands "make setupsgi" and then "make sgi".  The first command copies
    files called "Makefile.sgi" located in each source code subdirectory to 
    the file name "Makefile".  The second command does a build of the
    toolkit.  If a clean build is needed, do a "make clean" just before the
    "make sgi" command.

    If you have an Intel OpenStep system type the commands "make 
    setupnext" and then "make next" from the affine/ subdirectory.  The 
    first command copies files called "Makefile.next" located in each 
    source code subdirectory to the file name "Makefile".  The second 
    command does a build of the toolkit.  Missing from the NeXT 
    version of the build are the OpenGL based programs such as "showbits".  
    I will be looking into doing a NeXTStep version of "showbits".

    If you have Linux, sorry but for now you'll have to hack up the 
    Makefiles.  I will add Linux support, but right now the system I had
    running Linux only has OpenStep installed on it.  It will probably be
    easier to use the NeXT Makefiles.  You'll also need something such
    as Mesa to link the OpenGL programs with.

    The SGI Makefiles have macros for SGI's cc, CC and the freeware gcc.  
    I have compiled with all three compilers to remove the different 
    warnings that each came up with.  

    The compiled tools go into affine/bin/ and the libraries are located
    in affine/lib/.  So add affine/bin/ to your .cshrc's PATH or your 
    equivalent shell's resource file.  


    Quick Build Instructions for SGI
    --------------------------------
     Check that you have libtiff.a in "usr/lib/" or "usr/local/lib" or
     do the following:
             cd ~
             tar xvf ~/affine9804.tar
             ln -s affine9804 affine
             ln -s tiff-v3.4 libtiff
     Follow libtiff instructions for building.

     Install modified libz.a by doing the following:
             cd ~
             tar xvf ~/zlib-1.0.4affine.tar
             ln -s zlib-1.0.4affine zlib
     Follow zlib instructions for building.

     Build the Affine Toolkit with the following commands for SGI systems:
             cd affine/
             make setupsgi
             make sgi
     Add ~/affine/bin/ to your shell's path to access the tools.


    Problem Reports
    ---------------

    I have tried to give the toolkit's code a good bit of testing.  But 
    no matter how much you try there will always be something in a project
    this size.  

    If you think you have found a problem or bug in the toolkit, please take
    the following steps before sending e-mail:

    1)  Verify the problem against another related tool such as Pixar's 
        catrib, Adobe's PhotoShop or BMRT's rendrib.  This of course 
        depends on the problem, but give it a shot.

    2)  Have an example whether it be a RIB file, TIFF file or command
        line sequence.  Simplify any RIB files, but don't go over board 
        leaving only a general summary in a few comments.

    3)  Send small binaries in uuencoded form only.

    Send any problems or suggestions to teb@affine.com.


    Plugins
    -------

    Some of the tools were written with plugins in mind.  If I get time
    I will try to include some plugins and display drivers probably in a
    separate package to go along with the Affine Toolkit.

    For those writing there own plugins, I have tried to include some 
    basic, slightly primitive tools to check for memory leaks.  Command
    line utilities can hide memory leaks since the kernel frees the app's 
    resources at termination anyway.  But unlike stand-alone command-line 
    utilities, a plugin architecture has problems with such things as 
    memory leaks.  

    I have tried to do enough testing with the base libraries to avoid 
    having the code in the toolkit be the cause of any problems.  But if 
    you think you have found some problem or have a suggestion for how 
    some functions sould be changed to lend themselves better to plugin
    tools, please let me know.

    The base code in the Affine Toolkit has some simple memory tests that
    get included if you enable the option "-DRIB_MEMORY_TESTS" in the 
    affine/ribhash/Makefile and the Makefiles of other libraries you wish
    to try your code with.


    Notices
    -------

         The RenderMan (R) Interface Procedures and Protocol are:
                       Copyright 1988, 1989, Pixar
                           All Rights Reserved
             Renderman (R) is a registered trademark of Pixar


                        The libtiff.a library is:
                   Copyright (c) 1988-1996 Sam Leffler
              Copyright (c) 1991-1996 Silicon Graphics, Inc.


                         The libz.a library is:
          Copyright (c) 1995-1996 Jean-loup Gailly and Mark Adler


                    The Affine (R) Libraries and Tools are:
              Copyright (c) 1995, 1996, 1997, 1998 Thomas E. Burge
                             All rights reserved
            Affine (R) is a registered trademark of Thomas E. Burge
