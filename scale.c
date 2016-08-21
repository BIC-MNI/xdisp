/*
 * 	scale.c
 *
 *	Scaling routines for xdisp.
 *
 *	Scale_Image()
 *      Rescale()
 *      Auto_Scale()
 *      Scale_Short_to_Byte()
 *
 * 	Copyright (c) Bruce Pike 1993-2000
 *
 */

#include "xdisp.h"
#include <sys/stat.h>

/*---------------------------- Scale_Image -------------------------------*/
void Scale_Image(int min, int max)
{
  int 		        i, im;
  float 		fcmo, fnc, fwin, fmin, fmax, fd;
  register byte 	*bptr, pix_byte;
  register short 	*sptr;

  /* define a few things */
  if (Verbose) fprintf(stderr,
		       "Scaling image(s) to range %d to %d...\n",min,max);
  P_Min = S_Min = min;
  P_Max = S_Max = max;
  fcmo = ColorMapOffset;
  fnc = NumColors-NumGCColors-1;
  fmax = max;
  fmin = min;
  fwin = fmax-fmin;
  fd = fnc/fwin;
  sptr = short_Image;
  bptr = byte_Image;

  /* check the size of byte_image */
  if (zWidth<Width || zHeight<Height) {
    free(byte_Image);
    byte_Image = (byte *) malloc((bitmap_pad/8)*Width*Height*
				 (load_all_images?num_images:1));
  }

  /* perform scale */
  for (im=0; im<(load_all_images?num_images:1); im++) {
    if ((num_images>1)&&(load_all_images)) fprintf(stderr,"Scaling image %d\r",im);
    for (i=0; i<Height*Width; i++,sptr++) {
      if (*sptr <= ((min<max)?min:max)) {
	pix_byte = (min<max) ? (byte)ColorMapOffset :
	  (byte)ColorMapOffset+NumColors-NumGCColors-1;
      }
      else if (*sptr >= ((max>min)?max:min)) {
	pix_byte = (max<min) ? (byte)ColorMapOffset :
	  (byte)ColorMapOffset+NumColors-NumGCColors-1;
      }
      else {
	pix_byte = (byte) (fcmo+((float)(*sptr)-fmin)*fd);
      }
      if (Selected_Visual_Class == PseudoColor) {
	*bptr++ = pix_byte;	
      }
      else if (Selected_Visual_Class == DirectColor) {
	*bptr++ = pix_byte;	/* red   index */
	*bptr++ = pix_byte;	/* green index */
	*bptr++ = pix_byte;	/* blue  index */
	*bptr++ = 0;   	        /* pad */
      }
      else { /* TrueColor */
	if (color_table == 0) { /* grey scale color table */
	  *bptr++ = pix_byte; /* blue index */
	  *bptr++ = pix_byte; /* green index */
	  *bptr++ = pix_byte; /* red index */
	  *bptr++ = pix_byte; /* pad with index (bot used) */
	}
	else if (color_table == 1) { /* hot metal color table */
	  *bptr++ = hot_metal_table[2][(int)pix_byte]; /* blue index */
	  *bptr++ = hot_metal_table[1][(int)pix_byte]; /* green index */
	  *bptr++ = hot_metal_table[0][(int)pix_byte]; /* red index */
	  *bptr++ = pix_byte;	  		       /* pad */
	}
	else { /* spectral */
	  *bptr++ = spectral_table[2][(int)pix_byte]; /* blue index */
	  *bptr++ = spectral_table[1][(int)pix_byte]; /* green index */
	  *bptr++ = spectral_table[0][(int)pix_byte]; /* red index */
	  *bptr++ = pix_byte;	                      /* pad */
	}
      }	  
    }
    if ((num_images>1)&&(load_all_images)) 
      fprintf(stderr,"                             \r");
  }

  /* reset step size */
  steps=((max-min)/(2*zWidth)>=1) ? (max-min)/(2*zWidth): 1;
  update_sliders();
  update_msgs();
  

}

/*---------------------------- Rescale ------------------------------*/
void Rescale(void *data)
{
  int	w,h;

  /* define new cursors */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);

  /* perform rescale */
  Scale_Image(Lower,Upper); 

  /* resize if needed */
  if (Width!=zWidth || Height!=zHeight) {
    w=zWidth; h=zHeight;
    zWidth=Width; zHeight=Height;
    Resize(w,h);
    XResizeWindow(theDisp,mainW,
		  zWidth+(color_bar?color_bar_width:0),
		  zHeight+info_height);
  }
  Window_Level(Lower,Upper); 
  DrawWindow(0,0,zWidth,zHeight);
  update_sliders();
  update_msgs();          
    
  /* define new cursors */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}

/*-------------------------- Auto_Scale ----------------------------*/
void Auto_Scale(void *data)
{
  float	   sum, sq_sum;
  int		   i, count;

  sq_sum = sum = 0.0;
  count = Height*Width*(load_all_images?num_images:1);    
  if ((I_Mean==0)&&(I_Std==0)) {
    for (i=0; i<Height*Width*(load_all_images?num_images:1); i++) {
      if (short_Image[i] > I_Max) I_Max=short_Image[i];
      if (short_Image[i] < I_Min) I_Min=short_Image[i];
      sum += (float)short_Image[i];
      sq_sum += (float)(short_Image[i])*(short_Image[i]);
    }
    I_Mean = sum/count;
    I_Std = sqrt((sq_sum - 2*I_Mean*sum + I_Mean*I_Mean*count)/(count-1));
  }
  Lower = (int)(I_Mean - 0.5*I_Std) < I_Min ? 
    I_Min : (int)(I_Mean - 0.5*I_Std);
  Upper = (int)(I_Mean + 4.0*I_Std) > I_Max ?
    I_Max : (int)(I_Mean + 4.0*I_Std);
  Rescale(NULL);
}

/*------------------------- Scale_Short_to_Byte -------------------------------*/
void Scale_Short_to_Byte(short *s_im, int s_w, int s_h,
			byte *b_im,  int b_w, int b_h,
			int min, int max)
{
  int 		i;
  float 	fwin;
  byte          *tmp_byte;

  /* create work space */
  tmp_byte = (byte *) malloc(s_w*s_h);

  /* perform the scale */
  if (min<max) {
    fwin = 255.0 / (float)(max-min);
    for (i=0; i<s_w*s_h; i++) {
      if (s_im[i] <= min) {
	tmp_byte[i] = 0;
      }
      else if (s_im[i] >= max) {
	tmp_byte[i] = 255;
      }
      else {
	tmp_byte[i] = (byte) ((float)(s_im[i]-min) * fwin);
      }
    }
  }
  else if (min>max) {
    fwin = 255.0 / (float)(min-max);
    for (i=0; i<s_w*s_h; i++) {
      if (s_im[i] <= max) {
	tmp_byte[i] = 255;
      }
      else if (s_im[i] >= min) {
	tmp_byte[i] = 0;
      }
      else {
	tmp_byte[i] = 255 - (byte) ((float)(s_im[i]-max) * fwin);
      }
    }
  }
  else { /* min=max */
    for (i=0; i<s_w*s_h; i++) {
      if (s_im[i] <= min) {
	tmp_byte[i] = 0;
      }
      else {
	tmp_byte[i] = 255;
      }
    }
  }

  /* perform gamma correction if necessary */
  if (Gamma != 1.0) {
    for (i=0; i<s_w*s_h; i++) {
      tmp_byte[i] = Gamma_Table[tmp_byte[i]];
    }
  }
  
  /* check input and output size to see if interpolation is required */
  if (s_w!=b_w || s_h!=b_h) {
    if (Interpolation_Type==BILINEAR) {
      bilinear_byte_to_byte(tmp_byte,s_w,s_h,b_im,b_w,b_h);
    }
    else {
      nneighbour_byte_to_byte(tmp_byte,s_w,s_h,b_im,b_w,b_h);
    }
  }
  else {
    memcpy(b_im,tmp_byte,b_w*b_h); 
  }

  /* clear workspace */
  free(tmp_byte);
}

