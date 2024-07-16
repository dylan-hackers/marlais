/* object.c -- see COPYRIGHT for use */

#include <marlais/common.h>

Object
marlais_allocate_object (marlais_repr_t type, size_t size)
{
  Object obj;
  /* allocate memory for the object */
  obj = (Object) marlais_malloc_general (size);
  /* initialize header fields */
#ifdef POINTERTYPE
  POINTERTYPE(obj) = type;
#endif
#ifdef POINTERSIZE
  POINTERSIZE(obj) = size;
#endif
  /* return result */
  return obj;
}

#ifdef MARLAIS_OBJECT_MODEL_TAGGED

marlais_repr_t
marlais_object_repr (Object obj)
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
#endif

Object
marlais_object_class (Object obj)
{
  switch (marlais_object_repr (obj)) {
  case Integer:
    return (marlais_class_small_integer);
  case True:
  case False:
    return (marlais_class_boolean);
  case Ratio:
    return (marlais_class_ratio);
  case SingleFloat:
    return (marlais_class_single_float);
  case DoubleFloat:
    return (marlais_class_double_float);
  case ExtendedFloat:
    return (marlais_class_extended_float);
  case EmptyList:
    return (marlais_class_empty_list);
  case Pair:
    return (marlais_class_pair);
  case ByteString:
    return (marlais_class_byte_string);
#ifdef MARLAIS_ENABLE_WCHAR
  case WideCharacter:
    return (marlais_class_wide_character);
  case WideString:
    return (marlais_class_wide_string);
#endif
#ifdef MARLAIS_ENABLE_UCHAR
  case UnicodeCharacter:
    return (marlais_class_unicode_character);
  case UnicodeString:
    return (marlais_class_unicode_string);
#endif
  case ByteVector:
    return (marlais_class_byte_vector);
  case ObjectVector:
    return (marlais_class_simple_object_vector);
  case ObjectTable:
    return (marlais_class_object_table);
  case ObjectDeque:
    return (marlais_class_object_deque);
  case ObjectArray:
    return (marlais_class_object_array);
  case Condition:
    return (marlais_class_condition);
  case Symbol:
    return (marlais_class_symbol);
  case Name:
    return (marlais_class_name);
  case Character:
    return (marlais_class_byte_character);
  case NextMethod:
    return (marlais_class_method);
  case Class:
    return (marlais_class_class);
  case Instance:
    return (INSTCLASS (obj));
  case LimitedIntType:
    return (marlais_class_limited_integer);
  case UnionType:
    return (marlais_class_union);
  case Primitive:
    return (marlais_class_primitive);
  case GenericFunction:
    return (marlais_class_generic_function);
  case Method:
    return (marlais_class_method);
  case UnwindFunction:
    return (marlais_class_exit_function);
  case UnwindProtect:
    return (marlais_class_unwind_protect_function);
  case UnspecifiedValue:
    return (marlais_class_object);
  case EndOfFile:
    return (marlais_class_object);
  case ObjectTableEntry:
    return (marlais_class_object_table_entry);
  case ObjectDequeEntry:
    return (marlais_class_object_deque_entry);
  case Singleton:
    return (marlais_class_singleton);
  case Subclass:
    return (marlais_class_subclass);
  case ObjectHandle:
    return (marlais_class_object_handle);
  case ForeignPtr:
    return (marlais_class_foreign_pointer);
  case UninitializedValue:
    return (marlais_class_object);
  case Environment:
    return (marlais_class_environment);
  case Module:
    return (marlais_class_module);
#ifdef MARLAIS_ENABLE_GMP
  case BigFloat:
    return (marlais_class_big_float);
  case BigRatio:
    return (marlais_class_big_ratio);
  case BigInteger:
    return (marlais_class_big_integer);
#endif
  default:
    return marlais_error ("object-class: don't know class of object", obj, NULL);
  }
}

Object
marlais_make_handle (Object an_object)
{
  Object new_handle;

  new_handle = marlais_allocate_object (ObjectHandle, sizeof (struct marlais_handle));

  HDLOBJ (new_handle) = an_object;
  return (new_handle);
}
