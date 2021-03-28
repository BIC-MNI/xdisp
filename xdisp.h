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
#include <stdint.h>
/* #include <values.h> */
#include <float.h>
#include <unistd.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <olgx/olgx.h>

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

#include "xwins.h" /*uses ulong */



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
WHERE EZ_Widget     *File_Selector_Widget, *File_Save_Widget, 
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
WHERE int cmd_width, cmd_height, info_height;
WHERE unsigned
              button_height,
              title_height, 
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
/* bilinear.c */
WHERE void bilinear_byte_to_byte (byte *oim, int ox, int oy, 
                                  byte *nim, int nx, int ny);
WHERE void bilinear_short_to_short (short *oim, int ox, int oy, 
                                  short *nim, int nx, int ny);
WHERE void bilinear_int_to_int (int *oim, int ox, int oy, 
                                int *nim, int nx, int ny);
WHERE void bilinear_long_to_long (int32_t *oim, int ox, int oy, 
                                  int32_t *nim, int nx, int ny);
WHERE void bilinear_float_to_float(float *oim, int ox, int oy, 
                                   float *nim, int nx, int ny);
WHERE void bilinear_double_to_double(double *oim, int ox, int oy, 
                                     double *nim, int nx, int ny);
WHERE void bilinear_rgbp_to_rgbp(byte *oim, int ox, int oy, 
                                 byte *nim, int nx, int ny);

/* button.c */
WHERE void CreateMainWindow(char *name, char *geom, int argc, char **argv);
WHERE void CreateCommandWindow(int x, int y,
                               unsigned int width, 
                               unsigned int height, 
                               unsigned int bdwidth, 
                               uint32_t bdcolor, 
                               uint32_t bgcolor,
                               int32_t h_flag,
                               Window parent, char *label);
WHERE void Define_Cmd_Buttons(void);

/* color.c */
WHERE void Switch_Colormap(void *);
WHERE void ColorBar_Scale(int min, int max);
WHERE void Resize_ColorBar(int w, int h);

/* compress.c */
WHERE int File_Is_Compressed(char *);
WHERE int File_Compression_Type(char *);
WHERE char *Decompress_File(char *);

/* defcolor.c */
WHERE void Define_ColorMap(void);
WHERE void Define_ColorBar(void);
WHERE void Toggle_ColorBar(void *);
WHERE void TrueColor_Initialize(void);
WHERE void Toggle_Interpolation(void *);

/* evprocs.c */
WHERE void Quit(void *);
WHERE void DrawWindow(int x, int y, int w, int h);
WHERE void Adjust_Image_Slider(void *);
WHERE void Refresh(void *);
WHERE void Hide_cmdW(void *);

/* exit.c */
WHERE void exit_xdisp(int status);

/* ext_roi.c */
WHERE int draw_ext_roi(int ps_roi_draw, FILE *ps_fp, int ps_xoff, int ps_yoff); 

/* ezwerror.c */
WHERE void Initialize_Error_Widget(void);
WHERE void Open_Error_Widget(void);
WHERE void Close_Error_Widget(void *, void *);
WHERE void EZW_Error(char *);

/* ezwfs.c */
WHERE void Initialize_File_Widgets(void);
WHERE void Get_New_Filename(EZ_Widget *object, void *data);
WHERE void Open_File_Selector_Widget(void *data);
WHERE void Load_New_Image(char *fn);
WHERE void Spawn_New_xdisp( EZ_Widget *widget, void *data );
WHERE char * Get_Save_Filename(char *);
WHERE void Save_File_Select_Done_Callback( EZ_Widget *widget, void *data );
WHERE void Save_File_Select_Cancel_Callback( EZ_Widget *widget, void *data );

/* ezwhelp.c */
WHERE void Initialize_Help_Widget(void);
WHERE void Open_Help_Widget(void *);
WHERE void Close_Help_Widget(void *, void *);
WHERE void EZW_Online_Help(void);

/* ezwinfo.c */
WHERE void Initialize_Info_Widget(void);
WHERE void Open_Info_Widget(void *);
WHERE void Close_Info_Widget(void *, void *);

/* ezwinit.c */
WHERE void Create_EZ_Widgets(void);
WHERE void ReInitialize(void);

/* ezwinput.c */
WHERE void Initialize_Input_Widget(void);
WHERE void Open_Input_Widget(void *);
WHERE void Close_Input_Widget(void);
WHERE void Input_Widget_CallBack(EZ_Widget *widget, void *label);

/* ezwplot.c */
WHERE void Initialize_Plot_Widget(void);
WHERE void Open_Plot_Widget(void *);
WHERE void Close_Plot_Widget(void *, void *);
WHERE void Plot_Profile(EZ_Widget *Plot_Surface);
WHERE void Plot_Type_Switch(EZ_Widget *p_button, void *data);
WHERE void Plot_Style_Switch(EZ_Widget *p_button, void *data);
WHERE void ROI_Size_Switch(EZ_Widget *p_button, void *data);
WHERE void Draw_Plot_Axis(EZ_Widget  *p_canvas,
                          float xl_min, float xl_max, float yl_min, float yl_max,
                          char *xlabel, char *ylabel, char *title);

/* files.c */
WHERE void ps_file(void *);
WHERE void graphics_file(void *);
WHERE void flat_file(void *);
WHERE void short_to_grfx(short *image, char *filename, 
                         int cols, int rows, int zcols, int zrows, 
                         int min, int max, int type);
WHERE void matlab_file(void *);

/* handler.c */
WHERE void HandleEvent(XEvent *);

/* help.c */
WHERE void Syntax(char *);
WHERE void Help(void);
WHERE void online_Help(void);
WHERE void FatalError(char *);

/* image.c */
WHERE void Load_Image(char *fname);
WHERE void Increment_Image(void *);
WHERE void Decrement_Image(void *);
WHERE void New_Image(void);
WHERE void cmi_init(FILE *fp);
WHERE void Load_All(void *);
WHERE void Byte_Swap(byte *byte_data, int length);
WHERE void Reload(void *);

/* initialize.c */
WHERE void Initialize(void);

/* keypress.c */
WHERE void Handle_KeyPress(XEvent *);

/* minc.c */
#include <minc.h>

#ifndef MAX_VAR_DIMS
#define MAX_VAR_DIMS 3
#endif

#ifndef MAX_NC_NAME
#define MAX_NC_NAME 64
#endif

#include <stdlib.h>
#include <string.h>

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

/* mip.c */
void MIP(void);

/* motion.c */
WHERE void Handle_Motion(XEvent *);

/* nneighbour.c */
WHERE void nneighbour_byte_to_byte(byte *oim, int ox, int oy, 
                                   byte *nim, int nx, int ny);
WHERE void nneighbour_short_to_short(short *oim, int ox, int oy, 
                                     short *nim, int nx, int ny);
WHERE void nneighbour_int_to_int(int *oim, int ox, int oy, 
                                 int *nim, int nx, int ny);
WHERE void nneighbour_long_to_long(int32_t *oim, int ox, int oy, 
                                   int32_t *nim, int nx, int ny);
WHERE void nneighbour_float_to_float(float *oim, int ox, int oy, 
                                     float *nim, int nx, int ny);
WHERE void nneighbour_double_to_double(double *oim, int ox, int oy, 
                                       double *nim, int nx, int ny);
WHERE void nneighbour_rgbp_to_rgbp(uint32_t *oim, int ox, int oy, 
                                   uint32_t *nim, int nx, int ny);

/* olinit.c */
WHERE void olgx_init(void);

/* profile.c */
WHERE void H_Profile(void);
WHERE void V_Profile(void);
WHERE void H_Profile_Switch(void *);
WHERE void V_Profile_Switch(void *);

/* read.c */
WHERE void Read_Byte(void);
WHERE void Read_UByte(void);
WHERE void Read_Short(void);
WHERE void Read_UShort(void);
WHERE void Read_Long(void);
WHERE void Read_ULong(void);
WHERE void Read_Float(void);
WHERE void Read_Double(void);

/* reshape.c */
WHERE void Resize(int, int);
WHERE void zoom(void *);
WHERE void Crop(void *);
WHERE void Flip_Image(void *);
WHERE void Rotate_Image(void *);
WHERE void Reorient_Volume(void *);
WHERE void _Reorient_Volume(short volume[], int num_slices, int width, int height);

/* roi.c */
WHERE void image_stats(void *);
WHERE void roi_stats(int, int, int, int, int);
WHERE void draw_roi(int, int, int, int);

/* scale.c */
WHERE void Scale_Image(int min, int max);
WHERE void Rescale(void *);
WHERE void Auto_Scale(void *);
WHERE void Scale_Short_to_Byte(short *s_im, int s_w, int s_h,
                               byte *b_im,  int b_w, int b_h,
                               int min, int max);

/* selzoom.c */
WHERE void Selective_Zoom(void *);
WHERE void Trim(void);

/* setcicon.c */
WHERE void SetCIcon(Display *dsp, Window wnd, int state);

/* setmicon.c */
WHERE void SetMIcon(int state);

/* tic.c */
WHERE void TIC(void *);

/* update.c */
WHERE void
  update_msgs(void),
  update_sliders(void);

/* winlev.c */
WHERE void Window_Level(int lower, int upper);
WHERE void toggle(void *);
WHERE void inc_upper(void *);
WHERE void dec_upper(void *);
WHERE void inc_lower(void *);
WHERE void dec_lower(void *);
WHERE void inc_steps(void *);
WHERE void dec_steps(void *);
WHERE void Adjust_Upper_Slider(void *);
WHERE void Adjust_Lower_Slider(void *);
WHERE void Reset_Window_Level(void *);
WHERE void Show_Window_Level(void);

/* xbutton.c */
WHERE XWIN *MakeXButton(int x, int y,
                        unsigned width, unsigned height, unsigned bdwidth,
                        ulong bdcolor, ulong bgcolor,
                        Window parent, char *label,
                        void (*button_action)(void *),
                        void *action_data,
                        int state);
WHERE XWIN *MakeXSlider(int x, int y,
                        unsigned slider_width, unsigned slider_height,
                        unsigned left_label_width, unsigned right_label_width,
                        unsigned value,
                        ulong bdcolor, ulong bgcolor,
                        Window parent,
                        char *left_label, char *right_label,
                        void (*slider_action)(void *),
                        void *action_data);
WHERE void draw_slider(XWIN *p_xwin, int state);

/* ------------------------- MINC additions ------------------------------*/

/* Function prototypes */

/* global stuff */
WHERE Volume_Info minc_volume_info;
WHERE int	      minc_icvid;

WHERE int     slider_image[MAX_VAR_DIMS-2], slider_length[MAX_VAR_DIMS-2];
WHERE int     image_number, num_images, image_increment, current_dim;
WHERE int     ndimensions, compressed;
WHERE XWIN    *NextButton[MAX_VAR_DIMS-2], 
              *PreviousButton[MAX_VAR_DIMS-2],
              *Image_Slider[MAX_VAR_DIMS-2];
WHERE int     constant[MAX_VAR_DIMS-2];


