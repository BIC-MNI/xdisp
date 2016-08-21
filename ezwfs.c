/*
 * 	ezwfs.c
 *
 *	Routines related to the EZ file selector widget for xdisp
 *
 *      Initialize_File_Widgets()
 *	Get_New_Filename()
 *      Open_File_Selector_Widget()
 *	Load_New_Image()
 *	Spawn_New_xdisp()
 *      Get_Save_Filename()
 *
 * 	Copyright Bruce Pike, 1997-2000
 */

#include "xdisp.h"


/*-------------- Initialize_File_Widgets() --------------------*/
void Initialize_File_Widgets(void)
{
  /* create the file selector widget */    
  File_Selector_Widget = EZ_CreateFileSelector(NULL, NULL);
  
  /* add the file selector callback */
  EZ_SetWidgetCallBack(File_Selector_Widget, Get_New_Filename, NULL);

  /* set WM hints for user placement */
  EZ_SetWMHintsAndSizeHints(File_Selector_Widget, 0);

  /* create the file save selector widget */    
  File_Save_Widget = EZ_CreateFileSelector(NULL, NULL);

  /* add the file save callback */
  EZ_SetWidgetCallBack(File_Save_Widget, Save_File_Select_Done_Callback, NULL); 

  /* set WM hints for user placement */
  EZ_SetWMHintsAndSizeHints(File_Save_Widget, 0);
}

/*----------------------- Get_New_Filename() --------------------------*/
void Get_New_Filename( EZ_Widget *widget, void *data)
{
  char	*new_fname;
  FILE 	*fp;
  char  *cptr;

  /* malloc space for filename */
  new_fname = (char *) malloc(256);
  
  /* get the selected file name and close the FS widget*/
  new_fname = EZ_GetFileSelectorSelection(File_Selector_Widget);
  /*  EZ_DeActivateWidget(File_Selector_Widget); */

  /* check to make sure file is valid and readbale */
  if ((fp = fopen(new_fname,"rb")) != NULL) {
    fclose(fp);    	
    /* Strip off leading directory names to form basfname */
    cptr = rindex(new_fname,'/');
    if (cptr) {
      strcpy(basfname,cptr+1);        	
    }
    else {
      strcpy(basfname,new_fname);
    }
    cptr = rindex(basfname,'\\');
    if (cptr) strcpy(basfname,cptr+1);
        
    /* clean up old tmp file if necessary */
    if (compressed) unlink(fname);

    /* Save the original name and if necessary, decompress the input file */
    strcpy(original_fname,new_fname);
    if (File_Is_Compressed(new_fname)){
      compressed = TRUE;
      /* fname is changed to point to a temporary decompressed file */
      new_fname = Decompress_File(new_fname);
    } else {
      compressed = FALSE;
    }

    /* change the window and icon names */
    strcpy(window_name, basfname);
    XStoreName(theDisp, cmdW, window_name); 
    XSetIconName(theDisp, cmdW, window_name); 
    XStoreName(theDisp, mainW, window_name); 
    XSetIconName(theDisp, mainW, window_name);

    /* load the new file */
    Load_New_Image(new_fname);
  }

  /* Invalid file */
  else {
    EZW_Error("Error: Unable to read selected file!");
  }
}

/*-------------------- Open_File_Selector_Widget() ------------------------*/
void Open_File_Selector_Widget(void *data)
{
  int  spawn;
  char *cptr, init_glob[128];

  /* get the spawn flag */
  spawn = *((int *) data);

  /* remove all File_Selector_Widget callbacks */
  EZ_RemoveAllWidgetCallBacks(File_Selector_Widget,EZ_CALLBACK);  

  /* set the File_Selector_Widget callback to spawn or not */
  if (spawn) {  
    EZ_SetWidgetCallBack(File_Selector_Widget, Spawn_New_xdisp, NULL); 
  }  
  else {  
    EZ_SetWidgetCallBack(File_Selector_Widget, Get_New_Filename, NULL); 
  }  

  /* set the initial globbing pattern */
  strcpy(init_glob,original_fname);
  cptr = rindex(init_glob,'/');
  if (cptr) {
    strcpy(cptr,"/*");
  }
  else {
    strcpy(init_glob, "*");
  }
  EZ_SetFileSelectorInitialPattern(File_Selector_Widget, init_glob); 

  /* now (re)activate the file selector widget and display it */
  EZ_ActivateWidget(File_Selector_Widget); 
  EZ_DisplayWidget(File_Selector_Widget);
  XStoreName(theDisp,
	     EZ_GetWidgetWindow(File_Selector_Widget),
	     "xdisp: File Selector\0");
}

/*----------------------------  Load_New_Image() --------------------------*/
void Load_New_Image(char *fn)
{
  int	   i, nsliders,
           old_w, old_h, 
           old_zw, old_zh, 
           old_x, old_y,
           y_skip;
  Window   dummyW;
  XWindowAttributes xwa;
  XSizeHints xsh;

  /* save the current comand window location */
  XGetWindowAttributes(theDisp,cmdW,&xwa);
  XTranslateCoordinates(theDisp, cmdW, rootW, 
			xwa.x, xwa.y, &old_x, &old_y, &dummyW);
  
  /* save current image and window size */
  old_w = Width;
  old_h = Height;
  old_zw = zWidth;
  old_zh = zHeight;

  /* reinitialize */
  ReInitialize();
    
  /* free up some stuff */
  free(short_Image);
  free(byte_Image);

  /* now reload */
  if (Verbose) fprintf(stderr,"The selected file is %s\n",fn);
  fname=fn; 
  Load_Image(fn);

  /* Create Color Bar */
  Define_ColorBar();

  /* Reset X image info */
  theImage->width=Width;
  theImage->height=Height;
  theImage->bytes_per_line=Width;
  theImage->data = (char *) byte_Image;

  /* Define corrected size if a command line zoom was used */
  zHeight = Height;
  zWidth = file_format!=CMI_FORMAT ? Width : Width*10/8; 

  /* Destroy and recreate the cmdW in case sliders/buttons need changing */
  XDestroyWindow(theDisp,cmdW);

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

  /* create the command window and force it to be automatically placed 
     in the position of the old command window */
  CreateCommandWindow(old_x,old_y,cmd_width,cmd_height,1,
		      fcol,bcol,USPosition,rootW,window_name);

  /* Create/Open X Resources for command window */
  wGC = XCreateGC(theDisp,cmdW,0,NULL);
  XSetForeground(theDisp,wGC, newC[wp].pixel);
  bGC = XCreateGC(theDisp,cmdW,0,NULL);
  XSetForeground(theDisp, bGC,newC[bp].pixel);
  theCGC = XCreateGC(theDisp,cmdW,0,NULL);
  XSetFont(theDisp,theCGC,ol_text_finfo->fid);
  XSetForeground(theDisp,theCGC,ol_fg.pixel);
  XSetBackground(theDisp,theCGC,ol_bg1.pixel);

  /* define the buttons */    
  Define_Cmd_Buttons();
  XDefineCursor(theDisp,cmdW,cmdCursor);

  /* define event mask for the command window */
  XSelectInput(theDisp, cmdW, 
	       ExposureMask |
	       EnterWindowMask |
	       ButtonPressMask |
	       ButtonReleaseMask |
	       StructureNotifyMask );

  /* Define the cmdW icon and initial state */
  SetCIcon(theDisp, cmdW, NormalState);
    
  /* map the window */    
  if (cmdW_State==NormalState) {
    XMapWindow(theDisp,cmdW); 
  }
  update_msgs();
  update_sliders();

  /* scale and resize image data */
  Scale_Image(Lower, Upper);
  Window_Level(Lower, Upper);
  if (Height==old_h && Width==old_w) {
    Resize(old_zw, old_zh);
  }
  else {
    Resize(Width, Height); 
  }
  XResizeWindow(theDisp,mainW,
		zWidth+(color_bar?color_bar_width:0),
		zHeight+info_height);
  DrawWindow(0,0,zWidth,zHeight);
  update_msgs();


  /* set the command window placement back in the user's hands for
     all future operations */
  XGetSizeHints(theDisp, cmdW, &xsh, XA_WM_NORMAL_HINTS);
  xsh.flags = PPosition | PSize | PMaxSize;
  XSetSizeHints(theDisp, cmdW, &xsh, XA_WM_NORMAL_HINTS);

  /* define new cursor */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}


/*---------------------------- Spawn_New_xdisp() --------------------------*/
void Spawn_New_xdisp( EZ_Widget *widget, void *data )
{
  char  *new_fname, *spawn_cmd;
  FILE  *fp;
  int   i;

  /* malloc space */
  new_fname = (char *) malloc(1024);
  spawn_cmd = (char *) malloc(2048);
  
  /* get the selected file name and close the FS widget*/
  new_fname = EZ_GetFileSelectorSelection(File_Selector_Widget);
  EZ_DeActivateWidget(File_Selector_Widget);

  /* check to make sure file is valid and readable */
  if ((fp = fopen(new_fname,"rb")) != NULL) { 
    fclose(fp);    	 
  } 
  else { 
    EZW_Error("Error: Unable to read selected file!"); 
    return; 
  } 

  /* built command line from original (minus filename) */
   sprintf(spawn_cmd,"%s",xd_argv[0]); 
   for (i = 1; i<xd_argc; i++) { 
     if (strcmp(xd_argv[i],fname)!=0) {	
       sprintf(spawn_cmd,"%s %s",spawn_cmd,xd_argv[i]); 
     } 
   } 

  /* now add the new file name to the command*/
   sprintf(spawn_cmd,"%s %s &",spawn_cmd, new_fname); 
   if (Verbose) fprintf(stderr,"Command line: %s \n",spawn_cmd);
   system(spawn_cmd);  
}

/*----------------------- Get_Save_Filename() --------------------------*/
char *Get_Save_Filename( char *def_fname )
{
  char	    *save_fname;
  EZ_Widget *pEntry, *sEntry, *dList, *fList, *okBtn, *fBtn, *cBtn;
  
  /* unset the save file done flag */
  Save_Select_Done = 0;
  
  /* set the initial filename pattern */
  EZ_GetFileSelectorWidgetComponents(File_Save_Widget, &pEntry, &sEntry, 
			       &dList, &fList, &okBtn, &fBtn, &cBtn); 
  EZ_SetOptionalEntryString(sEntry, def_fname); 
  EZ_ConfigureWidget(cBtn, EZ_LABEL_STRING, "Cancel", 0);
  EZ_SetWidgetCallBack(cBtn, Save_File_Select_Cancel_Callback, NULL);

  /* activate the file save widget and display it */
  EZ_ActivateWidget(File_Save_Widget); 
  EZ_DisplayWidget(File_Save_Widget);
  XStoreName(theDisp,
	     EZ_GetWidgetWindow(File_Save_Widget),
	     "xdisp: File Save\0");

  /* wait til selecton is done and only allow save widget events */
  EZ_SetGrab(File_Save_Widget);
  
  while (Save_Select_Done==0) {
    while(XPending(theDisp)) {
        XNextEvent(theDisp,&theEvent);
        HandleEvent(&theEvent);
	EZ_InvokePrivateEventHandler(&theEvent);
	EZ_WidgetDispatchEvent(&theEvent);
    }
    EZ_CheckTimerEvents();
    if(EZ_CheckAppInputs(1000)!= 0) EZ_SitForALittleBit(1000);
  }
  
  /* malloc space for filename */
  save_fname = (char *) malloc(256);

  /* get the selected file name and close the FS widget */
  if (Save_Select_Done==1) {
    save_fname = EZ_GetFileSelectorSelection(File_Save_Widget);
  }
  else {
    save_fname = "\0";
  }
  EZ_DeActivateWidget(File_Save_Widget);

  /* refresh the command and main windows */
/*   XUnmapSubwindows(theDisp,cmdW); */
/*   XMapSubwindows(theDisp,cmdW); */
/*   Refresh(); */

  /* return the selected file name */
  return(save_fname);
}

/*------------------- Save_File_Select_Done_Callback() -----------------------*/
void Save_File_Select_Done_Callback( EZ_Widget *widget, void *data )
{
  Save_Select_Done = 1;
}


/*------------------- Save_File_Select_Cancel_Callback() -----------------------*/
void Save_File_Select_Cancel_Callback( EZ_Widget *widget, void *data )
{
  Save_Select_Done = -1;
}

