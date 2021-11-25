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
