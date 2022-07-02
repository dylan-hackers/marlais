
#include <marlais/bytevector.h>

#include <marlais/alloc.h>
#include <marlais/prim.h>
#include <marlais/sequence.h>

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
  int num = sizeof (bytevector_prims) / sizeof (struct primitive);
  marlais_register_prims(num, bytevector_prims);
}

Object
marlais_make_bytevector (int size, uint8_t fill)
{
  struct marlais_bytevector *res;

  res = MARLAIS_ALLOCATE_OBJECT(ByteVector, struct marlais_bytevector);

  res->bv_size = size;
  res->bv_data = (uint8_t *) marlais_allocate_atomic(size);

  if(fill != 0) {
    memset(res->bv_data, fill, size);
  }

  return res;
}

Object
marlais_make_bytevector_entrypoint (Object args)
{
  int size;
  Object size_obj, fill_obj;

  marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<byte-vector>");

  // TODO errors
  if(fill_obj == MARLAIS_FALSE || !UNSIGNEDP(fill_obj)) {
    fill_obj = marlais_make_integer(0);
  }
  if(size_obj == NULL || !UNSIGNEDP(size_obj)) {
    size_obj = marlais_make_integer(0);
  }

  return marlais_make_bytevector (INTVAL(size_obj), INTVAL(fill_obj));
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
