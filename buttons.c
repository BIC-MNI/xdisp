/*
 *      buttons.c
 *      
 *      Xdisp routines for the main & command window setup
 *      and usage.
 *      
 *      CreateMainWindow()
 *      CreateCommandWindow()
 *      Def_Cmd_Buttons()
 *
 *      Copyrght (c) Bruce Pike, 1993-2000
 */

#include "xdisp.h"
 
/*------------------------ CreateMainWindow -----------------------*/
void CreateMainWindow(char *name, char *geom, int argc, char **argv)
{
  XSetWindowAttributes  xswa;
  unsigned int          xswamask;
  XSizeHints            *hints;
  int                   i, x, y;
  unsigned int          w, h;

  /* parse the geometry command line option */
  x=y=w=h=1;
  i=XParseGeometry(geom,&x,&y,&w,&h);
  if (i&WidthValue)  zWidth = w - (color_bar ? color_bar_width : 0);
  if (i&HeightValue) zHeight = h;
  hints = XAllocSizeHints();
  /* let the user select position if desired */
  if (i&XValue || i&YValue) {
    hints->flags = USPosition;
  }
  else {
    hints->flags = 0;
  }

  /* check specified window position */
  if (i&XValue && i&XNegative) 
    x = XDisplayWidth(theDisp,theScreen)-zWidth-abs(x);
  if (i&YValue && i&YNegative) 
    y = XDisplayHeight(theDisp,theScreen)-zHeight-abs(y);

  /* set up hints */
  hints->x = x;
  hints->y = y;
  hints->width = zWidth + (color_bar ? color_bar_width : 0);
  hints->height = zHeight+info_height;
  hints->max_width = DisplayWidth(theDisp,theScreen);
  hints->max_height = DisplayHeight(theDisp,theScreen);
  hints->flags |= PSize | PMaxSize;

  /* set attributes */
  xswa.colormap = NewCmap; 
  xswa.background_pixel = newC[bcol].pixel;
  xswa.border_pixel     = newC[fcol].pixel;
  xswa.backing_store = Always;
  xswamask =  CWColormap | CWBackPixel | CWBorderPixel | CWBackingStore;

  /* create the main image window */
  mainW = XCreateWindow(theDisp,rootW,x,y, 
                        zWidth+(color_bar ? color_bar_width : 0),
                        zHeight+info_height,
                        2,theDepth,CopyFromParent,
                        theVisual, xswamask, &xswa);
  if (!mainW) FatalError("Can't open main display");
  if (window_name[0]=='\0') strcpy(window_name,basfname); 

  /* set the main windows properties */
  XSetStandardProperties(theDisp,mainW,window_name,window_name,None,
                         0,0,hints);
  XFree(hints);
}


/*--------------------- CreateCommandWindow ---------------------------*/
void CreateCommandWindow(int x, int y,
                         unsigned int width, 
                         unsigned int height, 
                         unsigned int bdwidth, 
                         uint32_t bdcolor, 
                         uint32_t bgcolor,
                         int32_t h_flag,
                         Window parent, char *label)
{
  XSetWindowAttributes xswa;
  unsigned int         xswamask;        
  XSizeHints           *hints;    

  hints = XAllocSizeHints();
  /* Set hints */
  hints->x = x;
  hints->y = y;
  hints->width = (int)width;
  hints->height = (int)height;
  hints->max_width = DisplayWidth(theDisp,theScreen);
  hints->max_height = DisplayHeight(theDisp,theScreen);
  hints->flags =  h_flag | PSize | PMaxSize ;

  /* set attributes */
  xswa.colormap = NewCmap;
  xswa.background_pixel = newC[bcol].pixel;
  xswa.border_pixel     = newC[fcol].pixel;
  /*xswa.backing_store = Always; */
  xswamask = CWColormap | CWBackPixel | CWBorderPixel /*| CWBackingStore*/;

  /* Create the window */
  cmdW=XCreateWindow(theDisp,parent,x,y,width,height,
                     bdwidth,theDepth,CopyFromParent,theVisual,
                     xswamask,&xswa);

  if (!cmdW) FatalError("Can't open command window");
  /* set the main windows properties */
  XSetStandardProperties(theDisp,cmdW,label,label,None,
                         0,0,hints);
  XFree(hints);
}

/*--------------------------- Def_Cmd_Buttons ---------------------*/
void Define_Cmd_Buttons(void)
{
  int   label_w, extra_w, skip_y, skip_x, ec_w;
  char  ll[32], rl[32];
  int   idim, text_w;
    
  /* create the upper and lower sliders */
  if (file_format==MINC_FORMAT ||
      file_format==ACR_NEMA_FORMAT || 
      Input_Data_Type >= FLOAT_DATA) {
    sprintf(ll,"%8.3g",
            (double)(rw_offset + ((double)(P_Min-I_Min)*rw_scale)) );
    sprintf(rl,"%-8.3g",
            (double)(rw_offset + ((double)(P_Max-I_Min)*rw_scale)) );        
    label_w = XTextWidth(ol_text_finfo,"8888888888",10);
    extra_w = XTextWidth(ol_text_finfo,"Upper=8888888888 ",16)+12;
  }
  else if (Input_Data_Type==USHORT_DATA || Input_Data_Type==ULONG_DATA) {
    sprintf(ll,"%8lu",
            (ulong)floor((rw_offset + ((double)(P_Min-I_Min)*rw_scale))+0.5) );
    sprintf(rl,"%-8lu",
            (ulong)floor((rw_offset + ((double)(P_Max-I_Min)*rw_scale))+0.5) );        
    label_w = XTextWidth(ol_text_finfo,"8888888888",10);
    extra_w = XTextWidth(ol_text_finfo,"Upper=8888888888 ",16)+12;
  }
  else if (Input_Data_Type==LONG_DATA) {
    sprintf(ll,"%8ld",
            (long)floor((rw_offset + ((double)(P_Min-I_Min)*rw_scale))+0.5) );
    sprintf(rl,"%-8ld",
            (long)floor((rw_offset + ((double)(P_Max-I_Min)*rw_scale))+0.5) );        
    label_w = XTextWidth(ol_text_finfo,"8888888888",10);
    extra_w = XTextWidth(ol_text_finfo,"Upper=8888888888 ",16)+12;
  }
  else {
    sprintf(ll,"%7d",P_Min);
    sprintf(rl,"%-7d",P_Max);
    label_w = XTextWidth(ol_text_finfo,"-32767*",7);
    extra_w = XTextWidth(ol_text_finfo,"Upper=-32762 ",13)+12;
  }
  skip_y = (int)(button_height*3/2);
  ec_w = ButtonEndcap_Width(ol_ginfo)*2;
  next_button_y = title_height+button_height/2;

  Upper_Slider = MakeXSlider(extra_w, next_button_y,
                             cmd_width-label_w*2-4-extra_w, button_height,
                             label_w, label_w,
                             cmd_width-label_w*2-extra_w-4-HorizSliderControl_Width(ol_ginfo),
                             ol_fg.pixel, ol_bg1.pixel, cmdW,
                             ll, rl,
                             Adjust_Upper_Slider, NULL);

  next_button_y += skip_y;
  Lower_Slider = MakeXSlider(extra_w, next_button_y,
                             cmd_width-label_w*2-extra_w-4, button_height,
                             label_w,label_w, 0,
                             ol_fg.pixel, ol_bg1.pixel, cmdW,
                             ll, rl,
                             Adjust_Lower_Slider, NULL);


  /* create the color map buttons */
  next_button_x = 2;
  next_button_y += skip_y;

  skip_x = XTextWidth(ol_text_finfo,"Auto Scale",10)+ec_w;
  Auto_ScaleButton = MakeXButton(next_button_x, next_button_y,
                                 skip_x, button_height,
                                 1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                                 "Auto Scale", Auto_Scale, NULL, True);

  next_button_x += skip_x + 6;                                
  skip_x = XTextWidth(ol_text_finfo,"Reset W/L",9)+ec_w;
  ResetButton = MakeXButton(next_button_x, next_button_y,
                            skip_x, button_height,
                            1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                            "Reset W/L", Reset_Window_Level, NULL, True);


  next_button_x += skip_x + 6;                                
  skip_x = XTextWidth(ol_text_finfo,"Invert",6)+ec_w;
  InvertButton = MakeXButton(next_button_x, next_button_y,
                             skip_x, button_height,
                             1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                             "Invert", toggle, NULL, True);

  next_button_x += skip_x + 6;                                
  skip_x = XTextWidth(ol_text_finfo,"Rescale",7)+ec_w;
  RescaleButton = MakeXButton(next_button_x, next_button_y,
                              skip_x, button_height,
                              1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                              "Rescale", Rescale, NULL, True);
                                   
  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Gamma",5)+ec_w;
  LowerButton = MakeXButton(next_button_x, next_button_y,
                            skip_x, button_height,
                            1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                            "Gamma", Open_Input_Widget, "Gamma:", True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Lower",5)+ec_w;
  LowerButton = MakeXButton(next_button_x, next_button_y,
                            skip_x, button_height,
                            1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                            "Lower", Open_Input_Widget, "Lower:", True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Upper",5)+ec_w;
  UpperButton = MakeXButton(next_button_x, next_button_y,
                            skip_x, button_height,
                            1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                            "Upper", Open_Input_Widget, "Upper:", True);

  /* new line of buttons */
  next_button_x = 2;
  next_button_y += skip_y;
  skip_x = XTextWidth(ol_text_finfo,"Color Map",9)+ec_w;
  ColorMapButton = MakeXButton(next_button_x, next_button_y,
                             skip_x, button_height,
                             1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                             "Color Map", Switch_Colormap,
                             NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Color Bar",9)+ec_w;
  ColorBarButton = MakeXButton(next_button_x, next_button_y,
                             skip_x, button_height,
                             1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                             "Color Bar", Toggle_ColorBar,
                             NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Interpolation",13)+ec_w;
  InterpolationButton = MakeXButton(next_button_x, next_button_y,
                             skip_x, button_height,
                             1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                             "Interpolation", Toggle_Interpolation,
                             NULL, True);


  /* create the input buttons */
  next_button_y += skip_y*2;

  next_button_x = 2;
  skip_x = XTextWidth(ol_text_finfo,"New File",9)+ec_w;        
  NewFileButton = MakeXButton(next_button_x, next_button_y,
                              skip_x, button_height,
                              1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                              "New File", Open_File_Selector_Widget, 
                              (void *)&zero, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"New xdisp",9)+ec_w;        
  NewFileButton = MakeXButton(next_button_x, next_button_y,
                              skip_x, button_height,
                              1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                              "New xdisp", Open_File_Selector_Widget, 
                              (void *)&one, True);
  next_button_x += skip_x + 6;

  skip_x = XTextWidth(ol_text_finfo,"Load All",8)+ec_w;        
  LoadAllButton = MakeXButton(next_button_x, next_button_y,
                              skip_x, button_height,
                              1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                              "Load All", Load_All, NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Refresh",7)+ec_w;
  RefreshButton = MakeXButton(next_button_x, next_button_y,
                              skip_x, button_height,
                              1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                              "Refresh", Refresh, NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Reload",6)+ec_w;        
  ReloadButton = MakeXButton(next_button_x, next_button_y,
                             skip_x, button_height,
                             1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                             "Reload", Reload, NULL, True);

  /* create the output buttons */
  next_button_x = 2;
  next_button_y += skip_y*2;
  skip_x = XTextWidth(ol_text_finfo,"Print",5)+ec_w;    
  PrintButton = MakeXButton(next_button_x,next_button_y,
                            skip_x,button_height,
                            1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                            "Print",ps_file,(caddr_t)&zero, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"PS File",7)+ec_w;
  PSFileButton = MakeXButton(next_button_x, next_button_y,
                             skip_x, button_height,
                             1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                             "PS File", ps_file, (caddr_t)&one, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"EPS File",8)+ec_w;
  EPSFileButton = MakeXButton(next_button_x, next_button_y,
                              skip_x, button_height,
                              1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                              "EPS File", ps_file, (caddr_t)&two, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Byte File",9)+ec_w;
  ByteFileButton = MakeXButton(next_button_x, next_button_y,
                               skip_x, button_height,
                               1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                               "Byte File", flat_file, (caddr_t)&zero, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Short File",10)+ec_w;
  ShortFileButton = MakeXButton(next_button_x, next_button_y,
                                skip_x, button_height,
                                1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                                "Short File", flat_file, (caddr_t)&one, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Matlab File",11)+ec_w;
  MatlabFileButton = MakeXButton(next_button_x, next_button_y,
                                 skip_x, button_height,
                                 1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                                 "Matlab File", matlab_file, NULL, True);
  /* new row */
  next_button_x = 2;
  next_button_y += skip_y;
  skip_x = XTextWidth(ol_text_finfo,"GIF File",8)+ec_w;
  GIFFileButton = MakeXButton(next_button_x, next_button_y,
                              skip_x, button_height,
                              1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                              "GIF File", graphics_file, (caddr_t)&gr_gif, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"TIFF File",9)+ec_w;
  TIFFFileButton = MakeXButton(next_button_x, next_button_y,
                               skip_x, button_height,
                               1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                               "TIFF File", graphics_file, (caddr_t)&gr_tiff, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"PICT File",9)+ec_w;
  PICTFileButton = MakeXButton(next_button_x, next_button_y,
                               skip_x, button_height,
                               1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                               "PICT File", graphics_file, (caddr_t)&gr_pict, True);


  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"SGI File",8)+ec_w;
  SGIFileButton = MakeXButton(next_button_x, next_button_y,
                              skip_x, button_height,
                              1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                              "SGI File", graphics_file, (caddr_t)&gr_sgi, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"RAST File",9)+ec_w;
  RASTFileButton = MakeXButton(next_button_x, next_button_y,
                               skip_x, button_height,
                               1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                               "RAST File", graphics_file, (caddr_t)&gr_rast, True);


  /* create the image buttons */
  next_button_x = 2;
  next_button_y += skip_y*2;
  skip_x = XTextWidth(ol_text_finfo,"Zoom +",6)+ec_w;        
  ZoomUpButton = MakeXButton(next_button_x,next_button_y,
                             skip_x,button_height,
                             1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                             "Zoom +", zoom, (caddr_t)&one, True);
  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Zoom -",6)+ec_w;        
  ZoomDownButton = MakeXButton(next_button_x,next_button_y,
                               skip_x,button_height,
                               1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                               "Zoom -", zoom, (caddr_t)&zero, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Selective Zoom",14)+ec_w;        
  SelectiveZoomButton = MakeXButton(next_button_x,next_button_y,
                                    skip_x,button_height,
                                    1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                                    "Selective Zoom", Selective_Zoom, (caddr_t)&zero, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Flip X",6)+ec_w;        
  FlipXButton = MakeXButton(next_button_x,next_button_y,
                            skip_x,button_height,
                            1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                            "Flip X", Flip_Image, (caddr_t)&zero, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Flip Y",6)+ec_w;        
  FlipYButton = MakeXButton(next_button_x,next_button_y,
                            skip_x,button_height,
                            1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                            "Flip Y", Flip_Image, (caddr_t)&one, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Rotate",6)+ec_w;        
  RotateButton = MakeXButton(next_button_x,next_button_y,
                             skip_x,button_height,
                             1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                             "Rotate", Rotate_Image, NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Reorient",8)+ec_w;        
  ReorientVolumeButton = MakeXButton(next_button_x,next_button_y,
                                     skip_x,button_height,
                                     1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                                     "Reorient", Reorient_Volume, NULL, True);

  next_button_x = 2;
  next_button_y += skip_y;
  skip_x = XTextWidth(ol_text_finfo,"Crop",4)+ec_w;    
  CropButton = MakeXButton(next_button_x,next_button_y,
                           skip_x,button_height,
                           1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                           "Crop", Crop, NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Statistics",10)+ec_w;    
  StatsButton = MakeXButton(next_button_x,next_button_y,
                            skip_x,button_height,
                            1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                            "Statistics", image_stats, NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"TIC",3)+ec_w;        
  TICButton = MakeXButton(next_button_x,next_button_y,
                          skip_x,button_height,
                          1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                          "TIC", TIC, NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Horiz. Profile",14)+ec_w;    
  HProfileButton = MakeXButton(next_button_x,next_button_y,
                               skip_x,button_height,
                               1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                               "Horiz. Profile", H_Profile_Switch, NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Vert. Profile",13)+ec_w;        
  VProfileButton = MakeXButton(next_button_x,next_button_y,
                          skip_x,button_height,
                          1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                          "Vert. Profile", V_Profile_Switch, NULL, True);


  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Minc Header",11)+ec_w;    
  FileInfoButton = MakeXButton(next_button_x, next_button_y,
                               skip_x, button_height,
                               1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                               "Minc Header", Open_Info_Widget, NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Plot",4)+ec_w;        
  PlotButton = MakeXButton(next_button_x, next_button_y,
                           skip_x, button_height,
                           1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                           "Plot", Open_Plot_Widget, NULL, True);

  if (file_format == MINC_FORMAT){

    /* Find maximum label widths */
    label_w = 0;
    for (idim=0; idim<ndimensions-2; idim++){
      slider_length[idim] = minc_volume_info.length[idim];
      if (minc_volume_info.length[idim] > 1){
        sprintf(ll, "%s:888  0", minc_volume_info.dimension_names[idim]);
        text_w = XTextWidth(ol_text_finfo,ll,strlen(ll));
        label_w = (text_w >= label_w ? text_w : label_w);
      }
    }
    extra_w = XTextWidth(ol_text_finfo,"000",3)+12;

    /* For each volume dimension over 2 (min. dimensions for an image)
     * with a dimension length greater than one, 
     * draw Next/Previous buttons and a Slider.
     * 
     * note: draw the sliders in order of decreasing speed 
     *       (i.e. inner loops -> outer loops)
     */
    for (idim=ndimensions-3; idim>=0; idim--) { 

      if (minc_volume_info.length[idim] > 1){

        /* Draw Next button */
        next_button_x = 2;
        next_button_y += skip_y;
        skip_x = XTextWidth(ol_text_finfo,"Next",4)+ec_w;        
        NextButton[idim] = MakeXButton(next_button_x,next_button_y,
                                       skip_x,button_height,
                                       1,newC[fcol].pixel,newC[bcol].pixel,cmdW, "Next", 
                                       Increment_Image, (caddr_t)&constant[idim], 
                                       True);

        /* Draw Previous button */
        next_button_x += skip_x + 6;
        skip_x = XTextWidth(ol_text_finfo,"Previous",8)+ec_w;        
        PreviousButton[idim] = MakeXButton(next_button_x, next_button_y,
                                           skip_x, button_height,
                                           1, newC[fcol].pixel, newC[bcol].pixel, cmdW, "Previous", 
                                           Decrement_Image, (caddr_t)&constant[idim], 
                                           True);

        next_button_x += skip_x + 6;
        sprintf(ll,"%s:%-3d  0", minc_volume_info.dimension_names[idim],
                image_number);
        sprintf(rl,"%-3ld",minc_volume_info.length[idim]-1);
        Image_Slider[idim] = MakeXSlider(next_button_x, next_button_y+3,
                                         cmd_width-next_button_x-label_w-extra_w, 
                                         button_height,
                                         label_w, extra_w, 0,
                                         ol_fg.pixel, ol_bg1.pixel, cmdW,
                                         ll, rl,
                                         Adjust_Image_Slider, (caddr_t)&constant[idim]);

      } /* end if */
    } /* end for */

  } else {

    if (num_images > 1) {

      next_button_x = 2;
      next_button_y += skip_y;
      skip_x = XTextWidth(ol_text_finfo,"Next",4)+ec_w;        
      NextButton[0] = MakeXButton(next_button_x,next_button_y,
                                  skip_x,button_height,
                                  1,newC[fcol].pixel,newC[bcol].pixel,cmdW,
                                  "Next", 
                                  Increment_Image, (caddr_t)&constant[0], 
                                  True);

      next_button_x += skip_x + 6;
      skip_x = XTextWidth(ol_text_finfo,"Previous",8)+ec_w;        
      PreviousButton[0] = MakeXButton(next_button_x, next_button_y,
                                      skip_x, button_height,
                                      1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                                      "Previous", 
                                      Decrement_Image, (caddr_t)&constant[0], 
                                      True);

      next_button_x += skip_x + 6;
      sprintf(ll,"Image:%-3d  0",image_number);
      sprintf(rl,"%-3d",num_images-1);
      label_w = XTextWidth(ol_text_finfo,"Image:888  0",12);
      extra_w = XTextWidth(ol_text_finfo,"000",3)+12;
      Image_Slider[0] = MakeXSlider(next_button_x, next_button_y+3,
                                    cmd_width-next_button_x-label_w-extra_w, 
                                    button_height,
                                    label_w, extra_w, 0,
                                    ol_fg.pixel, ol_bg1.pixel, cmdW,
                                    ll, rl,
                                    Adjust_Image_Slider, (caddr_t)&constant[0]);
    }

  }

  /* basic command buttons (i.e quit and help) */
  next_button_x = 2;
  next_button_y += skip_y*2;
  skip_x = XTextWidth(ol_text_finfo,"Quit",4)+ec_w;        
  QuitButton = MakeXButton(next_button_x, next_button_y,
                           skip_x, button_height,
                           1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                           "Quit", Quit, NULL, True);

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Help",4)+ec_w;        
  HelpButton = MakeXButton(next_button_x, next_button_y,
                           skip_x, button_height,
                           1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                           "Help", Open_Help_Widget, NULL, True); 

  next_button_x += skip_x + 6;
  skip_x = XTextWidth(ol_text_finfo,"Hide",4)+ec_w;        
  HideButton = MakeXButton(next_button_x, next_button_y,
                           skip_x, button_height,
                           1, newC[fcol].pixel, newC[bcol].pixel, cmdW,
                           "Hide", Hide_cmdW, NULL, True);

} 
