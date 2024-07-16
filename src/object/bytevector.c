
#include <marlais/object/bytevector.h>

#include <marlais/object/sequence.h>

/* Exported functions */

Object
marlais_make_bytevector (int size, uint8_t fill)
{
  struct marlais_bytevector *res;

  res = MARLAIS_ALLOCATE_OBJECT(ByteVector, struct marlais_bytevector);

  res->bv_size = size;
  res->bv_data = (uint8_t *) marlais_malloc_atomic(size);

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
