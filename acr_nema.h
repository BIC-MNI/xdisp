/* ----------------------------- MNI Header -----------------------------------
@NAME       : acr_nema.h
@DESCRIPTION: Header file for ACR-NEMA code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 10, 1993 (Peter Neelin)
@MODIFIED   : $Log: acr_nema.h,v $
@MODIFIED   : Revision 1.2  2005-03-02 13:14:15  rotor
@MODIFIED   :  * Changes for autobuild process
@MODIFIED   :  * removed public/private syntactic sugar
@MODIFIED   :  * removed minc_def ugliness - replaced with equally ugly FREE and MALLOC
@MODIFIED   :
@MODIFIED   : Revision 1.1  2005/02/26 14:08:32  rotor
@MODIFIED   :  * Initial checkin to CVS for autoconf build
@MODIFIED   :
 * Revision 1.4  94/01/06  13:31:27  neelin
 * Changed acr_need_invert to a function.
 * 
 * Revision 1.3  93/12/08  09:07:02  neelin
 * 
 * Revision 1.2  93/11/24  11:26:09  neelin
 * Added TRUE and FALSE.
 * 
 * Revision 1.1  93/11/19  12:49:34  neelin
 * Initial revision
 * 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

/* Define constants */
#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif


/* defs */
#define MALLOC(size) ((void *) malloc(size))
#define FREE(ptr) free(ptr)

#include <file_io.h>


/* Length of short and long for ACR-NEMA protocol */
#define ACR_SIZEOF_SHORT 2
#define ACR_SIZEOF_LONG  4

/* Status for io */
typedef enum {
   ACR_OK, ACR_END_OF_INPUT, ACR_PROTOCOL_ERROR, ACR_OTHER_ERROR
} Acr_Status;

/* Functions */
int acr_set_vax_byte_ordering(int has_vax_byte_ordering);
int acr_need_invert(void);
void acr_get_short(long nvals, void *input_value, 
                          unsigned short *mach_value);
void acr_get_long(long nvals, void *input_value, long *mach_value);
void acr_put_short(long nvals, unsigned short *mach_value, 
                          void *output_value);
void acr_put_long(long nvals, long *mach_value, void *output_value);
Acr_Status acr_test_byte_ordering(Acr_File *afp);
Acr_Status acr_read_one_element(Acr_File *afp,
                                       int *group_id, int *element_id,
                                       long *data_length, char **data_pointer);
Acr_Status acr_write_one_element(Acr_File *afp,
                                        int group_id, int element_id,
                                        long data_length, char *data_pointer);


/* Include files */
/* #include <acr_nema_io.h> */
#include <element.h>
#include <group.h>
#include <message.h>
