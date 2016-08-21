/*
 * 	ezwinit.c
 *
 *	Routines to initialize the EZ widgets used in xdisp.
 *
 *      Create_EZ_Widgets()
 *	ReInitialize()
 *
 * 	Copyright Bruce Pike, 1997
 */

#include "xdisp.h"

/*----------------------- Create_EZ_Widgets() --------------------------*/
void Create_EZ_Widgets()
{

  /* initialize all widgets */
  Initialize_File_Widgets();
  Initialize_Info_Widget();
  Initialize_Help_Widget();
  Initialize_Input_Widget();
  Initialize_Plot_Widget(); 
  Initialize_Error_Widget();
  
  
  /* set the default font for all EZ widgets */
  if (font!=NULL) {
    EZ_SetDefaultLabelFont(font);
    EZ_SetDefaultMenuTitleFont(font);
  }
  else {
    EZ_SetDefaultLabelFont(DEFAULT_EZ_FONT);
    EZ_SetDefaultMenuTitleFont(DEFAULT_EZ_FONT);
  }
  
}

/*------------------------ ReInitialize --------------------------*/
void ReInitialize()
{
  /* image dimensions, etc */
  oWidth = Width = oHeight = Height  = 0;
  minc_x_dim = minc_y_dim = minc_z_dim = minc_t_dim = -1;
  file_format = UNKNOWN_FORMAT;
  RW_valid = True;
  rw_offset = 0.0;
  rw_scale = 1.0;
  X_reverse = Y_reverse = False;
  cropped = False;
  crop_y = crop_x = 0;
  image_increment = 0;
    
  /* a other few handy things */
  Lower = SHRT_MAX;
  Upper = SHRT_MIN;
  rw_Lower = (double) Lower;
  rw_Upper = (double) Upper;
  I_Max = SHRT_MIN;
  I_Min = SHRT_MAX;
  zoom_flag = 0;
  zoom_factor_x = zoom_factor_y = 1.0;
  h_profile_active = 0;
  v_profile_active = 0;
  ez_buffer_mode = 1;
  selzoom_active = 0;
  selzoom_factor_x = selzoom_factor_y = 2;
  ox = oy = 0;
  plot_active = -1;
  plot_style = 1;
  plot_roi_size = 0;
}
