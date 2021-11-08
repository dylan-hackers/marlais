/*
   Data Representation:

   pointer:   PPPPPPPPPPPPPPPPPPPPPPPPPPPPPP00  (P=pointer address bit)
   immed:     DDDDDDDDDDDDDDDDDDDDDDDDDDSSSS01  (D=immediate data, S=secondary tag)
   integer:   IIIIIIIIIIIIIIIIIIIIIIIIIIIIII10  (I=immediate integer data)

 */

/* Type for marlais objects */
typedef void *Object;

/* Type for handling value tags */
typedef DyUnsigned MarlaisTag;
#define MARLAIS_TAG_POINTER       (0x0)
#define MARLAIS_TAG_IMMEDIATE     (0x1)
#define MARLAIS_TAG_INTEGER       (0x2)
#define MARLAIS_TAG_MASK          (0x3)

/* Type for handling value subtags */
typedef DyUnsigned MarlaisSub;
#define MARLAIS_SUB_SHIFT         2
#define MARLAIS_SUB_TRUE          (0x0 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_FALSE         (0x1 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_EMPTYLIST     (0x2 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_CHARACTER     (0x3 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_EOF           (0x4 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_UNSPECIFIED   (0x5 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_UNINITIALIZED (0x6 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_MASK          (0xf << MARLAIS_SUB_SHIFT)

/* Shift distances for value fields */
#define MARLAIS_INTEGER_SHIFT   (2)
#define MARLAIS_IMMEDIATE_SHIFT (6)

/* Immediate constants */
#define TRUEVAL         ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_TRUE))
#define FALSEVAL        ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_FALSE))
#define EMPTYLISTVAL    ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_EMPTYLIST))
#define EOFVAL          ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_EOF))
#define UNSPECVAL       ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_UNSPECIFIED))
#define UNINITVAL       ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_UNINITIALIZED))
#define MAX_SMALL_INT   (INTPTR_MAX >> MARLAIS_INTEGER_SHIFT)

/* Field extraction */
static inline DyUnsigned TAGPART(Object obj) {
  return (((DyUnsigned)obj) & MARLAIS_TAG_MASK);
}
static inline DyUnsigned SUBPART(Object obj) {
  return (((DyUnsigned)obj) & MARLAIS_SUB_MASK);
}
static inline DyUnsigned INTEGERPART(Object obj) {
  return (((DyUnsigned)obj) >> MARLAIS_INTEGER_SHIFT);
}
static inline DyUnsigned IMMEDPART(Object obj) {
  return (((DyUnsigned)obj) >> MARLAIS_IMMEDIATE_SHIFT);
}

/* Tag type predicates */
static inline bool POINTERP(Object obj) {
  return (TAGPART(obj) == MARLAIS_TAG_POINTER);
}
static inline bool IMMEDP(Object obj) {
  return (TAGPART(obj) == MARLAIS_TAG_IMMEDIATE);
}
static inline bool INTEGERP(Object obj) {
  return (TAGPART(obj) == MARLAIS_TAG_INTEGER);
}

/* Immediate extraction */
static inline DyInteger INTVAL(Object obj) {
  return ((DyInteger)INTEGERPART(obj));
}
static inline DyUnsigned CHARVAL(Object obj) {
  return ((DyUnsigned)IMMEDPART(obj));
}

/* Immediate composition */
static inline Object MAKE_IMMEDIATE(MarlaisSub sub, DyUnsigned val) {
  return (Object)(MARLAIS_TAG_IMMEDIATE|sub|(val << MARLAIS_IMMEDIATE_SHIFT));
}
static inline Object MAKE_CHAR(DyUnsigned ch) {
  return MAKE_IMMEDIATE(MARLAIS_SUB_CHARACTER, ch);
}
static inline Object MAKE_INT(DyInteger ch) {
  return (Object)(MARLAIS_TAG_INTEGER|(((DyUnsigned)ch) << MARLAIS_INTEGER_SHIFT));
}

/* Immediate type predicates */
#define DEFINE_IMMEDP_PREDICATE(_name, _sub)        \
  static inline bool _name(Object obj) {            \
    return (IMMEDP(obj) && (SUBPART(obj) == _sub)); \
  }
DEFINE_IMMEDP_PREDICATE(TRUEP, MARLAIS_SUB_TRUE);
DEFINE_IMMEDP_PREDICATE(FALSEP, MARLAIS_SUB_FALSE);
DEFINE_IMMEDP_PREDICATE(EMPTYLISTP, MARLAIS_SUB_EMPTYLIST);
DEFINE_IMMEDP_PREDICATE(CHARP, MARLAIS_SUB_CHARACTER);
DEFINE_IMMEDP_PREDICATE(EOFP, MARLAIS_SUB_EOF);
DEFINE_IMMEDP_PREDICATE(UNSPECP, MARLAIS_SUB_UNSPECIFIED);
DEFINE_IMMEDP_PREDICATE(UNINITSLOTP, MARLAIS_SUB_UNINITIALIZED);
#undef DEFINE_IMMEDP_PREDICATE

/* Pointer type predicates */
#define DEFINE_POINTERP_PREDICATE(_name, _type)                \
  static inline bool _name(Object obj) {                   \
    return (POINTERP(obj) && (POINTERTYPE(obj) == _type)); \
  }
DEFINE_POINTERP_PREDICATE(BIGINTP, BigInteger);
DEFINE_POINTERP_PREDICATE(SFLOATP, SingleFloat);
DEFINE_POINTERP_PREDICATE(DFLOATP, DoubleFloat);
DEFINE_POINTERP_PREDICATE(PAIRP, Pair);
DEFINE_POINTERP_PREDICATE(BYTESTRP, ByteString);
DEFINE_POINTERP_PREDICATE(SOVP, SimpleObjectVector);
DEFINE_POINTERP_PREDICATE(TEP, TableEntry);
DEFINE_POINTERP_PREDICATE(TABLEP, ObjectTable);
DEFINE_POINTERP_PREDICATE(DEP, DequeEntry);
DEFINE_POINTERP_PREDICATE(DEQUEP, Deque);
DEFINE_POINTERP_PREDICATE(ARRAYP, Array);
DEFINE_POINTERP_PREDICATE(CONDP, Condition);
DEFINE_POINTERP_PREDICATE(NAMEP, Name);
DEFINE_POINTERP_PREDICATE(SYMBOLP, Symbol);
DEFINE_POINTERP_PREDICATE(SLOTDP, SlotDescriptor);
DEFINE_POINTERP_PREDICATE(INSTANCEP, Instance);
DEFINE_POINTERP_PREDICATE(CLASSP, Class);
DEFINE_POINTERP_PREDICATE(SINGLETONP, Singleton);
DEFINE_POINTERP_PREDICATE(LIMINTP, LimitedIntType);
DEFINE_POINTERP_PREDICATE(UNIONP, UnionType);
DEFINE_POINTERP_PREDICATE(PRIMP, Primitive);
DEFINE_POINTERP_PREDICATE(GFUNP, GenericFunction);
DEFINE_POINTERP_PREDICATE(METHODP, Method);
DEFINE_POINTERP_PREDICATE(NMETHP, NextMethod);
DEFINE_POINTERP_PREDICATE(VALUESP, Values);
DEFINE_POINTERP_PREDICATE(EXITP, Exit);
DEFINE_POINTERP_PREDICATE(UNWINDP, Unwind);
#ifdef NO_COMMON_DYLAN_SPEC
DEFINE_POINTERP_PREDICATE(STREAMP, Stream);
#endif
DEFINE_POINTERP_PREDICATE(FOREIGNP, ForeignPtr);
DEFINE_POINTERP_PREDICATE(ENVIRONMENTP, Environment);
DEFINE_POINTERP_PREDICATE(HDLP, ObjectHandle);
#undef DEFINE_POINTERP_PREDICATE

/* List type predicates */
static inline bool NULLP(Object obj) {
  return EMPTYLISTP(obj);
}
static inline bool LISTP(Object obj) {
  return NULLP(obj)||PAIRP(obj);
}

struct big_integer {
    ObjectHeader header;
    void *val;
};

#define BIGINTVAL(obj)    (((struct big_integer *)obj)->val)

struct ratio {
    ObjectHeader header;
    DyInteger numerator, denominator;
};

#define RATIOTYPE(obj)    (((struct ratio *)obj)->type)
#define RATIONUM(obj)     (((struct ratio *)obj)->numerator)
#define RATIODEN(obj)     (((struct ratio *)obj)->denominator)

struct double_float {
    ObjectHeader header;
    double val;
};

#define DFLOATTYPE(obj)   (((struct double_float *)obj)->type)
#define DFLOATVAL(obj)    (((struct double_float *)obj)->val)

struct pair {
    ObjectHeader header;
    Object car, cdr;
};

#define CAR(obj)          (((struct pair *)obj)->car)
#define CDR(obj)          (((struct pair *)obj)->cdr)

struct byte_string {
    ObjectHeader header;
    int size;
    char *val;
};

#define BYTESTRSIZE(obj)  (((struct byte_string *)obj)->size)
#define BYTESTRVAL(obj)   (((struct byte_string *)obj)->val)

struct simple_object_vector {
    ObjectHeader header;
    int size;
    Object *els;
};

#define SOVTYPE(obj)      (((struct simple_object_vector *)obj)->type)
#define SOVSIZE(obj)      (((struct simple_object_vector *)obj)->size)
#define SOVELS(obj)       (((struct simple_object_vector *)obj)->els)

struct table_entry {
    ObjectHeader header;
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

struct table {
    ObjectHeader header;
    int size;
    Object *the_table;
};

#define TABLESIZE(obj)    (((struct table *)obj)->size)
#define TABLETABLE(obj)   (((struct table *)obj)->the_table)

struct deque_entry {
    ObjectHeader header;
    Object value;
    Object prev, next;
};

#define DEVALUE(obj)      (((struct deque_entry *)obj)->value)
#define DEPREV(obj)       (((struct deque_entry *)obj)->prev)
#define DENEXT(obj)       (((struct deque_entry *)obj)->next)

struct deque {
    ObjectHeader header;
    Object first, last;
};

#define DEQUEFIRST(obj)   (((struct deque *)obj)->first)
#define DEQUELAST(obj)    (((struct deque *)obj)->last)

struct array {
    ObjectHeader header;
    int size;
    Object dimensions;
    Object *elements;
};

#define ARRSIZE(obj)      (((struct array *)obj)->size)
#define ARRDIMS(obj)      (((struct array *)obj)->dimensions)
#define ARRELS(obj)       (((struct array *)obj)->elements)

enum condtype {
    SimpleError, TypeError, SimpleWarning,
    SimpleRestart, Abort
};

struct condition {
    ObjectHeader header;
    enum condtype condtype;
};

#define CONDCTYPE(obj)    (((struct condition *)obj)->condtype)

struct symbol {
    ObjectHeader header;
    char *name;
};

#define SYMBOLNAME(obj)   (((struct symbol *)obj)->name)
#define NAMENAME(obj)     (((struct symbol *)obj)->name)

struct slot_descriptor {
    ObjectHeader header;
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

struct clas {
    ObjectHeader header;
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
    ObjectHeader header;
    Object class;
    Object *slots;
};

#define INSTCLASS(obj)    (((struct instance *)obj)->class)
#define INSTSLOTS(obj)    (((struct instance *)obj)->slots)

struct singleton {
    ObjectHeader header;
    Object val;
};

#define SINGLEVAL(obj)    (((struct singleton *)obj)->val)

struct limited_int_type {
    ObjectHeader header;
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

struct union_type {
    ObjectHeader header;
    Object list;
};

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
    ObjectHeader header;
    struct primitive p;
};

#define PRIMNAME(obj)     (((struct prim *)obj)->p.name)
#define PRIMPTYPE(obj)    (((struct prim *)obj)->p.prim_type)
#define PRIMFUN(obj)      (((struct prim *)obj)->p.fun)

struct generic_function {
    ObjectHeader header;
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

struct method {
    ObjectHeader header;
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

struct next_method {
    ObjectHeader header;
    Object generic_function;
    Object next_method_;
    Object rest_methods;
    Object args;
};

#define NMGF(obj)         (((struct next_method *)obj)->generic_function)
#define NMMETH(obj)       (((struct next_method *)obj)->next_method_)
#define NMREST(obj)       (((struct next_method *)obj)->rest_methods)
#define NMARGS(obj)       (((struct next_method *)obj)->args)

struct values {
    ObjectHeader header;
    int num;
    Object *els;
};

#define VALUESNUM(obj)    (((struct values *)obj)->num)
#define VALUESELS(obj)    (((struct values *)obj)->els)

struct exitproc {
    ObjectHeader header;
    Object sym;
    jmp_buf *ret;
    struct binding *exit_binding;
};

#define EXITSYM(obj)      (((struct exitproc *)obj)->sym)
#define EXITRET(obj)      (((struct exitproc *)obj)->ret)
#define EXITBINDING(obj)  (((struct exitproc *)obj)->exit_binding)

struct unwind {
    ObjectHeader header;
    Object body;
};

#define UNWINDBODY(obj)   (((struct unwind *)obj)->body)

#ifdef NO_COMMON_DYLAN_SPEC
enum streamtype {
    Input, Output
};
struct stream {
    ObjectHeader header;
    enum streamtype stream_type;
    FILE *fp;
};

#define STREAMSTYPE(obj)   (((struct stream *)obj)->stream_type)
#define STREAMFP(obj)      (((struct stream *)obj)->fp)
#define INPUTSTREAMP(obj)  (STREAMP(obj) && (STREAMSTYPE(obj) == Input))
#define OUTPUTSTREAMP(obj) (STREAMP(obj) && (STREAMSTYPE(obj) == Output))
#endif

struct foreign_ptr {
    ObjectHeader header;
    void *ptr;
};

#define FOREIGNPTR(obj)      (((struct foreign_ptr *)obj)->ptr)

struct environment {
    ObjectHeader header;
    struct frame *env;
};

#define ENVIRONMENT(obj)      (((struct environment *)obj)->env)

struct object_handle {
    ObjectHeader header;
    Object the_object;
};

#define HDLOBJ(obj)      (((struct object_handle *)obj)->the_object)
