
struct empty {
    ObjectHeader header;
};

#if defined(MARLAIS_OBJECT_MODEL_LARGE)
struct integer {
    ObjectHeader header;
    DyInteger val;
};
#define INTVAL(obj)       (((struct integer *)obj)->val)

struct character {
    ObjectHeader header;
    char val;
};
#define CHARVAL(obj)       (((struct character *)obj)->val)
#endif

#if defined(MARLAIS_OBJECT_MODEL_LARGE) && defined(MARLAIS_ENABLE_WCHAR)
struct wide_character {
    ObjectHeader header;
    wchar_t val;
};
#define WCHARVAL(obj)       (((struct wide_character *)obj)->val)
#endif

#if defined(MARLAIS_OBJECT_MODEL_LARGE) && defined(MARLAIS_ENABLE_UCHAR)
struct unicode_character {
    ObjectHeader header;
    UChar32 val;
};
#define UCHARVAL(obj)       (((struct unicode_character *)obj)->val)
#endif

struct big_integer {
    ObjectHeader header;
    void *val;
};

#define BIGINTVAL(obj)    (((struct big_integer *)obj)->val)

struct ratio {
    ObjectHeader header;
    DyInteger numerator, denominator;
};

#define RATIONUM(obj)     (((struct ratio *)obj)->numerator)
#define RATIODEN(obj)     (((struct ratio *)obj)->denominator)

struct single_float {
    ObjectHeader header;
    float val;
};

#define SFLOATVAL(obj)    (((struct single_float *)obj)->val)

struct double_float {
    ObjectHeader header;
    double val;
};

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

#ifdef MARLAIS_ENABLE_WCHAR
struct wide_string {
    ObjectHeader header;
    int size;
    wchar_t *val;
};

#define WIDESTRSIZE(obj)  (((struct wide_string *)obj)->size)
#define WIDESTRVAL(obj)   (((struct wide_string *)obj)->val)
#endif

#ifdef MARLAIS_ENABLE_UCHAR
struct unicode_string {
    ObjectHeader header;
    int size;
    UChar *val;
};

#define USTRSIZE(obj)  (((struct unicode_string *)obj)->size)
#define USTRVAL(obj)   (((struct unicode_string *)obj)->val)
#endif

struct byte_vector {
    ObjectHeader header;
    int size;
    uint8_t *els;
};

#define BYTEVSIZE(obj)      (((struct byte_vector *)obj)->size)
#define BYTEVELS(obj)       (((struct byte_vector *)obj)->els)

struct simple_object_vector {
    ObjectHeader header;
    int size;
    Object *els;
};

#define SOVSIZE(obj)      (((struct simple_object_vector *)obj)->size)
#define SOVELS(obj)       (((struct simple_object_vector *)obj)->els)

struct table_entry {
    ObjectHeader header;
    int row;
    Object key;
    Object value;
    Object next;
};

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

enum {
      /* class is defined by the interpreter */
      MARLAIS_CLASS_BUILTIN = 1,
      /* class is abstract */
      MARLAIS_CLASS_ABSTRACT = 2,
      /* class is primary */
      MARLAIS_CLASS_PRIMARY = 4,
      /* class is sealed */
      MARLAIS_CLASS_SEALED  = 8,
      /* class slots are uninitialized */
      MARLAIS_CLASS_UNINITIALIZED = 16,
      /* by default a class is none of those things */
      MARLAIS_CLASS_DEFAULT = 0
};

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
#define ABSTRACTP(obj)      (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_ABSTRACT))
#define PRIMARYP(obj)      (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_PRIMARY))
#define SEALEDP(obj)      (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_SEALED))
#define CLASSUNINITIALIZED(obj)  (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_UNINITIALIZED))
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

struct subclass {
    ObjectHeader header;
    Object val;
};

#define SUBCLASSVAL(obj)    (((struct subclass *)obj)->val)

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
    prim_0, prim_1, prim_2, prim_3, prim_4, prim_5,
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

struct stdio_handle {
  ObjectHeader header;
  bool owned;
  FILE *file;
};

#define STDIOOWNEDP(obj) (((struct stdio_handle *)obj)->owned)
#define STDIOFILE(obj) (((struct stdio_handle *)obj)->file)
