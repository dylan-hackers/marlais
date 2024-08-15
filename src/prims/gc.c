
#include <marlais/core/gc.h>

#include <marlais/object/prim.h>

#include <gc.h>

/* Primitives */

static Object prim_gc_collect (void);
static Object prim_gc_disable (void);
static Object prim_gc_enable (void);
static Object prim_gc_enabled_p (void);
static Object prim_gc_expand (Object amount);
static Object prim_gc_report (void);

static struct primitive gc_prims[] =
{
    {"%gc-collect",  prim_0, prim_gc_collect},
    {"%gc-disable",  prim_0, prim_gc_disable},
    {"%gc-enable",   prim_0, prim_gc_enable},
    {"%gc-enabled?", prim_0, prim_gc_enabled_p},
    {"%gc-expand",   prim_1, prim_gc_expand},
    {"%gc-report",   prim_0, prim_gc_report},
};

/* Exported functions */

void
marlais_register_gc (void)
{
  MARLAIS_REGISTER_PRIMS (gc_prims);
}

/* Primitives */

static Object prim_gc_collect (void)
{
  marlais_gc_collect();
  return MARLAIS_UNSPECIFIED;
}

static Object prim_gc_disable (void)
{
  GC_disable();
  return MARLAIS_UNSPECIFIED;
}

static Object prim_gc_enable (void)
{
  GC_enable();
  return MARLAIS_UNSPECIFIED;
}

static Object prim_gc_enabled_p (void)
{
  return marlais_make_boolean(!GC_is_disabled());
}

static Object prim_gc_expand (Object amount)
{
  if(!marlais_uint_p (amount)) {
    marlais_error("%gc-expand: amount must be an unsigned integer\n", amount, NULL);
  }
  GC_expand_hp(marlais_get_uint(amount));
  return MARLAIS_UNSPECIFIED;
}

static Object prim_gc_report (void)
{
  marlais_gc_report();
  return MARLAIS_UNSPECIFIED;
}
