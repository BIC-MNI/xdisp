/*
 * 	profile.c
 *
 *	Profiling routine for Xdisp.
 *
 *	H_Profile()
 *	V_Profile()
 *	H_Profile_Switch()
 *	V_Profile_Switch()
 *
 * 	Copyright (c) Bruce Pike 1993-1997
 *
 */

#include "xdisp.h"
#include <sys/stat.h>

/*----------------------- H_Profile -----------------------------*/
void H_Profile(void)
{
  int		i;
  float	xscale, yscale;
  int		xp[4096], yp[4096];

  /* draw a line where the profile was taken */
  XDrawLine(theDisp, mainW, grGC, 0, profile_y, zWidth-1, profile_y);

  /* set scale value */
  xscale = (float)(zWidth-1)/(float)(Width-1);
  yscale = 0.5 * (float)(zHeight-1)/(float)(I_Max-I_Min);

  /* correct for any zoom */
  profile_y = (int)( (float)profile_y * 
		     (float)(Height-1.0)/(float)(zHeight-1.0) + 0.5);

  /* get profile  */
  for (i=0; i<Width; i++) {
    xp[i] = (int)(xscale * (float)i);
    yp[i] = zHeight-(int)(abs ((short_Image[profile_y*Width + i]-I_Min) * 
			       yscale));
    if(Verbose) fprintf(stderr,"xp,yp=%d,%d\n",xp[i],yp[i]);
  }

  /* draw the profile */    
  for (i=0; i<(Width-1); i++) {
    XDrawLine(theDisp, mainW, grGC, xp[i], yp[i], xp[i+1], yp[i+1]);
  }

  /* clear all but last extra motion events */
  i = 0;
  while(XCheckTypedEvent(theDisp,MotionNotify,&theEvent)) i++;
  if (i>0) XPutBackEvent(theDisp,&theEvent);
}
    
/*----------------------- V_Profile -----------------------------*/
void V_Profile(void)
{
  int		i;
  float	xscale, yscale;
  int		xp[4096], yp[4096];

  /* draw a line where the profile was taken */
  XDrawLine(theDisp, mainW, grGC, profile_x, 0, profile_x, zHeight-1);

  /* set scale value */
  yscale = (float)(zHeight-1)/(float)(Height-1);
  xscale = 0.5 * (float)(zWidth-1)/(float)(I_Max-I_Min);

  /* correct for any zoom */
  profile_x = (int)( (float)profile_x * 
		     (float)(Width-1)/(float)(zWidth-1)+0.5);

  /* get profile */
  for (i=0; i<Height; i++) {
    yp[i] = (int)(yscale * (float)i);
    xp[i] = (int)(abs ((short_Image[i*Width + profile_x]-I_Min) * xscale));
    if(Verbose) fprintf(stderr,"xp,y%d,%d\n",xp[i],yp[i]);
  }

  /* draw the profile */    
  for (i=0; i<(Height-1); i++) {
    XDrawLine(theDisp, mainW, grGC, xp[i], yp[i], xp[i+1], yp[i+1]);
  }

  /* clear all but last extra motion events */
  i = 0;
  while(XCheckTypedEvent(theDisp,MotionNotify,&theEvent)) i++;
  if (i>0) XPutBackEvent(theDisp,&theEvent);
}    

/*--------------------- H_Profile_Switch ------------------------*/
void H_Profile_Switch(void *data)
{
  h_profile_active = h_profile_active==0 ? 1 : 0;
}

/*--------------------- V_Profile_Switch ------------------------*/
void V_Profile_Switch(void *data)
{
  v_profile_active = v_profile_active==0 ? 1 : 0;
}
