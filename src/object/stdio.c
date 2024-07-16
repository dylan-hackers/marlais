
#include <marlais/object/stdio.h>

/* Exported functions */

Object
marlais_make_stdio_stream (FILE *fp, bool owned)
{
  Object res;

  res = marlais_allocate_object (StdioHandle, sizeof (struct marlais_stdio_handle));

  STDIOOWNEDP (res) = owned;
  STDIOFILE (res) = fp;

  return res;
}
