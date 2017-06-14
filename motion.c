/*
 * 	motion.c
 *
 *	The Motion event handler for Xdisp.
 *
 *	Handle_Motion()
 *
 * 	Copyright Bruce Pike, 1993-2000
 */

#include "xdisp.h"

/*----------------------------- Handle_Motion -----------------------*/
void Handle_Motion(XEvent *event)
{
  int 	pix, i, ind, ix, iy, ml8, ml9, ml10, xp, yp, dw, dh, flag; 
  float	fxp, fyp, rw_pix, rw_coord[MAX_VAR_DIMS],
        fx, fy, tmp1, tmp2, tmp3, tx, ty, tz;
  char  orient_msg[32]="";
  XEvent mEvent;
  

  /* draw profiles if active */
  if (v_profile_active || h_profile_active) {
    Refresh(NULL);
    if (v_profile_active && theEvent.xmotion.x < zWidth) {
      profile_x = theEvent.xmotion.x;
      V_Profile();
    }
    if (h_profile_active && theEvent.xmotion.y < zHeight) {
      profile_y = theEvent.xmotion.y;
      H_Profile();
    }
  }

  /* plot data if plot function is active */
  if (plot_active!=-1) {

    /* draw the roi box on the image */ 
    Refresh(NULL);
    dw = (int)floor(((float)((plot_roi_size*2 + 1)*zWidth) / (float)(Width*2))+0.5);
    dh = (int)floor(((float)((plot_roi_size*2 + 1)*zWidth) / (float)(Width*2))+0.5);
    XDrawLine(theDisp,mainW,grGC, 
              theEvent.xmotion.x-dw, theEvent.xmotion.y-dh,
	      theEvent.xmotion.x+dw, theEvent.xmotion.y-dh);
    XDrawLine(theDisp,mainW,grGC, 
              theEvent.xmotion.x+dw, theEvent.xmotion.y-dh,
	      theEvent.xmotion.x+dw, theEvent.xmotion.y+dh);
    XDrawLine(theDisp,mainW,grGC, 
              theEvent.xmotion.x+dw, theEvent.xmotion.y+dh,
	      theEvent.xmotion.x-dw, theEvent.xmotion.y+dh);
    XDrawLine(theDisp,mainW,grGC, 
              theEvent.xmotion.x-dw, theEvent.xmotion.y+dh,
	      theEvent.xmotion.x-dw, theEvent.xmotion.y-dh);
    

    /* plot the profile */
    Plot_Profile(Plot_Canvas_Widget);

    /* this may have taken a little while so remove all but 
       the last motion event to avoid huge motion event 
       buffers and hence time lags */
    flag=0;
    while (XCheckTypedWindowEvent(theDisp, mainW, MotionNotify, &mEvent))
      {
	flag=1;
      }
    if (flag) XSendEvent(theDisp, mainW, True, PointerMotionMask, &mEvent);
  }

  /* handle selective zooming box */
  if (selzoom_active) {
    draw_roi(roi_x1, roi_y1, roi_x2, roi_y2);
    if ((theEvent.xmotion.x - zWidth/(selzoom_factor_x*2)) < 0) {
      roi_x1 = 0;
      roi_x2 = zWidth/selzoom_factor_x - 1;
    }
    else if ((theEvent.xmotion.x + zWidth/(selzoom_factor_x*2))
	     >= (zWidth-1)) {
      roi_x1 = zWidth - zWidth/selzoom_factor_x;
      roi_x2 = zWidth - 1;
    }
    else {
      roi_x1 = (theEvent.xmotion.x -
		zWidth/(selzoom_factor_x*2));
      roi_x2 = (theEvent.xmotion.x +
		zWidth/(selzoom_factor_x*2) - 1);
    }

    if ((theEvent.xmotion.y - zHeight/(selzoom_factor_y*2))< 0) {
      roi_y1 = 0;
      roi_y2 = zHeight/selzoom_factor_y - 1;
    }
    else if ((theEvent.xmotion.y + zHeight/(selzoom_factor_y*2))
	     >= (zHeight-1)) {
      roi_y1 = zHeight - zHeight/selzoom_factor_y;
      roi_y2 = zHeight-1;
    }
    else {
      roi_y1 = (theEvent.xmotion.y -
		zHeight/(selzoom_factor_y*2));
      roi_y2 = (theEvent.xmotion.y +
		zHeight/(selzoom_factor_y*2) - 1);
    }
    draw_roi(roi_x1, roi_y1, roi_x2, roi_y2);
  }
              
  /* if Button 2 is down we are changing the window and level */
  if (theEvent.xmotion.state==Button2MotionMask)  {
    oLower=Lower; oUpper=Upper;
    Upper=Upper+(oy-theEvent.xmotion.y)*steps;
    Lower=Lower+(oy-theEvent.xmotion.y)*steps;
    Lower=Lower+(ox-theEvent.xmotion.x)*steps;
    Upper=Upper-(ox-theEvent.xmotion.x)*steps;
    if ((oLower<=oUpper) && (Lower<=Upper)) {
      Window_Level(Lower,Upper);
    }
    else if ((oLower>=oUpper) && (Lower>=Upper)) {
      Window_Level(Lower,Upper);
    }
    else if ((oLower<=oUpper) && (Lower>=Upper)) {
      Lower=oLower;
      Upper=Lower+1;
      Window_Level(Lower,Upper);
    }
    else {
      Lower=oLower;
      Upper=Lower-1;
      Window_Level(Lower,Upper);
    }
  }

  /* output the position and pixel values */
  if (theEvent.xmotion.y<zHeight && theEvent.xmotion.y>=0 &&
      theEvent.xmotion.x<zWidth && theEvent.xmotion.x>=0) {
    xp = theEvent.xmotion.x;
    yp = theEvent.xmotion.y;
    /* if not zoomed... simple */
    if (zWidth==Width && zHeight==Height) {
      ind = (theEvent.xmotion.y * Width) +
	theEvent.xmotion.x;
      pix = (long int)short_Image[ind +
				 (load_all_images?image_number*Width*Height:0)];
    }
    else {   /* if zoomed use selected interpolation */
      if (Interpolation_Type==BILINEAR) {
	fx = theEvent.xmotion.x*(Width-1.0)/(zWidth-1.0);
	fy = theEvent.xmotion.y*(Height-1.0)/(zHeight-1.0);
	ix = (int)fx;
	iy = (int)fy;
	ind = (iy * Width) + ix;
	if (ind+1 > (Width*Height-1)) {
	  pix = (long int)short_Image[(Width*Height-1) +
				     (load_all_images?image_number*Width*Height:0)];
	}
	else {
	  tmp1 = short_Image[ind+(load_all_images?
				  image_number*Width*Height:0)]*
	    (1.0-(fx-ix)) + short_Image[ind+1+(load_all_images?
					       image_number*Width*Height:0)]*(fx-ix);
	  if ((ind+Width+(load_all_images?
			  image_number*Width*Height:0)) >
	      (Width*Height-1+(load_all_images?
			       image_number*Width*Height:0))) {
	    tmp2 = short_Image[Width*Height-1+
			      (load_all_images ? 
			       image_number*Width*Height:0)];
	  }
	  else {
	    tmp2 = short_Image[ind+Width+(load_all_images?
					  image_number*Width*Height:0)]*
	      (1.0-(fx-ix)) + 
	      short_Image[ind+Width+1+(load_all_images?
				       image_number*Width*Height:0)]*(fx-ix);
	  }
	  tmp3 = tmp1*(1.0-(fy-iy)) + tmp2*(fy-iy);
	  pix = (long int)(tmp3 + 0.5);
	}
      }
      else {         /* nearest neighbour */
	fx = theEvent.xmotion.x*(Width-1.0)/(zWidth-1.0);
	fy = theEvent.xmotion.y*(Height-1.0)/(zHeight-1.0);
	ix = (int) floor(fx+0.5);
	iy = (int) floor(fy+0.5);
	ind = (iy * Width) + ix;
	if (ind+1 > (Width*Height-1)) {
	  pix = (long int)short_Image[(Width*Height-1) +
				     (load_all_images?image_number*Width*Height:0)];
	}
	else {
	  pix = (long int)short_Image[ind + 
				     (load_all_images?image_number*Width*Height:0)];
	}
      }
    }
  }
  else {
    xp = 0;
    yp = 0;
    pix = 0;
  }
  if ((file_format==ACR_NEMA_FORMAT) ||
      (file_format==MINC_FORMAT) ||
      (Input_Data_Type > SHORT_DATA) ) {
    rw_pix =  (float) (rw_offset + ((float)(pix-I_Min)*rw_scale));
    /* calculate the real world coordinates of cursor if Minc */
    if (file_format==MINC_FORMAT && RW_valid) { 
      if (X_reverse) { /* account for X flipped images */
	fxp = (float)(zWidth-1-theEvent.xmotion.x)*(Width-1.0)/(zWidth-1.0);
      }
      else {
	fxp = (float)(theEvent.xmotion.x)*(Width-1.0)/(zWidth-1.0);
      }
      if (Y_reverse) { /* account for Y flipped images */
	fyp = (float)(zHeight-1-theEvent.xmotion.y)*(Height-1.0)/(zHeight-1.0);
      }
      else {
	fyp = (float)(theEvent.xmotion.y)*(Height-1.0)/(zHeight-1.0);
      }
      /* in plane voxel space coordinates */
      rw_coord[ndimensions-1] = minc_volume_info.start[ndimensions-1] + 
 	                        fxp * minc_volume_info.step[ndimensions-1];
      rw_coord[ndimensions-2] = minc_volume_info.start[ndimensions-2] + 
	                        fyp * minc_volume_info.step[ndimensions-2];
      /* out of plane voxel space coordinates */
      for (i=2; i<ndimensions; i++) {
	rw_coord[ndimensions-(i+1)]= minc_volume_info.start[ndimensions-(i+1)] + 
	  (float) slider_image[ndimensions-(i+1)] * 
	  minc_volume_info.step[ndimensions-(i+1)];
      }

      /* Voxel space to Real World coordinate transformation */
      tx=rw_coord[minc_x_dim];
      ty=rw_coord[minc_y_dim];
      tz=rw_coord[minc_z_dim];
      rw_coord[minc_x_dim] = tx*minc_volume_info.direction_cosines[minc_x_dim][0] +
                             ty*minc_volume_info.direction_cosines[minc_y_dim][0] +
                             tz*minc_volume_info.direction_cosines[minc_z_dim][0]; 
      rw_coord[minc_y_dim] = tx*minc_volume_info.direction_cosines[minc_x_dim][1] +
       	                     ty*minc_volume_info.direction_cosines[minc_y_dim][1] +
	                     tz*minc_volume_info.direction_cosines[minc_z_dim][1];
      rw_coord[minc_z_dim] = tx*minc_volume_info.direction_cosines[minc_x_dim][2] +
	                     ty*minc_volume_info.direction_cosines[minc_y_dim][2] +
	                     tz*minc_volume_info.direction_cosines[minc_z_dim][2];

      /* command window message */
      if (minc_t_dim>=0) { 
	if (minc_volume_info.start[minc_t_dim]==0 && 
	    minc_volume_info.step[minc_t_dim]==0) {
	  sprintf(pix_msg,"        RW x=%g, y=%g, z=%g, frame=%d  Value=%g         ",  
		  rw_coord[minc_x_dim], 
		  rw_coord[minc_y_dim],
		  rw_coord[minc_z_dim],
		  slider_image[minc_t_dim]+1,
		  rw_pix);  
	}
	else {
	  sprintf(pix_msg,"        RW x=%g, y=%g, z=%g, t=%g  Value=%g         ",  
		  rw_coord[minc_x_dim], 
		  rw_coord[minc_y_dim],
		  rw_coord[minc_z_dim],
		  rw_coord[minc_t_dim],
		  rw_pix);  
	}
      } 
      else { 
	sprintf(pix_msg,"        RW x=%g, y=%g, z=%g  Value=%g           ",  
		rw_coord[minc_x_dim], 
		rw_coord[minc_y_dim], 
		rw_coord[minc_z_dim], 
		rw_pix); 
      } 
    }
    else {
      if (num_images==1) {
	if (Input_Data_Type==ULONG_DATA ||
	    Input_Data_Type==USHORT_DATA ) 
	  sprintf(pix_msg,"       pixel %d, %d = %lu      ",
		  xp, yp, (unsigned long) floor(rw_pix+0.5));
	else if (Input_Data_Type==LONG_DATA) 
	  sprintf(pix_msg,"       pixel %d, %d = %ld     ",
		  xp, yp, (long)floor(rw_pix+0.5));
	else
	  sprintf(pix_msg,"       pixel %d, %d = %g      ",
		  xp, yp, rw_pix);
      }
      else {
	if (Input_Data_Type==ULONG_DATA ||
	    Input_Data_Type==USHORT_DATA ) 
	  sprintf(pix_msg,"       image %d, pixel %d, %d = %lu      ",
		  image_number, xp, yp, (unsigned long) floor(rw_pix+0.5));
	else if (Input_Data_Type==LONG_DATA)
	  sprintf(pix_msg,"       image %d, pixel %d, %d = %ld      ",
		  image_number, xp, yp, (long) (floor)(rw_pix+0.5));
	else {
	  sprintf(pix_msg,"       image %d, pixel %d, %d = %g      ", 
		  image_number, xp, yp, rw_pix); 
	}
      }
    }
  }
      
  else {
    if (num_images==1) {
      sprintf(pix_msg,"       pixel %d, %d = %d      ",
	      xp, yp, pix);
    }
    else {
      sprintf(pix_msg,"       image %d, pixel %d, %d = %d      ",
	      image_number, xp, yp, pix);
    }
  }

  /* display the command window position message */
  XDrawImageString(theDisp,cmdW,theCGC,
		   CENTERX(ol_text_finfo,cmd_width/2,pix_msg),
		   (button_height+4)*2-8,pix_msg, strlen(pix_msg));

  if (cmdW_Initial_State==IconicState) { 
    ml8=XTextWidth(ol_text_finfo,loc_msg,strlen(loc_msg))+4;
    ml9=XTextWidth(ol_text_finfo,wl_msg,strlen(wl_msg))+4;
    ml10=XTextWidth(ol_text_finfo,orient_msg,strlen(orient_msg))+4;

    /* prepare the command window location message */
    if (file_format==MINC_FORMAT) {
      sprintf(loc_msg,"(%d,%d",xp,yp);
      for (i=2; i<ndimensions; i++) {
	sprintf(loc_msg,"%s,%d",loc_msg,slider_image[ndimensions-(i+1)]+1);
      }
      sprintf(loc_msg,"%s) = %g",loc_msg,rw_pix);
    }
    else if ((file_format==ACR_NEMA_FORMAT) ||
	(Input_Data_Type > SHORT_DATA) ) {
      if (num_images>1) {
	if (Input_Data_Type==ULONG_DATA ||
	    Input_Data_Type==USHORT_DATA ) 
	  sprintf(loc_msg,"(%d,%d,%d) = %lu",
		  xp,yp,image_number+1,
		  (unsigned long)floor(rw_pix+0.5));
	else if (Input_Data_Type==LONG_DATA)
	  sprintf(loc_msg,"(%d,%d,%d) = %ld",
		  xp,yp,image_number+1,
		  (long)floor(rw_pix+0.5));
	else
	  sprintf(loc_msg,"(%d,%d,%d) = %g",
		  xp,yp,image_number+1,rw_pix);
      }
      else {
	if (Input_Data_Type==ULONG_DATA ||
	    Input_Data_Type==USHORT_DATA ) 
	  sprintf(loc_msg,"(%d,%d) = %lu",
		  xp,yp,(unsigned long) floor(rw_pix+0.5));
	else if (Input_Data_Type==LONG_DATA)
	  sprintf(loc_msg,"(%d,%d) = %ld",
		  xp,yp,(long) floor(rw_pix+0.5));
	else
	  sprintf(loc_msg,"(%d,%d) = %g",
		  xp,yp,rw_pix);
      }
    }
    else {
      if (num_images>1) {
	sprintf(loc_msg,"(%d,%d,%d) = %d",
		xp,yp,image_number+1,pix);
      }
      else {
	sprintf(loc_msg,"(%d,%d) = %d",xp,yp,pix);
      }
    }

    /* clear are of window if needed */
    if (ml8 > XTextWidth(ol_text_finfo,loc_msg,strlen(loc_msg))+4) {
      XClearArea(theDisp, mainW, 0, zHeight, 
		 zWidth,zHeight+info_height, False);
    }

    /* if we have valid MINC info display orientation information */
    if (file_format==MINC_FORMAT && RW_valid ) { 
      /* screen x direction */
      if (ndimensions-1==minc_x_dim) {
	if (X_reverse) 
	  strcpy(orient_msg,"R");
	else
	  strcpy(orient_msg,"L");
      }
      else if (ndimensions-1==minc_y_dim) {
	if (X_reverse) 
	  strcpy(orient_msg,"A");
	else
	  strcpy(orient_msg,"P");
      }
      else if (ndimensions-1==minc_z_dim) {
	if (X_reverse) 
	  strcpy(orient_msg,"S");
	else
	  strcpy(orient_msg,"I");
      }
      else {
	  strcpy(orient_msg,"?");
      }
      

      /* screen y direction */
      if (ndimensions-2==minc_x_dim) {
	if (Y_reverse) 
	  strcat(orient_msg,"-R");
	else
	  strcat(orient_msg,"-L");
      }
      else if (ndimensions-2==minc_y_dim) {
	if (Y_reverse) 
	  strcat(orient_msg,"-A");
	else
	  strcat(orient_msg,"-P");
      }
      else if (ndimensions-2==minc_z_dim) {
	if (Y_reverse) 
	  strcat(orient_msg,"-S");
	else
	  strcat(orient_msg,"-I");
      }
      else {
	  strcat(orient_msg,"-?");
      }

      /* actually display the orientation message */
      if (!(wl_display ||(theEvent.xmotion.state==Button2MotionMask))) {
	if (ml10 > XTextWidth(ol_text_finfo,orient_msg,strlen(orient_msg))+4) {
	  XClearArea(theDisp, mainW, zWidth-ml10-4,
		     zHeight, zWidth, zHeight+info_height, False);
	}
	XDrawImageString(theDisp,mainW,theMGC,
			 zWidth-XTextWidth(ol_text_finfo, orient_msg,
					   strlen(orient_msg))-4,
			 zHeight+info_height-4,orient_msg,strlen(orient_msg));
      }
    }


    /* prepare the command window window/level message */
    sprintf(wl_msg,"%d W  %d L  %s",(Upper-Lower),(Upper+Lower)/2, orient_msg);
    if (wl_display ||(theEvent.xmotion.state==Button2MotionMask)) {
      if (ml9 > XTextWidth(ol_text_finfo,wl_msg,strlen(wl_msg))+4) {
	XClearArea(theDisp, mainW, zWidth-ml9-4,
		   zHeight, zWidth, zHeight+info_height, False);
      }
      /* display the main window window/level message */
      XDrawImageString(theDisp,mainW,theMGC,
		       zWidth-XTextWidth(ol_text_finfo,wl_msg,
					 strlen(wl_msg))-4,
		       zHeight+info_height-4,wl_msg,strlen(wl_msg));
    }

    /* display the main window location message */
    XDrawImageString(theDisp,mainW,theMGC,4,
		     zHeight+info_height-4,loc_msg,strlen(loc_msg));
  } 

  /* set the old x and y locations to the current values */
  ox = theEvent.xmotion.x;
  oy = theEvent.xmotion.y;

  /* draw the roi box if active */
  if (roi_active) {
    draw_roi(roi_x1, roi_y1, roi_x2, roi_y2);
    roi_x2 = ox < zWidth ? (ox>0 ? ox : 0) : zWidth-1;
    roi_y2 = oy < zHeight ? (oy>0 ? oy : 0) : zHeight-1;
    draw_roi(roi_x1, roi_y1, roi_x2, roi_y2);
  }
}


