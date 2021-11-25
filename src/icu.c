
#include <marlais/icu.h>

#include <marlais/alloc.h>
#include <marlais/env.h>
#include <marlais/number.h>
#include <marlais/prim.h>
#include <marlais/string.h>
#include <marlais/symbol.h>

#include <unicode/utypes.h>

/* Internal function declarations */

/* Allocation wrappers for ICU memory management */
static void *icu_gc_alloc (void *context, size_t size);
static void *icu_gc_realloc (void *context, void *obj, size_t new_size);
static void  icu_gc_free (void *context, void *obj);

/* Primitives */

static Object prim_char_block (Object chr);
static Object prim_char_name (Object chr);

static struct primitive icu_prims[] =
{
 {"%icu-char-block", prim_1, prim_char_block},
 {"%icu-char-name",  prim_1, prim_char_name},
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
  int num = sizeof (icu_prims) / sizeof (struct primitive);
  marlais_register_prims (num, icu_prims);

  marlais_add_export (marlais_make_name ("$minimum-unicode-codepoint"),
                      marlais_make_integer (0),
                      1);
  marlais_add_export (marlais_make_name ("$maximum-unicode-codepoint"),
                      marlais_make_integer (UCHAR_MAX_VALUE),
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

static Object prim_char_block (Object chr)
{
  return marlais_make_integer (ublock_getCode (UCHARVAL (chr)));
}

static Object prim_char_name (Object chr)
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
