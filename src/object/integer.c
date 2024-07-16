/* number.c -- see COPYRIGHT for use */

#include <marlais/object/integer.h>

/* Internal variables */

/* Cache for <small-integer> */
#ifndef MARLAIS_OBJECT_MODEL_SMALL
#if MARLAIS_CONFIG_INTEGER_CACHE > 0
static Object integer_cache[MARLAIS_CONFIG_INTEGER_CACHE];
#endif
#endif

/* Exported functions */

#ifndef MARLAIS_OBJECT_MODEL_SMALL
/* small version is inline in marlais/number.h */
Object
marlais_make_integer (marlais_int_t i)
{
  Object obj;

#if MARLAIS_CONFIG_INTEGER_CACHE > 0
  if(i >= 0 && i < MARLAIS_CONFIG_INTEGER_CACHE) {
    if(integer_cache[i] != NULL) {
      return integer_cache[i];
    }
  }
#endif

  obj = marlais_allocate_object (Integer, sizeof (struct integer));
  INTVAL (obj) = i;

#if MARLAIS_CONFIG_INTEGER_CACHE > 0
  if(i >= 0 && i < MARLAIS_CONFIG_INTEGER_CACHE) {
    integer_cache[i] = obj;
  }
#endif

  return (obj);
}
#endif
