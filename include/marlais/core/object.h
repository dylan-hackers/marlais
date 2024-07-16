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

#if defined(MARLAIS_OBJECT_MODEL_BOXED)
#include <marlais/core/object-boxed.h>
#elif defined(MARLAIS_OBJECT_MODEL_TAGGED)
#include <marlais/core/object-tagged.h>
#else
#error No object model configured.
#endif

#include <marlais/core/globals.h>

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

struct marlais_empty {
    ObjectHeader header;
};

struct marlais_handle {
    ObjectHeader header;
    Object handle_reference;
};

#define HDLOBJ(obj)      (((struct marlais_handle *)obj)->handle_reference)

struct marlais_instance {
    ObjectHeader header;
    Object class;
    Object *slots;
};

#define INSTCLASS(obj)    (((struct marlais_instance *)obj)->class)
#define INSTSLOTS(obj)    (((struct marlais_instance *)obj)->slots)

/* Allocate an object with casting */
#define MARLAIS_ALLOCATE_OBJECT(_repr, _type)           \
  ((_type *)marlais_allocate_object(_repr, sizeof(_type)))

/* Cast an object */
#define MARLAIS_CAST_OBJECT(_obj, _repr, _type) \
  ((_type *)_obj)

/* Allocate an object */
extern Object marlais_allocate_object (ObjectType type, size_t size);

extern ObjectType marlais_object_type (Object obj);

extern Object marlais_object_class (Object obj);

extern Object marlais_make_handle (Object an_object);

#endif
