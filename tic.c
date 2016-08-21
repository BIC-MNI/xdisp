/*
 * 	tic.c
 *
 *	Time Intensity Plots
 *
 *	TIC()
 *
 * 	Copyright (c) Bruce Pike, 1996-2000
 */

#include "xdisp.h"
#include <sys/stat.h>


/*-------------------------- TIC ---------------------------------*/
void TIC(void *data)
{
  int 	im, org_image; 
  char  tmp_name[40], tmp_cmd[80];
  float rw_loc;
  FILE	*out;    
    
  /* check that an roi is defined */
  if (roi_present != 1) {
    fprintf(stderr,"An ROI must be drawn before calculating its time course.\n");
    return;
  }

  /* open output file */
  tmpnam(tmp_name);
  if(!(out = fopen(tmp_name,"w"))){ 
    fprintf(stderr,"Unable to open temporary file\n");
    return;
  }

  /* define new cursors */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);

  /* save current image position */
  org_image = image_number;
    
  /* cycle through images and dump results to file */
  for (im=0; im<slider_length[current_dim]; im++) {
    fprintf(stderr,"Processing image %d\r",im+1);
    image_increment = im - slider_image[current_dim];
    New_Image();
    draw_roi(roi_x1,roi_y1,roi_x2,roi_y2);
    roi_stats(roi_x1*(Width-1)/(zWidth-1),
	      roi_y1*(Height-1)/(zHeight-1),
	      roi_x2*(Width-1)/(zWidth-1),
	      roi_y2*(Height-1)/(zHeight-1),1);
    if (file_format==MINC_FORMAT && 
	RW_valid &&
	minc_volume_info.step[current_dim]!=0 ) {
      rw_loc = minc_volume_info.start[current_dim] + 
	(float) slider_image[current_dim] * 
	minc_volume_info.step[current_dim];
      fprintf(out,"%f %f %f \n",rw_loc,tic_mean,tic_std);
    }
    else {
      fprintf(out,"%d  %f %f\n",im+1,tic_mean,tic_std);
    }
  }

  /* display original image */
  image_increment = org_image - image_number;
  New_Image();
  draw_roi(roi_x1,roi_y1,roi_x2,roi_y2);
    
  /* print plot commands */
  fprintf(out,"@s0 type xy \n");
  fprintf(out,"@s0 symbol 2 \n");
  fprintf(out,"@s0 symbol fill 1 \n");
  if (file_format==MINC_FORMAT && 
      RW_valid &&
      minc_volume_info.step[current_dim]!=0 ) {
    fprintf(out,"@xaxis label \"%s (%s)\"\n", 
	    minc_volume_info.dimension_names[current_dim],
	    minc_volume_info.dimension_units[current_dim]);
  }
  else if (file_format==MINC_FORMAT) {
    fprintf(out,"@xaxis label \"Frame (%s)\"\n", 
	    minc_volume_info.dimension_names[current_dim]);
  }
  else {
    fprintf(out,"@xaxis label \"Image Number\"\n");
  }
  fprintf(out,"@yaxis label \"ROI Mean\"\n");
  fprintf(out,"@title \"%s\"\n",basfname);
  fprintf(out,"@frame fill on\n");
  fprintf(out,"@frame background color 7\n");

  /* close file */
  fclose(out);
  fprintf(stderr,"                                              \r");

  /* issue shell commands */
  if (GRACE) {
    sprintf(tmp_cmd,"cat %s | xmgrace -pipe &\n",tmp_name); 
  }
  else {
    sprintf(tmp_cmd,"cat %s | xmgr -xy -source stdin &\n",tmp_name); 
  }
  system (tmp_cmd); 
  sprintf(tmp_cmd,"rm %s &\n",tmp_name); 
  system (tmp_cmd); 
    
  /* define new cursors */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}
