// Image handling tools
// Copyright (c) 2001 Adrian Kennard
// This software is provided under the terms of the GPL v2 or later.
// This software is provided free of charge with a full "Money back" guarantee.
// Use entirely at your own risk. We accept no liability. If you don't like that - don't use it.

// Indended for use in applications that need to make images for web pages on the fly
//
// $Log: image.h,v $
// Revision 1.4  2006/12/25 08:59:13  cvs
// Updated for CASHGBP CASHUSD CASHEUR types
//
// Revision 1.3  2006/01/25 11:10:43  cvs
// New sqlhtml
//
// Revision 1.2  2004/09/09 07:45:09  cvs
// Added change history to source files
// Added "info" type to IEC16022
// Added exact size checking shortcodes on encoding generation for iec16022
//

typedef uint32_t Colour;        // RGB value

// Image object
typedef struct
{
   int W,                       // width
     L,                         // Line length in Image (W+1)
     H;                         // height
   uint8_t *Image;              // image array, one byte per pixel
   int C;                       // colours (can be non power of 2, max 256)
   Colour *Colour;              // colour map (must have entry for each colour)
}
Image;

typedef struct
{                               // Structure for SVG path tools
   int initx,
     inity;                     // Initial x/y for closed path
   int lastx,
     lasty;                     // Last output in path
   int skipx,
     skipy;                     // Last point, not yet output
   unsigned char last:1;        // If Last set
   unsigned char skip:1;        // If skip set
   unsigned char noskip:1;      // Don't skip parts (e.g. marker-mid is being used)
   char text[24];               // Allow L/M %d %d
} Path;

// macros and functions

#define ImagePixel(i,x,y)	((i)->Image[1+(i)->L*(y)+(x)])

Image *ImageNew (int w, int h, int c);  // create a new blank image
void ImageFree (Image * i);     // free an image
void ImageWriteGif (Image * i, int fh, int back, int trans, char *comment);     // write image as a gif
void ImageWritePNGz (Image * i, int fh, int back, int trans, unsigned int dpm,char *comment, int z);     // write image as a png
#define ImageWritePNG(i,fh,back,trans,comment) ImageWritePNGz(i,fh,back,trans,0,comment,9)
void ImageWriteSVG (Image * i, int fh, int back, int trans, char *comment, double scale); // scale is mm per pixel, 0 for no scale just units
void ImageSVGPath (Image * i, FILE * o, int c);
void PathInit (Path * p);       // Initialise a path
char *PathPlot (Path * p, int x, int y);        // add a point, returns string to output or NULL
char *PathDone (Path * p);      // finish path, returns string to output or NULL

typedef void ImagePixelFunc (Image * i, int X, int Y, int x, int y, int c);
ImagePixelFunc ImageTextPixel;
void ImageText8x8F (Image * i, int x, int y, int c, char *text, ImagePixelFunc *);      // write 8x8 text
void ImageText3x5F (Image * i, int x, int y, int c, char *text, ImagePixelFunc *);      // write 3x5 text
void ImageText5x9F (Image * i, int x, int y, int c, char *text, ImagePixelFunc *);      // write 5x9 text
void ImageText (Image * i, int x, int y, int c, char *text);    // write 8x8 text
void ImageSmall (Image * i, int x, int y, int c, char *text);   // write 4x6 text
void ImageRect (Image * i, int x, int y, int w, int h, int c);  // fill a box
#define ImageWrite ImageWritePNG        // default
#define ImageText8x8(i,x,y,c,t) ImageText8x8F(i,x,y,c,t,ImageTextPixel)
#define ImageText3x5(i,x,y,c,t) ImageText3x5F(i,x,y,c,t,ImageTextPixel)
#define ImageText5x9(i,x,y,c,t) ImageText5x9F(i,x,y,c,t,ImageTextPixel)
