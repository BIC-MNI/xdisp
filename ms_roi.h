/* ----------------------------- MNI Header -----------------------------------
@NAME       : ms_roi.h
@DESCRIPTION: constants, typedefs and external vars for 
              roi list structure
@CREATED    : Wed Jul  7 20:50:20 EST 1993 Louis Collins
@MODIFIED   : 
---------------------------------------------------------------------------- */

#ifndef  DEF_MS_ROI
#define  DEF_MS_ROI


#define  MAX_POINTS    3000  /* Changed by Youpu 13/05/91*/ 

typedef struct point_list {
  int  x;
  int  y;
  struct point_list  *next_point;
} POINT_LIST, *POINT_LIST_PTR;


typedef struct roi_list { 
  int    roi_id;
  int    number_of_points;
  int    maxx, minx, miny, maxy;
  struct point_list  *next_point;
  struct roi_list    *next_roi;
} ROI_LIST, *ROI_LIST_PTR;

typedef struct stackroi_list {
  int number_of_rois;
  struct roi_list  *roilist;
} STACKROI_LIST, *STACKROI_LIST_PTR;

#endif
