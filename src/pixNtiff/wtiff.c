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
 * FILE:  wtiff.c
 *
 * HISTORY:
 *    Although code for a simple TIFF writer function is included in this
 *    file, I have switched over to using a libtiff library.  The TIFF
 *    writer function I wrote is very simple and creates noncompressed 
 *    RGB TIFF bitmaps.   It is included here in case someone is interested
 *    or doesn't have the libtiff library.  Please use the libtiff library
 *    which has a copyright mentioned below.  
 *
 * TO-DO:
 *    Get EXTRASAMPLE_ASSOCALPHA to work.
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          WriteTiff() - using libtiff library.
 *          WriteTiff() - ifdef-ed out.  Uses only standard C libraries.
 *
 *    References:
 *          [ALDU92]  TIFF: Revision 6.0, Final -- June 3, 1992, 
 *                    Aldus Corp., CA, June 1992.
 * 
 *             The libtiff.a library used by this program is:
 *          Copyright (c) 1988, 1989, 1990, 1991, 1992 Sam Leffler
 *             Copyright (c) 1991, 1992 Silicon Graphics, Inc.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#ifndef LIBTIFF
#define  TIFF_WRITTER_PRIV
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bitmap.h"
#include "wtiff.h"

#define SOFTWAREVALUE "Affine Toolkit Version 3C-11"

#ifdef LIBTIFF
#include <tiffio.h>

int WriteTiff( PBITMAP pBmp, char *filename )
{
   TIFF *tif;
   char *p;
   int  pixelsize;
   uint32  i;
   char SoftwareValue[] = SOFTWAREVALUE;

   pixelsize = pBmp->bitspersample*pBmp->nsamples;
   /* Restrict the number of bits per pixel to be divisible by 8. */
   if (pBmp->bitspersample & 0x0007 != pBmp->bitspersample )
   {
      fprintf( stderr, "Unsupported bit depth of %d bit(s) per pixel.\n",
              pixelsize );
      return 1; 
   }
   if ( pBmp->sampleformat==BITMAP_IEEE && pBmp->nsamples!=1 )
      return 1;

   tif = TIFFOpen( filename, "w" );
   if (!tif) 
      return 1;

   TIFFSetField( tif, TIFFTAG_SUBFILETYPE, (uint16)0 );
   TIFFSetField( tif, TIFFTAG_IMAGEWIDTH, (uint32)pBmp->xres );
   TIFFSetField( tif, TIFFTAG_IMAGELENGTH, (uint32)pBmp->yres );
   TIFFSetField( tif, TIFFTAG_XPOSITION, (float)pBmp->xpos );
   TIFFSetField( tif, TIFFTAG_YPOSITION, (float)pBmp->ypos );
   TIFFSetField( tif, TIFFTAG_COMPRESSION, (uint16)COMPRESSION_LZW );
   /* skip setting STRIPOFFSETS */
   /* Note that the flags for orientation in bitmap.h are the TIFF 
    *    orientation values minus one. 
    */
   TIFFSetField( tif, TIFFTAG_ORIENTATION, (uint16)(pBmp->orientation+1) );

   if (pBmp->sampleformat==BITMAP_IEEE)
   {
      TIFFSetField( tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK );
      TIFFSetField( tif, TIFFTAG_BITSPERSAMPLE, 32 );
      TIFFSetField( tif, TIFFTAG_SAMPLESPERPIXEL,(uint16)1);
      TIFFSetField( tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP );
   }
   else
   {
      TIFFSetField( tif, TIFFTAG_PHOTOMETRIC, (pixelsize==8 
                                               ? PHOTOMETRIC_MINISBLACK 
                                               : PHOTOMETRIC_RGB ));
      TIFFSetField( tif, TIFFTAG_BITSPERSAMPLE, (uint16)(pBmp->bitspersample));
      TIFFSetField( tif, TIFFTAG_SAMPLESPERPIXEL,(uint16)(pBmp->nsamples));
      /* Don't set TIFFTAG_PREDICTOR, it makes libtiff corrupt the data 
       *    after it writes out the data as an LZW compressed TIFF file.
       */
      /*TIFFSetField( tif, TIFFTAG_PREDICTOR, (uint16)2 );*/
   }
   /* skip setting STRIPBYTECOUNTS */           
   TIFFSetField( tif, TIFFTAG_XRESOLUTION, 1.0 /*72.0*/ );
   TIFFSetField( tif, TIFFTAG_YRESOLUTION, 1.0 /*72.0*/ );
   TIFFSetField( tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG );
   TIFFSetField( tif, TIFFTAG_RESOLUTIONUNIT, 1/*RESUNIT_INCH*/ );
   TIFFSetField( tif, TIFFTAG_SOFTWARE, SoftwareValue );
   if ( pBmp->nsamples==4 
	&& (pBmp->bitspersample==8||pBmp->bitspersample==16) )
   {
      uint16  us[1];

      /* The libtiff library doesn't seem to set TIFFTAG_SAMPLEFORMAT unless 
       *    there are four channels.
       */
      TIFFSetField( tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT );

      us[0] = EXTRASAMPLE_ASSOCALPHA;
      TIFFSetField( tif, TIFFTAG_EXTRASAMPLES, (uint16)1, us );
   }
   i = (3*8*1024)/TIFFScanlineSize(tif);
   TIFFSetField( tif, TIFFTAG_ROWSPERSTRIP, ( i == 0 ? 1 : i ) );

   p = (char*)(pBmp->pbits);
   for ( i=0; i<pBmp->yres; i++ )
   {
      if (TIFFWriteScanline( tif, p, i, 0 ) < 0)
      {
         printf( "Error writing %s\n", filename );
         break;
      }
      p += pBmp->rowbytes;
   }

   TIFFClose( tif );

   return 0;
}

#else

int WriteTiff( PBITMAP pBmp, char *filename )
{
   static char SoftwareValue[] = SOFTWAREVALUE;
   static char BitsPerSampleValue[] = { 0,8,0,8,0,8 };
   static char XandYResolutionValue[] = { 0x00, 0x0a, 0xfc, 0x80, 
                                       0x00, 0x00, 0x27, 0x10 };
   unsigned long int  RowsPerStripValue;
   unsigned long int  StripByteCountsValue;
   unsigned long int  x,y,offset;

   static char tiffheader[] = {
      'M','M',  /* bytes 0-1  MM - BigEndian, II - LittleEndian           */
      0x00, 0x2A, /* bytes 2-3  42=0x2A stored in BigEndian order         */
      0x00,     /* bytes 4-7  Offset from begining of file to the         */
      0x00,     /*            first Image File Directory (IFD) structure. */
      0x00,     /*            The first byte has offset 0.                */
      0x08      /*            The offset in this case will basically be   */
                /*            the size of the TIFF header.                */
   };
   
   static IFD_ENTRY  ifde[] = { /* array of IFD Entries to append to ifd */
      /* NOTE:  Things marked by "(set later)" can't be hardcoded here and
       *        are set during runtime.
       */
      /* NewSubFileType, TAG = 0xFE */
      0x00, 0xfe,  /* bytes  0-1  Tag ID                                    */
      0x00, LONG_TYPE,  /* bytes  2-3  Field type                           */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /*             (only one value SHORT or LONG             */
      0x00, 0x00,  /* bytes 8-11  The value (if it fits), otherwise offset. */
      0x00, 0x00,  /*             (set later)                               */
      /* ImageWidth, TAG = 0x100 */
      0x01, 0x00,  /* bytes  0-1  Tag ID                                    */
      0x00, 0x00,  /* bytes  2-3  Field type (set later)                    */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /*             (only one value SHORT or LONG             */
      0x00, 0x00,  /* bytes 8-11  The value (if it fits), otherwise offset. */
      0x00, 0x00,  /*             (set later)                               */
      /* ImageLength, TAG = 0x101 */
      0x01, 0x01,  /* bytes  0-1  Tag ID                                    */
      0x00, 0x00,  /* bytes  2-3  Field type (set later)                    */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /*             (Only one value)                          */
      0x00, 0x00,  /* bytes 8-11  The value (if it fits), otherwise offset. */
      0x00, 0x00,  /*             (set later)                               */
      /* BitsPerSample, TAG = 0x102 */
      0x01, 0x02,  /* bytes  0-1  Tag ID                                    */
      0x00, SHORT_TYPE, /* bytes  2-3  Field type                           */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x03,  /*  (three values)                                       */
      0x00, 0x00,  /* bytes 8-11  Use offset. (3 sets of 2 bytes won't fit  */
      0x00, 0x00,  /*             into 4 bytes.  Use an offset. (set later) */
      /* Compression, TAG = 0x103 */
      0x01, 0x03,  /* bytes  0-1  Tag ID                                    */
      0x00, SHORT_TYPE,  /* bytes  2-3  Field type                          */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /* (only 1 value)                                        */
      0x00, 0x01,  /* bytes 8-11  The value of 1.                           */
      0x00, 0x00,  /*   (1==no compression, left justified SHORT_TYPE)      */
                   /*   Inshort put value in 2nd byte not the 4th byte.     */
      /* PhotometricInterpretation, TAG = 0x106 */
      0x01, 0x06,  /* bytes  0-1  Tag ID                                    */
      0x00, SHORT_TYPE,  /* bytes  2-3  Field type                          */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /* (only 1 value)                                        */
      0x00, 0x02,  /* bytes 8-11  The value                                 */
      0x00, 0x00,  /*   (1==no compression, left justified SHORT_TYPE)      */
                   /*   Inshort put value in 2nd byte not the 4th byte.     */
      /* StripOffsets, TAG = 0x111 */
      0x01, 0x11,  /* bytes  0-1  Tag ID                                    */
      0x00, LONG_TYPE,  /* bytes  2-3  Field type                           */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /*             (only one value)                          */
      0x00, 0x00,  /* bytes 8-11  The value (if it fits), otherwise offset. */
      0x00, 0x00,  /*             (set later)                               */
      /* SamplesPerPixel, TAG = 0x115 */
      0x01, 0x15,  /* bytes  0-1  Tag ID                                    */
      0x00, SHORT_TYPE,  /* bytes  2-3  Field type                          */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /* (only 1 value)                                        */
      0x00, 0x03,  /* bytes 8-11  The value of 3.                           */
      0x00, 0x00,  /*   (3==3 color sample R, G and B.  Left justified      */
                   /*   SHORT_TYPE.   Inshort put value in 2nd byte not the */
                   /*   4th byte.                                           */
      /* RowsPerStrip, TAG = 0x116 */
      0x01, 0x16,  /* bytes  0-1  Tag ID                                    */
      0x00, LONG_TYPE,  /* bytes  2-3  Field type                           */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /*             (only one value)                          */
      0x00, 0x00,  /* bytes 8-11  The value (if it fits), otherwise offset. */
      0x00, 0x00,  /*             (set later)                               */
      /* StripByteCounts, TAG = 0x117 */
      0x01, 0x17,  /* bytes  0-1  Tag ID                                    */
      0x00, LONG_TYPE,  /* bytes  2-3  Field type                           */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /*             (only one value)                          */
      0x00, 0x00,  /* bytes 8-11  Offset to value                           */
      0x00, 0x00,  /*             (set later)                               */
      /* XResolution, TAG = 0x11a */
      0x01, 0x1a,  /* bytes  0-1  Tag ID                                    */
      0x00, RATIONAL_TYPE,  /* bytes  2-3  Field type                       */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /*             (Only one value)                          */
      0x00, 0x00,  /* bytes 8-11  The value (if it fits), otherwise offset. */
      0x00, 0x00,  /*             (set later)                               */
      /* YResolution, TAG = 0x11b */
      0x01, 0x1b,  /* bytes  0-1  Tag ID                                    */
      0x00, RATIONAL_TYPE,  /* bytes  2-3  Field type                       */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /*             (Only one value)                          */
      0x00, 0x00,  /* bytes 8-11  The value (if it fits), otherwise offset. */
      0x00, 0x00,  /*             (set later)                               */
      /* PlanarConfiguration, TAG = 0x11c */
      0x01, 0x1c,  /* bytes  0-1  Tag ID                                    */
      0x00, SHORT_TYPE,  /* bytes  2-3  Field type                          */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /* (only 1 value)                                        */
      0x00, 0x01,  /* bytes 8-11  The value of 1.                           */
      0x00, 0x00,  /*   (1==compacked RGBRGBRGB style)                      */
      /* ResolutionUnit, TAG = 0x128 */
      0x01, 0x28,  /* bytes  0-1  Tag ID                                    */
      0x00, SHORT_TYPE,  /* bytes  2-3  Field type                          */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, 0x01,  /* (only 1 value)                                        */
      0x00, 0x02,  /* bytes 8-11  The value of 3.                           */
      0x00, 0x00,  /*   (2==inch (default used).  Value is a left justified */
                   /*   SHORT_TYPE.   Inshort put value in 2nd byte not the */
                   /*   4th byte.  (Big-Endian)                             */
      /* Software, TAG = 0x131 */
      0x01, 0x31,  /* bytes  0-1  Tag ID                                    */
      0x00, ASCII_TYPE,  /* bytes  2-3  Field type                          */
      0x00, 0x00,  /* bytes  4-7  Count of how many of the field            */
      0x00, sizeof(SoftwareValue), /* (length of string for count)         */
      0x00, 0x00,  /* bytes 8-11  Offset to string (set later).             */
      0x00, 0x00,  /*                                                       */
   };
   static char  ifd[2] = {  /* First part of IFD anyway. */
      0x00, sizeof(ifde)/sizeof(IFD_ENTRY),  /* Number of IFD Entries ().   */
   };
   FILE *fp;
   int i;
   char *p;

   if ( !pBmp )
     return 1;

   if ( pBmp->bitspersample!=8 || pBmp->nsamples!=3 ) /* 24 bits per pel */ 
   {
      printf( "Can only do 24bpp.  Please recompile and use libtiff.a\n" );
      return 1; /* can only do RGB */
   }
   x = pBmp->xres;
   y = pBmp->yres;

   /* Calculate the values and offsets in IFD and IFD entries. */

   offset = sizeof(tiffheader) + sizeof(ifd) + sizeof(ifde) + 4;

   /* Set ImageWidth. */
   if (x > 0xffff) /* if x can't fit into a 2 byte value then use 4 bytes */
   {
      ifde[IMAGEWIDTH].fieldtype[1] = SHORT_TYPE; /* [0] already set to 0 */
      ifde[IMAGEWIDTH].valueoffset[3] = (x & 0xff00)>>8; /* value fits  */
      ifde[IMAGEWIDTH].valueoffset[2] = (x & 0x00ff);    /* so write it */
   }
   else
   {
      ifde[IMAGEWIDTH].fieldtype[1] = LONG_TYPE; /* [0] already set to 0 */
      ifde[IMAGEWIDTH].valueoffset[0] = (x & 0xff000000)>>24;
      ifde[IMAGEWIDTH].valueoffset[1] = (x & 0x00ff0000)>>16;
      ifde[IMAGEWIDTH].valueoffset[2] = (x & 0x0000ff00)>>8;
      ifde[IMAGEWIDTH].valueoffset[3] = (x & 0x000000ff);
   }

   /* Set ImageLength. */
   if (y > 0xffff) /* if y can't fit into a 2 byte value then use 4 bytes */
   {
      ifde[IMAGELENGTH].fieldtype[1] = SHORT_TYPE; /* [0] already set to 0 */
      ifde[IMAGELENGTH].valueoffset[0] = (y & 0xff00)>>8;
      ifde[IMAGELENGTH].valueoffset[1] = (y & 0x00ff);
   }
   else
   {
      ifde[IMAGELENGTH].fieldtype[1] = LONG_TYPE; /* [0] already set to 0 */
      ifde[IMAGELENGTH].valueoffset[0] = (y & 0xff000000)>>24;
      ifde[IMAGELENGTH].valueoffset[1] = (y & 0x00ff0000)>>16;
      ifde[IMAGELENGTH].valueoffset[2] = (y & 0x0000ff00)>>8;
      ifde[IMAGELENGTH].valueoffset[3] = (y & 0x000000ff);
   }

   /* Set BitsPerSample (offset to value ). */
   /*   Assume offset doesn't go beyond 2 bytes at this point. */
   ifde[BITSPERSAMPLE].valueoffset[2] = (offset & 0xff00)>>8; 
   ifde[BITSPERSAMPLE].valueoffset[3] = (offset & 0x00ff);

   offset += sizeof(BitsPerSampleValue);

   /* Set RowsPerStrip LONG_TYPE value. */
   /*     It's recommended that this value result in an 8k strip */
    RowsPerStripValue = y; /* Just use y and set one StripByteCountsValue. */
    ifde[ROWSPERSTRIP].valueoffset[0] = ( RowsPerStripValue & 0xff00)>>24; 
    ifde[ROWSPERSTRIP].valueoffset[1] = ( RowsPerStripValue & 0xff00)>>16; 
    ifde[ROWSPERSTRIP].valueoffset[2] = ( RowsPerStripValue & 0xff00)>>8; 
    ifde[ROWSPERSTRIP].valueoffset[3] = ( RowsPerStripValue & 0x00ff);

   /* Set StripByteCounts LONG_TYPE value. */
   StripByteCountsValue = 3 * x * RowsPerStripValue;
   ifde[STRIPBYTECOUNTS].valueoffset[0]=(StripByteCountsValue&0xff000000)>>24; 
   ifde[STRIPBYTECOUNTS].valueoffset[1]=(StripByteCountsValue&0x00ff0000)>>16; 
   ifde[STRIPBYTECOUNTS].valueoffset[2]=(StripByteCountsValue&0x0000ff00)>>8; 
   ifde[STRIPBYTECOUNTS].valueoffset[3]=(StripByteCountsValue&0x000000ff);

   /* Set XResolution (offset to value ). */
   ifde[XRESOLUTION].valueoffset[2] = (offset & 0xff00)>>8; 
   ifde[XRESOLUTION].valueoffset[3] = (offset & 0x00ff); 

   offset += 2*4; /* sizeof of two LONG_TYPEs */

   /* Set YResolution (offset to value ). */
   ifde[YRESOLUTION].valueoffset[2] = (offset & 0xff00)>>8; 
   ifde[YRESOLUTION].valueoffset[3] = (offset & 0x00ff); 

   offset += 2*4; /* sizeof of two LONG_TYPEs */

   /* Set Software ASCII_TYPE */
   ifde[SOFTWARE].valueoffset[2] = ( offset & 0xff00)>>8; 
   ifde[SOFTWARE].valueoffset[3] = ( offset & 0x00ff);

   offset += sizeof(SoftwareValue);

   /* Set StripOffsets LONG_TYPE value. */
   ifde[STRIPOFFSETS].valueoffset[2] = ( offset & 0xff00)>>8; 
   ifde[STRIPOFFSETS].valueoffset[3] = ( offset & 0x00ff);

   fp = fopen( filename, "wb" );
   if (!fp) 
      return 1;

   /* Write the header */ 
   p=&tiffheader[0];
   for ( i=0; i<sizeof(tiffheader); i++ )
     fputc( *p++, fp );

   /* Write first part of IFD (number of IFD Entries). */
   p=&ifd[0];
   for ( i=0; i<sizeof(ifd); i++ )
      fputc( *p++, fp );

   /* Write out the IFD Entries and values that don't fit into 4 bytes. */   
   p=(char*)&ifde[0];
   for ( i=0; i<sizeof(ifde); i++ )
     fputc( *p++, fp );
   
   /* Write the four zeros that mark the end of an IFD */
   for ( i=0; i<4; i++ )
     fputc( '\0', fp );

   /* Write out the values that didn't fit into 4 bytes */

   /* BitsPerSample value 3 SHORT_TYPEs */
   p=&BitsPerSampleValue[0];
   for ( i=0; i<sizeof(BitsPerSampleValue); i++ )
     fputc( *p++, fp );

   /* XResolution value */
   p=&XandYResolutionValue[0];
   for ( i=0; i<sizeof(XandYResolutionValue); i++ )
     fputc( *p++, fp );
   /* YResolution value (same as above) */
   p=&XandYResolutionValue[0];
   for ( i=0; i<sizeof(XandYResolutionValue); i++ )
     fputc( *p++, fp );
   
   /* Software value. */
   p=&SoftwareValue[0];
   for ( i=0; i<sizeof(SoftwareValue); i++ )
     fputc( *p++, fp );

   /* Write out image data. */
   p = (char*)pBmp->pbits;
   for ( i=0; i<(x*y*3); i++ )
     fputc( *p++, fp );

   fclose( fp );

   return 0;
}

#endif
