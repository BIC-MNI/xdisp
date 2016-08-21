/*
 * 	files.c
 *
 *	Output file creation routines.
 *
 *	ps_file()	
 *	graphics_file()
 *	flat_file()
 *	short_to_grfx()
 *	matlab_file()
 *
 *
 * 	Copyright (c) Bruce Pike, 1993-1997
 */

#include "xdisp.h"
#include <sys/stat.h>

extern void add_ext_roi(byte *tmp_image1);

/*---------------------------- ps_file ---------------------------*/
void ps_file(void *data)
{
  char    line[8192+1], 
          grey_hex_table[256][3],
          red_hex_table[256][3],
          green_hex_table[256][3],
          blue_hex_table[256][3],
          *cptr, *getenv(), *lp_cmd;
  byte    *tmp_image1, *tmp_image2;
  float   c1, c2, xscale, yscale;
  int     i, j, tmp, i_offset, index,
          x1, y1, x2, y2,
          page_width=612,
          page_height=792,
          w, h, printer;
  time_t  cr_time;            
  FILE    *fp;

  /* set the print flag */
  printer = *((int *) data);

  /* set the data offset for the current image */
  i_offset = (load_all_images?image_number:0) * Width * Height;
    
  /* construct a ps file name */
  if (printer!=0) {
    if (printer==1) 
      if (num_images>1)
	sprintf(ps_fname,"%s_image%d.ps",basfname,image_number+1);
      else
	sprintf(ps_fname,"%s.ps",basfname);
    else
      if (num_images>1)
	sprintf(ps_fname,"%s_image%d.eps",basfname,image_number+1);
      else
	sprintf(ps_fname,"%s.eps",basfname);
    cr_time = time(NULL);
    if (Verbose) fprintf(stderr,"Preparing postscript file %s\n",ps_fname);
    strncpy(ps_fname,Get_Save_Filename(ps_fname),128); 
    if (ps_fname[0]=='\0' || rindex(ps_fname,' ') ) {
      if (Verbose || Save_Select_Done==1) {
	fprintf(stderr,"Invalid file name: %s \n", ps_fname);
	fprintf(stderr,"File write aborted!\n");
      }
      return;
    }
  }
  else {
    ps_fname[0] = '\0';
    strcat(ps_fname,PRINTER_PORT);
    if (Verbose) fprintf(stderr,"Printing postscript...\n");
  }

  /* open the ps file for output */
  if (printer==0) 
    if ((lp_cmd=getenv("LP_CMD"))==NULL) {
      fp = popen("lpr","w");
    }
    else {
      fp = popen(lp_cmd,"w");
    }

  else
    fp = fopen(ps_fname,"wb");
  if (fp==NULL) {
    if (printer==0) 
      fprintf(stderr,"Error opening printer pipe!\n");
    else 
      fprintf(stderr,"Error opening file!\n");
    return;
  }

  /* define new cursor */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);

  /* display working notice */
  if (printer==0) 
    fprintf(stderr,"Printing...");
  else 
    fprintf(stderr,"Writing file %s...", ps_fname);

  /* scale image data to current Upper, Lower */
  tmp_image1 = (byte *) malloc(Width*Height);
  c1 = (float)(Upper-Lower) / 255.0;
  if (Verbose) fprintf(stderr,"scaling image data to min=%d, max=%d\n",
		       Lower, Upper);
  for (i=0; i<Width*Height; i++) {
    c2 = (float)(short_Image[i+i_offset] - Lower) / c1;
    c2 = (c2 < 0.0) ? 0.0 : c2;
    c2 = (c2 > 255.0) ? 255.0 : c2;
    tmp_image1[i] = (byte) c2;
  }

  /* perform interpolation */
  tmp_image2 = (byte *) malloc(zWidth*zHeight);
  if (Interpolation_Type==BILINEAR) {
    bilinear_byte_to_byte (tmp_image1, Width, Height,
			   tmp_image2, zWidth, zHeight);
  }
  else {
    nneighbour_byte_to_byte (tmp_image1, Width, Height,
			     tmp_image2, zWidth, zHeight);
  }

  /* center image on page and use 1 ps point per pixel*/
  w = zWidth<page_width ? zWidth : page_width;
  h = zHeight<page_height ? zHeight : page_height;
  x1 = (page_width-w)/2;
  y1 = (page_height-h)/2;
  x2 = x1 + w;
  y2 = y1 + h;
    
  /* start pumping out the postscript */
  fprintf(fp,"%%!PS-Adobe-3.0 EPSF-3.0\n");
  fprintf(fp,"%%%%Title: %s\n",ps_fname);
  fprintf(fp,"%%%%Creator: Xdisp %s\n",version);
  fprintf(fp,"%%%%CreationDate: %s",ctime(&cr_time));
  fprintf(fp,"%%%%Pages: 001\n");
  fprintf(fp,"%%%%Document Fonts:\n");
  fprintf(fp,"%%%%BoundingBox: %d %d %d %d\n", x1, y1, x2, y2);
  fprintf(fp,"%%%%EndComments\n");
  fprintf(fp,"%%%%BeginSetup\n");
  fprintf(fp,"/bdef {bind def} bind def\n");
  fprintf(fp,"/ldef {load def} bind def\n");
  fprintf(fp,"/bplot /gsave ldef\n");
  fprintf(fp,"/eplot {stroke grestore} bdef\n");
  fprintf(fp,"%%%%EndSetup\n\n");    
  if (printer==0) {    
    fprintf(fp,"/Helvetica findfont\n");
    fprintf(fp,"15 scalefont setfont\n");
    fprintf(fp,"/centershow {dup stringwidth pop 2 div neg 0 rmoveto show} def\n");
    fprintf(fp,"%d %d 2 div add %d 36 add moveto\n",x1,w,y1);
    fprintf(fp,"%d %d 2 div add %d %d add 36 add moveto\n",x1,w,y1,h);
    fprintf(fp,"( %s   %d W  %d L ) centershow\n",
	    basfname, (Upper-Lower), (Upper+Lower)/2);
  }
  fprintf(fp,"bplot\n");
  fprintf(fp,"/picstr %d string def\n",zWidth);
  fprintf(fp,"%d %d translate\n", x1, y1);
  fprintf(fp,"%d %d scale\n", w, h ) ;
  fprintf(fp,"%d %d 8 [%d 0 0 %d 0 %d]\n",
	  zWidth, zHeight, zWidth, -zHeight, zHeight) ;
  if (color_table==0) {
    fprintf(fp,"{currentfile picstr readhexstring pop} image\n\n");
  }
  else {
    fprintf(fp,"{currentfile picstr readhexstring pop} false 3 colorimage\n\n");
  }

  if (Verbose) fprintf(stderr,"Creating postscript for colormap %d\n",color_table);
  if (color_table==0) {
    if (PS_Gamma==1.0) {
      for (i=0; i<256; i++)
	sprintf(grey_hex_table[i],"%02x",i);
    }
    else {
      for (i=0; i<256; i++) {
	tmp = pow(((float)i)/255.0, (1.0/PS_Gamma))*255.0 + 0.5;
	if(tmp > 255) tmp = 255;
	else if(tmp < 0) tmp = 0;
	sprintf(grey_hex_table[i],"%02x",tmp);
      }
    }
  }
  else if (color_table==1) { /* hot metal color table */
    for (i=0; i<256; i++) {
      sprintf(red_hex_table[i],"%02x", hot_metal_table[0][i]);
      sprintf(green_hex_table[i],"%02x", hot_metal_table[1][i]);
      sprintf(blue_hex_table[i],"%02x", hot_metal_table[2][i]);
    }
  }
  else {
    for (i=0; i<256; i++) { /* spectral color table */
      sprintf(red_hex_table[i],"%02x", spectral_table[0][i]);
      sprintf(green_hex_table[i],"%02x", spectral_table[1][i]);
      sprintf(blue_hex_table[i],"%02x", spectral_table[2][i]);
    }
  }
    
  if (color_table==0) {
    for (i=0; i<zHeight; i++)  {
      cptr = line;
      for (j=0; j<zWidth; j++) {
	index = i*zWidth+j;
	*cptr++ = grey_hex_table[tmp_image2[index]][0];
	*cptr++ = grey_hex_table[tmp_image2[index]][1];
	if ( (((j+1)/40)*40) == (j+1) )
	  *cptr++ = 10;		/* new line */
      }
      *cptr = 10;  			/* final nl */
      fwrite(line,1,2*zWidth+(zWidth/40)+1,fp);
    }
  }
  else {
    for (i=0; i<zHeight; i++)  {
      cptr = line;
      for (j=0; j<zWidth; j++) {
	index = i*zWidth+j;            	
	*cptr++ = red_hex_table[tmp_image2[index]][0];
	*cptr++ = red_hex_table[tmp_image2[index]][1];
	*cptr++ = green_hex_table[tmp_image2[index]][0];
	*cptr++ = green_hex_table[tmp_image2[index]][1];
	*cptr++ = blue_hex_table[tmp_image2[index]][0];
	*cptr++ = blue_hex_table[tmp_image2[index]][1];
	if ( (((j+1)/12)*12) == (j+1) )
	  *cptr++ = 10;		/* new line */
      }
      *cptr = 10;  			/* final nl */
      fwrite(line,1,3*2*zWidth+(zWidth/12)+1,fp);
    }
  }
  fprintf(fp,"/#copies 1 def\n");
  fprintf(fp,"eplot\n\n");

  /* print profiles if they are being displayed */
  if (v_profile_active) {
    fprintf(fp,"%% image profiles\n");
    fprintf(fp,"bplot\n");
    xscale = (float)(zWidth-1)/(float)(Width-1);
    yscale = (float)(zHeight-1)/(float)(Height-1);
    fprintf(fp,"newpath\n");
    fprintf(fp,"%d %d moveto\n", x1+(int)(profile_x*xscale), y1+zHeight);
    fprintf(fp,"%d %d lineto\n", x1+(int)(profile_x*xscale), y1);
    fprintf(fp,"%d %d moveto\n", x1, y1+zHeight);
    xscale = 0.5 * (float)(zWidth-1)/(float)(I_Max-I_Min);
    for (i=0; i<Height; i++) {
      x2 = (int)(abs ((short_Image[i*Width + profile_x]-I_Min) * xscale));
      y2 = (int)(yscale * (float)i); 
      fprintf(fp,"%d %d lineto \n",x1+x2,y1+zHeight-y2);
    }
    fprintf(fp,"%f %f %f setrgbcolor\n",
	    (float)newC[gcol].red/65535.0,
	    (float)newC[gcol].green/65535.0,
	    (float)newC[gcol].blue/65535.0);
    fprintf(fp,"1 setlinewidth\n");
    fprintf(fp,"stroke\n");
    fprintf(fp,"eplot\n");
  }

  if (h_profile_active) {
    fprintf(fp,"bplot\n");
    xscale = (float)(zWidth-1)/(float)(Width-1);
    yscale = (float)(zHeight-1)/(float)(Height-1);
    fprintf(fp,"newpath\n");
    fprintf(fp,"%d %d moveto\n", x1, y1+zHeight-(int)((float)profile_y*yscale));
    fprintf(fp,"%d %d lineto\n", x1+zWidth, y1+zHeight-(int)((float)profile_y*yscale));
    fprintf(fp,"%d %d moveto\n", x1, y1);
    yscale = 0.5 * (float)(zHeight-1)/(float)(I_Max-I_Min);
    for (i=0; i<Width; i++) {
      x2 = (int)(xscale * (float)i);
      y2 = (int)(abs ((short_Image[profile_y*Width + i]-I_Min) * yscale));
      fprintf(fp,"%d %d lineto \n",x1+x2,y1+y2);
    }
    fprintf(fp,"%f %f %f setrgbcolor\n",
	    (float)newC[gcol].red/65535.0,
	    (float)newC[gcol].green/65535.0,
	    (float)newC[gcol].blue/65535.0);
    fprintf(fp,"1 setlinewidth\n");
    fprintf(fp,"stroke\n");
    fprintf(fp,"eplot\n");
  }

  /* print roi's if they are being displayed */
  if (ext_roi_on) draw_ext_roi(1,fp,x1,y1);

  /* finish up the postscript */
  if (printer!=2) fprintf(fp,"showpage\n");
  fprintf(fp,"%%%%EndDocument\n");

  /* close the file */
  if (printer==0)
    pclose(fp);
  else
    fclose(fp);

  /* end working notice */
  fprintf(stderr," Done.\n");
  ps_fname[0] = '\0';

  /* free memory */
  free(tmp_image1);
  free(tmp_image2);

  /* define new cursor */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}

/*-------------------------- graphics_file ---------------------------*/
void graphics_file(void *data)
{
  char    	gr_fname[128];
  int     	type, i_offset;

  /* set the file type flag */
  type = *((int *) data);

  /* data offset for this image */
  i_offset = (load_all_images?image_number:0) * Width * Height;

  /* send off to correct routine */
  switch (type) {
  case GIF: /* GIF */
    if (num_images>1) 
      sprintf(gr_fname, "%s_image%d.gif",basfname,image_number+1);
    else
      sprintf(gr_fname, "%s.gif",basfname);
    break;
  case TIFF: /* TIFF */
    if (num_images>1) 
      sprintf(gr_fname, "%s_image%d.tiff",basfname,image_number+1);
    else
      sprintf(gr_fname, "%s.tiff",basfname);
    break;
  case PICT: /* PICT */
    if (num_images>1) 
      sprintf(gr_fname, "%s_image%d.pict",basfname,image_number+1);
    else
      sprintf(gr_fname, "%s.pict",basfname);
    break;
  case SGI: /* SGI */
    if (num_images>1) 
      sprintf(gr_fname, "%s_image%d.sgi",basfname,image_number+1);
    else
      sprintf(gr_fname, "%s.sgi",basfname);
    break;
  case RAST: /* SUN raster */
    if (num_images>1) 
      sprintf(gr_fname, "%s_image%d.rast",basfname,image_number+1);
    else
      sprintf(gr_fname, "%s.rast",basfname);
    break;
  }
    strncpy(gr_fname,Get_Save_Filename(gr_fname),128);
    if (gr_fname[0]=='\0' || rindex(gr_fname,' ') ) {
      if (Verbose || Save_Select_Done==1) {
	fprintf(stderr,"Invalid file name: %s \n", gr_fname);
	fprintf(stderr,"File write aborted!\n");
      }
      return;
    }
  short_to_grfx(&short_Image[i_offset], gr_fname, Width, Height, 
		zWidth, zHeight, Lower, Upper, type);
}

/*---------------------------- flat_file ---------------------------*/
void flat_file(void *data)
{
  char    	byte_fname[128], short_fname[128];
  byte    	*tmp_byte_image;
  short    	*tmp_short_image;
  float		c1, value;
  int     	i, type;
  FILE    	*fp;

  /* set the output format */
  type = *((int *) data);

  /* construct a file name */
  if ((type==0) || (type==2)) {
    if (num_images>1)
      sprintf(byte_fname,"%s_image%d.byte",basfname,image_number+1);
    else
      sprintf(byte_fname,"%s.byte",basfname);
    if (Verbose) fprintf(stderr,"Preparing flat byte file %s\n",byte_fname);
    strncpy(byte_fname,Get_Save_Filename(byte_fname),128); 
    if (byte_fname[0]=='\0' || rindex(byte_fname,' ') ) {
      if (Verbose || Save_Select_Done==1) {
	fprintf(stderr,"Invalid file name: %s \n", byte_fname);
	fprintf(stderr,"File write aborted!\n");
      }
      return;
    }
  }
  else {
    if (num_images>1)
      sprintf(short_fname,"%s_image%d.int",basfname,image_number+1);
    else
      sprintf(short_fname,"%s.int",basfname);
    if (Verbose) fprintf(stderr,"Preparing flat short int file %s\n",short_fname);
    strncpy(short_fname,Get_Save_Filename(short_fname),128); 
    if (short_fname[0]=='\0' || rindex(short_fname,' ') ) {
      if (Verbose || Save_Select_Done==1) {
	fprintf(stderr,"Invalid file name: %s \n", short_fname);
	fprintf(stderr,"File write aborted!\n");
      }
      return;
    }
  }

  /* open file */
  if ((type==0) || (type==2)) 
    fp = fopen(byte_fname,"wb");
  else
    fp = fopen(short_fname,"wb");
  if (fp==NULL) {
    fprintf(stderr,"Error opening file!\n");
    return;
  }

  /* define new cursor */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);

  /* display message */
  if ((type==0) || (type==2))
    fprintf(stderr,"Writing file %s...",byte_fname);
  else
    fprintf(stderr,"Writing file %s...",short_fname);

  /* create tmp short image */
  tmp_short_image = (short *) malloc(zWidth*zHeight*2);
  if (Interpolation_Type) {
    bilinear_short_to_short( &short_Image[Width*Height*(load_all_images?image_number:0)],
			     Width, Height, tmp_short_image, zWidth, zHeight);
  }
  else {
    nneighbour_short_to_short( &short_Image[Width*Height*(load_all_images?image_number:0)],
			       Width, Height, tmp_short_image, zWidth, zHeight);
  }

  /* scale image data (if necessary) and write result */
  if (type==0) {
    tmp_byte_image = (byte *) malloc(zWidth*zHeight);
    if ((P_Min>=0)&&(P_Min<=255)&&(P_Max>=0)&&(P_Max<=255)) {
      for (i=0; i<zWidth*zHeight; i++) 
	tmp_byte_image[i] = (byte) tmp_short_image[i];
    }
    else {
      c1 = (float)(P_Max-P_Min) / 255.0;
      if (Verbose) fprintf(stderr,"scaling image data to min=0, max=255\n");
      for (i=0; i<zWidth*zHeight; i++) {
	value = (float)(tmp_short_image[i] - P_Min) / c1;
	value = (value < 0.0) ? 0.0 : value;
	value = (value > 255.0) ? 255.0 : value;
	tmp_byte_image[i] = (byte) value;
      }
    }
    fwrite(tmp_byte_image,1,zWidth*zHeight,fp);
    free(tmp_byte_image);
    fclose(fp);
  }
  else if (type==1) {
    fwrite(tmp_short_image,1,zWidth*zHeight*2,fp);
    fclose(fp);
  }
  else {
    tmp_byte_image = (byte *) malloc(zWidth*zHeight);
    if (abs(Upper-Lower)<256)
      for (i=0; i<zWidth*zHeight; i++) {
	if (tmp_short_image[i]<=Lower)
	  tmp_byte_image[i] = 0;
	else if (tmp_short_image[i]>=Upper)
	  tmp_byte_image[i] = (byte)(abs(Upper-Lower));
	else
	  tmp_byte_image[i] = (byte)(tmp_short_image[i]-Lower);
      }
    else {
      c1 = (float)(abs(Upper-Lower)) / 255.0;
      for (i=0; i<zWidth*zHeight; i++) {
	value = (float)(tmp_short_image[i] - Lower) / c1;
	value = (value < 0.0) ? 0.0 : value;
	value = (value > 255.0) ? 255.0 : value;
	tmp_byte_image[i] = (byte) value;
      }
    }
    fwrite(tmp_byte_image,1,zWidth*zHeight,fp);
    free(tmp_byte_image);
    free(tmp_short_image);
    fclose(fp);
  }

  /* define new cursor */
  fprintf(stderr,"Done.\n");
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}


/*------------------------ short_to_grfx ----------------------*/
void short_to_grfx(short *image, char *filename, 
                   int cols, int rows, int zcols, int zrows, 
                   int min, int max, int type)
{
  FILE 	*out;
  char 	command_line[256];
  byte 	gamma_table[256];  
  byte 	*line;
  byte 	*tmp_image1, *tmp_image2;
  int 	i, j, value, tmp;


  /* get memory */
  line = (byte *) malloc(zcols*3);    	

  /* test to make sure we can write a file */
  out = fopen(filename,"wb");
  if (out==NULL) {
    fprintf(stderr,"Error opening file!\n");
    return;
  }
  fclose(out);

  /* pipe to rawtoppm and ... */
  switch (type) {
  case GIF:
    sprintf(command_line,"rawtoppm %d %d | ppmtogif > %s\n", 
	    zcols, zrows, filename);
    break;
  case TIFF:
    sprintf(command_line,"rawtoppm %d %d | pnmtotiff > %s\n", 
	    zcols, zrows, filename);
    break;
  case PICT:
    sprintf(command_line,"rawtoppm %d %d | ppmtopict > %s\n", 
	    zcols, zrows, filename);
    break;
  case SGI:
    sprintf(command_line,"rawtoppm %d %d | pnmtosgi -rle > %s\n", 
	    zcols, zrows, filename);
    break;
  case RAST:
    sprintf(command_line,"rawtoppm %d %d | pnmtorast -rle > %s\n", 
	    zcols, zrows, filename);
    break;
  }
  if (!(out = popen (command_line,"w"))) {
    fprintf(stderr,"Error opening pipe!\n");
    return;
  }

  /* define new cursor */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);
  fprintf(stderr,"Writing file %s...",filename);

  /* first, scale to 0-255 */
  tmp_image1 = (byte *) malloc(rows*cols);
  for (i=0; i<rows*cols; i++) {
    value = ((image[i] - min))  * (float) (255.0 / (max-min)) ;
    value = (value < 0) ? 0: value;
    value = (value > 255) ? 255 : value;
    tmp_image1[i] = (byte) value;
  }

  /* now interpolate */
  tmp_image2 = (byte *) malloc(zrows*zcols);
  if (Interpolation_Type) {
    bilinear_byte_to_byte (tmp_image1, cols, rows, tmp_image2, zcols, zrows);
  }
  else {
    nneighbour_byte_to_byte (tmp_image1, cols, rows, tmp_image2, zcols, zrows);
  }

  /* adjust hex table for gamma */
  if (Gamma==1.0) {
    for (i=0; i<256; i++) {
      gamma_table[i] = i;
    }
  }
  else {
    for (i=0; i<256; i++) {
      tmp = pow(((float)i)/255.0, (1.0/Gamma))*255.0 + 0.5;
      if(tmp > 255) tmp = 255;
      else if(tmp < 0) tmp = 0;
      gamma_table[i] = tmp;
    }
  }

  /* now write out the hex values... */
  for (i=0; i<zrows; i++) {
    if (color_table==0) { 
      for (j=0; j<zcols; j++) { /* grey scale color map */
	line[j*3] =   gamma_table[tmp_image2[i*zcols+j]];
	line[j*3+1] = gamma_table[tmp_image2[i*zcols+j]];
	line[j*3+2] = gamma_table[tmp_image2[i*zcols+j]];
      }
      fwrite(line,1,zcols*3,out);        	
    }
    else if (color_table==1) {
      for (j=0; j<zcols; j++) { /* hot metal color map */
	line[j*3] =   hot_metal_table[0][gamma_table[tmp_image2[i*zcols+j]]];
	line[j*3+1] = hot_metal_table[1][gamma_table[tmp_image2[i*zcols+j]]];
	line[j*3+2] = hot_metal_table[2][gamma_table[tmp_image2[i*zcols+j]]];
      }
      fwrite(line,1,zcols*3,out);        	
    }
    else {
      for (j=0; j<zcols; j++) {	/* spectral color map */
	line[j*3] =   spectral_table[0][gamma_table[tmp_image2[i*zcols+j]]];
	line[j*3+1] = spectral_table[1][gamma_table[tmp_image2[i*zcols+j]]];
	line[j*3+2] = spectral_table[2][gamma_table[tmp_image2[i*zcols+j]]];
      }
      fwrite(line,1,zcols*3,out);        	
    }
  }

  /* clean up */
  free(line);
  free(tmp_image1);
  free(tmp_image2);
  pclose(out);

  /* define new cursor */
  fprintf(stderr,"Done.\n");
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}


/*------------------------ short_to_pict ----------------------*/
void short_to_pict(
     short	*image, char 	*filename, 
     int    	cols, int rows, int zcols, int zrows, int min, int max)
{
  FILE 	*out;
  char 	command_line[256];
  byte 	gamma_table[256];  
  byte 	*line;
  byte 	*tmp_image1, *tmp_image2;
  int 	i, j, value, tmp;


  /* get memory */
  line = (byte *) malloc(zcols*3);    	

  /* test to make sure we can write a file */
  out = fopen(filename,"wb");
  if (out==NULL) {
    fprintf(stderr,"Error opening file!\n");
    return;
  }
  fclose(out);

  /* pipe to rawtoppm */
  sprintf(command_line,"rawtoppm %d %d | ppmtopict > %s\n", 
	  zcols, zrows, filename);
  if (!(out = popen (command_line,"w"))) {
    fprintf(stderr,"Error opening pipe!\n");
    return;
  }

  /* define new cursor */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);
  fprintf(stderr,"Writing file %s...",filename);

  /* first, scale to 0-255 */
  tmp_image1 = (byte *) malloc(rows*cols);
  for (i=0; i<rows*cols; i++) {
    value = ((image[i] - min))  * (float) (255.0 / (max-min)) ;
    value = (value < 0) ? 0: value;
    value = (value > 255) ? 255 : value;
    tmp_image1[i] = (byte) value;
  }

  /* now interpolate */
  tmp_image2 = (byte *) malloc(zrows*zcols);
  if (Interpolation_Type) {
    bilinear_byte_to_byte (tmp_image1, cols, rows, tmp_image2, zcols, zrows);
  }
  else {
    nneighbour_byte_to_byte (tmp_image1, cols, rows, tmp_image2, zcols, zrows);
  }

  /* adjust hex table for gamma */
  if (Gamma==1.0) {
    for (i=0; i<256; i++) {
      gamma_table[i] = i;
    }
  }
  else {
    for (i=0; i<256; i++) {
      tmp = pow(((float)i)/255.0, (1.0/Gamma))*255.0 + 0.5;
      if(tmp > 255) tmp = 255;
      else if(tmp < 0) tmp = 0;
      gamma_table[i] = tmp;
    }
  }

  /* now write out the hex values... */
  for (i=0; i<zrows; i++) {
    if (color_table==0) { 
      for (j=0; j<zcols; j++) { /* grey scale color map */
	line[j*3] =   gamma_table[tmp_image2[i*zcols+j]];
	line[j*3+1] = gamma_table[tmp_image2[i*zcols+j]];
	line[j*3+2] = gamma_table[tmp_image2[i*zcols+j]];
      }
      fwrite(line,1,zcols*3,out);        	
    }
    else if (color_table==1) {
      for (j=0; j<zcols; j++) { /* hot metal color map */
	line[j*3] =   hot_metal_table[0][gamma_table[tmp_image2[i*zcols+j]]];
	line[j*3+1] = hot_metal_table[1][gamma_table[tmp_image2[i*zcols+j]]];
	line[j*3+2] = hot_metal_table[2][gamma_table[tmp_image2[i*zcols+j]]];
      }
      fwrite(line,1,zcols*3,out);        	
    }
    else {
      for (j=0; j<zcols; j++) {	/* spectral color map */
	line[j*3] =   spectral_table[0][gamma_table[tmp_image2[i*zcols+j]]];
	line[j*3+1] = spectral_table[1][gamma_table[tmp_image2[i*zcols+j]]];
	line[j*3+2] = spectral_table[2][gamma_table[tmp_image2[i*zcols+j]]];
      }
      fwrite(line,1,zcols*3,out);        	
    }
  }

  /* clean up */
  free(line);
  free(tmp_image1);
  free(tmp_image2);
  pclose(out);

  /* define new cursor */
  fprintf(stderr,"Done.\n");
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}

/*---------------------------- matlab_file ---------------------------*/
void matlab_file(void *data)
{
  char    		mat_fname[128], mat_vname[20], *cptr;
  short		        *tmp_short_image;
  double 		value;
  int     		i, j;
  FILE    		*fp;
  struct mat_header {
    long    type; 
    long    nrows;
    long    ncols;
    long    imag_flag;      
    long    name_len;
  } mat_var;

  /* construct a file name */
  strcpy(mat_fname,basfname);
  cptr = rindex(mat_fname,'.');
  if (cptr) *cptr = '\0';
  if ((num_images>1)&&(cptr))
    sprintf(cptr,"_image%d.mat",image_number+1);
  else
    strcat(mat_fname,".mat");
  if (Verbose) fprintf(stderr,"Preparing matlab file %s\n",mat_fname);
  strncpy(mat_fname,Get_Save_Filename(mat_fname),128); 
  if (mat_fname[0]=='\0' || rindex(mat_fname,' ') ) {
    if (Verbose || Save_Select_Done==1) {
      fprintf(stderr,"Invalid file name: %s \n", mat_fname);
      fprintf(stderr,"File write aborted!\n");
    }
    return;
  }

  /* open file */
  fp = fopen(mat_fname,"wb");
  if (fp==NULL) {
    fprintf(stderr,"Error opening file!\n");
    return; 
  }

  /* define new cursor */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);

  /* display message */
  fprintf(stderr,"Writing file %s...",mat_fname);

  /* define matlab header information */
  /* mat_var.type = M0PT 
     M = 0: IEEE Little Endian
     1: IEEE Big Endian
     0 = reserved (allways zero)
     P = 0: double
     1: float
     2: signed long (32-bit)
     3: signed short  (16-bit)
     4: unsigned short (16-bit)
     5: unsigned char (8-bit)
     T = 0: numeric (full) matrix
     1: text matrix
     2: sparse matrix
  */
  if (file_format==MINC_FORMAT || 
      file_format==ACR_NEMA_FORMAT ||
      Input_Data_Type > SHORT_DATA){
#ifdef LINUX
    mat_var.type = 0000;   /* IEEE Little Endian doubles */
#else
    mat_var.type = 1000;   /* IEEE Big Endian doubles */
#endif
  } else {
#ifdef LINUX
    mat_var.type = 30;   /* IEEE Little Endian shorts */
#else    
    mat_var.type = 1030;   /* IEEE Big Endian shorts */
#endif
  }

  mat_var.nrows = zHeight;
  mat_var.ncols = zWidth;
  mat_var.imag_flag = 0;
  if (num_images>1)
    sprintf(mat_vname,"im%d",image_number);
  else
    sprintf(mat_vname,"im");
  mat_var.name_len = strlen(mat_vname)+1;

  /* create tmp short image */
  tmp_short_image = (short *) malloc(zWidth*zHeight*2);
  if (Interpolation_Type==BILINEAR) {
    bilinear_short_to_short( &short_Image[Width*Height*
					 (load_all_images?image_number:0)],
			     Width, Height, 
			     tmp_short_image, zWidth, zHeight);
  }
  else {
    nneighbour_short_to_short( &short_Image[Width*Height*
					   (load_all_images?image_number:0)],
			       Width, Height, 
			       tmp_short_image, zWidth, zHeight);
  }

  /* write the header and variable name */
  fwrite(&mat_var,sizeof(mat_var),1,fp);
  fwrite(mat_vname,sizeof(char),(int)(mat_var.name_len),fp);

  /* write the image data */
  if (file_format==MINC_FORMAT || 
      file_format==ACR_NEMA_FORMAT ||
      Input_Data_Type > SHORT_DATA){
    for (i=0; i<zWidth; i++){
      for (j=0; j<zHeight; j++){
	value = (double)(rw_offset+((double)((long)tmp_short_image[j*zWidth+i]-
					     I_Min)*rw_scale));
	fwrite(&value,sizeof(double),1,fp);
      }
    }
  } 
  else {
    for (i=0; i<zWidth; i++)
      for (j=0; j<zHeight; j++)
	fwrite(&tmp_short_image[j*zWidth+i],2,1,fp);
  }

  /* close file and clean up */    
  fclose(fp);
  free(tmp_short_image);

  /* define new cursor */
  fprintf(stderr,"Done.\n");
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}

