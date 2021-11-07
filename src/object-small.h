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

#define POINTERP(obj)        (((DyUnsigned)obj & 3) == POINTERTAG)
#define IMMEDP(obj)          (((DyUnsigned)obj & 3) == IMMEDTAG)
#define INTEGERP(obj)        (((DyUnsigned)obj & 3) == INTEGERTAG)
#define MAX_SMALL_INT        (INTPTR_MAX >> 3)


/* macros for identifying immediates other than integers
 */
#define SUBPART(obj)         (((DyUnsigned)obj & 0x3c) >> 2)

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

#define NULLP(obj)       (EMPTYLISTP(obj))
#define LISTP(obj)       (NULLP(obj) || PAIRP(obj))

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
#define IMMED_PART(obj)      ((DyUnsigned)obj >> 6)
#define INTEGER_PART(obj)    ((DyInteger)obj >> 2)
#define INTVAL(obj)          ((DyInteger)obj >> 2)
#define CHARVAL(obj)         ((DyUnsigned)obj >> 6)

/* macros for synthesizing immediates
 */
#define MAKE_CHAR(ch)        ((Object)(((DyUnsigned)ch << 6) | (CHARSUB << 2) | IMMEDTAG))
#define MAKE_INT(i)          ((Object)((i << 2) | INTEGERTAG))

/* common structure of all heap objects */
struct object {
  ObjectType type;
};

#define POINTERTYPE(obj)     (((struct object *)obj)->type)

/* BigInteger support. <pcb> */

struct big_integer {
    ObjectType type;
    void *val;
};

#define BIGINTVAL(obj)    (((struct big_integer *)obj)->val)
#define BIGINTP(obj)      (((struct big_integer *)obj)->type == BigInteger)
#define BIGINTTYPE(obj)   (((struct big_integer *)obj)->type)

struct ratio {
    ObjectType type;
    int numerator, denominator;
};

#define RATIOTYPE(obj)    (((struct ratio *)obj)->type)
#define RATIONUM(obj)     (((struct ratio *)obj)->numerator)
#define RATIODEN(obj)     (((struct ratio *)obj)->denominator)
#define RATIOP(obj)       (POINTERP(obj) && (RATIOTYPE(obj) == Ratio))

struct double_float {
    ObjectType type;
    double val;
};

#define DFLOATTYPE(obj)   (((struct double_float *)obj)->type)
#define DFLOATVAL(obj)    (((struct double_float *)obj)->val)
#define DFLOATP(obj)      (POINTERP(obj) && (DFLOATTYPE(obj) == DoubleFloat))

struct pair {
    ObjectType type;
    Object car, cdr;
};

#define PAIRTYPE(obj)     (((struct pair *)obj)->type)
#define CAR(obj)          (((struct pair *)obj)->car)
#define CDR(obj)          (((struct pair *)obj)->cdr)
#define PAIRP(obj)        (POINTERP(obj) && (PAIRTYPE(obj) == Pair))

struct byte_string {
    ObjectType type;
    int size;
    char *val;
};

#define BYTESTRTYPE(obj)  (((struct byte_string *)obj)->type)
#define BYTESTRSIZE(obj)  (((struct byte_string *)obj)->size)
#define BYTESTRVAL(obj)   (((struct byte_string *)obj)->val)
#define BYTESTRP(obj)     (POINTERP(obj) && (BYTESTRTYPE(obj) == ByteString))

struct simple_object_vector {
    ObjectType type;
    int size;
    Object *els;
};

#define SOVTYPE(obj)      (((struct simple_object_vector *)obj)->type)
#define SOVSIZE(obj)      (((struct simple_object_vector *)obj)->size)
#define SOVELS(obj)       (((struct simple_object_vector *)obj)->els)
#define SOVP(obj)         (POINTERP(obj) && (SOVTYPE(obj) == SimpleObjectVector))

struct table_entry {
    ObjectType type;
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
    ObjectType type;
    int size;
    Object *the_table;
};

#define TABLETYPE(obj)    (((struct table *)obj)->type)
#define TABLESIZE(obj)    (((struct table *)obj)->size)
#define TABLETABLE(obj)   (((struct table *)obj)->the_table)
#define TABLEP(obj)       (POINTERP(obj) && (TABLETYPE(obj) == ObjectTable))

struct deque_entry {
    ObjectType type;
    Object value;
    Object prev, next;
};

#define DETYPE(obj)       (((struct deque_entry *)obj)->type)
#define DEVALUE(obj)      (((struct deque_entry *)obj)->value)
#define DEPREV(obj)       (((struct deque_entry *)obj)->prev)
#define DENEXT(obj)       (((struct deque_entry *)obj)->next)
#define DEP(obj)          (POINTERP(obj) && (DETYPE(obj) == Deque))

struct deque {
    ObjectType type;
    Object first, last;
};

#define DEQUETYPE(obj)    (((struct deque *)obj)->type)
#define DEQUEFIRST(obj)   (((struct deque *)obj)->first)
#define DEQUELAST(obj)    (((struct deque *)obj)->last)
#define DEQUEP(obj)       (POINTERP(obj) && (DEQUETYPE(obj) == Deque))

struct array {
    ObjectType type;
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
    ObjectType type;
    enum condtype condtype;
};

#define CONDTYPE(obj)     (((struct condition *)obj)->type)
#define CONDCTYPE(obj)    (((struct condition *)obj)->condtype)
#define CONDP(obj)        (POINTERP(obj) && (CONDTYPE(obj) == Condition))

struct symbol {
    ObjectType type;
    char *name;
};

#define SYMBOLTYPE(obj)   (((struct symbol *)obj)->type)
#define SYMBOLNAME(obj)   (((struct symbol *)obj)->name)
#define SYMBOLP(obj)      (POINTERP(obj) && (SYMBOLTYPE(obj) == Symbol))
#define KEYNAME(obj)      (((struct symbol *)obj)->name)
#define KEYWORDP(obj)     (POINTERP(obj) && (SYMBOLTYPE(obj) == Keyword))

struct slot_descriptor {
    ObjectType type;
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
    ObjectType type;
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
    ObjectType type;
    Object class;
    Object *slots;
};

#define INSTTYPE(obj)     (((struct instance *)obj)->type)
#define INSTCLASS(obj)    (((struct instance *)obj)->class)
#define INSTSLOTS(obj)    (((struct instance *)obj)->slots)
#define INSTANCEP(obj)    (POINTERP(obj) && (INSTTYPE(obj) == Instance))

struct singleton {
    ObjectType type;
    Object val;
};

#define SINGLETYPE(obj)   (((struct singleton *)obj)->type)
#define SINGLEVAL(obj)    (((struct singleton *)obj)->val)
#define SINGLETONP(obj)   (POINTERP(obj) && (SINGLETYPE(obj) == Singleton))


struct limited_int_type {
    ObjectType type;
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
    ObjectType type;
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
    ObjectType type;
    struct primitive p;
};

#define PRIMTYPE(obj)     (((struct prim *)obj)->type)
#define PRIMNAME(obj)     (((struct prim *)obj)->p.name)
#define PRIMPTYPE(obj)    (((struct prim *)obj)->p.prim_type)
#define PRIMFUN(obj)      (((struct prim *)obj)->p.fun)
#define PRIMP(obj)        (POINTERP(obj) && (PRIMTYPE(obj) == Primitive))

struct generic_function {
    ObjectType type;
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
    ObjectType type;
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
    ObjectType type;
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
    ObjectType type;
    int num;
    Object *els;
};

#define VALUESTYPE(obj)   (((struct values *)obj)->type)
#define VALUESNUM(obj)    (((struct values *)obj)->num)
#define VALUESELS(obj)    (((struct values *)obj)->els)
#define VALUESP(obj)      (POINTERP(obj) && (VALUESTYPE(obj) == Values))

#define UNSPECIFIEDP(obj) ((obj)->type == Unspecified)

struct exitproc {
    ObjectType type;
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
    ObjectType type;
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
    ObjectType type;
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

/* <pcb> a wrapper around a system poiter. */

struct foreign_ptr {
    ObjectType type;
    void *ptr;
};

#define FOREIGNPTR(obj)      (((struct foreign_ptr *)obj)->ptr)
#define FOREIGNP(obj)        (POINTERP(obj) && (POINTERTYPE(obj) == ForeignPtr))
#define FOREIGNTYPE(obj)     (((struct foreign_ptr *)obj)->type)

struct environment {
    ObjectType type;
    struct frame *env;
};

#define ENVIRONMENT(obj)      (((struct environment *)obj)->env)
#define ENVIRONMENTP(obj)        (POINTERP(obj) && (POINTERTYPE(obj) == Environment))
#define ENVIRONMENTTYPE(obj)     (((struct environment *)obj)->type)

struct object_handle {
    ObjectType type;
    Object the_object;
};

#define HDLOBJ(obj)      (((struct object_handle *)obj)->the_object)
#define HDLTYPE(obj)     (((struct object_handle *)obj)->type)
#define HDLP(obj)        (POINTERP(obj) && (HDLTYPE(obj) == ObjectHandle))
