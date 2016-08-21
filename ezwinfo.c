/*
 * 	ezwinfo.c
 *
 *	Routines related to the EZ minc info widget for xdisp
 *      (imlemented using EZwgl).
 *
 *      Initialize_Info_Widget()
 *      Open_Info_Widget()
 *	Close_Info_Widget()
 *
 * 	Copyright Bruce Pike, 1993-2000
 */

#include "xdisp.h"

/*------------------ Initialize_Info_Widget() ----------------------*/
void Initialize_Info_Widget(void)
{
  EZ_Widget    *tmp_w1, *tmp_w2;

  /* create and configure the information display widget */
  Info_Widget = EZ_CreateFrame(NULL, fname);
  EZ_ConfigureWidget(Info_Widget,
		     EZ_PADX,         0,
		     EZ_PADY,         0,
		     EZ_STACKING,  EZ_VERTICAL,
		     EZ_WIDTH,     650, EZ_HEIGHT, 500,
		     EZ_FILL_MODE, EZ_FILL_BOTH, 
		     0);

  tmp_w1 = EZ_CreateFrame(Info_Widget, NULL);
  EZ_ConfigureWidget(tmp_w1,
		     EZ_PADX,         8,
		     EZ_PADY,         0,
		     EZ_EXPAND,       True,
		     EZ_HEIGHT,       0,
		     EZ_STACKING,     EZ_HORIZONTAL_LEFT,
		     0);

  Info_Close_Button = EZ_CreateButton(tmp_w1,"Close",-1);

  /* create a widget for the actual mincheader text */
  tmp_w2 = EZ_CreateFrame(Info_Widget, NULL);
  EZ_ConfigureWidget(tmp_w2,
		     EZ_FILL_MODE, EZ_FILL_BOTH,
		     0);
  Mincheader_Widget = EZ_CreateTextWidget(tmp_w2,0,1,1);

  /* add the callback for shutdown of the info widget */
  EZ_AddWidgetCallBack(Info_Close_Button,
		       EZ_CALLBACK, Close_Info_Widget, NULL,0);

  /* set WM hints for user placement */
  EZ_SetWMHintsAndSizeHints(File_Save_Widget, 0);
}

/*---------------------- Open_Info_Widget() ------------------------*/
void Open_Info_Widget(void *data)
{
  char        tmp_name[128], tmp_cmd[256];
    
  /* check to make sure we have a minc file loaded */
  if (file_format!=MINC_FORMAT) {
    EZW_Error("Error: This is not a minc file!");
    return;
  }

  /* create a temp filename */
  tmpnam(tmp_name);

  /* issue shell command to create a mincheader dump file  */
  sprintf(tmp_cmd,"mincheader %s > %s \n",fname, tmp_name); 
  if (Verbose) fprintf(stderr,"issue shell command: %s \n", tmp_cmd); 
  system (tmp_cmd); 

  /* set window name to the current filename (minus leading dirs) */
  EZ_ConfigureWidget(Info_Widget,EZ_LABEL_STRING,basfname,0);
  
  /* load the mincheader dump info into a text widget and siplay the result */
  EZ_ActivateWidget(Info_Widget); 
  EZ_TextLoadFile(Mincheader_Widget,tmp_name);
  EZ_DisplayWidget(Info_Widget);
  XStoreName(theDisp,
	     EZ_GetWidgetWindow(Info_Widget),
	     "xdisp: Minc Header\0");

  /* clean up the tmp file */
  sprintf(tmp_cmd,"rm %s &\n",tmp_name); 
  if (Verbose) fprintf(stderr,"issue shell command: %s\n",tmp_cmd);
  system (tmp_cmd); 
}

/*-------------------- Close_Info_Widget() --------------------------*/
void Close_Info_Widget(void *object, void *data)
{
  EZ_TextClear(Mincheader_Widget);
  EZ_DeActivateWidget(Info_Widget);
}
