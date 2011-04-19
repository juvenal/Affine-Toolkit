/* 
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
 * FILE:  wtiff.h
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          Structures used by a simple writer function in tiff.c.
 *
 *    References:
 *          [ALDU92]  TIFF: Revision 6.0, Final -- June 3, 1992, 
 *                    Aldus Corp., CA, June 1992.
 * 
 */
#ifndef __TIFF_WRITER_HEADER__
#define __TIFF_WRITER_HEADER__

#ifdef TIFF_WRITTER_PRIV

#define BYTE_TYPE      1 
#define ASCII_TYPE     2 
#define SHORT_TYPE     3 
#define LONG_TYPE      4
#define RATIONAL_TYPE  5 

enum { 
   NEWSUBFILETYPE,
   IMAGEWIDTH, 
   IMAGELENGTH, 
   BITSPERSAMPLE, 
   COMPRESSION, 
   PHOTOMETRICINTERPRETATION, 
   STRIPOFFSETS,
   SAMPLESPERPIXEL,  
   ROWSPERSTRIP,              
   STRIPBYTECOUNTS,           
   XRESOLUTION,               
   YRESOLUTION,   
   PLANARCONFIGURATION,
   RESOLUTIONUNIT,            
   SOFTWARE 
};                 

   typedef struct _TIFFHEADER {
      char endian[2];    /* bytes 0-1 MM - BigEndian, II - LittleEndian     */
      char magicnumber;  /* bytes 2-3  42=0x2A stored in BigEndian order.   */
      char IFDoffset[4]; /* bytes 4-7  Offset from begining of file to the  */
                         /*            the Image File Directory (IFD)       */
                         /*            struct.  The 1st byte has offset 0.  */
   } TIFFHEADER;
   typedef TIFFHEADER *PTIFFHEADER;

   /* TIFF Field is basically an Image File Directory Entry (IFD Entry) 
    *    structure when the information that a TIFF Field contains all fits
    *    into an IFD structure.  But when the information doesn't all
    *    fit, an IFD can point to another structure to contain the extra
    *    information.  An IFD structure and any "run-off" data together 
    *    make-up a TIFF Field.  
    *
    *    But people still refer to a TIFF Field and an IFD Entry as the same 
    *    thing.
    */
   typedef struct _IFD_ENTRY {
      char  tagID[2];       /* bytes  0-1  Tag ID      */
      char  fieldtype[2];   /* bytes  2-3  Field type  */
      char  count[4];       /* bytes  4-7  Count of how many of the field   */
                            /*             type unit are used.              */
      char  valueoffset[4]; /* bytes 8-11  The value (if it fits).  If the  */
                            /*             value doesn't fit into 4 bytes,  */
                            /*             this field holds an offset from  */
                            /*             the beginning of the file to     */
                            /*             where the data is located.  The  */
                            /*             field count telss how long the   */
                            /*             data is measured in fieldtype    */
                            /*             units.                           */
                            /*             If value is less than 4 bytes,   */
                            /*             left justify it.                 */
                            /*             Example:  BitsPerSample=8,8,8    */
                            /*             valueoffset will be an offset    */
                            /*             from the begining of the file    */
                            /*             (zero based) to 0x00,0x08,0x00,  */
                            /*             0x08,0x00,0x08,0x00,0x00         */
                            /*             (Big-Endian).                    */
   } IFD_ENTRY;
   typedef IFD_ENTRY *PIFD_ENTRY;
   
   typedef struct _IFD {   
      char       IFDEntryCount[2]; /* Number of IFD Entries            */
      IFD_ENTRY  IFDEntries[1];    /* Array of IFD Entries (atleast 1) */
   } IFD;
   typedef IFD *PIFD;
#endif

int WriteTiff( PBITMAP pBmp, char *filename );

#endif
