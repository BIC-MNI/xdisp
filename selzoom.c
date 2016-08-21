/*
 * 	selzoom.c
 *
 *	Selective zoom routines.
 *
 *	Selective_Zoom()
 *	Trim()
 *
 *
 * 	Copyright (c) B. Pike, 1993-1997
 */

#include "xdisp.h"
#include <sys/stat.h>


/*-------------------------- Selective_Zoom ---------------------------------*/
void Selective_Zoom(void *data)
{
  int 	w, h, done;

  /* get start/done flag */
  done = *((int *) data);
    
  /* set roi size to 1/2 the current image size and draw an roi box */
  if (!done) {
    if (selzoom_active) {
      draw_roi(roi_x1, roi_y1, roi_x2, roi_y2);
    }
    w = zWidth/selzoom_factor_x;
    h = zHeight/selzoom_factor_y;
    roi_x1 = zWidth/2 - w/2;
    roi_y1 = zHeight/2 - h/2;
    roi_x2 = zWidth/2 + w/2 - 1;
    roi_y2 = zHeight/2 + h/2 - 1;
    draw_roi(roi_x1, roi_y1, roi_x2, roi_y2);
    roi_present = 1;
    selzoom_active = 1;
  }
  else {
    /* define new cursors */
    XDefineCursor(theDisp,mainW,waitCursor);
    XDefineCursor(theDisp,cmdW,waitCursor);
    XFlush(theDisp);

    /* do it */
    Trim();
    w = zWidth*selzoom_factor_x;
    h = zHeight*selzoom_factor_y;
    Resize(w,h);
    XResizeWindow(theDisp,mainW,
		  zWidth+(color_bar?color_bar_width:0),
		  zHeight+info_height);
    Window_Level(Lower,Upper);
    update_msgs();
    update_sliders();

    /* reset some parameters */        
    roi_present = 0;
    selzoom_active = 0;
    selzoom_factor_x = 2;
    selzoom_factor_y = 2;

    /* define new cursors */
    XDefineCursor(theDisp,mainW,mainCursor);
    XDefineCursor(theDisp,cmdW,cmdCursor);
    XFlush(theDisp);
  }
}

/*---------------------------- Trim ------------------------------*/
void Trim(void)
{
  int		i, j, index, im, im_index, x1, x2, y1, y2;

  /* info */
  if (Verbose) fprintf(stderr,"Trim image at (%d,%d) to (%d,%d)\n",
		       roi_x1,roi_y1,roi_x2,roi_y2);

  /* get roi corners in original image coordinates */
  x1 = roi_x1*(Width-1)/(zWidth-1);
  x2 = roi_x2*(Width-1)/(zWidth-1);
  y1 = roi_y1*(Height-1)/(zHeight-1);
  y2 = roi_y2*(Height-1)/(zHeight-1);
  if (x2 < x1) {
    i  = x1;
    x1 = x2;
    x2 = i;
  }
  if (y2 < y1) {
    i  = y1;
    y1 = y2;
    y2 = i;
  }

  /* perform trim in-place */
  for (im=0; im<(load_all_images?num_images:1); im++) {
    if ((num_images>1)&&load_all_images) fprintf(stderr,"Cropping image %d\r",im);
    im_index = Width*Height*im;
    index = abs((x2-x1+1)*(y2-y1+1)*im);
    for (j=y1; j<=y2; j++) {
      for (i=x1; i<=x2; i++, index++) {
	short_Image[index] = short_Image[j*Width+i+im_index];
      }
    }
    im_index = zWidth*zHeight*im*(bitmap_pad/8);
    index = abs((roi_x2-roi_x1+1)*(roi_y2-roi_y1+1)*im*(bitmap_pad/8));
    if (Selected_Visual_Class!=TrueColor) {
      if (bitmap_pad == 8) {
	for (j=roi_y1; j<=roi_y2; j++) {
	  for (i=roi_x1; i<=roi_x2; i++, index++) {
	    byte_Image[index] = byte_Image[j*zWidth+i+im_index];
	  }
	}
      }
      else {
	for (j=roi_y1; j<=roi_y2; j++) {
	  for (i=roi_x1; i<=roi_x2; i++) {
	    byte_Image[index++] = byte_Image[(j*zWidth+i)*4+im_index];
	    byte_Image[index++] = byte_Image[(j*zWidth+i)*4+1+im_index];
	    byte_Image[index++] = byte_Image[(j*zWidth+i)*4+2+im_index];
	    byte_Image[index++] = byte_Image[(j*zWidth+i)*4+3+im_index];
	  }
	}
      }
    }
  }
  if ((num_images>1)&&load_all_images) fprintf(stderr,"                     \r");

  /* define new image dimensions */   
  Resize_ColorBar(color_bar_width,(roi_y2 - roi_y1) + 1);
  Width = (x2-x1+1);
  zWidth = theImage->width = (roi_x2 - roi_x1) + 1;
  theImage->bytes_per_line = ((roi_x2 - roi_x1) + 1) * (bitmap_pad/8);
  Height= (y2-y1+1);
  zHeight = theImage->height = (roi_y2 - roi_y1) + 1;

}

