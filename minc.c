/* 
 *  Minc support routines (coutesy Peter Neelin)
 * 
 *  Multiple mods by B Pike.
 */

#include <xdisp.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_info
@INPUT      : infile - input file name
@OUTPUT     : volume_info - input volume information
@RETURNS    : Id of icv created
@DESCRIPTION: Routine to read volume information for a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int get_volume_info(char *infile, Volume_Info *volume_info)
{
   int icvid, mincid;

   /* Create and set up icv for input */
   icvid = miicv_create();
   setup_input_icv(icvid);

   /* Open the image file */
   mincid = miopen(infile, NC_NOWRITE);

   /* Attach the icv to the file */
   (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));

   /* Get dimension information */
   get_dimension_info(infile, icvid, volume_info);

   /* Get the volume min and max */
   (void) miicv_inqdbl(icvid, MI_ICV_NORM_MIN, &volume_info->minimum);
   (void) miicv_inqdbl(icvid, MI_ICV_NORM_MAX, &volume_info->maximum);
   (void) miicv_inqdbl(icvid, MI_ICV_VALID_MAX, &volume_info->valid_maximum);
   (void) miicv_inqdbl(icvid, MI_ICV_VALID_MIN, &volume_info->valid_minimum);
   return icvid;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_input_icv
@INPUT      : icvid - id of icv to set up
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up an icv
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void setup_input_icv(int icvid)
{
   /* Set desired type */
   (void) miicv_setint(icvid, MI_ICV_TYPE, NC_SHORT);
   (void) miicv_setstr(icvid, MI_ICV_SIGN, MI_SIGNED);

   /* Set range of values */
   (void) miicv_setint(icvid, MI_ICV_VALID_MIN, SHRT_MIN);
   (void) miicv_setint(icvid, MI_ICV_VALID_MAX, SHRT_MAX);

   /* Do normalization so that all pixels are on same scale */
   (void) miicv_setint(icvid, MI_ICV_DO_NORM, TRUE);

   /* Make sure that any out of range values are mapped to lowest value
      of type (for input only) */
   (void) miicv_setint(icvid, MI_ICV_DO_FILLVALUE, TRUE);

   /* We want to ensure that images have X, Y and Z dimensions in the
      positive direction, giving patient left on left and for drawing from
      bottom up. If we wanted patient right on left and drawing from
      top down, we would set to MI_ICV_NEGATIVE. */
   (void) miicv_setint(icvid, MI_ICV_DO_DIM_CONV, TRUE);
   (void) miicv_setint(icvid, MI_ICV_DO_SCALAR, TRUE);
   (void) miicv_setint(icvid, MI_ICV_XDIM_DIR, MI_ICV_NEGATIVE);
   (void) miicv_setint(icvid, MI_ICV_YDIM_DIR, MI_ICV_NEGATIVE);
   (void) miicv_setint(icvid, MI_ICV_ZDIM_DIR, MI_ICV_NEGATIVE);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_dimension_info
@INPUT      : infile - name of input file
              icvid - id of the image conversion variable
@OUTPUT     : volume - input volume data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to get dimension information from an input file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void get_dimension_info(char *infile, int icvid, 
                               Volume_Info *volume_info)
{
   int mincid, imgid, varid;
   int idim, ndims;
   int dim[MAX_VAR_DIMS];
   char *dimname;

   /* Get the minc file id and the image variable id */
   (void) miicv_inqint(icvid, MI_ICV_CDFID, &mincid);
   (void) miicv_inqint(icvid, MI_ICV_VARID, &imgid);
   if ((mincid == MI_ERROR) || (imgid == MI_ERROR)) {
      (void) fprintf(stderr, "File %s is not attached to an icv!\n",
                     infile);
      exit_xdisp(EXIT_FAILURE);
   }

   /* Get the list of dimensions subscripting the image variable */
   (void) ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL);
   (void) miicv_inqint(icvid, MI_ICV_NUM_DIMS, &ndims);
   volume_info->number_of_dimensions = ndims;

   /* Check that there are at least two dimensions */
   if (ndims < 2 || ndims > MAX_VAR_DIMS) {
      (void) fprintf(stderr, "File %s requires at least two and at most %d dimensions\n",
                     infile, MAX_VAR_DIMS);
      exit_xdisp(EXIT_FAILURE);
   }

   /* Initialize step and start to unknown values */
   for (idim=0; idim<ndims; idim++){
      volume_info->step[idim] = 0;
      volume_info->start[idim] = 0;
   }

   set_ncopts(0);

   /* Loop through dimensions, checking them and getting their sizes */
   for (idim=0; idim<ndims; idim++){
      /* Get pointers to the appropriate dimension size and name */
      dimname = volume_info->dimension_names[idim];
      (void)ncdiminq(mincid,dim[idim],dimname,&(volume_info->length[idim]));

      /* Get step and start directly from the variables */
      /* GBP: add direction_cosines and units */
      if ((varid = ncvarid(mincid, volume_info->dimension_names[idim]))
          != MI_ERROR){

         (void)miattget1(mincid, varid, MIstep, NC_DOUBLE,
                         &(volume_info->step[idim]));
         (void)miattget1(mincid, varid, MIstart, NC_DOUBLE,
                         &(volume_info->start[idim]));
         (void)miattget(mincid, varid, MIdirection_cosines, NC_DOUBLE, 3,
                         &(volume_info->direction_cosines[idim]), NULL);
         (void)miattgetstr(mincid, varid, MIunits, 20,
                         volume_info->dimension_units[idim]);

      }
   }

   /* get image plane start and step values */
   /* A is fastest dimension (screen x) and B is next fastest (screen y) */
   (void)miicv_inqdbl(icvid, MI_ICV_ADIM_START, &(volume_info->a_start));
   (void)miicv_inqdbl(icvid, MI_ICV_ADIM_STEP, &(volume_info->a_step));
   (void)miicv_inqdbl(icvid, MI_ICV_BDIM_START, &(volume_info->b_start));
   (void)miicv_inqdbl(icvid, MI_ICV_BDIM_STEP, &(volume_info->b_step));

   set_ncopts(NC_OPTS_VAL);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : close_volume
@INPUT      : icvid - id of open icv
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to close a minc file and free the associated icv
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : March 16, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void close_volume(int icvid)
{
   int mincid;

   /* Get the minc file id and close the file */
   set_ncopts(0);
   if (miicv_inqint(icvid, MI_ICV_CDFID, &mincid) != MI_ERROR) {
      (void) miclose(mincid);
   }
   set_ncopts(NC_OPTS_VAL);

   /* Free the icv */
   (void) miicv_free(icvid);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_slice
@INPUT      : icvid - id of icv to read
              volume_info - info for volume
              slice_num - number of slice to read in (counting from zero)
@OUTPUT     : image - image that is read in
@RETURNS    : (nothing)
@DESCRIPTION: Routine to read in an image.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void get_volume_slice(int icvid, Volume_Info *volume_info, 
                             int slice_num[], short *image)
{
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
   int idim, ndims;

   /* Get number of dimensions */
   ndims = volume_info->number_of_dimensions;

   /* Check slice_num */
   for (idim=0; idim<ndims-2; idim++){
      if (slice_num[idim] >= volume_info->length[idim]) {
         (void) fprintf(stderr, "%s %d is not in the file.\n",
                        volume_info->dimension_names[idim], slice_num[idim]);
         exit_xdisp(EXIT_FAILURE);
      }
   }

   /* Set up the start and count variables for reading the volume */
   (void) miset_coords(MAX_VAR_DIMS, 0, start);
   for (idim=0; idim<ndims-2; idim++){
      start[idim] = slice_num[idim];
      count[idim] = 1;
   }
   count[ndims-2] = volume_info->length[ndims-2];
   count[ndims-1] = volume_info->length[ndims-1];

   /* Read in the volume */
   (void) miicv_get(icvid, start, count, image);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_volume_info
@INPUT      : input_icvid - input file icvid (MI_ERROR means no input volume)
              outfile - output file name
              arg_string - string giving argument list
              volume_info - volume information
@OUTPUT     : (nothing)
@RETURNS    : icv of output file
@DESCRIPTION: Routine to save a 3-D volume, copying information
              from an optional input file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
int save_volume_info(int input_icvid, char *outfile, char *arg_string, 
                            Volume_Info *volume_info)
{
   int mincid, icvid, inmincid;

   /* Create output file */
   mincid = micreate(outfile, NC_NOCLOBBER);

   /* Open the input file if it is provided */
   inmincid = MI_ERROR;
   if (input_icvid != MI_ERROR) {
      (void) miicv_inqint(input_icvid, MI_ICV_CDFID, &inmincid);
   }

   /* Set up variables and put output file in data mode */
   setup_variables(inmincid, mincid, volume_info, arg_string);

   /* Create an icv and set it up */
   icvid = miicv_create();
   setup_output_icv(icvid);

   /* Attach the icv to the file */
   (void) miicv_attach(icvid, mincid, ncvarid(mincid, MIimage));

   return icvid;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_output_icv
@INPUT      : icvid - id of icv to set up
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up an icv
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void setup_output_icv(int icvid)
{
   /* Set desired type */
   (void) miicv_setint(icvid, MI_ICV_TYPE, NC_BYTE);
   (void) miicv_setstr(icvid, MI_ICV_SIGN, MI_UNSIGNED);

   /* Set range of values */
   (void) miicv_setint(icvid, MI_ICV_VALID_MIN, 0);
   (void) miicv_setint(icvid, MI_ICV_VALID_MAX, 255);

   /* No normalization so that pixels are scaled to the slice */
   (void) miicv_setint(icvid, MI_ICV_DO_NORM, FALSE);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_variables
@INPUT      : inmincid - id of input minc file (MI_ERROR if no file)
              mincid - id of output minc file
              volume_info - volume information
              arg_string - string giving argument list
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up variables in the output minc file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void setup_variables(int inmincid, int mincid, 
                            Volume_Info *volume_info, 
                            char *arg_string)
{
   int dim[MAX_VAR_DIMS], ndims, idim, varid;
   int excluded_vars[10], nexcluded;

   /* Create the dimensions */
   ndims = volume_info->number_of_dimensions;
   for(idim=0; idim<ndims; idim++){
      dim[idim] = ncdimdef(mincid, volume_info->dimension_names[idim],
                           volume_info->length[idim]);
   }

   /* If an input file is provided, copy all header info from that file except
      image, image-max, image-min */
   if (inmincid != MI_ERROR) {

      /* Look for the image variable and the image-max/min variables so that
         we can exclude them from the copy */
      nexcluded = 0;
      excluded_vars[nexcluded] = ncvarid(inmincid, MIimage);
      if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
      excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemax);
      if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;
      excluded_vars[nexcluded] = ncvarid(inmincid, MIimagemin);
      if (excluded_vars[nexcluded] != MI_ERROR) nexcluded++;

      /* Copy the variable definitions */
      (void) micopy_all_var_defs(inmincid, mincid, nexcluded, excluded_vars);

   }

   /* Set up the dimension variables. If the variable doesn't exist, create
      it (either no input file or variable did not exist in it). If the
      dimensions are not standard, then no variable is created. */

   for (idim=0; idim < ndims; idim++) {
      set_ncopts(0);
      varid = ncvarid(mincid, volume_info->dimension_names[idim]);
      if (varid == MI_ERROR) {
         varid = micreate_std_variable(mincid, 
                                       volume_info->dimension_names[idim],
                                       NC_LONG, 0, NULL);
      }
      set_ncopts(NC_OPTS_VAL);
      if (varid != MI_ERROR) {
         (void) miattputdbl(mincid, varid, MIstep, 
                            volume_info->step[idim]);
         (void) miattputdbl(mincid, varid, MIstart, 
                            volume_info->start[idim]);
      }
   }
   
   /* Create the image, image-max and image-min variables */
   setup_image_variables(inmincid, mincid, ndims, dim);

   /* Add the time stamp to the history */
   update_history(mincid, arg_string);

   /* Put the file in data mode */
   (void) ncendef(mincid);

   /* Copy over variable values */
   if (inmincid != MI_ERROR) {
      (void) micopy_all_var_values(inmincid, mincid,
                                   nexcluded, excluded_vars);
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : setup_image_variables
@INPUT      : inmincid - id of input minc file (MI_ERROR if no file)
              mincid - id of output minc file
              ndims - number of dimensions
              dim - list of dimension ids
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to set up image, image-max and image-min variables 
              in the output minc file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void setup_image_variables(int inmincid, int mincid, 
                                  int ndims, int dim[])
{
   int imgid, maxid, minid;
   static double valid_range[2] = {0.0, 255.0};

   /* Create the image variable, copy attributes, set the signtype attribute,
      set the valid range attribute and delete valid max/min attributes */
   imgid = micreate_std_variable(mincid, MIimage, NC_BYTE, ndims, dim);
   if (inmincid != MI_ERROR) {
      (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimage),
                             mincid, imgid);
      set_ncopts(0);
      (void) ncattdel(mincid, imgid, MIvalid_max);
      (void) ncattdel(mincid, imgid, MIvalid_min);
      set_ncopts(NC_OPTS_VAL);
   }
   (void) miattputstr(mincid, imgid, MIsigntype, MI_UNSIGNED);
   (void) ncattput(mincid, imgid, MIvalid_range, NC_DOUBLE, 2, valid_range);

   /* Create the image max and min variables (varying over slices) */
   maxid = micreate_std_variable(mincid, MIimagemax, NC_DOUBLE, 1, dim);
   minid = micreate_std_variable(mincid, MIimagemin, NC_DOUBLE, 1, dim);
   if (inmincid != MI_ERROR) {
      (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemax),
                             mincid, maxid);
      (void) micopy_all_atts(inmincid, ncvarid(inmincid, MIimagemin),
                             mincid, minid);
   }

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : update_history
@INPUT      : mincid - id of output minc file
              arg_string - string giving list of arguments
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to update the history global variable in the output 
              minc file
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void update_history(int mincid, char *arg_string)
{
   nc_type datatype;
   int att_length;
   char *string;

   /* Get the history attribute length */
   set_ncopts(0);
   if ((ncattinq(mincid, NC_GLOBAL, MIhistory, &datatype,
                 &att_length) == MI_ERROR) ||
       (datatype != NC_CHAR))
      att_length = 0;
   att_length += strlen(arg_string) + 1;

   /* Allocate a string and get the old history */
   string = MALLOC(att_length);
   string[0] = '\0';
   (void) miattgetstr(mincid, NC_GLOBAL, MIhistory, att_length, 
                      string);
   set_ncopts(NC_OPTS_VAL);

   /* Add the new command and put the new history. */
   (void) strcat(string, arg_string);
   (void) miattputstr(mincid, NC_GLOBAL, MIhistory, string);
   FREE(string);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : save_volume_slice
@INPUT      : icvid - id of icv to write
              volume_info - volume information (minimum and maximum are
                 ignored)
              slice_num - number of slice to write
              image - image to write
              slice_min - minimum real value for slice
              slice_max - maximum real value for slice
@OUTPUT     : (nothing)
@RETURNS    : (nothing)
@DESCRIPTION: Routine to write out a slice.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
void save_volume_slice(int icvid, Volume_Info *volume_info, 
                              int slice_num[], unsigned char *image,
                              double slice_min, double slice_max)
{
   int mincid;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
   int  idim, ndims;

   /* Get the minc file id */
   (void) miicv_inqint(icvid, MI_ICV_CDFID, &mincid);
   ndims = volume_info->number_of_dimensions;

   /* Set up the start and count variables for writing the volume */
   (void) miset_coords(MAX_VAR_DIMS, 0, start);

   for(idim=0; idim<ndims-2; idim++){
      start[idim] = slice_num[idim];
      count[idim] = 1;
   }
   count[ndims-2] = volume_info->length[ndims-2];
   count[ndims-1] = volume_info->length[ndims-1];  

   /* Write out the slice min and max */
   (void) mivarput1(mincid, ncvarid(mincid, MIimagemin), start, NC_DOUBLE,
                    NULL, &slice_min);
   (void) mivarput1(mincid, ncvarid(mincid, MIimagemax), start, NC_DOUBLE,
                    NULL, &slice_max);

   /* Write out the volume */
   (void) miicv_put(icvid, start, count, image);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_volume_data
@INPUT      : icvid - id of icv to set up
@OUTPUT     : volume data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to read in the volume data.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 26, 1993 (Peter Neelin (mods B Pike Mar 30, 1994))
@MODIFIED   : 
---------------------------------------------------------------------------- */
void read_volume_data(int icvid, Volume_Info *volume_info, short *volume)
{
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
   int  idim, ndims;

   /* Get number of dimensions */
   ndims = volume_info->number_of_dimensions;

   /* Set up the start and count variables for reading the volume */
   (void) miset_coords(MAX_VAR_DIMS, 0, start);

   for(idim=0; idim<ndims; idim++){
      count[idim] = volume_info->length[idim];
   }
/*
   if (ndims > 3){
      for(idim=0; idim<ndims-3; idim++){
         start[idim] = slider_image[idim];
         count[idim] = 1;
      }
   }
*/
   /* Read in the volume */
   (void) miicv_get(icvid, start, count, volume);
}


