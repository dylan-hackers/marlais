
#include <marlais/type.h>

#include <marlais/alloc.h>
#include <marlais/class.h>
#include <marlais/list.h>
#include <marlais/prim.h>

/* Primitives */

static Object instance_p (Object obj, Object class);
static Object subtype_p (Object class1, Object class2);

static struct primitive type_prims[] =
{
    {"%instance?", prim_2, instance_p},
    {"%subtype?", prim_2, subtype_p},
    {"%limited-integer", prim_1, marlais_make_limited_integer},
    {"%singleton", prim_1, marlais_make_singleton},
    {"%union-type", prim_1, marlais_make_union},
};

/* Exported functions */

void
marlais_register_type (void)
{
  int num = sizeof (type_prims) / sizeof (struct primitive);
  marlais_register_prims (num, type_prims);
}

int
marlais_instance_p (Object obj, Object type)
{
  Object objtype;

  if (SINGLETONP (type)) {
    return marlais_identical_p (obj, SINGLEVAL (type));
  } else if (LIMINTP (type)) {
    if (INTEGERP (obj) &&
        ((!LIMINTHASMIN (type)) ||
         INTVAL (obj) >= LIMINTMIN (type)) &&
        ((!LIMINTHASMAX (type)) ||
         INTVAL (obj) <= LIMINTMAX (type))) {
      return 1;
    } else {
      return 0;
    }
  } else if (LIMINTP (obj)) {
    /* not sure on this one.  jnw */
    return marlais_subtype_p (type_class, type);
  } else if (UNIONP (type)) {
    Object ptr;

    for (ptr = UNIONLIST (type); PAIRP (ptr); ptr = CDR (ptr)) {
      if (marlais_instance_p (obj, (CAR (ptr)))) {
        return 1;
      }
    }
    return 0;
  }
  objtype = marlais_object_class (obj);
  if (objtype == type) {
    return 1;
  } else {
    return (marlais_subtype_p (objtype, type));
  }
}

int
marlais_subtype_p (Object type1, Object type2)
{
  Object supers;

  if (type1 == type2) {
    return 1;
  } else if (SINGLETONP (type1)) {
    return (marlais_instance_p (SINGLEVAL (type1), type2));
  } else if (LIMINTP (type1)) {
    if (LIMINTP (type2)) {
      if (((!LIMINTHASMIN (type2)) ||
           (LIMINTHASMIN (type1) &&
            (LIMINTMIN (type1) >= LIMINTMIN (type2))))
          &&
          ((!LIMINTHASMAX (type2)) ||
           (LIMINTHASMAX (type1) &&
            (LIMINTMAX (type1) <= LIMINTMAX (type2))))) {
        return 1;
      } else {
        return 0;
      }
    } else {
      return (marlais_subtype_p (integer_class, type2));
    }
  } else if (UNIONP (type1)) {
    Object ptr;

    for (ptr = UNIONLIST (type1); PAIRP (ptr); ptr = CDR (ptr)) {
      if (!marlais_subtype_p (CAR (ptr), type2)) {
        return 0;
      }
    }
    return 1;
  } else if (UNIONP (type2)) {
    Object ptr;

    for (ptr = UNIONLIST (type2); PAIRP (ptr); ptr = CDR (ptr)) {
      if (marlais_subtype_p (type1, CAR (ptr))) {
        return 1;
      }
    }
    return 0;
  } else {
    supers = CLASSSUPERS (type1);
    if (!supers) {
      return 0;
    }
    while (!EMPTYLISTP (supers)) {
      if (marlais_subtype_p (CAR (supers), type2)) {
        return 1;
      }
      supers = CDR (supers);
    }
    return 0;
  }
}

int
marlais_same_class_p (Object class1, Object class2)
{
  if (class1 == class2) {
    return 1;
  } else if ((POINTERTYPE (class1) == Singleton) &&
             (POINTERTYPE (class2) == Singleton)) {
    if (marlais_identical_p (SINGLEVAL(class1), SINGLEVAL(class2))) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

Object
marlais_make_singleton (Object val)
{
  Object obj;
  obj = marlais_allocate_object (Singleton, sizeof (struct singleton));
  SINGLEVAL (obj) = val;
  return (obj);
}

/*
 * Incredibly speculative!
 */
Object
marlais_make_union (Object typelist)
{
  Object obj, ptr, qtr, union_types;

  obj = marlais_allocate_object (UnionType, sizeof (struct union_type));

  union_types = make_empty_list ();

  for (ptr = typelist; PAIRP (ptr); ptr = CDR (ptr)) {
    if (UNIONP (CAR (ptr))) {
      for (qtr = UNIONLIST (CAR (ptr)); PAIRP (qtr); qtr = CDR (qtr)) {
        union_types = cons (CAR (qtr), union_types);
      }
    } else {
      union_types = cons (CAR (ptr), union_types);
    }
  }
  UNIONLIST (obj) = union_types;

  return obj;
}

/*
 * Largely speculative.  Probably will change all around.
 */
Object
marlais_make_limited_integer (Object args)
{
  Object obj;

  obj = marlais_allocate_object (LimitedIntType, sizeof (struct limited_int_type));

  while (!EMPTYLISTP (args)) {
    if (FIRST (args) == min_keyword) {
      if (LIMINTHASMIN (obj)) {
        marlais_error ("Minimum value for limited type specified twice", NULL);
      } else {
        LIMINTMIN (obj) = INTVAL (SECOND (args));
        LIMINTPROPS (obj) |= LIMMINMASK;
      }
    } else if (FIRST (args) == max_keyword) {
      if (LIMINTHASMAX (obj)) {
        marlais_error ("Maximum value for limited type specified twice", NULL);
      } else {
        LIMINTMAX (obj) = INTVAL (SECOND (args));
        LIMINTPROPS (obj) |= LIMMAXMASK;
      }
    } else {
      marlais_error ("make: unsupported keyword for limited integer type",
                     FIRST (args), NULL);
    }
    args = CDR (CDR (args));
  }

  return (obj);
}

/* Internal functions */

static Object
instance_p (Object obj, Object type)
{
  return (marlais_instance_p (obj, type) ? MARLAIS_TRUE : MARLAIS_FALSE);
}

static Object
subtype_p (Object type1, Object type2)
{
  return (marlais_subtype_p (type1, type2) ? MARLAIS_TRUE : MARLAIS_FALSE);
}