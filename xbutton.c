/*
 * 	xbutton.c
 *	
 *      Basic routines for the command window buttons and sliders.
 *	
 *	MakeXButton()
 *	MakeXSlider()
 *	button_handler()
 *	slider_handler()
 *	draw_button()
 *	draw_slider()
 *	
 *
 *	Copyright (c) B. Pike, 1993-2000
 */

#include <time.h> 
#include "xdisp.h"

static void button_handler(XWIN *p_xwin);
static void slider_handler(XWIN *p_xwin);
static void draw_button(XWIN *p_xwin, int state);

/*------------------------ MakeXButton -------------------------
 *
 * A labeled button you can press to perform an action
 *
 */

XWIN    *MakeXButton(
                     int 	    x, int y,
                     unsigned    width, unsigned height, unsigned bdwidth,
                     ulong       bdcolor, ulong bgcolor,
                     Window 	    parent,
                     char 	    *label,
                     void (*button_action)(void *),
                     void *action_data,
                     int 	    state)
{
  XWIN			*new_button;
  D_BUTTON    		*p_data;
  XSizeHints  		*bhints;    
  XSetWindowAttributes 	xswa;
  unsigned int 		xswamask;
    
  /* Allocate button specifc data */
  if ((p_data=calloc(1,sizeof(D_BUTTON)))==NULL) {
    fprintf(stderr, "No memory for button's data");
    exit_xdisp(1);    	
  }

  /* Allocate a XWIN structure */
  if ((new_button=calloc(1,sizeof(XWIN)))==NULL) {    
    fprintf(stderr, "No memory for button");
    exit_xdisp(1);
  }

  /* Set hints */
  bhints=XAllocSizeHints();
  bhints->x=x;
  bhints->y=y;
  bhints->width=width;
  bhints->height=height;
  bhints->max_width = DisplayWidth(theDisp,theScreen);
  bhints->max_height = DisplayHeight(theDisp,theScreen);
  bhints->flags |= USSize | PMaxSize | USPosition;

  /* Set attributes */
  xswa.backing_store = Always;
  xswamask = CWBackingStore;    

  /* Initialize button's data and save pointer in new button */
  p_data->action=button_action;
  p_data->action_args=action_data;
  p_data->label = (char *) malloc(strlen(label)+1);
  strcpy(p_data->label,label);
  p_data->x=x;
  p_data->y=y;
  p_data->width=width;
  p_data->height=height;
  p_data->enabled=state;
  new_button->data=p_data;
  new_button->event_handler=button_handler;
  new_button->parent=parent;
  new_button->xid=XCreateSimpleWindow(theDisp,parent,x,y,width,height,
				      0,bgcolor,bgcolor);  
  XSetWMNormalHints(theDisp, new_button->xid, bhints);
  XFree(bhints);
   
  /* save new_button as data assocaited with this window ID and
   * the context "xwin_context"
   */
  if (XSaveContext(theDisp,new_button->xid,xwin_context,
		   (caddr_t) new_button) != 0) {
    fprintf(stderr,"Error saving xwin_context data");
    exit_xdisp(1);
  }
   
  XSelectInput(theDisp, new_button->xid, 
	       ExposureMask |
	       ButtonPressMask |
	       ButtonReleaseMask );
  
  XChangeWindowAttributes(theDisp, new_button->xid, xswamask, &xswa);

  XMapWindow(theDisp,new_button->xid);
  return new_button;
}

/*------------------------ MakeXSlider -------------------------
 *
 * A double labeled slider 
 *
 */

XWIN    *MakeXSlider(
                     int      x, int y,
                     unsigned slider_width, unsigned slider_height,
                     unsigned left_label_width, unsigned right_label_width,
                     unsigned value,
                     ulong    bdcolor, ulong bgcolor,
                     Window   parent,
                     char     *left_label, char *right_label,
                     void     (*slider_action)(void *),
                     void     *action_data)
{
  XWIN	*new_slider;
  D_SLIDER    *p_data;
  XSizeHints  *shints;    
  XSetWindowAttributes 	xswa;
  unsigned int 		xswamask;
    
  /* Allocate slider specifc data */
  if ((p_data=calloc(1,sizeof(D_SLIDER)))==NULL) {
    fprintf(stderr, "No memory for slider's data");
    exit_xdisp(1);    	
  }

  /* Allocate a XWIN structure */
  if ((new_slider=calloc(1,sizeof(XWIN)))==NULL) {    
    fprintf(stderr, "No memory for slider");
    exit_xdisp(1);
  }
  shints=XAllocSizeHints();
  /* Set hints */
  shints->x=x;
  shints->y=y;
  shints->width=slider_width+right_label_width+left_label_width+8;
  shints->height=slider_height;
  shints->max_width = DisplayWidth(theDisp,theScreen);
  shints->max_height = DisplayHeight(theDisp,theScreen);
  shints->flags |= USSize | PMaxSize | USPosition;

  /* Set attributes */
  xswa.backing_store = Always;
  xswamask = CWBackingStore;    

  /* Initialize slider's data and save pointer in new_slider */
  p_data->action=slider_action;
  p_data->action_args=action_data;
  p_data->right_label = (char *) malloc(strlen(right_label)+2);
  p_data->left_label = (char *) malloc(strlen(left_label)+2);
  strcpy(p_data->right_label,right_label);
  strcpy(p_data->left_label,left_label);
  strcat(p_data->right_label,"\0");
  strcat(p_data->left_label,"\0");
  p_data->slider_width=slider_width;
  p_data->slider_height=slider_height;
  p_data->right_label_width=right_label_width;
  p_data->left_label_width=left_label_width;
  p_data->old_value=value;
  p_data->new_value=value;
  p_data->x=x;
  p_data->y=y;
  new_slider->data=p_data;
  new_slider->event_handler=slider_handler;
  new_slider->parent=parent;
  new_slider->xid=XCreateSimpleWindow(theDisp,parent,x,y,
				      shints->width,shints->height,
				      0,bgcolor,bgcolor);  
  XSetWMNormalHints(theDisp, new_slider->xid, shints);
  XFree(shints);
  /* save new_slider as data assocaited with this window ID and
   * the context "xwin_context"
   */
  if (XSaveContext(theDisp,new_slider->xid,xwin_context,
		   (caddr_t) new_slider) != 0) {
    fprintf(stderr,"Error saving xwin_context data");
    exit_xdisp(1);
  }
   
  XSelectInput(theDisp, new_slider->xid, ExposureMask |
	       ButtonPressMask |
	       Button1MotionMask |
	       ButtonReleaseMask );

  XChangeWindowAttributes(theDisp, new_slider->xid, xswamask, &xswa);

  XMapWindow(theDisp,new_slider->xid);
  return new_slider;
}

/*------------------------ button_handler ---------------------*/
static void button_handler(XWIN *p_xwin)
{
  D_BUTTON *p_data=(D_BUTTON *) p_xwin->data;

  /* handle events in this window */
  if(theEvent.xany.window==p_xwin->xid)
    {
      switch(theEvent.type)
	{
	case Expose:
	  if (theEvent.xexpose.count==0)
	    {
	      if (p_data->enabled) 
		draw_button(p_xwin,OLGX_NORMAL);
 	      else 
		draw_button(p_xwin,OLGX_INACTIVE); 
	    }
	  break;
	case ButtonPress: {    /* Call the action routine */
	  if (p_data->enabled) {
	    draw_button(p_xwin,OLGX_INVOKED);
	    XFlush(theDisp);
	    if (p_data->action != NULL)
	      (p_data->action)(p_data->action_args);
	    draw_button(p_xwin,OLGX_NORMAL);
	    XFlush(theDisp);
	  }
	  break;
	}
	case ButtonRelease: {
	  if (p_data->enabled)
	    draw_button(p_xwin,OLGX_NORMAL);
	  XFlush(theDisp);
	  break;
	}
	}
    }
}

/*------------------------ slider_handler ---------------------*/
static void slider_handler(XWIN *p_xwin)
{
  D_SLIDER *p_data=(D_SLIDER *) p_xwin->data;

  /* handle events in this window */
  if(theEvent.xany.window==p_xwin->xid)
    {
      switch(theEvent.type)
	{
	case Expose:
	  if (theEvent.xexpose.count==0) {
	    draw_slider(p_xwin,OLGX_HORIZONTAL);
	  }
	  break;
	case ButtonPress:
	  if (p_data->action != NULL)
	    (p_data->action)(p_data->action_args);
	  draw_slider(p_xwin,OLGX_UPDATE | OLGX_HORIZONTAL);
	  break;
	case MotionNotify:
	  if (p_data->action != NULL)
	    (p_data->action)(p_data->action_args);
	  draw_slider(p_xwin,OLGX_UPDATE | OLGX_HORIZONTAL);
	  break;
	case ButtonRelease: 
	  draw_slider(p_xwin,OLGX_UPDATE | OLGX_HORIZONTAL);
	  break;
	}
    }
}

/*------------------------ draw_button ----------------------------------*/
static void draw_button(XWIN *p_xwin, int state)
{
  D_BUTTON 	*p_data=(D_BUTTON *) p_xwin->data;

  /* draw button */
  olgx_draw_button(ol_ginfo,p_xwin->xid,0,0,
		   p_data->width,0,p_data->label,state);
}

/*------------------------ draw_slider ----------------------------------*/
void draw_slider(XWIN *p_xwin, int state)
{
  D_SLIDER 	*p_data=(D_SLIDER *) p_xwin->data;
  int		label_pos, label_w, max_label_w;

  /* get the label height */
  label_pos = Ascent_of_TextFont(ol_ginfo);
  label_w = XTextWidth(ol_text_finfo,
		       p_data->left_label,
		       strlen(p_data->left_label));
  max_label_w = XTextWidth(ol_text_finfo,"-32767",6);

  /* draw left label */
  XClearArea(theDisp, p_xwin->xid, 0, 0,
	     max_label_w, label_pos, False);
  XDrawImageString(theDisp, p_xwin->xid, theCGC,
		   p_data->left_label_width-label_w, label_pos,
		   p_data->left_label, strlen(p_data->left_label));

  /* draw slider */
  olgx_draw_slider(ol_ginfo, p_xwin->xid, p_data->left_label_width+4,0,
		   p_data->slider_width, p_data->old_value,
		   p_data->new_value, state);

  /* draw right label */
  XDrawImageString(theDisp, p_xwin->xid, theCGC,
		   p_data->left_label_width+4+p_data->slider_width+4, label_pos,
		   p_data->right_label, strlen(p_data->right_label));
}
