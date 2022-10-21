/* object.h -- see COPYRIGHT for use */

#ifndef MARLAIS_OBJECT_H
#define MARLAIS_OBJECT_H

typedef intptr_t  marlais_int_t;
#define MARLAIS_INT_MIN   (INTPTR_MIN)
#define MARLAIS_INT_MAX   (INTPTR_MAX)
#if 0
#define MARLAIS_INT_WIDTH (INTPTR_WIDTH)
#endif
#define MARLAIS_INT_PRI   PRIdPTR

typedef uintptr_t marlais_uint_t;
#define MARLAIS_UINT_MAX   (UINTPTR_MAX)
#if 0
#define MARLAIS_UINT_WIDTH (UINTPTR_WIDTH)
#endif
#define MARLAIS_UINT_PRI    PRIuPTR

typedef enum {
    Uninitialized = 0,

    /* core constants */
    True, False, EmptyList, EndOfFile, UnspecifiedValue, UninitializedValue,

    /* characters */
    Character, WideCharacter, UnicodeCharacter,

    /* numbers */
    Integer, SingleFloat, DoubleFloat, ExtendedFloat,

    /* collections */
    Pair,
    ByteVector,
    ObjectVector,
    ObjectArray,
    ObjectDeque, ObjectDequeEntry,
    ObjectTable, ObjectTableEntry,

    /* strings */
    ByteString, WideString, UnicodeString,

    /* conditions */
    Condition,

    /* symbols */
    Symbol, Name,

    /* types */
    Class, Singleton, Subclass, LimitedIntType, UnionType, SlotDescriptor,

    /* functions */
    Primitive, GenericFunction, Method, Function, NextMethod,

    /* multiple values */
    Values,

    /* instances */
    Instance,

    /* GMP numbers */
    BigFloat, BigRatio, BigInteger,

    /* miscelaneous */
    ObjectHandle,
    ForeignPtr,

    /* bindings */
    Environment,
    Module,
    UnwindFunction, UnwindProtect,

    /* C level interfaces */
    StdioHandle,

    Ratio,
} ObjectType;

/* Common header of all heap objects */
typedef struct {
  /* Low-level type of the object */
  ObjectType object_type;
#if 0
  /* Size of the object in bytes */
  size_t     object_size;
#endif
} ObjectHeader;

/* Accessor for the type of a heap object */
#define POINTERTYPE(obj) (((ObjectHeader *)obj)->object_type)
//#define POINTERSIZE(obj) (((ObjectHeader *)obj)->object_size)

#if defined(MARLAIS_OBJECT_MODEL_SMALL)
#include <marlais/object-small.h>
#elif defined(MARLAIS_OBJECT_MODEL_LARGE)
#include <marlais/object-large.h>
#else
#error No object model configured.
#endif

#include <marlais/object-heap.h>

#include <marlais/globals.h>

#ifdef MARLAIS_OBJECT_MODEL_SMALL
#define MARLAIS_TRUE  (TRUEVAL)
#define MARLAIS_FALSE (FALSEVAL)
#define MARLAIS_NIL (EMPTYLISTVAL)
#define MARLAIS_EOF (EOFVAL)
#define MARLAIS_UNSPECIFIED (UNSPECVAL)
#define MARLAIS_UNINITIALIZED (UNINITVAL)
#else
#define MARLAIS_TRUE  (marlais_true)
#define MARLAIS_FALSE (marlais_false)
#define MARLAIS_NIL (marlais_nil)
#define MARLAIS_EOF  (marlais_eof)
#define MARLAIS_UNSPECIFIED (marlais_unspecified)
#define MARLAIS_UNINITIALIZED (marlais_uninitialized)
#endif

/* Constructor for booleans */
static inline Object marlais_make_boolean(bool b) {
  return b ? MARLAIS_TRUE : MARLAIS_FALSE;
}

/* Additional predicates */
static inline bool ZEROP(Object obj) {
  return INTEGERP(obj) && (INTVAL(obj) == 0);
}
static inline bool UNSIGNEDP(Object obj) {
  return INTEGERP(obj) && (INTVAL(obj) >= 0);
}
static inline bool NULLP(Object obj) {
  return EMPTYLISTP(obj);
}
static inline bool LISTP(Object obj) {
  return NULLP(obj)||PAIRP(obj);
}

Object make_handle (Object an_object);

ObjectType marlais_object_type (Object obj);

#define MARLAIS_CAST_OBJECT(_obj, _mtype, _ctype)        \
  ((_ctype *)_obj)

#endif
