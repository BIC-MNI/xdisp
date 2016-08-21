/*
 * 	roi.c
 *
 *	ROI proceesing routines for Xdisp.
 *
 *	image_stats()
 *	roi_stats()
 *	draw_roi()
 *
 * 	Copyright (c) Bruce Pike 1993-1997
 *
 */

#include "xdisp.h"
#include <sys/stat.h>

/*---------------------------- image_stats -----------------------------*/
void image_stats(void *data)
{
  roi_stats(0,0,Width-1,Height-1,0);
}

/*---------------------------- roi_stats -------------------------------*/
void roi_stats(int x1, int y1, int x2, int y2, int Quiet )
{
  double roi_sum, roi_mean, roi_variance,
         rw_roi_min, rw_roi_max, 
         rw_roi_mean, rw_roi_variance, tmp;
  int 	 i, j, index, roi_max, roi_min, roi_count, i_offset;

  /* initialize a few things */
  roi_sum = 0.0;
  roi_variance = 0.0;
  rw_roi_variance = 0.0;
  roi_max = SHRT_MIN;
  roi_min = SHRT_MAX;
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
  roi_count = (x2-x1+1)*(y2-y1+1);
  i_offset = load_all_images ? image_number*Width*Height : 0;
    
  /* first loop through roi data to get mean */
  for (i=x1; i<=x2; i++) {
    for (j=y1; j<=y2; j++) {
      index = j*Width+i + i_offset;
      roi_sum += short_Image[index]; 
      roi_min = (short_Image[index] < roi_min) ?
	short_Image[index] : roi_min;
      roi_max = (short_Image[index] > roi_max) ?
	short_Image[index] : roi_max;
    }
  }

  /* calculate roi_mean */
  roi_mean = (float) roi_sum / roi_count;

  /* perform scale correction if needed */
  if ((file_format==ACR_NEMA_FORMAT)||
      (file_format==MINC_FORMAT) ||
      (Input_Data_Type > SHORT_DATA)) {
    rw_roi_min = (double) (rw_offset + 
			   ((double)(roi_min-I_Min)*rw_scale));
    rw_roi_max = (double) (rw_offset + 
			   ((double)(roi_max-I_Min)*rw_scale));
    rw_roi_mean = (double) (rw_offset + 
			    ((roi_mean-(double)I_Min)*rw_scale));
  }

  /* second loop through roi data to get variance */
  for (i=x1; i<=x2; i++) {
    for (j=y1; j<=y2; j++) {
      roi_variance += ((short_Image[j*Width+i+i_offset] - roi_mean) *
		       (short_Image[j*Width+i+i_offset] - roi_mean)) /
	(roi_count-1); 
      if ((file_format==ACR_NEMA_FORMAT)||
	  (file_format==MINC_FORMAT) ||
	  (Input_Data_Type > SHORT_DATA)) {
	tmp = (double)(rw_offset + 
		       ((double)(short_Image[j*Width+i+i_offset]-I_Min)*rw_scale));
	rw_roi_variance += ((tmp - rw_roi_mean) *
			    (tmp - rw_roi_mean)) /
	  (roi_count-1); 
      }
    }
  }

  /* set return values */
  if ((file_format==ACR_NEMA_FORMAT)||
      (file_format==MINC_FORMAT) ||
      (Input_Data_Type > SHORT_DATA)) {
    tic_mean = rw_roi_mean;
    if (roi_count<2)
      tic_std = 0;
    else
      tic_std = sqrt(rw_roi_variance);
  }
  else {
    tic_mean = roi_mean;
    if (roi_count<2) 
      tic_std = 0;
    else
      tic_std = sqrt(roi_variance);
  }

  if (roi_count>2 && !Quiet) {
    /* display the results */
    if (num_images>1)
      fprintf(stderr,"\nROI data from source image %d:\n",image_number);
    else
      fprintf(stderr,"\nROI data from source image:\n");
    fprintf(stderr,"------------------------------\n");
    fprintf(stderr,"upper left   = %d, %d\n", x1, y1);
    fprintf(stderr,"lower right  = %d, %d\n", x2, y2);
    fprintf(stderr,"pixel count  = %d\n", roi_count);
    if ((file_format==ACR_NEMA_FORMAT) ||
	(file_format==MINC_FORMAT) ||
	(Input_Data_Type >= FLOAT_DATA)) {
      fprintf(stderr,"roi min      = %f\n", rw_roi_min);
      fprintf(stderr,"roi max      = %f\n", rw_roi_max);
      fprintf(stderr,"roi mean     = %f\n", rw_roi_mean);
      fprintf(stderr,"roi variance = %f\n\n", rw_roi_variance);
    }
    else if (Input_Data_Type==USHORT_DATA || Input_Data_Type==ULONG_DATA) {
      fprintf(stderr,"roi min      = %lu\n", (ulong) floor(rw_roi_min+0.5));
      fprintf(stderr,"roi max      = %lu\n", (ulong) floor(rw_roi_max+0.5));
      fprintf(stderr,"roi mean     = %f\n", rw_roi_mean);
      fprintf(stderr,"roi variance = %f\n\n", rw_roi_variance);
    }
    else if (Input_Data_Type==LONG_DATA) {
      fprintf(stderr,"roi min      = %ld\n", (long) floor(rw_roi_min+0.5));
      fprintf(stderr,"roi max      = %ld\n", (long) floor(rw_roi_max+0.5));
      fprintf(stderr,"roi mean     = %f\n", rw_roi_mean);
      fprintf(stderr,"roi variance = %f\n\n", rw_roi_variance);
    }
    else {
      fprintf(stderr,"roi min      = %d\n", roi_min);
      fprintf(stderr,"roi max      = %d\n", roi_max);
      fprintf(stderr,"roi mean     = %f\n", roi_mean);
      fprintf(stderr,"roi variance = %f\n\n", roi_variance);
    }
  }
}

/*----------------------- draw_roi -----------------------------*/
void draw_roi(int x1, int y1, int x2, int y2)
{
  int		i;

  /* check corners */    
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

  /* draw the roi */    
  XDrawRectangle(theDisp, mainW, roiGC, x1, y1, x2-x1+1, y2-y1+1);
  roi_present = 1;
}    
