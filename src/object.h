/* object.h -- see COPYRIGHT for use */

#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>

enum objtype {
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
};

#ifndef SMALL_OBJECTS

typedef struct object *Object;

#define TRUEP(obj)        ((obj)->type == True)
#define FALSEP(obj)       ((obj)->type == False)

struct integer {
    int val;
};

#define INTVAL(obj)       ((obj)->u.integer.val)
#define INTEGERP(obj)     ((obj)->type == Integer)

struct big_integer {
    void *val;
};

#define BIGINTVAL(obj)    ((obj)->u.big_integer.val)
#define BIGINTP(obj)      ((obj)->type == BigInteger)
#define BIGINTTYPE(obj)   ((obj)->type)

struct ratio {
    int numerator, denominator;
};

#define RATIOTYPE(obj)    ((obj)->type)
#define RATIONUM(obj)     ((obj)->u.ratio.numerator)
#define RATIODEN(obj)     ((obj)->u.ratio.denominator)
#define RATIOP(obj)       ((obj)-type == Ratio)

struct single_float {
    float val;
};

#define SFLOATVAL(obj)    ((obj)->u.single_float.val)
#define SFLOATP(obj)      ((obj)->type == SingleFloat)

struct double_float {
    double val;
};

#define DFLOATVAL(obj)    ((obj)->u.double_float.val)
#define DFLOATP(obj)      ((obj)->type == DoubleFloat)
#define DFLOATTYPE(obj)   ((obj)->type)

#define EMPTYLISTP(obj)   ((obj)->type == EmptyList)
#define NULLP(obj)        ((obj)->type == EmptyList)

struct pair {
    Object car, cdr;
};

#define CAR(obj)          ((obj)->u.pair.car)
#define CDR(obj)          ((obj)->u.pair.cdr)
#define PAIRP(obj)        ((obj)->type == Pair)
#define PAIRTYPE(obj)     ((obj)->type)

struct byte_string {
    int size;
    char *val;
};

#define BYTESTRSIZE(obj)  ((obj)->u.byte_string.size)
#define BYTESTRVAL(obj)   ((obj)->u.byte_string.val)
#define BYTESTRP(obj)     ((obj)->type == ByteString)
#define BYTESTRTYPE(obj)  ((obj)->type)

struct simple_object_vector {
    int size;
    Object *els;
};

#define SOVSIZE(obj)      ((obj)->u.simple_object_vector.size)
#define SOVELS(obj)       ((obj)->u.simple_object_vector.els)
#define SOVP(obj)         ((obj)->type == SimpleObjectVector)
#define SOVTYPE(obj)      ((obj)->type)

struct table_entry {
    int row;
    Object key;
    Object value;
    Object next;
};

#define TEROW(obj)        ((obj)->u.table_entry.row)
#define TEKEY(obj)        ((obj)->u.table_entry.key)
#define TEVALUE(obj)      ((obj)->u.table_entry.value)
#define TENEXT(obj)       ((obj)->u.table_entry.next)
#define TEP(obj)          ((obj)->type == TableEntry)
#define TETYPE(obj)       ((obj)->type)

struct table {
    int size;
    Object *the_table;
};

#define TABLESIZE(obj)    ((obj)->u.table.size)
#define TABLETABLE(obj)   ((obj)->u.table.the_table)
#define TABLEP(obj)       ((obj)->type == ObjectTable)
#define TABLETYPE(obj)    ((obj)->type)

struct deque_entry {
    Object value;
    Object prev, next;
};

#define DEVALUE(obj)      ((obj)->u.deque_entry.value)
#define DEPREV(obj)       ((obj)->u.deque_entry.prev)
#define DENEXT(obj)       ((obj)->u.deque_entry.next)
#define DEP(obj)          ((obj)->type == DequeEntry)
#define DETYPE(obj)       ((obj)->type)

struct deque {
    Object first, last;
};

#define DEQUEFIRST(obj)   ((obj)->u.deque.first)
#define DEQUELAST(obj)    ((obj)->u.deque.last)
#define DEQUEP(obj)       ((obj)->type == Deque)
#define DEQUETYPE(obj)    ((obj)->type)

struct array {
    int size;
    Object dimensions;
    Object *elements;
};

#define ARRSIZE(obj)      ((obj)->u.array.size)
#define ARRDIMS(obj)      ((obj)->u.array.dimensions)
#define ARRELS(obj)       ((obj)->u.array.elements)
#define ARRAYP(obj)       ((obj)->type == Array)
#define ARRTYPE(obj)      ((obj)->type)

enum condtype {
    SimpleError, TypeError, SimpleWarning,
    SimpleRestart, Abort
};
struct condition {
    enum condtype type;
};

#define CONDCTYPE(obj)     ((obj)->u.condition.type)
#define CONDP(obj)         ((obj)->type == Condition)
#define CONDTYPE(obj)      ((obj)->type)

struct symbol {
    char *name;
};

#define SYMBOLNAME(obj)   ((obj)->u.symbol.name)
#define SYMBOLP(obj)      ((obj)->type == Symbol)
#define SYMBOLTYPE(obj)   ((obj)->type)
#define KEYNAME(obj)      ((obj)->u.symbol.name)
#define KEYWORDP(obj)     ((obj)->type == Keyword)

struct character {
    char val;
};

#define CHARVAL(obj)      ((obj)->u.character.val)
#define CHARP(obj)        ((obj)->type == Character)

struct slot_descriptor {
    unsigned char properties;
    Object getter;
    Object setter;
    Object slot_type;
    Object init;
    Object init_keyword;
    Object allocation;
    Object dynamism;
};

#define SLOTDPROPS(obj)         ((obj)->u.slot_descriptor.properties)
#define SLOTDKEYREQMASK         0x01
#define SLOTDINHERITEDMASK      0x02
#define SLOTDINITFUNCTIONMASK   0x04
#define SLOTDDEFERREDTYPEMASK	0x08
#define SLOTDKEYREQ(obj)        (SLOTDPROPS (obj) & SLOTDKEYREQMASK)
#define SLOTDINHERITED(obj)     (SLOTDPROPS (obj) & SLOTDINHERITEDMASK)
#define SLOTDINITFUNCTION(obj)  (SLOTDPROPS (obj) & SLOTDINITFUNCTIONMASK)
#define SLOTDDEFERREDTYPE(obj)  (SLOTDPROPS (obj) & SLOTDDEFERREDTYPEMASK)
#define SLOTDGETTER(obj)        ((obj)->u.slot_descriptor.getter)
#define SLOTDSETTER(obj)        ((obj)->u.slot_descriptor.setter)
#define SLOTDSLOTTYPE(obj)      ((obj)->u.slot_descriptor.slot_type)
#define SLOTDINIT(obj)          ((obj)->u.slot_descriptor.init)
#define SLOTDINITKEYWORD(obj)   ((obj)->u.slot_descriptor.init_keyword)
#define SLOTDALLOCATION(obj)    ((obj)->u.slot_descriptor.allocation)
#define SLOTDDYNAMISM(obj)      ((obj)->u.slot_descriptor.dynamism)
#define SLOTDP(obj)             ((obj)->type == SlotDescriptor)
#define SLOTDTYPE(obj)          ((obj)->type)

struct clas {
    Object name;
    Object supers;
    Object subs;
    Object inherited_slot_descriptors;
    Object inst_slot_descriptors;
    Object class_slot_descriptors;
    Object class_slots;
    Object eachsubclass_slot_descriptors;
    Object eachsubclass_slots;
    Object constant_slot_descriptors;
    Object virtual_slot_descriptors;
    Object precedence_list;
    Object sorted_prec_list;
    int num_precs;
    int properties;
    int ordinal_index;
    struct frame *creation_env;
};

#define CLASSNAME(obj)     ((obj)->u.clas.name)
#define CLASSSUPERS(obj)   ((obj)->u.clas.supers)
#define CLASSSUBS(obj)     ((obj)->u.clas.subs)
#define CLASSSLOTDS(obj)   ((obj)->u.clas.inst_slot_descriptors)
#define CLASSINSLOTDS(obj) ((obj)->u.clas.inherited_slot_descriptors)
#define CLASSCSLOTDS(obj)  ((obj)->u.clas.class_slot_descriptors)
#define CLASSCSLOTS(obj)   ((obj)->u.clas.class_slots)
#define CLASSESSLOTDS(obj) ((obj)->u.clas.eachsubclass_slot_descriptors)
#define CLASSESSLOTS(obj)  ((obj)->u.clas.eachsubclass_slots)
#define CLASSCONSTSLOTDS(obj) ((obj)->u.clas.constant_slot_descriptors)
#define CLASSVSLOTDS(obj)  ((obj)->u.clas.virtual_slot_descriptors)
#define CLASSPRECLIST(obj) ((obj)->u.clas.precedence_list)
#define CLASSSORTEDPRECS(obj) ((obj)->u.clas.sorted_prec_list)
#define CLASSNUMPRECS(obj) ((obj)->u.clas.num_precs)
#define CLASSP(obj)        ((obj)->type == Class)
#define CLASSTYPE(obj)     ((obj)->type)
#define CLASSPROPS(obj)    ((obj)->u.clas.properties)
#define CLASSSEAL          0x01
#define SEALEDP(obj)       (CLASSP (obj) && (CLASSPROPS (obj) & CLASSSEAL))
#define CLASSINSTANTIABLE  0x02
#define INSTANTIABLE(obj)  (CLASSP (obj) && (CLASSPROPS (obj) & CLASSINSTANTIABLE))
#define CLASSSLOTSUNINIT   0x04
#define CLASSUNINITIALIZED(obj)  (CLASSP (obj) && (CLASSPROPS (obj) & CLASSSLOTSUNINIT))
#define CLASSENV(obj)      ((obj)->u.clas.creation_env)
#define CLASSINDEX(obj)    ((obj)->u.clas.ordinal_index)

struct instance {
    Object class;
    Object *slots;
};

#define INSTCLASS(obj)    ((obj)->u.instance.class)
#define INSTSLOTS(obj)    ((obj)->u.instance.slots)
#define INSTANCEP(obj)    ((obj)->type == Instance)
#define INSTTYPE(obj)     ((obj)->type)

struct singleton {
    Object val;
};

#define SINGLEVAL(obj)    ((obj)->u.singleton.val)
#define SINGLETONP(obj)   ((obj)->type == Singleton)
#define SINGLETYPE(obj)   ((obj)->type)

struct limited_int_type {
    unsigned char properties;
    int min, max;
};

#define LIMINTPROPS(obj)  ((obj)->u.limited_int_type.properties)
#define LIMMINMASK 0x01
#define LIMINTHASMIN(obj) (LIMINTPROPS (obj) & LIMMINMASK)
#define LIMMAXMASK 0x02
#define LIMINTHASMAX(obj) (LIMINTPROPS (obj) & LIMMAXMASK)
#define LIMINTMIN(obj)    ((obj)->u.limited_int_type.min)
#define LIMINTMAX(obj)    ((obj)->u.limited_int_type.max)
#define LIMINTP(obj)      ((obj)->type == LimitedIntType)
#define LIMINTTYPE(obj)   ((obj)->type)

struct union_type {
    Object list;
};

#define UNIONP(obj)       ((obj)->type == UnionType)
#define UNIONTYPE(obj)    ((obj)->type)
#define UNIONLIST(obj)    ((obj)->u.union_type.list)

enum primtype {
    /* prim_n: n required  */
    /* prim_n_m: n requied, m optional */
    /* prim_n_rest: n required, rest args */
    prim_0, prim_1, prim_2, prim_3,
    prim_0_1, prim_0_2, prim_0_3,
    prim_1_1, prim_1_2, prim_2_1,
    prim_0_rest, prim_1_rest, prim_2_rest
};

struct primitive {
    char *name;
    enum primtype prim_type;
    Object (*fun) ();
};

#define PRIMNAME(obj)     ((obj)->u.primitive.name)
#define PRIMPTYPE(obj)    ((obj)->u.primitive.prim_type)
#define PRIMFUN(obj)      ((obj)->u.primitive.fun)
#define PRIMP(obj)        ((obj)->type == Primitive)
#define PRIMTYPE(obj)     ((obj)->type)

struct generic_function {
    Object name;
    unsigned char properties;
    Object required_params;
    Object key_params;
    Object rest_param;
    Object required_return_types;
    Object rest_return_type;
    Object methods;
    Object method_cache;
    Object active_next_methods;
};

#define GFNAME(obj)       ((obj)->u.generic_function.name)
#define GFPROPS(obj)      ((obj)->u.generic_function.properties)
#define GFALLKEYSMASK     0x01
#define GFKEYSMASK        0x02
#define GFHASKEYS(obj)    (GFPROPS(obj) & GFKEYSMASK)
#define GFALLKEYS(obj)    (GFPROPS(obj) & GFALLKEYSMASK)
#define GFREQPARAMS(obj)  ((obj)->u.generic_function.required_params)
#define GFKEYPARAMS(obj)  ((obj)->u.generic_function.key_params)
#define GFRESTPARAM(obj)  ((obj)->u.generic_function.rest_param)
#define GFREQVALUES(obj)  ((obj)->u.generic_function.required_return_types)
#define GFRESTVALUES(obj) ((obj)->u.generic_function.rest_return_type)
#define GFMETHODS(obj)    ((obj)->u.generic_function.methods)
#define GFCACHE(obj)      ((obj)->u.generic_function.method_cache)
#define GFACTIVENM(obj)   ((obj)->u.generic_function.active_next_methods)
#define GFUNP(obj)        ((obj)->type == GenericFunction)
#define GFTYPE(obj)       ((obj)->type)

struct method {
    Object name;
    unsigned char properties;
    Object required_params;
    Object next_method;
    Object key_params;
    Object rest_param;
    Object required_return_types;
    Object rest_return_type;
    Object my_handle;
    Object body;
    struct frame *env;
};

#define METHNAME(obj)       ((obj)->u.method.name)
#define METHPROPS(obj)      ((obj)->u.method.properties)
#define METHALLKEYSMASK     0x01
#define METHALLKEYS(obj)    (METHPROPS(obj) & METHALLKEYSMASK)
#define METHREQPARAMS(obj)  ((obj)->u.method.required_params)
#define METHNEXTMETH(obj)   ((obj)->u.method.next_method)
#define METHKEYPARAMS(obj)  ((obj)->u.method.key_params)
#define METHRESTPARAM(obj)  ((obj)->u.method.rest_param)
#define METHREQVALUES(obj)  ((obj)->u.method.required_return_types)
#define METHRESTVALUES(obj) ((obj)->u.method.rest_return_type)
#define METHBODY(obj)       ((obj)->u.method.body)
#define METHENV(obj)        ((obj)->u.method.env)
#define METHHANDLE(obj)     ((obj)->u.method.my_handle)
#define METHODP(obj)        ((obj)->type == Method)
#define METHTYPE(obj)       ((obj)->type)

struct next_method {
    Object generic_function;
    Object next_method_;
    Object rest_methods;
    Object args;
};

#define NMGF(obj)         ((obj)->u.next_method.generic_function)
#define NMMETH(obj)       ((obj)->u.next_method.next_method_)
#define NMREST(obj)       ((obj)->u.next_method.rest_methods)
#define NMARGS(obj)       ((obj)->u.next_method.args)
#define NMETHP(obj)       ((obj)->type == NextMethod)
#define NMTYPE(obj)       ((obj)->type)

#define EOFP(obj)         ((obj)->type == EndOfFile)

struct values {
    int num;
    Object *els;
};

#define VALUESNUM(obj)    ((obj)->u.values.num)
#define VALUESELS(obj)    ((obj)->u.values.els)
#define VALUESP(obj)      ((obj)->type == Values)
#define VALUESTYPE(obj)   ((obj)->type)

#define UNSPECIFIEDP(obj) ((obj)->type == Unspecified)

struct exitproc {
    Object sym;
    jmp_buf *ret;
    struct binding *exit_binding;
};

#define EXITSYM(obj)      ((obj)->u.exitproc.sym)
#define EXITRET(obj)      ((obj)->u.exitproc.ret)
#define EXITP(obj)        ((obj)->type == Exit)
#define EXITBINDING(obj)  ((obj)->u.exitproc.exit_binding)
#define EXITTYPE(obj)     ((obj)->type)

struct unwind {
    Object body;
};

#define UNWINDBODY(obj)   ((obj)->u.unwind.body)
#define UNWINDP(obj)      ((obj)->type == Unwind)
#define UNWINDTYPE(obj)   ((obj)->type)

#ifdef NO_COMMON_DYLAN_SPEC
enum streamtype {
    Input, Output
};
struct stream {
    enum streamtype stream_type;
    FILE *fp;
};

#define STREAMSTYPE(obj)   ((obj)->u.stream.stream_type)
#define STREAMFP(obj)      ((obj)->u.stream.fp)
#define STREAMP(obj)       ((obj)->type == Stream)
#define STREAMTYPE(obj)    ((obj)->type)
#define INPUTSTREAMP(obj)  (STREAMP(obj) && (STREAMSTYPE(obj) == Input))
#define OUTPUTSTREAMP(obj) (STREAMP(obj) && (STREAMSTYPE(obj) == Output))
#endif

#define UNINITSLOTP(obj)   ((obj)->type == UninitializedSlotValue)

/* <pcb> a wrapper around a system pointer. */

struct foreign_ptr {
    void *ptr;
};

#define FOREIGNPTR(obj)      ((obj)->u.foreign_ptr.ptr)
#define FOREIGNP(obj)        (POINTERP(obj) && (POINTERTYPE(obj) == ForeignPtr))
#define FOREIGNTYPE(obj)     ((obj)->type)

struct environment {
    struct frame *env;
};

#define ENVIRONMENT(obj)      ((obj)->u.environment.env)
#define ENVIRONMENTP(obj)        (POINTERP(obj) && (POINTERTYPE(obj) == Environment))
#define ENVIRONMENTTYPE(obj)     ((obj)->type)

struct object_handle {
    Object the_object;
};

#define HDLOBJ(obj)      ((obj)->u.object_handle.the_object)
#define HDLP(obj)        (POINTERP(obj) && (POINTERTYPE(obj) == ObjectHandle))
#define HDLTYPE(obj)     ((obj)->type)

struct object {
    enum objtype type;
    union {
	struct integer integer;
	struct big_integer big_integer;		/* <pcb> */
	struct ratio ratio;
	struct single_float single_float;
	struct double_float double_float;
	struct pair pair;
	struct byte_string byte_string;
	struct simple_object_vector simple_object_vector;
	struct table table;
	struct deque deque;
	struct array array;
	struct condition condition;
	struct symbol symbol;
	struct character character;
	struct clas clas;
	struct instance instance;
	struct singleton singleton;
	struct limited_int_type limited_int_type;
	struct union_type union_type;
	struct slot_descriptor slot_descriptor;
	struct primitive primitive;
	struct generic_function generic_function;
	struct method method;
	struct next_method next_method;
	struct values values;
	struct exitproc exitproc;
	struct unwind unwind;
	struct environment environment;
#ifdef NO_COMMON_DYLAN_SPEC
	struct stream stream;
#endif
	struct table_entry table_entry;
	struct deque_entry deque_entry;
	struct foreign_ptr foreign_ptr;
	struct object_handle object_handle;
    } u;
};

#define TYPE(obj)        ((obj)->type)
#define POINTERTYPE(obj) ((obj)->type)
#define LISTP(obj)       (NULLP(obj) || PAIRP(obj))
#define POINTERP(obj)    (1)

#else /* SMALL_OBJECTS */

/*
   Data Representation:

   pointer:   PPPPPPPPPPPPPPPPPPPPPPPPPPPPPP00  (P=pointer address bit)
   immed:     DDDDDDDDDDDDDDDDDDDDDDDDDDSSSS01  (D=immediate data, S=secondary tag)
   integer:   IIIIIIIIIIIIIIIIIIIIIIIIIIIIII10  (I=immediate integer data)

 */

typedef void *Object;

#define POINTERTAG           0
#define IMMEDTAG             1
#define INTEGERTAG           2

#define POINTERP(obj)        (((unsigned)obj & 3) == POINTERTAG)
#define IMMEDP(obj)          (((unsigned)obj & 3) == IMMEDTAG)
#define INTEGERP(obj)        (((unsigned)obj & 3) == INTEGERTAG)

/* macros for identifying immediates other than integers
 */
#define SUBPART(obj)         (((unsigned)obj & 0x3c) >> 2)

#define TRUESUB    0x00
#define FALSESUB   0x01
#define EMPTYSUB   0x02
#define CHARSUB    0x03
#define EOFSUB     0x04
#define UNSPECSUB  0x05
#define UNINITSUB  0x06
/* add more immediate types here */
/* WARNING!! These macros cause obj to be evaluated more than once */

#define TRUEP(obj)          (IMMEDP(obj) && (SUBPART(obj) == TRUESUB))
#define FALSEP(obj)         (IMMEDP(obj) && (SUBPART(obj) == FALSESUB))
#define EMPTYLISTP(obj)     (IMMEDP(obj) && (SUBPART(obj) == EMPTYSUB))
#define CHARP(obj)          (IMMEDP(obj) && (SUBPART(obj) == CHARSUB))
#define EOFP(obj)           (IMMEDP(obj) && (SUBPART(obj) == EOFSUB))
#define UNSPECP(obj)        (IMMEDP(obj) && (SUBPART(obj) == UNSPECSUB))
#define UNINITSLOTP(obj)    (IMMEDP(obj) && (SUBPART(obj) == UNINITSUB))

/* actual values of constant immediates
 */
#define TRUEVAL         ((Object)((TRUESUB << 2)   | IMMEDTAG))
#define FALSEVAL        ((Object)((FALSESUB << 2)  | IMMEDTAG))
#define EMPTYLISTVAL    ((Object)((EMPTYSUB << 2)  | IMMEDTAG))
#define EOFVAL          ((Object)((EOFSUB << 2)    | IMMEDTAG))
#define UNSPECVAL       ((Object)((UNSPECSUB << 2) | IMMEDTAG))
#define UNINITVAL       ((Object)((UNINITSUB << 2) | IMMEDTAG))

/* macros for extracting relevant parts of object
 */
#define POINTER_PART(obj)    (obj)
#define IMMED_PART(obj)      ((unsigned)obj >> 6)
#define INTEGER_PART(obj)    ((int)obj >> 2)
#define INTVAL(obj)          ((int)obj >> 2)
#define CHARVAL(obj)         ((unsigned)obj >> 6)

/* macros for synthesizing immediates
 */
#define MAKE_CHAR(ch)        ((Object)(((unsigned)ch << 6) | (CHARSUB << 2) | IMMEDTAG))
#define MAKE_INT(i)          ((Object)((i << 2) | INTEGERTAG))

/* BigInteger support. <pcb> */

struct big_integer {
    enum objtype type;
    void *val;
};

#define BIGINTVAL(obj)    (((struct big_integer *)obj)->val)
#define BIGINTP(obj)      (((struct big_integer *)obj)->type == BigInteger)
#define BIGINTTYPE(obj)   (((struct big_integer *)obj)->type)

struct ratio {
    enum objtype type;
    int numerator, denominator;
};

#define RATIOTYPE(obj)    (((struct ratio *)obj)->type)
#define RATIONUM(obj)     (((struct ratio *)obj)->numerator)
#define RATIODEN(obj)     (((struct ratio *)obj)->denominator)
#define RATIOP(obj)       (POINTERP(obj) && (RATIOTYPE(obj) == Ratio))

struct double_float {
    enum objtype type;
    double val;
};

#define DFLOATTYPE(obj)   (((struct double_float *)obj)->type)
#define DFLOATVAL(obj)    (((struct double_float *)obj)->val)
#define DFLOATP(obj)      (POINTERP(obj) && (DFLOATTYPE(obj) == DoubleFloat))

struct pair {
    enum objtype type;
    Object car, cdr;
};

#define PAIRTYPE(obj)     (((struct pair *)obj)->type)
#define CAR(obj)          (((struct pair *)obj)->car)
#define CDR(obj)          (((struct pair *)obj)->cdr)
#define PAIRP(obj)        (POINTERP(obj) && (PAIRTYPE(obj) == Pair))

struct byte_string {
    enum objtype type;
    int size;
    char *val;
};

#define BYTESTRTYPE(obj)  (((struct byte_string *)obj)->type)
#define BYTESTRSIZE(obj)  (((struct byte_string *)obj)->size)
#define BYTESTRVAL(obj)   (((struct byte_string *)obj)->val)
#define BYTESTRP(obj)     (POINTERP(obj) && (BYTESTRTYPE(obj) == ByteString))

struct simple_object_vector {
    enum objtype type;
    int size;
    Object *els;
};

#define SOVTYPE(obj)      (((struct simple_object_vector *)obj)->type)
#define SOVSIZE(obj)      (((struct simple_object_vector *)obj)->size)
#define SOVELS(obj)       (((struct simple_object_vector *)obj)->els)
#define SOVP(obj)         (POINTERP(obj) && (SOVTYPE(obj) == SimpleObjectVector))

struct table_entry {
    enum objtype type;
    int row;
    Object key;
    Object value;
    Object next;
};

#define TETYPE(obj)       (((struct table_entry *)obj)->type)
#define TEROW(obj)        (((struct table_entry *)obj)->row)
#define TEKEY(obj)        (((struct table_entry *)obj)->key)
#define TEVALUE(obj)      (((struct table_entry *)obj)->value)
#define TENEXT(obj)       (((struct table_entry *)obj)->next)
#define TEP(obj)          (POINTERP(obj) && (TETYPE(obj) == TableEntry))

struct table {
    enum objtype type;
    int size;
    Object *the_table;
};

#define TABLETYPE(obj)    (((struct table *)obj)->type)
#define TABLESIZE(obj)    (((struct table *)obj)->size)
#define TABLETABLE(obj)   (((struct table *)obj)->the_table)
#define TABLEP(obj)       (POINTERP(obj) && (TABLETYPE(obj) == ObjectTable))

struct deque_entry {
    enum objtype type;
    Object value;
    Object prev, next;
};

#define DETYPE(obj)       (((struct deque_entry *)obj)->type)
#define DEVALUE(obj)      (((struct deque_entry *)obj)->value)
#define DEPREV(obj)       (((struct deque_entry *)obj)->prev)
#define DENEXT(obj)       (((struct deque_entry *)obj)->next)
#define DEP(obj)          (POINTERP(obj) && (DETYPE(obj) == Deque))

struct deque {
    enum objtype type;
    Object first, last;
};

#define DEQUETYPE(obj)    (((struct deque *)obj)->type)
#define DEQUEFIRST(obj)   (((struct deque *)obj)->first)
#define DEQUELAST(obj)    (((struct deque *)obj)->last)
#define DEQUEP(obj)       (POINTERP(obj) && (DEQUETYPE(obj) == Deque))

struct array {
    enum objtype type;
    int size;
    Object dimensions;
    Object *elements;
};

#define ARRTYPE(obj)      (((struct array *)obj)->type)
#define ARRSIZE(obj)      (((struct array *)obj)->size)
#define ARRDIMS(obj)      (((struct array *)obj)->dimensions)
#define ARRELS(obj)       (((struct array *)obj)->elements)
#define ARRAYP(obj)       (POINTERP(obj) && (ARRTYPE(obj) == Array))

enum condtype {
    SimpleError, TypeError, SimpleWarning,
    SimpleRestart, Abort
};
struct condition {
    enum objtype type;
    enum condtype condtype;
};

#define CONDTYPE(obj)     (((struct condition *)obj)->type)
#define CONDCTYPE(obj)    (((struct condition *)obj)->condtype)
#define CONDP(obj)        (POINTERP(obj) && (CONDTYPE(obj) == Condition))

struct symbol {
    enum objtype type;
    char *name;
};

#define SYMBOLTYPE(obj)   (((struct symbol *)obj)->type)
#define SYMBOLNAME(obj)   (((struct symbol *)obj)->name)
#define SYMBOLP(obj)      (POINTERP(obj) && (SYMBOLTYPE(obj) == Symbol))
#define KEYNAME(obj)      (((struct symbol *)obj)->name)
#define KEYWORDP(obj)     (POINTERP(obj) && (SYMBOLTYPE(obj) == Keyword))

struct slot_descriptor {
    enum objtype type;
    unsigned char properties;
    Object getter_name;
    Object setter_name;
    Object slot_type;
    Object init;
    Object init_keyword;
    Object allocation;
    Object dynamism;
};

#define SLOTDPROPS(obj)        (((struct slot_descriptor *)obj)->properties)
#define SLOTDKEYREQMASK        0x01
#define SLOTDINHERITEDMASK     0x02
#define SLOTDINITFUNCTIONMASK  0x04
#define SLOTDDEFERREDTYPEMASK  0x08
#define SLOTDKEYREQ(obj)       (SLOTDPROPS (obj) & SLOTDKEYREQMASK)
#define SLOTDINHERITED(obj)    (SLOTDPROPS (obj) & SLOTDINHERITEDMASK)
#define SLOTDINITFUNCTION(obj) (SLOTDPROPS (obj) & SLOTDINITFUNCTIONMASK)
#define SLOTDDEFERREDTYPE(obj) (SLOTDPROPS (obj) & SLOTDDEFERREDTYPEMASK)
#define SLOTDGETTER(obj)       (((struct slot_descriptor *)obj)->getter_name)
#define SLOTDSETTER(obj)       (((struct slot_descriptor *)obj)->setter_name)
#define SLOTDSLOTTYPE(obj)     (((struct slot_descriptor *)obj)->slot_type)
#define SLOTDINIT(obj)         (((struct slot_descriptor *)obj)->init)
#define SLOTDINITKEYWORD(obj)  (((struct slot_descriptor *)obj)->init_keyword)
#define SLOTDALLOCATION(obj)   (((struct slot_descriptor *)obj)->allocation)
#define SLOTDDYNAMISM(obj)     (((struct slot_descriptor *)obj)->dynamism)
#define SLOTDP(obj)            (POINTERP(obj) && (CLASSTYPE(obj) == SlotDescriptor))
#define SLOTDTYPE(obj)         (((struct slot_descriptor *)obj)->type)

struct clas {
    enum objtype type;
    Object name;
    Object supers;
    Object subs;
    Object inherited_slot_descriptors;
    Object inst_slot_descriptors;
    Object class_slot_descriptors;
    Object class_slots;
    Object eachsubclass_slot_descriptors;
    Object eachsubclass_slots;
    Object constant_slot_descriptors;
    Object virtual_slot_descriptors;
    Object precedence_list;
    Object sorted_prec_list;
    int num_precs;
    int ordinal_index;
    int properties;
    struct frame *creation_env;
};

#define CLASSTYPE(obj)    (((struct clas *)obj)->type)
#define CLASSNAME(obj)    (((struct clas *)obj)->name)
#define CLASSSUPERS(obj)  (((struct clas *)obj)->supers)
#define CLASSSUBS(obj)    (((struct clas *)obj)->subs)
#define CLASSINSLOTDS(obj) (((struct clas *)obj)->inherited_slot_descriptors)
#define CLASSSLOTDS(obj)  (((struct clas *)obj)->inst_slot_descriptors)
#define CLASSCSLOTS(obj)  (((struct clas *)obj)->class_slots)
#define CLASSCSLOTDS(obj) (((struct clas *)obj)->class_slot_descriptors)
#define CLASSESSLOTDS(obj) (((struct clas *)obj)->eachsubclass_slot_descriptors)
#define CLASSESSLOTS(obj) (((struct clas *)obj)->eachsubclass_slots)
#define CLASSCONSTSLOTDS(obj) (((struct clas *)obj)->constant_slot_descriptors)
#define CLASSVSLOTDS(obj) (((struct clas *)obj)->virtual_slot_descriptors)
#define CLASSPRECLIST(obj)(((struct clas *)obj)->precedence_list)
#define CLASSSORTEDPRECS(obj) (((struct clas *)obj)->sorted_prec_list)
#define CLASSNUMPRECS(obj) (((struct clas *)obj)->num_precs)
#define CLASSP(obj)       (POINTERP(obj) && (CLASSTYPE(obj) == Class))
#define CLASSPROPS(obj)   (((struct clas *)obj)->properties)
#define CLASSSEAL         0x01
#define SEALEDP(obj)      (CLASSP (obj) && (CLASSPROPS (obj) & CLASSSEAL))
#define CLASSINSTANTIABLE 0x02
#define INSTANTIABLE(obj) (CLASSP (obj) && (CLASSPROPS (obj) & CLASSINSTANTIABLE))
#define CLASSSLOTSUNINIT   0x04
#define CLASSUNINITIALIZED(obj)  (CLASSP (obj) && (CLASSPROPS (obj) & CLASSSLOTSUNINIT))
#define CLASSENV(obj)     (((struct clas *)obj)->creation_env)
#define CLASSINDEX(obj)     (((struct clas *)obj)->ordinal_index)

struct instance {
    enum objtype type;
    Object class;
    Object *slots;
};

#define INSTTYPE(obj)     (((struct instance *)obj)->type)
#define INSTCLASS(obj)    (((struct instance *)obj)->class)
#define INSTSLOTS(obj)    (((struct instance *)obj)->slots)
#define INSTANCEP(obj)    (POINTERP(obj) && (INSTTYPE(obj) == Instance))

struct singleton {
    enum objtype type;
    Object val;
};

#define SINGLETYPE(obj)   (((struct singleton *)obj)->type)
#define SINGLEVAL(obj)    (((struct singleton *)obj)->val)
#define SINGLETONP(obj)   (POINTERP(obj) && (SINGLETYPE(obj) == Singleton))


struct limited_int_type {
    enum objtype type;
    unsigned char properties;
    int min, max;
};

#define LIMINTPROPS(obj)  (((struct limited_int_type *)obj)->properties)
#define LIMMINMASK 0x01
#define LIMINTHASMIN(obj) (LIMINTPROPS (obj) & LIMMINMASK)
#define LIMMAXMASK 0x02
#define LIMINTHASMAX(obj) (LIMINTPROPS (obj) & LIMMAXMASK)
#define LIMINTMIN(obj)    (((struct limited_int_type *)obj)->min)
#define LIMINTMAX(obj)    (((struct limited_int_type *)obj)->max)
#define LIMINTP(obj)      (POINTERP(obj) && (LIMINTTYPE(obj) == LimitedIntType))
#define LIMINTTYPE(obj)   (((struct limited_int_type *)obj)->type)

struct union_type {
    enum objtype type;
    Object list;
};

#define UNIONP(obj)       (POINTERP(obj) && (UNIONTYPE(obj) == UnionType))
#define UNIONTYPE(obj)    (((struct union_type *)obj)->type)
#define UNIONLIST(obj)    (((struct union_type *)obj)->list)
enum primtype {
    /* prim_n: n required  */
    /* prim_n_m: n requied, m optional */
    /* prim_n_rest: n required, rest args */
    prim_0, prim_1, prim_2, prim_3,
    prim_0_1, prim_0_2, prim_0_3,
    prim_1_1, prim_1_2, prim_2_1,
    prim_0_rest, prim_1_rest, prim_2_rest
};

struct primitive {
    char *name;
    enum primtype prim_type;
    Object (*fun) ();
};

struct prim {
    enum objtype type;
    struct primitive p;
};

#define PRIMTYPE(obj)     (((struct prim *)obj)->type)
#define PRIMNAME(obj)     (((struct prim *)obj)->p.name)
#define PRIMPTYPE(obj)    (((struct prim *)obj)->p.prim_type)
#define PRIMFUN(obj)      (((struct prim *)obj)->p.fun)
#define PRIMP(obj)        (POINTERP(obj) && (PRIMTYPE(obj) == Primitive))

struct generic_function {
    enum objtype type;
    Object name;
    unsigned char properties;
    Object required_params;
    Object key_params;
    Object rest_param;
    Object required_return_types;
    Object rest_return_type;
    Object methods;
    Object method_cache;
    Object active_next_methods;
};

#define GFTYPE(obj)       (((struct generic_function *)obj)->type)
#define GFNAME(obj)       (((struct generic_function *)obj)->name)
#define GFNAME(obj)       (((struct generic_function *)obj)->name)
#define GFPROPS(obj)      (((struct generic_function *)obj)->properties)
#define GFALLKEYSMASK     0x01
#define GFKEYSMASK        0x02
#define GFHASKEYS(obj)    (GFPROPS(obj) & GFKEYSMASK)
#define GFALLKEYS(obj)    (GFPROPS(obj) & GFALLKEYSMASK)
#define GFREQPARAMS(obj)  (((struct generic_function *)obj)->required_params)
#define GFKEYPARAMS(obj)  (((struct generic_function *)obj)->key_params)
#define GFRESTPARAM(obj)  (((struct generic_function *)obj)->rest_param)
#define GFREQVALUES(obj)  (((struct generic_function *)obj)->required_return_types)
#define GFRESTVALUES(obj) (((struct generic_function *)obj)->rest_return_type)
#define GFMETHODS(obj)    (((struct generic_function *)obj)->methods)
#define GFCACHE(obj)      (((struct generic_function *)obj)->method_cache)
#define GFACTIVENM(obj)   (((struct generic_function *)obj)->active_next_methods)
#define GFUNP(obj)        (POINTERP(obj) && (GFTYPE(obj) == GenericFunction))

struct method {
    enum objtype type;
    Object name;
    unsigned char properties;
    Object required_params;
    Object next_method;
    Object key_params;
    Object rest_param;
    Object required_return_types;
    Object rest_return_type;
    Object body;
    Object my_handle;
    struct frame *env;
};

#define METHTYPE(obj)       (((struct method *)obj)->type)
#define METHNAME(obj)       (((struct method *)obj)->name)
#define METHNEXTMETH(obj)   (((struct method *)obj)->next_method)
#define METHPROPS(obj)      (((struct method *)obj)->properties)
#define METHALLKEYSMASK     0x01
#define METHALLKEYS(obj)    (METHPROPS(obj) & METHALLKEYSMASK)
#define METHREQPARAMS(obj)  (((struct method *)obj)->required_params)
#define METHKEYPARAMS(obj)  (((struct method *)obj)->key_params)
#define METHRESTPARAM(obj)  (((struct method *)obj)->rest_param)
#define METHREQVALUES(obj)  (((struct method *)obj)->required_return_types)
#define METHRESTVALUES(obj) (((struct method *)obj)->rest_return_type)
#define METHBODY(obj)       (((struct method *)obj)->body)
#define METHENV(obj)        (((struct method *)obj)->env)
#define METHHANDLE(obj)     (((struct method *)obj)->my_handle)
#define METHODP(obj)        (POINTERP(obj) && (METHTYPE(obj) == Method))

struct next_method {
    enum objtype type;
    Object generic_function;
    Object next_method_;
    Object rest_methods;
    Object args;
};

#define NMTYPE(obj)       (((struct next_method *)obj)->type)
#define NMGF(obj)         (((struct next_method *)obj)->generic_function)
#define NMMETH(obj)       (((struct next_method *)obj)->next_method_)
#define NMREST(obj)       (((struct next_method *)obj)->rest_methods)
#define NMARGS(obj)       (((struct next_method *)obj)->args)
#define NMETHP(obj)       (POINTERP(obj) && (NMTYPE(obj) == NextMethod))

struct values {
    enum objtype type;
    int num;
    Object *els;
};

#define VALUESTYPE(obj)   (((struct values *)obj)->type)
#define VALUESNUM(obj)    (((struct values *)obj)->num)
#define VALUESELS(obj)    (((struct values *)obj)->els)
#define VALUESP(obj)      (POINTERP(obj) && (VALUESTYPE(obj) == Values))

#define UNSPECIFIEDP(obj) ((obj)->type == Unspecified)

struct exitproc {
    enum objtype type;
    Object sym;
    jmp_buf *ret;
    struct binding *exit_binding;
};

#define EXITTYPE(obj)     (((struct exitproc *)obj)->type)
#define EXITSYM(obj)      (((struct exitproc *)obj)->sym)
#define EXITRET(obj)      (((struct exitproc *)obj)->ret)
#define EXITBINDING(obj)  (((struct exitproc *)obj)->exit_binding)
#define EXITP(obj)        (POINTERP(obj) && (EXITTYPE(obj) == Exit))

struct unwind {
    enum objtype type;
    Object body;
};

#define UNWINDTYPE(obj)   (((struct unwind *)obj)->type)
#define UNWINDBODY(obj)   (((struct unwind *)obj)->body)
#define UNWINDP(obj)      (POINTERP(obj) && (UNWINDTYPE(obj) == Unwind)

#ifdef NO_COMMON_DYLAN_SPEC
enum streamtype {
    Input, Output
};
struct stream {
    enum objtype type;
    enum streamtype stream_type;
    FILE *fp;
};

#define STREAMTYPE(obj)    (((struct stream *)obj)->type)
#define STREAMSTYPE(obj)   (((struct stream *)obj)->stream_type)
#define STREAMFP(obj)      (((struct stream *)obj)->fp)
#define STREAMP(obj)       (POINTERP(obj) && (STREAMTYPE(obj) == Stream))
#define INPUTSTREAMP(obj)  (STREAMP(obj) && (STREAMSTYPE(obj) == Input))
#define OUTPUTSTREAMP(obj) (STREAMP(obj) && (STREAMSTYPE(obj) == Output))
#endif

#define TYPE(obj)        (object_type (obj))
#define POINTERTYPE(obj) (PAIRTYPE(obj))
#define NULLP(obj)       (EMPTYLISTP(obj))
#define LISTP(obj)       (NULLP(obj) || PAIRP(obj))

/* <pcb> a wrapper around a system poiter. */

struct foreign_ptr {
    enum objtype type;
    void *ptr;
};

#define FOREIGNPTR(obj)      (((struct foreign_ptr *)obj)->foreign_ptr.ptr)
#define FOREIGNP(obj)        (POINTERP(obj) && (POINTERTYPE(obj) == ForeignPtr))
#define FOREIGNTYPE(obj)     (((struct foreign_ptr *)obj)->foreign_ptr.type)

struct environment {
    enum objtype type;
    struct frame *env;
};

#define ENVIRONMENT(obj)      (((struct environment *)obj)->env)
#define ENVIRONMENTP(obj)        (POINTERP(obj) && (POINTERTYPE(obj) == Environment))
#define ENVIRONMENTTYPE(obj)     (((struct environment *)obj)->type)

struct object_handle {
    enum objtype type;
    Object the_object;
};

#define HDLOBJ(obj)      (((struct object_handle *)obj)->the_object)
#define HDLTYPE(obj)     (((struct object_handle *)obj)->type)
#define HDLP(obj)        (POINTERP(obj) && (HDLTYPE(obj) == ObjectHandle))

#endif /* not SMALL_OBJECTS */

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

enum objtype object_type (Object obj);

#endif
