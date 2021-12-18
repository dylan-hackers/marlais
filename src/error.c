/* error.c -- see COPYRIGHT for use */

#include <marlais/error.h>

#include <marlais/alloc.h>
#include <marlais/apply.h>
#include <marlais/debug.h>
#include <marlais/string.h>
#include <marlais/class.h>
#include <marlais/env.h>
#include <marlais/eval.h>
#include <marlais/parser.h>
#include <marlais/prim.h>
#include <marlais/print.h>
#include <marlais/read.h>
#include <marlais/stream.h>

/* Primitives */

static Object signal_error_jump (void);
static Object dylan_error (Object msg_str, Object rest);
static Object dylan_warning (Object msg_str, Object rest);

static struct primitive error_prims[] =
{
  {"%signal-error-jump", prim_0, signal_error_jump},
  {"%error", prim_1_rest, dylan_error},
  {"%warning", prim_1_rest, dylan_warning},
};

/* Exported functions */

void
marlais_register_error (void)
{
  int num = sizeof (error_prims) / sizeof (struct primitive);
  marlais_register_prims (num, error_prims);
}

Object
marlais_warning (const char *msg,...)
{
  va_list args;
  Object obj;

  va_start (args, msg);
  fprintf (stderr, "warning: %s", msg);
  obj = va_arg (args, Object);

  if (obj) {
    fprintf (stderr, ": ");
  }
  while (obj) {
    marlais_print_object (marlais_standard_error, obj, 0);
    obj = va_arg (args, Object);

    if (obj) {
      fprintf (stderr, ", ");
    }
  }
  fprintf (stderr, ".\n");
  return MARLAIS_UNSPECIFIED;
}

Object
marlais_error (const char *msg, ...)
{
  va_list args;
  Object obj, signal_value, ret;

  va_start (args, msg);
  fprintf (stderr, "error: %s", msg);
  obj = va_arg (args, Object);

  if (obj) {
    fprintf (stderr, ": ");
  }
  while (obj) {
    marlais_print_object (marlais_standard_error, obj, 0);
    obj = va_arg (args, Object);

    if (obj) {
      fprintf (stderr, ", ");
    }
  }
  fprintf (stderr, ".\n");

  /* TODO make this optional? dispatch to dylan? */
  ret = marlais_debugger();
  if (ret != MARLAIS_UNSPECIFIED) {
    return ret;
  }

  /* TODO this should be a lookup in dylan module */
  signal_value = marlais_symbol_value (signal_symbol);
  if (signal_value) {
    ret = marlais_apply (signal_value,
                         marlais_cons (marlais_make (simple_error_class, MARLAIS_NIL),
                                       MARLAIS_NIL));
  } else {
    longjmp (*marlais_error_jump, 1);
  }

  return ret;
}

void
marlais_fatal (const char *msg, ...)
{
  va_list args;
  Object obj;

  va_start (args, msg);
  fprintf (stderr, "fatal error: %s", msg);
  obj = va_arg (args, Object);

  if (obj) {
    fprintf (stderr, ": ");
  }
  while (obj) {
    marlais_print_object (marlais_standard_error, obj, 0);
    obj = va_arg (args, Object);

    if (obj) {
      fprintf (stderr, ", ");
    }
  }
  fprintf (stderr, ".\n");

  abort();
}

/* Primitives */

static Object
signal_error_jump ()
{
  longjmp (*marlais_error_jump, 1);
}

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

static Object
dylan_error (Object msg_str, Object rest)
{
  print_dylan_error_helper("error", msg_str, rest);
  longjmp (*marlais_error_jump, 1);
}

static Object
dylan_warning (Object msg_str, Object rest)
{
  print_dylan_error_helper("warning", msg_str, rest);
  return MARLAIS_UNSPECIFIED;
}
