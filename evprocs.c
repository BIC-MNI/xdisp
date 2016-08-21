/*
 * 	evproc.c
 *
 *	Misc event proceesing routines for Xdisp.
 *
 *	Quit()
 *	DrawWindow()
 *	Adjust_Image_Slider()
 *      Refresh()
 *      Hide_cmdW()
 *
 * 	Copyright (c) Bruce Pike 1993-1997
 *
 */

#include "xdisp.h"
#include <sys/stat.h>

/*-------------------------------- Quit ----------------------------*/
void Quit(void *data)
{
  if (Selected_Visual_Class==DirectColor) {
    XStoreColors(theDisp,NewCmap,rootC,ColorMapSize); 
    XFlush(theDisp);
  }
  exit_xdisp(0);
}

/*---------------------------- DrawWindow --------------------------*/
void DrawWindow(int x, int y, int w, int h)
{
  char	 msg1[16], msg2[16], msg3[16], msg4[16], msg5[16];
  double fp_min, fp_max, tmv[5], tmr;
  int    i;

  /* draw the image */    
  XPutImage(theDisp,mainW,theMGC,theImage,x,y,x,y,w,h);
  roi_present = 0;

  /* draw the rois */    
  if ( ext_roi_on ) { 
    ext_roi_active  = draw_ext_roi(0,NULL,0,0);
  }
  else {
    ext_roi_active = FALSE;
  }

  /* draw the color bar */    
  if (color_bar) {
    XPutImage(theDisp,mainW,theMGC,theColorBar,0,0,
	      zWidth,0,
	      color_bar_width,color_bar_height);
    if (file_format==MINC_FORMAT || 
	file_format==ACR_NEMA_FORMAT ||
	Input_Data_Type >= FLOAT_DATA) {
      fp_min = rw_offset + (double)(P_Min-I_Min)*rw_scale;
      fp_max = rw_offset + (double)(P_Max-I_Min)*rw_scale;

      /* calculate the tick mark values */
      tmv[0] = fp_max;
      tmv[1] = fp_min+(fp_max-fp_min)*0.75;
      tmv[2] = fp_min+(fp_max-fp_min)*0.50;
      tmv[3] = fp_min+(fp_max-fp_min)*0.25;
      tmv[4] = fp_min;
      
      /* chechk for any anoying values that are `almost' zero */
      tmr = fabs(fp_min-fp_max);
      for (i=0; i<5; i++) {
	if (fabs(tmv[i])<(tmr*0.001)) {
	  tmv[i]=0.0;
	}
      }

      /* create the tick mark text */
      sprintf(msg1,"%8.3g",tmv[0]);
      sprintf(msg2,"%8.3g",tmv[1]);
      sprintf(msg3,"%8.3g",tmv[2]);
      sprintf(msg4,"%8.3g",tmv[3]);
      sprintf(msg5,"%8.3g",tmv[4]);

      /* display the tick mark text */
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, Ascent_of_TextFont(ol_ginfo),
		       msg1, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight/4 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg2, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight/2 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg3, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight*3/4 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg4, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight,
		       msg5, 8);
    }
    else if (Input_Data_Type==ULONG_DATA || Input_Data_Type==USHORT_DATA) {
      fp_min = rw_offset + (double)(P_Min-I_Min)*rw_scale;
      fp_max = rw_offset + (double)(P_Max-I_Min)*rw_scale;
      sprintf(msg1,"%8lu",(ulong) floor(fp_max+0.5));
      sprintf(msg2,"%8lu",(ulong) floor((fp_min+(fp_max-fp_min)*0.75)+0.5));
      sprintf(msg3,"%8lu",(ulong) floor((fp_min+(fp_max-fp_min)*0.50)+0.5));
      sprintf(msg4,"%8lu",(ulong) floor((fp_min+(fp_max-fp_min)*0.25)+0.5));
      sprintf(msg5,"%8lu",(ulong) floor(fp_min+0.5));
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, Ascent_of_TextFont(ol_ginfo),
		       msg1, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight/4 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg2, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight/2 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg3, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight*3/4 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg4, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight,
		       msg5, 8);
    }
    else if (Input_Data_Type==LONG_DATA) {
      fp_min = rw_offset + (double)(P_Min-I_Min)*rw_scale;
      fp_max = rw_offset + (double)(P_Max-I_Min)*rw_scale;
      sprintf(msg1,"%8ld",(long) floor(fp_max+0.5));
      sprintf(msg2,"%8ld",(long) floor((fp_min+(fp_max-fp_min)*0.75)+0.5));
      sprintf(msg3,"%8ld",(long) floor((fp_min+(fp_max-fp_min)*0.50)+0.5));
      sprintf(msg4,"%8ld",(long) floor((fp_min+(fp_max-fp_min)*0.25)+0.5));
      sprintf(msg5,"%8ld",(long) floor(fp_min+0.5));
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, Ascent_of_TextFont(ol_ginfo),
		       msg1, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight/4 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg2, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight/2 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg3, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight*3/4 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg4, 8);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight,
		       msg5, 8);
    }
    else {
      sprintf(msg1,"%6d",P_Max);
      sprintf(msg2,"%6d",P_Min + (int)((float)(P_Max-P_Min)*0.75));
      sprintf(msg3,"%6d",P_Min + (int)((float)(P_Max-P_Min)*0.50));
      sprintf(msg4,"%6d",P_Min + (int)((float)(P_Max-P_Min)*0.25));
      sprintf(msg5,"%6d",P_Min);    
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, Ascent_of_TextFont(ol_ginfo),
		       msg1, 6);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight/4 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg2, 6);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight/2 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg3, 6);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight*3/4 + Ascent_of_TextFont(ol_ginfo)/2,
		       msg4, 6);
      XDrawImageString(theDisp, mainW, theMGC,
		       zWidth+2, zHeight,
		       msg5, 6);
    }
  }

  /* draw the actual tick marks */
  XDrawLine(theDisp, mainW, theMGC, zWidth+color_bar_text_width-5, 0,
	    zWidth+color_bar_text_width, 0);
  XDrawLine(theDisp, mainW, theMGC, zWidth+color_bar_text_width-5, zHeight/4,
	    zWidth+color_bar_text_width, zHeight/4);
  XDrawLine(theDisp, mainW, theMGC, zWidth+color_bar_text_width-5, zHeight/2,
	    zWidth+color_bar_text_width, zHeight/2);
  XDrawLine(theDisp, mainW, theMGC, zWidth+color_bar_text_width-5, zHeight*3/4,
	    zWidth+color_bar_text_width, zHeight*3/4);
  XDrawLine(theDisp, mainW, theMGC, zWidth+color_bar_text_width-5, zHeight-1,
	    zWidth+color_bar_text_width, zHeight-1);
}


/*-------------------------- Adjust_Image_Slider ------------------*/
void Adjust_Image_Slider(void * data)
{
  int		x, cw;
  int           idim = *(int *)data;
  D_SLIDER 	*p_data=(D_SLIDER *) Image_Slider[idim]->data;

  /* find cursor position on slider */
  x = theEvent.xmotion.x - p_data->left_label_width;
  cw = HorizSliderControl_Width(ol_ginfo)/2;

  /* update position value and Upper if within slider limits */
  x = (x<cw) ? cw : x;
  x = (x>(p_data->slider_width-cw)) ? p_data->slider_width-cw : x;
  p_data->old_value = p_data->new_value;
  p_data->new_value = x-cw;
  image_increment = (int)(((slider_length[idim]-1.0) * 
			   ((float)(x-cw)/(p_data->slider_width-cw*2)))+0.5)
    - slider_image[idim];
  current_dim = idim;
  update_sliders();
  New_Image();

  /* update label */
  sprintf(p_data->left_label,"%s:%-3d  1",
	  (file_format == MINC_FORMAT ? minc_volume_info.dimension_names[idim] :
	   "Image"), slider_image[idim]+1);
  XDrawImageString(theDisp, Image_Slider[idim]->xid, theCGC,
		   p_data->left_label_width-
		   XTextWidth(ol_text_finfo,p_data->left_label,
			      strlen(p_data->left_label)), 
		   p_data->y + Ascent_of_TextFont(ol_ginfo),
		   p_data->left_label, strlen(p_data->left_label));
}


/*-------------------------- Refresh ----------------------------*/
void Refresh(void *data)
{
  DrawWindow(0,0,zWidth,zHeight);
}

/*------------------------ Hide_cmdW ----------------------------*/
void Hide_cmdW(void *data)
{
  XUnmapWindow(theDisp,cmdW);
  cmdW_State = IconicState;
}

