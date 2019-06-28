/* stream.c, see COPYRIGHT for use */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "stream.h"

/* for throwing errors */
#include "error.h"
/* for declaring primitives */
#include "prim.h"

/* primitives */

static Object eof_object_p (Object obj);
static Object dylan_write (Object fd, Object str);

static struct primitive stream_prims[] =
{
  {"%close-stream", prim_1, close_stream},
  {"%eof-object?", prim_1, eof_object_p},
  {"%write", prim_2, dylan_write},
};

/* globals */
Object standard_output_stream;
Object standard_error_stream;

void
init_stream_prims (void)
{
  int num = sizeof (stream_prims) / sizeof (struct primitive);
  init_prims (num, stream_prims);
}

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
