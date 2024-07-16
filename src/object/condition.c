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

/* Exported functions */

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
                         marlais_cons (marlais_make (marlais_class_simple_error, MARLAIS_NIL),
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
