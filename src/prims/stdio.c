
#include <marlais/object/stdio.h>

#include <marlais/object/prim.h>

/* Primitives */

static Object prim_stdio_fopen  (Object pathname, Object mode);
static Object prim_stdio_fseek  (Object stream, Object offset, Object whence);
static Object prim_stdio_fread  (Object stream, Object buffer, Object offset, Object count, Object eof);
static Object prim_stdio_fwrite (Object stream, Object buffer, Object offset, Object count, Object eof);
static Object prim_stdio_ftell  (Object stream);
static Object prim_stdio_fflush  (Object stream);
static Object prim_stdio_fclose (Object stream);

static struct primitive stdio_prims[] =
{
  {"%stdio-fopen",  prim_2, prim_stdio_fopen},
  {"%stdio-fseek",  prim_3, prim_stdio_fseek},
  {"%stdio-ftell",  prim_1, prim_stdio_ftell},
  {"%stdio-fread",  prim_5, prim_stdio_fread},
  {"%stdio-fwrite", prim_5, prim_stdio_fwrite},
  {"%stdio-fflush", prim_1, prim_stdio_fflush},
  {"%stdio-fclose", prim_1, prim_stdio_fclose},
};

/* Exported functions */

void
marlais_register_stdio (void)
{
  MARLAIS_REGISTER_PRIMS (stdio_prims);
}

/* Primitives */

static Object
prim_stdio_fopen (Object pathname, Object mode)
{
  FILE *fp;

  fp = fopen (BYTESTRVAL (pathname), BYTESTRVAL (mode));

  return marlais_make_stdio_stream (fp, true);
}

static Object
prim_stdio_fseek (Object fpo, Object offset, Object whence)
{
  int res;
  FILE *fp = STDIOFILE (fpo);

  res = fseek (fp, marlais_get_int (offset), marlais_get_int (whence));

  return MARLAIS_UNSPECIFIED;
}

static Object
prim_stdio_ftell (Object fpo)
{
  long res;
  FILE *fp = STDIOFILE (fpo);

  res = ftell (fp);

  return marlais_make_integer(res);
}

static Object
prim_stdio_fread  (Object stream, Object buffer, Object offset, Object count, Object eof)
{
  return eof;
}

static Object
prim_stdio_fwrite (Object stream, Object buffer, Object offset, Object count, Object eof)
{
  return eof;
}

static Object
prim_stdio_fflush (Object fpo)
{
  FILE *fp = STDIOFILE (fpo);

  fflush (fp);

  return MARLAIS_UNSPECIFIED;
}

static Object
prim_stdio_fclose (Object fpo)
{
  FILE *fp = STDIOFILE (fpo);

  fclose (fp);

  return MARLAIS_UNSPECIFIED;
}
