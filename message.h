/* ----------------------------- MNI Header -----------------------------------
@NAME       : message.h
@DESCRIPTION: Header file for acr-nema message code
@METHOD     : 
@GLOBALS    : 
@CREATED    : November 16, 1993 (Peter Neelin)
@MODIFIED   : $Log: message.h,v $
@MODIFIED   : Revision 1.2  2005-03-02 13:14:16  rotor
@MODIFIED   :  * Changes for autobuild process
@MODIFIED   :  * removed public/private syntactic sugar
@MODIFIED   :  * removed minc_def ugliness - replaced with equally ugly FREE and MALLOC
@MODIFIED   :
@MODIFIED   : Revision 1.1  2005/02/26 14:08:32  rotor
@MODIFIED   :  * Initial checkin to CVS for autoconf build
@MODIFIED   :
 * Revision 1.3  93/11/24  11:27:10  neelin
 * Added dump message routine./
 * 
 * Revision 1.2  93/11/22  13:12:55  neelin
 * Changed to use new Acr_Element_Id stuff.
 * 
 * Revision 1.1  93/11/19  12:50:37  neelin
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

/* Group type */
typedef struct Acr_Message {
   int ngroups;
   long total_length;
   long message_length_offset;
   Acr_Element message_length_element;
   Acr_Group list_head;
   Acr_Group list_tail;
} *Acr_Message;

/* Functions */
Acr_Message acr_create_message(void);
void acr_delete_message(Acr_Message message);
void acr_message_add_group(Acr_Message message, Acr_Group group);
Acr_Group acr_get_message_group_list(Acr_Message message);
long acr_get_message_total_length(Acr_Message message);
int acr_get_message_ngroups(Acr_Message message);
Acr_Status acr_input_message(Acr_File *afp, Acr_Message *message);
Acr_Status acr_output_message(Acr_File *afp, Acr_Message message);
void acr_dump_message(FILE *file_pointer, Acr_Message message);
