/*
 * 	keypress.c
 *
 *	The KeyPress event handler for Xdisp.
 *
 *	Handle_KeyPress()
 *
 * 	Copyright Bruce Pike, 1993-2000
 */

#include "xdisp.h"

/*---------------------------- Handle_KeyPress ----------------------*/
void Handle_KeyPress(XEvent *event)
{
  XKeyEvent      *key_event = &event->xkey;
  char           buf[128];
  KeySym         ks;
  XComposeStatus status;
  int            w, h, btmp, dw, dh;

  /* initialize the input string */
  buf[0]=0; 

  /* do it */
  XLookupString(key_event,buf,128,&ks,&status);
  if (buf[0]=='q' || buf[0]=='Q') Quit(NULL); 
  else if (buf[0]=='h' || buf[0]=='H' || buf[0]=='?') Open_Help_Widget(NULL);
  else if (buf[0]=='b') Toggle_ColorBar(NULL);
  else if (buf[0]=='i') Toggle_Interpolation(NULL);
  else if (buf[0]=='C') Switch_Colormap(NULL);
  else if (buf[0]=='v' || buf[0]=='V') 
    fprintf(stderr,"xdisp: %s\n",version);
  else if (buf[0]=='c') Crop(NULL);
  else if (buf[0]=='r') Rescale(NULL);
  else if (buf[0]=='w') Show_Window_Level();
  else if (buf[0]=='s') roi_stats(0,0,Width-1,Height-1,0);
  else if (buf[0]=='a') Auto_Scale(NULL);
  else if (buf[0]=='p') ps_file((void *)&zero); 
  else if (buf[0]=='P') { 
    if (!(plot_active==-1)) {
      Refresh(NULL);
      plot_active = -1;
      if ((plot_roi_size == 0)&&((zWidth<=Width)||(zHeight<=Height))) { /* draw a small cross hair */
	XDrawLine(theDisp,mainW,grGC,
		  theEvent.xmotion.x-5,theEvent.xmotion.y,
		  theEvent.xmotion.x+5,theEvent.xmotion.y);
	XDrawLine(theDisp,mainW,grGC,
		  theEvent.xmotion.x,theEvent.xmotion.y-5,
		  theEvent.xmotion.x,theEvent.xmotion.y+5);
      }
      else { /* draw a box */
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
      }
    }
    else {
      Open_Plot_Widget(NULL);
    }
  }
  else if (buf[0]=='f' || buf[0]=='F') ps_file((void *)&one); 
  else if (buf[0]=='e' || buf[0]=='E') ps_file((void *)&two);
  else if (buf[0]=='G') graphics_file((void *)&gr_gif);
  else if (buf[0]=='T') graphics_file((void *)&gr_tiff);
  else if (buf[0]=='B') flat_file((void *)&zero);
  else if (buf[0]=='S') flat_file((void *)&one);
  else if (buf[0]=='l') Load_All(NULL);
  else if (buf[0]=='m') MIP();
  else if (buf[0]=='M') matlab_file(NULL);
  else if (buf[0]=='t') toggle(NULL);
  else if (buf[0]=='o') Reorient_Volume(NULL);
  else if (buf[0]=='U') Open_Input_Widget("Upper:");
  else if (buf[0]=='L') Open_Input_Widget("Lower:");
  else if (buf[0]=='d'){ /* cycle current volume dimension */
    current_dim = (current_dim+1)%(ndimensions-2);
    if (Verbose)
      fprintf(stderr,"Current dimension: %d\n", current_dim);
  }
  else if (buf[0]=='+') { /* increment image */
    if (slider_image[current_dim]<slider_length[current_dim]-1){
      image_increment=1;
    } else {
      image_increment=-(slider_length[current_dim]-1);
    }
    New_Image(); 
  }
  else if (buf[0]=='-') {  /* decrement image */
    if (slider_image[current_dim]>0){
      image_increment=-1;
    } else {
      image_increment=slider_length[current_dim]-1;
    }
    New_Image(); 
  }
  else if (buf[0]=='*') {  /* increment image by 10 */
    image_increment=10;
    New_Image(); 
  }
  else if (buf[0]=='/') { /* decrement image by 10 */
    image_increment=-10;
    New_Image(); 
  }
  else if (buf[0]=='R') { /* refresh */
    btmp =  ext_roi_on;
    ext_roi_on = FALSE;
    DrawWindow(0,0,zWidth,zHeight);
    ext_roi_on = btmp;
  }
  else if (buf[0]=='u') { /* unzoom image to original size */
    w = Width;
    h = Height;
    Resize(w,h);
    XResizeWindow(theDisp,mainW,
		  w+(color_bar?color_bar_width:0),
		  h+info_height);
  }
  else if (buf[0]=='n' || buf[0]=='I') Reload(NULL);
  else if (buf[0]=='z') { /* zoom down */
    w = zWidth/2>0 ? zWidth/2 : zWidth;
    h = zHeight/2>0 ? zHeight/2 : zHeight;
    Resize(w,h);
    XResizeWindow(theDisp,mainW,
		  w+(color_bar?color_bar_width:0),
		  h+info_height);
  }
  else if (buf[0]=='Z') { /* zoom up */
    w = zWidth*2;
    h = zHeight*2;
    Resize(w,h);
    XResizeWindow(theDisp,mainW,
		  w+(color_bar?color_bar_width:0),
		  h+info_height);
  }
  else if (buf[0]=='x') { /* zoom down x */
    w = zWidth/2>0 ? zWidth/2 : zWidth;
    h = zHeight;
    Resize(w,h);
    XResizeWindow(theDisp,mainW,
		  w+(color_bar?color_bar_width:0),
		  h+info_height);
  }
  else if (buf[0]=='X') { /* zoom up x */
    w = zWidth*2;
    h = zHeight;
    Resize(w,h);
    XResizeWindow(theDisp,mainW,
		  w+(color_bar?color_bar_width:0),
		  h+info_height);
  }
  else if (buf[0]=='y') { /* zoom down y */
    w = zWidth;
    h = zHeight/2>0 ? zHeight/2 : zHeight;
    Resize(w,h);
    XResizeWindow(theDisp,mainW,
		  w+(color_bar?color_bar_width:0),
		  h+info_height);
  }
  else if (buf[0]=='Y') { /* zoom up y */
    w = zWidth;
    h = zHeight*2;
    Resize(w,h);
    XResizeWindow(theDisp,mainW,
		  w+(color_bar?color_bar_width:0),
		  h+info_height);
  }
  else if (buf[0]=='') {  /* load new file in current xdisp window */
    Open_File_Selector_Widget((void *)&zero);
  }
  else if (buf[0]=='') {  /* load new file and spawn new xdisp */
    Open_File_Selector_Widget((void *)&one);
  }
  else if (buf[0]=='	') {  /* ^I open mincheader info window */
    Open_Info_Widget(NULL);
  }
  else if (buf[0]=='') { /* flip image about x */
    Flip_Image((void *)&zero);
  }
  else if (buf[0]=='') { /* flip image about y */
    Flip_Image((void *)&one);
  }
  else if (buf[0]=='') TIC(NULL); /* display a TIC */
  else if (buf[0]=='') { /* rotate image clockwise */
    Rotate_Image(NULL);
  }
  else if (buf[0]=='') { /* write clipped byte image */
    flat_file((void *)&two);
  }
  else if (buf[0]=='') { /* display horizontal profile */
    H_Profile_Switch(NULL); 
  }
  else if (buf[0]=='') { /* display vertical profile */
    V_Profile_Switch(NULL); 
  }
  else if (buf[0]=='') { /* display external ROIs*/
    if ( ext_roi_on ) {
      ext_roi_on = FALSE;
      DrawWindow(0,0,zWidth,zHeight);
    }
    else {
      ext_roi_on = TRUE;
      draw_ext_roi(0,NULL,0,0);
    }
  }
  buf[0]=0;
}
