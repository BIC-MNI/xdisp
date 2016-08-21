/*
 * 	update.c
 *	
 *      Xdisp routines for updating messages and sliders.
 *	
 *	update_msgs()
 *      update_sliders()
 *	
 *
 *	Copyrght (c) Bruce Pike, 1993-2000
 */

#include "xdisp.h"

/*--------------------------- update_msgs ---------------------------*/
void update_msgs(void)
{
  D_SLIDER	*ud = (D_SLIDER *) Upper_Slider->data;
  D_SLIDER	*ld = (D_SLIDER *) Lower_Slider->data;
  char	        upper_msg[32], lower_msg[32];
  int           nsliders, idim, y_pos, y_skip;
 
  /* title message */
  sprintf(title_msg,"Filename: %s, Min=%d, Max=%d, size=%dx%d",
	  basfname, P_Min, P_Max, zWidth, zHeight); 

  /* title info */
  olgx_draw_box(ol_ginfo,cmdW,1,1,cmd_width-1,title_height,OLGX_NORMAL,1);
  XDrawImageString(theDisp,cmdW,theCGC,
		   (cmd_width-XTextWidth(ol_text_finfo,title_msg,strlen(title_msg)))/2,
		   button_height-4,title_msg,strlen(title_msg));

  /* y position and skip between buttons */
  y_skip = (int)(button_height*1/2);
  y_pos =  (int)(title_height + (button_height+y_skip)*4 + button_height);

  /* color map labels */
  if (file_format==MINC_FORMAT || 
      file_format==ACR_NEMA_FORMAT ||
      Input_Data_Type >= FLOAT_DATA) {
    sprintf(upper_msg,"Upper= %-8.3g ",
	    (double)(rw_offset + ((double)(Upper-I_Min)*rw_scale)) );
    sprintf(lower_msg,"Lower= %-8.3g ",
	    (double)(rw_offset + ((double)(Lower-I_Min)*rw_scale)) );        
  }
  else if (Input_Data_Type==USHORT_DATA || Input_Data_Type==ULONG_DATA) {
    sprintf(upper_msg,"Upper= %-8lu ",
	    (ulong)floor((rw_offset + ((double)(Upper-I_Min)*rw_scale))+0.5) );
    sprintf(lower_msg,"Lower= %-8lu ",
	    (ulong)floor((rw_offset + ((double)(Lower-I_Min)*rw_scale))+0.5) );        
  }
  else if (Input_Data_Type==LONG_DATA) {
    sprintf(upper_msg,"Upper= %-8ld ",
	    (long)floor((rw_offset + ((double)(Upper-I_Min)*rw_scale))+0.5) );
    sprintf(lower_msg,"Lower= %-8ld ",
	    (long)floor((rw_offset + ((double)(Lower-I_Min)*rw_scale))+0.5) );        
  }
  else {
    sprintf(upper_msg,"Upper= %-7d ",Upper);
    sprintf(lower_msg,"Lower= %-7d ",Lower);
  }
  olgx_draw_text_ledge(ol_ginfo, cmdW, 1, y_pos, cmd_width-2);
  olgx_draw_text(ol_it_ginfo, cmdW, "Color Map", 4, 
		 y_pos, cmd_width, OLGX_NORMAL);

  if (file_format==MINC_FORMAT || 
      file_format==ACR_NEMA_FORMAT ||
      Input_Data_Type > SHORT_DATA) {
    XClearArea(theDisp, cmdW, 12, ud->y,
	       XTextWidth(ol_text_finfo,"Upper= 888888888",16),
	       Ascent_of_TextFont(ol_ginfo), False);
    XDrawImageString(theDisp, cmdW, theCGC,
		     12, ud->y + Ascent_of_TextFont(ol_ginfo),
		     upper_msg, 16);
    XClearArea(theDisp, cmdW, 12, ld->y,
	       XTextWidth(ol_text_finfo,"Lower= 888888888",16),
	       Ascent_of_TextFont(ol_ginfo), False);
    XDrawImageString(theDisp, cmdW, theCGC,
		     12, ld->y + Ascent_of_TextFont(ol_ginfo),
		     lower_msg, 16);
  }
  else {
    XClearArea(theDisp, cmdW, 12, ud->y,
	       XTextWidth(ol_text_finfo,"Upper= -32767",13),
	       Ascent_of_TextFont(ol_ginfo), False);
    XDrawImageString(theDisp, cmdW, theCGC,
		     12, ud->y + Ascent_of_TextFont(ol_ginfo),
		     upper_msg, 16);
    XClearArea(theDisp, cmdW, 12, ld->y,
	       XTextWidth(ol_text_finfo,"Lower= -32767",13),
	       Ascent_of_TextFont(ol_ginfo), False);
    XDrawImageString(theDisp, cmdW, theCGC,
		     12, ld->y + Ascent_of_TextFont(ol_ginfo),
		     lower_msg, 16);
  }


  /* input file label */
  y_pos += button_height*3;
  olgx_draw_text_ledge(ol_ginfo, cmdW, 1, y_pos, cmd_width-2);
  olgx_draw_text(ol_it_ginfo, cmdW, "Input", 4, y_pos,
		 cmd_width, OLGX_NORMAL);

  /* output file label */
  y_pos += button_height*3 + (button_height+y_skip);
  olgx_draw_text_ledge(ol_ginfo, cmdW, 1, y_pos, cmd_width-2);
  olgx_draw_text(ol_it_ginfo, cmdW, "Output", 4, y_pos,
		 cmd_width, OLGX_NORMAL);

  /* image label */
  for(idim=0,nsliders=0; idim<ndimensions-2; idim++){
    if (slider_length[idim] > 1) nsliders++;
  }
  y_pos += button_height*3 + (button_height+y_skip)*(2+nsliders-1);
  olgx_draw_text_ledge(ol_ginfo, cmdW, 1, y_pos, cmd_width-2);
  olgx_draw_text(ol_it_ginfo, cmdW, "Image", 4, y_pos,
		 cmd_width, OLGX_NORMAL);
}


/*--------------------------- update_sliders ---------------------------*/
void update_sliders(void)
{
  D_SLIDER	*upper_data, *lower_data;
  int   	x, cw;
  float	d;
  D_SLIDER    *image_data[MAX_VAR_DIMS-2];
  int         idim;
    
  /* set pointer to slider data */
  upper_data = Upper_Slider->data;
  lower_data = Lower_Slider->data;
  for(idim=0; idim<ndimensions-2; idim++){
    if (slider_length[idim] > 1){
      image_data[idim] = Image_Slider[idim]->data;
    }
  }

  /* calculate handy constants */
  cw = HorizSliderControl_Width(ol_ginfo);
  d = (float)(S_Max-S_Min);

  /* check slider ranges */
  if (Lower<=Upper) {
    if (Lower<S_Min) S_Min=Lower;
    if (Upper>S_Max) S_Max=Upper;
  }
  else {
    if (Upper<S_Min) S_Min=Upper;
    if (Lower>S_Max) S_Max=Lower;
  }
    
  /* define slider labels */
  if (file_format==MINC_FORMAT ||
      file_format==ACR_NEMA_FORMAT ||
      Input_Data_Type >= FLOAT_DATA) {
    sprintf(upper_data->left_label,"%8.3g",
	    (double)(rw_offset + ((double)(S_Min-I_Min)*rw_scale)) );
    sprintf(upper_data->right_label,"%-8.3g",
	    (double)(rw_offset + ((double)(S_Max-I_Min)*rw_scale)) );        
    sprintf(lower_data->left_label,"%8.3g",
	    (double)(rw_offset + ((double)(S_Min-I_Min)*rw_scale)) );
    sprintf(lower_data->right_label,"%-8.3g",
	    (double)(rw_offset + ((double)(S_Max-I_Min)*rw_scale)) );        
  }
  else if (Input_Data_Type==USHORT_DATA || Input_Data_Type==ULONG_DATA) {
    sprintf(upper_data->left_label,"%8lu",
	    (ulong)floor((rw_offset + ((double)(S_Min-I_Min)*rw_scale))+0.5) );
    sprintf(upper_data->right_label,"%-8lu",
	    (ulong)floor((rw_offset + ((double)(S_Max-I_Min)*rw_scale))+0.5) );        
    sprintf(lower_data->left_label,"%8lu",
	    (ulong)floor((rw_offset + ((double)(S_Min-I_Min)*rw_scale))+0.5) );
    sprintf(lower_data->right_label,"%-8lu",
	    (ulong)floor((rw_offset + ((double)(S_Max-I_Min)*rw_scale))+0.5) );        
  }    
  else if (Input_Data_Type==LONG_DATA) {
    sprintf(upper_data->left_label,"%8ld",
	    (long)floor((rw_offset + ((double)(S_Min-I_Min)*rw_scale))+0.5) );
    sprintf(upper_data->right_label,"%-8ld",
	    (long)floor((rw_offset + ((double)(S_Max-I_Min)*rw_scale))+0.5) );        
    sprintf(lower_data->left_label,"%8ld",
	    (long)floor((rw_offset + ((double)(S_Min-I_Min)*rw_scale))+0.5) );
    sprintf(lower_data->right_label,"%-8ld",
	    (long)floor((rw_offset + ((double)(S_Max-I_Min)*rw_scale))+0.5) );        
  }    
  else {
    sprintf(upper_data->left_label,"%7d",S_Min);
    sprintf(upper_data->right_label,"%-7d",S_Max);
    sprintf(lower_data->left_label,"%7d",S_Min);
    sprintf(lower_data->right_label,"%-7d",S_Max);
  }
  for(idim=0; idim<ndimensions-2; idim++){
    if (slider_length[idim] > 1){
      sprintf(image_data[idim]->left_label,"%7s:%-3d  1",
	      (file_format == MINC_FORMAT ? 
	       minc_volume_info.dimension_names[idim] : "Image"),
	      slider_image[idim]+1);
      sprintf(image_data[idim]->right_label,"%-3d",
	      slider_length[idim]);
    }
  }

  /* reset slider positions */
  upper_data->old_value = upper_data->new_value;
  x = (int)(((float)(Upper-S_Min)/d)*(float)(upper_data->slider_width-cw));
  if (x>(upper_data->slider_width-cw)) 
    upper_data->new_value = upper_data->slider_width-cw;
  else if (x<0) 
    upper_data->new_value = 0;
  else 
    upper_data->new_value = x;

  lower_data->old_value = lower_data->new_value;
  x = (int)(((float)(Lower-S_Min)/d)*(float)(lower_data->slider_width-cw));
  if (x>(lower_data->slider_width-cw)) 
    lower_data->new_value = lower_data->slider_width-cw;
  else if (x<0) 
    lower_data->new_value = 0;
  else 
    lower_data->new_value = x;

  for(idim=0; idim<ndimensions-2; idim++){
    if (slider_length[idim] > 1){
      image_data[idim]->old_value = image_data[idim]->new_value;
      x = (int)( ((float)(slider_image[idim])/(slider_length[idim]-1))
                 * (float)(image_data[idim]->slider_width-cw) );
      if (x>(image_data[idim]->slider_width-cw)) 
	image_data[idim]->new_value = image_data[idim]->slider_width-cw;
      else if (x<0) 
	image_data[idim]->new_value = 0;
      else 
	image_data[idim]->new_value = x;

    }
  }

  /* redraw the sliders */
  draw_slider(Upper_Slider, OLGX_HORIZONTAL | OLGX_UPDATE);
  draw_slider(Lower_Slider, OLGX_HORIZONTAL | OLGX_UPDATE);
  for (idim=0; idim<ndimensions-2; idim++){
    if (slider_length[idim] > 1){
      draw_slider(Image_Slider[idim], OLGX_HORIZONTAL | OLGX_UPDATE);
    }
  }
}

