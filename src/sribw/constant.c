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
 * FILE:  constant.c
 *
 * DESCRIPTION:  The RI_ constants and matices to set a bicubic basis.
 *               
 *               Use libribwrtr.a for a more complete version of a RIB
 *               writer library.  The write.c file is just provided for 
 *               those who want something simple to use or refer to.
 *   
 *    Contains:
 *
 *    References:
 *        [PIXA89]  Pixar, The RenderMan Interface, Version 3.1, Richmond, CA, 
 *                  September 1989.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             Renderman (R) is a registered trademark of Pixar
 */
#define _RIPRIV_TYPES
#define __RIB_FILE_CONSTANT 
#define __FILE_CNSTNT 
#define __FILE_SBASIS 
#include <ripriv.h>

RtToken  RI_FRAMEBUFFER = "framebuffer";
RtToken  RI_FILE = "file";

RtToken  RI_RGB   = "rgb";
RtToken  RI_RGBA  = "rgba";
RtToken  RI_RGBZ  = "rgbz";
RtToken  RI_RGBAZ = "rgbaz";
RtToken  RI_A     = "a";
RtToken  RI_Z     = "z";
RtToken  RI_AZ    = "az";

RtToken  RI_PERSPECTIVE  = "perspective";
RtToken  RI_ORTHOGRAPHIC = "orthographic";

RtToken  RI_AFFINENOTICE= 
             "\t\tThe Affine Libraries and Tools are\n" \
             "\tCopyright (c) 1995, 1996, 1997, 1998 Thomas E. Burge.\n" \
             "\t\tAll rights reserved.\n";
RtToken  RI_HIDDEN   = "hidden";
RtToken  RI_PAINT    = "paint";
RtToken  RI_CONSTANT = "constant";
RtToken  RI_SMOOTH   = "smooth";
RtToken  RI_FLATNESS = "flatness";
RtToken  RI_FOV      = "fov";

RtToken  RI_AMBIENTLIGHT = "ambientlight"; 
RtToken  RI_POINTLIGHT   = "pointlight";
RtToken  RI_DISTANTLIGHT = "distantlight"; 
RtToken  RI_SPOTLIGHT    = "spotlight";

RtToken  RI_INTENSITY        = "intensity";
RtToken  RI_LIGHTCOLOR       = "lightcolor";
RtToken  RI_FROM             = "from";
RtToken  RI_TO               = "to";
RtToken  RI_CONEANGLE        = "coneangle";
RtToken  RI_CONEDELTAANGLE   = "conedeltaangle";
RtToken  RI_BEAMDISTRIBUTION = "beamdistribution";

RtToken  RI_MATTE      = "matte"; 
RtToken  RI_METAL      = "metal";
RtToken  RI_SHINYMETAL = "shinymetal";

RtToken  RI_PLASTIC = "plastic";
RtToken  RI_PAINTEDPLASTIC = "paintedplastic";

RtToken  RI_KA = "Ka";
RtToken  RI_KD = "Kd";
RtToken  RI_KS = "Ks";
RtToken  RI_KR = "Kr";
RtToken  RI_ROUGHNESS     = "roughness";
RtToken  RI_TEXTURENAME   = "texturename";
RtToken  RI_SPECULARCOLOR = "specularcolor";

RtToken  RI_DEPTHCUE = "depthcue";
RtToken  RI_FOG      = "fog";
RtToken  RI_BUMPY    = "bumpy";

RtToken  RI_DISTANCE    = "distance";
RtToken  RI_MINDISTANCE = "mindistance";
RtToken  RI_MAXDISTANCE = "maxdistance";
RtToken  RI_BACKGROUND  = "background";

RtToken  RI_INSIDE  = "inside";
RtToken  RI_OUTSIDE = "outside";
RtToken  RI_LH = "lh";
RtToken  RI_RH = "rh";

RtToken  RI_P  = "P";
RtToken  RI_PZ = "Pz";
RtToken  RI_PW = "Pw";

RtToken  RI_N  = "N";
RtToken  RI_NP = "Np";
RtToken  RI_CS = "Cs";
RtToken  RI_OS = "Os";
RtToken  RI_S  = "s";
RtToken  RI_T  = "t";
RtToken  RI_ST = "st";

RtToken  RI_PRIMITIVE    = "primitive";
RtToken  RI_INTERSECTION = "intersection";
RtToken  RI_UNION        = "union";
RtToken  RI_DIFFERENCE   = "difference";

RtToken  RI_CLAMP = "clamp";
RtToken  RI_BLACK = "black";

RtToken  RI_IGNORE  = "ignore";
RtToken  RI_PRINT   = "print";
RtToken  RI_ABORT   = "abort";
RtToken  RI_HANDLER = "handler";

RtToken  RI_BILINEAR    = "bilinear";
RtToken  RI_BICUBIC     = "bicubic";
RtToken  RI_PERIODIC    = "periodic";
RtToken  RI_NONPERIODIC = "nonperiodic";
RtToken  RI_TEAPOT      = "teapot";
RtToken  RI_COMMENT     = "comment";
RtToken  RI_STRUCTURE   = "structure";

RtToken RI_AMPLITUDE = "amplitude"; /* for bumpy shader */
RtToken RI_ORIGIN = "origin"; /* RiDisplay */

RtToken RI_PIXARNOTICE = 
          "\tThe RenderMan (R) Interface Procedures and Protocol are:\n" \
                     "\t\tCopyright 1988, 1989, Pixar\n" \
                         "\t\t\tAll Rights Reserved\n\n" \
              "\tRenderman (R) is a registered trademark of Pixar\n";

RtBasis RiBezierBasis = { { -1.0,  3.0, -3.0,  1.0 },
			  {  3.0, -6.0,  3.0,  0.0 },
			  { -3.0,  3.0,  0.0,  0.0 },
			  {  1.0,  0.0,  0.0,  0.0 }
			 };

/* Define One Sixth of */
#define Sx(a) ((a)/6)
RtBasis RiBSplineBasis = { { Sx(-1.0), Sx( 3.0), Sx(-3.0), Sx( 1.0) },
			   { Sx( 3.0), Sx(-6.0), Sx( 3.0), Sx( 0.0) },
			   { Sx(-3.0), Sx( 0.0), Sx( 3.0), Sx( 0.0) },
			   { Sx( 1.0), Sx( 4.0), Sx( 1.0), Sx( 0.0) }
                         };

/* Define One Half of */
#define Hf(a) ((a)/2)
RtBasis RiCatmullRomBasis = { { Hf(-1.0), Hf( 3.0), Hf(-3.0), Hf( 1.0) },
			      { Hf( 2.0), Hf(-5.0), Hf( 4.0), Hf(-1.0) },
			      { Hf(-1.0), Hf( 0.0), Hf( 1.0), Hf( 0.0) },
			      { Hf( 0.0), Hf( 2.0), Hf( 0.0), Hf( 0.0) }
			     };

RtBasis RiHermiteBasis = { {  2.0, -3.0, 0.0, 1.0 },
			   { -2.0,  3.0, 0.0, 0.0 },
			   {  1.0, -2.0, 1.0, 0.0 },
			   {  1.0, -1.0, 0.0, 0.0 }
			  };

RtBasis RiPowerBasis = { { 1.0, 0.0, 0.0, 0.0 },
			 { 0.0, 1.0, 0.0, 0.0 },
			 { 0.0, 0.0, 1.0, 0.0 },
			 { 0.0, 0.0, 0.0, 1.0 }
			};

