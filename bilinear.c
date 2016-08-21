/*
 * bilinear.c
 *
 * perform arbitrary bilinear interpolation on a 2D array
 *
 * Usage: bilinear_x_to_x ( original_data, org_xsize, org_ysize, 
 *                          new_data,      new_xsize, new_ysize );
 *
 * original_data: pointer to data
 * org_xsize:     int number of cols in original_data
 * org_ysize:     int number of rows in original_data
 * new_data: 	  pointer to data (must already be malloc'd)
 * new_xsize:     int number of cols to produce
 * new_ysize:     int number of rows to produce
 *
 * bilinear_byte_to_byte()
 * bilinear_short_to_short()
 * bilinear_int_to_int()
 * bilinear_long_to_long()
 * bilinear_float_to_float()
 * bilinear_double_to_double()
 * bilinear_rgbp_to_rgbp()
 *
 * last update: Nov 16, 1996 GBP
 *
 * Copyright (c), Bruce Pike 1993-1997
 */

#include "xdisp.h"

/*------------------- bilinear_byte_to_byte ----------------*/
void bilinear_byte_to_byte (byte *oim, int ox, int oy, 
			    byte *nim, int nx, int ny)
{
  int 	i, j, jo, jn, k1, k2, ind, row;
  int 	*r1, *r2, *col;
  float 	tmp1, tmp2, tmp3;
  float	xscale, yscale, dx, dy;
  float 	*fx1, *fx2, *fy1, *fy2;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    for (i=0; i<nx*ny; i++) nim[i]=oim[i];
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

  /* another special case... double size */
  if (nx==2*ox && ny==2*oy) {

    /* do the rows first */
    for (j=0; j<oy; j++) {
      jo = j*ox;
      jn = j*2*nx;
      for (i=0; i<ox-1; i++) {
	nim[jn + (i*2)]    =  oim[jo + i];
	nim[jn + (i*2)+1]  = (oim[jo + i] + oim[jo + i + 1])>>1;
      }
      nim[jn + nx - 2] =  oim[jo + (ox-1)];
      nim[jn + nx - 1] =  oim[jo + (ox-1)];
    }

    /* now the columns */
    for (i=0; i<nx; i++) {
      for (j=0; j<ny-2; j+=2) {
	nim[(j+1)*nx + i] = (nim[(j*nx)+i]+nim[(j+2)*nx+i])>>1;
      }
      nim[(ny-2)*nx + i] = nim[((oy-1)*2*nx) + i];
      nim[(ny-1)*nx + i] = nim[((oy-1)*2*nx) + i];
    }
    return;
  }
   
  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  r1  = (int *)malloc(ny * sizeof(int));
  r2  = (int *)malloc(ny * sizeof(int));
  fx1 = (float *)malloc(nx * sizeof(float));
  fx2 = (float *)malloc(nx * sizeof(float));
  fy1 = (float *)malloc(ny * sizeof(float));
  fy2 = (float *)malloc(ny * sizeof(float));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    dx     = i * xscale;
    col[i] = (int) dx;
    fx2[i] = dx - col[i];
    fx1[i] = 1.0 - fx2[i];
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    dy     = j * yscale;
    row    = (int) dy;
    r1[j]  = row * ox;
    r2[j]  = (row+1) * ox;
    fy2[j] = dy - row;
    fy1[j] = 1.0 - fy2[j];
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny-1; j++) {
    for (i=0; i<nx-1; i++) {
      k1 = r1[j] + col[i];
      k2 = r2[j] + col[i];
      tmp1 = oim[k1]*fx1[i] + oim[k1+1]*fx2[i];
      tmp2 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
      tmp3 = tmp1*fy1[j] + tmp2*fy2[j];
      nim[ind++] = (byte) (tmp3+0.5);
    }
    /* last column */
    k1 = r1[j] + col[nx-1];
    k2 = r2[j] + col[nx-1];
    tmp3 = oim[k1]*fy1[j] + oim[k2]*fy2[j];
    nim[ind++] = (byte) (tmp3+0.5);
  }
  /* last row */
  for (i=0; i<nx-1; i++) {
    k2 = (oy-1)*ox + col[i];
    tmp3 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
    nim[ind++] = (byte) (tmp3+0.5);
  }
  /* last point */
  nim[ind++] = oim[(ox*oy)-1];
   
  /* free memory */   
  free (fx1);
  free (fx2);
  free (fy1);
  free (fy2);
  free (r1);
  free (r2);
  free (col);
}



/*------------------- bilinear_short_to_short ----------------*/
void bilinear_short_to_short (short *oim, int ox, int oy, 
			      short *nim, int nx, int ny)
{
  int 	i, j, jo, jn, k1, k2, ind, row;
  int 	*r1, *r2, *col;
  float 	tmp1, tmp2, tmp3;
  float	xscale, yscale, dx, dy;
  float 	*fx1, *fx2, *fy1, *fy2;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    for (i=0; i<nx*ny; i++) nim[i]=oim[i];
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

  /* another special case... double size */
  if (nx==2*ox && ny==2*oy) {

    /* do the rows first */
    for (j=0; j<oy; j++) {
      jo = j*ox;
      jn = j*2*nx;
      for (i=0; i<ox-1; i++) {
	nim[jn + (i*2)]    =  oim[jo + i];
	nim[jn + (i*2)+1]  = (oim[jo + i] + oim[jo + i + 1])>>1;
      }
      nim[jn + nx - 2] =  oim[jo + (ox-1)];
      nim[jn + nx - 1] =  oim[jo + (ox-1)];
    }

    /* now the columns */
    for (i=0; i<nx; i++) {
      for (j=0; j<ny-2; j+=2) {
	nim[(j+1)*nx + i] = (nim[(j*nx)+i]+nim[(j+2)*nx+i])>>1;
      }
      nim[(ny-2)*nx + i] = nim[((oy-1)*2*nx) + i];
      nim[(ny-1)*nx + i] = nim[((oy-1)*2*nx) + i];
    }
    return;
  }
   
  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  r1  = (int *)malloc(ny * sizeof(int));
  r2  = (int *)malloc(ny * sizeof(int));
  fx1 = (float *)malloc(nx * sizeof(float));
  fx2 = (float *)malloc(nx * sizeof(float));
  fy1 = (float *)malloc(ny * sizeof(float));
  fy2 = (float *)malloc(ny * sizeof(float));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    dx     = i * xscale;
    col[i] = (int) dx;
    fx2[i] = dx - col[i];
    fx1[i] = 1.0 - fx2[i];
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    dy     = j * yscale;
    row    = (int) dy;
    r1[j]  = row * ox;
    r2[j]  = (row+1) * ox;
    fy2[j] = dy - row;
    fy1[j] = 1.0 - fy2[j];
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny-1; j++) {
    for (i=0; i<nx-1; i++) {
      k1 = r1[j] + col[i];
      k2 = r2[j] + col[i];
      tmp1 = oim[k1]*fx1[i] + oim[k1+1]*fx2[i];
      tmp2 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
      tmp3 = tmp1*fy1[j] + tmp2*fy2[j];
      nim[ind++] = (short) floor(tmp3+0.5);
    }
    /* last column */
    k1 = r1[j] + col[nx-1];
    k2 = r2[j] + col[nx-1];
    tmp3 = oim[k1]*fy1[j] + oim[k2]*fy2[j];
    nim[ind++] = (short) floor(tmp3+0.5);
  }
  /* last row */
  for (i=0; i<nx-1; i++) {
    k2 = (oy-1)*ox + col[i];
    tmp3 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
    nim[ind++] = (short) floor(tmp3+0.5);
  }
  /* last point */
  nim[ind++] = oim[(ox*oy)-1];
   
  /* free memory */   
  free (fx1);
  free (fx2);
  free (fy1);
  free (fy2);
  free (r1);
  free (r2);
  free (col);
}



/*------------------- bilinear_int_to_int ----------------*/
void bilinear_int_to_int (int *oim, int ox, int oy, 
			  int *nim, int nx, int ny)
{
  int 	i, j, jo, jn, k1, k2, ind, row;
  int 	*r1, *r2, *col;
  float 	tmp1, tmp2, tmp3;
  float	xscale, yscale, dx, dy;
  float 	*fx1, *fx2, *fy1, *fy2;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    for (i=0; i<nx*ny; i++) nim[i]=oim[i];
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

  /* another special case... double size */
  if (nx==2*ox && ny==2*oy) {

    /* do the rows first */
    for (j=0; j<oy; j++) {
      jo = j*ox;
      jn = j*2*nx;
      for (i=0; i<ox-1; i++) {
	nim[jn + (i*2)]    =  oim[jo + i];
	nim[jn + (i*2)+1]  = (oim[jo + i] + oim[jo + i + 1])>>1;
      }
      nim[jn + nx - 2] =  oim[jo + (ox-1)];
      nim[jn + nx - 1] =  oim[jo + (ox-1)];
    }

    /* now the columns */
    for (i=0; i<nx; i++) {
      for (j=0; j<ny-2; j+=2) {
	nim[(j+1)*nx + i] = (nim[(j*nx)+i]+nim[(j+2)*nx+i])>>1;
      }
      nim[(ny-2)*nx + i] = nim[((oy-1)*2*nx) + i];
      nim[(ny-1)*nx + i] = nim[((oy-1)*2*nx) + i];
    }
    return;
  }
   
  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  r1  = (int *)malloc(ny * sizeof(int));
  r2  = (int *)malloc(ny * sizeof(int));
  fx1 = (float *)malloc(nx * sizeof(float));
  fx2 = (float *)malloc(nx * sizeof(float));
  fy1 = (float *)malloc(ny * sizeof(float));
  fy2 = (float *)malloc(ny * sizeof(float));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    dx     = i * xscale;
    col[i] = (int) dx;
    fx2[i] = dx - col[i];
    fx1[i] = 1.0 - fx2[i];
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    dy     = j * yscale;
    row    = (int) dy;
    r1[j]  = row * ox;
    r2[j]  = (row+1) * ox;
    fy2[j] = dy - row;
    fy1[j] = 1.0 - fy2[j];
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny-1; j++) {
    for (i=0; i<nx-1; i++) {
      k1 = r1[j] + col[i];
      k2 = r2[j] + col[i];
      tmp1 = oim[k1]*fx1[i] + oim[k1+1]*fx2[i];
      tmp2 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
      tmp3 = tmp1*fy1[j] + tmp2*fy2[j];
      nim[ind++] = (int) floor(tmp3+0.5);
    }
    /* last column */
    k1 = r1[j] + col[nx-1];
    k2 = r2[j] + col[nx-1];
    tmp3 = oim[k1]*fy1[j] + oim[k2]*fy2[j];
    nim[ind++] = (int) floor(tmp3+0.5);
  }
  /* last row */
  for (i=0; i<nx-1; i++) {
    k2 = (oy-1)*ox + col[i];
    tmp3 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
    nim[ind++] = (int) floor(tmp3+0.5);
  }
  /* last point */
  nim[ind++] = oim[(ox*oy)-1];
   
  /* free memory */   
  free (fx1);
  free (fx2);
  free (fy1);
  free (fy2);
  free (r1);
  free (r2);
  free (col);
}

/*------------------- bilinear_long_to_long ----------------*/
void bilinear_long_to_long (int32_t *oim, int ox, int oy, 
			    int32_t *nim, int nx, int ny)
{
  int 	i, j, jo, jn, k1, k2, ind, row;
  int 	*r1, *r2, *col;
  float 	tmp1, tmp2, tmp3;
  float	xscale, yscale, dx, dy;
  float 	*fx1, *fx2, *fy1, *fy2;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    for (i=0; i<nx*ny; i++) nim[i]=oim[i];
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

  /* another special case... double size */
  if (nx==2*ox && ny==2*oy) {

    /* do the rows first */
    for (j=0; j<oy; j++) {
      jo = j*ox;
      jn = j*2*nx;
      for (i=0; i<ox-1; i++) {
	nim[jn + (i*2)]    =  oim[jo + i];
	nim[jn + (i*2)+1]  = (oim[jo + i] + oim[jo + i + 1])>>1;
      }
      nim[jn + nx - 2] =  oim[jo + (ox-1)];
      nim[jn + nx - 1] =  oim[jo + (ox-1)];
    }

    /* now the columns */
    for (i=0; i<nx; i++) {
      for (j=0; j<ny-2; j+=2) {
	nim[(j+1)*nx + i] = (nim[(j*nx)+i]+nim[(j+2)*nx+i])>>1;
      }
      nim[(ny-2)*nx + i] = nim[((oy-1)*2*nx) + i];
      nim[(ny-1)*nx + i] = nim[((oy-1)*2*nx) + i];
    }
    return;
  }
   
  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  r1  = (int *)malloc(ny * sizeof(int));
  r2  = (int *)malloc(ny * sizeof(int));
  fx1 = (float *)malloc(nx * sizeof(float));
  fx2 = (float *)malloc(nx * sizeof(float));
  fy1 = (float *)malloc(ny * sizeof(float));
  fy2 = (float *)malloc(ny * sizeof(float));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    dx     = i * xscale;
    col[i] = (int) dx;
    fx2[i] = dx - col[i];
    fx1[i] = 1.0 - fx2[i];
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    dy     = j * yscale;
    row    = (int) dy;
    r1[j]  = row * ox;
    r2[j]  = (row+1) * ox;
    fy2[j] = dy - row;
    fy1[j] = 1.0 - fy2[j];
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny-1; j++) {
    for (i=0; i<nx-1; i++) {
      k1 = r1[j] + col[i];
      k2 = r2[j] + col[i];
      tmp1 = oim[k1]*fx1[i] + oim[k1+1]*fx2[i];
      tmp2 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
      tmp3 = tmp1*fy1[j] + tmp2*fy2[j];
      nim[ind++] = (long) floor(tmp3+0.5);
    }
    /* last column */
    k1 = r1[j] + col[nx-1];
    k2 = r2[j] + col[nx-1];
    tmp3 = oim[k1]*fy1[j] + oim[k2]*fy2[j];
    nim[ind++] = (long) floor(tmp3+0.5);
  }
  /* last row */
  for (i=0; i<nx-1; i++) {
    k2 = (oy-1)*ox + col[i];
    tmp3 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
    nim[ind++] = (long) floor(tmp3+0.5);
  }
  /* last point */
  nim[ind++] = oim[(ox*oy)-1];
   
  /* free memory */   
  free (fx1);
  free (fx2);
  free (fy1);
  free (fy2);
  free (r1);
  free (r2);
  free (col);
}

/*------------------- bilinear_float_to_float ----------------*/
void bilinear_float_to_float (float *oim, int ox, int oy, 
			      float *nim, int nx, int ny)
{
  int 	        i, j, jo, jn, k1, k2, ind, row;
  int 	        *r1, *r2, *col;
  float 	tmp1, tmp2, tmp3;
  float	        xscale, yscale, dx, dy;
  float 	*fx1, *fx2, *fy1, *fy2;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    for (i=0; i<nx*ny; i++) nim[i]=oim[i];
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

  /* another special case... double size */
  if (nx==2*ox && ny==2*oy) {

    /* do the rows first */
    for (j=0; j<oy; j++) {
      jo = j*ox;
      jn = j*2*nx;
      for (i=0; i<ox-1; i++) {
	nim[jn + (i*2)]    =  oim[jo + i];
	nim[jn + (i*2)+1]  = (oim[jo + i] + oim[jo + i + 1])/2.0;
      }
      nim[jn + nx - 2] =  oim[jo + (ox-1)];
      nim[jn + nx - 1] =  oim[jo + (ox-1)];
    }

    /* now the columns */
    for (i=0; i<nx; i++) {
      for (j=0; j<ny-2; j+=2) {
	nim[(j+1)*nx + i] = (nim[(j*nx)+i]+nim[(j+2)*nx+i])/2.0;
      }
      nim[(ny-2)*nx + i] = nim[((oy-1)*2*nx) + i];
      nim[(ny-1)*nx + i] = nim[((oy-1)*2*nx) + i];
    }
    return;
  }
   
  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  r1  = (int *)malloc(ny * sizeof(int));
  r2  = (int *)malloc(ny * sizeof(int));
  fx1 = (float *)malloc(nx * sizeof(float));
  fx2 = (float *)malloc(nx * sizeof(float));
  fy1 = (float *)malloc(ny * sizeof(float));
  fy2 = (float *)malloc(ny * sizeof(float));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    dx     = i * xscale;
    col[i] = (int) dx;
    fx2[i] = dx - col[i];
    fx1[i] = 1.0 - fx2[i];
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    dy     = j * yscale;
    row    = (int) dy;
    r1[j]  = row * ox;
    r2[j]  = (row+1) * ox;
    fy2[j] = dy - row;
    fy1[j] = 1.0 - fy2[j];
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny-1; j++) {
    for (i=0; i<nx-1; i++) {
      k1 = r1[j] + col[i];
      k2 = r2[j] + col[i];
      tmp1 = oim[k1]*fx1[i] + oim[k1+1]*fx2[i];
      tmp2 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
      tmp3 = tmp1*fy1[j] + tmp2*fy2[j];
      nim[ind++] = tmp3;
    }
    /* last column */
    k1 = r1[j] + col[nx-1];
    k2 = r2[j] + col[nx-1];
    tmp3 = oim[k1]*fy1[j] + oim[k2]*fy2[j];
    nim[ind++] = tmp3;
  }
  /* last row */
  for (i=0; i<nx-1; i++) {
    k2 = (oy-1)*ox + col[i];
    tmp3 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
    nim[ind++] = tmp3;
  }
  /* last point */
  nim[ind++] = oim[(ox*oy)-1];
   
  /* free memory */   
  free (fx1);
  free (fx2);
  free (fy1);
  free (fy2);
  free (r1);
  free (r2);
  free (col);
}


/*------------------- bilinear_double_to_double ----------------*/
void bilinear_double_to_double (double *oim, int ox, int oy, 
				double *nim, int nx, int ny)
{
  int 	i, j, jo, jn, k1, k2, ind, row;
  int 	*r1, *r2, *col;
  double 	tmp1, tmp2, tmp3;
  double	xscale, yscale, dx, dy;
  double 	*fx1, *fx2, *fy1, *fy2;

  /* very special case... just copy data */
  if (ox==nx && oy==ny) {
    for (i=0; i<nx*ny; i++) nim[i]=oim[i];
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

  /* another special case... double size */
  if (nx==2*ox && ny==2*oy) {

    /* do the rows first */
    for (j=0; j<oy; j++) {
      jo = j*ox;
      jn = j*2*nx;
      for (i=0; i<ox-1; i++) {
	nim[jn + (i*2)]    =  oim[jo + i];
	nim[jn + (i*2)+1]  = (oim[jo + i] + oim[jo + i + 1])/2.0;
      }
      nim[jn + nx - 2] =  oim[jo + (ox-1)];
      nim[jn + nx - 1] =  oim[jo + (ox-1)];
    }

    /* now the columns */
    for (i=0; i<nx; i++) {
      for (j=0; j<ny-2; j+=2) {
	nim[(j+1)*nx + i] = (nim[(j*nx)+i]+nim[(j+2)*nx+i])/2.0;
      }
      nim[(ny-2)*nx + i] = nim[((oy-1)*2*nx) + i];
      nim[(ny-1)*nx + i] = nim[((oy-1)*2*nx) + i];
    }
    return;
  }
   
  /* all other cases */
  /* allocate memory */
  col = (int *)malloc(nx * sizeof(int));
  r1  = (int *)malloc(ny * sizeof(int));
  r2  = (int *)malloc(ny * sizeof(int));
  fx1 = (double *)malloc(nx * sizeof(float));
  fx2 = (double *)malloc(nx * sizeof(float));
  fy1 = (double *)malloc(ny * sizeof(float));
  fy2 = (double *)malloc(ny * sizeof(float));

  /* scale factors */
  xscale = (float)(ox-1)/(float)(nx-1);
  yscale = (float)(oy-1)/(float)(ny-1);

  /* calculate the row index values */        
  for (i=0; i<nx; i++) {
    dx     = i * xscale;
    col[i] = (int) dx;
    fx2[i] = dx - col[i];
    fx1[i] = 1.0 - fx2[i];
  }

  /* calculate the col index values */        
  for (j=0; j<ny; j++) {
    dy     = j * yscale;
    row    = (int) dy;
    r1[j]  = row * ox;
    r2[j]  = (row+1) * ox;
    fy2[j] = dy - row;
    fy1[j] = 1.0 - fy2[j];
  }

  /* do the interpolation */
  ind = 0;
  for (j=0; j<ny-1; j++) {
    for (i=0; i<nx-1; i++) {
      k1 = r1[j] + col[i];
      k2 = r2[j] + col[i];
      tmp1 = oim[k1]*fx1[i] + oim[k1+1]*fx2[i];
      tmp2 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
      tmp3 = tmp1*fy1[j] + tmp2*fy2[j];
      nim[ind++] = tmp3;
    }
    /* last column */
    k1 = r1[j] + col[nx-1];
    k2 = r2[j] + col[nx-1];
    tmp3 = oim[k1]*fy1[j] + oim[k2]*fy2[j];
    nim[ind++] = tmp3;
  }
  /* last row */
  for (i=0; i<nx-1; i++) {
    k2 = (oy-1)*ox + col[i];
    tmp3 = oim[k2]*fx1[i] + oim[k2+1]*fx2[i];
    nim[ind++] = tmp3;
  }
  /* last point */
  nim[ind++] = oim[(ox*oy)-1];
   
  /* free memory */   
  free (fx1);
  free (fx2);
  free (fy1);
  free (fy2);
  free (r1);
  free (r2);
  free (col);
}


/*------------------- bilinear_rgbp_to_rgbp ----------------*/
/* this routine handles 32 bit pixel data (8 red, 8 green, 8 blue, 8 pad) */
void bilinear_rgbp_to_rgbp (byte *oim, int ox, int oy, 
			    byte *nim, int nx, int ny)
{
  int         i, j;
  byte        *tmp_in_byte, *tmp_out_byte;

  /* use bilinear_byte_to_byte on each component */
  tmp_in_byte = (byte *) malloc(ox*oy);
  tmp_out_byte = (byte *) malloc(nx*ny);

  /* sort the data and interpolate */
  for (i=0; i<4; i++) {
    for (j=0; j<ox*oy; j++) {
      tmp_in_byte[j] = oim[j*4+i];
    }
    bilinear_byte_to_byte(tmp_in_byte,ox,oy,tmp_out_byte,nx,ny);
    for (j=0; j<nx*ny; j++) {
      nim[j*4+i] = tmp_out_byte[j];
    }
  }

  /* free memory */
  free(tmp_in_byte);
  free(tmp_out_byte);
}
