/*
 * 	SetCIcon.c 
 *
 *	Defines command window icon for Xdisp
 *
 *	SetCIcon()
 *
 * 	Copyright (c) B. Pike, 1993-1997
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "xdcicon.bit"
#include "xdcmask.bit"

void SetCIcon(Display *dsp, Window wnd, int state)
{

  Pixmap 	 icon_bmp, icon_mask_bmp;
  XWMHints 	 xwmh;

  /* create icon bitmap */    
  if ((icon_bmp=XCreateBitmapFromData(dsp,wnd,
				      xdcicon_bits,
				      xdcicon_width,
				      xdcicon_height))==None)
    fprintf(stderr,"Error on icon creation\n");

  /* create mask bitmap */
  if ((icon_mask_bmp=XCreateBitmapFromData(dsp,wnd,
					   xdcmask_bits,
					   xdcmask_width,
					   xdcmask_height))==None)
    fprintf(stderr,"Error on icon mask creation\n");

  /* define the WM attributes */
  xwmh.flags = IconPixmapHint |
    IconMaskHint |
    StateHint ;
  xwmh.icon_pixmap = icon_bmp;
  xwmh.icon_mask   = icon_mask_bmp;
  xwmh.initial_state = state;

  /* set the Wm attribrutes */
  XSetWMHints(dsp,wnd,&xwmh);
} 
