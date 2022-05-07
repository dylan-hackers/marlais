#ifndef MARLAIS_TYPE_H
#define MARLAIS_TYPE_H

#include <marlais/common.h>

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

/* Register type primitives */
extern void marlais_register_type (void);

/* Instance check */
extern bool marlais_instance_p (Object obj, Object class);
/* Subtype check */
extern bool marlais_subtype_p (Object class1, Object class2);
/* Same-class check */
extern bool marlais_same_class_p (Object class1, Object class2);

/* Make a <limited-integer> */
extern Object marlais_make_limited_integer (Object args);
/* Make a <singleton>  */
extern Object marlais_make_singleton (Object val);
/* Make a <subclass>  */
extern Object marlais_make_subclass (Object cls);
/* Make a <type-union> */
extern Object marlais_make_union (Object typelist);

#endif
