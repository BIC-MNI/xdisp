/*
 * 	image.c
 *	
 *      Xdisp routines for image loading.
 *	
 *	Load_Image()
 *	Increment_Image()
 *	Decrement_Image()
 *	New_Image()
 *	cmi_init()
 *      Load_All()
 *      Byte_Swap()
 *      Reload()
 *
 *	Copyright (c) B. Pike, 1993-2000
 */

#include "xdisp.h"
#include "acr_nema.h"

/* Define some ACR_NEMA constants */
#define ACR_IMAGE_EID 0x10
#define ACR_IMAGE_GID 0x7fe0
#define PACK_BYTES 3
#define PACK_BITS 12
#define PACK_MASK 0x0F
#define PACK_SHIFT 4

/* Define ACR_NEMA element id's */
DEFINE_ELEMENT(static, ACR_rows          , 0x0028, 0x0010);
DEFINE_ELEMENT(static, ACR_columns       , 0x0028, 0x0011);
DEFINE_ELEMENT(static, ACR_bits_allocated, 0x0028, 0x0100);
DEFINE_ELEMENT(static, ACR_image_location, 0x0028, 0x0200);
DEFINE_ELEMENT(static, ACR_fp_scaled_min,  0x0029, 0x1130);
DEFINE_ELEMENT(static, ACR_fp_scaled_max,  0x0029, 0x1140);
DEFINE_ELEMENT(static, ACR_pixel_min,      0x0028, 0x0104);
DEFINE_ELEMENT(static, ACR_pixel_max,      0x0028, 0x0105);

/*---------------------------- Load_Image ---------------------------*/
void Load_Image(char *fn)
{
  char           fname[128];
  long           filesize;
  int            i, count, 
                 bits_per_pixel, need_byte_swap, 
                 acr_pixel_min, acr_pixel_max,
  	         initial_slice[MAX_VAR_DIMS];
  long int	 magic[5];
  float	         sum, sq_sum;
  register byte  *bptr;
  register short *sptr;
  FILE 	         *fp;
  byte	         temp_byte, *packed, pixel[2][2];
  void	         *data=NULL;
  Acr_File 	 *afp;
  Acr_Group      group_list;
  Acr_Element    element;
  Acr_Element_Id element_id;
  memset(initial_slice, 0, MAX_VAR_DIMS*sizeof(int));

  /* setup filename and open file */
  strcpy(fname,fn);
  strcpy(fullfname,fn);
  if (Verbose) fprintf(stderr,"Filename: %s \n",fname);
  if (read_from_stdin)
    fp = stdin;
  else {
    fp = fopen(fname,"rb");
    if (!fp) FatalError("file not found"); 
  }

  /* find the size of the file and position for read*/
  if (!read_from_stdin) {
    fseek(fp, 0L, 2); 
    filesize = ftell(fp);
    if (Verbose) fprintf(stderr,"File size: %ld \n",filesize);

    /* if we know it's a CMI file */
    if (file_format==CMI_FORMAT) {
      cmi_init(fp);
      Byte_Offset = cmi_offset;
      Width = cmi_x;
      Height = cmi_y;
      Image_Depth = cmi_bits <= 8 ? 1 : 2;
      ndimensions = 3;
      num_images = 1;
      slider_length[0] = 1;
    }

    /* otherwise check for know image formats */
    /* check for Signa 5X format (code fragment thanks to TJB)*/
    if (file_format==UNKNOWN_FORMAT) {
      fseek(fp, 0L, 0); 
      fread(magic, sizeof(long int), 5, fp);
      if (magic[0] == SIGNA_5X_MAGIC) {
	file_format = SIGNA_5X_FORMAT;
	Byte_Offset = magic[1];
	Width =  magic[2];
	Height = magic[3];
	Image_Depth = 2;
	if (Verbose) fprintf(stderr,"Signa 5.x image format found\n");
	if (Verbose) fprintf(stderr,"Image size: %d x %d\n", Width, Height);
	num_images = (filesize-Byte_Offset)/(Height*Width*Image_Depth);
	slider_length[0] = num_images;
	ndimensions = 3;
      }
    }

    /* check for MINC file format */
    if (file_format==UNKNOWN_FORMAT) {
      fseek(fp, 0L, 0); 
      fread(id_field, 1, 4, fp);
      fseek(fp, 0L, 0); 

      /* MINC file found */
      if (strncmp(id_field,"CDF",3)==0 || strncmp(id_field+1,"HDF",3)==0) {
	file_format = MINC_FORMAT;

	/* get some minc volume information */
	minc_icvid = get_volume_info(fn, &minc_volume_info);

	/* set some local image parameters */
	ndimensions = minc_volume_info.number_of_dimensions;
	Width = minc_volume_info.length[minc_volume_info.number_of_dimensions-1];
	Height = minc_volume_info.length[minc_volume_info.number_of_dimensions-2];
	num_images = 1;
	for(i=0; i<ndimensions-2; i++){
	  slider_length[i] = minc_volume_info.length[i];
	  num_images *= minc_volume_info.length[i]; 
	} 
	image_number = 0;
	Image_Depth = 2;
	rw_fp_min = minc_volume_info.minimum;
	rw_fp_max = minc_volume_info.maximum;

        /* check to see if A and B (screen X and Y) DIM values are valid */
	/* if they are,  use them */
	if (!(minc_volume_info.a_start==0 && minc_volume_info.a_step==0)) {
	  minc_volume_info.start[ndimensions-1] = minc_volume_info.a_start;
	  minc_volume_info.step[ndimensions-1] = minc_volume_info.a_step;
	}
	if (!(minc_volume_info.b_start==0 && minc_volume_info.b_step==0)) {
	  minc_volume_info.start[ndimensions-2] = minc_volume_info.b_start;
	  minc_volume_info.step[ndimensions-2] = minc_volume_info.b_step;
	}

        /* identify the x,y,z and t dimensions if possible */
        for  (i=0; i<ndimensions; i++) {
	  if (strstr(minc_volume_info.dimension_names[i],"xspace")!=NULL) 
	    minc_x_dim = i;
	  if (strstr(minc_volume_info.dimension_names[i],"yspace")!=NULL) 
	    minc_y_dim = i;
	  if (strstr(minc_volume_info.dimension_names[i],"zspace")!=NULL) 
	    minc_z_dim = i;
	  if ((strstr(minc_volume_info.dimension_names[i],"tspace")!=NULL) ||
              (strstr(minc_volume_info.dimension_names[i],"time")!=NULL))
	    minc_t_dim = i;
	}

        /* make sure the direction cosines are valid */
	/* if not, set to default */
	if (minc_volume_info.direction_cosines[minc_x_dim][0]==0 &&
	    minc_volume_info.direction_cosines[minc_x_dim][1]==0 &&
	    minc_volume_info.direction_cosines[minc_x_dim][2]==0) {
	  minc_volume_info.direction_cosines[minc_x_dim][0]=1;
	  minc_volume_info.direction_cosines[minc_x_dim][1]=0;
	  minc_volume_info.direction_cosines[minc_x_dim][2]=0;
	}
	if (minc_volume_info.direction_cosines[minc_y_dim][0]==0 &&
	    minc_volume_info.direction_cosines[minc_y_dim][1]==0 &&
	    minc_volume_info.direction_cosines[minc_y_dim][2]==0) {
	  minc_volume_info.direction_cosines[minc_y_dim][0]=0;
	  minc_volume_info.direction_cosines[minc_y_dim][1]=1;
	  minc_volume_info.direction_cosines[minc_y_dim][2]=0;
	}
	if (minc_volume_info.direction_cosines[minc_z_dim][0]==0 &&
	    minc_volume_info.direction_cosines[minc_z_dim][1]==0 &&
	    minc_volume_info.direction_cosines[minc_z_dim][2]==0) {
	  minc_volume_info.direction_cosines[minc_z_dim][0]=0;
	  minc_volume_info.direction_cosines[minc_z_dim][1]=0;
	  minc_volume_info.direction_cosines[minc_z_dim][2]=1;
	}

	if (Verbose) {
	  /* Print out volume information */
	  fprintf(stderr,"File %s:\n", fn);
	  fprintf(stderr,"  type: MINC\n");
	  fprintf(stderr,"  image max = %10g, min = %10g\n",
		  minc_volume_info.maximum, minc_volume_info.minimum);
          for (i=0; i<ndimensions; i++) {
	    fprintf(stderr,"   %10s: length = %3d, start = %8g, step = %8g, units=%s\n",
		    minc_volume_info.dimension_names[i], 
		    (int) minc_volume_info.length[i],
		    minc_volume_info.start[i],
		    minc_volume_info.step[i], 
		    minc_volume_info.dimension_units[i]);
	  }
          fprintf(stderr,"  MINC x,y,z dim = %d, %d, %d\n",
		  minc_x_dim, minc_y_dim, minc_z_dim);
	  fprintf(stderr,"   X direction cosines = %g, %g, %g\n",
		  minc_volume_info.direction_cosines[minc_x_dim][0], 
		  minc_volume_info.direction_cosines[minc_x_dim][1], 
		  minc_volume_info.direction_cosines[minc_x_dim][2]);
	  fprintf(stderr,"   Y direction cosines = %g, %g, %g\n",
		  minc_volume_info.direction_cosines[minc_y_dim][0], 
		  minc_volume_info.direction_cosines[minc_y_dim][1], 
		  minc_volume_info.direction_cosines[minc_y_dim][2]);
	  fprintf(stderr,"   Z direction cosines = %g, %g, %g\n",
		  minc_volume_info.direction_cosines[minc_z_dim][0], 
		  minc_volume_info.direction_cosines[minc_z_dim][1], 
		  minc_volume_info.direction_cosines[minc_z_dim][2]);
          fprintf(stderr,"   screen X is MINC %s\n",
		  minc_volume_info.dimension_names[ndimensions-1]);
          fprintf(stderr,"   screen X start = %g, step = %g\n", 
		  minc_volume_info.a_start, minc_volume_info.a_step);
          fprintf(stderr,"   screen Y is MINC %s\n",
		  minc_volume_info.dimension_names[ndimensions-2]);
          fprintf(stderr,"   screen Y start = %g, step = %g\n", 
		  minc_volume_info.b_start, minc_volume_info.b_step);
	}
      }
    }
	
    /* check for ACR-NEMA format */
    if (file_format==UNKNOWN_FORMAT) {
      fseek(fp, 32L, 0); 
      fread(id_field, 1, 12, fp);
      fseek(fp, 0L, 0);
      if (strncmp(id_field,"ACR-NEMA",8)==0) {
	if (Verbose) fprintf(stderr,"ACR-NEMA image format found...\n");
	file_format = ACR_NEMA_FORMAT;
	afp = acr_file_initialize(fp, 0, acr_stdio_read);
	(void) acr_test_byte_ordering(afp);
	(void) acr_input_group_list(afp, &group_list, 0);
	acr_file_free(afp);
	Height = acr_find_short(group_list, ACR_rows, 0);
	Width  = acr_find_short(group_list, ACR_columns, 0);
	bits_per_pixel = acr_find_short(group_list, ACR_bits_allocated,0);
	num_images = 1;
	ndimensions = 3;
	slider_length[0] = 1;
	if ((Width <= 0) || (Height <= 0)) 
	  FatalError("Invalid ACR-NEMA image size found");
	if ((bits_per_pixel != 16) && (bits_per_pixel != 12))
	  FatalError("Invalid ACR-NEMA image size found.");
	else
	  Image_Depth=2;
	if (Verbose) fprintf(stderr,"ACR-NEMA Image size: %d x %d\n", 
			     Width, Height);
	if (Verbose) fprintf(stderr,"ACR-NEMA Image Depth: %d bits/pixel\n", 
			     bits_per_pixel);
	acr_pixel_min = (int)acr_find_short(group_list, ACR_pixel_min, 0);
	acr_pixel_max = (int)acr_find_short(group_list, ACR_pixel_max, 4095);
	if (Verbose) fprintf(stderr,"ACR-NEMA pixel_min: %d\n", acr_pixel_min);
	if (Verbose) fprintf(stderr,"ACR-NEMA pixel_max: %d\n", acr_pixel_max);
	rw_fp_min = (float)acr_find_double(group_list, ACR_fp_scaled_min, 
					   (float)acr_pixel_min);
	rw_fp_max = (float)acr_find_double(group_list, ACR_fp_scaled_max, 
					   (float)acr_pixel_max);
	if (Verbose) fprintf(stderr,"ACR-NEMA Image fp_scaled_min: %f\n", 
			     rw_fp_min);
	if (Verbose) fprintf(stderr,"ACR-NEMA Image fp_scaled_max: %f\n", 
			     rw_fp_max);
      }
    }
	
    /* check for Signa 4X format */
    if (file_format==UNKNOWN_FORMAT) {        
      if (filesize==SIGNA_4X_SIZE) {
	file_format = SIGNA_4X_FORMAT;
	Byte_Offset = SIGNA_4X_HEADER_SIZE;
	num_images = 1;
	ndimensions = 3;
	slider_length[0] = num_images;
	if (Verbose) fprintf(stderr,"Signa 4.x image format found\n");
      }
    }
	
    /* determine image size */
    if ((Width!=0) && (Height==0)) {
      Height = (filesize-Byte_Offset)/(Width*(Image_Depth+Byte_Skip));
      if (Verbose) 
	fprintf(stderr,"Image height not specified: assuming %d x %d\n",
		Width,Height);
    }
    else if ((Height!=0) && (Width==0)) {
      Width = (filesize-Byte_Offset)/(Height*(Image_Depth+Byte_Skip));
      if (Verbose) 
	fprintf(stderr,"Image width not specified: assuming %d x %d\n",
		Width,Height);
    }
    else if ((Width==0) && (Height==0)) {
      Width=Height=sqrt((filesize-Byte_Offset)/(Image_Depth+Byte_Skip));
      if (Verbose) 
	fprintf(stderr, 
	 "Image size not specified: assuming %d x %d (depth = %d byte(s))\n",
		Width,Height,Image_Depth);
    }
  }

  /* calculate number of images if not already known */
  if (file_format==UNKNOWN_FORMAT) {
    num_images = read_from_stdin ? 1 :
      (filesize-Byte_Offset)/(Height*Width*(Image_Depth+Byte_Skip));   
    ndimensions = 3;
    slider_length[0] = num_images;
  }

  /* position for read */    
  fseek(fp, Byte_Offset, 0); 

  /* get memory */
  if (!(sptr = short_Image = (short *) 
        malloc(Height*Width*2*(load_all_images ? num_images : 1))))
    FatalError("not enough memory to read image");
  if (!(bptr = byte_Image = (byte *) 
        malloc((bitmap_pad/8)*Height*Width*(load_all_images ? num_images : 1))))
    FatalError("not enough memory to read image");

  /* read image */
  /* ACR-NEMA format */
  if (file_format==ACR_NEMA_FORMAT) {
    element_id = malloc(sizeof(*element_id));
    element_id->element_id = ACR_IMAGE_EID;
    element_id->group_id = acr_find_short(group_list, ACR_image_location,
					  ACR_IMAGE_GID);
    element = acr_find_group_element(group_list, element_id);
    if (element != NULL) {
      if (bits_per_pixel==16) {	/* unpacked data */
	acr_get_short(Width*Height, (void *) acr_get_element_data(element),
		      (unsigned short *) short_Image);
      }
      else {			/* packed data */
	if (Verbose) fprintf(stderr,"Unpacking ACR-NEMA image data...\n");
	data = acr_get_element_data(element);
	need_byte_swap = acr_need_invert();
	packed = (unsigned char *) data;
	for (i=0; i < Width*Height; i+=2) {
	  pixel[0][0] = packed[0];
	  pixel[0][1] = packed[1] & PACK_MASK;
	  pixel[1][0] = (packed[1] >> PACK_SHIFT) |
	    ((packed[2] & PACK_MASK) << PACK_SHIFT);
	  pixel[1][1] = packed[2] >> PACK_SHIFT;
	  if (need_byte_swap) {
	    temp_byte = pixel[0][0];
	    pixel[0][0] = pixel[0][1];
	    pixel[0][1] = temp_byte;
	    temp_byte = pixel[1][0];
	    pixel[1][0] = pixel[1][1];
	    pixel[1][1] = temp_byte;
	  }
	  short_Image[i]   = *((unsigned short *) pixel[0]);
	  short_Image[i+1] = *((unsigned short *) pixel[1]);
	  packed += PACK_BYTES;
	}
      }
    }
    free(element_id);
    fclose(fp);
  }

  /* MINC format */
  else if (file_format==MINC_FORMAT) {
    if (load_all_images) {
      fprintf(stderr,"Loading %d %dx%d minc images... \r",
	      num_images,Width,Height);
      read_volume_data(minc_icvid,&minc_volume_info,&short_Image[0]); 
      fprintf(stderr,"                                                 \r");
    }
    else {
      get_volume_slice(minc_icvid, &minc_volume_info, initial_slice, short_Image);
    }
    fclose(fp);
  }

  /* other formats */
  else { 
    if ((num_images>1)&&(load_all_images)) 
      fprintf(stderr,"Loading %d %dx%d images... \r",num_images,Width,Height);
    for (image_number=0; 
	 image_number<(load_all_images ? num_images : 1); 
	 image_number++) {
      switch(Input_Data_Type) {
      case BYTE_DATA: 
	Read_Byte();
	break;
      case UBYTE_DATA: 
	Read_UByte();
	break;
      case SHORT_DATA: 
	Read_Short();
	break;
      case USHORT_DATA: 
	Read_UShort();
	break;
      case LONG_DATA: 
	Read_Long();
	break;
      case ULONG_DATA: 
	Read_ULong();
	break;
      case FLOAT_DATA: 
	Read_Float();
	break;
      case DOUBLE_DATA: 
	Read_Double();
	break;
      }
    }
    if ((num_images>1)&&(load_all_images)) 
      fprintf(stderr,"                                                 \r");
  }

  /* if image is in cmi format rescale to original range */
  if (file_format==CMI_FORMAT) {
    sptr = short_Image;
    for (i=0; i<Height*Width*num_images; i++,sptr++) {
      *sptr = (short) (*sptr<<cmi_shift) - cmi_sub;
    }
  }
    
  /* find Image Min, Max, mean, and std */
  sq_sum = sum = 0.0;
  sptr = short_Image;
  count = Height*Width*(load_all_images?num_images:1);
  if (file_format!=MINC_FORMAT) {
    for (i=0; i<Height*Width*(load_all_images?num_images:1); i++,sptr++) {
      if (*sptr > I_Max) I_Max=*sptr;
      if (*sptr < I_Min) I_Min=*sptr;
      sum += (float)*sptr;
      sq_sum += (float)(*sptr)*(*sptr);
    }
    I_Mean = sum/count;
    I_Std = sqrt((sq_sum - 2*I_Mean*sum + I_Mean*I_Mean*count)/(count-1));
  }
  else {
    I_Min = (short)minc_volume_info.valid_minimum;
    I_Max = (short)minc_volume_info.valid_maximum;        
  }
  
  if (Verbose) fprintf(stderr,"Image Min=%d, Max=%d, Mean=%f, Std=%f\n",
		       I_Min, I_Max, I_Mean, I_Std);
  Delta = I_Max-I_Min;
  if (Delta==0) FatalError("Flat Image!");
    

  /* setup scale parameters */
  P_Min=S_Min=I_Min; P_Max=S_Max=I_Max;
  if (Lower==SHRT_MAX) Lower = P_Min;
  if (Upper==SHRT_MIN) Upper = P_Max;
  oLower = Lower; oUpper = Upper;
  oWidth = Width; oHeight = Height;
  inc = 1; acc_flag = 0;

  /* set up ACR-NEMA/MINC floating point scale values */
  if (file_format==MINC_FORMAT) {
    rw_scale = (rw_fp_max - rw_fp_min)/
      (double)(I_Max-I_Min);
    rw_offset = rw_fp_min;
  }
  else if (file_format==ACR_NEMA_FORMAT) {
    rw_scale = (rw_fp_max - rw_fp_min)/
      (double)(acr_pixel_max-acr_pixel_min);
    rw_offset = rw_fp_min;
  }

  /* set initial Lower and Upper values */
  if (file_format==MINC_FORMAT || 
      file_format==ACR_NEMA_FORMAT ||
      Input_Data_Type > SHORT_DATA ) {
    if ((int)rw_Lower==Lower) Lower = (int)
				((double)I_Min + ((rw_Lower-rw_offset)/rw_scale));
    if ((int)rw_Upper==Upper) Upper = (int)
				((double)I_Min + ((rw_Upper-rw_offset)/rw_scale));
  }
    
  /* set image counter to 0 */
  image_number = 0;
  onum_images = num_images;
  if (Verbose) fprintf(stderr,"%d image(s) found in this file.\n",num_images);

}

/*---------------------------- Increment_Image ---------------------------*/
void Increment_Image(void *data)
{
  image_increment = 1;
  current_dim = *(int *)data;
  New_Image();
}

/*---------------------------- Decrement_Image ---------------------------*/
void Decrement_Image(void *data)
{
  image_increment = -1;
  current_dim = *(int *)data;
  New_Image();
}

/*---------------------------- New_Image ---------------------------*/
void New_Image(void)
{
  int            i, count, w, h;
  float	         sum, sq_sum;
  FILE	         *fp;
  int            offset;

  /* be a little verbose if requested */
  if (Verbose) {
    if (image_increment<0)
      fprintf(stderr,"Display previous image...\n");
    else 
      fprintf(stderr,"Display next image...\n");
  }

  /* check for position in the file */
  if ((image_increment+slider_image[current_dim]<0) ||
      (image_increment+slider_image[current_dim] > 
       slider_length[current_dim]-1)) {
    fprintf(stderr, "Image number requested is out of range.\n");
    return;
  }
    
  /* set image number */
  slider_image[current_dim] += image_increment;
/*   if (current_dim == ndimensions-3){  */
/*     image_number = slider_image[current_dim]; */
/*   }  */
  image_number = slider_image[current_dim];

  /* display image number */
  if (Verbose) fprintf(stderr,"Displaying image %d...\n",image_number);

  /* Reset X image info if images are all loaded */
  if (load_all_images) {
    if (file_format == MINC_FORMAT){
      offset = slider_image[0];
      for(i=1; i<ndimensions-2; i++){
	offset = offset*slider_length[i] + slider_image[i];
      }
      theImage->data = (char *) &byte_Image[zWidth*zHeight*offset*(bitmap_pad/8)];
    } else {
      theImage->data = (char *) &byte_Image[zWidth*zHeight*image_number*(bitmap_pad/8)];
    }
  }

  /* load new image from file (only MINC and flat format are possible) */
  else {
    /* define new cursor */
    XDefineCursor(theDisp,mainW,waitCursor);
    XDefineCursor(theDisp,cmdW,waitCursor);
    XFlush(theDisp);

    /* reset the X & Y reverse flags */
    X_reverse = False;
    Y_reverse = False;

    /* do it */
    short_Image = realloc(short_Image,oWidth*oHeight*2);
    byte_Image = realloc(byte_Image,oWidth*oHeight*(bitmap_pad/8));
    Width=oWidth; Height=oHeight;
    if (file_format==MINC_FORMAT) {
      fp = fopen(fullfname,"rb");
      if (!fp) FatalError("Error opening file"); 
      get_volume_slice(minc_icvid, &minc_volume_info, 
		       slider_image, short_Image);
      fclose(fp);
    }
    if (file_format==RAW_FORMAT) {
      switch(Input_Data_Type) {
      case BYTE_DATA: 
	Read_Byte();
	break;
      case UBYTE_DATA: 
	Read_UByte();
	break;
      case SHORT_DATA: 
	Read_Short();
	break;
      case USHORT_DATA: 
	Read_UShort();
	break;
      case LONG_DATA: 
	Read_Long();
	break;
      case ULONG_DATA: 
	Read_ULong();
	break;
      case FLOAT_DATA: 
	Read_Float();
	break;
      case DOUBLE_DATA: 
	Read_Double();
	break;
      }
    }

    /* find Image Min, Max, mean, and std */
    sq_sum = sum = 0.0;
    count = Height*Width*(load_all_images?num_images:1);
    if (file_format!=MINC_FORMAT) {
      for (i=0; i<oHeight*oWidth*(load_all_images?num_images:1); i++) {
	if (short_Image[i] > I_Max) I_Max=short_Image[i];
	if (short_Image[i] < I_Min) I_Min=short_Image[i];
	sum += (float)short_Image[i];
	sq_sum += (float)(short_Image[i])*(short_Image[i]);
      }
      I_Mean = sum/count;
      I_Std = sqrt((sq_sum - 2*I_Mean*sum + I_Mean*I_Mean*count)/(count-1));
    }
    else {
      I_Min = (short)minc_volume_info.valid_minimum;
      I_Max = (short)minc_volume_info.valid_maximum;
    }
    if (Verbose) fprintf(stderr,"Image Min=%d, Max=%d, Mean=%f, Std=%f\n",
			 I_Min, I_Max, I_Mean, I_Std);
    Delta = I_Max-I_Min;
    if (Delta==0) FatalError("Flat Image!");
    
    /* setup scale parameters */
    P_Min=S_Min=I_Min; P_Max=S_Max=I_Max;
    if (Lower==SHRT_MAX) Lower = P_Min;
    if (Upper==SHRT_MIN) Upper = P_Max;
    oLower = Lower; oUpper = Upper;
    inc = 1; acc_flag = 0;
    Scale_Image(P_Min,P_Max); 
    if (cropped) {
      zWidth = oWidth;
      zHeight = oHeight;
      Resize(oWidth,oHeight); 
      cropped = False;
      crop_x = crop_y = 0;
    }
    else {
      w = zWidth; 
      h = zHeight;
      zWidth = oWidth;
      zHeight = oHeight;
      Resize(w,h);
    }

    /* define new cursor */
    XDefineCursor(theDisp,mainW,mainCursor);
    XDefineCursor(theDisp,cmdW,cmdCursor);
    XFlush(theDisp);
  }

  /* set up for display */
  XResizeWindow(theDisp,mainW,
		zWidth+(color_bar?color_bar_width:0),
		zHeight+info_height);
  DrawWindow(0,0,zWidth,zHeight);
  Window_Level(Lower, Upper);
  update_sliders();
  update_msgs();
  
  /* all done */
  XFlush(theDisp);
}


/*----------------------------- cmi_init -----------------------------*/
void cmi_init(FILE *fp)
{
  /* position to start of file */
  fseek(fp, 0L, 0);

  /* read header data */
  fread(cmi_patient,  1, 32, fp);
  fread(&cmi_type,    4,  1, fp);
  fread(&cmi_fov,     4,  1, fp);
  fread(&cmi_slth,    4,  1, fp);
  fread(&cmi_orient,  4,  1, fp);
  fread(&cmi_x,       4,  1, fp);
  fread(&cmi_y,       4,  1, fp);
  fread(&cmi_bits,    4,  1, fp);
  fread(cmi_comment,  1, 80, fp);
  fread(&cmi_stereo,  4,  1, fp);
  fread(&cmi_shift,   4,  1, fp);
  fread(&cmi_sub,     4,  1, fp);
  cmi_offset = 512;
  cmi_patient[28] = '\0'; /* zap additional header crud */
  cmi_comment[64] = '\0'; /* zap additional header crud */

  /* display extra data if in verbose mode */
  if (Verbose) {
    fprintf(stderr,"patient name: %s\n",cmi_patient);
    if (cmi_type==1)
      fprintf(stderr,"data type: CT\n");
    else if (cmi_type==2)    	    
      fprintf(stderr,"data type: MR\n");
    else
      fprintf(stderr,"data type: DR\n"); 
    fprintf(stderr,"field of view: %d mm\n",cmi_fov);
    fprintf(stderr,"slice thickness: %d mm\n",cmi_slth);
    fprintf(stderr,"orientation: %d\n",cmi_orient);
    fprintf(stderr,"x size: %d\n", cmi_x);
    fprintf(stderr,"y size: %d\n", cmi_y);
    fprintf(stderr,"bits per pixel: %d\n", cmi_bits);
    fprintf(stderr,"comment field: %s\n", cmi_comment);
    fprintf(stderr,"stereo image: %d\n", cmi_stereo);
    fprintf(stderr,"data bit shift: %d\n", cmi_shift);
    fprintf(stderr,"data offset: %d\n", cmi_sub);
  }
}


/*-------------------------- Load_All --------------------------------*/
void Load_All(void *data)
{
  /* define new cursor */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);
  if (Verbose) fprintf(stderr,"Loading all images from file...\n");

  /* reset image dimensions */
  cropped = False;
  Width = oWidth;
  Height = oHeight;
  num_images = onum_images;
  load_all_images = True;
  zoom_factor_x = zoom_factor_y = 1;
  X_reverse = Y_reverse = False;
  RW_valid = True;

  /* reset min/max values */
  P_Max = I_Max = SHRT_MIN;
  P_Min = I_Min = SHRT_MAX;

  /* reload the image from file */
  Load_Image(fname);

  /* Reset X image info */
  theImage->width=Width;
  theImage->height=Height;
  theImage->bytes_per_line=Width;
  theImage->data = (char *) byte_Image;

  /* Define corrected size if a command line zoom was used */
  zWidth = file_format!=CMI_FORMAT ? Width*zoom_factor_x : 
                                     Width*zoom_factor_x*10/8;
  zHeight = Height*zoom_factor_y;

  /* scale and resize image data */
  Scale_Image(Lower, Upper);
  Window_Level(Lower, Upper);
  Resize(zWidth,zHeight);
  XResizeWindow(theDisp,mainW,
		zWidth+(color_bar?color_bar_width:0),
		zHeight+info_height);
  image_increment=0;
  New_Image();

  /* update messages and sliders */
  update_msgs();
  update_sliders();

  /* define new cursor */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}


/*---------------------------- Byte_Swap -------------------------------*/
void Byte_Swap(byte *byte_data, int length)
{
  int 	i;
  byte	tmp_byte;

  /* do it */
  for (i=0; i<length; i+=2) {
    tmp_byte = byte_data[i];
    byte_data[i] = byte_data[i+1];
    byte_data[i+1] = tmp_byte;
  }
}

/*-------------------------- Reload --------------------------------*/
void Reload(void *data)
{
  int i;
    
  /* define new cursor */
  XDefineCursor(theDisp,mainW,waitCursor);
  XDefineCursor(theDisp,cmdW,waitCursor);
  XFlush(theDisp);
  if (Verbose) fprintf(stderr,"Reloading image from file...\n");

  /* reset image dimensions */
  Width = oWidth;
  Height = oHeight;
  num_images = onum_images;
  cropped = False;
  zoom_factor_x = zoom_factor_y = 1;
  X_reverse = Y_reverse = False;
  RW_valid = True;
  Resize_ColorBar(color_bar_width,Height);

  /* reset min/max values */
  P_Max = I_Max = SHRT_MIN;
  P_Min = I_Min = SHRT_MAX;

  /* reload the image from file */
  Load_Image(fname);

  /* Reset X image info */
  theImage->width=Width;
  theImage->height=Height;
  theImage->bytes_per_line=Width;
  theImage->data = (char *) byte_Image;

  /* Define corrected size if a command line zoom was used */
  zWidth = file_format!=CMI_FORMAT ? Width*zoom_factor_x : 
                                     Width*zoom_factor_x*10/8;
  zHeight = Height*zoom_factor_y;

  /* scale and resize image data */
  Scale_Image(Lower, Upper);
  Window_Level(Lower, Upper);
  Resize(zWidth,zHeight);
  XResizeWindow(theDisp,mainW,
		zWidth+(color_bar?color_bar_width:0),
		zHeight+info_height);
  DrawWindow(0,0,zWidth,zHeight);

  /* reset the slider values */
  for (i=0; i<ndimensions-2; i++) {
    slider_image[i] = 0;
  }
  update_sliders();
  update_msgs();

  /* define new cursor */
  XDefineCursor(theDisp,mainW,mainCursor);
  XDefineCursor(theDisp,cmdW,cmdCursor);
  XFlush(theDisp);
}

