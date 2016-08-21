/*
 * 	ezwinput.c
 *
 *	Routines related to the EZ input widget for xdisp
 *      (imlemented using EZwgl).
 *
 *      Initialize_Input_Widget()
 *      Open_Input_Widget()
 *	Close_Input_Widget()
 *      Input_Widget_CallBack()
 *
 * 	Copyright Bruce Pike, 1993-2000
 */

#include "xdisp.h"

/*------------------- Initialize_Input_Widget() ---------------------*/
void Initialize_Input_Widget(void)
{
  /* create and configure the input widget */
  Input_Widget = EZ_CreateFrame(NULL, fname); 
  EZ_ConfigureWidget(Input_Widget, 
 		     EZ_PADX,         5, 
 		     EZ_PADY,         5, 
    		     EZ_LABEL_STRING, "",    
 		     0);
  Input_Widget_Label = EZ_CreateLabel(Input_Widget, "Upper:");
  Input_Text_Widget = EZ_CreateEntry(Input_Widget, NULL);
  EZ_ConfigureWidget(Input_Text_Widget, 
 		     EZ_WIDTH, 100, 
 		     EZ_HEIGHT, 20, 
 		     0); 

  /* set WM hints for user placement */
  EZ_SetWMHintsAndSizeHints(Input_Widget, 0);
}

/*------------------- Input_Widget_CallBack() -----------------------*/
void Input_Widget_CallBack(EZ_Widget *widget, void *label)
{
  char   *str;
  int    i, tmp;

  /* get the input string */
  str = EZ_GetEntryString(Input_Text_Widget);

  /* close the widget */
  Close_Input_Widget();
  
  /* set the upper/lower value */
  if (strcmp(label,"Upper:")==0) {
    Upper = (int) ((double) I_Min + ((atof(str)-rw_offset)/rw_scale));
  }
  else if (strcmp(label,"Lower:")==0) {
    Lower = (int) ((double) I_Min + ((atof(str)-rw_offset)/rw_scale));
  }
  else if (strcmp(label,"Gamma:")==0) {
    Gamma = atof(str)>0.0 ? atof(str) : 1.0;
    for (i=0; i<256; i++) {
      tmp = (int) (pow(((float)i/255.0), (1.0/Gamma)) * 255.0 + 0.5);
      Gamma_Table[i] = (byte) ((tmp <= 255) ? tmp : 255);
    }
  }
  Window_Level(Lower, Upper);
  update_sliders();
  update_msgs();
} 

/*---------------------- Open_Input_Widget() ------------------------*/
void Open_Input_Widget(void *data)
{
  char *label = (char *) data;

  /* set the input label */
  EZ_ConfigureWidget(Input_Widget_Label,
  		     EZ_LABEL_STRING, label,  
		     0);

  /* clear the input string */
  EZ_SetEntryString(Input_Text_Widget, NULL);
  
  /* register the callback with label data */		     
  EZ_SetWidgetCallBack(Input_Text_Widget, Input_Widget_CallBack, label);

  /* activate and display the widget */
  EZ_ActivateWidget(Input_Widget); 
  EZ_DisplayWidget(Input_Widget);

  /* force the title we want */
  XStoreName(theDisp,EZ_GetWidgetWindow(Input_Widget),"xdisp\0");
}

/*-------------------- Close_Input_Widget() --------------------------*/
void Close_Input_Widget(void)
{
  EZ_DeActivateWidget(Input_Widget);
}
