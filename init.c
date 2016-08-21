/*
 * 	init.c
 *
 *	Miscellaneous initialization stuff.
 *
 *	Initialize()
 *
 *	Copyright (c) Bruce Pike, 1993-1997
 */

#include "xdisp.h"
#include "spectral.h"

/*------------------------ Initialize --------------------------*/
void Initialize(void)
{
  int			i, j;

  /* image dimensions, etc */
  oWidth = Width = oHeight = Height  = 0;
  zero=0; one=1; two=2; three=3; four=4;
  gr_gif=GIF; gr_tiff=TIFF; gr_pict=PICT; gr_sgi=SGI; gr_rast=RAST;
  minc_x_dim = minc_y_dim = minc_z_dim = minc_t_dim = -1;
  RW_valid = True; 
  rw_offset = 0.0;
  rw_scale = 1.0;
  X_reverse = Y_reverse = False;
  file_format = UNKNOWN_FORMAT;
  Input_Data_Type = SHORT_DATA;
  Image_Depth = 2;
  Byte_Offset = 0;
  Byte_Skip = 0;
  Scale_Data  = 0;
  load_all_images = False;
  Swap = 0;
  Interpolation_Type = BILINEAR;
  cropped = False;
  crop_y = crop_x = 0;
  image_increment = 0;
  color_bar = 0;
  color_bar_text_width = COLOR_BAR_TEXT_WIDTH;
  color_bar_scale_width = COLOR_BAR_SCALE_WIDTH;
  color_bar_width = color_bar_scale_width+color_bar_text_width;
  window_name[0] = '\0';
  compressed = FALSE;
  number_of_files = 0;
    
  /* X window stuff */
  cmdW_Initial_State = cmdW_State = IconicState;
  NumColors = NUM_IM_COLORS + NUM_GC_COLORS;
  ColorMapSize = COLOR_MAP_SIZE;
  ColorMapOffset = COLOR_MAP_OFFSET;
  NumGCColors = NUM_GC_COLORS;
  private_cmap = False;
  Preferred_Visual_Class = -1;
  red_shift = 0;
  green_shift = 0;
  blue_shift = 1;
  if (ColorMapOffset+NumColors > COLOR_MAP_SIZE)
    FatalError("ColorMap Size/Offset Conflict");
  grc = "red";

  /* a few handy things */
  Lower = SHRT_MAX;
  Upper = SHRT_MIN;
  rw_Lower = (double) Lower;
  rw_Upper = (double) Upper;
  I_Max = SHRT_MIN;
  I_Min = SHRT_MAX;
  t_fast = 200;
  max_steps = 8192;      
  steps = 1;
  accel = 2;
  wl_display = 0;
  zoom_flag = 0;
  zoom_factor_x = zoom_factor_y = 1.0;
  roi_active = 0;
  roi_present = 0;
  h_profile_active = 0;
  v_profile_active = 0;
  plot_active = -1;
  plot_style = 1;
  plot_roi_size = 0;
  ez_buffer_mode = 1;
  selzoom_active = 0;
  selzoom_factor_x = selzoom_factor_y = 2;
  ox = oy = 0;
  Verbose = VERBOSE;
  read_from_stdin = 0;
  roiceps = 0;
  ext_roi_on = FALSE;

  /* environment variables */
  if (getenv("XDISP_GAMMA")==NULL) 
    Gamma = 1.0;
  else 
    Gamma = atof(getenv("XDISP_GAMMA"));
  if (getenv("XDISP_PS_GAMMA")==NULL) 
    PS_Gamma = 1.0;
  else 
    PS_Gamma = atof(getenv("XDISP_PS_GAMMA"));
  if (getenv("XDISP_COLOR_TABLE")==NULL) 
    color_table = 0;
  else 
    color_table = atoi(getenv("XDISP_COLOR_TABLE"));
  fg=getenv("XDISP_FG");
  bg=getenv("XDISP_BG");
  version = REVDATE;

  /* hot metal color table */
  for (i=0; i<256; i++) {
    if (i<128) {
      hot_metal_table[0][i] = (int)(255.0*(float)i/127.0);
    }
    else {
      hot_metal_table[0][i] = 255;
    }
    if ((i>=64)&&(i<192)) {
      hot_metal_table[1][i] = (int)(255.0*(float)(i-64)/127.0);
    }
    else if (i<64) {
      hot_metal_table[1][i] = 0;
    }
    else {
      hot_metal_table[1][i] = 255;
    }
    if ((i>=128)&&(i<256)) {
      hot_metal_table[2][i] = (int)(255.0*(float)(i-128)/127.0);
    }
    else if (i<128) {
      hot_metal_table[2][i] = 0;
    }
    else {
      hot_metal_table[2][i] = 255;
    }
  }

  /* Spectral color table */
  for(i=0; i<3; i++) {
    for (j=0; j<256; j++) {
      spectral_table[i][j] = (int)(spectral_rgb[i][j]*255.0);
    }
  }

}

