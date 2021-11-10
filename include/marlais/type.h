#ifndef MARLAIS_TYPE_H
#define MARLAIS_TYPE_H

#include <marlais/common.h>

/* Register type primitives*/
extern void marlais_register_type (void);

/* Instance check */
extern int marlais_instance_p (Object obj, Object class);
/* Subtype check */
extern int marlais_subtype_p (Object class1, Object class2);
/* Same-class check */
extern int marlais_same_class_p (Object class1, Object class2);

/* Type creation */
extern Object marlais_make_limited_integer (Object args);
extern Object marlais_make_singleton (Object val);
extern Object marlais_make_union (Object typelist);

#endif
