/* class.h -- see COPYRIGHT for use */

#ifndef MARLAIS_CLASS_H
#define MARLAIS_CLASS_H

#include <marlais/common.h>
#include <marlais/symbol.h>

/* Data structures */

struct marlais_class {
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
    struct environment *creation_env;
};

enum {
      /* class is abstract */
      MARLAIS_CLASS_ABSTRACT = (1<<1),
      /* class is primary */
      MARLAIS_CLASS_PRIMARY = (1<<2),
      /* class is sealed */
      MARLAIS_CLASS_SEALED = (1<<3),
      /* class was defined by the interpreter */
      MARLAIS_CLASS_BUILTIN = (1<<4),
      /* class was defined at toplevel */
      MARLAIS_CLASS_DEFINED = (1<<5),
      /* class slots are uninitialized */
      MARLAIS_CLASS_UNINITIALIZED = (1<<6),
      /* instances of this class are immediate values */
      MARLAIS_CLASS_IMMEDIATE = (1<<7),
      /* by default a class is none of those things */
      MARLAIS_CLASS_DEFAULT = 0
};

#define CLASSNAME(obj)    (((struct marlais_class *)obj)->name)
#define CLASSSUPERS(obj)  (((struct marlais_class *)obj)->supers)
#define CLASSSUBS(obj)    (((struct marlais_class *)obj)->subs)
#define CLASSINSLOTDS(obj) (((struct marlais_class *)obj)->inherited_slot_descriptors)
#define CLASSSLOTDS(obj)  (((struct marlais_class *)obj)->inst_slot_descriptors)
#define CLASSCSLOTS(obj)  (((struct marlais_class *)obj)->class_slots)
#define CLASSCSLOTDS(obj) (((struct marlais_class *)obj)->class_slot_descriptors)
#define CLASSESSLOTDS(obj) (((struct marlais_class *)obj)->eachsubclass_slot_descriptors)
#define CLASSESSLOTS(obj) (((struct marlais_class *)obj)->eachsubclass_slots)
#define CLASSCONSTSLOTDS(obj) (((struct marlais_class *)obj)->constant_slot_descriptors)
#define CLASSVSLOTDS(obj) (((struct marlais_class *)obj)->virtual_slot_descriptors)
#define CLASSPRECLIST(obj)(((struct marlais_class *)obj)->precedence_list)
#define CLASSSORTEDPRECS(obj) (((struct marlais_class *)obj)->sorted_prec_list)
#define CLASSNUMPRECS(obj) (((struct marlais_class *)obj)->num_precs)
#define CLASSPROPS(obj)   (((struct marlais_class *)obj)->properties)
#define CLASSABSTRACTP(obj)      (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_ABSTRACT))
#define CLASSDEFINEDP(obj)      (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_DEFINED))
#define CLASSIMMEDIATEP(obj)    (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_IMMEDIATE))
#define CLASSPRIMARYP(obj)      (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_PRIMARY))
#define CLASSSEALEDP(obj)      (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_SEALED))
#define CLASSBUILTINP(obj)      (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_BUILTIN))
#define CLASSUNINITIALIZED(obj)  (CLASSP (obj) && (CLASSPROPS (obj) & MARLAIS_CLASS_UNINITIALIZED))
#define CLASSENV(obj)     (((struct marlais_class *)obj)->creation_env)
#define CLASSINDEX(obj)     (((struct marlais_class *)obj)->ordinal_index)

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

/* Globals */
extern int last_class_index;
#define NEWCLASSINDEX (++last_class_index)

/* Initialization */
extern void marlais_initialize_class (void);
extern void marlais_register_class (void);

/* Instance methods */
extern Object marlais_object_class (Object obj);

/* Class methods */
extern Object marlais_direct_subclasses (Object class);
extern Object marlais_direct_superclasses (Object class);
extern Object marlais_all_superclasses (Object class);

/* Instance creation */
extern Object marlais_make (Object class, Object rest);

/* Class creation */
extern Object marlais_make_class (Object class_object,
                                  Object supers,
                                  Object slot_descriptors,
                                  int flags,
                                  char *debug_name);
extern Object marlais_make_slot_descriptor_list (Object slots, int do_eval);
extern void marlais_make_getter_setter_gfs (Object slotds);

#endif
