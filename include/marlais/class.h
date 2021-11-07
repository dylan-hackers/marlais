/* class.h -- see COPYRIGHT for use */

#ifndef MARLAIS_CLASS_H
#define MARLAIS_CLASS_H

#include <marlais/common.h>
#include <marlais/symbol.h>

/* globals */

extern int last_class_index;

#define NEWCLASSINDEX (++last_class_index)

void init_class_prims (void);
void init_class_hierarchy (void);
Object make_class (Object class_object, Object supers, Object slot_descriptors,
		   Object abstract_p, char *debug_name);
void make_uninstantiable (Object class);
void make_primary (Object class);
Object make_instance (Object class, Object *initializers);
Object make_singleton (Object val);
Object make (Object class, Object rest);
Object instance_p (Object obj, Object class);
int instance (Object obj, Object class);
Object make_union_type (Object typelist);

#define GENERIC_INSTANCE_P(obj, class)  (apply (eval(make_symbol("instance?")), listem (obj, class, NULL)))
Object subtype_p (Object class1, Object class2);
int subtype (Object class1, Object class2);
Object objectclass (Object obj);
Object singleton (Object val);
Object direct_superclasses (Object class);
Object direct_subclasses (Object class);
Object same_class_p (Object class1, Object class2);
Object eval_slot (Object slot);
Object eval_slot_key_values (Object slot_tail);
Object find_getter_name (Object slot);
Object seal (Object class);
Object slot_descriptor_list (Object slots, int do_eval);
void make_getter_setter_gfs (Object slotds);

#endif
