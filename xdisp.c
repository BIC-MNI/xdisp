/*
 * 	xdisp.c
 *
 * 	Main routine for xdisp.
 *
 *	xdisp is an utility for viewing images in X 
 *	and is based only on Xlib.
 *
 *	main()
 *
 * 	Copyright (c) Bruce Pike, 1993-2000
 *
 */

#define MAIN
#include "xdisp.h"
#include "cross_hr.bit"
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

/* Include for unlink() */
#include <unistd.h>

/*---------------------------- main -------------------------*/
int main(int argc, char **argv)
{
  int         i, j, fn_flag, y_skip;
  char        *display, *geom, *getenv();
  char        *cptr, *spawn_cmd, *spawn_cmd_root;
  XVisualInfo Vis_Info;
  int 	      nsliders;

  /* Perform initialization */
  for(i=0; i<MAX_VAR_DIMS; i++){
    constant[i] = i;
  }
  fname = display = geom = NULL;
  Initialize();
  EZ_Initialize(argc,argv,0);

  /* malloc space for spawn command */
  spawn_cmd_root = (char *) malloc(2048);
  spawn_cmd = (char *) malloc(2048);
  
  /* inital state*/
  cmdW_Initial_State = cmdW_State = IconicState; /*command window hidden*/

  /* Parse command line options */
  cmd = argv[0];
  xd_argv = argv;
  xd_argc = argc;
  for (i = 1; i<argc; i++) {

    if ((!strncmp(argv[i],"-geom",5)) || 
	(!strncmp(argv[i],"-geometry",9))) { /* geometry */
      i++;
      geom = argv[i];
      continue;
    }

    if (!strncmp(argv[i],"-name",5)) {       /*  window name */
      i++;
      strcpy(window_name,argv[i]);
      continue;
    }
            
    if ((!strncmp(argv[i],"-fn",3)) || 
	(!strncmp(argv[i],"-font",5))){      /* font */
      i++;
      font = argv[i];
      continue;
    }

    if ((!strncmp(argv[i],"-fg",3)) || 
	(!strncmp(argv[i],"-foreground",11))){/* fg color */
      i++;
      fg = argv[i];
      continue;
    }

    if ((!strncmp(argv[i],"-bg",3)) || 
	(!strncmp(argv[i],"-background",11))){/* bg color */
      i++;
      bg = argv[i];
      continue;
    }

    if (!strncmp(argv[i],"-grc",4)) {	      /* graphics color */
      i++;
      grc = argv[i];
      roic = argv[i];
      roiceps = atoi(argv[i]);
      continue;
    }

    if (!strncmp(argv[i],"-noiconify",8)) { /* initial cmdW state */
      cmdW_Initial_State = cmdW_State = NormalState;
      continue;
    }
        
    if (!strncmp(argv[i],"-private",8)) {	/* private_cmap */
      private_cmap = True;
      continue;
    }

    if (!strncmp(argv[i],"-DirectColor",12)) {/* use a DirectColor Visual */
      Preferred_Visual_Class = DirectColor;
      continue;
    } 

    if (!strncmp(argv[i],"-TrueColor",10))  {/* use a TrueColor Visual */
      Preferred_Visual_Class = TrueColor;
      continue;
    }

    if (!strncmp(argv[i],"-PseudoColor",12)) {/* use a PseudoColor Visual */
      Preferred_Visual_Class = PseudoColor;
      continue;
    }
            
    if (!strncmp(argv[i],"-help",5)) {	/* help */
      Help();
      exit(1);
    }

    if (!strncmp(argv[i],"-cmi",4)) {	/* CMI format */
      file_format = CMI_FORMAT;
      continue;
    }

    if (!strncmp(argv[i],"-grey",5)) {	/* gery color table */
      color_table = 0;
      continue;
    }

    if (!strncmp(argv[i],"-hot",4)) {	/* hot metal color table */
      color_table = 1;
      continue;
    }

    if (!strncmp(argv[i],"-spectral",9)) {	/* spectral color table */
      color_table = 2;
      continue;
    }

    if (!strncmp(argv[i],"-swap",5)) {	/* perform byte swap */
      Swap = True;
      continue;
    }
            
    if (!strncmp(argv[i],"-color_bar",10)) {/* display a color bar */
      color_bar = 1;
      continue;
    }

    if (!strncmp(argv[i],"-all",4)) {	/* load all images */
      load_all_images = 1;
      continue;
    }

    if (!strncmp(argv[i],"-image",6)) {	/* Starting image */
      i++;
      image_increment = atoi(argv[i])-1;
      continue;
    }

    if (!strncmp(argv[i],"-gamma",6)) {     /* initial Gamma factor */
      i++;
      Gamma = atof(argv[i]);
      Gamma = Gamma>0.0 ? Gamma : 1.0;
      continue;
    }
            
    if (!strncmp(argv[i],"-byte",5)) {	/* byte data */
      Input_Data_Type = BYTE_DATA;
      Image_Depth = sizeof(char);
      continue;
    }

    if (!strncmp(argv[i],"-ubyte",6)) {	/* unsigned byte data */
      Input_Data_Type = UBYTE_DATA;
      Image_Depth = sizeof(unsigned char);
      continue;
    }

    if (!strncmp(argv[i],"-short",6)) {	/* short int data */
      Input_Data_Type = SHORT_DATA;
      Image_Depth = sizeof(short);
      continue;
    }

    if (!strncmp(argv[i],"-ushort",7)) {	/* unsigned short int data */
      Input_Data_Type = USHORT_DATA;
      Image_Depth = sizeof(unsigned short);
      continue;
    }

    if (!strncmp(argv[i],"-long",5) ||
	!strncmp(argv[i],"-int",4)) {	/* long int data */
      Input_Data_Type = LONG_DATA;
      Image_Depth = sizeof(long);
      continue;
    }

    if (!strncmp(argv[i],"-ulong",6) ||
	!strncmp(argv[i],"-uint",5)) {	/* unsigned long int data */
      Input_Data_Type = ULONG_DATA;
      Image_Depth = sizeof(unsigned long);
      continue;
    }

    if (!strncmp(argv[i],"-float",6)) {	/* float data */
      Input_Data_Type = FLOAT_DATA;
      Image_Depth = sizeof(float);
      continue;
    }

    if (!strncmp(argv[i],"-double",7)) {	/* double data */
      Input_Data_Type = DOUBLE_DATA;
      Image_Depth = sizeof(double);
      continue;
    }

    if (!strncmp(argv[i],"-nc",3)) {	/* numcolors */
      i++;
      NumColors = atoi(argv[i]);
      NumColors = (NumColors-NumGCColors>1) ?
	NumColors : NUM_IM_COLORS+NumGCColors;
      NumColors = (NumColors<COLOR_MAP_SIZE-56) ?
	NumColors : COLOR_MAP_SIZE-56;
      continue;
    }

    if (!strncmp(argv[i],"-nn",3) ||        /* nearest neighbour */
	!strncmp(argv[i],"-nearest_neighbour",18)) { 
      Interpolation_Type = NEAREST_NEIGHBOUR;
      continue;
    }

    if (!strncmp(argv[i],"-bilinear",3)) {	/* bilinear interpolation */
      Interpolation_Type = BILINEAR;
      continue;
    }

    if (!strncmp(argv[i],"-wl",3)) {	/* window/level display */
      wl_display = 1;
      continue;
    }

    if (!strncmp(argv[i],"-roi",4)) {	/* roi filename */
      i++;
      roifname = argv[i];
      ext_roi_on = TRUE;
      continue;
    }

    if (!strncmp(argv[i],"-rescale",8)) {	/* always rescale image */
      Scale_Data = 1;
      continue;
    }

    if (!strncmp(argv[i],"-version",8)) {	/* Version */
      fprintf(stderr, "xdisp: %s\n",version);
      exit_xdisp(0);
    }

    if (!strncmp(argv[i],"-v",2) ||
	!strncmp(argv[i],"-verbose",8)) {	/* debug */
      Verbose = True;
      continue;
    }

    if (!strncmp(argv[i],"-width",6) ||
	!strncmp(argv[i],"-w",2) ) {       /* width (columns) */
      i++;
      Width = atoi(argv[i]);
      continue;
    }

    if (!strncmp(argv[i],"-h",2) ||
	!strncmp(argv[i],"-height",7)) {	/* height (rows) */
      if (i+1 == argc) {
	fprintf(stderr,"Note: -h is used to specify image height.\n");
	fprintf(stderr,"Please use 'xdisp -help' if you are looking for help.\n");
	exit(1);
      }
      i++;
      Height = atoi(argv[i]);
      continue;
    }
	        
    if (!strncmp(argv[i],"-o",2) ||
	!strncmp(argv[i],"-offset",7)) {	/* image offset */
      i++;
      Byte_Offset = atoi(argv[i]);
      continue;
    }

    if (!strncmp(argv[i],"-skip",5))  {	/* byte skip between reads */
      i++;
      Byte_Skip = atoi(argv[i]);
      continue;
    }

    if (!strncmp(argv[i],"-u",2) ||
	!strncmp(argv[i],"-upper",6)) {	/* Starting Upper */
      i++;
      Upper = atoi(argv[i]);
      rw_Upper = atof(argv[i]);
      continue;
    }

    if (!strncmp(argv[i],"-l",2) ||
	!strncmp(argv[i],"-lower",6)) {	       /* Starting Lower */
      i++;
      Lower = atoi(argv[i]);
      rw_Lower = atof(argv[i]);
      continue;
    }

    if (!strncmp(argv[i],"-z",2) ||
	!strncmp(argv[i],"-zoom",5)) {	       /* initial zoom factor */
      i++;
      zoom_factor_x = atof(argv[i]);
      zoom_factor_x = zoom_factor_x>0.0 ? zoom_factor_x : 1;
      zoom_factor_y = zoom_factor_x;
      continue;
    }

    if (!strncmp(argv[i],"--",2)) {		/* read from stdin */
      fname = "stdin";
      read_from_stdin = 1;
      load_all_images = False;
      if (Width==0) Width = 256;
      if (Height==0) Height = 256;
      continue;
    }

    if (argv[i][0] != '-') {		        /* the file name */
      cmd_line_fnames[number_of_files] = argv[i];
      number_of_files++;
      /*      fname = argv[i]; */
      continue;
    }

    if (!strncmp(argv[i],"-",1)) {
      Syntax(cmd);
    }

    Syntax(cmd);
  }

  /*  if (fname==NULL) Syntax(cmd); */
  /* Make sure we have at least one filename.  If more, spawn new xdisp */
  if (number_of_files==0) {
    Syntax(cmd);
  }
  else {
    fname = cmd_line_fnames[0];
    sprintf(spawn_cmd_root,"%s",xd_argv[0]); 
    for (i=1; i<xd_argc; i++) { 
      fn_flag = 0;
      for (j=0; j<number_of_files; j++)
	if (strcmp(xd_argv[i],cmd_line_fnames[j])==0) {	
	  fn_flag = 1;
	} 
      if (!fn_flag) {	
	sprintf(spawn_cmd_root,"%s %s",spawn_cmd_root,xd_argv[i]); 
      } 
    } 
    for (i=1; i<number_of_files; i++) {
      sprintf(spawn_cmd,"%s %s &",spawn_cmd_root,cmd_line_fnames[i]);
      if (Verbose) 
	fprintf(stderr,"Spawned command line: %s \n",spawn_cmd);
      system(spawn_cmd);
    }
  }

  /* Open up the display. */
  theDisp=EZ_GetDisplay();

  /* Get some basic info */
  theScreen = DefaultScreen(theDisp);
  rootCmap  = DefaultColormap(theDisp, theScreen);
  rootW     = RootWindow(theDisp,theScreen);
  theVisual = DefaultVisual(theDisp,theScreen);
  theDepth  = DefaultDepth(theDisp,theScreen);    
  dispcells = DisplayCells(theDisp,theScreen);
  if (Verbose) {
    fprintf(stderr,
    "The default visual (ID=0x%lx) depth is %d bits, with %d colormap cells.\n",
    theVisual->visualid,theDepth,dispcells);
  }

  /* Select a visual: first try for any Preferred visuals */
  if ((Preferred_Visual_Class == PseudoColor) &&
      XMatchVisualInfo(theDisp, theScreen, 8, PseudoColor, &Vis_Info)) { 
    theDepth = 8;
    bitmap_pad = 8;
    red_shift = 0;
    green_shift = 0;
    blue_shift = 1;
    theVisual = Vis_Info.visual;
    ColorMapSize = Vis_Info.colormap_size;
    Selected_Visual_Class = PseudoColor;
    if (Verbose) 
      fprintf(stderr,"%d bit PseudoColor visual (ID = 0x%lx) selected.\n",
	      theDepth,theVisual->visualid);
  }
  else if ((Preferred_Visual_Class==DirectColor) &&
	   XMatchVisualInfo(theDisp, theScreen, 24, DirectColor, &Vis_Info)) {
    theDepth = 24;
    bitmap_pad = 32;
    red_shift = 65536;
    green_shift = 256;
    blue_shift = 1;
    theVisual = Vis_Info.visual;
    ColorMapSize = Vis_Info.colormap_size;
    Selected_Visual_Class = DirectColor;
    if (Verbose) 
      fprintf(stderr,"%d bit DirectColor visual (ID = 0x%lx) selected.\n",
	      theDepth,theVisual->visualid);
  }
  else if ((Preferred_Visual_Class==TrueColor) &&
	   XMatchVisualInfo(theDisp, theScreen, 24, TrueColor, &Vis_Info)) {
    theDepth = 24;
    bitmap_pad = 32;
    red_shift = 65536;
    green_shift = 256;
    blue_shift = 1;
    ColorMapOffset = 0;
    NumGCColors = 0;
    NumColors = 256;
    theVisual = Vis_Info.visual;
    ColorMapSize = Vis_Info.colormap_size;
    Selected_Visual_Class = TrueColor;
    if (Verbose) 
      fprintf(stderr,"%d bit TrueColor visual (ID = 0x%lx) selected.\n",
	      theDepth,theVisual->visualid);
  }
  /* Oh well, try for visuals in the following preferred order...
     an 8 bit PseudoColor, a 24 bit TrueColor, a 24 bit DirectColor */
  else if (XMatchVisualInfo(theDisp, theScreen, 8, PseudoColor, &Vis_Info)) { 
    theDepth = 8;
    bitmap_pad = 8;
    red_shift = 0;
    green_shift = 0;
    blue_shift = 1;
    theVisual = Vis_Info.visual;
    ColorMapSize = Vis_Info.colormap_size;
    Selected_Visual_Class = PseudoColor;
    if (Verbose) 
      fprintf(stderr,"%d bit PseudoColor visual (ID = 0x%lx) selected.\n",
	      theDepth,theVisual->visualid);
  }
  else if (XMatchVisualInfo(theDisp, theScreen, 24, TrueColor, &Vis_Info)) {
    theDepth = 24;
    bitmap_pad = 32;
    red_shift = 65536;
    green_shift = 256;
    blue_shift = 1;
    ColorMapOffset = 0;
    NumGCColors = 0;
    NumColors = 256;
    theVisual = Vis_Info.visual;
    ColorMapSize = Vis_Info.colormap_size;
    Selected_Visual_Class = TrueColor;
    if (Verbose) 
      fprintf(stderr,"%d bit TrueColor visual (ID = 0x%lx) selected.\n",
	      theDepth,theVisual->visualid);
  }
  else if (XMatchVisualInfo(theDisp, theScreen, 24, DirectColor, &Vis_Info)) {
    theDepth = 24;
    bitmap_pad = 32;
    red_shift = 65536;
    green_shift = 256;
    blue_shift = 1;
    theVisual = Vis_Info.visual;
    ColorMapSize = Vis_Info.colormap_size;
    Selected_Visual_Class = DirectColor;
    if (Verbose) 
      fprintf(stderr,"%d bit DirectColor visual (ID = 0x%lx) selected.\n",
	      theDepth,theVisual->visualid);
  }
  else {
    FatalError("Sorry, no 8 bit PseudoColor or 24 bit DirectColor or TrueColor visual.");
  }
  if (Selected_Visual_Class != Preferred_Visual_Class &&
      Preferred_Visual_Class!= -1) {
    if (Selected_Visual_Class==PseudoColor) {
      if (Verbose) fprintf(stderr,"Preferred visual not available... a %d bit PseudoColor viusal (ID = 0x%lx) was selected instead.\n",
	      theDepth,theVisual->visualid);
    }
    else if (Selected_Visual_Class==DirectColor) {
      if (Verbose) fprintf(stderr,"Preferred visual not available... a %d bit DirectColor viusal (ID = 0x%lx) was selected instead.\n",
	      theDepth,theVisual->visualid);
    }
    else {
      if (Verbose) fprintf(stderr,"Preferred visual not available... a %d bit TrueColor viusal (ID = 0x%lx) was selected instead.\n",
	      theDepth,theVisual->visualid);
    }
  }

  if (ColorMapSize<16) 
    FatalError("This program requires at least 16 colormap cells.");

  /* Setup the Colors */
  if (Selected_Visual_Class == TrueColor)
    TrueColor_Initialize();
  else 
    Define_ColorMap();

  /* create a Gamma table to speed up some calculations */
  for (i=0; i<256; i++) {
    tmp = (int) (pow(((float)i/255.0), (1.0/Gamma)) * 255.0 + 0.5);
    Gamma_Table[i] = (byte) ((tmp <= 255) ? tmp : 255);
  }

  /* set up olgx stuff */
  olgx_init();

  /* Strip off leading directory names to form basfname */
  cptr = rindex(fname,'/');
  if (cptr) {
    strcpy(basfname,cptr+1);
  }
  else {
    strcpy(basfname,fname);
  }
  cptr = rindex(basfname,'\\');
  if (cptr) strcpy(basfname,cptr+1);

  /* Save the original file name and if necessary, decompress the input file */
  strcpy(original_fname,fname);
  if (File_Is_Compressed(fname)){
    compressed = TRUE;
    /* fname is changed to point to a temporary decompressed file */
    fname = Decompress_File(fname);
  } 
  else {
    compressed = FALSE;
  }

  /* Get the image */
  Load_Image(fname);

  /* Create Color Bar */
  Define_ColorBar();    

  /* Allocate X image for the color bar */
  theColorBar = XCreateImage(theDisp,	        /* the Display */
			     theVisual,	        /* the Visual */
			     theDepth,	        /* the visual depth */
			     ZPixmap,           /* format */
			     0,     		/* data offset */
			     (char *)byte_ColorBar,/* pointer to the data */  
			     color_bar_width,   /* width in pixels */
			     Height,		/* height in pixels */
			     bitmap_pad,	/* bitmap pad */
			     0);		/* bytes per line (0=let X figure it out) */
  if (!theColorBar) FatalError("unable to create XImage");
    
  /* Allocate the X Image for the image */
  theImage = XCreateImage(theDisp,
			  theVisual,
			  theDepth,
			  ZPixmap,
			  0,
			  (char *)byte_Image,
			  Width,
			  Height,
			  bitmap_pad,
			  0);
  if (!theImage) FatalError("unable to create XImage");

  /* Define some more defaults */
  Width = theImage->width;  
  Height = theImage->height;
  zWidth = file_format != CMI_FORMAT ? Width*zoom_factor_x : 
                          Width*zoom_factor_x*10/8;
  zHeight = Height*zoom_factor_y;
  info_height = (ol_text_finfo->max_bounds.ascent +
		 ol_text_finfo->max_bounds.descent + 4) *
                (cmdW_Initial_State==IconicState);

  /* create main window */                             
  CreateMainWindow(cmd,geom,argc,argv);

  /* Setup command window and button sizes */
  button_height = Button_Height(ol_ginfo);
  y_skip = (int) button_height*0.5;
  title_height = button_height*2 + 4;
  sprintf(title_msg,"Crop Statistics TIC Horizontal Profile Vertical Profile Minc Header");
  cmd_width = (unsigned int)
    ((XTextWidth(ol_text_finfo,title_msg,strlen(title_msg)) +
      ButtonEndcap_Width(ol_ginfo)*2*6 + 6*5 + 8) < 400 ?
     400 : XTextWidth(ol_text_finfo,title_msg,strlen(title_msg)) +
      ButtonEndcap_Width(ol_ginfo)*2*6 + 6*5 + 8);
  sprintf(title_msg,"Filename: %s, Min=%d, Max=%d, size=%dx%d",
	  basfname, P_Min, P_Max, zWidth, zHeight); 
  cmd_width = (unsigned int)
    (XTextWidth(ol_text_finfo,title_msg,strlen(title_msg))+8 < cmd_width ?
     cmd_width : XTextWidth(ol_text_finfo,title_msg,strlen(title_msg))+8);
  
  for(i=0, nsliders=0; i<ndimensions-2; i++){
    if (slider_length[i] > 1) nsliders++;
  }
  cmd_height = (unsigned int)(title_height +
			      (button_height+y_skip)*(6+nsliders) +
			      button_height*12 +
                              (int)y_skip/2);

  /* create the command window */
  CreateCommandWindow(100,100,cmd_width,cmd_height,1,
		      fcol,bcol,PPosition,rootW,window_name);
  Define_Cmd_Buttons();

  /* Create/Open X Resources for command window */
  wGC = XCreateGC(theDisp,cmdW,0,NULL);
  XSetForeground(theDisp,wGC, newC[wp].pixel);
  bGC = XCreateGC(theDisp,cmdW,0,NULL);
  XSetForeground(theDisp, bGC,newC[bp].pixel);
  theCGC = XCreateGC(theDisp,cmdW,0,NULL);
  XSetFont(theDisp,theCGC,ol_text_finfo->fid);
  XSetForeground(theDisp,theCGC,ol_fg.pixel);
  XSetBackground(theDisp,theCGC,ol_bg1.pixel);

  /* set up a GC for the main window */
  theMGC = XCreateGC(theDisp, mainW, 0, NULL);
  XSetFont(theDisp,theMGC,ol_text_finfo->fid);
  XSetForeground(theDisp,theMGC,ol_fg.pixel);
  XSetBackground(theDisp,theMGC,ol_bg1.pixel);

  /* set up a GC for the roi rubber-banding */
  xgcv.foreground = newC[wp].pixel;
  xgcv.background = newC[bp].pixel;
  xgcv.line_style = LineDoubleDash;
  xgcv.function = GXxor;
  roiGC = XCreateGC(theDisp, mainW,
		    GCForeground | 
		    GCBackground |
		    GCLineStyle | 
		    GCFunction,
		    &xgcv);

  /* set up a GC for other graphics */
  grGC = XCreateGC(theDisp, mainW, 0, NULL);
  XSetForeground(theDisp, grGC, newC[gcol].pixel);

  /* create required cursors */
  if ((cross_hr_bmap=XCreateBitmapFromData(theDisp,mainW,
       cross_hr_bits,cross_hr_width,cross_hr_height))==None) 
    fprintf(stderr,"Error on cursor creation\n");
  if ((cross_hrmsk_bmap=XCreateBitmapFromData(theDisp,mainW,
       cross_hrmsk_bits,cross_hr_width,cross_hr_height))==None)
    fprintf(stderr,"Error on cursor creation\n");
  mainCursor=XCreatePixmapCursor(theDisp, cross_hr_bmap,
				 cross_hrmsk_bmap, &cur_fg, &cur_bg,
				 cross_hr_x_hot, cross_hr_y_hot);
  cmdCursor  = XCreateFontCursor(theDisp,XC_top_left_arrow);
  waitCursor = XCreateFontCursor(theDisp,XC_watch);
  wlCursor   = XCreateFontCursor(theDisp,XC_fleur);
  roiCursor  = XCreateFontCursor(theDisp,XC_top_left_arrow);
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);


  /* define event mask for the main window */
  XSelectInput(theDisp, mainW, 
	       ExposureMask | 
	       KeyPressMask |
	       EnterWindowMask |
	       LeaveWindowMask |			 	 
	       ButtonPressMask |
	       ButtonReleaseMask | 
	       PointerMotionMask |
	       StructureNotifyMask );

  /* define event mask for the command window */
  XSelectInput(theDisp, cmdW, 
	       ExposureMask |
	       EnterWindowMask |
	       ButtonPressMask |
	       ButtonReleaseMask |
	       StructureNotifyMask );

  /* Define the Icons and initial state */
  SetCIcon(theDisp, cmdW, IconicState);
  SetMIcon(NormalState);

  /* Create the EZ widgets if enabled */
  Create_EZ_Widgets();

  /* Load specific image if requested */
  if (image_increment!=0) {
    New_Image();
    update_sliders(); 
  }

  /* Map main window and cmdW (if cmdW_Initial_State is open) */
  XMapWindow(theDisp,mainW);
  if (cmdW_Initial_State==NormalState) {
    XMapWindow(theDisp,cmdW);
  }
  XFlush(theDisp);

  /* Scale and Resize */
  Rescale(NULL);
  /*printf("Main window id=%ld command window id=%ld\n",mainW,cmdW);*/

  /* Main loop */
  while(1){
    while(XPending(theDisp)) {
      XNextEvent(theDisp,&theEvent);
      HandleEvent(&theEvent);
      EZ_InvokePrivateEventHandler(&theEvent);
      EZ_WidgetDispatchEvent(&theEvent);
    }
    EZ_CheckTimerEvents();
    if(EZ_CheckAppInputs(1000)!= 0) EZ_SitForALittleBit(1000);
  }
  return 0;
}
