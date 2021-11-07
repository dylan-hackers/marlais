/* class.h -- see COPYRIGHT for use */

#ifndef MARLAIS_CLASS_H
#define MARLAIS_CLASS_H

#include <marlais/common.h>
#include <marlais/symbol.h>

/* Globals */
extern int last_class_index;
#define NEWCLASSINDEX (++last_class_index)

/* Initialization */
extern void marlais_initialize_class (void);
extern void marlais_register_class (void);

/* Instance methods */
extern Object marlais_object_class (Object obj);

/* Instance check */
extern Object marlais_instance_p (Object obj, Object class);
extern int marlais_instance (Object obj, Object class);

/* Subtype check */
extern Object marlais_subtype_p (Object class1, Object class2);
extern int marlais_subtype (Object class1, Object class2);

/* Same-class check */
extern Object marlais_same_class_p (Object class1, Object class2);

/* Class methods */
extern Object marlais_direct_subclasses (Object class);
extern Object marlais_direct_superclasses (Object class);
extern Object marlais_make (Object class, Object rest);

/* Type creation */
extern Object marlais_singleton (Object val);
extern Object marlais_make_union_type (Object typelist);

/* Class creation */
extern Object marlais_make_class (Object class_object,
                                  Object supers,
                                  Object slot_descriptors,
                                  Object abstract_p,
                                  char *debug_name);
extern Object marlais_make_slot_descriptor_list (Object slots, int do_eval);
extern void marlais_make_getter_setter_gfs (Object slotds);
extern void marlais_make_class_primary (Object class);
extern Object marlais_make_class_sealed (Object class);
extern void marlais_make_class_uninstantiable (Object class);

#endif
