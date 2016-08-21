/*
 * 	color.c
 *	
 *      Xdisp routines for color map and bar.
 *	
 *      Switch_Colormap()
 *      ColorBar_Scale()
 *      Resize_ColorBar()
 *
 *	Copyright (c) B. Pike, 1993-1997
 */

#include "xdisp.h"
#include "acr_nema.h"



/*------------------ Switch_Colormap -----------------*/
void Switch_Colormap(void *data)
{
  int			i, j;
  register byte	        *bptr, tmp_byte;

  /* cycle through the color maps */
  if (color_table==0) 
    color_table=1;
  else if (color_table==1)
    color_table=2;
  else
    color_table=0;

  /* change pixel values if using a TrueColor Visual */
  if (Selected_Visual_Class==TrueColor) {
    bptr = byte_ColorBar;
    for (i=0; i<color_bar_height; i++) {
      for (j=0; j<color_bar_width; j++) {
	if (j<color_bar_text_width) {
          if (XBitmapBitOrder(theDisp)==MSBFirst) {
	    *bptr++ = 0;	/* pad */
	    *bptr++ = (byte) (newC[bcol].red>>8);
	    *bptr++ = (byte) (newC[bcol].green>>8);
	    *bptr++ = (byte) (newC[bcol].blue>>8);
	  }
	  else {
	    *bptr++ = (byte) (newC[bcol].blue>>8);
	    *bptr++ = (byte) (newC[bcol].green>>8);
	    *bptr++ = (byte) (newC[bcol].red>>8);
	    *bptr++ = 0;	/* pad */
	  }
	}
	else {	
	  tmp_byte = (byte) ((float)((color_bar_height-1)-i)*
			     255.0/(float)(color_bar_height-1));
          if (XBitmapBitOrder(theDisp)==MSBFirst) {
	  if (color_table == 0) { /* grey scale color table */
	    *bptr++ = tmp_byte; /* pad */
	    *bptr++ = tmp_byte; /* red index */
	    *bptr++ = tmp_byte; /* green index */
	    *bptr++ = tmp_byte; /* blue index */
	  }
	  else if (color_table == 1) { /* hot metal color table */
	    *bptr++ = tmp_byte;	  		   	 /* pad */
	    *bptr++ = hot_metal_table[2][(int)tmp_byte]; /* red index */
	    *bptr++ = hot_metal_table[1][(int)tmp_byte]; /* green index */
	    *bptr++ = hot_metal_table[0][(int)tmp_byte]; /* blue index */
	  }
	  else {
	    *bptr++ = tmp_byte;	  		  	/* pad */
	    *bptr++ = spectral_table[2][(int)tmp_byte]; /* red index */
	    *bptr++ = spectral_table[1][(int)tmp_byte]; /* green index */
	    *bptr++ = spectral_table[0][(int)tmp_byte]; /* blue index */
	  }
	  }
	  else {
	  if (color_table == 0) { /* grey scale color table */
	    *bptr++ = tmp_byte; /* blue index */
	    *bptr++ = tmp_byte; /* green index */
	    *bptr++ = tmp_byte; /* red index */
	    *bptr++ = tmp_byte; /* pad (not used but could be handy) */
	  }
	  else if (color_table == 1) { /* hot metal color table */
	    *bptr++ = hot_metal_table[2][(int)tmp_byte]; /* blue index */
	    *bptr++ = hot_metal_table[1][(int)tmp_byte]; /* green index */
	    *bptr++ = hot_metal_table[0][(int)tmp_byte]; /* red index */
	    *bptr++ = tmp_byte;	  		   	 /* pad */
	  }
	  else {
	    *bptr++ = spectral_table[2][(int)tmp_byte]; /* blue index */
	    *bptr++ = spectral_table[1][(int)tmp_byte]; /* green index */
	    *bptr++ = spectral_table[0][(int)tmp_byte]; /* red index */
	    *bptr++ = tmp_byte;	  		  	/* pad */
	  }
	  }
	}
      }
    }
  }

  /* update */    
  Window_Level(Lower,Upper);
  DrawWindow(0,0,zWidth,zHeight);
}


/*------------------------- ColorBar_Scale -------------------------------*/
void ColorBar_Scale(int min, int max)
{
  int 		i, j, skip;
  float 	fval, finc;
  byte          *tmp_byte;

  /* create a tmp array for colorbar index vlaues */
  tmp_byte = (byte *) malloc(color_bar_height);

  /* create a single index column */
  if (min<max) {
    finc = (float)(P_Max-P_Min)/(float)(color_bar_height-1);
    for (i=0; i<color_bar_height; i++) {
      fval = P_Max - ((float) i * finc);
      if ( fval <= (float) min )
	tmp_byte[i] = 0;
      else if  ( fval >= (float) max )
	tmp_byte[i] = 255;
      else 
	tmp_byte[i] = (byte) (255.0 * (fval-(float)min)/((float)(max-min)));
    }
  }
  else if (min>max) {
    finc = (float)(P_Min-P_Max)/(float)(color_bar_height-1);
    for (i=0; i<color_bar_height; i++) {
      fval = P_Min - ((float) i * finc);
      if ( fval >= (float) min )
	tmp_byte[i] = 255;
      else if  ( fval <= (float) max )
	tmp_byte[i] = 0;
      else 
	tmp_byte[i] = (byte) (255.0 * (fval-(float)max)/((float)(min-max)));
    }
  }
  else { /* min=max */
    finc = (float)(P_Max-P_Min)/255.0;
    for (i=0; i<color_bar_height; i++) {
      fval = P_Max - ((float) i * finc);
      if ( fval <= (float) min )
	tmp_byte[i] = 0;
      else 
	tmp_byte[i] = 255;
    }
  }

  /* perform Gamma correction if necessary */
  if (Gamma != 1.0) {
    for (i=0; i<color_bar_height; i++) {
      tmp_byte[i] = Gamma_Table[tmp_byte[i]];
    }
  }

  /* fill the color bar */
  for (i=0; i<color_bar_height; i++) {
    skip = i*color_bar_width*4;
    for(j=color_bar_text_width*4; j<color_bar_width*4; j+=4) {
      if (XBitmapBitOrder(theDisp)==MSBFirst) {
	if (color_table == 0) {      /* grey scale color table */
	  byte_ColorBar[skip+j+3] = tmp_byte[i]; /* blue index */
	  byte_ColorBar[skip+j+2] = tmp_byte[i]; /* green index */
	  byte_ColorBar[skip+j+1] = tmp_byte[i]; /* red index */
	  byte_ColorBar[skip+j+0] = tmp_byte[i]; /* pad (not used but could be handy) */
	}
	else if (color_table == 1) { /* hot metal color table */
	  byte_ColorBar[skip+j+3] = hot_metal_table[2][(int)tmp_byte[i]]; /* blue index */
	  byte_ColorBar[skip+j+2] = hot_metal_table[1][(int)tmp_byte[i]]; /* green index */
	  byte_ColorBar[skip+j+1] = hot_metal_table[0][(int)tmp_byte[i]]; /* red index */
	  byte_ColorBar[skip+j+0] = tmp_byte[i];	  		   	/* pad */
	}
	else {                       /* spectral color table */
	  byte_ColorBar[skip+j+3] = spectral_table[2][(int)tmp_byte[i]]; /* blue index */
	  byte_ColorBar[skip+j+2] = spectral_table[1][(int)tmp_byte[i]]; /* green index */
	  byte_ColorBar[skip+j+1] = spectral_table[0][(int)tmp_byte[i]]; /* red index */
	  byte_ColorBar[skip+j+0] = tmp_byte[i];	  		       /* pad */
	}
      }
      else {
	if (color_table == 0) {      /* grey scale color table */
	  byte_ColorBar[skip+j] = tmp_byte[i];   /* blue index */
	  byte_ColorBar[skip+j+1] = tmp_byte[i]; /* green index */
	  byte_ColorBar[skip+j+2] = tmp_byte[i]; /* red index */
	  byte_ColorBar[skip+j+3] = tmp_byte[i]; /* pad (not used but could be handy) */
	}
	else if (color_table == 1) { /* hot metal color table */
	  byte_ColorBar[skip+j] = hot_metal_table[2][(int)tmp_byte[i]];   /* blue index */
	  byte_ColorBar[skip+j+1] = hot_metal_table[1][(int)tmp_byte[i]]; /* green index */
	  byte_ColorBar[skip+j+2] = hot_metal_table[0][(int)tmp_byte[i]]; /* red index */
	  byte_ColorBar[skip+j+3] = tmp_byte[i];	  		   	/* pad */
	}
	else {                       /* spectral color table */
	  byte_ColorBar[skip+j] = spectral_table[2][(int)tmp_byte[i]];   /* blue index */
	  byte_ColorBar[skip+j+1] = spectral_table[1][(int)tmp_byte[i]]; /* green index */
	  byte_ColorBar[skip+j+2] = spectral_table[0][(int)tmp_byte[i]]; /* red index */
	  byte_ColorBar[skip+j+3] = tmp_byte[i];	  		       /* pad */
	}
      }
    }
  }

  /* clear workspace */
  free(tmp_byte);
}

/*--------------------------- Resize_ColorBar -------------------------*/
void Resize_ColorBar(int w, int h)
{
  byte 	*tmp_byte;

  /* interpolate color bar */
  if (Verbose) fprintf(stderr,"Resizing color bar to %dx%d ...",w,h);
  tmp_byte = (byte *) malloc((bitmap_pad/8)*color_bar_width*color_bar_height);
  memcpy(tmp_byte,byte_ColorBar,(bitmap_pad/8)*color_bar_width*color_bar_height);
  free(byte_ColorBar);
  byte_ColorBar=(byte *) malloc((bitmap_pad/8)*w*h);
  if (bitmap_pad == 8) {
    if (Interpolation_Type==BILINEAR) {
      bilinear_byte_to_byte ( &tmp_byte[0], color_bar_width, color_bar_height,
			      &byte_ColorBar[0], w, h );
    }
    else {
      nneighbour_byte_to_byte ( &tmp_byte[0], color_bar_width, color_bar_height,
				&byte_ColorBar[0], w, h );
    }
  }
  else {
    if (Interpolation_Type==BILINEAR) {
      bilinear_rgbp_to_rgbp ( &tmp_byte[0], color_bar_width, color_bar_height,
			      &byte_ColorBar[0], w, h );
    }
    else{
      nneighbour_rgbp_to_rgbp ( (uint32_t *) &tmp_byte[0],
                                color_bar_width, color_bar_height,
				(uint32_t *) &byte_ColorBar[0], w, h );
    }
  }
  color_bar_width = w;
  color_bar_height = h;                            
  theColorBar->data = (char *) &byte_ColorBar[0];
  theColorBar->height=h;
  theColorBar->width=w;
  theColorBar->bytes_per_line=(bitmap_pad/8)*w;
  free(tmp_byte);
  if (Verbose) fprintf(stderr,"Done.\n");    
}
