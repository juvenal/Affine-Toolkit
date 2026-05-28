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
 *               Modernized using SDL2 for native cross-platform support.
 *
 */
#include <SDL.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "bitmap.h"
#include "rpix.h"
#include "rtiff.h"
#include "rtga.h"
#include "riff.h"


enum {
   UNKNOWN_FILETYPE,
   PIX_FILETYPE,
   TIFF_FILETYPE,
   TGA_FILETYPE,
   IFF_FILETYPE
};

void PrintVersion(const char* toolname)
{
   printf("%s version %s\n", toolname, AFFINE_VERSION);
   printf("%s", RAT_COPYRIGHT_STATEMENT);
   printf("%s", RENDERMAN_COPYRIGHT_STATEMENT);
}

void PrintHelp(const char* toolname)
{
   printf("%s\n", toolname);
   printf("%s", RAT_COPYRIGHT_STATEMENT);
   printf("%s", RENDERMAN_COPYRIGHT_STATEMENT);
   printf( 
"\nUsage: %s [[-pix|-tiff|-tga|-iff|-auto] filename ...]...\n"
"   [-pix|-tiff|     Force following filenames to be read as pix, tiff, tga\n"\
"    -tga-iff|-auto] or iff respectively.  The -auto option switches back\n" \
"                    to the default action of trying to determine each\n"    \
"                    filename's format automatically.\n"                     \
"   filename         TIFF, TGA, or Alias/WaveFront PIX/IFF file to display.\n", toolname
);
}

int DisplayBitmap( PBITMAP pBmp, char *filename );
int GetFileType( char *filename );
int main(int argc, char **argv);


int main(int argc, char **argv) 
{
   int      forced;
   int      filetype;
   int      i;
   PBITMAP  pBmp;
   const char *toolname = argv[0];


   if (argc > 1) {
      if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
         PrintVersion(toolname);
         return 0;
      }
      if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
         PrintHelp(toolname);
         return 0;
      }
   }

   if ( argc==1 )
   {
      PrintHelp(toolname);
      return 1;      
   }

   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
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
            PrintHelp(toolname);
            SDL_Quit();
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
               SDL_Quit();
               exit(1);
            }
   
            if (pBmp) {
                DisplayBitmap( pBmp, argv[i] );
                DestroyBitmap( pBmp );
            }
            if (!forced)
            {
               filetype = UNKNOWN_FILETYPE;
            }
         }
         else
         {
            printf( "ERROR:  Unknown file type: %s.\n", argv[i] );
            PrintHelp(toolname);
         }
      }     
      i++;
   }

   SDL_Quit();
   return 0;
}


int DisplayBitmap( PBITMAP pBmp, char *filename )
{
   SDL_Window* window = NULL;
   SDL_Renderer* renderer = NULL;
   SDL_Texture* texture = NULL;
   SDL_Event event;
   int quit = 0;
   char *title;
   char *s;
   Uint32 pixel_format;

   if (!pBmp)
   {
      fprintf( stderr, "Can't read %s\n", filename );
      return 1;
   }

   /* SDL prefers Top-Left orientation for easy blitting. */
   pBmp = OrientBitmap( pBmp, BITMAP_TOPLEFT );

   switch ( pBmp->nsamples )
   {
    case 4:
      pixel_format = SDL_PIXELFORMAT_ABGR8888;
      break;
    case 3:
      pixel_format = SDL_PIXELFORMAT_RGB24;
      break;
    case 1:
      pixel_format = SDL_PIXELFORMAT_INDEX8; /* For grayscale, might need mapping */
      /* Note: SDL_PIXELFORMAT_INDEX8 needs a palette. For simple display, let's convert to RGB if needed or use a gray palette. */
      break;
    default:
      fprintf( stderr, "Can't display %s: unsupported number of samples (%d).\n", filename, pBmp->nsamples );
      return 1;
   }

   s = strrchr( filename, '/' );
   if (!s)
      title = filename;
   else
      title = s + 1;

   window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                             pBmp->xres, pBmp->yres, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
   if (window == NULL) {
      fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
      return 1;
   }

   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
   if (renderer == NULL) {
      renderer = SDL_CreateRenderer(window, -1, 0);
   }

   texture = SDL_CreateTexture(renderer, pixel_format, SDL_TEXTUREACCESS_STATIC, pBmp->xres, pBmp->yres);

   if (pBmp->nsamples == 1) {
       /* Create a grayscale palette for 8-bit index mode */
       SDL_Color colors[256];
       for(int i=0; i<256; i++) {
           colors[i].r = colors[i].g = colors[i].b = i;
           colors[i].a = 255;
       }
       /* Note: Setting palette on texture is not direct in SDL2. 
          Usually we'd convert to RGB or use an SDL_Surface. 
          For simplicity, let's just handle RGB/RGBA for now. 
       */
   }

   SDL_UpdateTexture(texture, NULL, pBmp->pbits, pBmp->rowbytes);

   while (!quit) {
      while (SDL_PollEvent(&event) != 0) {
         if (event.type == SDL_QUIT) {
            quit = 1;
         } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
                quit = 1;
            }
         }
      }

      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      SDL_Delay(10);
   }

   SDL_DestroyTexture(texture);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);

   return 0;
}


int GetFileType( char *filename )
{
   static char *fileExtentions[] = {
      ".tiff",
      ".tif",
      ".TIF",
      ".pix", 
      ".PIX",
      ".M", 
      ".S", 
      ".als", 
      ".tga",
      ".TGA",
      ".iff",
      ".IFF",
      ".iffo", 
      ".iffe"  
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
         if ( b[2]==0 && b[3]==0x2A 
             && ((b[0]=='M'&&b[1]=='M')||(b[0]=='I'&&b[1]=='I')) )
         {
            filetype = TIFF_FILETYPE;
         }

	 if ( !filetype )
	 {
	    if ( b[0]=='F' && b[1]=='O' && b[2]=='R' && b[3]=='4' )
	    {
	       filetype = IFF_FILETYPE;
	    }	    
	 }

         if ( !filetype )
         {
            yres = (b[2]<<8) | b[3];
            if ( fread( &b[4], 1, 6, fp )==6 )
            {
               n = (b[6]<<8) | b[7];
               i = (b[8]<<8) | b[9];
               if ( b[4]==0 && b[5]==0 
                   && (yres-1 == n) 
                   && (i==8 || i==24 || i==32) )
               {
                  filetype = PIX_FILETYPE;
               }
               else 
               {
                  i = b[2];
                  if ( i==10 || i==2 || i==0 || i==1 || i==3 || i==9 || i==11 )
                  {
                     if (fread( &b[10], 1, 8, fp )==8 )
                     {
                        i = b[16];
                        if ( i==16 || i==24 || i==32 )
                        {
                           i = b[17];
                           if ( (0x30 & i)==0 ) 
                           {
                              filetype = TGA_FILETYPE;
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      fclose(fp);
   }

   return filetype;
}
