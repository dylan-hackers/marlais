/* object.h -- see COPYRIGHT for use */

#ifndef MARLAIS_OBJECT_H
#define MARLAIS_OBJECT_H

typedef intptr_t  DyInteger;
#define MARLAIS_INT_MIN   (INTPTR_MIN)
#define MARLAIS_INT_MAX   (INTPTR_MAX)
#if 0
#define MARLAIS_INT_WIDTH (INTPTR_WIDTH)
#endif
#define MARLAIS_INT_PRI   PRIdPTR

typedef uintptr_t DyUnsigned;
#define MARLAIS_UINT_MAX   (UINTPTR_MAX)
#if 0
#define MARLAIS_UINT_WIDTH (UINTPTR_WIDTH)
#endif
#define MARLAIS_UINT_PRI    PRIuPTR

typedef enum {
    Uninitialized = 0,

    /* names */
    Name,

    /* booleans */
    True, False,

    /* numbers */
    Integer, BigInteger, Ratio, SingleFloat, DoubleFloat,

    /* collections */
    EmptyList, Pair, SimpleObjectVector,
    ObjectTable, Deque, Array,

    /* strings */
    ByteString, WideString, UnicodeCharacter,

    /* conditions */
    Condition,

    /* symbols */
    Symbol,

    /* characters */
    Character, WideCharacter, UnicodeString,

    /* types */
    Class, Instance, Singleton, LimitedIntType, UnionType, SlotDescriptor,

    /* functions */
    Primitive, GenericFunction, Method, Function, NextMethod,

    /* misc */
    EndOfFile, Values, Unspecified, Exit, Unwind,
#ifdef NO_COMMON_DYLAN_SPEC
Stream,
#endif
    TableEntry, UninitializedSlotValue, DequeEntry,
    ObjectHandle,
    ForeignPtr,			/* <pcb> */
    Environment,

    /* GMP numbers */
    MPFloat, MPRatio, MPInteger,

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

#ifdef MARLAIS_ENABLE_GMP
#include <marlais/object-gmp.h>
#endif

#ifdef MARLAIS_OBJECT_MODEL_SMALL
#define MARLAIS_TRUE  (TRUEVAL)
#define MARLAIS_FALSE (FALSEVAL)
#else
#define MARLAIS_TRUE  (marlais_true)
#define MARLAIS_FALSE (marlais_false)
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

/* important objects */
extern Object key_symbol, hash_rest_symbol, next_symbol;
extern Object quote_symbol;
extern Object getter_keyword, setter_keyword, else_keyword;
extern Object type_keyword, init_value_keyword, init_function_keyword;
extern Object init_keyword_keyword, required_init_keyword_keyword, allocation_keyword;
extern Object unwind_symbol, next_method_symbol, initialize_symbol;
extern Object equal_hash_symbol;
extern Object quasiquote_symbol, unquote_symbol, unquote_splicing_symbol;

/* builtin classes */
extern Object object_class;
extern Object name_class;
extern Object boolean_class;
extern Object number_class, real_class, integer_class, ratio_class;
extern Object single_float_class, double_float_class;
extern Object collection_class, sequence_class, mutable_sequence_class;
extern Object mutable_collection_class;
extern Object list_class, empty_list_class, pair_class, string_class;
extern Object byte_string_class, vector_class, simple_object_vector_class;
extern Object stretchy_vector_class;
extern Object explicit_key_collection_class, mutable_explicit_key_collection_class;
extern Object table_class, deque_class, array_class;
extern Object condition_class;
extern Object symbol_class;
extern Object character_class;
extern Object function_class, primitive_class, generic_function_class,
  method_class;
extern Object class_class, table_entry_class, deque_entry_class;

#ifdef NO_COMMON_DYLAN_SPEC
stream_class,
#endif

/* convenience macro functions */
#define FIRST(obj)      (CAR(obj))
#define SECOND(obj)     (CAR(CDR(obj)))
#define THIRD(obj)      (CAR(CDR(CDR(obj))))
#define FOURTH(obj)     (CAR(CDR(CDR(CDR(obj)))))
#define FIFTH(obj)      (CAR(CDR(CDR(CDR(CDR(obj))))))

#define FIRSTVALP(vals)  (VALUESNUM(vals)>0)
#define SECONDVALP(vals) (VALUESNUM(vals)>1)
#define THIRDVALP(vals)  (VALUESNUM(vals)>2)
#define FOURTHVALP(vals) (VALUESNUM(vals)>3)
#define FIFTHVALP(vals)  (VALUESNUM(vals)>4)

#define FIRSTVAL(vals)  (VALUESELS(vals)[0])
#define SECONDVAL(vals) (VALUESELS(vals)[1])
#define THIRDVAL(vals)  (VALUESELS(vals)[2])
#define FOURTHVAL(vals) (VALUESELS(vals)[3])
#define FIFTHVAL(vals)  (VALUESELS(vals)[4])

ObjectType object_type (Object obj);

#endif
