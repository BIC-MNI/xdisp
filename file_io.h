/* ----------------------------- MNI Header -----------------------------------
@NAME       : file_io.h
@DESCRIPTION: Header file for doing io from acr_nema code.
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 9, 1993 (Peter Neelin)
@MODIFIED   : $Log: file_io.h,v $
@MODIFIED   : Revision 1.2  2005-03-02 13:14:15  rotor
@MODIFIED   :  * Changes for autobuild process
@MODIFIED   :  * removed public/private syntactic sugar
@MODIFIED   :  * removed minc_def ugliness - replaced with equally ugly FREE and MALLOC
@MODIFIED   :
@MODIFIED   : Revision 1.1  2005/02/26 14:08:32  rotor
@MODIFIED   :  * Initial checkin to CVS for autoconf build
@MODIFIED   :
 * Revision 1.2  93/11/25  10:37:43  neelin
 * Added file free and ungetc and trace.
 * 
 * Revision 1.1  93/11/10  10:33:33  neelin
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

/* Define io routine prototype */

typedef int (*Acr_Io_Routine)
     (void *user_data, void *buffer, int nbytes);

/* Structure used for reading and writing in acr_nema routines */

typedef struct {
   void *user_data;
   Acr_Io_Routine io_routine;
   int maxlength;
   unsigned char *start;
   unsigned char *end;
   unsigned char *ptr;
   int length;
   int stream_type;
} Acr_File;

/* Macros for getting and putting a character */

#define acr_getc(afp)   ( ((afp)->ptr < (afp)->end) ? \
                         *((afp)->ptr++) : \
                         acr_file_read_more(afp) )

#define acr_putc(c,afp) ( ((afp)->ptr < (afp)->end) ? \
                         (int) (*((afp)->ptr++) = (unsigned char) (c)) : \
                         acr_file_write_more(afp, c) )

/* Function definitions */

void acr_enable_input_trace(void);
void acr_disable_input_trace(void);
void acr_enable_output_trace(void);
void acr_disable_output_trace(void);
Acr_File *acr_file_initialize(void *user_data,
                                     int maxlength,
                                     Acr_Io_Routine io_routine);
void acr_file_free(Acr_File *afp);
int acr_file_read_more(Acr_File *afp);
int acr_file_write_more(Acr_File *afp, int character);
int acr_file_flush(Acr_File *afp);
int acr_ungetc(int c, Acr_File *afp);
int acr_stdio_read(void *user_data, void *buffer, int nbytes);
int acr_stdio_write(void *user_data, void *buffer, int nbytes);

                       
