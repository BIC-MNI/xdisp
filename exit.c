/*
 * 	exit.c
 *
 * 	exit routines for xdisp.
 *
 *      exit_xdisp()
 *
 * 	Copyright (c) Bruce Pike, 1993-1997
 *
 */

#include "xdisp.h"

void exit_xdisp(int status){

  /* Clean up before exiting */
  if (compressed) unlink(fname);

  /* Done! */
  exit(status);
}
