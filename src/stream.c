/*

   stream.c

   This software is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this software; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Original copyright notice follows:

   Copyright, 1993, Brent Benson.  All Rights Reserved.
   0.4 & 0.5 Revisions Copyright 1994, Joseph N. Wilson.  All Rights Reserved.
   0.6 Revisions Copyright 2001, Douglas M. Auclair.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include "stream.h"

#include "error.h"
#include "prim.h"

/* primitives */

static Object eof_object_p (Object obj);
static Object standard_input (void);
static Object standard_output (void);
static Object standard_error (void);

static struct primitive stream_prims[] =
{
    {"%open-input-file", prim_1, open_input_file},
    {"%open-output-file", prim_1, open_output_file},
    {"%close-stream", prim_1, close_stream},
    {"%eof-object?", prim_1, eof_object_p},
    {"%standard-input", prim_0, standard_input},
    {"%standard-output", prim_0, standard_output},
    {"%standard-error", prim_0, standard_error},
};

/* globals */
Object standard_input_stream;
Object standard_output_stream;
Object standard_error_stream;

void
init_stream_prims (void)
{
  int num = sizeof (stream_prims) / sizeof (struct primitive);
  init_prims (num, stream_prims);
}

Object
make_stream (enum streamtype type, FILE * fp)
{
  Object obj = allocate_object (sizeof (struct stream));

  STREAMTYPE (obj) = Stream;
  STREAMSTYPE (obj) = type;
  STREAMFP (obj) = fp;
  return (obj);
}

Object
open_input_file (Object filename)
{
  FILE *fp;
  char *str = BYTESTRVAL (filename);
  fp = fopen (str, "r");
  if (!fp) {
    error ("open-input-file: could not open file", filename, NULL);
  }
  return (make_stream (Input, fp));
}

Object
open_output_file (Object filename)
{
  FILE *fp;
  char *str = BYTESTRVAL (filename);
  fp = fopen (str, "w");
  if (!fp) {
    error ("open-output-file: could not open file", filename, NULL);
  }
  return (make_stream (Output, fp));
}

Object
close_stream (Object stream)
{
  fclose (STREAMFP (stream));
  return (unspecified_object);
}

static Object
eof_object_p (Object obj)
{
  if (obj == eof_object) {
    return (true_object);
  } else {
    return (false_object);
  }
}

static Object
standard_input (void)
{
  return (standard_input_stream);
}

static Object
standard_output (void)
{
  return (standard_output_stream);
}

static Object
standard_error (void)
{
  return (standard_error_stream);
}
