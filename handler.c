/*
 * 	handler.c
 *
 *	The main event handling loop for Xdisp.
 *
 *	HandleEvent()
 *
 * 	Copyright Bruce Pike, 1993-2000
 */

#include "xdisp.h"

/*---------------------------- HandleEvent --------------------------*/
void HandleEvent(XEvent *event)
{
  if(event->xany.window==mainW) {

    switch (event->type) {
    case Expose: 
 
      if (event->xexpose.count==0) {
	DrawWindow(event->xexpose.x,event->xexpose.y,
		   event->xexpose.width, event->xexpose.height);
      }
    
    break;

    case EnterNotify: {
      if (Selected_Visual_Class==DirectColor)
	XStoreColors(theDisp,NewCmap,newC,ColorMapSize);
      if (Selected_Visual_Class!=TrueColor)
	XInstallColormap(theDisp,NewCmap);
    }
    break;
    
    case LeaveNotify: {
      if (Selected_Visual_Class==DirectColor)
	XStoreColors(theDisp,NewCmap,rootC,ColorMapSize); 
    }
    break;

    case KeyPress: {	/* allow a quit from the image window */
      Handle_KeyPress(event);
    }
    break;

    case MotionNotify: {
      Handle_Motion(event);
    }
    break;

    case ConfigureNotify: {
      XConfigureEvent *conf_event = (XConfigureEvent *) event;

      if (conf_event->window == cmdW && 
	  (conf_event->width != cmd_width ||
	   conf_event->height != cmd_height))
	XResizeWindow(theDisp,cmdW,cmd_width,cmd_height);
      if (conf_event->window == mainW && 
	  (conf_event->width != zWidth+(color_bar?color_bar_width:0) ||
	   conf_event->height != zHeight+info_height))
	Resize(conf_event->width-(color_bar?color_bar_width:0),
	       conf_event->height-info_height);
    }
    break;

    case ButtonPress: {
      if (event->xbutton.button==Button1) {
	if (selzoom_active) {
	  Selective_Zoom((caddr_t)&one);
	  break;
	}
	XDefineCursor(theDisp,mainW,roiCursor);
	XFlush(theDisp);
	if (roi_present) draw_roi(roi_x1, roi_y1, roi_x2, roi_y2);
	roi_active = 1;
	roi_x1 = event->xmotion.x < zWidth ?
	  (event->xmotion.x>0 ?
	   event->xmotion.x : 0) : zWidth-1;
	roi_y1 = event->xmotion.y < zHeight ?
	  (event->xmotion.y>0 ?
	   event->xmotion.y : 0) : zHeight-1;
	roi_x2 = roi_x1;
	roi_y2 = roi_y1;
	draw_roi(roi_x1, roi_y1, roi_x2, roi_y2);
	break;
      } 
      if (event->xbutton.button==Button2) {
	XDefineCursor(theDisp,mainW,wlCursor);
	XClearArea(theDisp,mainW,0,zHeight,
		   zWidth,zHeight+info_height,False);
	XDrawImageString(theDisp,mainW,theMGC,
			 zWidth-XTextWidth(ol_text_finfo,
					   wl_msg,strlen(wl_msg))-4,
			 zHeight+info_height-4,wl_msg,strlen(wl_msg));
	XDrawImageString(theDisp,mainW,theMGC,4,
			 zHeight+info_height-4,loc_msg,strlen(loc_msg));
	XFlush(theDisp);
	break;
      } 
      if (event->xbutton.button==Button3) {
	/* note: not really 'iconic' anymore, simply map or unmap */
	if (cmdW_State==NormalState) {
	  XUnmapWindow(theDisp, cmdW);
	} else {
	  XMapRaised(theDisp, cmdW);
          XMapRaised(theDisp, cmdW);/*VF: don't understand why I need to events! Maybe somebody is eating up an extra event from the queue*/
	}
	break;
      } 
    }
    break;

    case ButtonRelease: {
      int xa, xb, ya, yb;
      if (event->xbutton.button==Button1 && roi_active) {
	roi_active = 0;
	xa = roi_x1*(Width-1)/(zWidth-1);
	xb = roi_x2*(Width-1)/(zWidth-1);
	ya = roi_y1*(Height-1)/(zHeight-1);
	yb = roi_y2*(Height-1)/(zHeight-1);
	roi_stats(xa, ya, xb, yb, 0);
	XDefineCursor(theDisp,mainW,mainCursor);
	XFlush(theDisp);
      }
      if (event->xbutton.button==Button2) {
	XDefineCursor(theDisp,mainW,mainCursor);
	XFlush(theDisp);
	if (Scale_Data && (oUpper != Upper || oLower != Lower)) {
	  Rescale(NULL);
	  Window_Level(Lower,Upper); 
	  oLower=Lower; oUpper=Upper; 
	}
	XClearArea(theDisp,mainW,0,zHeight,
		   zWidth,zHeight+info_height,False);
	if (wl_display) 
	  XDrawImageString(theDisp,mainW,theMGC,
			   zWidth-XTextWidth(ol_text_finfo,wl_msg,strlen(wl_msg))-4,
			   zHeight+info_height-4,wl_msg,strlen(wl_msg));
	XDrawImageString(theDisp,mainW,theMGC,4,
			 zHeight+info_height-4,loc_msg,strlen(loc_msg));
	update_sliders();
	update_msgs();
      }
    }
    break;

    case UnmapNotify: {
      if (bitmap_pad==8) {
	if (Interpolation_Type==BILINEAR) {
	  bilinear_byte_to_byte(byte_Image, Width, Height,
				icon_byte_image, ICON_WIDTH, ICON_HEIGHT);
	}
	else {
	  nneighbour_byte_to_byte(byte_Image, Width, Height,
				  icon_byte_image, ICON_WIDTH, ICON_HEIGHT);
	}
      }
      else {
	if (Interpolation_Type==BILINEAR) {
	  bilinear_rgbp_to_rgbp(byte_Image, Width, Height,
				icon_byte_image, ICON_WIDTH, ICON_HEIGHT);
	}
	else {
	  nneighbour_rgbp_to_rgbp((uint32_t *) byte_Image, Width, Height,
				  (uint32_t *) icon_byte_image, ICON_WIDTH, ICON_HEIGHT);
	}
      }
      XPutImage(theDisp,iconW,theMGC,iconImage,0,0,0,0,
		ICON_WIDTH, ICON_HEIGHT);
    }
    break;
                
    case CirculateNotify:
        break;
    case MapNotify:
        break;
    case DestroyNotify:
    case GravityNotify:
    case ReparentNotify:
    default: 
    break;
    }  /* end of switch */
  } else if(event->xany.window==cmdW) {

    switch (event->type) {
    case Expose:
      if (event->xexpose.count==0)
              update_msgs();
      /*printf("Should expose cmdW\n");*/
    break;

    case EnterNotify: 
      if (Selected_Visual_Class==DirectColor)
	XStoreColors(theDisp,NewCmap,newC,ColorMapSize); 
      if (Selected_Visual_Class!=TrueColor)
	XInstallColormap(theDisp,NewCmap);
      
    break;

    case LeaveNotify: 
      if (Selected_Visual_Class==DirectColor)
	XStoreColors(theDisp,NewCmap,rootC,ColorMapSize); 
    break;

    case UnmapNotify: 
      cmdW_State = IconicState;
      break;
            
    case MapNotify: 
      cmdW_State = NormalState;
      break;

    case ConfigureNotify: 
      if( event->xconfigure.window == cmdW && 
         (event->xconfigure.width != cmd_width ||
          event->xconfigure.height != cmd_height) )
        XResizeWindow(theDisp,cmdW,cmd_width,cmd_height);
    break;

    default: break;
    }
  }
  /* button press events */
  if (XFindContext(theDisp, event->xany.window, xwin_context,
		   (caddr_t *) &which_xwin)==0) {
    if(*(which_xwin->event_handler)!=NULL)
      (*(which_xwin->event_handler))(which_xwin);
    if (Scale_Data && (oUpper != Upper || oLower != Lower)) {
      Rescale(NULL);
      oLower=Lower; oUpper=Upper;
    }
  }
}

