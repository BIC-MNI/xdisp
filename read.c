/*
 * 	read.c
 *	
 *      xdisp routines for reading raw data.
 *
 *      Read_Byte()
 *      Read_UByte()
 *      Read_Short()
 *      Read_UShort()
 *      Read_Long()
 *      Read_ULong()
 *      Read_Float()
 *      Read_Double()
 *
 *	Copyright (c) B. Pike, 1993-1997
 */

#include "xdisp.h"
#include "acr_nema.h"

/*---------------------------- Read_Byte -------------------------------*/
void Read_Byte(void)
{
  int 	i;
  FILE 	*fp;
  char  *char_data;

  /* open the file */
  if (read_from_stdin) {
    fp = stdin;
  }
  else {
    fp = fopen(fullfname,"rb");
    if (!fp) FatalError("Error opening file"); 
  }

  /* position for read */
  fseek(fp, Byte_Offset+(Width*Height*image_number*(sizeof(char)+Byte_Skip)), 0);

  /* get the memory and read the data */
  char_data = (char *) malloc(Width*Height);
  if (!Byte_Skip) {
    if (fread(char_data, Width*Height, 1, fp)!=1)
      FatalError("image data read failed!");
  }
  else {
    for (i=0; i<Width*Height; i++) {
      if (fread(&char_data[i], 1, 1, fp)!=1)
	FatalError("image data read failed!");
      if (fseek(fp,Byte_Skip,SEEK_CUR))
	FatalError("image data read failed!");
    }
  }

  /* move the data to short_Image */
  for (i=0; i<Height*Width; i++) {
    short_Image[i+(load_all_images ? (Width*Height*image_number): 0)] = 
      (short) char_data[i];
  }

  /* clean up and close the file */
  free(char_data);
  fclose(fp);
}


/*---------------------------- Read_UByte -------------------------------*/
void Read_UByte(void)
{
  int 		i;
  FILE 		*fp;
  unsigned char *uchar_data;

  /* open the file */
  if (read_from_stdin) {
    fp = stdin;
  }
  else {
    fp = fopen(fullfname,"rb");
    if (!fp) FatalError("Error opening file"); 
  }

  /* position for read */
  fseek(fp, Byte_Offset+(Width*Height*image_number*(sizeof(char)+Byte_Skip)), 0);

  /* get the memory and read the data */
  uchar_data = (unsigned char *) malloc(Width*Height);
  if (!Byte_Skip) {
    if (fread(uchar_data, Width*Height, 1, fp)!=1) 
      FatalError("image data read failed!");
  }
  else {
    for (i=0; i<Width*Height; i++) {
      if (fread(&uchar_data[i], 1, 1, fp)!=1)
	FatalError("image data read failed!");
      if (fseek(fp,Byte_Skip,SEEK_CUR))
	FatalError("image data read failed!");
    }
  }

  /* move the data to short_Image */
  for (i=0; i<Height*Width; i++) {
    short_Image[i+(load_all_images ? (Width*Height*image_number): 0)] = 
      (short) uchar_data[i];
  }

  /* clean up and close the file */
  free(uchar_data);
  fclose(fp);
}

/*---------------------------- Read_Short -------------------------------*/
void Read_Short(void)
{
  int 	i;
  FILE  *fp;

  /* open the file */
  if (read_from_stdin) {
    fp = stdin;
  }
  else {
    fp = fopen(fullfname,"rb");
    if (!fp) FatalError("Error opening file"); 
  }

  /* position for read */
  fseek(fp, Byte_Offset+(Width*Height*image_number*(sizeof(short)+Byte_Skip)), 0);

  /* read the data directly into short_Image */
  if (!Byte_Skip) {
    if(fread(&short_Image[(load_all_images ? (Width*Height*image_number): 0)], 
	     Height*Width*2, 1, fp)!=1)
      FatalError("image data read failed");
  }
  else {
    for (i=0; i<Width*Height; i++) {
      if (fread(&short_Image[i+(load_all_images ? 
				(Width*Height*image_number): 0)], 2, 1, fp)!=1)
	FatalError("image data read failed!");
      if (fseek(fp,Byte_Skip,SEEK_CUR))
	FatalError("image data read failed!");
    }
  }

  /* swap bytes if needed */        
  if (Swap) {
      Byte_Swap((byte *) &short_Image[(load_all_images?(Width*Height*image_number):0)],  
	       Width*Height*2);  
    }
  
  /* close the file */
  fclose(fp);
}

/*---------------------------- Read_UShort -------------------------------*/
void Read_UShort(void)
{
  int 		      i;
  FILE 		      *fp;
  float               fscale;
  unsigned short int  min, max;
  unsigned short int  *ushort_data;

  /* open the file */
  if (read_from_stdin) {
    fp = stdin;
  }
  else {
    fp = fopen(fullfname,"rb");
    if (!fp) FatalError("Error opening file"); 
  }

  /* position for read */
  fseek(fp, Byte_Offset+(Width*Height*image_number*(sizeof(short)+Byte_Skip)), 0);

  /* get the memory and read the data */
  ushort_data = (unsigned short int *) malloc(Width*Height*2);
  if (!Byte_Skip) {
    if (fread(ushort_data, Width*Height*2, 1, fp)!=1) 
      FatalError("image data read failed!");
  }
  else {
    for (i=0; i<Width*Height; i++) {
      if (fread(&ushort_data[i], 2, 1, fp)!=1)
	FatalError("image data read failed!");
      if (fseek(fp,Byte_Skip,SEEK_CUR))
	FatalError("image data read failed!");
    }
  }

  /* swap bytes if needed */        
  if (Swap) Byte_Swap((byte *) ushort_data,Width*Height*2);

  /* find the min and max */
  min = USHRT_MAX;
  max = 0;
  for (i=0; i<Width*Height; i++) {
    if (ushort_data[i]<min) 
      min = ushort_data[i];
    else
      if (ushort_data[i]>max) max = ushort_data[i];
  }
  fscale = (float)(SHRT_MAX-SHRT_MIN)/(float)(max-min);
  if (Verbose) fprintf(stderr,"Unsigned short image min,max = %d,%d\n",min,max);

  /* scale the data and move to short_Image */
  for (i=0; i<Height*Width; i++) {
    short_Image[i+(load_all_images ? (Width*Height*image_number): 0)] = 
      (short) ((float)SHRT_MIN + 
	       (float)(ushort_data[i]-min)*fscale);
  }

  /* set the real world scale and offset values */
  rw_scale = 1.0 / (double) fscale;
  rw_offset = (double) min;

  /* clean up and close the file */
  free(ushort_data);
  fclose(fp);
}

/*---------------------------- Read_Long -------------------------------*/
void Read_Long(void)
{
  int 		i;
  FILE 		*fp;
  float         fscale;
  long int	min, max;
  long int 	*long_data;

  /* open the file */
  if (read_from_stdin) {
    fp = stdin;
  }
  else {
    fp = fopen(fullfname,"rb");
    if (!fp) FatalError("Error opening file"); 
  }

  /* position for read */
  fseek(fp, Byte_Offset+(Width*Height*image_number*(sizeof(long)+Byte_Skip)), 0);

  /* get the memory and read the data */
  long_data = (long int *) malloc(Width*Height*4);
  if (!Byte_Skip) {
    if (fread(long_data, Width*Height*4, 1, fp)!=1) 
      FatalError("image data read failed!");
  }
  else {
    for (i=0; i<Width*Height; i++) {
      if (fread(&long_data[i], 4, 1, fp)!=1)
	FatalError("image data read failed!");
      if (fseek(fp,Byte_Skip,SEEK_CUR))
	FatalError("image data read failed!");
    }
  }

  /* swap bytes if needed */        
  if (Swap) Byte_Swap((byte *) long_data,Width*Height*4);

  /* find the min and max */
  min = LONG_MAX;
  max = LONG_MIN;
  for (i=0; i<Width*Height; i++) {
    if (long_data[i]<min) 
      min = long_data[i];
    else
      if (long_data[i]>max) max = long_data[i];
  }
  fscale = (float)(SHRT_MAX-SHRT_MIN)/(float)(max-min);
  if (Verbose) fprintf(stderr,"Long int image min,max = %ld,%ld\n",min,max);

  /* scale the data and move to short_Image */
  for (i=0; i<Height*Width; i++) {
    short_Image[i+(load_all_images ? (Width*Height*image_number): 0)] = 
      (short) ((float)SHRT_MIN + (float)(long_data[i]-min)*fscale);
  }

  /* set the real world scale and offset values */
  rw_scale = 1.0 / (double) fscale;
  rw_offset = (double) min;

  /* clean up and close the file */
  free(long_data);
  fclose(fp);
}


/*---------------------------- Read_ULong -------------------------------*/
void Read_ULong(void)
{
  int 		      i;
  FILE 		      *fp;
  float               fscale;
  unsigned long int   min, max;
  unsigned long int   *ulong_data;

  /* open the file */
  if (read_from_stdin) {
    fp = stdin;
  }
  else {
    fp = fopen(fullfname,"rb");
    if (!fp) FatalError("Error opening file"); 
  }

  /* position for read */
  fseek(fp, Byte_Offset+(Width*Height*image_number*(sizeof(long)+Byte_Skip)), 0);

  /* get the memory and read the data into byte_Image*/
  ulong_data = (unsigned long int *) malloc(Width*Height*4);
  if (!Byte_Skip) {
    if (fread(ulong_data, Width*Height*4, 1, fp)!=1) 
      FatalError("image data read failed!");
  }
  else {
    for (i=0; i<Width*Height; i++) {
      if (fread(&ulong_data[i], 4, 1, fp)!=1)
	FatalError("image data read failed!");
      if (fseek(fp,Byte_Skip,SEEK_CUR))
	FatalError("image data read failed!");
    }
  }

  /* swap bytes if needed */        
  if (Swap) Byte_Swap((byte *) ulong_data,Width*Height*4);

  /* find the min and max */
  min = ULONG_MAX;
  max = 0;
  for (i=0; i<Width*Height; i++) {
    if (ulong_data[i]<min) 
      min = ulong_data[i];
    else
      if (ulong_data[i]>max) max = ulong_data[i];
  }
  fscale = (float)(SHRT_MAX-SHRT_MIN)/(float)(max-min);
  if (Verbose) 
    fprintf(stderr,"Unsigned long int image min,max = %lu,%lu\n",min,max);

  /* scale the data and move to short_Image */
  for (i=0; i<Height*Width; i++) {
    short_Image[i+(load_all_images ? (Width*Height*image_number): 0)] = 
      (short) ((float)SHRT_MIN + (float)(ulong_data[i]-min)*fscale);
  }

  /* set the real world scale and offset values */
  rw_scale = 1.0 / (double) fscale;
  rw_offset = (double) min;

  /* clean up and close the file */
  free(ulong_data);
  fclose(fp);
}


/*---------------------------- Read_Float -------------------------------*/
void Read_Float(void)
{
  int 		i;
  FILE 		*fp;
  float         fscale;
  float		min, max;
  float		*float_data;

  /* open the file */
  if (read_from_stdin) {
    fp = stdin;
  }
  else {
    fp = fopen(fullfname,"rb");
    if (!fp) FatalError("Error opening file"); 
  }

  /* position for read */
  fseek(fp, Byte_Offset+(Width*Height*image_number*(sizeof(float)+Byte_Skip)), 0);

  /* get the memory and read the data into byte_Image*/
  float_data = (float *) malloc(Width*Height*4);
  if (!Byte_Skip) {
    if (fread(float_data, Width*Height*4, 1, fp)!=1) 
      FatalError("image data read failed!");
  }
  else {
    for (i=0; i<Width*Height; i++) {
      if (fread(&float_data[i], 4, 1, fp)!=1)
	FatalError("image data read failed!");
      if (fseek(fp,Byte_Skip,SEEK_CUR))
	FatalError("image data read failed!");
    }
  }

  /* swap bytes if needed */        
  if (Swap) Byte_Swap((byte *) float_data,Width*Height*4);

  /* find the min and max */
  min = FLT_MAX;
  max = FLT_MIN;
  for (i=0; i<Width*Height; i++) {
    if (float_data[i]<min) 
      min = float_data[i];
    else
      if (float_data[i]>max) max = float_data[i];
  }
  fscale = (float)(SHRT_MAX-SHRT_MIN)/(max-min);
  if (Verbose) fprintf(stderr,"Float image min,max = %g,%g\n",min,max);

  /* scale the data and move to short_Image */
  for (i=0; i<Height*Width; i++) {
    short_Image[i+(load_all_images ? (Width*Height*image_number): 0)] =
      (short) ((float)SHRT_MIN + (float_data[i]-min)*fscale);
  }

  /* set the real world scale and offset values */
  rw_scale = 1.0 / (double) fscale;
  rw_offset = (double) min;

  /* clean up and close the file */
  free(float_data);
  fclose(fp);
}

/*---------------------------- Read_Double -------------------------------*/
void Read_Double(void)
{
  int 		i;
  FILE 		*fp;
  double        fscale;
  double	min, max;
  double	*double_data;

  /* open the file */
  if (read_from_stdin) {
    fp = stdin;
  }
  else {
    fp = fopen(fullfname,"rb");
    if (!fp) FatalError("Error opening file"); 
  }

  /* position for read */
  fseek(fp, Byte_Offset+(Width*Height*image_number*
			 (sizeof(double)+Byte_Skip)), 0);

  /* get the memory and read the data into byte_Image*/
  double_data = (double *) malloc(Width*Height*8);
  if (!Byte_Skip) {
    if (fread(double_data, Width*Height*8, 1, fp)!=1) 
      FatalError("image data read failed!");
  }
  else {
    for (i=0; i<Width*Height; i++) {
      if (fread(&double_data[i], 8, 1, fp)!=1)
	FatalError("image data read failed!");
      if (fseek(fp,Byte_Skip,SEEK_CUR))
	FatalError("image data read failed!");
    }
  }

  /* swap bytes if needed */        
  if (Swap) Byte_Swap((byte *) double_data,Width*Height*8);

  /* find the min and max */
  min = DBL_MAX;
  max = DBL_MIN;
  for (i=0; i<Width*Height; i++) {
    if (double_data[i]<min) 
      min = double_data[i];
    else
      if (double_data[i]>max) max = double_data[i];
  }
  fscale = (double)(SHRT_MAX-SHRT_MIN)/(max-min);
  if (Verbose) fprintf(stderr,"Double image min,max = %g,%g\n",min,max);

  /* scale the data and move to short_Image */
  for (i=0; i<Height*Width; i++) {
    short_Image[i+(load_all_images ? (Width*Height*image_number): 0)] = 
      (short) ((double)SHRT_MIN + (double_data[i]-min)*fscale);
  }

  /* set the real world scale and offset values */
  rw_scale = 1.0 / fscale;
  rw_offset = min;

  /* clean up and close the file */
  free(double_data);
  fclose(fp);
}

