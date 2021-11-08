
typedef struct object *Object;

/* all objects are pointers */
#define POINTERP(obj)    (1)
/* and therefore have a type slot */
#define POINTERTYPE(obj) ((obj)->type)


#define TRUEP(obj)        ((obj)->type == True)
#define FALSEP(obj)       ((obj)->type == False)

struct integer {
    DyInteger val;
};

#define INTVAL(obj)       ((obj)->u.integer.val)
#define INTEGERP(obj)     ((obj)->type == Integer)
#define MAX_SMALL_INT     (INTPTR_MAX)

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

#define NULLP(obj)        ((obj)->type == EmptyList)
#define EMPTYLISTP(obj)   ((obj)->type == EmptyList)
#define LISTP(obj)        (EMPTYLISTP(obj) || PAIRP(obj))

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
#define NAMENAME(obj)     ((obj)->u.symbol.name)
#define NAMEP(obj)        ((obj)->type == Name)

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
    ObjectType type;
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
