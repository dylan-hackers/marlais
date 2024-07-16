
#include <marlais/object/type.h>

#include <marlais/core/alloc.h>
#include <marlais/object/class.h>
#include <marlais/object/prim.h>

/* Primitives */

static Object prim_instance_p (Object obj, Object class);
static Object prim_subtype_p (Object class1, Object class2);

static struct primitive type_prims[] =
{
    {"%instance?", prim_2, prim_instance_p},
    {"%subtype?", prim_2, prim_subtype_p},
    {"%make-limited-integer", prim_1, marlais_make_limited_integer},
    {"%make-singleton", prim_1, marlais_make_singleton},
    {"%make-subclass", prim_1, marlais_make_subclass},
    {"%make-union", prim_1, marlais_make_union},
};

/* Exported functions */

void
marlais_register_type (void)
{
  MARLAIS_REGISTER_PRIMS (type_prims);
}

/* Primitives */

static Object
prim_instance_p (Object obj, Object type)
{
  return marlais_make_boolean (marlais_instance_p (obj, type));
}

static Object
prim_subtype_p (Object type1, Object type2)
{
  return marlais_make_boolean (marlais_subtype_p (type1, type2));
}
