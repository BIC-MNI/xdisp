/*
 * 	ezwhelp.c
 *
 *	Routines related to the EZ help widget for xdisp
 *      (imlemented using EZwgl).
 *
 *      Initialize_Help_Widget()
 *      Open_Help_Widget()
 *	Close_Help_Widget()
 *      EZW_Online_Help()
 *
 * 	Copyright Bruce Pike, 1993-2000
 */

#include "xdisp.h"

/*-------------------- Initialize_Help_Widget() ---------------------*/
void Initialize_Help_Widget(void) 
{
  EZ_Widget    *tmp_w1, *tmp_w2;

  /* create and configure the help display widget */
  Help_Widget = EZ_CreateFrame(NULL, fname);
  EZ_ConfigureWidget(Help_Widget,
		     EZ_PADX,         0,
		     EZ_PADY,         0,
		     EZ_STACKING,  EZ_VERTICAL,
		     EZ_WIDTH,     650, EZ_HEIGHT, 500,
		     EZ_FILL_MODE, EZ_FILL_BOTH, 
		     EZ_LABEL_STRING, "Help",
		     0);

  tmp_w1 = EZ_CreateFrame(Help_Widget, NULL);
  EZ_ConfigureWidget(tmp_w1,
		     EZ_PADX,         8,
		     EZ_PADY,         0,
		     EZ_EXPAND,       True,
		     EZ_HEIGHT,       0,
		     EZ_STACKING,     EZ_HORIZONTAL_LEFT,
		     0);

  Help_Close_Button = EZ_CreateButton(tmp_w1,"Close",-1);

  /* create a widget for the actual help text */
  tmp_w2 = EZ_CreateFrame(Help_Widget, NULL);
  EZ_ConfigureWidget(tmp_w2,
		     EZ_FILL_MODE, EZ_FILL_BOTH,
		     0);
  Help_Text_Widget = EZ_CreateTextWidget(tmp_w2,0,1,1);

  /* define the help text */
  EZW_Online_Help();

  /* add the callback for shutdown of the help widget */
  EZ_AddWidgetCallBack(Help_Close_Button,
		       EZ_CALLBACK, Close_Help_Widget, NULL,0);

  /* set WM hints for user placement */
  EZ_SetWMHintsAndSizeHints(File_Save_Widget, 0);
}


/*---------------------- Open_Help_Widget() ------------------------*/
void Open_Help_Widget(void *data)
{
  /* activate and display the widget */
  EZ_ActivateWidget(Help_Widget); 
  EZ_DisplayWidget(Help_Widget);

  /* force the title we want */
  XStoreName(theDisp,EZ_GetWidgetWindow(Help_Widget),"xdisp: Help\0");
}

/*-------------------- Close_Help_Widget() --------------------------*/
void Close_Help_Widget(void *object, void *data)
{
  EZ_DeActivateWidget(Help_Widget);
}

/*--------------------- EZW_Online_Help() ---------------------------*/
void EZW_Online_Help(void)
{
  EZ_TextProperty  *title=NULL, 
                   *command=NULL, 
                   *plain=NULL;
  char             str[1024];
  
  /* get text properties */
  title = EZ_GetTextProperty(EZ_FONT_NAME, 
			     "-Adobe-Helvetica-Bold-R-Normal-*-14-*-*-*-*-*-*-*",
                             EZ_FOREGROUND, "red", 0);
  command = EZ_GetTextProperty(EZ_FONT_NAME, 
			     "-*-Courier-bold-r-normal-*-*-*-*-*-*-*-*-*",  
                             EZ_FOREGROUND, "blue", 0);
  plain = EZ_GetTextProperty(EZ_FONT_NAME,
     			     "-*-Courier-medium-r-normal-*-12-*-*-*-*-*-*-*", 
                             EZ_FOREGROUND, "black", 0);

  /* start pumping out the text */
  sprintf(str,"xdisp (%s): An image display utility for X systems.\n\n",version);
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, title);

  sprintf(str,"xdisp uses two primary windows: an "); 
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  sprintf(str,"image "); 
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str,"window and a ");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  sprintf(str,"command ");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str,"window.\n\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  
  sprintf(str,"The image window is always open (unless it has been explicitly iconified) whereas\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  sprintf(str,"the command window is, by default, closed.  Within the image window, pressing the\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  sprintf(str,"right mouse button opens or closes the command window.  Moving the mouse with the\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  sprintf(str,"middle mouse button depressed changes the image window/level.    Moving the mouse\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  sprintf(str,"with the left button depressed draws an ROI on the image.   In the command window\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  sprintf(str,"are multiple buttons and sliders with variuos (hopefully self evident) functions.\n"); 
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  sprintf(str,"Within the image window the following keys are defined:\n\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     q");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   quit\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     a");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   auto-scale image\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     b");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   toggle the color bar state\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     B");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   save image to file in byte format\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     c");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   crop image to current roi size\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     C");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   cycle through color maps (grey, hot metal, spectral)\n");    
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     d");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   toggle current volume dimension\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     f");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   output image to an ecapsulated postscript (EPSF) file\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     G");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   create a GIF file\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     h");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   this help text\n"); 
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     H");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   this help text\n"); 
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     i");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   toggle between bilinear and nearest neighbour interpolation\n"); 
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     l");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   load all images from file\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     L");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   enter Lower value for color map\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     m");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   calculate maximum intensity projection image\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     M");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   save image to a file in matlab format\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   reload image as a new file\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     o");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   reorient entire volume\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     p");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   print image\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     P");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   suspend plotting\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     r");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   rescale image to current window/level range\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     R");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   refresh image (erase any overlays)\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     s");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   image statistics\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     S");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   save image to file in short integer format\n");    
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     t");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   toggle (invert) greyscale\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     T");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   create a TIFF file\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     u");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   unzoom image (restore original size)\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     U");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   enter Upper value for color map\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     v");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   print version number\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     w");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   toggle window/level display\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     +");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   increment image number by 1\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     -");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   decrement image number by 1\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     *");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   increment image number by 10\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     /");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   decrement image number by 10\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"     ?");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   this help text\n"); 
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"   x/X");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   shrink/enlarge image width by a factor of 2\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"   y/Y");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   shrink/enlarge image height by a factor of 2\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"   z/Z");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   shrink/enlarge image by a factor of 2\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"    ^T");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   toggle external roi display state\n");    
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str," ^h/^v");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   horizontal/vertical profiles\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str," ^X/^Y");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   flip in x/y direction\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"    ^F");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   open file selector to load new image\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"    ^I");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   open minc header information window\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"    ^S");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   open file selector to spawn a new xdisp\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"    ^P");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   plot a time intensity curve for the current ROI\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"    ^R");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
  sprintf(str," ->   rotate image clockwise\n\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"More detailed information on the operation of xdisp and a complete listing of\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);
  sprintf(str,"command line options can be found in the man pages.\n"); 
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, plain);

  sprintf(str,"\n\nB Pike \nbruce@bic.mni.mcgill.ca\n");
  EZ_TextInsertStringWithProperty(Help_Text_Widget, str, command);
}
