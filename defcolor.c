/*
 *	defcolor.c
 *
 *	Color-map and -bar setup code for Xdisp.
 *
 *	Define_ColorMap()
 *	Define_ColorBar()
 *      Toggle_ColorBar()
 *      TrueColor_Initialize()
 *      Toggle_Interpolation()
 *
 *	Copyright (c) Bruce Pike, 1993-2000
 */

#include <math.h>
#include "xdisp.h"

/*--------------------------- Define_ColorMap ------------------------*/
void Define_ColorMap(void)
{
  long         i;
	
  /* Get default ColorMap */
  for (i=0; i<ColorMapSize; i++) {
    rootC[i].pixel = i*red_shift + i*green_shift + i*blue_shift;
  }
  XQueryColors(theDisp,rootCmap,rootC,ColorMapSize);

  /* Attempt to allocate the required color map space politely */
  if (private_cmap)  {
    ColorMapOffset= COLOR_MAP_OFFSET;
    if (Verbose) fprintf(stderr,"Installing a new private colormap.\n");
    NewCmap = XCreateColormap(theDisp, rootW, theVisual, AllocAll);
    if (NewCmap==0) FatalError("Create colormap failed!\n");
    for (i=0; i<ColorMapSize; i++) {
      newC[i].pixel = i*red_shift + i*green_shift + i*blue_shift;
      newC[i].red   = rootC[i].red;
      newC[i].green = rootC[i].green;
      newC[i].blue  = rootC[i].blue;
      newC[i].flags = DoRed | DoGreen | DoBlue;
    }
  }
  else {
    if (Verbose) fprintf(stderr,
	 "Attempting to allocate %d colors in the default colormap... ",NumColors);
    if (XAllocColorCells(theDisp, rootCmap, True, &plane_mask, 0,
			 color_cells, NumColors)) {
      private_cmap = False;
      NewCmap = rootCmap;
      ColorMapOffset = color_cells[0];
      if (Verbose) {
	fprintf(stderr,"Done.\n");
	fprintf(stderr,"%d Colormap cells allocated in default colormap.\n", 
		NumColors);
      }
    }
    else {
      private_cmap = True;
      ColorMapOffset= COLOR_MAP_OFFSET;
      if (Verbose) fprintf(stderr,"Falied!\n");
      if (Verbose) fprintf(stderr,"Installing a new private colormap... ");

      /* Create new Colormap forcefully */
      NewCmap = XCreateColormap(theDisp, rootW, theVisual, AllocAll);  
      if (NewCmap==0) {
	FatalError("Failed!\n");
      }
      else {
	if (Verbose) fprintf(stderr,"Done.\n");            	
      }

      /* Setup colormap with root colors outside our range */
      for (i=0; i<ColorMapSize; i++) {
	newC[i].pixel = i*red_shift + i*green_shift + i*blue_shift;
	newC[i].red   = rootC[i].red;
	newC[i].green = rootC[i].green;
	newC[i].blue  = rootC[i].blue;
	newC[i].flags = DoRed | DoGreen | DoBlue;
      }
    }
  }

  /* define cells for various GC's */
  bcol = ColorMapOffset+NumColors-NumGCColors;
  fcol = ColorMapOffset+NumColors-NumGCColors+1;
  wp   = ColorMapOffset+NumColors-NumGCColors+2;
  bp   = ColorMapOffset+NumColors-NumGCColors+3;
  gcol = ColorMapOffset+NumColors-NumGCColors+4;

  /* define slot for black */
  newC[bp].pixel = bp*red_shift + bp*green_shift + bp*blue_shift;
  newC[bp].red = newC[bp].green = newC[bp].blue = 0<<8;
  newC[bp].flags = DoRed | DoGreen | DoBlue;
  cur_bg = newC[bp];

  /* define slot for white */
  newC[wp].pixel = wp*red_shift + wp*green_shift + wp*blue_shift;
  newC[wp].red = newC[wp].green = newC[wp].blue = 255<<8; 
  newC[wp].flags = DoRed | DoGreen | DoBlue;
  cur_fg = newC[wp];

  /* define slot for the foreground color */
  if (fg==NULL) {
    newC[fcol].pixel = fcol*red_shift + fcol*green_shift + fcol*blue_shift;
    newC[fcol].red =  newC[fcol].green = newC[fcol].blue  = 0<<8; 
    newC[fcol].flags = DoRed | DoGreen | DoBlue;
  }
  else {
    newC[fcol].pixel = fcol*red_shift + fcol*green_shift + fcol*blue_shift;
    if (!XParseColor(theDisp, NewCmap, fg, &newC[fcol]))
      FatalError("bad color specification");
  }

  /* define slot for the background color */
  if (bg==NULL) {
    newC[bcol].pixel = bcol*red_shift + bcol*green_shift + bcol*blue_shift;
    newC[bcol].red = newC[bcol].green = newC[bcol].blue  = 0xb800; 
    newC[bcol].flags = DoRed | DoGreen | DoBlue;
  }
  else {
    newC[bcol].pixel = bcol*red_shift + bcol*green_shift + bcol*blue_shift;
    if (!XParseColor(theDisp, NewCmap, bg, &newC[bcol]))
      FatalError("bad color specification");
  }

  /* define slot for the graphics color */
  if (grc==NULL) {
    newC[gcol].pixel = gcol*red_shift + gcol*green_shift + gcol*blue_shift;
    newC[gcol].red = 0xff00;
    newC[gcol].green = newC[gcol].blue  = 0;
    newC[gcol].flags = DoRed | DoGreen | DoBlue;
  }
  else {
    newC[gcol].pixel = gcol*red_shift + gcol*green_shift + gcol*blue_shift;
    if (!XParseColor(theDisp, NewCmap, grc, &newC[gcol]))
      FatalError("bad color specification");
    if (Verbose) fprintf(stderr,"OK...\n");
  }


  /* Store colors */

  if (private_cmap) {
    if (Verbose) fprintf(stderr,
			 "Storing %d colors in private colormap... ",
			 ColorMapSize);
    XStoreColors(theDisp,NewCmap,newC,ColorMapSize); 
    if (Verbose) fprintf(stderr,"Done.\n");
  } 
  else {
    if (Verbose) fprintf(stderr,
			 "Storing %d colors in shared colormap at offset %d ... ",
			 NumGCColors, ColorMapOffset+NumColors-NumGCColors);
    XStoreColors(theDisp,NewCmap,
		 &newC[ColorMapOffset+NumColors-NumGCColors],NumGCColors);
    if (Verbose) fprintf(stderr,"Done.\n");
  }

}

/*--------------------------- Define_ColorBar ------------------------*/
void Define_ColorBar(void)
{
  int i, j;
  byte	*bptr, tmp_byte;

  /* Create color bar byte image */
  if (file_format==MINC_FORMAT || 
      file_format==ACR_NEMA_FORMAT ||
      Input_Data_Type>SHORT_DATA) {
    color_bar_text_width = (color_bar_text_width >
			    XTextWidth(ol_text_finfo,"88888888-",9)) ?
      color_bar_text_width :
      XTextWidth(ol_text_finfo,"88888888-",9);
  }
  else {
    color_bar_text_width = (color_bar_text_width >
			    XTextWidth(ol_text_finfo,"-32767-",7)) ?
      color_bar_text_width :
      XTextWidth(ol_text_finfo,"-32767-",7);
  }
    
  color_bar_width = color_bar_text_width + color_bar_scale_width;
  color_bar_height = Height;
  byte_ColorBar = (byte *) malloc((bitmap_pad/8)*color_bar_width*Height);
  bptr = byte_ColorBar;
  for (i=0; i<Height; i++) {
    for (j=0; j<color_bar_width; j++) {
      if (Selected_Visual_Class != TrueColor) { /* Pseudo or Direct Color Visual */
	if (j<color_bar_text_width)
	  tmp_byte = (byte) bcol;
	else
	  tmp_byte = (byte) ((float)ColorMapOffset +
			     ((float)((Height-1)-i) *
			      (float)(NumColors-NumGCColors-1))/
			     (float)(Height-1));
	if (Selected_Visual_Class == PseudoColor) {
	  *bptr++ = tmp_byte;	/* pseudo color lookup index */        	
	}
	else {
	  *bptr++ = tmp_byte;   /* red index */
	  *bptr++ = tmp_byte;	/* green index */
	  *bptr++ = tmp_byte;	/* blue index */
	  *bptr++ = 0;	        /* pad */
	}
      }
      else {	/* TrueColor Visual */
	if (j<color_bar_text_width) {
          if (XBitmapBitOrder(theDisp)==MSBFirst) {
 	    *bptr++ = bcol;	/* pad */
	    *bptr++ = (byte) (newC[bcol].red>>8);
	    *bptr++ = (byte) (newC[bcol].green>>8);
	    *bptr++ = (byte) (newC[bcol].blue>>8);
	    }
	  else {
	    *bptr++ = (byte) (newC[bcol].blue>>8);
	    *bptr++ = (byte) (newC[bcol].green>>8);
	    *bptr++ = (byte) (newC[bcol].red>>8);
	    *bptr++ = bcol;	/* pad */
	  }
	}
	else {
	  tmp_byte = (byte) ((float)((Height-1)-i)*255.0/(float)(Height-1));
	  if (color_table == 0) { /* grey scale color table */
	    *bptr++ = tmp_byte;  /* blue index */
	    *bptr++ = tmp_byte;  /* green index */
	    *bptr++ = tmp_byte;  /* red index */
	    *bptr++ = tmp_byte;  /* pad (not used but could be handy) */
	  }
	  else if (color_table == 1) { /* hot metal color table */
            if (XBitmapBitOrder(theDisp)==MSBFirst) {
	      *bptr++ = tmp_byte;	  		         /* pad */
	      *bptr++ = hot_metal_table[0][(int)tmp_byte]; /* red index */
	      *bptr++ = hot_metal_table[1][(int)tmp_byte]; /* green index */
	      *bptr++ = hot_metal_table[2][(int)tmp_byte]; /* blue index */
	    }
	    else {
	      *bptr++ = hot_metal_table[2][(int)tmp_byte]; /* blue index */
	      *bptr++ = hot_metal_table[1][(int)tmp_byte]; /* green index */
	      *bptr++ = hot_metal_table[0][(int)tmp_byte]; /* red index */
	      *bptr++ = tmp_byte;	  		         /* pad */
	    }
	  }
	  else {
            if (XBitmapBitOrder(theDisp)==MSBFirst) {
	      *bptr++ = tmp_byte;	  		        /* pad */
	      *bptr++ = spectral_table[0][(int)tmp_byte]; /* red index */
	      *bptr++ = spectral_table[1][(int)tmp_byte]; /* green index */
	      *bptr++ = spectral_table[2][(int)tmp_byte]; /* blue index */
	    }
	    else {
	      *bptr++ = spectral_table[2][(int)tmp_byte]; /* blue index */
	      *bptr++ = spectral_table[1][(int)tmp_byte]; /* green index */
	      *bptr++ = spectral_table[0][(int)tmp_byte]; /* red index */
	      *bptr++ = tmp_byte;	  		        /* pad */
	    }
	  }
	}
      }
    }
  }
}

/*--------------------------- Toggle_ColorBar ------------------------*/
void Toggle_ColorBar(void *data)
{
  if (color_bar) 
    color_bar = 0;
  else 
    color_bar = 1;
  XResizeWindow(theDisp,mainW,
		zWidth+(color_bar?color_bar_width:0),
		zHeight+info_height);
  Resize(zWidth,zHeight);
}

/*--------------------------- TrueColor_Initialize ------------------------*/
void TrueColor_Initialize(void)
{

  /* create a new colormap */
  NewCmap = XCreateColormap(theDisp, rootW, theVisual, AllocNone);  
  if (NewCmap==0) {
    FatalError("Error creating colormap!\n");
  }

  /* define black pixel */
  bp = 0;
  newC[bp].pixel = 0;
  newC[bp].red = newC[bp].green = newC[bp].blue = 0;
  newC[bp].flags = DoRed | DoGreen | DoBlue;
  cur_bg.pixel = newC[bp].pixel;

  /* define white pixel */
  wp = 255;
  newC[wp].pixel = wp*red_shift + wp*green_shift + wp*blue_shift;
  newC[wp].red = newC[wp].green = newC[wp].blue = 255<<8;
  newC[wp].flags = DoRed | DoGreen | DoBlue;
  cur_fg = newC[wp];
    
  /* define slot for the foreground color (default=black)*/
  fcol = 1;
  if (fg==NULL) {
    newC[fcol].pixel = 0;
    newC[fcol].red =  newC[fcol].green = newC[fcol].blue  = 0<<8; 
    newC[fcol].flags = DoRed | DoGreen | DoBlue;
  }
  else {
    if (!XParseColor(theDisp, rootCmap, fg, &newC[fcol]))
      FatalError("bad color specification");
    newC[fcol].pixel = (newC[fcol].red>>8)*red_shift + 
      (newC[fcol].green>>8)*green_shift + 
      (newC[fcol].blue>>8)*blue_shift;
    if ((newC[fcol].flags&DoRed)==0) newC[fcol].red = 0;
    if ((newC[fcol].flags&DoGreen)==0) newC[fcol].green = 0;
    if ((newC[fcol].flags&DoBlue)==0) newC[fcol].blue = 0;
  }

  /* define slot for the background color (defualt=grey) */
  bcol = 2;
  if (bg==NULL) {
    newC[bcol].pixel = 0xb8*red_shift + 0xb8*green_shift + 0xb8*blue_shift;
    newC[bcol].red = newC[bcol].green = newC[bcol].blue  = 0xb800; 
    newC[bcol].flags = DoRed | DoGreen | DoBlue;
  }
  else {
    if (!XParseColor(theDisp, rootCmap, bg, &newC[bcol]))
      FatalError("bad color specification");
    newC[bcol].pixel = (newC[bcol].red>>8)*red_shift + 
      (newC[bcol].green>>8)*green_shift + 
      (newC[bcol].blue>>8)*blue_shift;
    if ((newC[bcol].flags&DoRed)==0) newC[bcol].red = 0;
    if ((newC[bcol].flags&DoGreen)==0) newC[bcol].green = 0;
    if ((newC[bcol].flags&DoBlue)==0) newC[bcol].blue = 0;
  }

  /* define slot for the graphics color (default=red) */
  gcol = 3;
  if (grc==NULL) {
    newC[gcol].pixel = 255*red_shift + 255*green_shift + 255*blue_shift;
    newC[gcol].red = newC[gcol].green = newC[gcol].blue  = 0xff00;
    newC[gcol].flags = DoRed | DoGreen | DoBlue;
  }
  else {
    if (!XParseColor(theDisp, NewCmap, grc, &newC[gcol]))
      FatalError("bad color specification");
    newC[gcol].pixel = (newC[gcol].red>>8)*red_shift + 
      (newC[gcol].green>>8)*green_shift + 
      (newC[gcol].blue>>8)*blue_shift;
    if ((newC[gcol].flags&DoRed)==0) newC[gcol].red = 0;
    if ((newC[gcol].flags&DoGreen)==0) newC[gcol].green = 0;
    if ((newC[gcol].flags&DoBlue)==0) newC[gcol].blue = 0;
  }

  /* define the pixel scale range */

  if (Verbose) fprintf(stderr,"Done with TrureColor initialization\n");
}

/*--------------------------- Toggle_Interpolation ------------------------*/
void Toggle_Interpolation(void *data)
{
  if (Interpolation_Type==BILINEAR) 
    Interpolation_Type= NEAREST_NEIGHBOUR;
  else 
    Interpolation_Type= BILINEAR;
  image_increment=0;
  New_Image();
}
