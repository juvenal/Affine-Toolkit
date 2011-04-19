/* 
 * Copyright (c) 1998 Thomas E. Burge.  All rights reserved.  
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
 * FILE:  riff.c
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          ReadIff() - Read an Alias|wavefront Maya IFF bitmap file.
 *
 *    History:
 *        5-24-98  Created.
 *
 *    References:
 *          [AFFI98]  Affine Toolkit, affine/pixNtiff/iffinfo.c,
 *                    1998.
 * 
 *    Notes:
 *       There wasn't any difference when Maya wrote IFF as default 
 *       top-to-bottom or with the bottom-to-top option set.
 *
 *       Compiling with the flag LITTLE_ENDIAN defined has not been tested
 *       yet.
 *
 *    To-Do:
 *       Go through and clean up some of the loops.  For now the code does
 *       work on IFF files from Maya 1.0 IFF and Composer 4.5.
 *    
 */
#define __FILE_RIFF__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"
#include "riff.h"

#ifndef FALSE
#define TRUE  1
#define FALSE 0
#endif

static char *ErrSampleFormat = "ERROR:  Unsupported sample format.\n";

static unsigned int alignsize( unsigned int size, int alignment );
static unsigned int alignsize( unsigned int size, int alignment )
{
   auto unsigned int   mod;


   mod = size % alignment;

   if (mod)
   {
      mod = alignment - mod;
      size += mod;
   }

   return size;
}


PBITMAP ReadIff( char *filename, unsigned int iffflags )
{
   FILE                *iff;
   PBITMAP             pBmp = NULL; 
   unsigned char       b[32];
   unsigned int        chunksize;
   unsigned int        size;
   unsigned int        rgbasize;
   unsigned int        tbhdsize;
   unsigned int        xres, yres;
   unsigned int        flags;
   unsigned int        bytes;
   unsigned int        tiles; /* RGBA structures. */
   unsigned int        tilesz = 0; /* Used for ZBUF tiles in depth maps. */
   unsigned int        tiles2 = 0; /* Used for RGB2 tiles from Composer 4.5. */
   unsigned int        rgb2 = FALSE;
   unsigned int        compressed,tilecompressed;
   int                 orgx;
   int                 orgy;
   unsigned int        nsamples;
   unsigned int        sampleformat;
   unsigned int        bitspersample;
   unsigned int        x1, y1, x2, y2;
   int                 channel;
   auto unsigned char  *p, *pp, *last;
   auto unsigned int   count,i,n,y,npixels;


   iff = fopen( filename, "rb" );
   if (!iff) 
      return NULL;

   /* Read FOR4 <size> CIMG. */
   for (;;)
   {
      if ( fread( b, 1, 8, iff ) < 8 )
	 goto Error;

      size = b[4]<<24 | b[5]<<16 | b[6]<<8 | b[7];
      chunksize = alignsize( size, 4 ); /* add to mac code. */

      if ( size > 4 /* needs to be atleast big enough to store 'CIMG'. */
	   && b[0]=='F' && b[1]=='O' && b[2]=='R' && b[3]=='4' )
      {
	 /* Group type. */
	 if ( fread( b, 1, 4, iff ) < 4 )
	    goto Error;

	 /* Done if group type is CIMG. */
	 if ( b[0]=='C' &&  b[1]=='I' &&  b[2]=='M' &&  b[3]=='G' )
	    break;

	 /* Account for four bytes read for group type. */
	 size -= 4;
      }

      /* Skip to the next block. */
      fseek( iff, (long)chunksize, SEEK_CUR );
   }

   /* Read TBHD. */
   for (;;)
   {
      if ( fread( b, 1, 8, iff ) < 8 )
	 goto Error;
   
      size = b[4]<<24 | b[5]<<16 | b[6]<<8 | b[7];
      chunksize = alignsize( size, 4 ); 

      if ( b[0]=='T' && b[1]=='B' && b[2]=='H' && b[3]=='D' )
      {
	 tbhdsize = size;

	 /* Check if size is correct. */
	 if ( tbhdsize!=24 && tbhdsize!=32 )
	    goto Error;

	 /* Read the TBHD structure.  Refer to Alias's ilib.h header. */
	 if ( fread( b, 1, tbhdsize, iff ) < tbhdsize )
	    goto Error;

	 /* Found TBHD, so move onto the next step. */
	 break;
      }

      /* Skip to the next block. */
      fseek( iff, (long)chunksize, SEEK_CUR );      
   }

   xres = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3];
   yres = b[4]<<24 | b[5]<<16 | b[6]<<8 | b[7];
   /* The values prnum and prdeb appear to always be set to 1 by Maya. */
   /*prnum      = b[8]<<8 | b[9];*/
   /*prden      = b[10]<<8 | b[11];*/
   flags      = b[12]<<24 | b[13]<<16 | b[14]<<8 | b[15];
   bytes      = b[16]<<8 | b[17];
   tiles      = b[18]<<8 | b[19];
   compressed = b[20]<<24 | b[21]<<16 | b[22]<<8 | b[23];
   if (tbhdsize==32)
   {
      orgx = b[24]<<24 | b[25]<<16 | b[26]<<8 | b[27];
      orgy = b[28]<<24 | b[29]<<16 | b[30]<<8 | b[31];
   }
   else
   {
      orgx = 0;
      orgy = 0;
   }
   
   if ( orgx || orgy )
   {
      printf( "Warning:  Nonzero orgx/orgy values in TBHD data chunk.\n" );
   }

   sampleformat = 0;
   nsamples = 0;
   if ( flags & 0x00000003 ) 
   {
      /* See if greyscale flag is set, it shouldn't be. */
      if ( flags & 0x00000010 )
      {
	 printf("ERROR:  Grayscale flag set along with RGB flag.\n");
	 goto Error;
      }

      /* Check for Composer 4.5 flag indicating 12bit channel data. */
      if ( flags & 0x00002000 )
      {
	 tiles2 = tiles;
	 rgb2 = TRUE;
	 sampleformat = BITMAP_UINT16;
	 bitspersample = 16;
      }
      else if (bytes==0)
      {
	 sampleformat = BITMAP_UINT8;
	 bitspersample = 8;
      }
      else if (bytes==1)
      {
	 sampleformat = BITMAP_UINT16;
	 bitspersample = 16;
      }
      else
      {
	 printf( ErrSampleFormat );
	 goto Error;
      }

      /* Check for RGB channels. */
      if ( flags & 0x00000001 )
	 nsamples = 3;

      /* Check for an Alpha channel. */
      if ( flags & 0x00000002 ) 
	 nsamples++; 
   }
   /* Check for Z-buffer.  Although RGBA and Zbuffer can both be 
    *    present, ignore the Z-buffer if RGBA information is given. 
    */
   else if ( flags & 0x00000004 ) 
   {
      /* Zbuffer */
      tilesz = tiles;
      tiles = 0;
      sampleformat = BITMAP_IEEE;
      nsamples = 1;
      bitspersample = 32;

      if (bytes!=0)
      {
	 printf( ErrSampleFormat );
	 goto Error;
      }
   }
   else
   {
      printf( ErrSampleFormat );
      goto Error;
   }

   if ( (iffflags & READIFF_ALLOW8ONLY) && bitspersample!=8 )
   {
      printf( "ERROR:  IFF needs to be 8 bits per channel.\n" );
      goto Error;
   }

   /* 0 -- no compression
    * 1 -- RLE compression
    * 2 -- QRL
    * 3 -- QR4
    */
   if ( compressed != 0 && compressed != 1 /* RLE */ )
   {
      printf( "ERROR:  Unsupported compression scheme.\n" );
      goto Error;
   }

   pBmp = CreateBitmap( xres, yres, 0, 0, 
			nsamples, sampleformat, bitspersample );
   if (!pBmp)
     goto Error;

   /* The 12bits needs to be shifted up by 4 to become 16bits.
    *    This means that 4bits from the lower byte splits across
    *    to the upper byte.  Since the upper and lower bytes are 
    *    stored in separate data chunks that can be read in any order,
    *    The memory has to be cleared so an OR operation can be done.
    */
   if (rgb2)
      memset( pBmp->pbits, '\0', pBmp->yres*pBmp->rowbytes );
   
   /* Read FOR4 <size> TBMP block. */
   for (;;)
   {
      if ( fread( b, 1, 8, iff ) < 8 )
	 goto Error;

      size = b[4]<<24 | b[5]<<16 | b[6]<<8 | b[7];
	       
      chunksize = alignsize( size, 4 ); /* add to mac code. */

      if ( size > 4 /* needs to be atleast big enough to store 'TBHD'. */
	  && b[0]=='F' && b[1]=='O' && b[2]=='R' && b[3]=='4' )
      {
	 /* Read TBHD group type name. */
	 if ( fread( b, 1, 4, iff ) < 4 )
	    goto Error;
	 
	 if ( b[0]=='T' && b[1]=='B' && b[2]=='M' && b[3]=='P' )
	 {
	    /* Found FOR4 <size> TBMP, so move onto the next step. */
	    break;
	 }    

	 /* Account for bytes storing 'TBMP'. */
	 if (chunksize<4)
	    goto Error;
	 chunksize-=4;
      }

      /* Skip to the next block. */
      fseek( iff, (long)chunksize, SEEK_CUR );
   }

   /* Read RGBA/ZBUF/RGB2 data. */
   for (;;)
   {
      if ( fread( b, 1, 8, iff ) < 8 )
	 goto Error;

      rgbasize = size = b[4]<<24 | b[5]<<16 | b[6]<<8 | b[7];

      chunksize = alignsize( size, 4 ); 

      if ( tiles && b[0]=='R' && b[1]=='G' && b[2]=='B' && b[3]=='A' )
      {
	 /* Read RGBA rectangle coordinates. */
	 if ( fread( b, 1, 8, iff ) < 8 )
	    goto Error;
	 if (size<8)
	    goto Error;
	 size-=8;
	 
	 x1 = b[0]<<8 | b[1];
	 y1 = b[2]<<8 | b[3];
	 x2 = b[4]<<8 | b[5];
	 y2 = b[6]<<8 | b[7];
	       
	 if (x1>x2 || y1>y2 || x2>=xres || y2>=yres)
	 {
	    printf("ERROR:  RGBA coordinates invalid.\n" );
	    goto Error;
	 }

	 npixels = x2-x1+1;
	 if (npixels==0)
	 {
	    if (size>0)
	    {
	       printf("ERROR:  RGBA data chunk size invalid.\n" );
	       goto Error;
	    }
	    else
	    {
	       continue;
	    }
	 }

	 /* When RLE compression fails to be smaller than the size of the
	  *    raw data, the tile is written as uncompressed even though
	  *    the flag in the IFF header says RLE compression is used.
	  * To signal these exceptions in writing out tiles, the size of
	  *    the RGBA or RGB2 chunks minus 8 equals the size of the tile
	  *    uncompressed.
	  */
	 n = npixels*(y2-y1+1)*nsamples*(bitspersample>>3) + 8;
	 if ( n==rgbasize )
	 {
	    tilecompressed = FALSE;
	 }
	 else
	    tilecompressed = TRUE;
	 
	 /* Handle Composer 4.5 12bit RGBA data chunks. */
	 if ( rgb2 && tilecompressed )
	 {
	    if ( sampleformat!=BITMAP_UINT16 || nsamples!=4 )
	    {
	       printf( "Only 12bit format with RGBA channels supported.\n" );
	       goto Error;
	    }
	    /* Pixels are stored as ABGR, but we need RGBA, so 
	     *    have channel go from (nsamples-1) down to 0 to reverse the 
	     *    order.  
	     */
	    for (channel=2*(nsamples-1); channel>=0; channel-=2)
	    {
	       /* Loop while there is data to read and y is still in the
		*    rectangle defined by the RGBA coordinates.
		* Note that because y is from y1 up to and including y2
		*    the orientation of the resulting bitmap will be 
		*    bottom row to top row.  
		*/
	       y = y1;
	       n = npixels;
	       /* Set target position. */
	       p = (unsigned char*)(pBmp->pbits);
	       p += 2*((y*xres + x1)*nsamples) + channel;
#ifdef LITTLE_ENDIAN
	       p++;
#endif
	       while ( size && y<=y2 )
	       {
		  /* Read RLE value and alpha value. */
		  if ( fread( b, 1, 2, iff ) < 2 )
		     goto Error;
		  if (size<2)
		     goto Error;
		  size-=2;

		  /* Get length of run. */
		  count = (b[0] & 0x7f); /* Don't add the one yet. */ 

		  /* If the highest bit is not set (0x80), then run length
		   *    encoding is not used and count is the number
		   *    of values that follow and need to be assigned
		   *    to their corresponding pixel.
		   */
		  b[2]=b[1]<<4; 
		  if (b[0] & 0x80)
		  {
		     /* Add one to count here because the b[1] value
		      *    that has already been read can be handled in 
		      *    the loop.  In the case above, the loop handled 
		      *    b[0] values, so the existing b[1] value was 
		      *    handled before going into the loop.
		      */
		     count++;

		     for ( i=0; i<count; i++ )
		     {
			p[0] |= b[2];
			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += 2*((y*xres + x1)*nsamples) + channel;
#ifdef LITTLE_ENDIAN
			   p++;
#endif
			   n = npixels;
			}
			else
			{
			   p+=2*nsamples;
			}
		     }
		  }
		  else
		  {
		     p[0] |= b[2];

		     n--;
		     if ( !n )
		     {
			y++;
			if (y>y2)
			   break;
			/* Set target position. */
			p = (unsigned char*)(pBmp->pbits);
			p += 2*((y*xres + x1)*nsamples) + channel;
#ifdef LITTLE_ENDIAN
			p++;
#endif
			n = npixels;
		     }
		     else
		     {
			p+=2*nsamples;
		     }

		     /* Account for the amount of data that will be read. */
		     if (size<count)
			goto Error;
		     size-=count;
		     
		     while (count)
		     {
			if ( fread( b, 1, 1, iff ) < 1 )
			   goto Error;

			p[0] |= b[0]<<4;

			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += (y*xres + x1)*2*nsamples + channel;
#ifdef LITTLE_ENDIAN
			   p++;
#endif
			   n = npixels;
			}
			else
			{
			   p+=2*nsamples;
			}
			count--;
		     }
		  }
		  /* Depending on size, go back for more data. */
	       } /* End of while(size && y<=y2). */
	    } /* End of for (channel=nsamples; channel>0; channel--). */
	 }
	 else if ( rgb2 && !tilecompressed )
	 {
	    /* No examples of this since Composer only outputs compressed. */
	    printf( "RGB2 uncompressed not supported.\n" );
	    goto Error;
	 }
	 else if ( sampleformat==BITMAP_UINT8
		   && tilecompressed 
		   && (nsamples==1 || nsamples==3 || nsamples==4) ) 
	 {
	    /* Pixels are stored as ABGR, but we need RGBA, so have
	     *    have channel go from (nsamples-1) down to 0 to reverse the 
	     *    order.  
	     */
	    for (channel=nsamples-1; channel>=0; channel--)
	    {
	       /* Loop while there is data to read and y is still in the
		*    rectangle defined by the RGBA coordinates.
		* Note that because y is from y1 up to and including y2
		*    the orientation of the resulting bitmap will be 
		*    bottom row to top row.  
		*/
	       y = y1;
	       n = npixels;
	       /* Set target position. */
	       p = (unsigned char*)(pBmp->pbits);
	       p += (y*xres + x1)*nsamples + channel;
	       while ( size && y<=y2 )
	       {
		  /* Read RLE value and alpha value. */
		  if ( fread( b, 1, 2, iff ) < 2 )
		     goto Error;
		  if (size<2)
		     goto Error;
		  size-=2;

		  /* Get length of run. */
		  count = (b[0] & 0x7f); /* Don't add the one yet. */ 

		  /* If the highest bit is not set (0x80), then run length
		   *    encoding is not used and count is the number
		   *    of values that follow and need to be assigned
		   *    to their corresponding pixel.
		   */
		  if (b[0] & 0x80)
		  {
		     /* Add one to count here because the b[1] value
		      *    already read can be handled in the loop.  
		      *    In the case above, the loop handled b[0]
		      *    values, so the existing b[1] value was handled
		      *    before going into the loop.
		      */
		     count++;

		     for ( i=0; i<count; i++ )
		     {
			*p = b[1];
			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += (y*xres + x1)*nsamples + channel;
			   n = npixels;
			}
			else
			{
			   p+=nsamples;
			}
		     }
		  }
		  else
		  {
		     *p = b[1];

		     n--;
		     if ( !n )
		     {
			y++;
			if (y>y2)
			   break;
			/* Set target position. */
			p = (unsigned char*)(pBmp->pbits);
			p += (y*xres + x1)*nsamples + channel;
			n = npixels;
		     }
		     else
		     {
			p+=nsamples;
		     }

		     /* Account for the amount of data that will be read. */
		     if (size<count)
			goto Error;
		     size-=count;

		     while (count)
		     {
			if ( fread( b, 1, 1, iff ) < 1 )
			   goto Error;

			*p = b[0];
			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += (y*xres + x1)*nsamples + channel;
			   n = npixels;
			}
			else
			{
			   p+=nsamples;
			}
			count--;
		     }
		  }
		  /* Depending on size, go back for more data. */
	       } /* End of while(size && y<=y2). */
	    } /* End of for (channel=nsamples; channel>0; channel--). */
	 }
	 else if ( sampleformat==BITMAP_UINT16
		   && tilecompressed 
		   && (nsamples==1 || nsamples==3 || nsamples==4) ) 
	 {
#ifdef LITTLE_ENDIAN
	    static int channelmapR16[6] = { 1, 0 };
	    static int channelmapRGB16[6] =  { 5, 3, 1, 4, 2, 0 };
	    static int channelmapRGBA16[8] = { 7, 5, 3, 1, 6, 4, 2, 0 };
#else
	    static int channelmapR16[6] = { 0, 1 };
	    /* RGB16 has orders the high (h) and low (l) bytes of the 
	     *    R, G and B channels as: 
	     *       BhGhRhBlGlRl. 
	     *
	     * Note the indices listed are for zero based array:
	     *       RhRlGhGlBhBl (big-endian)
	     *       RlRhGlGhBlBh (little-endian)
	     */  
	    static int channelmapRGB16[6] = { 4, 2, 0, 5, 3, 1 };
	    /* RGBA16 has orders the high (h) and low (l) bytes of the 
	     *    R, G B and A channels as: 
	     *       AhBhGhRhAlBlGlRl.
	     *
	     * Note the indexes listed are for zero based array:
	     *       RhRlGhGlBhBlAhAl (big-endian)
	     *       RlRhGlGhBlBhAlAh (little-endian)
	     */
	    static int channelmapRGBA16[8] = { 6, 4, 2, 0, 7, 5, 3, 1 };
#endif
	    int *channelmap;
	    if (nsamples==3)
	       channelmap=channelmapRGB16;
	    else if (nsamples==4)
	       channelmap=channelmapRGBA16;
	    else if (nsamples==1)
	       channelmap=channelmapR16;

	    for (channel=0; channel<2*nsamples; channel++)
	    {
	       /* Loop while there is data to read and y is still in the
		*    rectangle defined by the RGBA coordinates.
		* Note that because y is from y1 up to and including y2
		*    the orientation of the resulting bitmap will be 
		*    bottom row to top row.  
		*/
	       y = y1;
	       n = npixels;
	       /* Set target position. */
	       p = (unsigned char*)(pBmp->pbits);
	       p += 2*((y*xres + x1)*nsamples) + channelmap[channel];
	       while ( size && y<=y2 )
	       {
		  /* Read RLE value and one pixel value. */
		  if ( fread( b, 1, 2, iff ) < 2 )
		     goto Error;
		  if (size<2)
		     goto Error;
		  size-=2;

		  /* Get length of run. */
		  count = (b[0] & 0x7f); /* Don't add the one yet. */ 

		  /* If the highest bit is not set (0x80), then run length
		   *    encoding is not used and count is the number
		   *    of values that follow and need to be assigned
		   *    to their corresponding pixel.
		   */
		  if (b[0] & 0x80)
		  {
		     /* Add one to count here because the b[1] value
		      *    already read can be handled in the loop.  
		      *    In the case above, the loop handled b[0]
		      *    values, so the existing b[1] value was handled
		      *    before going into the loop.
		      */
		     count++;

		     for ( i=0; i<count; i++ )
		     {
			p[0] = b[1];
			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += 2*((y*xres + x1)*nsamples)+channelmap[channel];
			   n = npixels;
			}
			else
			{
			   p+=2*nsamples;
			}
		     }
		  }
		  else
		  {
		     p[0] = b[1];

		     n--;
		     if ( !n )
		     {
			y++;
			if (y>y2)
			   break;
			/* Set target position. */
			p = (unsigned char*)(pBmp->pbits);
			p += 2*((y*xres + x1)*nsamples)+channelmap[channel];
			n = npixels;
		     }
		     else
		     {
			p+=2*nsamples;
		     }

		     /* Account for the amount of data that will be read. */
		     if (size<count)
			goto Error;
		     size-=count;
		     
		     while (count)
		     {
			if ( fread( b, 1, 1, iff ) < 1 )
			   goto Error;

			p[0] = b[0];
			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += 2*((y*xres + x1))*nsamples+channelmap[channel];
			   n = npixels;
			}
			else
			{
			   p+=2*nsamples;
			}
			count--;
		     }
		  }
		  /* Depending on size, go back for more data. */
	       } /* End of while(size && y<=y2). */
	    } /* End of for (channel=nsamples; channel>0; channel--). */
	 }
	 else if (nsamples==1 && !tilecompressed )
	 {
	    if ( sampleformat==BITMAP_UINT8 )
	    {
	       for ( y=y1; y<=y2; y++ )
	       {
		  /* Set target position. */
		  p = (unsigned char*)(pBmp->pbits);
		  p += (y*xres + x1);
		  
		  /* Read row of ARGB values. */
		  count = npixels;
		  while (count)
		  {
		     if ( fread( b, 1, 1, iff ) < 1 )
			goto Error;
		     if (size<1)
			goto Error;
		     size-=1;
		     p[0] = b[0]; /* r */
		     p += 1;
		     count--;		     
		  }
	       }
	    }
	    else /* sampleformat==BITMAP_UINT16 */
	    {
	       for ( y=y1; y<=y2; y++ )
	       {
		  /* Set target position. */
		  p = (unsigned char*)(pBmp->pbits);
		  p += (y*xres + x1)*2;
		  
		  /* Read row of ARGB values. */
		  count = npixels;
		  while (count)
		  {
		     if ( fread( b, 1, 2, iff ) < 2 )
			goto Error;
		     if (size<2)
			goto Error;
		     size-=2;
#ifdef LITTLE_ENDIAN         
		     p[0] = b[1]; /* r */
		     p[1] = b[0]; /* r */
#else
		     p[0] = b[0]; /* r */
		     p[1] = b[1]; /* r */
#endif
		     p += 2;
		     count--;		     
		  }
	       }
	    }
	 } /* End of "else if (nsamples==1 && !tilecompressed )". */
	 else if (nsamples==3 && !tilecompressed )
	 {
	    if ( sampleformat==BITMAP_UINT8 )
	    {
	       for ( y=y1; y<=y2; y++ )
	       {
		  /* Set target position. */
		  p = (unsigned char*)(pBmp->pbits);
		  p += (y*xres + x1)*3;
		  
		  /* Read row of ARGB values. */
		  count = npixels;
		  while (count)
		  {
		     if ( fread( b, 1, 3, iff ) < 3 )
			goto Error;
		     if (size<3)
			goto Error;
		     size-=3;
		     p[0] = b[2]; /* r */
		     p[1] = b[1]; /* g */
		     p[2] = b[0]; /* b */
		     p += 3;
		     count--;		     
		  }
	       }
	    }
	    else /* sampleformat==BITMAP_UINT16 */
	    {
	       for ( y=y1; y<=y2; y++ )
	       {
		  /* Set target position. */
		  p = (unsigned char*)(pBmp->pbits);
		  p += (y*xres + x1)*6;
		  
		  /* Read row of ARGB values. */
		  count = npixels;
		  while (count)
		  {
		     if ( fread( b, 1, 6, iff ) < 6 )
			goto Error;
		     if (size<6)
			goto Error;
		     size-=6;
#ifdef LITTLE_ENDIAN         
		     p[0] = b[5]; /* r */
		     p[1] = b[4]; /* r */
		     p[2] = b[3]; /* g */
		     p[3] = b[2]; /* g */
		     p[4] = b[1]; /* b */
		     p[5] = b[0]; /* b */
#else
		     p[0] = b[4]; /* r */
		     p[1] = b[5]; /* r */
		     p[2] = b[2]; /* g */	
		     p[3] = b[3]; /* g */
		     p[4] = b[0]; /* b */
		     p[5] = b[1]; /* b */
#endif
		     p += 6;
		     count--;		     
		  }
	       }
	    }
	 } /* End of "else if (nsamples==3 && !compressed )". */
	 else if (nsamples==4 && !tilecompressed )
	 {
	    if ( sampleformat==BITMAP_UINT8 )
	    {
	       for ( y=y1; y<=y2; y++ )
	       {
		  /* Set target position. */
		  p = (unsigned char*)(pBmp->pbits);
		  p += (y*xres + x1)*4;
		  
		  /* Read row of ARGB values. */
		  count = npixels;
		  while (count)
		  {
		     if ( fread( b, 1, 4, iff ) < 4 )
			goto Error;
		     if (size<4)
			goto Error;
		     size-=4;
		     p[0] = b[3]; /* r */
		     p[1] = b[2]; /* g */
		     p[2] = b[1]; /* b */
		     p[3] = b[0]; /* a */
		     p += 4;
		     count--;		     
		  }
	       }
	    }
	    else /* sampleformat==BITMAP_UINT16 */
	    {
	       for ( y=y1; y<=y2; y++ )
	       {
		  /* Set target position. */
		  p = (unsigned char*)(pBmp->pbits);
		  p += (y*xres + x1)*8;
		  
		  /* Read row of ARGB values. */
		  count = npixels;
		  while (count)
		  {
		     if ( fread( b, 1, 8, iff ) < 8 )
			goto Error;
		     if (size<8)
			goto Error;
		     size-=8;
#ifdef LITTLE_ENDIAN         
		     p[0] = b[3]; /* r */
		     p[1] = b[2]; /* r */
		     p[2] = b[5]; /* g */
		     p[3] = b[4]; /* g */
		     p[4] = b[7]; /* b */
		     p[5] = b[6]; /* b */
		     p[6] = b[1]; /* a */
		     p[7] = b[0]; /* a */
#else
		     p[0] = b[2]; /* r */
		     p[1] = b[3]; /* r */
		     p[2] = b[4]; /* g */
		     p[3] = b[5]; /* g */
		     p[4] = b[6]; /* b */
		     p[5] = b[7]; /* b */
		     p[6] = b[0]; /* a */
		     p[7] = b[1]; /* a */
#endif
		     p += 8;
		     count--;		     
		  }
	       }
	    }
	 } /* End of "else if (nsamples==4 && !tilecompressed )". */
	 else
	 {
	    printf("ERROR:  Invalid sampleformat.\n");
	    goto Error;
	 }

	 /* Skip to the next block. */
	 i = chunksize-rgbasize;
	 if (i)
	    fseek( iff, (long)i, SEEK_CUR );

	 tiles--;
      }
      /* It was not an RGBA chunk, so check for ZBUF. */
      else if (tilesz && b[0]=='Z' && b[1]=='B' && b[2]=='U' && b[3]=='F')
      {
	 /* Read ZBUF rectangle coordinates. */
	 if ( fread( b, 1, 8, iff ) < 8 )
	    goto Error;
	 if (size<8)
	    goto Error;
	 size-=8;
	 
	 x1 = b[0]<<8 | b[1];
	 y1 = b[2]<<8 | b[3];
	 x2 = b[4]<<8 | b[5];
	 y2 = b[6]<<8 | b[7];
	       
	 if (x1>x2 || y1>y2 || x2>=xres || y2>=yres)
	 {
	    printf("ERROR:  ZBUF coordinates invalid.\n" );
	    goto Error;
	 }
	 
	 npixels = x2-x1+1;
	 if (npixels==0)
	 {
	    if (size>0)
	    {
	       printf("ERROR:  ZBUF data chunk size invalid.\n" );
	       goto Error;
	    }
	    else
	    {
	       continue;
	    }
	 }

	 /* When RLE compression fails to be smaller than the size of the
	  *    raw data, the tile is written as uncompressed even though
	  *    the flag in the IFF header says RLE compression is used.
	  * To signal these exceptions in writing out tiles, the size of
	  *    the RGBA or RGB2 chunks minus 8 equals the size of the tile
	  *    uncompressed.
	  */
	 n = npixels*(y2-y1+1)*4 + 8;
	 if ( n==rgbasize )
	    tilecompressed = FALSE;
	 else
	    tilecompressed = TRUE;
	 
	 if ( tilecompressed )
	 {
	    for (channel=3; channel>=0; channel--)
	    {
	       /* Loop while there is data to read and y is still in the
		*    rectangle defined by the RGBA coordinates.
		* Note that because y is from y1 up to and including y2
		*    the orientation of the resulting bitmap will be 
		*    bottom row to top row.  
		*/
	       y = y1;
	       n = npixels;
	       /* Set target position. */
	       p = (unsigned char*)(pBmp->pbits);
	       p += (y*xres + x1)*4 + channel;
	       while ( size && y<=y2 )
	       {
		  /* Read RLE value and alpha value. */
		  if ( fread( b, 1, 2, iff ) < 2 )
		     goto Error;
		  if (size<2)
		     goto Error;
		  size-=2;

		  /* Get length of run. */
		  count = (b[0] & 0x7f); /* Don't add the one yet. */ 

		  /* If the highest bit is not set (0x80), then run length
		   *    encoding is not used and count is the number
		   *    of values that follow and need to be assigned
		   *    to their corresponding pixel.
		   */
		  if (b[0] & 0x80)
		  {
		     /* Add one to count here because the b[1] value
		      *    already read can be handled in the loop.  
		      *    In the case above, the loop handled b[0]
		      *    values, so the existing b[1] value was handled
		      *    before going into the loop.
		      */
		     count++;

		     for ( i=0; i<count; i++ )
		     {
			*p = b[1];
			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += (y*xres + x1)*4 + channel;
			   n = npixels;
			}
			else
			{
			   p+=4;
			}
		     }
		  }
		  else
		  {
		     *p = b[1];

		     n--;
		     if ( !n )
		     {
			y++;
			if (y>y2)
			   break;
			/* Set target position. */
			p = (unsigned char*)(pBmp->pbits);
			p += (y*xres + x1)*4 + channel;
			n = npixels;
		     }
		     else
		     {
			p+=4;;
		     }

		     /* Account for the amount of data that will be read. */
		     if (size<count)
			goto Error;
		     size-=count;
		     
		     while (count)
		     {
			if ( fread( b, 1, 1, iff ) < 1 )
			   goto Error;

			*p = b[0];
			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += (y*xres + x1)*4 + channel;
			   n = npixels;
			}
			else
			{
			   p+=4;
			}
			count--;
		     }
		  }
		  /* Depending on size, go back for more data. */
	       } /* End of while(size && y<=y2). */
	    } /* End of for (channel=3; channel>0; channel--). */
	 }
	 else /* not tilecompressed. */
	 {
	    for ( y=y1; y<=y2; y++ )
	    {
	       /* Set target position. */
	       p = (unsigned char*)(pBmp->pbits);
	       p += (y*xres + x1)*4;
	       
	       /* Read row of IEEE values. */
	       count = npixels;
	       while (count)
	       {
		  if ( fread( b, 1, 4, iff ) < 4 )
		     goto Error;
		  if (size<4)
		     goto Error;
		  size-=4;
#ifdef LITTLE_ENDIAN         
		  p[0] = b[3];
		  p[1] = b[2];
		  p[2] = b[1];
		  p[3] = b[0];
#else
		  p[0] = b[0];
		  p[1] = b[1];
		  p[2] = b[2];
		  p[3] = b[3];
#endif
		  p += 4;
		  count--;		     
	       }
	    }
	 }
	 
	 /* Skip to the next block. */
	 i = chunksize-rgbasize;
	 if (i)
	    fseek( iff, (long)i, SEEK_CUR );
	 
	 tilesz--;
      }
      else if (rgb2 && tiles2 
	       && b[0]=='R' && b[1]=='G' && b[2]=='B' && b[3]=='2')
      {
	 /* Read RGBA rectangle coordinates. */
	 if ( fread( b, 1, 8, iff ) < 8 )
	    goto Error;
	 if (size<8)
	    goto Error;
	 size-=8;
	 
	 x1 = b[0]<<8 | b[1];
	 y1 = b[2]<<8 | b[3];
	 x2 = b[4]<<8 | b[5];
	 y2 = b[6]<<8 | b[7];
	       
	 if (x1>x2 || y1>y2 || x2>=xres || y2>=yres)
	 {
	    printf("ERROR:  RGB2 coordinates invalid.\n" );
	    goto Error;
	 }
	 
	 npixels = x2-x1+1;
	 if (npixels==0)
	 {
	    if (size>0)
	    {
	       printf("ERROR:  RGB2 data chunk size invalid.\n" );
	       goto Error;
	    }
	    else
	    {
	       continue;
	    }
	 }

	 /* When RLE compression fails to be smaller than the size of the
	  *    raw data, the tile is written as uncompressed even though
	  *    the flag in the IFF header says RLE compression is used.
	  * To signal these exceptions in writing out tiles, the size of
	  *    the RGBA or RGB2 chunks minus 8 equals the size of the tile
	  *    uncompressed.
	  */
	 n = npixels*(y2-y1+1)*nsamples*(bitspersample>>3) + 8;
	 if ( n==rgbasize )
	    tilecompressed = FALSE;
	 else
	    tilecompressed = TRUE;
	 
	 if ( tilecompressed )
	 {
	    /* Pixels are stored as ABGR, but we need RGBA, so have
	     *    have channel go from 2*(nsamples-1) down to 0 to 
	     *    reverse the order.  
	     */
	    for (channel=2*(nsamples-1); channel>=0; channel-=2)
	    {
	       /* Loop while there is data to read and y is still in the
		*    rectangle defined by the RGBA coordinates.
		* Note that because y is from y1 up to and including y2
		*    the orientation of the resulting bitmap will be 
		*    bottom row to top row.  
		*/
	       y = y1;
	       n = npixels;
	       /* Set target position. */
	       p = (unsigned char*)(pBmp->pbits);
	       p += (y*xres + x1)*2*nsamples + channel;

	       while ( size && y<=y2 )
	       {
		  /* Read RLE value and alpha value. */
		  if ( fread( b, 1, 2, iff ) < 2 )
		     goto Error;
		  if (size<2)
		     goto Error;
		  size-=2;

		  /* Get length of run. */
		  count = (b[0] & 0x7f); /* Don't add the one yet. */ 

		  /* If the highest bit is not set (0x80), then run length
		   *    encoding is not used and count is the number
		   *    of values that follow and need to be assigned
		   *    to their corresponding pixel.
		   */
		  b[2] = b[1]>>4;
		  b[3] = b[1]<<4;
		  if (b[0] & 0x80)
		  {
		     /* Add one to count here because the b[1] value
		      *    already read can be handled in the loop.  
		      *    In the case above, the loop handled b[0]
		      *    values, so the existing b[1] value was handled
		      *    before going into the loop.
		      */
		     count++;

		     for ( i=0; i<count; i++ )
		     {
#ifdef LITTLE_ENDIAN
			p[1] |= b[2];
			p[0] |= b[3];
#else
			p[0] |= b[2];
			p[1] |= b[3];
#endif
			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += (y*xres + x1)*2*nsamples + channel;
			   n = npixels;
			}
			else
			{
			   p+=2*nsamples;
			}
		     }
		  }
		  else
		  {
#ifdef LITTLE_ENDIAN
			p[1] |= b[2];
			p[0] |= b[3];
#else
			p[0] |= b[2];
			p[1] |= b[3];
#endif

		     n--;
		     if ( !n )
		     {
			y++;
			if (y>y2)
			   break;
			/* Set target position. */
			p = (unsigned char*)(pBmp->pbits);
			p += (y*xres + x1)*2*nsamples + channel;
			n = npixels;
		     }
		     else
		     {
			p+=2*nsamples;
		     }

		     /* Account for the amount of data that will be read. */
		     if (size<count)
			goto Error;
		     size-=count;
		     
		     while (count)
		     {
			if ( fread( b, 1, 1, iff ) < 1 )
			   goto Error;

#ifdef LITTLE_ENDIAN
			p[1] |= b[0]>>4;
			p[0] |= b[0]<<4;
#else
			p[0] |= b[0]>>4;
			p[1] |= b[0]<<4;
#endif
			n--; /* Account for how many written to scanline. */

			/* Keep track of when the end of a scanline occurs. */
			if ( !n )
			{
			   y++;
			   if (y>y2)
			      break;
			   /* Set target position. */
			   p = (unsigned char*)(pBmp->pbits);
			   p += (y*xres + x1)*2*nsamples + channel;
			   n = npixels;
			}
			else
			{
			   p+=2*nsamples;
			}
			count--;
		     }
		  }
		  /* Depending on size, go back for more data. */
	       } /* End of while(size && y<=y2). */
	    } /* End of for (channel=nsamples; channel>0; channel--). */
	 }
	 else /* not tilecompressed. */
	 {
	    /* No examples of this since Composer only outputs compressed. */
	    printf( "RGB2 uncompressed not supported.\n" );
	    goto Error;
	 }

	 /* Skip to the next block. */
	 i = chunksize-rgbasize;
	 if (i)
	    fseek( iff, (long)i, SEEK_CUR );
	 
	 tiles2--;
      }
      else
      {
	 /* Skip to the next block. */
	 fseek( iff, (long)chunksize, SEEK_CUR );
      }

      if ( tiles || tilesz || tiles2 )
	 continue;
      else
	 break;
   }

   fclose( iff );
   
   /* Set the orientation to match Maya's IFF data. */
   pBmp->orientation = BITMAP_BOTLEFT;
   
   /* Do a quick and dirty conversion for 16bpp to 8bpp. */
   if ( (READIFF_CONVERT16TO8 & iffflags)
       && pBmp->sampleformat==BITMAP_UINT16 
       && pBmp->bitspersample==16 )
   {
      last = pp = p = pBmp->pbits;
      last += pBmp->rowbytes*pBmp->yres;
#ifdef LITTLE_ENDIAN
      p++; 
#else
      /* Skip first pixel, data is already there with big-endian. */
      p+=2; 
      pp++;
#endif
      while ( p < last )
      {
	 pp[0] = p[0];
	 p+=2;
	 pp++;
      }
      pBmp->rowbytes = pBmp->rowbytes>>1;
      pBmp->sampleformat=BITMAP_UINT8;
      pBmp->bitspersample=8;
   }

   return pBmp;

Error:
   if (pBmp)
      free( pBmp );
   fclose( iff );
   return NULL;
}

