/* stream.c, see COPYRIGHT for use */

#include <marlais/stream.h>

#include <marlais/prim.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/* Primitives */

static Object stream_eof_object_p (Object obj);
static Object stream_write (Object fd, Object str);
static Object stream_close (Object fd);

static struct primitive stream_prims[] =
{
  {"%eof-object?", prim_1, stream_eof_object_p},
  {"%write", prim_2, stream_write},
  {"%close-stream", prim_1, stream_close},
};

/* Exported functions */

void
marlais_initialize_stream (void)
{
  marlais_standard_input = marlais_make_integer(0);
  marlais_standard_output = marlais_make_integer(1);
  marlais_standard_error = marlais_make_integer(2);
}

void
marlais_register_stream (void)
{
  int num = sizeof (stream_prims) / sizeof (struct primitive);
  marlais_register_prims (num, stream_prims);
}

/* Static functions */

static Object
stream_eof_object_p (Object obj)
{
  if (obj == MARLAIS_EOF) {
    return MARLAIS_TRUE;
  } else {
    return MARLAIS_FALSE;
  }
}

static Object
stream_write(Object fd_obj, Object str)
{
  int fd = INTVAL(fd_obj);
  write(fd, BYTESTRVAL(str), BYTESTRSIZE(str));
  return MARLAIS_UNSPECIFIED;
}

Object
stream_close (Object fd_obj)
{
  int fd = INTVAL(fd_obj);
  if(fd > 2) { /* ignore closing input/output/error */
    close (fd);
  }
  return MARLAIS_UNSPECIFIED;
}
