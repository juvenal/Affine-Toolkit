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
 * FILE:  iffinfo.c
 *
 * DESCRIPTION:  Utility to print description of Alias/Wavefront IFF files.
 *   
 *               This tool can also dump out information on Maya's scene
 *               files and AIFF Audio files.  If you figure out more of the 
 *               tags, let me know and I'll look into adding them into this
 *               tool.
 *
 *    Contains:
 * 
 *    References:
 *          [ALIA98a] Alias|Wavefront, Maya 1.0 ImageFiles.doc.html, 
 *                    110 Richmond St. East, Toronto, Ontario, 
 *                    Canada, M5C 1P1, 1998.
 *          [ALIA98b] Alias|Wavefront, Technical Support line and e-mails
 *                    to get English translation of French comments in 
 *                    ilib.h, May 15-27, 1998.
 *          [ALIA98c] Alias|Wavefront, Maya 1.0 ILgetsize.html
 *                    110 Richmond St. East, Toronto, Ontario, 
 *                    Canada, M5C 1P1, 1998.
 *          [ALIA98d] Alias|Wavefront, Maya 1.0 ILsetsize.html
 *                    110 Richmond St. East, Toronto, Ontario, 
 *                    Canada, M5C 1P1, 1998.
 *          [ALIA98e] Alias|Wavefront, ComposerLite Version 4.5 (included
 *                    with Maya 1.0), 110 Richmond St. East, Toronto, 
 *                    Ontario, 1998, "File/Export Image...".
 *                    Canada, M5C 1P1, 1998.
 *          [ALIA98f] Alias|Wavefront, flib.h
 *                    110 Richmond St. East, Toronto, Ontario, 
 *                    Canada, M5C 1P1, 1998.
 *          [METR98]  Metrowerks Corporation, CodeWarrior 2.1,
 *                    headers:UniversalHeaders:AIFF.H, 1998.
 *          [MURR96]  Murray, James D. and VanRyper, William, Graphics
 *                    File Formats: Second Edition, O'Reilly & Associates, 
 *                    Inc., 1996, pp. 112, pp. 465-483.
 *
 *    Notes:
 *          The main references of real use in figuring out Maya's IFF
 *          format proved to be the IFF section in [MURR96]; 
 *          one structure and 4 flags given in ilib.h; and a lot of 
 *          experimentation.  
 */
#include <stdio.h>
#include <stdlib.h>


#ifndef LITTLE_ENDIAN
#define TAG(a,b,c,d) (unsigned int)( (((unsigned int)(a))<<24) \
                                   | (((unsigned int)(b))<<16) \
                                   | (((unsigned int)(c))<<8)  \
                                   | (((unsigned int)(d))) )
#else
#define TAG(a,b,c,d) (unsigned int)( (((unsigned int)(d))<<24) \
                                   | (((unsigned int)(c))<<16) \
                                   | (((unsigned int)(b))<<8)  \
                                   | (((unsigned int)(a))) )
#endif


enum _IFF_GROUPS {
   IFFGROUP_____, /* Filler Group, four blank spaces */
   /* 2 byte alignment */
   IFFGROUP_FORM,
   IFFGROUP_CAT_,
   IFFGROUP_LIST,
   IFFGROUP_PROP,
   /* 4 byte alignment */
   IFFGROUP_FOR4,
   IFFGROUP_CAT4,
   IFFGROUP_LIS4,
   IFFGROUP_PRO4,
   /* 8 byte alignment */
   IFFGROUP_FOR8,
   IFFGROUP_CAT8,
   IFFGROUP_LIS8,
   IFFGROUP_PRO8,
   IFFGROUP_UNKNOWN
};
unsigned int gIFFGroups[] = {
   TAG(' ',' ',' ',' '),/* Filler Group, four blank spaces */

   TAG('F','O','R','M'),
   TAG('C','A','T',' '),
   TAG('L','I','S','T'),
   TAG('P','R','O','P'),

   TAG('F','O','R','4'),
   TAG('C','A','T','4'),
   TAG('L','I','S','4'),
   TAG('P','R','O','4'),

   TAG('F','O','R','8'),
   TAG('C','A','T','8'),
   TAG('L','I','S','8'),
   TAG('P','R','O','8')
};

enum _IFF_GROUPTYPES {
   IFFGROUPTYPE_ANIM,
   IFFGROUPTYPE_CIMG,
   IFFGROUPTYPE_CLIP,
   IFFGROUPTYPE_DRAW,
   IFFGROUPTYPE_FONT,
   IFFGROUPTYPE_IMAG,
   IFFGROUPTYPE_PICT,
   IFFGROUPTYPE_TBMP,
   IFFGROUPTYPE_TEXT,
   IFFGROUPTYPE_CONS,
   IFFGROUPTYPE_AIFF,
   IFFGROUPTYPE_UNKNOWN
};
unsigned int gIFFGroupTypes[] = {
   TAG('A','N','I','M'),
   TAG('C','I','M','G'),
   TAG('C','L','I','P'),
   TAG('D','R','A','W'),
   TAG('F','O','N','T'),
   TAG('I','M','A','G'),
   TAG('P','I','C','T'),
   TAG('T','B','M','P'),
   TAG('T','E','X','T'),
   TAG('C','O','N','S'),
   TAG('A','I','F','F')
};

enum _IFF_DATATYPES /* Chunks */ {
   IFFDATABLOCK_CHAR,
   IFFDATABLOCK_LONG,
   IFFDATABLOCK_IHDR,
   IFFDATABLOCK_BODY,
   IFFDATABLOCK_LINE,
   IFFDATABLOCK_BMHD,
   IFFDATABLOCK_RGBA,
   IFFDATABLOCK_RGB2,
   IFFDATABLOCK_ZBUF,

   /* History Example: "/usr/aw/maya1.0/bin/maya.bin" */
   IFFDATABLOCK_HIST,

   /* Version Example: "Maya cut 9801231655" */
   IFFDATABLOCK_VERS,

   IFFDATABLOCK_CLPZ,
   IFFDATABLOCK_ESXY,
   IFFDATABLOCK_PATH,
   IFFDATABLOCK_INCL,
   IFFDATABLOCK_EOVC,
   IFFDATABLOCK_BLCK,
   IFFDATABLOCK_GEND,

   /* Version Number Example with size of 4 bytes:  0x00010001  
    *   Maybe the example is showing a two short integers: a two byte major 
    *   int and two byte minor int (?).
    */
   IFFDATABLOCK_FVER,

   IFFDATABLOCK_TBHD,

   /* Author Example: "<acountname> on <machinename> IRIX 6.3 IP32\0\0" 
    *   The double NULLs at the end are for padding.  The example is from
    *   a FOR4 group which is aligned to 4 bytes.  Note that the size
    *   of items in the FOR4 group do not take the padding into account, but
    *   the size of the FOR4 does include the space to store the padding.
    */
   IFFDATABLOCK_AUTH, 

   /* Date Example: "Sat May  2 11:18:54 1998" */
   IFFDATABLOCK_DATE, 

   IFFDATABLOCK_STR_, /* Maya scene data. */ 
   IFFDATABLOCK_MADE, /* Maya scene data. */ 
   IFFDATABLOCK_CHNG, /* Maya scene data. */ 
   IFFDATABLOCK_OBJN, /* Maya scene data. */ 
   IFFDATABLOCK_UVER, /* Maya scene data. */ 
   IFFDATABLOCK_SLCT, /* Maya scene data. */ 

   IFFDATABLOCK_COMM, /* SGI AIFF data. */
   IFFDATABLOCK_SSND, /* SGI AIFF data. */
   IFFDATABLOCK_MARK, /* SGI AIFF data. */
   IFFDATABLOCK_INST, /* SGI AIFF data. */

   IFFDATABLOCK_UNKNOWN
};
unsigned int gIFFDataTypes[] = {
   TAG('C','H','A','R'),
   TAG('L','O','N','G'),
   TAG('I','H','D','R'),
   TAG('B','O','D','Y'),
   TAG('L','I','N','E'),
   TAG('B','M','H','D'),
   TAG('R','G','B','A'),
   TAG('R','G','B','2'),
   TAG('Z','B','U','F'),
   TAG('H','I','S','T'),
   TAG('V','E','R','S'),
   TAG('C','L','P','Z'),
   TAG('E','S','X','Y'),
   TAG('P','A','T','H'),
   TAG('I','N','C','L'),
   TAG('E','O','V','C'),
   TAG('B','L','C','K'),
   TAG('G','E','N','D'),
   TAG('F','V','E','R'),
   TAG('T','B','H','D'),
   TAG('A','U','T','H'),
   TAG('D','A','T','E'),
   TAG('S','T','R',' '), /*FOR4 MAYA*/
   TAG('M','A','D','E'), /*FOR4 MAYA*/
   TAG('C','H','N','G'), /*FOR4 MAYA*/
   TAG('O','B','J','N'), /*FOR4 MAYA*/
   TAG('U','V','E','R'), /*FOR4 MAYA*/
   TAG('S','L','C','T'), /*FOR4 MAYA*/
   TAG('C','O','M','M'),
   TAG('S','S','N','D'),
   TAG('M','A','R','K'),
   TAG('I','N','S','T')
};


typedef struct _GROUP {
   int            alignment;
   int            level;
   unsigned int   size;
   unsigned int   bytesleft;
   unsigned int   type;
   struct _GROUP  *next;
} GROUP;
typedef GROUP  *PGROUP;

static char *readerr = "Error reading %s\n";

void countbytes( PGROUP groups, unsigned int nbytes );
int groupid( char *tag, int *alignment );
int grouptype( char *tag );
int datatype( char *tag );
unsigned int alignsize( unsigned int size, int alignment );
PGROUP newgroup( PGROUP groups, int alignment );
PGROUP checkgroups( PGROUP groups );
void freegroups( PGROUP groups );
int readtag( FILE *iff, char *tag[4] );
void indent( PGROUP groups );
int handlechunk( FILE *iff, PGROUP groups, char tag[4], int id );
int handlegroup( FILE *iff, PGROUP groups, char tag[4], int id );
int iffinfo( char *filename );
int main(int argc, char **argv);


void countbytes( PGROUP groups, unsigned int nbytes ) 
{
   if (groups)
   {
      if (groups->bytesleft<nbytes)
      {
	 printf( "ERROR:  Size of nested block too large.\n" );
	 printf( "        Tried to subtract %u from nested block with "\
		 "%u bytes left.\n", nbytes, groups->bytesleft );
      }
      else
      {
	 groups->bytesleft-=nbytes;
      }
   }
}


int groupid( char *tag, int *alignment )
{
   unsigned int  *p;
   int           id;


   p = (unsigned int*)&tag[0];
   switch ( *p ) {
    case TAG(' ',' ',' ',' '):
      id = IFFGROUP_____;
      *alignment = 2;
      break;
    case TAG('F','O','R','M'):
      id = IFFGROUP_FORM;
      *alignment = 2;
      break;
    case TAG('C','A','T',' '):
      id = IFFGROUP_CAT_;
      *alignment = 2;
      break;
    case TAG('L','I','S','T'):
      id = IFFGROUP_LIST;
      *alignment = 2;
      break;
    case TAG('P','R','O','P'):
      id = IFFGROUP_PROP;
      *alignment = 2;
      break;
    case TAG('F','O','R','4'):
      id = IFFGROUP_FOR4;
      *alignment = 4;
      break;
    case TAG('C','A','T','4'):
      id = IFFGROUP_CAT4;
      *alignment = 4;
      break;
    case TAG('L','I','S','4'):
      id = IFFGROUP_LIS4;
      *alignment = 4;
      break;
    case TAG('P','R','O','4'):
      id = IFFGROUP_PRO4;
      *alignment = 4;
      break;
    case TAG('F','O','R','8'):
      id = IFFGROUP_FOR8;
      *alignment = 8;
      break;
    case TAG('C','A','T','8'):
      id = IFFGROUP_CAT8;
      *alignment = 8;
      break;
    case TAG('L','I','S','8'):
      id = IFFGROUP_LIS8;
      *alignment = 8;
      break;
    case TAG('P','R','O','8'):
      id = IFFGROUP_PRO8;
      *alignment = 8;
      break;
    default:
      id = IFFGROUP_UNKNOWN;
   }

   return id;
}


int grouptype( char *tag )
{
   unsigned int  *p;
   int  i,id;


   p = (unsigned int*)&tag[0];
   id = IFFGROUPTYPE_UNKNOWN;
   for ( i=0; i<IFFGROUPTYPE_UNKNOWN; i++ )
   {
      if ( *p==gIFFGroupTypes[i])
      {
	 id = i;
	 break;
      }
   }

   return id;
}


int datatype( char *tag )
{
   unsigned int  *p;
   int  i,id;


   p = (unsigned int*)&tag[0];
   id = IFFDATABLOCK_UNKNOWN;
   for ( i=0; i<IFFDATABLOCK_UNKNOWN; i++ )
   {
      if ( *p==gIFFDataTypes[i])
      {
	 id = i;
	 break;
      }
   }

   return id;
}


/* A sub group can only go up in alignment value from 2 to 4 to 8. 
 *   Atleast that seems to be what the Maya docs are trying to
 *   show in the example and brief description.  This implies that
 *   an elaborate scheme of using a stack to save nested alignments
 *   is not needed.  Only the level of nesting for each increase
 *   in the alignment size is needed.
 *
 * The Maya documentation states that the size does not take the
 *   padding into account.  This would imply that extra bytes added
 *   to the end of the group or chunk that the size applies to were
 *   not included in the size's value.
 *
 * Align the size by adding in any needed padding.  For example a
 *   group ID of FOR8 would mean the size not divisble by 8 would
 *   need to be padded by the number of bytes needed to make the
 *   size divisible by 8.
 *
 * Dumping values from an IFF file showed that an author string with
 *   tag AUTH in a FOR4 (alignement of 4 bytes) had two added bytes
 *   for padding shown as two astericks following "IRIX 6.3 IP32":
 *
 *     A    U    T    H   \0   \0   \0  032   # AUTH****
 *     t    e    b  040    o    n  040    n   # teb on n
 *     o    m    e    ,  040    I    R    I   # ome, IRI
 *     X  040    6    .    3  040    I    P   # X 6.3 IP
 *     3    2   \0   \0    D    A    T    E   # 32**DATE
 *    \0   \0   \0  030    S    a    t  040   # ****Sat 
 *     M    a    y  040  040    2  040    1   # May  2 1
 *     1    :    1    8    :    5    4  040   # 1:18:54 
 *     1    9    9    8    F    O    R    4   # 1998FOR4
 *
 * Since each sub group/chunk adds padding, the size given in the FOR4
 *   group has to take the padding into account (contrary to the Alias Maya
 *   documentation) because the padding can add a significant and unknown
 *   number of bytes to a group's size.
 */
unsigned int alignsize( unsigned int size, int alignment )
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


PGROUP newgroup( PGROUP groups, int alignment )
{
   PGROUP  p;


   p = (PGROUP)malloc( sizeof(GROUP) );
   if ( !p )
   {
      printf( "ERROR:  Can't allocate GROUP structure.\n" );
      exit(1);
   }

   p->alignment = alignment;
   p->size = 0;
   p->type = 0;
   p->bytesleft = 0;
   p->level = (groups ? groups->level+1 : 1);
   p->next = groups;
   
   return p;
}


PGROUP checkgroups( PGROUP groups )
{
   auto PGROUP  p = groups;
   auto PGROUP  pp;


   while (p && p->bytesleft==0)
   {
      pp = p;
      p = p->next;
      if (p)
      {
	 /* The 4 is added to account for the four bytes used for the
	  *    child groups group tag such as FOR4. 
	  */
	 countbytes( p, pp->size + 4 );
      }
      free(pp);
   }

   return p;
}


void freegroups( PGROUP groups )
{
   PGROUP  p,pp;

   p = groups;
   while (p)
   {
      pp = p;
      p = p->next;
      free(pp);
   }
}

void indent( PGROUP groups )
{
   int  i;

   if ( groups && groups->level )
   {
      for (i=0; i<groups->level; i++)
      {
	 putchar( '\t' );
      }
   }
   else
   {
      printf( "  " );
   }
}

int handlechunk( FILE *iff, PGROUP groups, char tag[4], int id )
{
   char          b[32]; /* Set to size of TBHD chunk. */
   unsigned int  size,alignedsize;
   int           alignment;
   int           type;
   auto int      i;


   (void)id;

   /* Read the size of data block that follows. */
   if ( fread( b, 1, 4, iff ) < 4 )
      return 1;

   /* Move the size into an integer value. */
   size = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3];

   alignment = (groups ? groups->alignment : 2);

   alignedsize = alignsize( size, alignment );

   /* Count the four bytes read for the size. */
   countbytes( groups, 4 );

   indent(groups);

   type = datatype( tag );
   
   printf( "%c%c%c%c ", tag[0], tag[1], tag[2], tag[3] );

      /* Check for IDs BLCK, EOVC and GEND which have used the following
       *    special size values:
       *
       *        szUnknown   0x80000000
       *        szFile      0x80000001
       *        szFifo      0x80000002
       *
       */
   if ( size & 0x80000000 /* Do the AND as a quick test. */
	&& (id==IFFDATABLOCK_BLCK 
	    || id==IFFDATABLOCK_EOVC
	    || id==IFFDATABLOCK_GEND) 
	&& (size==0x80000000
	    ||size==0x80000001
	    ||size==0x80000002) )
   {
      /* Composer writes image files with GEND tags that have a size of
       *    0x80000000 ([ALIA98f] refers to this size as szUnknown) with 
       *    no data that follows it.  This rather large size that causes 
       *    problems with the whole notion of the IFF file structures.  
       * Basically there are special sizes that indicate that a structure
       *    was of unknown size as the file was being written out.  These
       *    values are used when something like fseek() can not be used
       *    to patch a file, but instead the IFF needs to be quickly parsed
       *    in a second pass and negative sizes replaced.  
       * According to [ALIA98a], the GEND tag is supposed to be a zero 
       *    sized structure to indicate the end of a structure of unknown 
       *    size during the writing of an IFF file.  
       *    According to comments in flib.h [ALIA98f], a value of szUnknown 
       *    (0x80000000) is used to be compatible with older IFF parsers.
       * There is a set of special values assigned to a TAG's size.  There
       *    specific history and use is not documented, but the tags BLCK, 
       *    EOVC and GEND appear to make use of the following size values:
       *
       *        szUnknown   0x80000000
       *        szFile      0x80000001
       *        szFifo0x    0x80000002
       *
       * The above are hinted as representing negative values which would 
       *    just be meaningless sizes (refer to [ALIA98a] Extensions 
       *    section). 
       *    According to [MURR96] page 470 size is a DWORD and on page 112 
       *    a DWORD is an unsigned 32 bit integer.  
       *
       * If an EOVC is encountered, then the parser should search for a
       *    GEND tag with a size of 0x80000000.  It appears that it is
       *    hoped that following binary data does not accidentally have
       *    8 bytes that match the special GEND sequence :
       *       47  45  4e  44 80  00  00  00
       * Inshort EOVC of unknown size are not bullet proof.  Luckily, Maya
       *    image files (except for Composer's odd use) and Maya binary 
       *    scene files do not appear to use the 0x8... sizes.
       *
       * The following is a hex dump of the end of a ComposerLite Maya IFF
       *    image file:
       *
       *       ff  00  ff  00  ff  00  ff  00  
       *       ff  00  ff  00  ff  00  ff  00  
       *       ff  00  ff  00  47  45  4e  44  
       *       80  00  00  00  47  45  4e  44  
       *       80  00  00  00 
       *
       * The "ff 00" are RLE compressed pixels.  The "47  45  4e  44" 
       *    is a hexidecimal print out of 'GEND'.  The "80  00  00  00"
       *    is the size field following the GEND tage and because the
       *    upper most bit is set, this is supposed to be a negative 
       *    number.
       */
      char  *meaning;

      switch (size) {
       case 0x80000000:
	 meaning = "szUnknown size marker";
	 break;
       case 0x80000001:
	 meaning = "szFile";
	 break;
       case 0x80000002:
	 meaning = "szFifo0x";
	 break;
      }
      printf( "0x%x (%s)", size, meaning );
   }
   else
   {
      printf( "%u ", size );
   }
   
   switch (type) {
    case IFFDATABLOCK_CHAR:
    case IFFDATABLOCK_STR_:
      i = size;
      putchar( '\"' );
      for (i=0; i<size; i++)
      {
	 if ( fread( b, 1, 1, iff ) < 1 )
	    return 1;
	 if (b[0])
	 {
	    putchar( b[0] );
	 }
	 else
	 {
	    printf("\\0");
	 }
      }
      putchar( '\"' );
      fseek( iff, (long)(alignedsize-size), SEEK_CUR );
      break;
    case IFFDATABLOCK_HIST:
    case IFFDATABLOCK_VERS:
    case IFFDATABLOCK_AUTH:
    case IFFDATABLOCK_DATE:
    case IFFDATABLOCK_MADE:
    case IFFDATABLOCK_CHNG:
    case IFFDATABLOCK_OBJN:
    case IFFDATABLOCK_UVER:
    case IFFDATABLOCK_SLCT:
      i = size;
      putchar( '\"' );
      for (i=0; i<size; i++)
      {
	 if ( fread( b, 1, 1, iff ) < 1 )
	    return 1;
	 putchar( b[0] );
      }
      putchar( '\"' );
      fseek( iff, (long)(alignedsize-size), SEEK_CUR );
      break;
    case IFFDATABLOCK_FVER:
      /* Version Number Example with size of 4 bytes:  0x00010001  
       *   Maybe the example is showing a two short integers: a two byte major 
       *   int and two byte minor int (?).
       */
      if ( fread( b, 1, 4, iff ) < 4 )
      {
	 return 1;    
      }
      /* Block for variable major and minor declared below. */
      {
	 int  major, minor;

	 major = b[0]<<8 | b[1];
	 minor = b[2]<<8 | b[3];

	 printf( " %d.%d", major, minor );
      }
      break;
    case IFFDATABLOCK_SSND:
      /* Refer to [METR98]. */
      if ( size<8 || groups->type!=TAG('A','I','F','F') )
	 goto SkipData;      
      if ( fread( b, 1, 8, iff ) < 8 )
	 return 1;
      {
	 unsigned int  offset;
	 unsigned int  blocksize;


	 offset = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3];
	 blocksize = b[4]<<24 | b[5]<<16 | b[6]<<8 | b[7];

	 printf("(sound data)\n");
	 indent( groups );
	 printf( "        offset:%u\n", offset );
	 indent( groups );
	 printf( "        block size:%u", blocksize );
      }      
      fseek( iff, (long)alignedsize-8, SEEK_CUR );
      break;
    case IFFDATABLOCK_COMM:
      /* Refer to [METR98]. */
      if ( size!=18 || groups->type!=TAG('A','I','F','F') )
	 goto SkipData;      
      if ( fread( b, 1, 18, iff ) < 18 )
	 return 1;
      {
	 unsigned int  nchannels;
	 unsigned int  nsampleframes;
	 unsigned int  samplebitwidth;

	 nchannels = b[0]<<8 | b[1];
	 nsampleframes = b[2]<<24 | b[3]<<16 | b[4]<<8 | b[5];
	 samplebitwidth = b[6]<<8 | b[7];

	 /* Example COMM structure:
	   00  02  -- two channels
	   00  00  98  16  -- 0x9816 = 38964 sample frames for this example
	   00  10  -- sample bit width of 16
	   40  0e  ac  44  
	   00  00  00  00  00  00
	   */
	 printf( "number of channels:%u\n", nchannels );
	 indent( groups );
	 printf( "        number of sample frames:%u\n", nsampleframes );
	 indent( groups );
	 printf( "        sample bit width:%u", samplebitwidth );
      }      
      break;
    case IFFDATABLOCK_CLPZ:
      if ( size!=8 )
	 goto SkipData;
      if ( fread( b, 1, 8, iff ) < 8 )
	 return 1;
      {
	 unsigned int  i1, i2;
	 float  *f1 = (float*)&i1;
	 float  *f2 = (float*)&i2;

	 i1 = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3];
	 i2 = b[4]<<24 | b[5]<<16 | b[6]<<8 | b[7];

	 printf( "near:%g far:%g", *f1, *f2 );
      }
      break;
    case IFFDATABLOCK_ESXY:
      if ( size!=8 )
	 goto SkipData;
      if ( fread( b, 1, 8, iff ) < 8 )
	 return 1;
      {
	 unsigned int  i1, i2;
	 float  *f1 = (float*)&i1;
	 float  *f2 = (float*)&i2;

	 i1 = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3];
	 i2 = b[4]<<24 | b[5]<<16 | b[6]<<8 | b[7];

	 printf( "%g %g", *f1, *f2 );
      }
      break;
    case IFFDATABLOCK_RGB2: 
    case IFFDATABLOCK_RGBA:
    case IFFDATABLOCK_ZBUF:
      /* The header file flib.h says that RGB2 is obselete. 
       *    Refer to [ALIA98f].  Composer 4.5 writes RGB2 tags out,
       *    sets the bytes field of TBHD to 0, and uses RGB2
       *    to store the second of byte of each 16 bit channel.
       */
      if ( fread( b, 1, 8, iff ) < 8 )
	 return 1;
      /* Block for variable x1, y1, x2, and y2 declared below. */
      {
	 int  x1, y1, x2, y2;

	 x1 = b[0]<<8 | b[1];
	 y1 = b[2]<<8 | b[3];
	 x2 = b[4]<<8 | b[5];
	 y2 = b[6]<<8 | b[7];

	 printf( "[x1:%4d y1:%4d x2:%4d y2:%4d]", x1, y1, x2, y2 );
      }
      fseek( iff, (long)alignedsize-8, SEEK_CUR );
      break;
    case IFFDATABLOCK_TBHD:
      /* Composer 4.5 writes IFF files with a TBHD header structure of
       *    24 bytes, while Maya writes IFF files with 32 byte sized
       *    TBHD header structures.  Refer to [ALIA98e].
       */
      if ( size!=32 && size!=24 )
	 goto SkipData;

      /* The following is an excerpt from the output of the command
       *    "bin2hex composer.iff.0" showing the TBHD tag, size and data:
       *
       *       54  42  48  44  --  T B H D
       *       00  00  00  18  --  0x0018 == 24dec
       *       00  00  01  40  --  0x0140 == 320dec width
       *       00  00  00  f0  --  0x00f0 == 240dec height
       *       00  01  00  01  --  prnum/prdem pair (see comments below)
       *       00  00  00  03  --  flags (0x03==RGBA+Alpha)
       *       00  00          --  bytes 
       *       00  14          --  n tiles
       *       00  00  00  01  --  compression
       *
       * The only difference between the 24 and 32 byte structures is
       *    that the 32 byte version append xorg and yorg after compression.   
       */

      if ( fread( b, 1, size, iff ) < size )
	 return 1;
      /* Block for the variables declared below. */
      {
	 enum {
	    COMPRESS_NONE, 
	    COMPRESS_RLE,
	    COMPRESS_QRL,
	    COMPRESS_QR4,
	    COMPRESS_UNKNOWN
	 };
	 static char *compressiontypes[] = { 
	    "None", "rle", "qrl", "qr4", "unknown" };
	 unsigned int  width    = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3];
	 unsigned int  height   = b[4]<<24 | b[5]<<16 | b[6]<<8 | b[7];
	 /* Stretch or compress a width of prnum pixels into prdem pixels. 
	  *    Refer to [ALIA98d].
	  * prnum -- pixel aspect ratio numerator.
	  * prden -- pixel aspect ratio denominator. 
	  */
	 unsigned int  prnum    = b[8]<<8 | b[9];
	 unsigned int  prden    = b[10]<<8 | b[11];
	 unsigned int  flags    = b[12]<<24 | b[13]<<16 | b[14]<<8 | b[15];
	 /* Bytes is set to 1 for 16bit channels and 0 for 8bit.
	  *   Possible padding byte count (?).
	  */
	 unsigned int  bytes    = b[16]<<8 | b[17];
	 unsigned int  tiles    = b[18]<<8 | b[19];
	 unsigned int  compress = b[20]<<24 | b[21]<<16 | b[22]<<8 | b[23];
	 /* The meaning of the origin values is left for the user to 
	  *    decide.  Refer to [ALIA98d].
	  */
	 int           orgx     = b[24]<<24 | b[25]<<16 | b[26]<<8 | b[27];
	 int           orgy     = b[28]<<24 | b[29]<<16 | b[30]<<8 | b[31];
	 char          *p;

	 printf( "width:%u height:%u\n", width, height );
	 indent( groups );
	 printf( "        prnum:%u prden:%u\n", prnum, prden );
	 indent( groups );
	 printf( "        flags:0x%x (", flags );
	 /* Pixel data types. */
         if ( flags & 0x00000001 ) printf( " RGB" );
         if ( flags & 0x00000002 ) printf( " Alpha" );
         if ( flags & 0x00000004 ) printf( " Zbuffer" );
         if ( flags & 0x00000008 ) printf( " Abuffer" );
         if ( flags & 0x00000010 ) printf( " Grayscale" );
	 /* Note [ALIAS98d] says that the Grayscale flag (also referred to
	  *    as the Black flag) turns off the RGB flag when set using
	  *    the Alias libraries.
	  */
	 /* Texmap types. */
         if ( flags & 0x00020000 ) printf( " LBuffer" );
         if ( flags & 0x00100000 ) printf( " RBump" );
         if ( flags & 0x00200000 ) printf( " GBump" );
         if ( flags & 0x00400000 ) printf( " BBump" );
         if ( flags & 0x00800000 ) printf( " ABump" );
	 /* Note that there is a type mask of 0x0000001f for pixel type
	  *    and 0x00f20000 for texmap type. 
	  */
         if ( flags & ~0x00f2001f ) printf( " UnknownFlag(s)" );
	 printf( " )\n" );
	 indent( groups );
	 printf( "        bytes:%u\n", bytes );
	 indent( groups );
	 p = (compress>(sizeof(compressiontypes)/sizeof(char*)) 
	      ? compressiontypes[COMPRESS_UNKNOWN] 
	      : compressiontypes[compress] );
	 printf( "        compression:0x%x ( %s )\n", compress, p );
	 indent( groups );
	 printf( "        tiles:%u\n", tiles );
	 indent( groups );
	 if ( size==32)
	 {
	    /* When size is 24, the origin fields do not exist. */
	    printf( "        orgx:%d orgy:%d", orgx, orgy );
	 }
	 else
	 {
	    printf( "        orgx/orgy values not given in 24 byte TBHD" );
	 }
      }
      break;
    case IFFDATABLOCK_BMHD:
      /* Amiga IFF format uses BMHD tags.  The following shows an Amiga
       *    IFF written by PhotoShop 3.0:
       *       File: 36x36.amiga_iff
       *         FORM 1842 ILBM
       *               BMHD 20 width:36 height:36
       *                       left:0 top:0
       *                       bitplanes:24 masking:0
       *                       compression:1
       *                       padding:0 transparency:0
       *                       xaspectratio:1 yaspectratio:1
       *                       pagewidth:36 pageheight:36
       *               BODY 1802 Image data...
       */
      /* Note [MURR96] page 475 sizes of the BMHD structure are incorrect. 
       *   It gives sizes of 36 bytes for BMHD with ChunkID and ChunkSize
       *   when it should be 28.  The BMHD structure is given a size
       *   of 28 when it should be 20.
       */
      if ( size!=20 )
	 goto SkipData;

      if ( fread( b, 1, size, iff ) < size )
	 return 1;
      /* Block for the variables declared below. */
      {
	 /* Bigendian is used for 2 bytes words. */
	 unsigned int  width        = b[0]<<8 | b[1];
	 unsigned int  height       = b[2]<<8 | b[3];
	 unsigned int  left         = b[4]<<8 | b[5];
	 unsigned int  top          = b[6]<<8 | b[7];
	 unsigned int  bitplanes    = b[8];
	 unsigned int  masking      = b[9];
	 unsigned int  compression  = b[10];
	 unsigned int  padding      = b[11];
	 unsigned int  transparency = b[12]<<8 | b[13];
	 unsigned int  xaspectratio = b[14];
	 unsigned int  yaspectratio = b[15];
	 unsigned int  pagewidth    = b[16]<<8 | b[17];
	 unsigned int  pageheight   = b[18]<<8 | b[19];

	 printf( "width:%u height:%u\n", width, height );
	 indent( groups );
	 printf( "        left:%u top:%u\n", left, top );
	 indent( groups );
	 printf( "        bitplanes:%u masking:%u\n",bitplanes,masking);
	 indent( groups );
	 printf( "        compression:%u\n",compression);
	 indent( groups );
	 printf( "        padding:%u transparency:%u\n", 
		 padding, transparency );
	 indent( groups );
	 printf( "        xaspectratio:%u yaspectratio:%u\n",
		 xaspectratio, yaspectratio );
	 indent( groups );
	 printf( "        pagewidth:%u pageheight:%u", pagewidth, pageheight );
      }
      break;
    case IFFDATABLOCK_EOVC:
      /* See comments above about the BLCK, EOVC and GEND tags. */
      if ( size==0x80000000 ) 
      {
	 /* I don't have a real fifo IFF file with an EOVC tag in it,
	  *    so just print a warning and let the parser struggle with 
	  *    the unknown size. 
	  */
	 /* To-DO: Try to find a sequence 'EOVC' <size>. */
	 printf( "Warning: EOVC tag found with special unknown size marker." );
      }
      /* Fall through to BLCK and GEND case code. */
    case IFFDATABLOCK_BLCK: 
      /* Don't have any documentation on BLCK tags other than they too 
       *    can have the special negative size markers. 
       */
    case IFFDATABLOCK_GEND:
      /* Fix problem size with 0x8000000X sizes. */
      size = 0;
      /* Don't call alignsize() to align new size.  Just use zero since it
       *    aligns to zero reasonably well regardless of the alignment being
       *    used. 
       */
      alignedsize = 0; 
      break;
    case IFFDATABLOCK_BODY:
      printf( "Image data..." );
      /* Fall through to default. */
    default:
   SkipData:
      /* Skip over data. */
      fseek( iff, (long)alignedsize, SEEK_CUR );
   }
   putchar( '\n' );
   countbytes( groups, alignedsize );

   return 0;
}


int handlegroup( FILE *iff, PGROUP groups, char tag[4], int id )
{
   char          b[4];
   unsigned int  size;
   unsigned int  alignedsize;
   int           alignment;
   auto int      i;


   (void)id;

   if (!groups)
      return 1; /* Error. */

   /* Read the size of data block that follows. */
   if ( fread( b, 1, 4, iff ) < 4 )
      return 1;

   /* Move the size into an integer value. */
   size = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3];

   alignment = groups->alignment;

   alignedsize = alignsize( size, alignment );

   groups->size = alignedsize;
   groups->bytesleft = alignedsize;

   if ( groups->level-1 )
   {
      for (i=0; i<groups->level-1; i++)
      {
	 putchar( '\t' );
      }
   }
   else
   {
      printf( "  " );
   }

   printf( "%c%c%c%c %u ", tag[0], tag[1], tag[2], tag[3], size );

   /* Read the group type. */
   if ( fread( b, 1, 4, iff ) < 4 )
   {
      printf( "ERROR:  Expected to read a group type.\n" );
      return 1;
   }

   /* Store the group type. */
   groups->type = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3];

   /* Count the four bytes used to ID the group type. */
   countbytes( groups, 4 );

   printf( "%c%c%c%c\n", b[0], b[1], b[2], b[3] );

   return 0;
}


int iffinfo( char *filename )
{
   FILE      *iff;
   PGROUP    groups = NULL;
   char      tag[4];
   int       alignment;
   int       id;
   auto int  i;


   iff = fopen( filename, "rb" );
   if (!iff) 
   {
      fprintf( stderr, "Can't read %s\n", filename );
      return 1;
   }

   printf( "File: %s\n", filename );

   groups = NULL;

   for (;;)
   {
      i = fread( tag, 1, 4, iff );
      if ( i==0 )
	 break; /* Handle EOF. */
	  
      if ( i < 4 )
	 goto Error;

      /* Count the four bytes used to ID the group. */
      countbytes( groups, 4 );

      id = groupid( tag, &alignment );

      /* If unknown group, see if it is a data chunk. */
      if ( id==IFFGROUP_UNKNOWN )
      {
	 id = datatype( tag );
	 if (handlechunk( iff, groups, tag, id ))
	    goto Error;
      }
      else
      {
	 groups = newgroup( groups, alignment ); 
	 if (handlegroup( iff, groups, tag, id ))
	    goto Error;
      }

      groups = checkgroups( groups );
   }

   if (groups)
   {
      printf( "ERROR:  Improper nesting of groups.\n" );
      freegroups(groups);
      return 1;
   }

   return 0;

 Error:
   freegroups(groups);
   fprintf( stderr, readerr, filename );
   return 1;
}


int main(int argc, char **argv) 
{
   auto int  i;


   if ( argc < 2 || (argc > 1 && argv[1][0]=='-') )
   {
      /* User tried to specify an option, so just print help text. */
      printf( "iffinfo iff_filename . . .\n"                          \
      "   iff_filename . . .  List of Maya IFF files to read.\n" );
      return 1;
   }

   i = 1;
   if ( i < argc )
   {
      while ( i < argc )
        iffinfo( argv[i++] );
   }
   
   return 0;
}

