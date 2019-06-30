/* stream.h -- see COPYRIGHT for use */

#ifndef STREAM_H
#define STREAM_H

#include "common.h"

/* Standard input stream */
extern Object marlais_standard_input;
/* Standard output stream */
extern Object marlais_standard_output;
/* Standard error stream */
extern Object marlais_standard_error;

/* Initialize standard streams */
void marlais_initialize_stream (void);
/* Registream stream primitives */
void marlais_register_stream (void);

#endif
