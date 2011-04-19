/* 
 * Copyright (c) 1999 Thomas E. Burge.  All rights reserved.  
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
 *       Copyright (c) 1995, 1996, 1997, 1998, 1999 Thomas E. Burge.  
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
 * FILE:  cininfo.c
 *
 * DESCRIPTION:  Utility to print description of Kodak Cineon .cin files.
 *
 *               The Cineon format is referred to in [MURR96], a reference 
 *               listed below, as the Digital Picture Exchange (DPX) format.
 *               
 *               This code is tested with images from Alias|Wavefront 
 *               paint software and images from Pixars cineon display driver.
 *
 *               As a little programmer's note [MURR96] uses the type 
 *               definitions DWORD and SINGLE which on page 112 DWORD is
 *               defined as an unsigned 32 bit integer.  Although not 
 *               formally stated SINGLE turns out to be an IEEE single
 *               precision floating-point value.
 *             
 *               Also note that the [MURR96] is pretty much useless for
 *               the Cineon format.  It does not give the needed file
 *               header data strucures, instead it focuses on DPX which
 *               has a different generic file header structure.
 *
 *    Contains:
 * 
 *    References:
 *          [CINE99]  Version 4.5 , November 30, 1990 4.5 Draft - Image 
 *                    File Format Proposal for Digital Pictures, Kodak, 
 *                    November 30, 1990 draft from www.cineon.com.
 *          [MURR96]  Murray, James D. and VanRyper, William, Graphics
 *                    File Formats: Second Edition, O'Reilly & Associates, 
 *                    Inc., 1996, pp. 350-367.
 *
 *          Note:  [MURR96] does not provide much of any information 
 *                 regarding the Kodak Cineon standard.  I found the 
 *                 statements about DPX being a super set of the Kodak
 *                 Cineon format to be incorrect as the generic file
 *                 header is not the one used by the .cin files created
 *                 with Alias or PRMan.
 */
#include <stdio.h>
#include "cineon.h"

static char *NotNullTermString = 
   "Error with %s %s header field:\n   Expected a null terminated string.\n";
static char *ReservedAreaNotZeroed = 
   "Warning: Reservered %d byte buffer has data in it.\n";


int CheckString( char *filename, int l, char *s );
int cininfo( char *filename, int multiplefiles );
int main(int argc, char **argv);

int main(int argc, char **argv) 
{
   int multiplefiles;
   register int  i;

   if ( argc < 2 || (argc > 1 && argv[1][0]=='-') )
   {
      /* User tried to specify an option, so just print help text. */
      printf( "cininfo cin_filename . . .\n"                          \
      "   cin_filename . . .  List of Kodak Cineon files to read.\n" );
      return 1;
   }

   if ( 2 < argc )
      multiplefiles = TRUE;
   else
      multiplefiles = FALSE;

   i = 1;
   if ( i < argc )
   {
      while ( i < argc )
        cininfo( argv[i++], multiplefiles );
   }
   
   return 0;
}


int CheckString( char *filename, int l, char *s )
{
   register int   i=0;

   
   while (i<l)
      if (s[i++]=='\0')
	 return 0;
   
   fprintf( stderr, NotNullTermString, filename, "version" );
   return 1;
}


int cininfo( char *filename, int multiplefiles )
{
   static CINEON_FILEHEADER  header;
   static CINEON_IMAGEDESC   imgdesc;
   FILE               *cin;
   int                bigendian;
   char               *p;
   register int       i;


   cin = fopen( filename, "rb" );
   if (!cin) 
   {
      fprintf( stderr, "Can't read %s\n", filename );
      return 1;
   }

   /* Read file header. */
   if ( fread( (char*)&header, 1, sizeof(CINEON_FILEHEADER), cin ) 
	< sizeof(CINEON_FILEHEADER) )
     goto Error;

   /* [MURR96] states that header.magicnumber=='SDPX' for big-endian and
    *    header.magicnumber=='XPDS' for little-endian.  Unfortunately trying
    *    out Pixar's cineon driver and SGI's toimg tool, this magic number is
    *    actually fixed at {0x80,0x2a,0x5f,0xd7}.  
    * [CINE99] indicates that the magic number is reversed for little endian.
    */
   p = (char*)&header.magicnumber;
   if (p[0]!=0x80 || p[1]!=0x2a || p[2]!=0x5f || p[3]!=0xd7 )
   {
      if (p[0]!=0xd7 || p[1]!=0x5f || p[2]!=0x2a || p[3]!=0x80 )
      {
	 fprintf( stderr, "Error with %s Kodak Cineon magic number.\n", 
		  filename );
	 return 1;
      }
      else
      {
	 bigendian = FALSE;
      }
   }
   else
   {
      bigendian = TRUE;
   }

   /* Print file names if multiple files are being cininfo'd. 
    *   This is the behavior from such tools as libtiff's tiffinfo.
    */
   if (multiplefiles)
   {
      printf( "%s:\n", filename );
   }

   /* [MURR96] states that the strings in the DPX file header are 
    * free form -- hopefully this meant null terminated to everyone writting
    * Cineon file writers.
    *
    * It appears that folks zero out the structure before writting, so there
    * are a lot of NULLs.
    * 
    * To be safe check that the strings given in the Cineon file header
    * are actually null terminated strings before giving them to a stdlib
    * printf() function.
    */
   if (!CheckString( filename, 8, &header.version[0] ))
   {
      printf( "  Endedness: %s-endian\n  Version: %s\n", 
	      ( bigendian ? "big":"little"),
	      &header.version[0] );
   }
   printf("  File size: %u\n", header.filesize );
   printf("  Header data size: %u\n", header.genericlength );
   printf("  Industry data size: %u\n", header.industrylength );
   printf("  User data size: %u\n", header.variablelength );
   if (!CheckString( filename, 100, &header.filename[0] ))
   {
      printf( "  File Name:\"%s\"\n", &header.filename[0] );
   }

   /* For no reason, check that the reserved data area is zeroed out.
    * The SGI, Alias, Pixar Cineon writers all seem to do this.
    */
   for (i=0; i<sizeof(header.reserved); i++)
   {
      if (header.reserved[i]!='\0')
      {
	 printf( ReservedAreaNotZeroed, (int)sizeof(header.reserved) );
      }
   }

   /*  Note that SGI's toimg always writes the following as the date and time: 
    *      "1994:01:01\0\020:20:20PDT\0\0".
    */
   if (!CheckString( filename, 12, &header.creationdate[0] ))
   {
      printf( "  Creation Date:\"%s\"\n", &header.creationdate[0] );
   }
   if (!CheckString( filename, 12, &header.creationtime[0] ))
   {
      printf( "  Creation Time:\"%s\"\n", &header.creationtime[0] );
   }

   /* Read image description. */
   if ( fread( (char*)&imgdesc, 1, sizeof(CINEON_IMAGEDESC), cin ) 
	< sizeof(CINEON_IMAGEDESC) )
     goto Error;
   
   printf( "  Image Width: %u\n  Image Length: %u\n  Bits/Pixel: %u\n", 
	   imgdesc.width, imgdesc.height, 
	   (unsigned int)imgdesc.bitsperpixel );

   
   switch ( imgdesc.orientation ) {
    case CINEON_LEFTTOP: 
      p = "Left is row 0 / Top is column 0";
      break;
    case CINEON_LEFTBOT: 
      p = "Left is row 0 / Bottom is column 0";
      break;
    case CINEON_RIGHTTOP: 
      p = "Right is row 0 / Top is column 0";
      break;
    case CINEON_RIGHTBOT: 
      p = "Right is row 0 / Bottom is column 0";
      break;
    case CINEON_TOPLEFT:  
      p = "Top is row 0 / Left is column 0";
      break;
    case CINEON_TOPRIGHT: 
      p = "Top is row 0 / Right is column 0";
      break;
    case CINEON_BOTLEFT: 
      p = "Bottom is row 0 / Left is column 0";
      break;
    case CINEON_BOTRIGHT:  
      p = "Bottom is row 0 / Right is column 0";
      break;
    default:
      p = "Unkown orientation value.";
   }
   printf( "  Orientation: (%s)\n  Channels/Pixel: %u\n", 
	   p, (unsigned int)imgdesc.nchannels );

   if (!CheckString( filename, 200, &imgdesc.label[0] ))
   {
      printf( "  Image Label: \"%s\"\n", &imgdesc.label[0] );
   } 

   printf( "white[0]:%g white[1]:%g \n", imgdesc.white[0], imgdesc.white[1] );
   printf( "red[0]:%g red[1]:%g \n", imgdesc.red[0], imgdesc.red[1] );
   printf( "green[0]:%g green[1]:%g \n", imgdesc.green[0], imgdesc.green[1] );
   printf( "blue[0]:%g blue[1]:%g \n", imgdesc.blue[0], imgdesc.blue[1] );

   /* For no reason, check that the reserved data area is zeroed out.
    * The SGI, Alias, Pixar Cineon writers all seem to do this.
    */
   for (i=0; i<sizeof(imgdesc.reserved); i++)
   {
      if (imgdesc.reserved[i]!='\0')
      {
	 printf( ReservedAreaNotZeroed, (int)sizeof(imgdesc.reserved) );
      }
   }

   return 0;

 Error:
   fprintf( stderr, "Error reading %s\n", filename );
   return 1;
}
