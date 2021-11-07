/* object.h -- see COPYRIGHT for use */

#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <setjmp.h>
#include <limits.h>

typedef intptr_t  DyInteger;
typedef uintptr_t DyUnsigned;

typedef enum {
    Uninitialized = 0,

    /* booleans */
    True, False,

    /* numbers */
    Integer, BigInteger, Ratio, SingleFloat, DoubleFloat,

    /* collections */
    EmptyList, Pair, ByteString, SimpleObjectVector,
    ObjectTable, Deque, Array,

    /* conditions */
    Condition,

    /* keywords and symbols */
    Symbol, Keyword,

    /* characters */
    Character,

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
    Environment
} ObjectType;

#ifdef SMALL_OBJECTS
#include "object-small.h"
#else
#include "object-large.h"
#endif

#ifdef SMALL_OBJECTS
#define MARLAIS_TRUE  (TRUEVAL)
#define MARLAIS_FALSE (FALSEVAL)
#else
#define MARLAIS_TRUE  (marlais_true)
#define MARLAIS_FALSE (marlais_false)
#endif

/* globals */
extern jmp_buf error_return;

Object make_handle (Object an_object);

/* important objects */
extern Object true_object, false_object;
extern Object eof_object, unspecified_object, uninit_slot_object;
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
extern Object symbol_class, keyword_class;
extern Object character_class;
extern Object function_class, primitive_class, generic_function_class,
  method_class;
extern Object class_class, table_entry_class, deque_entry_class;

#ifdef NO_COMMON_DYLAN_SPEC
stream_class,
#endif

/* from alloc.c */
extern Object allocate_object (size_t size);

/* convenience macro functions */
#define FIRST(obj)      (CAR(obj))
#define SECOND(obj)     (CAR(CDR(obj)))
#define THIRD(obj)      (CAR(CDR(CDR(obj))))
#define FOURTH(obj)     (CAR(CDR(CDR(CDR(obj)))))
#define FIFTH(obj)      (CAR(CDR(CDR(CDR(CDR(obj))))))

#define FIRSTVAL(vals)  (VALUESELS(vals)[0])
#define SECONDVAL(vals) (VALUESELS(vals)[1])
#define THIRDVAL(vals)  (VALUESELS(vals)[2])
#define FOURTHVAL(vals) (VALUESELS(vals)[3])
#define FIFTHVAL(vals)  (VALUESELS(vals)[4])

/* arbitrary constants */
#define MAX_STRING_SIZE 10240
#define MAX_SYMBOL_SIZE 1024
#define MAX_NUMBER_SIZE 255

ObjectType object_type (Object obj);

#endif
