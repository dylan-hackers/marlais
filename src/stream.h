/* stream.h -- see COPYRIGHT for use */

#ifndef STREAM_H
#define STREAM_H

#include "object.h"

void init_stream_prims (void);
Object make_stream (enum streamtype type, FILE * fp);

#ifndef COMMON_DYLAN_SPEC
Object open_input_file (Object filename);
Object open_output_file (Object filename);
#endif

Object close_stream (Object stream);

/* globals */
extern Object standard_input_stream;
extern Object standard_output_stream;
extern Object standard_error_stream;

#endif
