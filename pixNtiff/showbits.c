/* 
 * Copyright (c) 1996, 1997, 1998 Thomas E. Burge.  All rights reserved.  
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
 * FILE:  showbits.c
 *
 * DESCRIPTION:  Utility to display pix, tiff, tga and Maya iff files.
 *
 *    Contains:
 * 
 *    To-Do:     Fix background of window that is seen when a bitmap is
 *               less than the width of the minimum sized title bar.
 * 
 *    History:
 *        5-24-98  Added Maya IFF support.
 *
 *    References:
 *        
 *          [JOHN89]  Johnson, Eric F. and Reichhard, Kevin, X Windows
 *                    Applications Programming, MIS: Press, 1989.
 *          [NEID93]  Neider, Jackie; Davis, Tom; Mason, Woo; OpenGL 
 *                    Programming Guide, Addison Wesley, 1993.
 *          [SGI96]   Silicon Graphics, man pages for glXIntro(3G),
 *                    IRIX 6.3, 1996.
 *
 */
#include <GL/glx.h>
#include <GL/gl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.h"
#include "rpix.h"
#include "rtiff.h"
#include "rtga.h"
#include "affine.x"
#include "riff.h"


enum {
   UNKNOWN_FILETYPE,
   PIX_FILETYPE,
   TIFF_FILETYPE,
   TGA_FILETYPE,
   IFF_FILETYPE
};

void PrintHelp( void );
int DisplayBitmap( PBITMAP pBmp, char *filename );
int GetFileType( char *filename );
int DisplayBits( PBITMAP pBmp, int x, int y, int width, int height );
int main(int argc, char **argv);


/* globals */
int attributeListSgl[] = {
   GLX_RGBA,
   GLX_RED_SIZE,   1, 
   GLX_GREEN_SIZE, 1,
   GLX_BLUE_SIZE,  1,
   None 
};
int attributeListDbl[] = {
   GLX_RGBA,
   GLX_DOUBLEBUFFER, 
   GLX_RED_SIZE,   1,
   GLX_GREEN_SIZE, 1,
   GLX_BLUE_SIZE,  1,
   None 
};
Display  *dspy;
Window   wnd = (Window)NULL;
int      switchbuffer = 0;


int main(int argc, char **argv) 
{
   int      forced;
   int      filetype;
   int      i;
   PBITMAP  pBmp;
   pid_t    pid;


   if ( argc==1 )
   {
      PrintHelp();
      return 1;      
   }

   filetype = UNKNOWN_FILETYPE;
   forced = 0;
   i = 1;
   while ( i < argc )
   {
      if ( argv[i][0]=='-' )
      {
         if (!strcmp(&argv[i][1],"pix"))
         {
            filetype = PIX_FILETYPE;
            forced = 1;
         }
         else if (!strcmp(&argv[i][1],"tiff") || !strcmp(&argv[i][1],"tif"))
         {
            filetype = TIFF_FILETYPE;
            forced = 1;
         }
         else if (!strcmp(&argv[i][1],"tga"))
         {
            filetype = TGA_FILETYPE;
            forced = 1;
         }
         else if (!strcmp(&argv[i][1],"iff"))
         {
            filetype = IFF_FILETYPE;
            forced = 1;
         }
         else if (!strcmp(&argv[i][1],"auto"))
         {
            filetype = UNKNOWN_FILETYPE;
            forced = 0;
         }
         else
         {
            PrintHelp();
            return 1;
         }
      }
      else
      {
         if ( !filetype )
         {
            filetype = GetFileType( argv[i] );
         }
         if ( filetype )
         {
            switch ( filetype )
            {
             case PIX_FILETYPE:
               pBmp = ReadPix( argv[i] );
               break;
             case TIFF_FILETYPE:
               pBmp = ReadTiff( argv[i], READTIFF_CONVERT16TO8 );
               break;
             case TGA_FILETYPE:
               pBmp = ReadTga( argv[i] );
               break;
             case IFF_FILETYPE:
               pBmp = ReadIff( argv[i], READIFF_CONVERT16TO8 );
               break;
             default:
               exit(1);
            }
   
            pid = fork();
            if ( pid < 0 )
            {
               printf( "Error:  fork()\n" );
               return 1;
            }
            else if ( pid==0 )
            {
               DisplayBitmap( pBmp, argv[i] );
               exit(1);
            }
            if (!forced)
            {
               filetype = UNKNOWN_FILETYPE;
            }
         }
         else
         {
            printf( "ERROR:  Unknown file type: %s.\n", argv[i] );
            PrintHelp();
         }
      }     
      i++;
   }

   return 0;
}


int DisplayBitmap( PBITMAP pBmp, char *filename )
{
   int                    scrn;
   XVisualInfo            *visi = NULL;
   XSetWindowAttributes   xwndsetattr;
   XWindowAttributes      xwndattr;
   Colormap               clrmap;
   XSizeHints             szhints;
   XEvent                 event;
   GLXContext             glctx;
   Atom                   wm_delete_window;
   XWMHints               hints;
   Pixmap                 icon;
   char                   *s;


   if (!pBmp)
   {
      fprintf( stderr, "Can't read %s\n", filename );
      return 1;
   }
   if ( pBmp->sampleformat!=BITMAP_UINT8 && pBmp->sampleformat!=BITMAP_IEEE )
   {
      fprintf( stderr, 
              "Can't display %s: not 8 bits or IEEE 32 bits per sample.\n", 
              filename );
      return 1;
   }

   if ( pBmp->nsamples!=1 && pBmp->nsamples!=3 && pBmp->nsamples!=4 )
   {
      fprintf( stderr, 
              "Can't display %s: not an 8, 24, or 32 bit per pixel image.\n", 
              filename );
      return 1;
   }

   dspy = XOpenDisplay( 0 );

   if (!dspy)
   {
      fprintf( stderr, "Error connecting to X server\n" );
      exit( -1 );
   }

   scrn = DefaultScreen( dspy );

   visi = glXChooseVisual( dspy, scrn, attributeListSgl );
   if ( !visi )
   {
      visi = glXChooseVisual( dspy, scrn, attributeListDbl );
      switchbuffer = 1;
   }

   glctx = glXCreateContext( dspy, visi, 0, GL_TRUE );

   clrmap = XCreateColormap( dspy, RootWindow(dspy, visi->screen ), 
                            visi->visual, AllocNone );

   xwndsetattr.colormap = clrmap;
   xwndsetattr.border_pixel = 0;
   xwndsetattr.event_mask = StructureNotifyMask;

   wnd = XCreateWindow( dspy,
                       RootWindow( dspy, visi->screen ),
                       0, /* x */
                       0, /* y */
                       pBmp->xres, /* width */
                       pBmp->yres, /* height */
                       0, 
                       visi->depth,
                       InputOutput,
                       visi->visual,
                       CWBorderPixel | CWColormap | CWEventMask,
                       &xwndsetattr );

   XGetWindowAttributes( dspy, wnd, &xwndattr );

   /* OpenGL likes the bitmap data to have the bottom row first, not the
    *   top row.  Note the left most column is the first column.
    */
   pBmp = OrientBitmap( pBmp, BITMAP_BOTLEFT );

   /* Get a hook in on the user quitting the app.  This will prevent
    *   that annoying "X connection to 0:0 broken" message.
    */
   wm_delete_window = XInternAtom( dspy, "WM_DELETE_WINDOW", False );
   (void) XSetWMProtocols( dspy, wnd, &wm_delete_window, 1 );

   s = strrchr( filename, '/' );
   if (!s)
      s = filename;
   else
      s++;
   XStoreName( dspy, wnd, s );

   icon = XCreateBitmapFromData( dspy, wnd, 
                                (const char *)affine_bits, 
                                affine_width, affine_height );

   hints.icon_pixmap = icon;
   hints.flags = IconPixmapHint;
   XSetWMHints( dspy, wnd, &hints );

   szhints.flags = PMinSize | PMaxSize;
   szhints.min_width = pBmp->xres;
   szhints.max_width = pBmp->xres;
   szhints.min_height = pBmp->yres;
   szhints.max_height = pBmp->yres;
   XSetNormalHints( dspy, wnd, &szhints );
 
   XMapWindow( dspy, wnd );

   /* Connect the context to the window. */
   glXMakeCurrent(dspy, wnd, glctx);
 
   glShadeModel(GL_FLAT);
   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
   glViewport(0,0,pBmp->xres,pBmp->yres);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0,pBmp->xres,0.0,pBmp->yres,-1.0,1.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glClearColor(0,0,0,0); 
   glClear(GL_COLOR_BUFFER_BIT);

   XSelectInput( dspy, wnd, ExposureMask | StructureNotifyMask );
   for (;;)
   {
      XNextEvent( dspy, &event );
      switch (event.type)
      {
       case MapNotify:
         DisplayBits( pBmp, 0, 0, pBmp->xres, pBmp->yres );         
         break;
       case Expose:
         if ( event.xexpose.window == wnd )
           DisplayBits( pBmp, 
                       event.xexpose.x,
                       event.xexpose.y,
                       event.xexpose.width,
                       event.xexpose.height );
         break;
       case ClientMessage:
         if (event.xclient.data.l[0] == wm_delete_window
             && event.xclient.window == wnd ) 
           goto Exit;
         break;
      }
   }

 Exit:
   if (wnd)
     XDestroyWindow( dspy, wnd );
   if (visi)
     XFree(visi);
   if (pBmp)
     DestroyBitmap( pBmp );
   return 0;
}


int DisplayBits( PBITMAP pBmp, int x, int y, int width, int height )
{
   GLenum  type;
   int     gly;


   switch ( pBmp->nsamples )
   {
    case 4:
      type = GL_RGBA;
      break;
    case 3:
      type = GL_RGB;
      break;
    case 1:
      if (pBmp->sampleformat==BITMAP_IEEE)
	 type = GL_RGBA;
      else
	 type = GL_LUMINANCE;
      break;
    default:
      return 1;
   }
   gly = pBmp->yres - (y + height);

   glPixelStorei( GL_UNPACK_ROW_LENGTH,  pBmp->xres );
   glPixelStorei( GL_UNPACK_SKIP_ROWS,   gly );
   glPixelStorei( GL_UNPACK_SKIP_PIXELS, x );
   glRasterPos2i( x, gly );
   glDrawPixels( width, height, type, GL_UNSIGNED_BYTE, pBmp->pbits );
   glFlush();
   if (switchbuffer) 
   {
      glXSwapBuffers(dspy,wnd);
   }
   
   return 0;
}


void PrintHelp( void )
{
   printf( 
"showbits [[-pix|-tiff|-tga|-iff|-auto] filename ...]...\n"
"   [-pix|-tiff|     Force following filenames to be read as pix, tiff, tga\n"\
"    -tga-iff|-auto] or iff respectively.  The -auto option switches back\n" \
"                    to the default action of trying to determine each\n"    \
"                    filename's format automatically.\n"                     \
"   filename         TIFF, TGA, or Alias/WaveFront PIX/IFF file to display.\n"
);

   return;
}


int GetFileType( char *filename )
{
   static char *fileExtentions[] = {
      ".tiff",
      ".tif",
      ".TIF",
      ".pix", /* Note:  There really is no standard extension for pix files. */
      ".PIX",
      ".M", /* Alias Medium sized icons. */
      ".S", /* Alias Small sized icons. */
      ".als", /* Course #20 SIGGRAPH '96 */
      ".tga",
      ".TGA",
      ".iff",
      ".IFF",
      ".iffo", /* Maya odd field. */
      ".iffe"  /* Maya even field. */
   };
   static int fileExtTypes[] = { 
      TIFF_FILETYPE, 
      TIFF_FILETYPE, 
      TIFF_FILETYPE, 
      PIX_FILETYPE, 
      PIX_FILETYPE, 
      PIX_FILETYPE, 
      PIX_FILETYPE, 
      PIX_FILETYPE, 
      TGA_FILETYPE,
      TGA_FILETYPE,
      IFF_FILETYPE,
      IFF_FILETYPE,
      IFF_FILETYPE,
      IFF_FILETYPE
   };
   static int  fileExtLengths[] = { 5, 4, 4, 4, 4, 2, 2, 4, 4, 4, 4, 4, 5, 5 };
   static char unsigned  b[18];
   FILE  *fp;
   int   filetype;
   auto int   i,l,e,n,yres;


   filetype = UNKNOWN_FILETYPE;
   l = strlen(filename);
   for ( n=0; n<sizeof(fileExtentions)/sizeof(char*); n++ )
   {
      e = l-fileExtLengths[n];
      if ( (l-fileExtLengths[n] > 0)
          && (!strcmp(filename+e,fileExtentions[n])) )
      {
         filetype = fileExtTypes[n];
         break;
      }
   }
   if ( !filetype )
   {
      fp = fopen( filename, "rb" );
      if (!fp)
        return UNKNOWN_FILETYPE;

      if ( fread( b, 1, 4, fp ) == 4 )
      {
         /* Typical TIFF Header:
          * 'M','M',    -- bytes 0-1  MM - BigEndian, II - LittleEndian
          * 0x00, 0x2A, -- bytes 2-3  42=0x2A stored in BigEndian order
          */
         if ( b[2]==0 && b[3]==0x2A 
             && ((b[0]=='M'&&b[1]=='M')||(b[0]=='I'&&b[1]=='I')) )
         {
            /* Try a TIFF file. */            
            printf( "Reading %s as a TIFF file.\n", filename );
            filetype = TIFF_FILETYPE;
         }

	 /* Try a Maya IFF file type before trying PIX. */
	 if ( !filetype )
	 {
	    if ( b[0]=='F' && b[1]=='O' && b[2]=='R' && b[3]=='4' )
	    {
	       /* Try a Maya IFF file. */            
	       printf( "Reading %s as a Maya IFF file.\n", filename );
	       filetype = IFF_FILETYPE;
	    }	    
	 }

         /* Maybe it's a PIX file. */
         if ( !filetype )
         {
            yres = (b[2]<<8) | b[3];
            if ( fread( &b[4], 1, 6, fp )==6 )
            {
               /* The n will be yres-1, if file is written by Alias anyway.*/
               n = (b[6]<<8) | b[7];
               /* The value i will be bits per pel. */
               i = (b[8]<<8) | b[9];
               /* These two are set to zero by Alias software. */
               if ( b[4]==0 && b[5]==0 
                   && (yres-1 == n) 
                   && (i==8 || i==24 || i==32) )
               {
                  /* Try a PIX file. */            
                  printf( "Reading %s as a PIX file.\n", filename );
                  filetype = PIX_FILETYPE;
               }
               /* else it may not be a pix after all.
                *
                * This could still be a pix file, but was written
                *   so that the unused bytes were actually unused.
                */
               else /* Maybe try a TGA file. */
               {
                  /* Check if the image type looks good. */
                  i = b[2];
                  if ( i==10 || i==2 || i==0 || i==1 || i==3 || i==9 || i==11 )
                  {
                     /* Image type looks good.  Check pixel depth. */
                     if (fread( &b[10], 1, 8, fp )==8 )
                     {
                        i = b[16];
                        /* These three pixel depths are supported.  It could
                         *   be a valid TGA file, but an unsupported pixel
                         *   depth -- which means the file might as well be 
                         *   from Mars anyway.
                         */
                        if ( i==16 || i==24 || i==32 )
                        {
                           /* Pixel depth looks good.  Maybe, also try
                            *    checking origin_alphabits.
                            */
                           i = b[17];
                           /* Check if the origin is at the bottom left. 
                            *   Nobody uses the other orientations (Adobe
                            *   and AutoDesk don't write out TGA files 
                            *   in any other way).  
                            *
                            * Inshort, this still could be a good TGA file,
                            *    but using an obscure origin/orientation
                            *    for the data (which rtga.c would support
                            *    reading), but to be on the safe side we'll 
                            *    assume this isn't a TGA file if it does
                            *    not use a bottom-left origin.
			    * The user will have to use the -tga command
			    *    line option to display this file that has
			    *    an odd orientation for a tga and an odd
			    *    filename extension.
                            */
                           if ( (0x30 & i)==0 ) /* Check if bottom-left. */
                           {
                              /* Try reading it as a TGA file. */            
                              printf( "Reading %s as a TGA file.\n", 
                                     filename );
                              filetype = TGA_FILETYPE;
                           }
                        }
                     }
                  }
               }
               /* 
                * else give up now.
                *
                */
            }
         }
      }
      fclose(fp);
   }

   return filetype;
}
