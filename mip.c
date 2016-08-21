/*
 * 	mip.c
 *
 *	Maximum Intensity Projection
 *
 *	MIP()
 *
 * 	Copyright (c) B. Pike, April, 1994-1997
 */

#include "xdisp.h"
#include <sys/stat.h>


/*-------------------------- MIP ---------------------------------*/
void MIP(void)
{
  int 	i, im, mip_offset, im_offset;
  short	*tmp_short;

  /* define new cursors */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);

  /* if all images are loaded */
  if (load_all_images) {
    short_Image = realloc(short_Image,Width*Height*(num_images+1)*2);
    byte_Image = realloc(byte_Image,zWidth*zHeight*(num_images+1));

    /* initilize mip image */
    mip_offset = num_images*Width*Height;
    for (i=0; i<Width*Height; i++) short_Image[i+mip_offset] = SHRT_MIN;
    mip_offset = num_images*zWidth*zHeight;
    for (i=0; i<zWidth*zHeight; i++) byte_Image[i+mip_offset] = 0;

    /* do it */
    for (im=0; im<num_images; im++) {
      fprintf(stderr,"Processing image %d\r",im+1);
      image_increment = im - image_number;
      New_Image();
      im_offset = im*Width*Height;
      mip_offset = num_images*Width*Height;
      for (i=0; i<Width*Height; i++) {
	if (short_Image[im_offset+i] > short_Image[mip_offset+i])
	  short_Image[mip_offset+i] = short_Image[im_offset+i];
      }
      im_offset = im*zWidth*zHeight;
      mip_offset = num_images*zWidth*zHeight;
      for (i=0; i<zWidth*zHeight; i++) {
	if (byte_Image[im_offset+i] > byte_Image[mip_offset+i])
	  byte_Image[mip_offset+i] = byte_Image[im_offset+i];
      }
    }
    fprintf(stderr,"                                              \r");

    /* reset some parameters */        
    num_images++;
    image_increment = 1;
    New_Image();
  }

  /* if only one image is loaded */
  else {
    /* create working image */
    tmp_short = (short *) malloc(Width*Height*2);
    for (i=0; i<Width*Height; i++) tmp_short[i] = SHRT_MIN;
    /* do it */
    for (im=0; im<num_images; im++) {
      fprintf(stderr,"Processing image %d\r",im+1);
      image_increment = im - image_number;
      New_Image();
      for (i=0; i<Width*Height; i++) {
	if (short_Image[i] > tmp_short[i]) tmp_short[i] = short_Image[i];
      }
    }
    fprintf(stderr,"                                             \r");

    /* clean up and show results */
    memcpy(short_Image,tmp_short,Width*Height*2);
    free(tmp_short);
    Scale_Image(S_Min,S_Max);
    DrawWindow(0,0,zWidth,zHeight);
  }

  /* fix command window if needed */
  update_msgs();
  update_sliders();

  /* define new cursors */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}
