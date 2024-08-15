/* number.c -- see COPYRIGHT for use */

#include <marlais/object/integer.h>

/* Internal variables */

/* Cache for <small-integer> */
#ifndef MARLAIS_OBJECT_MODEL_TAGGED
#if MARLAIS_CONFIG_INTEGER_CACHE > 0
static Object integer_cache[MARLAIS_CONFIG_INTEGER_CACHE];
#endif
#endif

/* Exported functions */

#ifndef MARLAIS_OBJECT_MODEL_TAGGED
/* small version is inline in marlais/number.h */
Object
marlais_make_integer (marlais_int_t i)
{
  struct marlais_integer *obj;

#if MARLAIS_CONFIG_INTEGER_CACHE > 0
  if(i >= 0 && i < MARLAIS_CONFIG_INTEGER_CACHE) {
    if(integer_cache[i] != NULL) {
      return integer_cache[i];
    }
  }
#endif

  obj = MARLAIS_ALLOCATE_OBJECT (Integer, struct marlais_integer);
  obj->integer_value = i;

#if MARLAIS_CONFIG_INTEGER_CACHE > 0
  if(i >= 0 && i < MARLAIS_CONFIG_INTEGER_CACHE) {
    integer_cache[i] = obj;
  }
#endif

  return (obj);
}
#endif
