/* object.c -- see COPYRIGHT for use */

#include <marlais/common.h>

#include <marlais/alloc.h>
#include <marlais/number.h>

#ifdef MARLAIS_OBJECT_MODEL_SMALL

ObjectType
object_type (Object obj)
{
  if (POINTERP (obj)) {
    return (POINTERTYPE (obj));
  } else if (INTEGERP (obj)) {
    return (Integer);
  } else if (IMMEDP (obj)) {
    switch (SUBPART (obj)) {
    case MARLAIS_SUB_TRUE:
      return (True);
    case MARLAIS_SUB_FALSE:
      return (False);
    case MARLAIS_SUB_EMPTYLIST:
      return (EmptyList);
    case MARLAIS_SUB_EOF:
      return (EndOfFile);
    case MARLAIS_SUB_UNSPECIFIED:
      return (UnspecifiedValue);
    case MARLAIS_SUB_UNINITIALIZED:
      return (UninitializedValue);
    case MARLAIS_SUB_CHARACTER:
      return (Character);
#ifdef MARLAIS_ENABLE_WCHAR
    case MARLAIS_SUB_WCHAR:
      return (WideCharacter);
#endif
#ifdef MARLAIS_ENABLE_UCHAR
    case MARLAIS_SUB_UCHAR:
      return (UnicodeCharacter);
#endif
    default:
      break;
    }
  }
  return (Uninitialized);
}

#else

ObjectType
object_type (Object obj) {
  return POINTERTYPE(obj);
}

#endif

Object
make_handle (Object an_object)
{
  Object new_handle;

  new_handle = marlais_allocate_object (ObjectHandle, sizeof (struct object_handle));

  HDLOBJ (new_handle) = an_object;
  return (new_handle);
}
