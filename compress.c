/*
 * 	compress.c
 *	
 *      xdisp routines to allow automatic decompression of compressed files.
 *
 *      File_Is_Compressed()
 *      File_Compression_Type()
 *      Decompress_File()
 *      
 *	Copyright (c) B. Pike & R. Kwan, 1993-1997
 */

#include "xdisp.h"

/* Define compression types */
#define UNCOMPRESSED 0
#define GZIP 1
#define COMPRESS 2

/*-------------------------- File_Is_Compressed --------------------------*/

int File_Is_Compressed(char *fn)
{
  if (File_Compression_Type(fn) == UNCOMPRESSED)
    return FALSE;
  else
    return TRUE;

}

/*---------------------- File_Compression_Type --------------------------*/

int File_Compression_Type(char *fn)
{
  int len, type;

  /* look for standard extensions */
  len = strlen(fn);
  if (strcmp(&fn[len-3], ".gz") == 0) {
    type = GZIP;
  } else if (strcmp(&fn[len-2], ".Z") == 0) {
    type = COMPRESS;
  } else {
    type = UNCOMPRESSED;
  }

  return type;
}

/*-------------------------- Decompress_File --------------------------*/

char *Decompress_File(char *fn)
{

  char *tmpname;
  char cmd_string[264];
  int  type;

  /* Get the compression type from the filename extension */
  type = File_Compression_Type(fn);
        
  /* Create a temporary file name for the decompressed file */
  tmpname = strdup(fn);
  (void)tmpnam(tmpname);
  switch(type){
  case GZIP:
    sprintf(cmd_string, "cp %s %s.gz\n", fn, tmpname);
    break;
  case COMPRESS:
    sprintf(cmd_string, "cp %s %s.Z\n", fn, tmpname);
    break;
  }

  if (Verbose) fprintf(stderr,"Decompressing to temporary file %s\n",
		       tmpname);
  system(cmd_string); 
    
  switch(type){
  case GZIP:
    sprintf(cmd_string, "gunzip %s.gz\n", tmpname);
    break;
  case COMPRESS:
    sprintf(cmd_string, "uncompress %s.Z\n", tmpname);
    break;
  }
  system(cmd_string); 

  return tmpname;
}
