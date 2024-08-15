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

typedef uintptr_t marlais_size_t;
typedef uintptr_t marlais_index_t;

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
} marlais_repr_t;

struct marlais_header;
struct marlais_empty;
struct marlais_instance;
struct marlais_handle;

typedef struct marlais_header marlais_header_t;
typedef struct marlais_empty marlais_empty_t;
typedef struct marlais_instance marlais_instance_t;
typedef struct marlais_handle marlais_handle_t;

/* Common header of all heap objects */
struct marlais_header {
  /* Low-level type of the object */
  marlais_repr_t object_repr;
};

#define POINTERTYPE(obj) (((marlais_header_t *)(obj))->object_repr)

#if defined(MARLAIS_OBJECT_MODEL_BOXED)
#include <marlais/core/object-boxed.h>
#elif defined(MARLAIS_OBJECT_MODEL_TAGGED)
#include <marlais/core/object-tagged.h>
#else
#error No object model configured.
#endif

/* Empty objects (used for constants in boxed model) */
struct marlais_empty {
    marlais_header_t header;
};

/* Instances of user-defined classes */
struct marlais_instance {
    marlais_header_t header;
    Object class;
    Object *slots;
};

#define INSTCLASS(obj)    (((struct marlais_instance *)obj)->class)
#define INSTSLOTS(obj)    (((struct marlais_instance *)obj)->slots)

/* Object handles (used internally) */
struct marlais_handle {
    marlais_header_t header;
    Object handle_reference;
};

#define HDLOBJ(obj)      (((struct marlais_handle *)obj)->handle_reference)

#include <marlais/core/globals.h>

/* Allocate an object with casting */
#define MARLAIS_ALLOCATE_OBJECT(_repr, _type)                   \
  ((_type *)marlais_allocate_object(_repr, sizeof(_type)))
/* Allocate an object with extra space and casting */
#define MARLAIS_ALLOCATE_OBJECT_EXTRA(_repr, _type, _extra)     \
  ((_type *)marlais_allocate_object(_repr, sizeof(_type) + (_extra)))

/* Cast an object */
#define MARLAIS_CAST_OBJECT(_obj, _repr, _type) \
  ((_type *)_obj)

/* Determine the object class of an object */
extern Object marlais_object_class (Object obj);

/* Allocate an object */
extern Object marlais_allocate_object (marlais_repr_t repr, size_t size);

/* Make a handle for the given object */
extern Object marlais_make_handle (Object obj);

#endif
