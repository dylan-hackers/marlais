/* error.c -- see COPYRIGHT for use */

#include <marlais/core/error.h>

#include <marlais/core/alloc.h>
#include <marlais/core/apply.h>
#include <marlais/core/debug.h>
#include <marlais/core/env.h>
#include <marlais/core/eval.h>
#include <marlais/core/print.h>
#include <marlais/core/read.h>
#include <marlais/object/class.h>
#include <marlais/object/prim.h>
#include <marlais/object/stream.h>
#include <marlais/parser/parser.h>

/* Primitives */

static Object prim_signal (void);
static Object prim_error (Object msg_str, Object rest);
static Object prim_warning (Object msg_str, Object rest);

static struct primitive error_prims[] =
{
  {"%signal", prim_0, prim_signal},
  {"%error", prim_1_rest, prim_error},
  {"%warning", prim_1_rest, prim_warning},
};

/* Exported functions */

void
marlais_register_error (void)
{
  MARLAIS_REGISTER_PRIMS (error_prims);
}

/* Internal functions */

static void
print_dylan_error_helper(const char* kind, Object msg_str, Object rest)
{
  fprintf (stderr, "%s: %s", kind, BYTESTRVAL (msg_str));
  if (!EMPTYLISTP (rest)) {
    fprintf (stderr, ": ");
  }
  while (!EMPTYLISTP (rest)) {
    marlais_print_object (marlais_standard_error, CAR (rest), 0);
    rest = CDR (rest);
    if (!EMPTYLISTP (rest)) {
      fprintf (stderr, ", ");
    }
  }
  fprintf (stderr, ".\n");
}

/* Primitives */

static Object
prim_signal (void)
{
  longjmp (*marlais_error_jump, 1);
}

static Object
prim_error (Object msg_str, Object rest)
{
  print_dylan_error_helper("error", msg_str, rest);
  longjmp (*marlais_error_jump, 1);
}

static Object
prim_warning (Object msg_str, Object rest)
{
  print_dylan_error_helper("warning", msg_str, rest);
  return MARLAIS_UNSPECIFIED;
}
