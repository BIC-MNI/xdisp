/*
 * nneighbour.c
 *
 * perform arbitrary nearest neighbour interpolation on a 2D array
 *
 * Usage: nneighbour_x_to_x ( original_data, org_xsize, org_ysize, 
 *                            new_data,      new_xsize, new_ysize );
 *
 * original_data: pointer to data
 * org_xsize:     int number of cols in original_data
 * org_ysize:     int number of rows in original_data
 * new_data: 	  pointer to data (must already be malloc'd)
 * new_xsize:     int number of cols to produce
 * new_ysize:     int number of rows to produce
 *
 * nneighbour_byte_to_byte()
 * nneighbour_short_to_short()
 * nneighbour_int_to_int()
 * nneighbour_long_to_long()
 * nneighbour_float_to_float()
 * nneighbour_double_to_double()
 * nneighbour_rgbp_to_rgbp()
 *
 * last update: Dec 1, 1996 GBP
 *
 * Copyright (c), Bruce Pike 1997
 */

#include "xdisp.h"

/*------------------- nneighbour_byte_to_byte ----------------*/
void nneighbour_byte_to_byte (byte *oim, int ox, int oy, 
			      byte *nim, int nx, int ny)
{
  int 	i, j, ind;
  int 	*row, *col;
  float	xscale, yscale;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    memcpy(nim,oim,ox*oy);
    return;
  }

  /* special case... half size */
  if (ox==2*nx && oy==2*ny) {
    ind = 0;
    for (j=0; j<ny; j++) {
      for (i=0; i<nx; i++) {
	nim[ind++]=oim[(j*2)*ox + 2*i];
      }
    }
    return;
  }

  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  row = (int *)malloc(ny * sizeof(int));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    col[i] = (int) floor((i * xscale) + 0.5);
    col[i] = (col[i]<=(ox-1) ? col[i] : ox-1);
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    row[j] = (int) floor((j * yscale) + 0.5);
    row[j] = (row[j]<=(oy-1) ? row[j] : oy-1);
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny; j++) {
    for (i=0; i<nx; i++) {
      nim[ind++] = oim[row[j]*ox+col[i]];
    }
  }
   
  /* free memory */   
  free (row);
  free (col);
}


/*------------------- nneighbour_short_to_short ----------------*/
void nneighbour_short_to_short (short *oim, int ox, int oy, 
				short *nim, int nx, int ny)
{
  int 	i, j, ind;
  int 	*row, *col;
  float	xscale, yscale;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    memcpy(nim,oim,ox*oy*sizeof(short));
    return;
  }

  /* special case... half size */
  if (ox==2*nx && oy==2*ny) {
    ind = 0;
    for (j=0; j<ny; j++) {
      for (i=0; i<nx; i++) {
	nim[ind++]=oim[(j*2)*ox + 2*i];
      }
    }
    return;
  }

  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  row = (int *)malloc(ny * sizeof(int));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    col[i] = (int) floor((i * xscale) + 0.5);
    col[i] = (col[i]<=(ox-1) ? col[i] : ox-1);
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    row[j] = (int) floor((j * yscale) + 0.5);
    row[j] = (row[j]<=(oy-1) ? row[j] : oy-1);
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny; j++) {
    for (i=0; i<nx; i++) {
      nim[ind++] = oim[row[j]*ox+col[i]];
    }
  }
   
  /* free memory */   
  free (row);
  free (col);
}

 
/*------------------- nneighbour_int_to_int ----------------*/
void nneighbour_int_to_int (int *oim, int ox, int oy, 
			    int *nim, int nx, int ny)
{
  int 	i, j, ind;
  int 	*row, *col;
  float	xscale, yscale;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    memcpy(nim,oim,ox*oy*sizeof(int));
    return;
  }

  /* special case... half size */
  if (ox==2*nx && oy==2*ny) {
    ind = 0;
    for (j=0; j<ny; j++) {
      for (i=0; i<nx; i++) {
	nim[ind++]=oim[(j*2)*ox + 2*i];
      }
    }
    return;
  }

  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  row = (int *)malloc(ny * sizeof(int));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    col[i] = (int) floor((i * xscale) + 0.5);
    col[i] = (col[i]<=(ox-1) ? col[i] : ox-1);
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    row[j] = (int) floor((j * yscale) + 0.5);
    row[j] = (row[j]<=(oy-1) ? row[j] : oy-1);
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny; j++) {
    for (i=0; i<nx; i++) {
      nim[ind++] = oim[row[j]*ox+col[i]];
    }
  }
   
  /* free memory */   
  free (row);
  free (col);
}

 
/*------------------- nneighbour_long_to_long ----------------*/
void nneighbour_long_to_long (int32_t *oim, int ox, int oy,
			      int32_t *nim, int nx, int ny)
{
  int 	i, j, ind;
  int 	*row, *col;
  float	xscale, yscale;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    memcpy(nim,oim,ox*oy*sizeof(int32_t));
    return;
  }

  /* special case... half size */
  if (ox==2*nx && oy==2*ny) {
    ind = 0;
    for (j=0; j<ny; j++) {
      for (i=0; i<nx; i++) {
	nim[ind++]=oim[(j*2)*ox + 2*i];
      }
    }
    return;
  }

  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  row = (int *)malloc(ny * sizeof(int));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    col[i] = (int) floor((i * xscale) + 0.5);
    col[i] = (col[i]<=(ox-1) ? col[i] : ox-1);
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    row[j] = (int) floor((j * yscale) + 0.5);
    row[j] = (row[j]<=(oy-1) ? row[j] : oy-1);
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny; j++) {
    for (i=0; i<nx; i++) {
      nim[ind++] = oim[row[j]*ox+col[i]];
    }
  }
   
  /* free memory */   
  free (row);
  free (col);
}


/*------------------- nneighbour_float_to_float ----------------*/
void nneighbour_float_to_float (float *oim, int ox, int oy, 
				float *nim, int nx, int ny)
{
  int 	i, j, ind;
  int 	*row, *col;
  float	xscale, yscale;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    memcpy(nim,oim,ox*oy*sizeof(float));
    return;
  }

  /* special case... half size */
  if (ox==2*nx && oy==2*ny) {
    ind = 0;
    for (j=0; j<ny; j++) {
      for (i=0; i<nx; i++) {
	nim[ind++]=oim[(j*2)*ox + 2*i];
      }
    }
    return;
  }

  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  row = (int *)malloc(ny * sizeof(int));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    col[i] = (int) floor((i * xscale) + 0.5);
    col[i] = (col[i]<=(ox-1) ? col[i] : ox-1);
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    row[j] = (int) floor((j * yscale) + 0.5);
    row[j] = (row[j]<=(oy-1) ? row[j] : oy-1);
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny; j++) {
    for (i=0; i<nx; i++) {
      nim[ind++] = oim[row[j]*ox+col[i]];
    }
  }
   
  /* free memory */   
  free (row);
  free (col);
}

 
/*------------------- nneighbour_double_to_double ----------------*/
void nneighbour_double_to_double (double *oim, int ox, int oy, 
				  double *nim, int nx, int ny)
{
  int 	i, j, ind;
  int 	*row, *col;
  float	xscale, yscale;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    memcpy(nim,oim,ox*oy*sizeof(double));
    return;
  }

  /* special case... half size */
  if (ox==2*nx && oy==2*ny) {
    ind = 0;
    for (j=0; j<ny; j++) {
      for (i=0; i<nx; i++) {
	nim[ind++]=oim[(j*2)*ox + 2*i];
      }
    }
    return;
  }

  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  row = (int *)malloc(ny * sizeof(int));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    col[i] = (int) floor((i * xscale) + 0.5);
    col[i] = (col[i]<=(ox-1) ? col[i] : ox-1);
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    row[j] = (int) floor((j * yscale) + 0.5);
    row[j] = (row[j]<=(oy-1) ? row[j] : oy-1);
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny; j++) {
    for (i=0; i<nx; i++) {
      nim[ind++] = oim[row[j]*ox+col[i]];
    }
  }
   
  /* free memory */   
  free (row);
  free (col);
}

/*------------------- nneighbour_rgbp_to_rgbp ----------------*/
void nneighbour_rgbp_to_rgbp (uint32_t *oim, int ox, int oy,
			      uint32_t *nim, int nx, int ny)
{
  int 	i, j, ind;
  int 	*row, *col;
  float	xscale, yscale;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    memcpy(nim,oim,ox*oy*sizeof(uint32_t));
    return;
  }

  /* special case... half size */
  if (ox==2*nx && oy==2*ny) {
    ind = 0;
    for (j=0; j<ny; j++) {
      for (i=0; i<nx; i++) {
	nim[ind++]=oim[(j*2)*ox + 2*i];
      }
    }
    return;
  }

  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  row = (int *)malloc(ny * sizeof(int));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    col[i] = (int) floor((i * xscale) + 0.5);
    col[i] = (col[i]<=(ox-1) ? col[i] : ox-1);
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    row[j] = (int) floor((j * yscale) + 0.5);
    row[j] = (row[j]<=(oy-1) ? row[j] : oy-1);
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny; j++) {
    for (i=0; i<nx; i++) {
      nim[ind++] = oim[row[j]*ox+col[i]];
    }
  }
   
  /* free memory */   
  free (row);
  free (col);
}
