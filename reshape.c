/*
 * 	reshape.c
 *
 *	Image reshaping routines for Xdisp.
 *
 *	Resize()
 *	zoom()
 *	Crop()
 *	Flip_Image()
 *	Rotate_Image()
 *	Reorient_Volume()
 *	_Reorient_Volume()
 *
 * 	Copyright (c) Bruce Pike 1993-2000
 *
 */

#include "xdisp.h"
#include <sys/stat.h>

/*----------------------------- Resize -----------------------------*/
void Resize(int w, int h)
{
  byte 	*tmp_byte;
  int	i;

  /* define new cursors */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);
  if (Verbose) fprintf(stderr,"Resizing image to %dx%d\n",w,h);

  /* place reasonable bounds on the width and height */
  w = w<1 ? 1 : w;
  h = h<1 ? 1 : h;
    
  /* first resize the color bar */
  Resize_ColorBar(color_bar_width,h);  
    
  /* special case */
  if (w==zWidth && h==zHeight) {
    theImage->data = (char *) &byte_Image[zWidth*zHeight*(bitmap_pad/8)*
                                          (load_all_images?image_number:0)];
  }
  else {
    /* perform the interpolation on the byte_image */
    if (Verbose) fprintf(stderr,"Interpolating image from %d x %d to %d x %d\n",
			 zWidth,zHeight,w,h);
    
    tmp_byte =  (byte *) malloc((bitmap_pad/8)*zWidth*zHeight*(load_all_images?num_images:1));
    
    memcpy(tmp_byte,byte_Image,(bitmap_pad/8)*zWidth*zHeight*(load_all_images?num_images:1));
    
    free(byte_Image);
    
    byte_Image=(byte *) malloc((bitmap_pad/8)*w*h*(load_all_images?num_images:1));

    for (i=0; i<(load_all_images?num_images:1); i++) {
      if ((num_images>1)&&(load_all_images)) {
        fprintf(stderr,"Resizing image %d \r",i+1);
      }
      if (bitmap_pad==8) {
	if (Interpolation_Type==BILINEAR) {
	  bilinear_byte_to_byte ( &tmp_byte[zWidth*zHeight*i], zWidth, zHeight,
				  &byte_Image[w*h*i], w, h );
	}
	else {
	  nneighbour_byte_to_byte ( &tmp_byte[zWidth*zHeight*i], zWidth, zHeight,
				    &byte_Image[w*h*i], w, h );
	}
      }
      else {
	if (Interpolation_Type==BILINEAR) {
	  bilinear_rgbp_to_rgbp ( &tmp_byte[(bitmap_pad/8)*zWidth*zHeight*i], 
				  zWidth, zHeight,
				  &byte_Image[(bitmap_pad/8)*w*h*i], w, h );
	}
	else {
	  nneighbour_rgbp_to_rgbp ( (uint32_t *) &tmp_byte[(bitmap_pad/8)*zWidth*zHeight*i], 
				    zWidth, zHeight,
				    (uint32_t *) &byte_Image[(bitmap_pad/8)*w*h*i], w, h );
	}
      }
    }
    if ((num_images>1)&&(load_all_images)) {
      fprintf(stderr,"                    \r");
    }
    
    /* set theImage variables */
    theImage->data = (char *) &byte_Image[(bitmap_pad/8)*w*h*
                                          (load_all_images?image_number:0)];

    /* clean up */
    free(tmp_byte);

    /* update dimensions */
    zWidth=w;
    zHeight=h;

    /* reset step size for window and level adjustment */
    steps=((P_Max-P_Min)/(2*Width)>=1) ? (P_Max-P_Min)/(2*Width): 1;
    XFlush(theDisp);                             
  }

  /* show result */
  theImage->width=w;
  theImage->height=h;
  theImage->bytes_per_line=w*(bitmap_pad/8);
  DrawWindow(0,0,zWidth,zHeight);
  Window_Level(Lower,Upper);

  /* define new cursors */
  zoom_factor_x = (float)zWidth/oWidth;
  zoom_factor_y = (float)zHeight/oHeight;
  update_msgs();
  XClearArea(theDisp, mainW, 0, zHeight,
	     zWidth,zHeight+info_height, False);
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);    
}

/*-------------------------- zoom ---------------------------------*/
void zoom(void *data)
{
  int 	w, h, up;

  up = *((int *) data);
    
  if (up) {
    w = zWidth*2;
    h = zHeight*2;
  }
  else {
    w = zWidth/2;
    h = zHeight/2;
  }

  Resize(w,h);
  XResizeWindow(theDisp,mainW,
		w+(color_bar?color_bar_width:0),
		h+info_height);
  Window_Level(Lower,Upper);
  update_sliders();
  update_msgs();
}


/*---------------------------- Crop ------------------------------*/
void Crop(void *data)
{
  int	i, j, index, im, im_index, x1, x2, y1, y2;

  /* check that an roi is defined */
  if (roi_present != 1) {
    fprintf(stderr,"An ROI must be drawn before cropping\n");
    return;
  }

  /* define new cursors */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);
  fprintf(stderr,"Cropping image at (%d,%d) to (%d,%d)\n",
	  roi_x1,roi_y1,roi_x2,roi_y2);

  /* get roi corners in original image coordinates */
  if (roi_x2 < roi_x1) {
    i  = roi_x1;
    roi_x1 = roi_x2;
    roi_x2 = i;
  }
  if (roi_y2 < roi_y1) {
    i  = roi_y1;
    roi_y1 = roi_y2;
    roi_y2 = i;
  }
  x1 = roi_x1*(Width-1)/(zWidth-1);
  x2 = roi_x2*(Width-1)/(zWidth-1);
  y1 = roi_y1*(Height-1)/(zHeight-1);
  y2 = roi_y2*(Height-1)/(zHeight-1);
  crop_x = crop_x + x1;
  crop_y = crop_y + y1;

  /* perform cropping in-place */
  index=0;
  for (im=0; im<(load_all_images?num_images:1); im++) {
    if ((num_images>1)&&load_all_images) fprintf(stderr,"Cropping short image %d\r",im);
    im_index=Width*Height*im;
    for (j=y1; j<=y2; j++) {
      for (i=x1; i<=x2; i++, index++) {
	short_Image[index]=short_Image[j*Width+i+im_index];
      }
    }
  }
  if ((num_images>1)&&load_all_images) 
    fprintf(stderr,"                                          \r");

  index=0;
  for (im=0; im<(load_all_images?num_images:1); im++) {
    if ((num_images>1)&&load_all_images) fprintf(stderr,"Cropping X image %d\r",im);
    im_index=(bitmap_pad/8)*zWidth*zHeight*im;
    if (bitmap_pad==8) {
      for (j=roi_y1; j<=roi_y2; j++) {
	for (i=roi_x1; i<=roi_x2; i++) {
	  byte_Image[index++]=byte_Image[j*zWidth+i+im_index];
	}
      }
    }
    else {
      for (j=roi_y1; j<=roi_y2; j++) {
	for (i=roi_x1; i<=roi_x2; i++) {
	  byte_Image[index++]=byte_Image[j*zWidth*4+i*4+im_index];
	  byte_Image[index++]=byte_Image[j*zWidth*4+i*4+im_index+1];
	  byte_Image[index++]=byte_Image[j*zWidth*4+i*4+im_index+2];
	  byte_Image[index++]=byte_Image[j*zWidth*4+i*4+im_index+3];
	}
      }
    }

  }
  if ((num_images>1)&&load_all_images) 
    fprintf(stderr,"                                          \r");

  /* resize the color bar */
  Resize_ColorBar(color_bar_width,abs(roi_y2-roi_y1)+1);

  /* define new image dimensions */
  zWidth = theImage->width = abs(roi_x2 - roi_x1) + 1;
  theImage->bytes_per_line = (bitmap_pad/8)* (abs(roi_x2 - roi_x1) + 1);
  zHeight = theImage->height = abs(roi_y2 - roi_y1) + 1;
  theImage->data = (char *) &byte_Image[(bitmap_pad/8)*zWidth*zHeight*
                                        (load_all_images?image_number:0)];
  Width = abs(x2 - x1) + 1;
  Height = abs(y2 - y1) + 1;

  /* resize window and image */
  XResizeWindow(theDisp,mainW,
		zWidth+(color_bar?color_bar_width:0),
		zHeight+info_height);
  DrawWindow(0,0,zWidth,zHeight);

  /* reset window/level step size */
  cropped = True;
  RW_valid = False;
  steps=((P_Max-P_Min)/(2*zWidth)>=1) ? (P_Max-P_Min)/(2*zWidth): 1;
  update_msgs();
  update_sliders();
    
  /* define new cursors */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}

/*----------------------- Flip_Image -------------------------------*/
void Flip_Image(void *data)
{
  int 	i, im, im_index, j, flip_y;
  short	*tmp_short;
  byte	*tmp_byte;

  flip_y = *((int *) data);
    
  /* define new cursors */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);
  if (Verbose) {
    if (flip_y) {
      fprintf(stderr,"Image flip about y-axis in progress...\n");
    }
    else {
      fprintf(stderr,"Image flip about x-axis in progress...\n");
    }
  }
  
  /* malloc space */
  tmp_short = (short *) malloc(Width*Height*2);
  tmp_byte = (byte *) malloc(zWidth*zHeight*(bitmap_pad/8));

  /* perform the flip */
  if (flip_y) { /* flip rows */
    for (im=0; im<(load_all_images?num_images:1); im++) {
      if ((num_images>1)&&load_all_images) fprintf(stderr,"Flipping image %d\r",im);
      memcpy(tmp_short,&short_Image[Width*Height*im],Width*Height*2);
      memcpy(tmp_byte,&byte_Image[zWidth*zHeight*im*(bitmap_pad/8)],
	     zWidth*zHeight*(bitmap_pad/8));
      im_index=zWidth*zHeight*im*(bitmap_pad/8);
      if (bitmap_pad==8) {
	for (i=0; i<zHeight; i++) {
	  for (j=0; j<zWidth; j++) {
	    byte_Image[(zHeight-i-1)*zWidth+j+im_index] = tmp_byte[i*zWidth+j];
	  }
	}
      }
      else {
	for (i=0; i<zHeight; i++) {
	  for (j=0; j<zWidth*4; j++) {
	    byte_Image[(zHeight-i-1)*zWidth*4+j+im_index] = tmp_byte[i*zWidth*4+j];
	  }
	}
      }
      im_index=Width*Height*im;
      for (i=0; i<Height; i++) {
	for (j=0; j<Width; j++) {
	  short_Image[(Height-i-1)*Width+j+im_index] = tmp_short[i*Width+j];
	}
      }
    }
    Y_reverse = !Y_reverse; /* toggle the Y_reverse flag */
  }
  else    {    	/* flip cols */
    for (im=0; im<(load_all_images?num_images:1); im++) {
      if ((num_images>1)&&load_all_images) fprintf(stderr,"Flipping image %d\r",im);
      memcpy(tmp_short,&short_Image[Width*Height*im],Width*Height*2);
      memcpy(tmp_byte,&byte_Image[zWidth*zHeight*im*(bitmap_pad/8)],
	     zWidth*zHeight*(bitmap_pad/8));
      im_index=zWidth*zHeight*im*(bitmap_pad/8);
      if (bitmap_pad == 8) {
	for (i=0; i<zHeight; i++){
	  for (j=0; j<zWidth; j++) {
	    byte_Image[i*zWidth+(zWidth-j-1)+im_index] = tmp_byte[i*zWidth+j];
	  }
	}
      }
      else {
	for (i=0; i<zHeight; i++){
	  for (j=0; j<zWidth; j++) {
	    byte_Image[i*zWidth*4+(zWidth-1-j)*4+im_index] = 
	      tmp_byte[i*zWidth*4+j*4];
	    byte_Image[i*zWidth*4+(zWidth-1-j)*4+im_index+1] = 
	      tmp_byte[i*zWidth*4+j*4+1];
	    byte_Image[i*zWidth*4+(zWidth-1-j)*4+im_index+2] = 
	      tmp_byte[i*zWidth*4+j*4+2];
	    byte_Image[i*zWidth*4+(zWidth-1-j)*4+im_index+3] = 
	      tmp_byte[i*zWidth*4+j*4+3];
	  }
	}
      }
      im_index=Width*Height*im;
      for (i=0; i<Height; i++) {
	for (j=0; j<Width; j++) {
	  short_Image[i*Width+(Width-j-1)+im_index] = tmp_short[i*Width+j];
	}
      }
    }
    X_reverse = !X_reverse; /* toggle the X_reverse flag */
  }
  if ((num_images>1)&&load_all_images) fprintf(stderr,"                     \r");

  /* Resize if necessary */
  Resize(zWidth,zHeight);

  /* redraw image and clean up */
  free(tmp_short);
  free(tmp_byte);
  DrawWindow(0,0,zWidth,zHeight);

  /* define new cursors */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}

/*----------------------- Rotate_Image -------------------------------*/
void Rotate_Image(void *data)
{
  int 	i, im, im_index, j, tmp;
  short	*tmp_short;
  byte	*tmp_byte;

  /* define new cursors */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);
  if (Verbose) fprintf(stderr,"Clockwise image rotate in progress...\n");

  /* resize the color bar */
  Resize_ColorBar(color_bar_width,zWidth);

  /* malloc space */
  tmp_short = (short *) malloc(Width*Height*2);
  tmp_byte = (byte *) malloc(zWidth*zHeight*(bitmap_pad/8));

  /* Swap the width and height */
  tmp=Height;  Height=Width;  Width=tmp;
  tmp=zHeight;  zHeight=zWidth; zWidth=tmp;
  theImage->width  = zWidth;
  theImage->height = zHeight;
  theImage->bytes_per_line = zWidth*(bitmap_pad/8);

  /* perform the rotation */
  for (im=0; im<(load_all_images?num_images:1); im++) {
    if ((num_images>1)&&load_all_images) fprintf(stderr,"Rotating image %d\r",im);
    memcpy(tmp_short,&short_Image[Width*Height*im],Width*Height*2);
    memcpy(tmp_byte,&byte_Image[zWidth*zHeight*im*(bitmap_pad/8)],
	   zWidth*zHeight*(bitmap_pad/8));
    im_index=zWidth*zHeight*im*(bitmap_pad/8);
    if (bitmap_pad == 8) {
      for (i=0; i<zHeight; i++) {
	for (j=0; j<zWidth; j++) {
	  byte_Image[i*zWidth+j+im_index] = tmp_byte[(zWidth-j-1)*zHeight+i];
	}
      }
    }
    else {
      for (i=0; i<zHeight; i++) {
	for (j=0; j<zWidth; j++) {
	  byte_Image[i*zWidth*4+j*4+im_index]   = 
	    tmp_byte[(zWidth-j-1)*zHeight*4+i*4];
	  byte_Image[i*zWidth*4+j*4+im_index+1] = 
	    tmp_byte[(zWidth-j-1)*zHeight*4+i*4+1];
	  byte_Image[i*zWidth*4+j*4+im_index+2] = 
	    tmp_byte[(zWidth-j-1)*zHeight*4+i*4+2];
	  byte_Image[i*zWidth*4+j*4+im_index+3] = 
	    tmp_byte[(zWidth-j-1)*zHeight*4+i*4+3];
	}
      }
    }
    im_index=Width*Height*im;
    for (i=0; i<Height; i++) {
      for (j=0; j<Width; j++) {
	short_Image[i*Width+j+im_index] = 
	  tmp_short[(Width-j-1)*Height+i];
      }
    }
  }
  if ((num_images>1)&&load_all_images) fprintf(stderr,"                     \r");

  /* Scale Image and Resize if necessary */
  Resize(zWidth,zHeight);
  XResizeWindow(theDisp,mainW,
		zWidth+(color_bar?color_bar_width:0),
		zHeight+info_height);

  /* redraw image and clean up */
  free(tmp_short);
  free(tmp_byte);
  DrawWindow(0,0,zWidth,zHeight);
  update_msgs();

  /* don't bother attempting to keep track of rotations */
  RW_valid = False;

  /* define new cursors */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}

/*----------------------- Reorient_Volume -------------------------------*/
void Reorient_Volume(void *data)
{
  int 	i, old_h, old_w, old_i, num_volumes;
  short 	*volume;
  int         vol_step;

  /* check that we have a volume */
  if ((num_images==1)||(!load_all_images)) {
    fprintf(stderr,"Only one image found!\n");
    return;
  }

  /* define new cursors */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);
  if (Verbose) fprintf(stderr,"Volume reorientation in progress...\n");

  /* perform the reorientation */
  num_volumes = 1;
  for(i=0; i<ndimensions-3; i++){
    num_volumes *= slider_length[i];
  }
  vol_step = Width*Height*slider_length[ndimensions-3];
  for(i=0; i<num_volumes; i++){
    fprintf(stderr,"Reorienting volume: %d\r", i);
    volume = &(short_Image[i*vol_step]);
    _Reorient_Volume(volume, slider_length[ndimensions-3], 
		     Width, Height);
  }

  /* resize the color bar */
  Resize_ColorBar(color_bar_width,slider_length[ndimensions-3]);

  /* swap the width, height, and num_images variables */
  old_h=Height;
  old_w=Width;
  old_i=slider_length[ndimensions-3];
  zHeight=Height=old_i;
  zWidth=Width=old_h;
  slider_length[ndimensions-3]=old_w;
  num_images=old_w*num_volumes;
  theImage->width = theImage->bytes_per_line = zWidth;
  theImage->height = zHeight;

  /* Scale Image and Resize if necessary */
  Scale_Image(P_Min,P_Max);
  image_number=num_images/2;
  Resize(Width,Height);
  XResizeWindow(theDisp,mainW,
		Width+(color_bar?color_bar_width:0),
		Height+info_height);

  /* redraw image and clean up */
  DrawWindow(0,0,Width,Height);
  update_msgs();
  update_sliders();

  /* don't bother attempting to keep track of rotations */
  RW_valid = False;

  /* define new cursors */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}

/*----------------------- _Reorient_Volume -------------------------------*/
void _Reorient_Volume(short volume[], int num_slices, int width, int height)
{
  int 	i, im, im_index, j, 
        old_h, old_w, old_i;
  short *tmp_short;

  /* malloc space */
  tmp_short = (short *) malloc(width*height*num_slices*2);

  /* copy data */
  memcpy(tmp_short,volume,width*height*num_slices*2);

  /* swap the width, height and num_images variables */
  old_h=height;  
  old_w=width;  
  old_i=num_slices;
  height = old_i;
  width  = old_h;
  num_slices=old_w;

  /* perform the reorientation */
  if (num_slices>1) fprintf(stderr,"                               \r");

  for (im=0; im<num_slices; im++) {
    if (num_slices>1) fprintf(stderr,"Sorting image %d\r",im);
    im_index=width*height*im;
    for (i=0; i<height; i++)
      for (j=0; j<width; j++) {
	volume[i*width+j+im_index] = 
	  tmp_short[j*old_w+im+(old_h*old_w*i)];
      }
  }

  if (num_slices>1) fprintf(stderr,"                               \r");

  free(tmp_short);
}

