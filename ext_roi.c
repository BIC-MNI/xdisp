/*
 * draw_ext_roi.c
 *
 * Draw roi identified in msp program
 *
 * T. Ranger 
 * Sept 1994
 */

#include "xdisp.h"

/* msp roi file stuff */
#include "ms_roi.h"
typedef int STATUS;
enum STATUS { OK, ERROR };
static STACKROI_LIST slice_array[300];

/* prototypes */
WHERE int draw_ext_roi(int ps_roi_draw, FILE *ps_fp, int ps_xoff, int ps_yoff);  
STATUS read_roi_file(int *num_slices, STACKROI_LIST slice_array[], char *infilename);
STATUS  load_region_of_interest(FILE *fp, int *slices,  STACKROI_LIST slice_array[]);
STATUS store_roi_position(int x, int y, STACKROI_LIST *slice_ptr);
STATUS add_point_to_roi(int x,int y, ROI_LIST *r);
void free_one_roi(ROI_LIST *r, STACKROI_LIST *sp);
void delete_one_roi(ROI_LIST *r, ROI_LIST **list, STACKROI_LIST *sp);
void delete_complete_roi_list(struct roi_list **list, STACKROI_LIST *sp);
void delete_roi_list(STACKROI_LIST slice_array[], int slice_count);
void add_ext_roi(byte *tmp_image1);
STATUS create_roi_list_pointer(STACKROI_LIST *slice_ptr);


void add_ext_roi(byte *tmp_image) {
  int roi;
  int vert;
  ROI_LIST *roip;
  POINT_LIST *vertp;
  int x1,y1,x2,y2;
  int i;
  float xscale,yscale;
  boolean first;
  int dx,dy;
  float xstep,ystep,n;

  if ( ext_roi_active ) {
    xscale = (float)(zWidth-1)/(float)(Width-1);
    yscale = (float)(zHeight-1)/(float)(Height-1);

    roip = slice_array[image_number].roilist;
    for (roi=0; roi<slice_array[image_number].number_of_rois; ++roi) {
      first = TRUE;
      vertp= roip->next_point;
      for (vert=0; vert<roip->number_of_points; ++vert) {
	x1=(int)(vertp->x*xscale);
	y1=(int)(vertp->y*yscale);
	if ( first == FALSE ) {
	  tmp_image[zWidth*y1+x1]=(byte)roiceps;
	  tmp_image[zWidth*y2+x2]=(byte)roiceps;
	  dx = abs(x2-x1);
	  dy = abs(y2-y1);
	  xstep = x2 > x1 ? 1 : (x2 < x1 ? -1 : 0);
	  ystep = y2 > y1 ? 1 : (y2 < y1 ? -1 : 0); 
	  n = dx > dy ? dx : dy;
	  xstep = xstep*dx/n;
	  ystep = ystep*dy/n;
	  while ( n-- > 1 ) {
	    i = (int)(zWidth*(y1+(int)(n*ystep))+(x1+(int)(n*xstep)));
	    if ( i < zWidth*zHeight ) { 
	      tmp_image[i]=(byte)roiceps;
	    }
	    else {
	      fprintf(stderr,"Index exceeds image boundary %d\n",i);
	    }
	  }
	}
	x2=x1;
	y2=y1;
	vertp = vertp->next_point;
	first= FALSE;
      }
      roip=roip->next_roi;
    }
  }
}

int draw_ext_roi(int ps_roi_draw, FILE *ps_fp, int ps_xoff, int ps_yoff)  
{
  int num_slices;
  int roi;
  int vert;
  ROI_LIST *roip;
  POINT_LIST *vertp;
  int x1,y1,x2,y2;
  int first = TRUE;
  static int roi_file=FALSE;
  float xscale,yscale;
  

  /* read roi from file into stack structure */
  if ( !roi_file ) {
    if ( read_roi_file(&num_slices,slice_array,roifname)== OK) {
      roi_file = TRUE;
    }
    else {
      fprintf(stderr,"Unable to read roi file %s\n",roifname);
      roi_file = 0;
    }
  }

  /* draw rois on slice */
  if ( roi_file ) { 
    /*    if ( zoom_factor_x  == 2.0  || zoom_factor_x == 0.5 ) 
	  xscale = zoom_factor_x;
	  else
	  */      xscale = (float)(zWidth-1)/(float)(Width-1);
	  /*    if ( zoom_factor_y  == 2.0  || zoom_factor_y == 0.5 ) 
		yscale = zoom_factor_y;
		else

		*/    yscale = (float)(zHeight-1)/(float)(Height-1);
    
		if (Verbose) fprintf(stderr,"Drawing %d ROIs on slice %d\n",slice_array[image_number].number_of_rois,image_number+1);
		roip = slice_array[image_number].roilist;
		for (roi=0; roi<slice_array[image_number].number_of_rois; ++roi) {
		  first = TRUE;
		  vertp= roip->next_point;
		  if (ps_roi_draw) fprintf(ps_fp,"bplot\n");
		  if (Verbose) 
		    fprintf(stderr,"ROI %d number of vertices %d\n",roi+1,roip->number_of_points);
		  for (vert=0; vert<roip->number_of_points; ++vert) {
		    x1=(int)((vertp->x-crop_x)*xscale);
		    y1=(int)((vertp->y-crop_y)*yscale);
		    if (ps_roi_draw && first) fprintf(ps_fp,"%d %d moveto\n", ps_xoff+x1, ps_yoff+zHeight-y1);
		    if ( first == FALSE ) {
		      if ( Verbose && roip->number_of_points<25 ) 
			fprintf(stderr,"Drawing x1,y1,x2,y2: %d %d %d %d\n",x1,y1,x2,y2);
		      XDrawLine(theDisp,mainW,grGC,x1,y1,x2,y2);
		      if (ps_roi_draw) fprintf(ps_fp,"%d %d lineto\n", ps_xoff+x2, ps_yoff+zHeight-y2);
		    }
		    x2=x1;
		    y2=y1;
		    vertp = vertp->next_point;
		    first = FALSE;
		  }
		  roip=roip->next_roi;
		  if (ps_roi_draw) {
		    fprintf(ps_fp,"%f %f %f setrgbcolor\n",
			    (float)newC[gcol].red/65535.0,
			    (float)newC[gcol].green/65535.0,
			    (float)newC[gcol].blue/65535.0);
		    fprintf(ps_fp,"1 setlinewidth\n");
		    fprintf(ps_fp,"stroke\n");
		    fprintf(ps_fp,"eplot\n");
		  }
		}
		XDefineCursor(theDisp,mainW,mainCursor);
		XFlush(theDisp);
		return(TRUE);
  }
  else {
    return(FALSE);
  }
}	 

STATUS read_roi_file(int *num_slices, STACKROI_LIST slice_array[], char *infilename)
{
  FILE *ifd;

  /* open roi file */
  ifd = fopen(infilename,"rb");
  if ( ifd==NULL) {
    fprintf(stderr,"Error opening roi file %s\n",infilename);
    return(ERROR);
  }
  else {

    /* initialize memory locations */
    memset (slice_array,0,300*sizeof(STACKROI_LIST));
    
    /* load and fill in the ROI data structure */
    if (Verbose)
      fprintf (stderr,"reading data and building internal data structures\n");
    if ( load_region_of_interest(ifd,num_slices,slice_array) != OK ) { 
      fprintf(stderr,"Problems reading rois in %s.\n", infilename);
      return(ERROR);
    }
    else {
      return(OK);
    }
  }
}

/***********************************************************/
/* roi manipulation routines                               */
/* blatantly stolen from msproi.c                          */
/***********************************************************/

STATUS  load_region_of_interest(FILE *fp, int *slices,  STACKROI_LIST slice_array[])
{
  int 
    i,j,k,x,y,no_roi,no_points,
    t1,t2,t3,t4,roi_ident;

  i = 0;

  while (  (fscanf(fp,"%d",&no_roi))==1 ) {

    if (slice_array[i].roilist  != NULL) {  /* free up used data structure */
      delete_roi_list(slice_array,i);
    }

    for (k=0;k<no_roi;k++)  {

      create_roi_list_pointer(&(slice_array[i]));

      fscanf(fp,"%d\n",&roi_ident);

      if (slice_array[i].roilist==NULL) {
        fprintf(stderr,"ERROR tagging - object does not exist\n");
	return(ERROR);
      }
      else {
	slice_array[i].roilist->roi_id = roi_ident;

	fscanf(fp,"%d %d %d %dl", &t1,&t2,&t3,&t4); /* max & mins ignored, since set in store roi */
        
	fscanf(fp,"%d",&no_points);
	
	for (j=0;j<no_points;j++) {
	  fscanf(fp, "%d %d",&x,&y);
	  store_roi_position (x,y,&(slice_array[i]));
	}
	
	if (Verbose)
	  fprintf(stderr,"Loaded %d points for ROI %d type %d\n",slice_array[i].roilist->number_of_points,k+1,slice_array[i].roilist->roi_id); 
      }
    }
    if (Verbose) 
      fprintf(stderr,"Loaded %d ROIs for slice %d\n",slice_array[i].number_of_rois,i+1);
    ++i;
  } /* while not eof */
  
  *slices = i;

  return(OK);
}/*end*/

/************************************************************/
/*    create a node for region of interest list pointer     */
/************************************************************/

STATUS create_roi_list_pointer(STACKROI_LIST *slice_ptr)
{
  struct roi_list  *roi;

  roi= (struct roi_list *) malloc (sizeof(*roi));
  if (roi==NULL) {
    fprintf(stderr,"allocation failed in roi.\n");
    return(ERROR);
  }
  roi->number_of_points=0;
  roi->maxx=0;
  roi->maxy=0;
  roi->minx=255;
  roi->miny=255;
  roi->next_point=NULL;
  roi->next_roi=slice_ptr->roilist;
  slice_ptr->roilist=roi;
  slice_ptr->number_of_rois+=1;

  return(OK);
}

/***********************************************************/
/*    create a node to store the coordinate  mouse position*/
/***********************************************************/
STATUS store_roi_position(int x, int y, STACKROI_LIST *slice_ptr)
{
  if (slice_ptr->roilist == NULL) 
    create_roi_list_pointer(slice_ptr);

  add_point_to_roi(x,y,slice_ptr->roilist);
  return(OK);

}/*end*/

/***********************************************************/
/*    create an x,y coord node, and add the given x,y      */
/*    to the list pointed at by r                          */
/***********************************************************/
STATUS add_point_to_roi(int x,int y, ROI_LIST *r)
{
  struct point_list *point;
  
  point= (struct point_list *) malloc (sizeof(*point));
  if (point==NULL)  {
    fprintf(stderr,"point allocation failed in add point to roi.\n");
    return(ERROR);
  }

  point->x=x;
  point->y=y;
  point->next_point=r->next_point;

  r->next_point=point;
  r->number_of_points+=1;

  if (x < r->minx) r->minx = x;
  else if (x> r->maxx)  r->maxx = x;
  
  if (y< r->miny)  r->miny = y;
  else if (y> r->maxy)  r->maxy = y;

  return(OK);
}
/***********************************************************/

/*    roi list delete routines:                            */

/***********************************************************/


/************************************************************/
/* delete one roi structure and its associated point list   */
/* freeing up the allocated memory for it                   */
void free_one_roi(ROI_LIST *r, STACKROI_LIST *sp)
{
  POINT_LIST *p,*s;
  
  p=r->next_point;
  while (p!=NULL)  {
    s=p;
    p=p->next_point;
    s->next_point=NULL;
    free(s);
  }

  free(r);

  --(sp->number_of_rois);
}

void delete_one_roi(ROI_LIST *r, ROI_LIST **list, STACKROI_LIST *sp)
{
  struct roi_list *q;
  
  if (r==NULL)   {
    fprintf (stderr,"request to kill NULL roi.\n");
    return;
  }

  if (*list==NULL)  {
    fprintf (stderr,"request to kill roi on NULL list.\n");
    return;
  }

  q = *list; 
  if (q==r) { /* delete first element of roi list */
    *list = r->next_roi;
    free_one_roi(r,sp);
  }
  else   { /* search list for r */
    while (q->next_roi !=NULL  && q->next_roi != r) 
      q=q->next_roi;
    if (q->next_roi == r) {
      q->next_roi = r->next_roi;
      free_one_roi(r,sp);
    }
    else  {
      fprintf (stderr,"Selected roi not found in roi list.\n");
      return;
    }
  }
}


/***********************************************************/
/* delete all roi structures for this slice*/
/***********************************************************/
void delete_complete_roi_list(struct roi_list **list, STACKROI_LIST *sp)
{
  struct roi_list *q,*r;
  
  if (*list !=NULL) {
    q = *list;
    while (q!=NULL) {
      r = q;
      q=q->next_roi;
      delete_one_roi(r,list,sp);
    }
  }
}/*end*/

void delete_roi_list(STACKROI_LIST slice_array[], int slice_count)
{
  delete_complete_roi_list(&(slice_array[slice_count].roilist),
			   &(slice_array[slice_count]));
  slice_array[slice_count].roilist = NULL;
  slice_array[slice_count].number_of_rois=0;
}












