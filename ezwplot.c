/*
 * 	ezplot.c
 *
 *	Routines related to the EZ Plot widget for xdisp
 *      (imlemented using EZwgl).
 *
 *      Initialize_Plot_Widget()
 *      Open_Plot_Widget()
 *	Close_Plot_Widget()
 *      Plot_Profile()
 *      Plot_Type_Switch()
 *      Plot_Style_Switch()
 *      ROI_Size_Switch()
 *      Draw_Plot_Axis()
 *
 * 	Copyright Bruce Pike, 1993-2000
 */

#include "xdisp.h"
#define PLOT_WIDGET_WIDTH 750
#define PLOT_WIDGET_HEIGHT 600

/*-------------------- Initialize_Plot_Widget() ---------------------*/
void Initialize_Plot_Widget() 
{
  EZ_Widget    *tmp_w1, *tmp_w2, *tmp_w3, *tmp_w4, *tmp_w5;
  int          i;

  /* create and configure the plot widget */
  Plot_Widget = EZ_CreateFrame(NULL, fname);
  EZ_ConfigureWidget(Plot_Widget,
		     EZ_PADX,         20,
		     EZ_PADY,         10,
		     EZ_STACKING,     EZ_VERTICAL,
		     EZ_WIDTH,        PLOT_WIDGET_WIDTH, 
                     EZ_HEIGHT,       PLOT_WIDGET_HEIGHT,
		     EZ_FILL_MODE,    EZ_FILL_BOTH, 
		     EZ_LABEL_STRING, "Xdisp - Plot Window",
		     0);

  /* create and configure the plot canvas */
  Plot_Canvas_Widget = EZ_Create3DCanvas(Plot_Widget);
  EZ_SetVectorFont(EZ_SMALL_VECTOR_FONT);
  

  /* create and configure the buttons */
  tmp_w1 = EZ_CreateFrame(Plot_Widget, NULL);
  EZ_ConfigureWidget(tmp_w1,
		     EZ_PADX,         4,
		     EZ_PADY,         0,
		     EZ_EXPAND,       True,
		     EZ_FILL_MODE,    EZ_FILL_BOTH, 
		     EZ_HEIGHT,       0,
		     EZ_STACKING,     EZ_HORIZONTAL_CENTER,
		     0);

  tmp_w2 = EZ_CreateFrame(tmp_w1, NULL);
  EZ_ConfigureWidget(tmp_w2,
		     EZ_PADX,         0,
		     EZ_PADY,         2,
		     EZ_FILL_MODE,    EZ_FILL_NONE, 
                     EZ_LABEL_STRING, "Dimension",
		     EZ_STACKING,     EZ_HORIZONTAL_LEFT,
		     0);

  /* add dimension buttons and callbacks */
  for (i=0; i<ndimensions; i++) {
    if (Verbose) fprintf(stderr,"Found Dimension: %s\n",
			 minc_volume_info.dimension_names[i]);
    if (!strncmp(minc_volume_info.dimension_names[i],"xspace",6)) {
      Plot_Button[i] = EZ_CreateRadioButton(tmp_w2, "X", -1, 0, i);
      EZ_AddWidgetCallBack(Plot_Button[i],
			   EZ_CALLBACK, Plot_Type_Switch, NULL, 0);
      EZ_SetRadioButtonGroupVariableValue(Plot_Button[i],i);
    }
    
    if (!strncmp(minc_volume_info.dimension_names[i],"yspace",6)) {
      Plot_Button[i] = EZ_CreateRadioButton(tmp_w2, "Y", -1, 0, i);
      EZ_AddWidgetCallBack(Plot_Button[i],
			   EZ_CALLBACK, Plot_Type_Switch, NULL, 0);
      EZ_SetRadioButtonGroupVariableValue(Plot_Button[i],i);
    }
    
    if (!strncmp(minc_volume_info.dimension_names[i],"zspace",6)) {
      Plot_Button[i] = EZ_CreateRadioButton(tmp_w2, "Z", -1, 0, i); 
      EZ_AddWidgetCallBack(Plot_Button[i],
			   EZ_CALLBACK, Plot_Type_Switch, NULL, 0);
      EZ_SetRadioButtonGroupVariableValue(Plot_Button[i],i);
    }
    
    if ((!strncmp(minc_volume_info.dimension_names[i],"time",4)) ||
        (!strncmp(minc_volume_info.dimension_names[i],"tspace",6))) {
      Plot_Button[i] = EZ_CreateRadioButton(tmp_w2, "T", -1, 0, i);
      EZ_AddWidgetCallBack(Plot_Button[i],
			   EZ_CALLBACK, Plot_Type_Switch, NULL, 0);
      EZ_SetRadioButtonGroupVariableValue(Plot_Button[i],i);
    }
  }

  /* create another holder for the plot format buttons */
  tmp_w3 = EZ_CreateFrame(tmp_w1, NULL);
  EZ_ConfigureWidget(tmp_w3,
		     EZ_PADX,         0,
		     EZ_PADY,         2,
		     EZ_FILL_MODE,    EZ_FILL_NONE, 
		     EZ_EXPAND,       False,
		     EZ_HEIGHT,       0,
		     EZ_STACKING,     EZ_HORIZONTAL_CENTER,
		     EZ_LABEL_STRING, "Plot Type",
                     EZ_CENTER_ALIGNED,
		     0);
  Raw_Plot_Button = EZ_CreateRadioButton(tmp_w3, "Raw", -1, 1, 1);
  EZ_AddWidgetCallBack(Raw_Plot_Button, 
		       EZ_CALLBACK, Plot_Style_Switch, NULL, 0);
  Normalized_Plot_Button = EZ_CreateRadioButton(tmp_w3, "Norm.", -1, 1, 2);
  EZ_AddWidgetCallBack(Normalized_Plot_Button, 
		       EZ_CALLBACK, Plot_Style_Switch, NULL, 0);
  Percent_Plot_Button = EZ_CreateRadioButton(tmp_w3, "%", -1, 1, 3); 
  EZ_AddWidgetCallBack(Percent_Plot_Button, 
		       EZ_CALLBACK, Plot_Style_Switch, NULL, 0);
  EZ_SetRadioButtonGroupVariableValue(Percent_Plot_Button,plot_style);

  /* create another holder for the plot_roi_size buttons */
  tmp_w4 = EZ_CreateFrame(tmp_w1, NULL);
  EZ_ConfigureWidget(tmp_w4,
		     EZ_PADX,         0,
		     EZ_PADY,         2,
		     EZ_FILL_MODE,    EZ_FILL_NONE, 
		     EZ_EXPAND,       False,
		     EZ_HEIGHT,       0,
		     EZ_STACKING,     EZ_HORIZONTAL_CENTER,
		     EZ_LABEL_STRING, "ROI Size",
                     EZ_CENTER_ALIGNED,
		     0);

  ROI_Size_Button[0] = EZ_CreateRadioButton(tmp_w4, "1x1", -1, 2, 0);
  EZ_AddWidgetCallBack(ROI_Size_Button[0], 
		       EZ_CALLBACK, ROI_Size_Switch, NULL, 0);
  ROI_Size_Button[1] = EZ_CreateRadioButton(tmp_w4, "3x3", -1, 2, 1);
  EZ_AddWidgetCallBack(ROI_Size_Button[1], 
		       EZ_CALLBACK, ROI_Size_Switch, NULL, 0);
  ROI_Size_Button[2] = EZ_CreateRadioButton(tmp_w4, "5x5", -1, 2, 2); 
  EZ_AddWidgetCallBack(ROI_Size_Button[2], 
		       EZ_CALLBACK, ROI_Size_Switch, NULL, 0);
  EZ_SetRadioButtonGroupVariableValue(ROI_Size_Button[0], plot_roi_size);
  

  /* create another holder for the plot close button */
  tmp_w5 = EZ_CreateFrame(tmp_w1, NULL);
  EZ_ConfigureWidget(tmp_w5,
		     EZ_PADX,         8,
		     EZ_PADY,         2,
		     EZ_FILL_MODE,    EZ_FILL_NONE, 
		     EZ_EXPAND,       False,
		     EZ_HEIGHT,       0,
		     EZ_STACKING,     EZ_HORIZONTAL_RIGHT,
		     0);

  /* create and configure close button */
  Plot_Close_Button = EZ_CreateButton(tmp_w5,"Close",-1);
  EZ_AddWidgetCallBack(Plot_Close_Button,
		       EZ_CALLBACK, Close_Plot_Widget, NULL, 0);

  /* set WM hints for user placement */
  EZ_SetWMHintsAndSizeHints(File_Save_Widget, 0);
}


/*---------------------- Open_Plot_Widget() ------------------------*/
void Open_Plot_Widget(void *data)
{
  /* First check a few basic requirements */
  /* make sure we have a MINC file with some valid dimensions */
  if (file_format!=MINC_FORMAT) {
    EZW_Error("Error: This function only works with a MINC file!");
    Close_Plot_Widget(NULL, NULL);
    return;
  }

  /* make sure we have a MINC file with some valid dimensions */
  if ((ndimensions<2) || (ndimensions>4)) {
    EZW_Error("Error: This function only supports MINC files of 3 or 4 dimensions");
    Close_Plot_Widget(NULL, NULL);
    return;
  }

  /* make sure we have not rotated or reoriented the MINC file */
  if (!RW_valid) {
    EZW_Error("Error: This function only works with a non rotated/reoriented MINC file!");
    Close_Plot_Widget(NULL, NULL);
    return;
  }

  /* activate and display the plot widget */
  EZ_ActivateWidget(Plot_Widget); 
  EZ_DisplayWidget(Plot_Widget); 

  /* set the plot_active state */
  plot_active = EZ_GetRadioButtonGroupVariableValue(Plot_Button[0]);
  if (Verbose) fprintf(stderr,"Opening Plot Window with plot_active = %d\n",
		       plot_active);
  
  /* now setup GL attributes */
  EZ_DisplayWidget(Plot_Canvas_Widget);
  EZ_RGBMode();                 /* we are using RGB mode  */
  EZ_AutoSelectBackBuffer();    /* select a back buffer   */
  EZ_DrawBuffer(ez_buffer_mode == 0 ? EZ_FRONT: EZ_BACK);

  /* We'll draw a 2D polygon, so we don't have to setup
   * a complex projection matrix. The default matrix mode
   * is EZ_MODELVIEW. We just set the projection matrix
   * on this default matrix stack.
   */
  EZ_LoadIdentity();                /* clear the top mtx 2 id   */     
  EZ_Ortho2(-0.20,1.05,-0.10,1.05); /* establish the plot scale */
  EZ_Clear(EZ_COLOR_BUFFER_BIT);    /* clear the frame buffer   */
  EZ_Color3f(1.0,1.0,0.0);
 
  /* force the title we want */
  XStoreName(theDisp,EZ_GetWidgetWindow(Plot_Widget),window_name);
  Refresh(NULL);
  Plot_Profile(Plot_Canvas_Widget);
}

/*-------------------- Close_Plot_Widget() --------------------------*/
void Close_Plot_Widget(void *object, void *data)
{
  /* deactivate the plot_active switch */
  plot_active = -1;

  /* close the window */
  EZ_DeActivateWidget(Plot_Widget);
}

/*--------------------- Plot_Profile() ---------------------------*/
void Plot_Profile(EZ_Widget *Plot_Surface)
{
  int		i, j, n[5][5], k[MAX_VAR_DIMS], ind[MAX_VAR_DIMS],
                ni, nj, size[MAX_VAR_DIMS],  pa_d;
  long          count[MAX_VAR_DIMS], start[MAX_VAR_DIMS];
  short         roi_data[5][5];
  float	        fx, fy, sum, mean, min, max, tx, ty, tz;
  float		xp[4096], yp[4096], yl_min, yl_max;
  float         rw_start[MAX_VAR_DIMS], 
                rw_end[MAX_VAR_DIMS];
  char          xlabel[32], ylabel[32], title[128];

  /* make sure we have not rotated or reoriented the MINC file */
  if (!RW_valid) {
    EZW_Error("Error: This function only works with a non rotated/reoriented MINC file!");
    Close_Plot_Widget(NULL, NULL);
    return;
  }

  /* must reverse the counting order for plot active since 
     minc orders diemsnions nested in and not out 
     (e.g. t,z,y,x and not x,y,z,t) */
  pa_d = (ndimensions-1) - plot_active;
  
  /* get current short_image matrix coordinates and size */
  fx = (float)theEvent.xmotion.x*((float)Width-1.0)/((float)zWidth-1.0);
  ind[0] = (int) floor(fx+0.5);
  size[0] = Width;
  fy = (float)theEvent.xmotion.y*((float)Height-1.0)/((float)zHeight-1.0);
  ind[1] = (int) floor(fy+0.5);
  size[1] = Height;
  for (i=2; i<ndimensions; i++) {
    ind[i] = slider_image[ndimensions-(i+1)];
    size[i] = slider_length[ndimensions-(i+1)];
  }
  

  /* get profile data */
  sum = 0; 
  min = (float)SHRT_MAX;
  max = (float)SHRT_MIN;
  for (i=0; i<size[pa_d]; i++) {
    for (j=0; j<ndimensions; j++) {
      k[j] = (j==pa_d ? i : ind[j]);
    }
    /* x plot value */
    xp[i] = ((float)i)/((float)size[pa_d]);

    if (load_all_images) { /* get the data directly from memory */
      /* y plot value */
      if (ndimensions==2) {
	for (ni=0; ni<(2*plot_roi_size)+1; ni++) 
	  for (nj=0; nj<(2*plot_roi_size)+1; nj++)
	    n[ni][nj] = MINMAX(k[0]-(ni-plot_roi_size),0,size[0]-1) + 
	      (MINMAX(k[1]-(nj-plot_roi_size),0,size[1]-1) * 
	       size[0]);
      }
      else if (ndimensions==3) {
	for (ni=0; ni<(2*plot_roi_size)+1; ni++) 
	  for (nj=0; nj<(2*plot_roi_size)+1; nj++)
	    n[ni][nj] = MINMAX(k[0]-(ni-plot_roi_size),0,size[0]-1) + 
	      (MINMAX(k[1]-(nj-plot_roi_size),0,size[1]-1) * 
	       size[0]) + 	
	      (k[2] * size[0] * size[1]);
      }
      else if (ndimensions==4) {
	for (ni=0; ni<(2*plot_roi_size)+1; ni++) 
	  for (nj=0; nj<(2*plot_roi_size)+1; nj++)
	    n[ni][nj] = MINMAX(k[0]-(ni-plot_roi_size),0,size[0]-1) + 
	      (MINMAX(k[1]-(nj-plot_roi_size),0,size[1]-1) * 
	       size[0]) + 	
	      (k[2] * size[0] * size[1]) +
	      (k[3] * size[0] * size[1] * size[2]);
      }
      
      else {
	EZW_Error("Sorry: these minc files are not supported by the plot function.");
	Close_Plot_Widget(NULL, NULL);
	return;
      }
      yp[i] = 0.0;
      for (ni=0; ni<(2*plot_roi_size)+1; ni++) 
	for (nj=0; nj<(2*plot_roi_size)+1; nj++)
	  yp[i] = yp[i] + (float)(rw_offset + 
				  ((float)(short_Image[n[ni][nj]]-I_Min)*rw_scale)) /
	    (float)( ((2*plot_roi_size)+1) * ((2*plot_roi_size)+1) ); 
      sum = sum + yp[i];
      if (yp[i]<min) min=yp[i];
      if (yp[i]>max) max=yp[i];
    }
    else {  /* must read all the needed data from file */
      for (ni=0; ni<ndimensions; ni++) {
        if (ni>ndimensions-3) {
	  start[ni] = (long)(MINMAX(k[ndimensions-1-ni]-plot_roi_size, 
                      0, size[ndimensions-1-ni]-1-(2*plot_roi_size)));
	  count[ni] = (long)((2 * plot_roi_size) + 1);
	}
	else {
	  start[ni]=(long)(MINMAX(k[ndimensions-1-ni], 0, size[ndimensions-1-ni]-1));
	  count[ni]=1;
	}
      }
      miicv_get(minc_icvid,start,count,&roi_data);
      yp[i]=0.0;
      for (ni=0; ni<(2*plot_roi_size)+1; ni++) 
	for (nj=0; nj<(2*plot_roi_size)+1; nj++) {
	  yp[i] = yp[i] + (float)(rw_offset + 
		 ((float)(roi_data[ni][nj]-I_Min)*rw_scale)) /
	         (float)( ((2*plot_roi_size)+1) * ((2*plot_roi_size)+1) ); 
	}
      sum = sum + yp[i];
      if (yp[i]<min) min=yp[i];
      if (yp[i]>max) max=yp[i];
    }
  }
  mean = sum / (float)size[pa_d];
  
  /* start plotting */
  EZ_DrawBuffer(ez_buffer_mode == 0 ? EZ_FRONT: EZ_BACK);

  /* clear the frame buffer */
  EZ_Clear(EZ_COLOR_BUFFER_BIT); 

  /* draw the axis and calculate normalized values */
  switch (plot_style) {
  case 1:  {
    strcpy(ylabel,"R.W.");
    strcpy(title,"Real World Pixel Values vs ");
    yl_min = rw_offset;
    yl_max = rw_offset + ((float)(I_Max-I_Min)*rw_scale);
    if ((yl_max-yl_min)==0.0) {  /* divide by zero case */
      for (i=0; i<size[pa_d]; i++) {
	yp[i] = 0.0;
      }
    }
    else {
      for (i=0; i<size[pa_d]; i++) {
	yp[i] = (yp[i]-yl_min)/(yl_max-yl_min);
      }
    }
  }
  break;
  
  case 2: {
    strcpy(ylabel,"N.U.");
    strcpy(title,"Normalized Pixel Values vs ");
    if ((max-min)==0.0) {  /* divide by zero case */
      for (i=0; i<size[pa_d]; i++) {
	yp[i] = 0.0;
      }
    }
    else {
      for (i=0; i<size[pa_d]; i++) {
	yp[i] = (yp[i]-min)/(max-min);
      }
    }
    yl_min = 0.0;
    yl_max = 1.0;
  }
  break;
    
  case 3: {
    strcpy(ylabel,"%");
    strcpy(title,"Percent of Mean vs ");
    yl_min = yl_max = 0.0;
    if (mean==0.0) {  /* divide by zero case */
      for (i=0; i<size[pa_d]; i++) {
	yp[i] = 0.0;
      }
    }
    else {
      for (i=0; i<size[pa_d]; i++) {
	yp[i] = ((yp[i]-mean)/mean);
	if (yp[i]<yl_min) yl_min=yp[i];
	if (yp[i]>yl_max) yl_max=yp[i];
      }
    }
    
    /* normalize */
    if ((yl_max-yl_min)==0.0) {  /* divide by zero case */
      for (i=0; i<size[pa_d]; i++){
	yp[i] = 0.0;
      }
    }
    else {
      for (i=0; i<size[pa_d]; i++){
	yp[i] = (yp[i]-yl_min)/(yl_max-yl_min);
      }
    }
    yl_min = yl_min * 100.0;  /* convert to percentage */
    yl_max = yl_max * 100.0;  /* convert to percentage */
  }
  break;

  default: break;
  }
  
  /* create a plot title */
  sprintf(xlabel,"%s", 
	    minc_volume_info.dimension_names[plot_active]);
  strcat(title,xlabel);
  sprintf(xlabel,"%s (%s)", 
	    minc_volume_info.dimension_names[plot_active],
	    minc_volume_info.dimension_units[plot_active]);

  /* determine in plane voxel space coordinates */
  if (plot_active==ndimensions-1) {
    rw_start[ndimensions-1] = minc_volume_info.start[ndimensions-1];
    rw_end[ndimensions-1] = minc_volume_info.start[ndimensions-1] + 
                           (minc_volume_info.length[ndimensions-1]-1) * 
                            minc_volume_info.step[ndimensions-1];
    rw_start[ndimensions-2] = minc_volume_info.start[ndimensions-2] + 
                              fy * minc_volume_info.step[ndimensions-2];
    rw_end[ndimensions-2] = rw_start[ndimensions-2];
  } 
  else if (plot_active==ndimensions-2) {
    rw_start[ndimensions-1] = minc_volume_info.start[ndimensions-1] + 
                              fx * minc_volume_info.step[ndimensions-1];
    rw_end[ndimensions-1] = rw_start[ndimensions-1];
    rw_start[ndimensions-2] = minc_volume_info.start[ndimensions-2];
    rw_end[ndimensions-2] = minc_volume_info.start[ndimensions-2] + 
                           (minc_volume_info.length[ndimensions-2]-1) * 
                            minc_volume_info.step[ndimensions-2];
  }
  else {
    rw_start[ndimensions-1] = minc_volume_info.start[ndimensions-1] + 
                              fx * minc_volume_info.step[ndimensions-1];
    rw_end[ndimensions-1] = rw_start[ndimensions-1];
    rw_start[ndimensions-2] = minc_volume_info.start[ndimensions-2] + 
                              fy * minc_volume_info.step[ndimensions-2];
    rw_end[ndimensions-2] = rw_start[ndimensions-2];
  }
  

  /* determine out of plane voxel space coordinates */
  for (i=ndimensions-3; i>=0; i--) {
    if (i==plot_active) {
      rw_start[i]= minc_volume_info.start[i];
      rw_end[i] =  minc_volume_info.start[i] + 
                  (minc_volume_info.length[i]-1) * minc_volume_info.step[i];
    }
    else {
      rw_start[i]= minc_volume_info.start[i] + 
	           (float) slider_image[i] * minc_volume_info.step[i];
      rw_end[i] = rw_start[i];
    }
  }

  /* coordinate transformation */
  tx=rw_start[minc_x_dim];
  ty=rw_start[minc_y_dim];
  tz=rw_start[minc_z_dim];
  rw_start[minc_x_dim] = tx*minc_volume_info.direction_cosines[minc_x_dim][0] +
                         ty*minc_volume_info.direction_cosines[minc_y_dim][0] +
                         tz*minc_volume_info.direction_cosines[minc_z_dim][0]; 
  rw_start[minc_y_dim] = tx*minc_volume_info.direction_cosines[minc_x_dim][1] +
                         ty*minc_volume_info.direction_cosines[minc_y_dim][1] +
                         tz*minc_volume_info.direction_cosines[minc_z_dim][1];
  rw_start[minc_z_dim] = tx*minc_volume_info.direction_cosines[minc_x_dim][2] +
                         ty*minc_volume_info.direction_cosines[minc_y_dim][2] +
                         tz*minc_volume_info.direction_cosines[minc_z_dim][2];
  tx=rw_end[minc_x_dim];
  ty=rw_end[minc_y_dim];
  tz=rw_end[minc_z_dim];
  rw_end[minc_x_dim] = tx*minc_volume_info.direction_cosines[minc_x_dim][0] +
                       ty*minc_volume_info.direction_cosines[minc_y_dim][0] +
                       tz*minc_volume_info.direction_cosines[minc_z_dim][0]; 
  rw_end[minc_y_dim] = tx*minc_volume_info.direction_cosines[minc_x_dim][1] +
                       ty*minc_volume_info.direction_cosines[minc_y_dim][1] +
                       tz*minc_volume_info.direction_cosines[minc_z_dim][1];
  rw_end[minc_z_dim] = tx*minc_volume_info.direction_cosines[minc_x_dim][2] +
                       ty*minc_volume_info.direction_cosines[minc_y_dim][2] +
                       tz*minc_volume_info.direction_cosines[minc_z_dim][2];

  /* add a check for unknown axis start and step */
  if (minc_volume_info.start[plot_active]==0 && 
      minc_volume_info.step[plot_active]==0) {
    rw_start[plot_active] = 1.0;
    rw_end[plot_active] = (float)minc_volume_info.length[plot_active];
    sprintf(xlabel,"%s %s (index)", 
	    minc_volume_info.dimension_names[plot_active],
	    minc_volume_info.dimension_units[plot_active]);
  }

  /* draw the plot axis */
  Draw_Plot_Axis(Plot_Canvas_Widget,
		 rw_start[plot_active], rw_end[plot_active],
                 yl_min,yl_max,xlabel,ylabel,title);

  /* set the color and start drawing the lines */
  EZ_Color3f(1.0,1.0,0.0);
  EZ_Begin(EZ_LINE_STRIP);       
  for(i = 0; i < size[pa_d]; i++) {
    EZ_Vertex2f(xp[i], yp[i]);
  }
  EZ_End();
  /* add a red cross indicating the current cursor location and ROI size */
  EZ_Color3f(1.0,0.0,0.0);
  EZ_Begin(EZ_LINE_STRIP);       
  EZ_Vertex2f(xp[ind[pa_d]]-(0.012*(float)(1 + plot_roi_size*2)), 
			     yp[ind[pa_d]]);
  EZ_Vertex2f(xp[ind[pa_d]]+(0.012*(float)(1 + plot_roi_size*2)), 
			     yp[ind[pa_d]]);
  EZ_End();
  EZ_Begin(EZ_LINE_STRIP);       
  EZ_Vertex2f(xp[ind[pa_d]], 
	      yp[ind[pa_d]]-(0.015*(float)(1 + plot_roi_size*2)));
  EZ_Vertex2f(xp[ind[pa_d]], 
	      yp[ind[pa_d]]+(0.015*(float)(1 + plot_roi_size*2)));
  EZ_End();
  if (ez_buffer_mode!=0) EZ_SwapBuffers();
}

/*--------------------- Plot_Type_Switch() ---------------------------*/
void Plot_Type_Switch(EZ_Widget *p_button, void *data)
{
  /* get the plot type (i.e. x, y, z, or t) */
  plot_active = EZ_GetRadioButtonGroupVariableValue(p_button);
  if (Verbose) {
    fprintf(stderr,"Plot Active Dimension (minc) = %d\n",plot_active);
  }

  /* clear any graphics overlays left behind*/
  Refresh(NULL);
}
 
/*--------------------- Plot_Style_Switch() ---------------------------*/
void Plot_Style_Switch(EZ_Widget *p_button, void *data)
{
  /* get the plot style */
  plot_style = EZ_GetRadioButtonGroupVariableValue(p_button);
  if (Verbose) {
    fprintf(stderr,"Plot Style = %d (0=raw, 1=normalized, 2=percent) \n",
	    plot_style);
  }
}

/*--------------------- Plot_Type_Switch() ---------------------------*/
void ROI_Size_Switch(EZ_Widget *p_button, void *data)
{
  /* get the plot type (i.e. x, y, z, or t) */
  plot_roi_size = EZ_GetRadioButtonGroupVariableValue(p_button);
  if (Verbose) {
    fprintf(stderr,"Plot ROI Size = %dx%d\n",plot_roi_size, plot_roi_size);
  }
}
 
/*--------------------- Draw_Plot_Axis() ---------------------------*/
void Draw_Plot_Axis(
                    EZ_Widget  *p_canvas,
  float      xl_min, float xl_max, float yl_min, float yl_max,
  char       *xlabel, char *ylabel, char *title)
{
  int        i;
  float      x_min, x_max, y_min, y_max, l;
  char       ctmp[64];

  /* hardwire the plot dimension (model space) for now */
  x_min = y_min = 0.0;
  x_max = y_max = 1.0;

  /* plot the axis lines, tick marks, labels, etc  */
  EZ_Color3f(1.0,1.0,1.0);
  EZ_Begin(EZ_LINE_STRIP);   
  EZ_Vertex2f( x_min, y_min );
  EZ_Vertex2f( x_max, y_min );  
  EZ_End();
  for (i=0; i<=10; i++) {
    EZ_Begin(EZ_LINE_STRIP);   
    EZ_Vertex2f( x_min + (float)i*(x_max-x_min)/10.0, y_min );
    EZ_Vertex2f( x_min + (float)i*(x_max-x_min)/10.0, y_min+(y_max-y_min)/70.0 );
    EZ_End();
    sprintf(ctmp,"%4g", xl_min + (float)i*(xl_max-xl_min)/10.0);
    l = (float)strlen(ctmp);
    EZ_CMove2f( ((float)i*(x_max-x_min)/10.0)-((l*0.01)/2), y_min-0.03 );
    EZ_DrawString(EZ_VECTOR_FONT,ctmp);
  }
  EZ_Begin(EZ_LINE_STRIP);   
  EZ_Vertex2f( x_min, y_min );
  EZ_Vertex2f( x_min, y_max );
  EZ_End();
  for (i=0; i<=10; i++) {
    EZ_Begin(EZ_LINE_STRIP);   
    EZ_Vertex2f( x_min, (float)i*(y_max-y_min)/10.0 );
    EZ_Vertex2f( x_min+(x_max-x_min)/70.0, (float)i*(y_max-y_min)/10.0 );
    EZ_End();
    sprintf(ctmp,"%4g", yl_min + (float)i*(yl_max-yl_min)/10.0);
    l = (float)strlen(ctmp);
    EZ_CMove2f( x_min-(0.01 + (l*0.01)), ((float)i*(y_max-y_min)/10.0)-0.01  );
    EZ_DrawString(EZ_VECTOR_FONT,ctmp);
  }
  EZ_CMove2f(((x_min+x_max)/2.0)-0.07, y_min-0.07);
  EZ_DrawString(EZ_VECTOR_FONT,xlabel);
  /*  EZ_CMove2f(x_min-0.09, ((x_min+x_max)/2.0)+0.05); */
  EZ_CMove2f(x_min-0.15, ((x_min+x_max)/2.0)+0.05);
  EZ_DrawString(EZ_VECTOR_FONT,ylabel);
  EZ_CMove2f(((x_min+x_max)/2.0)-0.15, y_max+0.01);
  EZ_DrawString(EZ_VECTOR_FONT,title);
}
 
 
