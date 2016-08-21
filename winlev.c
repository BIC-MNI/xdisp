/*
 * 	winlev.c
 *
 *	Window and level related event proceesing routines for Xdisp.
 *
 *	Window_Level()
 *	toggle()
 *	inc_upper()
 *	dec_upper()
 *	inc_lower()
 *	dec_lower()
 *	inc_steps()
 *	dec_steps()
 *	Adjust_Upper_Slider()
 *	Adjust_Lower_Slider()
 *      Reset_Window_Level()
 *	Show_Window_Level()
 *
 * 	Copyright (c) Bruce Pike 1993-2000
 *
 */

#include "xdisp.h"
#include <sys/stat.h>

/*--------------------------- Window_Level -------------------------*/
void Window_Level(int lower, int upper)
{
  float 	fdp, fdc, fwin, fmin, fnc, org_pix;
  register int 	grey, i, index;
  byte          *tmp_byte;

  /* if TrueColor rescale image */    
  if (Selected_Visual_Class == TrueColor) {
    tmp_byte = (byte *) malloc(zWidth*zHeight);
    Scale_Short_to_Byte(&short_Image[Width*Height*
                        (load_all_images?image_number:0)], Width, Height,
			tmp_byte, zWidth, zHeight, lower, upper);
    index = 0;
    if (color_table == 0) { 		/* greyscale */
      for (i = zWidth*zHeight*4*(load_all_images?image_number:0); 
	   i < zWidth*zHeight*4*(load_all_images?image_number+1:1); 
	   i+=4) {
	byte_Image[i] = tmp_byte[index];
	byte_Image[i+1] = tmp_byte[index];
	byte_Image[i+2] = tmp_byte[index];
	byte_Image[i+3] = tmp_byte[index];
	index++;
      }
    }
    else if (color_table == 1) { 	/* hot metal */
      for (i = zWidth*zHeight*4*(load_all_images?image_number:0); 
	   i < zWidth*zHeight*4*(load_all_images?image_number+1:1); 
	   i+=4) {
        if (XBitmapBitOrder(theDisp)==MSBFirst) {
	  byte_Image[i+3] =   hot_metal_table[2][(int)tmp_byte[index]];
	  byte_Image[i+2] = hot_metal_table[1][(int)tmp_byte[index]];
	  byte_Image[i+1] = hot_metal_table[0][(int)tmp_byte[index]];
	  byte_Image[i+0] = tmp_byte[index];
	  index++;
        }
        else {
	  byte_Image[i] =   hot_metal_table[2][(int)tmp_byte[index]];
	  byte_Image[i+1] = hot_metal_table[1][(int)tmp_byte[index]];
	  byte_Image[i+2] = hot_metal_table[0][(int)tmp_byte[index]];
	  byte_Image[i+3] = tmp_byte[index];
	  index++;
        }
      }
    }
    else { 				/* spectral */
      for (i = zWidth*zHeight*4*(load_all_images?image_number:0); 
	   i < zWidth*zHeight*4*(load_all_images?image_number+1:1); 
	   i+=4) {
        if (XBitmapBitOrder(theDisp)==MSBFirst) {
  	  byte_Image[i+3] = spectral_table[2][(int)tmp_byte[index]];
	  byte_Image[i+2] = spectral_table[1][(int)tmp_byte[index]];
	  byte_Image[i+1] = spectral_table[0][(int)tmp_byte[index]];
	  byte_Image[i+0] = tmp_byte[index];
	  index++;
	}
	else {
	  byte_Image[i+0] = spectral_table[2][(int)tmp_byte[index]];
	  byte_Image[i+1] = spectral_table[1][(int)tmp_byte[index]];
	  byte_Image[i+2] = spectral_table[0][(int)tmp_byte[index]];
	  byte_Image[i+3] = tmp_byte[index];
	  index++;
	}
      }
    }
    free(tmp_byte);
    /* scale the color bar */
    ColorBar_Scale(lower,upper); 
    /* redraw the window */
    DrawWindow(0,0,zWidth,zHeight);
    /* clear extra motion events */
    while(XCheckTypedEvent(theDisp,MotionNotify,&theEvent));
  }

  /* if PseudoColor or DirectColor... calculate colormap stuff */
  else {
    /* Calculate a few constants */
    fwin = (float)(P_Max-P_Min);
    fmin = (float)P_Min;
    fnc = (float)NumColors-NumGCColors;
    fdp = fwin/(fnc-1.0);
    fdc = (float)(ColorMapSize-1.0)/(float)(upper-lower);
    /* make changes to the colormap */
    for (i=0; i<NumColors-NumGCColors; i++) {
      org_pix = fmin + (i*fdp);
      if (org_pix <= ((lower>upper)?upper:lower))
	grey = (lower<upper) ? (0) : (255<<8);
      else if (org_pix >= ((upper>lower)?upper:lower))
	grey = (lower<upper) ? (255<<8) : (0);
      else {
	grey = ((int)((org_pix-lower)*fdc));
	if (Gamma != 1.0) {
	  grey = pow(((float)grey)/(ColorMapSize-1.0), (1.0/Gamma))
	       * (float)(ColorMapSize-1.0) + 0.5;	
	  grey = (grey<=(ColorMapSize-1)) ? grey : (ColorMapSize-1);
	}
	grey = grey<<8;
      }
      index = i+ColorMapOffset;
      newC[index].pixel = index*red_shift + index*green_shift + index*blue_shift;
      if (color_table==0) {
	newC[index].red = grey;
	newC[index].green = grey;
	newC[index].blue = grey;
      }
      else if (color_table==1) {
	newC[index].red = hot_metal_table[0][grey>>8]<<8;
	newC[index].green = hot_metal_table[1][grey>>8]<<8;
	newC[index].blue = hot_metal_table[2][grey>>8]<<8;
      }
      else {
	newC[index].red = spectral_table[0][grey>>8]<<8;
	newC[index].green = spectral_table[1][grey>>8]<<8;
	newC[index].blue = spectral_table[2][grey>>8]<<8;
      }
      newC[index].flags = DoRed | DoGreen | DoBlue;
    }

    /* Store colors */
    if (Selected_Visual_Class != TrueColor) {
      if (private_cmap) { 
	XStoreColors(theDisp,NewCmap,newC,ColorMapSize); 
      }
      else { 
	XStoreColors(theDisp,NewCmap,&newC[ColorMapOffset],
		     NumColors-NumGCColors);
      }
    }
  }
}

/*-------------------------- toggle --------------------------------*/
void toggle(void *data)
{
  tmp=Lower; Lower=Upper; Upper=tmp;
  Window_Level(Lower,Upper);
  update_sliders();
  update_msgs();
  if (Verbose) fprintf(stderr,"Inverting greyscale...\n");
}


/*-------------------------- inc_upper ----------------------------*/
void inc_upper(void * data)
{
  if ( (theEvent.xbutton.time-t_last_click <= t_fast) &&
       (acc_flag==1) )
    inc = (inc+steps<steps*accel) ? inc+steps: steps*accel;
  else {
    inc=steps;
    acc_flag=1;
  }
  t_last_click = theEvent.xbutton.time;
  Upper = (Upper+inc<SHRT_MAX) ? Upper+inc: SHRT_MAX;
  Window_Level(Lower,Upper);
}

/*-------------------------- dec_upper ----------------------------*/
void dec_upper(void * data)
{
  if ( (theEvent.xbutton.time-t_last_click <= t_fast) &&
       (acc_flag==2) )
    inc = (inc+steps<steps*accel) ? inc+steps: steps*accel;
  else {
    inc=steps;
    acc_flag=2;
  }
  t_last_click = theEvent.xbutton.time;
  Upper = (Upper-inc>SHRT_MIN) ? Upper-inc: SHRT_MIN;
  Window_Level(Lower,Upper);
}

/*-------------------------- inc_lower ----------------------------*/
void inc_lower(void * data)
{
  if ( (theEvent.xbutton.time-t_last_click <= t_fast) &&
       (acc_flag==3) ) 
    inc = (inc+steps<steps*accel) ? inc+steps: steps*accel;
  else {
    inc=steps;
    acc_flag=3;
  }
  t_last_click = theEvent.xbutton.time;
  Lower = (Lower+inc<SHRT_MAX) ? Lower+inc: SHRT_MAX;
  Window_Level(Lower,Upper);
}

/*-------------------------- dec_lower ----------------------------*/
void dec_lower(void * data)
{
  if ( (theEvent.xbutton.time-t_last_click <= t_fast) &&
       (acc_flag==4) )
    inc = (inc+steps<steps*accel) ? inc+steps: steps*accel;
  else {
    inc=steps;
    acc_flag=4;
  }
  t_last_click = theEvent.xbutton.time;
  Lower = (Lower-inc>SHRT_MIN) ? Lower-inc: SHRT_MIN;
  Window_Level(Lower,Upper);
}

/*-------------------------- inc_steps ----------------------------*/
void inc_steps(void * data)
{
  steps = (steps+1<max_steps) ? steps+1: max_steps;
}

/*-------------------------- dec_steps ----------------------------*/
void dec_steps(void * data)
{
  steps = (steps-1<1) ? 1: steps-1;
}

/*-------------------------- Adjust_Upper_Slider ------------------*/
void Adjust_Upper_Slider(void *data)
{
  int		x, cw;
  D_SLIDER 	*p_data=(D_SLIDER *) Upper_Slider->data;
  char	        msg[32];
    
  /* find cursor position on slider */
  x = theEvent.xmotion.x - p_data->left_label_width;
  cw = HorizSliderControl_Width(ol_ginfo)/2;

  /* update position value and Upper if within slider limits */
  x = (x<cw) ? cw : x;
  x = (x>(p_data->slider_width-cw)) ? p_data->slider_width-cw : x;
  p_data->old_value = p_data->new_value;
  p_data->new_value = x-cw;
  Upper = S_Min + (S_Max-S_Min)*((float)(x-cw)/(p_data->slider_width-cw*2));
  Window_Level(Lower, Upper);

  /* update label */
  if (file_format==MINC_FORMAT || 
      file_format==ACR_NEMA_FORMAT ||
      Input_Data_Type >= FLOAT_DATA) {
    sprintf(msg,"Upper= %-8.3g ",
	    (double)(rw_offset + ((double)(Upper-I_Min)*rw_scale)) );
  }
  else if (Input_Data_Type==ULONG_DATA || Input_Data_Type==USHORT_DATA) {
    sprintf(msg,"Upper= %-8lu ",
	    (ulong)floor((rw_offset + ((double)(Upper-I_Min)*rw_scale))+0.5) );
  }
  else if (Input_Data_Type==LONG_DATA) {
    sprintf(msg,"Upper= %-8ld ",
	    (long)floor((rw_offset + ((double)(Upper-I_Min)*rw_scale))+0.5) );
  }
  else {
    sprintf(msg,"Upper= %-7d ",Upper);
  }
  XDrawImageString(theDisp, cmdW, theCGC,
		   12, p_data->y + Ascent_of_TextFont(ol_ginfo),
		   msg, 16);
}

/*-------------------------- Adjust_Lower_Slider ------------------*/
void Adjust_Lower_Slider(void *data)
{
  int		x, cw;
  D_SLIDER 	*p_data=(D_SLIDER *) Lower_Slider->data;
  char	        msg[32];

  /* find cursor position on slider */
  x = theEvent.xmotion.x - p_data->left_label_width;
  cw = HorizSliderControl_Width(ol_ginfo)/2;

  /* update position and Lower value if within slider limits */
  x = (x<cw) ? cw : x;
  x = (x>(p_data->slider_width-cw)) ? p_data->slider_width-cw : x;
  p_data->old_value = p_data->new_value;
  p_data->new_value = x-cw;
  Lower = S_Min + (S_Max-S_Min)*((float)(x-cw)/(p_data->slider_width-cw*2));
  Window_Level(Lower, Upper);

  /* update label */
  if (file_format==MINC_FORMAT || 
      file_format==ACR_NEMA_FORMAT ||
      Input_Data_Type >= FLOAT_DATA) {
    sprintf(msg,"Lower= %-8.3g ",
	    (double)(rw_offset + ((double)(Lower-I_Min)*rw_scale)) );
  }
  else if (Input_Data_Type==ULONG_DATA || Input_Data_Type==USHORT_DATA) {
    sprintf(msg,"Lower= %-8lu ",
	    (ulong)floor((rw_offset + ((double)(Lower-I_Min)*rw_scale))+0.5) );
  }
  else if (Input_Data_Type==LONG_DATA) {
    sprintf(msg,"Lower= %-8ld ",
	    (long)floor((rw_offset + ((double)(Lower-I_Min)*rw_scale))+0.5) );
  }
  else {
    sprintf(msg,"Lower= %-7d ",Lower);
  }
  XDrawImageString(theDisp, cmdW, theCGC,
		   12, p_data->y + Ascent_of_TextFont(ol_ginfo),
		   msg, 16);
}

/*------------------ Reset_Window_Level -----------------*/
void Reset_Window_Level(void *data)
{
  Lower = S_Min = I_Min;
  Upper = S_Max = I_Max;
  Rescale(NULL);
}


/*------------------ Show_Window_Level -----------------*/
void Show_Window_Level(void)
{
  if (wl_display) {
    wl_display = 0;
    XClearArea(theDisp,mainW,0,zHeight,
	       zWidth+(color_bar?color_bar_width:0),
	       zHeight+info_height,False);
    XDrawImageString(theDisp,mainW,theMGC,4,
		     zHeight+info_height-4,loc_msg,strlen(loc_msg));
  }
  else {
    wl_display = 1;
    XClearArea(theDisp,mainW,0,zHeight,
	       zWidth+(color_bar?color_bar_width:0),
	       zHeight+info_height,False);
    XDrawImageString(theDisp,mainW,theMGC,
		     zWidth-XTextWidth(ol_text_finfo,wl_msg,strlen(wl_msg))-4,
		     zHeight+info_height-4,wl_msg,strlen(wl_msg));
    XDrawImageString(theDisp,mainW,theMGC,4,
		     zHeight+info_height-4,loc_msg,strlen(loc_msg));
  }
}
