/* ----------------------------- MNI Header -----------------------------------
@NAME       : roitominc.h
@DESCRIPTION: this file contains the list of include files and global variables
              needed for roitominc.c
@CREATED    : Wed Jul  7 20:50:20 EST 1993 Louis Collins
@MODIFIED   :
---------------------------------------------------------------------------- */

				/* include list */
#include <volume_io.h>
#include <ParseArgv.h>
#include <minc.h>
#include <time_stamp.h>
#include <print_error.h>

#include "r_roidat.h"
#include "ms_roi.h"
#include "r_roivars.h"

#include "fill.h"
#include "roi.h"
#include "msproi.h"


				/* globals */
int dumbglobalint;
int clobber_flag = FALSE;


STACKROI_LIST slice_array[300];

#define TRANSVERSE_VOL 0
#define SAGITTAL_VOL   1
#define CORONAL_VOL    2

char 
  *prog_name;

int  
  volume_type,
  ROI_FLAG,
  debug,
  verbose;
double 
  width;

static ArgvInfo argTable[] = {
 {"-no_clobber", ARGV_CONSTANT, (char *) FALSE, (char *) &clobber_flag,
     "Do not overwrite output file (default)."},
  {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &clobber_flag,
     "Overwrite output file."},
  {"-width", ARGV_FLOAT, (char *) 0, (char *) &width,
     "Slice width"},
  {"-roi_roi", ARGV_CONSTANT, (char *) FALSE , (char *) &ROI_FLAG,
     "roi file is from ROI program (default)"},
  {"-msp_roi", ARGV_CONSTANT, (char *) TRUE , (char *) &ROI_FLAG,
     "roi file is from MSP program"},
  {"-transverse", ARGV_CONSTANT, (char *)TRANSVERSE_VOL, 
     (char *) &volume_type,
     "Specify transverse slices (default)"},
  {"-sagittal", ARGV_CONSTANT, (char *)SAGITTAL_VOL, 
     (char *) &volume_type,
     "Specify sagittal slices"},
  {"-coronal", ARGV_CONSTANT, (char *)CORONAL_VOL, 
     (char *) &volume_type,
     "Specify coronal slices"},
  {NULL, ARGV_HELP, NULL, NULL,
     "Options for logging progress. Default = -verbose."},
  {"-verbose", ARGV_CONSTANT, (char *) TRUE, (char *) &verbose,
     "Write messages indicating progress"},
  {"-quiet", ARGV_CONSTANT, (char *) FALSE , (char *) &verbose,
     "Do not write log messages"},
  {"-debug", ARGV_CONSTANT, (char *) TRUE, (char *) &debug,
     "Print out debug info."},
  {NULL, ARGV_END, NULL, NULL, NULL}
};

