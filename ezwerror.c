/*
 * 	ezwerror.c
 *
 *	Routines related to the EZ Error widget for xdisp
 *      (imlemented using EZwgl).
 *
 *      Initialize_Error_Widget()
 *      Open_Error_Widget()
 *	Close_Error_Widget()
 *      EZW_Error()
 *
 * 	Copyright Bruce Pike, 1993-2000
 */

#include "xdisp.h"

/*-------------------- Initialize_Error_Widget() ---------------------*/
void Initialize_Error_Widget() 
{
  EZ_Widget    *tmp_w1, *tmp_w2;

  /* create and configure the error display widget */
  Error_Widget = EZ_CreateFrame(NULL, fname);
  EZ_ConfigureWidget(Error_Widget,
		     EZ_PADX,         0,
		     EZ_PADY,         0,
		     EZ_STACKING,     EZ_VERTICAL,
		     EZ_WIDTH,        300, 
                     EZ_HEIGHT,       140,
		     EZ_FILL_MODE,     EZ_FILL_BOTH, 
		     EZ_LABEL_STRING, "Error",
                     EZ_FONT_NAME, 
		     "-Adobe-Helvetica-Bold-R-Normal-*-16-*-*-*-*-*-*-*",
                     EZ_FOREGROUND, "red",
		     0);

  /* create a widget for the actual error text */
  tmp_w2 = EZ_CreateFrame(Error_Widget, NULL);
  EZ_ConfigureWidget(tmp_w2,
		     EZ_FILL_MODE, EZ_FILL_BOTH,
		     0);
  Error_Text_Widget = EZ_CreateTextWidget(tmp_w2,0,1,0);
  EZ_ConfigureWidget(Error_Text_Widget,
                     EZ_HEIGHT,       60,
		     EZ_FILL_MODE, EZ_FILL_BOTH,
		     0);

  /* create a frame */
  tmp_w1 = EZ_CreateFrame(Error_Widget, NULL);
  EZ_ConfigureWidget(tmp_w1,
		     EZ_PADX,         2,
		     EZ_PADY,         2,
		     EZ_EXPAND,       False,
		     EZ_HEIGHT,       0,
		     EZ_STACKING,     EZ_HORIZONTAL_CENTER,
		     0);

  /* create the close button */
  Error_Close_Button = EZ_CreateButton(tmp_w1,"Close",-1);

  /* add the callback for shutdown of the error widget */
  EZ_AddWidgetCallBack(Error_Close_Button,
		       EZ_CALLBACK, Close_Error_Widget, NULL,0);

  /* set WM hints for user placement */
  EZ_SetWMHintsAndSizeHints(File_Save_Widget, 0);
}


/*---------------------- Open_Error_Widget() ------------------------*/
void Open_Error_Widget(void)
{
  /* activate and display the widget */
  EZ_ActivateWidget(Error_Widget); 
  EZ_DisplayWidget(Error_Widget);

  /* force the title we want */
  XStoreName(theDisp,EZ_GetWidgetWindow(Error_Widget),"xdisp: Error\0");
}

/*-------------------- Close_Error_Widget() --------------------------*/
void Close_Error_Widget(void *object, void *data)
{
  EZ_DeActivateWidget(Error_Widget);
}

/*--------------------- EZW_Error() ---------------------------*/
void EZW_Error(char *error_text)
{
  EZ_TextProperty  *error_tp=NULL;
  
  /* open the error widget */
  Open_Error_Widget();
  
  /* get text properties */
  error_tp = EZ_GetTextProperty(EZ_FONT_NAME, 
				"-Adobe-Helvetica-Bold-R-Normal-*-12-*-*-*-*-*-*-*",
				EZ_FOREGROUND, "black", 0);

  /* display the error test */
  EZ_TextInsertStringWithProperty(Error_Text_Widget, error_text, error_tp);
}
