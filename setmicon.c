/*
 * 	SetMIcon.c 
 *
 *	Defines image window icon for Xdisp
 *
 *	SetMIcon()
 *
 * 	Copyright (c) B. Pike, 1993-1997
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "xdisp.h"

void SetMIcon(int state)
{
  XWMHints 	xwmh;

  /* create image (Main window) icon window */
  iconW = XCreateSimpleWindow(theDisp, mainW,
			      0, 0, ICON_WIDTH, ICON_HEIGHT, 
			      0, newC[fcol].pixel, newC[bcol].pixel);
  if (!(icon_byte_image = (byte *) malloc(ICON_WIDTH*ICON_HEIGHT*(bitmap_pad/8))))
    FatalError("not enough memory to create image icon");
  iconImage = XCreateImage(theDisp, theVisual, theDepth, ZPixmap, 0, 
                           (char *)icon_byte_image,
			   ICON_WIDTH, ICON_HEIGHT, bitmap_pad, 0);

  /* define the WM attributes */
  xwmh.flags = IconWindowHint |
    StateHint;
  xwmh.icon_window = iconW;
  xwmh.initial_state = state;

  /* set the Wm attribrutes */
  XSetWMHints(theDisp, mainW, &xwmh);
} 
