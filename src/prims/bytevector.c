
#include <marlais/object/bytevector.h>

#include <marlais/object/sequence.h>
#include <marlais/object/prim.h>

/* Primitives */

static Object prim_bytevector_size (Object vec);
static Object prim_bytevector_element (Object vec, Object index, Object def);
static Object prim_bytevector_element_setter (Object vec, Object index, Object val);

static struct primitive bytevector_prims[] =
{
    {"%bytevector-size", prim_1, prim_bytevector_size},
    {"%bytevector-element", prim_3, prim_bytevector_element},
    {"%bytevector-element-setter", prim_3, prim_bytevector_element_setter},
};

/* Exported functions */

void
marlais_register_bytevector (void)
{
  MARLAIS_REGISTER_PRIMS(bytevector_prims);
}

/* Primitives */

static Object
prim_bytevector_size (Object vec)
{
  struct marlais_bytevector *v = MARLAIS_CAST_BYTEVECTOR(vec);
  return marlais_make_integer(v->bv_size);
}

static Object
prim_bytevector_element (Object vec, Object index, Object default_obj)
{
  struct marlais_bytevector *v = MARLAIS_CAST_BYTEVECTOR(vec);
  int i, size;

  i = INTVAL (index);
  size = v->bv_size;
  if ((i < 0) || (i >= size)) {
    if (default_obj == marlais_default) {
      marlais_error("element: index out of range", vec, index, NULL);
    } else {
      return default_obj;
    }
  }
  return marlais_make_integer(v->bv_data[i]);
}

static Object
prim_bytevector_element_setter (Object vec, Object index, Object value)
{
  struct marlais_bytevector *v = MARLAIS_CAST_BYTEVECTOR(vec);
  int i, size;
  uint8_t new;

  i = INTVAL (index);
  size = v->bv_size;
  if ((i < 0) || (i >= size)) {
    marlais_error ("element-setter: index out of range", vec, index, NULL);
  }
  new = INTVAL (value) & 0xFF;
  return marlais_make_integer (v->bv_data[i] = new);
}
