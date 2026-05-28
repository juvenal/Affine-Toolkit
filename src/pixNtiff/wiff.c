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
 * FILE:  wiff.c
 *
 *
 * DESCRIPTION:  
 *   
 *    Contains:
 *
 *          WriteIff() - Write an Alias|wavefront Maya IFF bitmap file.
 *
 *    History:
 *        5-24-98  Created.
 *
 *    References:
 *          [AFFI98]  Affine Toolkit, affine/pixNtiff/iffinfo.c,
 *                    1998.
 *          [ALIA98a] Alias|Wavefront, Maya 1.0 ImageFiles.doc.html, 
 *                    110 Richmond St. East, Toronto, Ontario, 
 *                    Canada, M5C 1P1, 1998.
 *          [MUSI91]  Musial, Christopher J., An Integer Square Root 
 *                    Algorithm, Graphics Gems II, edited by James Arvo, 
 *                    Academic Press Limited, 1991, pp. 387-388.
 *
 *    TO-DO: 
 *            Compiling with the flag LITTLE_ENDIAN defined has not been tested
 *            yet.
 *
 *            Go through clean up code and condense somethings.  I was 
 *            writing this code while figuring out the Maya IFF format
 *            based on hardly any documentation.  The resulting code has
 *            some things I would like to rework to make it look better, 
 *            but I believe the code works and reasonably efficiently.  
 *
 *            Find sample grayscale IFF files.  I commented out some code
 *            to handle grayscale.  Ignore that code for now.  I found
 *            some documentation that seems to indicate that grayscale 
 *            images are stored as RGB data where the R, G and B channels
 *            equal the same value.  I need to check on this.  The code
 *            here was assuming that only one channel was stored.
 *
 */
#define __FILE_WIFF__
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "wiff.h"


/* IFF Header Layout:
 *
 *   FOR4 <size> CIMG        -- 4bytes + 4bytes + 4bytes
 *          TBHD 32          -- 4bytes + 4bytes + 32bytes
 *          FOR4 <size> TBMP -- 4bytes + 4bytes + 4bytes
 */
static char  header[4+4+4+4+4+32+4+4+4] = {
   'F','O','R','4','*','*','*','*','C','I','M','G',
   'T','B','H','D', 
#ifdef LITTLE_ENDIAN
   0x20,0x00,0x00,0x00,
#else
   0x00,0x00,0x00,0x20,
#endif
   /* Use 32 characters of filler. */
   '1','2','3','4','5','6','7','8','9','0',
   '1','2','3','4','5','6','7','8','9','0',
   '1','2','3','4','5','6','7','8','9','0',
   '1','2',

   'F','O','R','4','*','*','*','*',
   'T','B','M','P'
};
/* Offset to TBHD data chunk. */
#define TBHD (4+4+4+4+4)

static unsigned char  rect[16384];
static FILE           *iff; 
static unsigned char  b[16];
static unsigned int   size,alignedsize;
static unsigned int   x1,x2,y1,y2;
static unsigned int   width,w,lastwidth;
static unsigned int   height,h,lastheight;
static unsigned int   x,y;

/* Note that group sizes are aligned and datachunks have padding appened
 *    but their sizes are not aligned.  So a RGBA data chunk in a FOR4 group
 *    can have a size set to 17 bytes, but actually be 20 bytes long when
 *    you align the size to include the 3 bytes of padding that were added.
 *
 * The following is an example IFF's structure printed out by the iffinfo
 *    tool: 
 *  % iffinfo sample.iff
 *  File: j
 *    FOR4 133948 CIMG
 *          TBHD 32 width:211 height:211
 *                  prnum:1 prden:1
 *                  flags:0x1 ( RGB )
 *                  bytes:0
 *                  compression:0x1 ( rle )
 *                  tiles:20
 *                  orgx:0 orgy:0
 *          FOR4 133896 TBMP
 *                  RGBA 10928 [x1:   0 y1:   0 x2:  69 y2:  51]
 *                  RGBA 10928 [x1:   0 y1:  52 x2:  69 y2: 103]
 *                  RGBA 10928 [x1:   0 y1: 104 x2:  69 y2: 155]
 *                  RGBA 10928 [x1:   0 y1: 156 x2:  69 y2: 207]
 *                  RGBA 638 [x1:   0 y1: 208 x2:  69 y2: 210]
 *                  RGBA 10928 [x1:  70 y1:   0 x2: 139 y2:  51]
 *                  RGBA 10928 [x1:  70 y1:  52 x2: 139 y2: 103]
 *                  RGBA 10928 [x1:  70 y1: 104 x2: 139 y2: 155]
 *                  RGBA 10928 [x1:  70 y1: 156 x2: 139 y2: 207]
 *                  RGBA 638 [x1:  70 y1: 208 x2: 139 y2: 210]
 *                  RGBA 10928 [x1: 140 y1:   0 x2: 209 y2:  51]
 *                  RGBA 10928 [x1: 140 y1:  52 x2: 209 y2: 103]
 *                  RGBA 10928 [x1: 140 y1: 104 x2: 209 y2: 155]
 *                  RGBA 10928 [x1: 140 y1: 156 x2: 209 y2: 207]
 *                  RGBA 638 [x1: 140 y1: 208 x2: 209 y2: 210]
 *                  RGBA 164 [x1: 210 y1:   0 x2: 210 y2:  51]
 *                  RGBA 164 [x1: 210 y1:  52 x2: 210 y2: 103]
 *                  RGBA 164 [x1: 210 y1: 104 x2: 210 y2: 155]
 *                  RGBA 164 [x1: 210 y1: 156 x2: 210 y2: 207]
 *                  RGBA 17 [x1: 210 y1: 208 x2: 210 y2: 210]
 *
 *  Note the the last RGBA chunk above is actually 20 bytes long not 17.
 *
 */
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


/* The code that calls this is set up to allow for the output buffer
 *   to have a length of (length+1).
 */
static int handleRLE( unsigned char *output, 
		      unsigned char *buffer, unsigned int length );
static int handleRLE( unsigned char *output,
		      unsigned char *buffer, unsigned int length )
{
#ifdef TEST_BAD_RLE_COMPRESSION
   /* The following code makes all RLE compressed tiles too large causing
    *    the code calling this function to write each tile in raw data
    *    form.
    * Basically this code is just for test purposes.
    */
   (void)output;
   (void)buffer;
   return length;
#else
   unsigned char  *s, *t, *last, value;
   int  n;

   if (length==1)
   {
      /* Assume there is room for the one byte extra to be written. */
      output[0] = 0;
      output[1] = buffer[0];
      return 2; 
   }

   s = buffer;
   last = buffer + length;
   t = output;

   do
   {
      n = 0;
      value = s[0];
      s++;
      while ( s < last && value==s[0] )
      {
	 s++;
	 n++;
      }
      if (n)
      {
	 t[0] = n;
	 t[0] |= 0x80;
	 t[1] = value;
	 t += 2;
      }
      else
      {
	 t[1] = value;
	 n++;
	 while ( s < last && s[0]!=s[1] )
	 {
	    n++;	
	    t[n] = s[0];
	    s++;
	 }
	 t[0] = n - 1;
	 t += n + 1;
      }
   } while ( s < last );

   n = (int)(t - output);

   return n;
#endif
}


/* The following code has been moved around in this file a few times 
 *    as I answered some questions about the Maya IFF format.  I'll 
 *    go back and clean this up later.  For now the globals and odd
 *    macros will be left here as they've proven to be work so far.
 */
#define CALC_RECTANGLE8(nsamples)                                         \
	       x1 = x*width;                                              \
	       if ( x<w )                                                 \
	       {                                                          \
		  x2 = x1+width-1;                                        \
		  size = width;                                           \
	       }                                                          \
	       else                                                       \
	       {                                                          \
		  x2 = x1+lastwidth-1;                                    \
		  size = lastwidth;                                       \
	       }                                                          \
	       y1 = y*height;                                             \
	       if ( y<h )                                                 \
	       {                                                          \
		  y2 = y1+height-1;                                       \
		  size *= height;                                         \
	       }                                                          \
	       else                                                       \
	       {                                                          \
		  y2 = y1+lastheight-1;                                   \
		  size *= lastheight;                                     \
	       }                                                          \
	       /* Calculate size in bytes of each rectangle. */           \
	       size *= nsamples;

#define CALC_RECTANGLE16(nsamples)                                        \
	       x1 = x*width;                                              \
	       if ( x<w )                                                 \
	       {                                                          \
		  x2 = x1+width-1;                                        \
		  size = width;                                           \
	       }                                                          \
	       else                                                       \
	       {                                                          \
		  x2 = x1+lastwidth-1;                                    \
		  size = lastwidth;                                       \
	       }                                                          \
	       y1 = y*height;                                             \
	       if ( y<h )                                                 \
	       {                                                          \
		  y2 = y1+height-1;                                       \
		  size *= height;                                         \
	       }                                                          \
	       else                                                       \
	       {                                                          \
		  y2 = y1+lastheight-1;                                   \
		  size *= lastheight;                                     \
	       }                                                          \
	       /* Calculate size in bytes of each rectangle. */           \
	       size *= 2*nsamples;

#define WRITE_RECTANGLEINFO()                                             \
	       /* Handle coordinates. */                                  \
	       b[8]  = (x1 & 0xff00)>>8;                                  \
	       b[9]  = (x1 & 0x00ff);                                     \
	       b[10] = (y1 & 0xff00)>>8;                                  \
	       b[11] = (y1 & 0x00ff);                                     \
	       b[12] = (x2 & 0xff00)>>8;                                  \
	       b[13] = (x2 & 0x00ff);                                     \
	       b[14] = (y2 & 0xff00)>>8;                                  \
	       b[15] = (y2 & 0x00ff);                                     \
                                                                          \
	       /* Account for 8 bytes to store coordinates. */            \
	       alignedsize = size + 8;                                    \
                                                                          \
	       /* Don't align size just yet. */                           \
	       b[4] = (alignedsize & 0xff000000) >> 24;                   \
	       b[5] = (alignedsize & 0x00ff0000) >> 16;                   \
	       b[6] = (alignedsize & 0x0000ff00) >> 8;                    \
	       b[7] = (alignedsize & 0x000000ff);                         \
                                                                          \
	       /* Align size to four byte boundary. */                    \
	       alignedsize = alignsize( size + 8, 4 );                    \
		                                                          \
	       if ( fwrite( b, 1, 16, iff ) < 16 )                        \
	          goto Error;


int case24(PBITMAP pBmp);
int case24(PBITMAP pBmp)
{
   unsigned char         *s,*t;
   unsigned int          i,j;

               CALC_RECTANGLE8(3);
	       WRITE_RECTANGLEINFO();
	       t = &rect[0];
	       for ( i=y1;i<=y2;i++)
	       {
		  s = pBmp->pbits + i*pBmp->rowbytes + x1*3;
		  for ( j=x1;j<=x2;j++)
		  {
		     t[0]=s[2]; /* b */
		     t[1]=s[1]; /* g */
		     t[2]=s[0]; /* r */
		     t+=3; 
		     s+=3;
		  }
	       }
	       if ( fwrite( rect, 1, size, iff ) < size )
		  return 1;     

	       /* Write padding. */
	       size+=8; /* Have size also include coordinates. */
	       if (alignedsize!=size)                                     
	       {                                                          
		  i = alignedsize - size;
		  rect[0] = rect[1] = rect[2] = '\0';
		  if ( fwrite( rect, 1, i, iff ) < i )
		     return 1;
	       }                                                          
	       return 0;
Error:
	       return 1;
}


int case32(PBITMAP pBmp);
int case32(PBITMAP pBmp)
{
   unsigned char         *s,*t;
   unsigned int          i,j;

	       CALC_RECTANGLE8(4);
	       WRITE_RECTANGLEINFO();
	       t = &rect[0];
	       for ( i=y1;i<=y2;i++)
	       {
		  s = pBmp->pbits + i*pBmp->rowbytes + x1*4;
		  for ( j=x1;j<=x2;j++)
		  {
		     t[0]=s[3]; /*Write Alpha */
		     t[1]=s[2]; /*      Blue  */
		     t[2]=s[1]; /*      Green */
		     t[3]=s[0]; /*      Red   */
		     t+=4; 
		     s+=4;
		  }
	       }
	       if ( fwrite( rect, 1, size, iff ) < size )
		  return 1;

	       /* Data aligns to 4 bytes, so don't bother with padding. */

	       return 0;
Error:
	       return 1;
}


int case48(PBITMAP pBmp);
int case48(PBITMAP pBmp)
{
   unsigned char         *s,*t;
   unsigned int          i,j;

	       CALC_RECTANGLE16(3);
	       WRITE_RECTANGLEINFO();
	       t = &rect[0];
	       for ( i=y1;i<=y2;i++)
	       {
		  s = pBmp->pbits + i*pBmp->rowbytes + x1*6;
		  for ( j=x1;j<=x2;j++)
		  {
#ifdef LITTLE_ENDIAN
		     t[1]=s[5];
		     t[0]=s[4];
		     t[3]=s[3];
		     t[2]=s[2];
		     t[5]=s[1];
		     t[4]=s[0];
#else
		     t[0]=s[4]; /* r */
		     t[1]=s[5]; /* r */
		     t[2]=s[2]; /* g */
		     t[3]=s[3]; /* g */
		     t[4]=s[0]; /* b */
		     t[5]=s[1]; /* b */
#endif
		     t+=6; 
		     s+=6;
		  }
	       }
	       if ( fwrite( rect, 1, size, iff ) < size )
		  return 1;

	       /* Handle padding. */
	       size+=8; /* Have size also include coordinates. */
	       if (alignedsize!=size)                                     
	       {                                                          
		  i = alignedsize - size;
		  rect[0] = rect[1] = rect[2] = '\0';
		  if ( fwrite( rect, 1, i, iff ) < i )
		     return 1;
	       }                                                          
	       return 0;
Error:
	       return 1;
}


int case64(PBITMAP pBmp);
int case64(PBITMAP pBmp)
{
   unsigned char         *s,*t;
   unsigned int          i,j;

	       CALC_RECTANGLE16(4);
	       WRITE_RECTANGLEINFO();
	       t = &rect[0];
	       for ( i=y1;i<=y2;i++)
	       {
		  s = pBmp->pbits + i*pBmp->rowbytes + x1*8;
		  for ( j=x1;j<=x2;j++)
		  {
#ifdef LITTLE_ENDIAN
		     t[0] = s[3]; /* r */
		     t[1] = s[2]; /* r */
		     t[2] = s[5]; /* g */
		     t[3] = s[4]; /* g */
		     t[4] = s[7]; /* b */
		     t[5] = s[6]; /* b */
		     t[6] = s[1]; /* a */
		     t[7] = s[0]; /* a */
#else
		     t[0] = s[2]; /* r */
		     t[1] = s[3]; /* r */
		     t[2] = s[4]; /* g */
		     t[3] = s[5]; /* g */
		     t[4] = s[6]; /* b */
		     t[5] = s[7]; /* b */
		     t[6] = s[0]; /* a */
		     t[7] = s[1]; /* a */
#endif
		     t+=8; 
		     s+=8;
		  }
	       }
	       if ( fwrite( rect, 1, size, iff ) < size )
		  return 1;

	       /* Data aligns to 4 bytes, so don't bother with padding. */
	       return 0;
Error:
	       return 1;
}


unsigned int isqrt( unsigned int n );
unsigned int isqrt( unsigned int n )
{
   unsigned int  ans;
   unsigned int  next = n/2;


   if (n<=1)
      return n;
   do
   {
      ans = next;
      next = (next + (n/next))/2;
   } while ( next < ans );
   
   return ans;
}


int WriteIff( PBITMAP pBmp, char *filename, int rle )
{
   unsigned int          flags = 0;
   unsigned int          bytes;
   unsigned int          tiles;
   unsigned int          for4size1;
   unsigned int          for4size2;
   unsigned int          i,j;
   unsigned int          xtiles = 0,ytiles = 0;
   unsigned int          bitsperpixel;


   if ( ( pBmp->sampleformat != BITMAP_UINT8 
	  && pBmp->sampleformat != BITMAP_UINT16 )
	|| (pBmp->bitspersample != 8 
	    && pBmp->bitspersample != 16 )
	|| ( /*pBmp->nsamples != 1 && */ /*Don't support grayscale for now.*/
	    pBmp->nsamples != 3
	    && pBmp->nsamples != 4) 
	|| (pBmp->xres==0 || pBmp->yres==0) )
   {
      return 1;
   }
   
   iff = fopen( filename, "wb" );
   if (!iff) 
      return 1;

   /* Set width. */
   header[TBHD+0] = pBmp->xres >> 24;
   header[TBHD+1] = (pBmp->xres & 0x00ff0000) >> 16;
   header[TBHD+2] = (pBmp->xres & 0x0000ff00) >> 8;
   header[TBHD+3] = (pBmp->xres & 0x000000ff);

   /* Set height. */
   header[TBHD+4] = pBmp->yres >> 24;
   header[TBHD+5] = (pBmp->yres & 0x00ff0000) >> 16;
   header[TBHD+6] = (pBmp->yres & 0x0000ff00) >> 8;
   header[TBHD+7] = (pBmp->yres & 0x000000ff);

   /* Set prnum/prden. */
   header[TBHD+8] = header[TBHD+10] = 0;
   header[TBHD+9] = header[TBHD+11] = 1;

   /* Set flags. */
   if (pBmp->nsamples==4)
   {
      /* RGBA */
      flags = 0x00000003;
   }
   else if (pBmp->nsamples==3) 
   {
      /* RGB */
      flags = 0x00000001;
   }
   else if (pBmp->nsamples==1) 
   {
      /* Grayscale */
      flags = 0x00000010;
   }
   /* There are no else's since the other possibilities were filtered out 
    *    with WriteIff()'s first check. 
    */
   header[TBHD+12] = flags >> 24;
   header[TBHD+13] = (flags & 0x00ff0000) >> 16;
   header[TBHD+14] = (flags & 0x0000ff00) >> 8;
   header[TBHD+15] = (flags & 0x000000ff);   

   /* Set bytes. */
   if (pBmp->sampleformat==BITMAP_UINT8)
      bytes = 0;
   else /* Assume pBmp->sampleformat==BITMAP_UINT16. */
      bytes = 1;
   header[TBHD+16] = (bytes & 0x0000ff00) >> 8;
   header[TBHD+17] = (bytes & 0x000000ff);   

   /* Have i be the number of pixel channel values that can fit into 
    *   the buffer. 
    */
   i = sizeof(rect) / pBmp->nsamples;
   if ( pBmp->sampleformat==BITMAP_UINT16 )
      i = i/2;

   /* Compression should not result in more data than the uncompressed
    *    data, but it is possible that rle compression could take up 
    *    more space.
    * When uncompressed the channels are not written separately.  
    *   The entire pixel is written as ABGR values.
    */
   
   /* TO-DO:
    *    Improve the way the image is divided up into tiles. 
    */
   size = pBmp->xres*pBmp->yres;
   tiles = size/i; 

   if (!tiles||size%i)
      tiles += 1;

   lastwidth = 0;
   lastheight = 0;
   width = height = isqrt(i);
   if ( width )
      xtiles = pBmp->xres / width;
   else
      xtiles = 0;
   if ( height )
      ytiles = pBmp->yres / height;
   else
      height = 0;
   i = xtiles * ytiles;
   while (i<tiles)
   {
      if ( xtiles < ytiles && xtiles < pBmp->xres )
	 xtiles++;
      else if ( ytiles < pBmp->yres )
	 ytiles++;
      else
	 xtiles++;
      i = xtiles * ytiles;
   }
   width = pBmp->xres / xtiles;
   height = pBmp->yres / ytiles;
   x = pBmp->xres%xtiles;
   y = pBmp->yres%ytiles;
   w = xtiles;
   h = ytiles;
   if (x)
   {
      lastwidth = x;
      xtiles++;
   }
   if (y)
   {
      lastheight = y;
      ytiles++;
   }
   tiles = xtiles * ytiles;

   /* Write out the number of tiles. */
   header[TBHD+18] = (tiles & 0x0000ff00) >> 8;
   header[TBHD+19] = (tiles & 0x000000ff);

   /* Set compression. */
   if (rle)
      rle = 1; /* Make certain the integer value is one. */
   header[TBHD+20] = rle >> 24;
   header[TBHD+21] = (rle & 0x00ff0000) >> 16;
   header[TBHD+22] = (rle & 0x0000ff00) >> 8;
   header[TBHD+23] = (rle & 0x000000ff);   

   /* Set orgx/orgy. */
   header[TBHD+24] = header[TBHD+25] = header[TBHD+26] = header[TBHD+27] = 0;
   header[TBHD+28] = header[TBHD+29] = header[TBHD+30] = header[TBHD+31] = 0;
   
   /* Write header. */
   if ( fwrite( header, 1, sizeof(header), iff ) < sizeof(header) )
     goto Error;     

   /* The first size will always atleast include the header size. 
    *   But subtract 12 because the 'FOR4'. 'TBMP', and the first size don't 
    *   count here. 
    */
   for4size1 = sizeof(header)-12;
   for4size2 = 0;

   b[0] = 'R';
   b[1] = 'G';
   b[2] = 'B';
   b[3] = 'A';

   bitsperpixel = pBmp->bitspersample * pBmp->nsamples;
   
   if ( rle )
   {
      static unsigned char  tmp[128];
#ifdef LITTLE_ENDIAN
      static int channelmapRGB16[6] =  { 5, 3, 1, 4, 2, 0 };
      static int channelmapRGBA16[8] = { 7, 5, 3, 1, 6, 4, 2, 0 };
#else
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
       * Note the indices listed are for zero based array:
       *       RhRlGhGlBhBlAhAl (big-endian)
       *       RlRhGlGhBlBhAlAh (little-endian)
       */
      static int channelmapRGBA16[8] = { 6, 4, 2, 0, 7, 5, 3, 1 };
#endif
      static int channelmapRGB8[3] = { 2, 1, 0 };
      static int channelmapRGBA8[4] = { 3, 2, 1, 0 };
      int                 *channelmap = NULL;
      unsigned int        maxbuffer;
      int                 channel;
      unsigned char       *s,*t;
      auto unsigned int   count,n,npixels,nbytes,nrowbytes;
      

      switch (bitsperpixel) {
       case 24:
	 channelmap=channelmapRGB8;
	 break;
       case 32:
	 channelmap=channelmapRGBA8;
	 break;
       case 48:
	 channelmap=channelmapRGB16;
	 break;
       case 64:
	 channelmap=channelmapRGBA16;
	 break;
      }
      nbytes = (pBmp->sampleformat==BITMAP_UINT16 
		? 2*pBmp->nsamples : pBmp->nsamples);
      
      /* Go through the tiles for each channel. */
      for ( x=0; x<xtiles; x++ )
      {
	 for ( y=0; y<ytiles; y++ )
	 {
	    size = 0;
	    /* Set target position. */
	    t = &rect[0];

	    /* Go through each channel. */
	    for ( channel=0; channel<pBmp->nsamples; channel++ )
	    {
	       /* Calculate tile boundaries. */
	       x1 = x*width;
	       if ( x<w )
	       {
		  x2 = x1+width-1;
		  maxbuffer = width * nbytes;
	       }
	       else
	       {
		  x2 = x1+lastwidth-1;
		  maxbuffer = lastwidth * nbytes;
	       }
	       y1 = y*height;      
	       if ( y<h )          
	       {
		  y2 = y1+height-1;
		  maxbuffer *= height;
	       }
	       else                
	       {
		  y2 = y1+lastheight-1;
		  maxbuffer *= lastheight;
	       }
	       /* Subtract two to prevent the case where only one value 
		*    is left to be RLE encoded and only one byte of space 
		*    is left in the rectangle.
		*/
	       if (maxbuffer!=1)
		  maxbuffer-=2; 

	       /* Get source position. */
	       s = (unsigned char*)(pBmp->pbits); 
	       s += (y1*pBmp->xres + x1)*nbytes+channelmap[channel];
	       
	       /* Calculate the number of pixels going across the tile. */
	       npixels = x2-x1+1;

	       /* Calculate the number of bytes to the begining of the next
		*   scanline in the tile.
		*/
	       nrowbytes = pBmp->rowbytes - npixels * nbytes;

	       count = 0;
	       for ( j=y1; j<=y2; j++ )
	       {
		  for ( i=x1; i<=x2; i++ )
		  {
		     tmp[count] = s[0];
		     count++;
		     s += nbytes;
		     if ( &t[count] >= &rect[maxbuffer] )
		     {
			/* This tile is not working out well with RLE 
			 *    compression because the compression is
			 *    actually making the data larger.
			 * Write the tile out as raw data.
			 */
			switch ( bitsperpixel ) {
			 case 24:
			   if (case24(pBmp))
			      goto Error;
			   break;
			 case 32:
			   if (case32(pBmp))
			      goto Error;
			   break;
			 case 48:
			   if (case48(pBmp))
			      goto Error;
			   break;
			 case 64:
			   if (case64(pBmp))
			      goto Error;
			   break;
			}
			/* Move onto the next tile. */
			goto NextTile;
		     }
		     if (count==128)
		     {
			count = handleRLE(t,tmp,count);
			size += count;
			t += count;
			count = 0;
		     }
		  }
		  s += nrowbytes;
	       }
	       if (count)
	       {
		  count = handleRLE(t,tmp,count);
		  size += count;
		  t += count;
		  count = 0;
	       }	       
	    }
	    /* Write tile header. */
	    WRITE_RECTANGLEINFO();
	    
	    /* Write tile image data. */
	    if ( fwrite( rect, 1, size, iff ) < size )
	       goto Error;
	    
	    /* Write padding. */
	    size+=8; /* Have size also include coordinates. */
	    if (alignedsize!=size)                                     
	    {                                                          
	       n = alignedsize - size;
	       rect[0] = rect[1] = rect[2] = '\0';
	       if ( fwrite( rect, 1, n, iff ) < n )
		  goto Error;
	    }
	    
	 NextTile:
	    /* Add 8 for RGBA and size. */
	    for4size2+=alignedsize+8;
	 }
      }
   }
   else /* uncompressed */
   {
      /* Write tiles. */
      switch ( bitsperpixel ) {
	 
	 /* [ALIA98a] states that grayscale is expanded to RGB values. */
	 /* Removed the grayscale code for now. */

       case 24:
	 for ( x=0; x<xtiles; x++ )
	 {
	    for ( y=0; y<ytiles; y++ )
	    {
	       if (case24(pBmp))
		  goto Error;
	       /* Add 8 for RGBA and size. */
	       for4size2+=alignedsize+8;
	    }
	 }  
	 break;
       case 32:
	 for ( y=0; y<ytiles; y++ )
	 {
	    for ( x=0; x<xtiles; x++ )
	    {
	       if (case32(pBmp))
		  goto Error;
	       /* Add 8 for RGBA and size. */
	       for4size2+=alignedsize+8;
	    }
	 }  
	 break;
       case 48:
	 for ( x=0; x<xtiles; x++ )
	 {
	    for ( y=0; y<ytiles; y++ )
	    {
	       if (case48(pBmp))
		  goto Error;
	       /* Add 8 for RGBA and size. */
	       for4size2+=alignedsize+8;
	    }
	 }  
	 break;
       case 64:
	 for ( x=0; x<xtiles; x++ )
	 {
	    for ( y=0; y<ytiles; y++ )
	    {
	       if (case64(pBmp))
		  goto Error;
	       /* Add 8 for RGBA and size. */
	       for4size2+=alignedsize+8;
	    }
	 }  
	 break;
      }
   }

   /* Account for 'TBMP' by adding 4. */
   for4size2 += 4;

   /* 
    *  Go back and patch the size fields by using fseek(). 
    */
   fflush( iff );
   
   for4size1 += for4size2;

   /* First FOR4 size. */
   b[0] =  for4size1 >> 24;
   b[1] = (for4size1 & 0x00ff0000) >> 16;
   b[2] = (for4size1 & 0x0000ff00) >> 8;
   b[3] = (for4size1 & 0x000000ff);
   if ( fseek( iff, (long)4, SEEK_SET ))
      goto Error;
   if ( fwrite( b, 1, 4, iff ) < 4 )
      goto Error;     

   /* First FOR4 size. */
   b[0] =  for4size2 >> 24;
   b[1] = (for4size2 & 0x00ff0000) >> 16;
   b[2] = (for4size2 & 0x0000ff00) >> 8;
   b[3] = (for4size2 & 0x000000ff);
   fseek( iff, (long)TBHD+32+4, SEEK_SET ); 
   if ( fwrite( b, 1, 4, iff ) < 4 )
     goto Error;     

   fclose( iff );

   return 0;

 Error:
   fclose( iff );
   return 1;
}
