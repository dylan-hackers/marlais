/* stream.c, see COPYRIGHT for use */

#include <fcntl.h>
#include <stdio.h>
#include "stream.h"

#include "error.h"
#include "prim.h"

/* primitives */

static Object eof_object_p (Object obj);
static Object standard_input (void);
static Object standard_output (void);
static Object standard_error (void);
static Object dylan_write (Object fd, Object str);

static struct primitive stream_prims[] =
{
#ifndef COMMON_DYLAN_SPEC
    {"%open-input-file", prim_1, open_input_file},
    {"%open-output-file", prim_1, open_output_file},
#endif
    {"%close-stream", prim_1, close_stream},
    {"%eof-object?", prim_1, eof_object_p},
    {"%standard-input", prim_0, standard_input},
    {"%standard-output", prim_0, standard_output},
    {"%standard-error", prim_0, standard_error},
  {"%write", prim_2, dylan_write},
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

#ifndef COMMON_DYLAN_SPEC

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
#endif

Object
close_stream (Object stream_fd)
{
  int fd = INTVAL(stream_fd);
  if(fd > 2) { /* ignore closing input/output/error */
    close (fd);
  }
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
dylan_write(Object fd_obj, Object str)
{
  int fd = INTVAL(fd_obj);
  write(fd, BYTESTRVAL(str), BYTESTRSIZE(str));
  return unspecified_object;
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
