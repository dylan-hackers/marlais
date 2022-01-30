
#include <marlais/unicode.h>

#include <marlais/alloc.h>
#include <marlais/env.h>
#include <marlais/prim.h>

#include <unicode/utypes.h>

/* Internal function declarations */

/* Allocation wrappers for ICU memory management */
static void *icu_gc_alloc (void *context, size_t size);
static void *icu_gc_realloc (void *context, void *obj, size_t new_size);
static void  icu_gc_free (void *context, void *obj);

/* Primitives */

static Object uchar_block (Object chr);
static Object uchar_name (Object chr);

static struct primitive icu_prims[] =
{
 {"%uchar-block", prim_1, uchar_block},
 {"%uchar-name",  prim_1, uchar_name},
};

/* Exported functions */

void
marlais_initialize_icu (void)
{
  UErrorCode ue;

  /* set ICU memory management functions */
  ue = U_ZERO_ERROR;
  u_setMemoryFunctions(NULL,
                       icu_gc_alloc,
                       icu_gc_realloc,
                       icu_gc_free,
                       &ue);
  if(U_FAILURE(ue)) {
    goto uerror;
  }

  /* initialize ICU */
  ue = U_ZERO_ERROR;
  u_init(&ue);
  if(U_FAILURE(ue)) {
    goto uerror;
  }

  /* done */
  return;

  /* bailout path */
 uerror:
  fprintf(stderr, "Failed to initialize ICU: %s\n", u_errorName(ue));
  abort();
}

void
marlais_register_icu (void)
{
  MARLAIS_REGISTER_PRIMS (icu_prims);

  /* unicode constants */
  marlais_add_export (marlais_make_name ("$unicode-version"),
                      marlais_make_bytestring (U_UNICODE_VERSION),
                      1);
  marlais_add_export (marlais_make_name ("$minimum-unicode-block"),
                      marlais_make_integer (0),
                      1);
  marlais_add_export (marlais_make_name ("$maximum-unicode-block"),
                      marlais_make_integer (u_getIntPropertyMaxValue(UCHAR_BLOCK)),
                      1);
}

/* Internal functions */

static void *icu_gc_alloc (void *context, size_t size)
{
  return marlais_allocate_memory (size);
}

static void *icu_gc_realloc (void *context, void *obj, size_t new_size)
{
  return marlais_reallocate_memory (obj, new_size);
}

static void icu_gc_free (void *context, void *obj)
{
  marlais_free_memory (obj);
}

/* Primitives */

static Object uchar_block (Object chr)
{
  return marlais_make_integer (ublock_getCode (UCHARVAL (chr)));
}

static Object uchar_name (Object chr)
{
  UErrorCode ue;
  int32_t len;
  char buf[128];
  ue = U_ZERO_ERROR;
  len = u_charName(UCHARVAL (chr), U_UNICODE_CHAR_NAME, buf, sizeof(buf), &ue);
  assert(U_SUCCESS (ue));
  if (len > 0) {
    return marlais_make_bytestring(buf);
  } else {
    return marlais_empty_string;
  }
}
