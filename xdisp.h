/*
 *  xdisp.h  -  header file for xdisp
 *
 *
 *  Copyright (c) B Pike, 1993-2003
 */

#define REVDATE   "v4.3.55, May 20, 2003"
#define MAXZOOM 16
#define NUM_IM_COLORS 230
#define NUM_GC_COLORS 6
#define DEFAULT_SIZE 256
#define COLOR_MAP_OFFSET 20
#define COLOR_MAP_SIZE 256
#define COLOR_BAR_SCALE_WIDTH 20
#define COLOR_BAR_TEXT_WIDTH 20
#define PRINTER_PORT "LPT1"
#define ICON_WIDTH 64
#define ICON_HEIGHT 64
#define SIGNA_4X_SIZE 145408
#define SIGNA_4X_HEADER_SIZE 14336
#define SIGNA_5X_MAGIC 1229801286
#define VERBOSE False
#define NEAREST_NEIGHBOUR 0
#define BILINEAR 1
#define UNKNOWN_FORMAT 0
#define RAW_FORMAT 0
#define SIGNA_5X_FORMAT 1 
#define SIGNA_4X_FORMAT 2
#define ACR_NEMA_FORMAT 3
#define MINC_FORMAT 4
#define CMI_FORMAT 5
#define BYTE_DATA 1
#define UBYTE_DATA 2
#define SHORT_DATA 3
#define USHORT_DATA 4
#define LONG_DATA 5
#define ULONG_DATA 6
#define FLOAT_DATA 7
#define DOUBLE_DATA 8
#define GIF 1
#define TIFF 2
#define PICT 3
#define SGI 4
#define RAST 5
#define DEFAULT_FONT "-b&h-lucida-medium-r-normal-sans-10-100-75-75-p-58-iso8859-1"
#define DEFAULT_ITALIC_FONT "-b&h-lucida-medium-i-normal-sans-10-100-75-75-p-58-iso8859-1"
#define DEFAULT_BOLD_FONT "-b&h-lucida-bold-r-normal-sans-*-100-*-*-*-*-*-*"
#define DEFAULT_EZ_FONT "-adobe-helvetica-bold-r-normal--12-120-75-75-p-70-iso8859-1"
#define FALLBACK_FONT "variable"
#define GLYPH_FONT "-sun-open look glyph-*-*-*-*-*-100-*-*-*-*-*-*"
#define GLYPH_IT_FONT "-sun-open look glyph-*-*-*-*-*-100-*-*-*-*-*-*"
#define MINMAX(x,xmin,xmax) (x > xmax ? xmax : (x < xmin ? xmin : x))
#define MAX_NUM_FILES 128
#define GRACE 1

/* include files */
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>
/* #include <values.h> */
#include <float.h>
#include <unistd.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <olgx/olgx.h>
#include "xwins.h"

/* EZ Widget compile flag */
#include <EZ.h>

/* location stuff */
#ifndef MAIN
#define WHERE extern
#else
#define WHERE
#endif

/* type stuff */
#ifndef ulong
#define ulong unsigned long
#endif
typedef unsigned char byte;
typedef int boolean;

/* string manipulation stuff */
#define CENTERX(f,x,str) ((x)-XTextWidth(f,str,strlen(str))/2)
#define CENTERY(f,y) ((y)-((f->ascent+f->descent)/2)+f->ascent)

/* X stuff */
WHERE Display       *theDisp;
WHERE int           theScreen, theDepth, dispcells, bitmap_pad;
WHERE Colormap      rootCmap;
WHERE Colormap      NewCmap;
WHERE Window        rootW, mainW, cmdW, iconW;
WHERE GC            theCGC, theMGC, roiGC, wGC, bGC, grGC;
WHERE ulong	    fcol, bcol, gcol, wp, bp;
WHERE Visual        *theVisual;
WHERE XImage        *theImage, *expImage, *iconImage, *theColorBar;
WHERE XEvent        theEvent;
WHERE XWIN	    *which_xwin;
WHERE Cursor	    mainCursor, cmdCursor, wlCursor, waitCursor, roiCursor;
WHERE XColor	    cur_fg, cur_bg;
WHERE Pixmap 	    cross_hr_bmap, cross_hrmsk_bmap;
WHERE XGCValues	    xgcv;

/* olgx stuff */
WHERE Graphics_info *ol_ginfo, *ol_it_ginfo;
WHERE XFontStruct   *ol_text_finfo, *ol_glyph_finfo,
                    *ol_it_text_finfo, *ol_it_glyph_finfo;
WHERE ulong	    ol_pixvals[5];
WHERE XColor	    ol_fg, ol_bg1, ol_bg2, ol_bg3, ol_white;

/* EZ Widget stuff */
EZ_Widget           *File_Selector_Widget, *File_Save_Widget, 
                    *Info_Widget, *Mincheader_Widget, *Info_Close_Button,
                    *Help_Widget, *Help_Text_Widget, *Help_Close_Button,
                    *Error_Widget, *Error_Text_Widget, *Error_Close_Button,
                    *Input_Widget, *Input_Text_Widget, *Input_Widget_Label,
                    *Plot_Widget, *Plot_Canvas_Widget, *Plot_Close_Button,
                    *T_Plot_Button, *Raw_Plot_Button, *Normalized_Plot_Button,
                    *Percent_Plot_Button, *Plot_Button[4], *ROI_Size_Button[3];

/* global vars */
WHERE int     NumColors, ColorMapSize, ColorMapOffset, NumGCColors,
              Preferred_Visual_Class, Selected_Visual_Class,
              Width, Height, oWidth, oHeight, zWidth, zHeight,
              I_Min, I_Max, Delta, P_Min, P_Max, S_Min, S_Max,
              roi_active, roi_present, roi_x1, roi_x2, roi_y1, roi_y2,
	      Lower, Upper, inc, max_steps, steps, acc_flag, tmp,
	      oLower, oUpper, Input_Data_Type, Image_Depth, 
              Byte_Skip, Byte_Offset, zoom_flag,
              accel, ox, oy, Scale_Data, cmdW_State, cmdW_Initial_State,
              cmi_type, cmi_fov, cmi_slth, onum_images, 
              cmi_orient, cmi_x, cmi_y, cmi_bits, cmi_stereo, cmi_shift,
              cmi_sub, cmi_offset, wl_display,
	      color_bar, color_bar_width, color_bar_height,
	      color_bar_text_width, color_bar_scale_width,
              color_table, hot_metal_table[3][256], spectral_table[3][256],
              selzoom_factor_x, selzoom_factor_y, selzoom_active, 
              read_from_stdin, Swap, Interpolation_Type,
              file_format, h_profile_active, v_profile_active,
              profile_x, profile_y, crop_x, crop_y, Save_Select_Done,
              minc_x_dim, minc_y_dim, minc_z_dim, minc_t_dim, 
              ez_buffer_mode, plot_active, plot_style, plot_roi_size,
              number_of_files;

WHERE float   I_Mean, I_Std, zoom_factor_x, zoom_factor_y, Gamma, PS_Gamma,
              tic_mean, tic_std;
WHERE double  rw_fp_min, rw_fp_max, rw_Lower, rw_Upper, rw_scale, rw_offset;
WHERE XColor  rootC[COLOR_MAP_SIZE], newC[COLOR_MAP_SIZE];
WHERE char    *cmd, *fg, *bg, *grc, *font;
WHERE char    **xd_argv;
WHERE int     xd_argc;
WHERE char    fullfname[256], basfname[256], ps_fname[256], 
              original_fname[256], window_name[256],
              cmi_patient[32], cmi_comment[80], id_field[12],
              title_msg[128], pix_msg[128],  loc_msg[64], wl_msg[64];
WHERE byte    *byte_Image, *icon_byte_image, *byte_ColorBar, Gamma_Table[256];
WHERE short   *short_Image;
WHERE boolean private_cmap, load_all_images, cropped,  
              RW_valid, Y_reverse, X_reverse;
WHERE ulong   plane_mask, color_cells[COLOR_MAP_SIZE], 
              red_shift, green_shift, blue_shift;

/* Buttons & Sliders */
WHERE XWIN    *QuitButton, *PrintButton, *Inc_upperButton,
    	      *Dec_upperButton, *Inc_lowerButton, *Dec_lowerButton,
       	      *Inc_stepsButton, *Dec_stepsButton, *ZoomButton,
      	      *PSFileButton, *RescaleButton, *ResetButton, 
              *Upper_Slider, *Lower_Slider, *InvertButton,
              *Auto_ScaleButton, *RefreshButton, *EPSFileButton,
              *TIFFFileButton, *GIFFileButton, *SGIFileButton,
              *PICTFileButton, *ByteFileButton, *RASTFileButton,
              *ShortFileButton, *FlipXButton, *FlipYButton,
              *ZoomUpButton, *ZoomDownButton, *StatsButton,
              *RotateButton, *CropButton, *MatlabFileButton,
              *HelpButton, *ReloadButton, *ShowWLButton,
              *HotMetalButton, *SelectiveZoomButton, *TICButton,
              *Next10Button, *Previous10Button, *ReorientVolumeButton,
              *LoadAllButton, *NewFileButton, *FileInfoButton, 
              *NewXdispButton, *UpperButton, *LowerButton,
              *ColorMapButton, *ColorBarButton, *GammaButton,
              *HProfileButton, *VProfileButton, *HideButton,
              *InterpolationButton, *PlotButton;

WHERE Time    t_last_click, t_fast;
WHERE unsigned
              button_height,
              cmd_width, cmd_height,
              info_height, title_height, 
              next_button_x, next_button_y;
WHERE int     zero, one, two, three, four, gr_gif, gr_tiff, gr_sgi, gr_rast, gr_pict;
WHERE char    *fname, *version, *cmd_line_fnames[MAX_NUM_FILES];
WHERE boolean Verbose;

WHERE char    *roifname;
WHERE char    *roic;
WHERE ulong   rcol;
WHERE GC      tGC;
WHERE int     roiceps;
WHERE boolean ext_roi_on, ext_roi_active;

/* prototypes */
WHERE int    CreateMainWindow(), CreateCmdWindow(), Define_Cmd_Buttons(),
             update_msgs(), update_sliders(), Define_ColorMap(), Define_ColorBar(),
             Toggle_ColorBar(), TrueColor_Initialize(), Quit(), DrawWindow(),
             Resize(), Window_Level(), toggle(), zoom(), inc_upper(), dec_upper(),
             inc_lower(), dec_lower(), inc_steps(), dec_steps(), Toggle_Interpolation(),
             Adjust_Upper_Slider(), Adjust_Lower_Slider(), Adjust_Image_Slider(),
             Reload(), image_stats(), roi_stats(), draw_roi(), Rescale(), Crop(),
             Flip_Image(), Rotate_Image(), Reorient_Volume(), Auto_Scale(),
             Refresh(), Reset_Window_Level(), Switch_Colormap(),
             Show_Window_Level(), H_Profile(), V_Profile(), Load_Image(),
             Scale_Image(), HandleEvent(), FatalError(), Syntax(), Help(),
             ColorBar_Scale(), graphics_file(), ps_file(), draw_button(),
             Byte_Swap(), SetCIcon(), SetMIcon(), online_Help(), cmi_init(),
             Initialize(), Resize_ColorBar(), short_to_grfx(), ol_init(),
             matlab_file(), flat_file(), bilinear_byte_to_byte(),
             bilinear_short_to_short(), bilinear_int_to_int(),
             bilinear_long_to_long(), bilinear_float_to_float(),
             bilinear_double_to_double(), bilinear_rgbp_to_rgbp(),
             nneighbout_byte_to_byte(), nneighbour_short_to_short(),
             nneighbout_int_to_int(), nneighbout_long_to_long(),
             nneighbour_float_to_float(), nneighbour_double_to_double(),
             nneighbour_rgbp_to_rgbp(), Selective_Zoom(), Trim(), New_Image(),
             Increment_Image(), Decrement_Image(), MIP(), TIC(), Load_All(),
             Scale_Short_to_Byte(), File_Is_Compressed(), File_Compression_Type(),
             H_Profile_Swicth(), V_Profile_Swicth(), Hide_cmdW(), Plot_Profile(),
              _Reorient_Volume(short *, int, int, int);
WHERE void   Handle_KeyPress(), Handle_Motion();
WHERE char   *Decompress_File();
WHERE void   exit_xdisp(int);

WHERE void    Create_EZ_Widgets(), Save_File_Select_Done_Callback(),
              Save_File_Select_Cancel_Callback(), Get_New_Filename(), 
              Load_New_Image(), Spawn_New_xdisp(), Input_Widget_CallBack(),
              ReInitialize(), Close_Info_Widget(), Close_Help_Widget(),
              Close_Input_Widget(), Get_Input_Text(),
              Initialize_File_Widgets(), Initialize_Info_Widget(),
              Initialize_Help_Widget(), Initialize_Input_Widget(),
              Initialize_Plot_Widget(), Close_Plot_Widget(),
              Plot_Type_Switch(), Plot_Style_Switch(), Initialize_Error_Widget(),
              ROI_Size_Switch(), Close_Error_Widget();
WHERE char    *Get_Save_Filename();
WHERE int     Open_File_Selector_Widget(), Open_Info_Widget(), 
              Open_Help_Widget(), EZW_Online_Help(), Open_Input_Widget(),
              Open_Plot_Widget(), Draw_Plot_Axis(), EZW_Error();

/* ------------------------- MINC additions ------------------------------*/
#define MAX_VAR_DIMS 3
#define MAX_NC_NAME 64

#include <stdlib.h>
#include <string.h>
#include <minc.h>

/* Include the standard minc definitions for cross-platform compilation
   if we are compiling the package, otherwise, just define MALLOC,
   FREE, EXIT_SUCCESS and EXIT_FAILURE */
#define MALLOC(size) ((void *) malloc(size))
#define FREE(ptr) free(ptr)
#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 0
#  define EXIT_FAILURE 1
#endif

/* Constants */
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

/* Default ncopts values for error handling */
#define NC_OPTS_VAL NC_VERBOSE | NC_FATAL

/* Types */
typedef struct {
  int    number_of_dimensions;          /* num of dimensions in volume */
  int    axes[MAX_VAR_DIMS];            /* world to volume index transf. */
  long   length[MAX_VAR_DIMS];          /* size of volume */
  double maximum;                       /* Volume maximum */
  double minimum;                       /* Volume minimum */
  double valid_maximum;                 /* Valid volume maximum */
  double valid_minimum;                 /* Valid volume minimum */
  double step[MAX_VAR_DIMS];            /* Step sizes for dimensions */
  double start[MAX_VAR_DIMS];           /* Start positions for dimensions */
  double a_start;                       /* Start position for A dimension */
  double a_step;                        /* Step size for A dimension */
  double b_start;                       /* Start position for B dimension */
  double b_step;                        /* Step sizefor B dimension */
  char   dimension_names[MAX_VAR_DIMS][MAX_NC_NAME]; /* Dimension names */
  double direction_cosines[MAX_VAR_DIMS][3];         /* Direction cosines */
  char   dimension_units[MAX_VAR_DIMS][MAX_NC_NAME]; /* Dimension units */
} Volume_Info;

/* Function prototypes */
int  get_volume_info(char *infile, Volume_Info *volume_info);
void setup_input_icv(int icvid);
void get_dimension_info(char *infile, int icvid, 
                               Volume_Info *volume_info);
void close_volume(int icvid);
void get_volume_slice(int icvid, Volume_Info *volume_info, 
                             int slice_num[], short *image);
int  save_volume_info(int input_icvid, char *outfile, char *arg_string, 
			     Volume_Info *volume_info);
void setup_output_icv(int icvid);
void setup_variables(int inmincid, int mincid, 
                            Volume_Info *volume_info, 
                            char *arg_string);
void setup_image_variables(int inmincid, int mincid, 
                                  int ndims, int dim[]);
void update_history(int mincid, char *arg_string);
void save_volume_slice(int icvid, Volume_Info *volume_info, 
                              int slice_num[], unsigned char *image,
                              double slice_min, double slice_max);
void read_volume_data(int icvid, Volume_Info *volume_info, short *volume);

/* global stuff */
Volume_Info   minc_volume_info;
int	      minc_icvid;

WHERE int     slider_image[MAX_VAR_DIMS-2], slider_length[MAX_VAR_DIMS-2];
WHERE int     image_number, num_images, image_increment, current_dim;
WHERE int     ndimensions, compressed;
WHERE XWIN    *NextButton[MAX_VAR_DIMS-2], 
              *PreviousButton[MAX_VAR_DIMS-2],
              *Image_Slider[MAX_VAR_DIMS-2];
WHERE int     constant[MAX_VAR_DIMS-2];


