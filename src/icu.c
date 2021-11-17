
#include <marlais/icu.h>

#include <marlais/alloc.h>

#include <unicode/utypes.h>

/* Internal function declarations */

/* Allocation wrappers for ICU memory management */
static void *icu_gc_alloc (void *context, size_t size);
static void *icu_gc_realloc (void *context, void *obj, size_t new_size);
static void  icu_gc_free (void *context, void *obj);

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
                       &status);
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
  fprintf("Failed to initialize ICU: %s\n", u_errorName(ue));
  abort();
}

/* Internal functions */

static void *icu_gc_alloc (void *context, size_t size)
{
  return GC_alloc(size);
}

static void *icu_gc_realloc (void *context, void *obj, size_t new_size)
{
  return GC_realloc(obj, new_size);
}

static void icu_gc_free (void *context, void *obj)
{
  GC_free(obj);
}
