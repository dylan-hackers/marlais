/* prim.c -- see COPYRIGHT for use */

#include <marlais/prim.h>

#include <marlais/alloc.h>
#include <marlais/env.h>

void
marlais_register_prims (int num, struct primitive prims[])
{
  int i;
  Object sym, prim;

  for (i = 0; i < num; ++i) {
    struct primitive *p = &prims[i];
    sym = marlais_make_name (p->name);
    prim = marlais_make_primitive (p->name, p->prim_type, p->fun);
    if(p->name[0] != '%') {
      marlais_add_export (sym, prim, 1);
    } else {
      marlais_add_binding (sym, prim, 1);
    }
  }
}

Object
marlais_make_primitive (char *name, enum primtype type, Object (*fun) ())
{
  Object obj;

#ifndef MARLAIS_OBJECT_MODEL_SMALL
  obj = marlais_allocate_object (Primitive, sizeof (struct primitive));
#else
  obj = marlais_allocate_object (Primitive, sizeof (struct prim));
#endif

  PRIMNAME (obj) = marlais_allocate_strdup (name);
  PRIMPTYPE (obj) = type;
  PRIMFUN (obj) = fun;
  return (obj);
}

/* I think this could be handled WAAAY better -- dma */
Object
marlais_apply_prim (Object prim, Object args)
{
  Object (*fun) ();

  fun = PRIMFUN (prim);
  switch (PRIMPTYPE (prim)) {
  case prim_0:
    return (*fun) ();
  case prim_1:
    return (*fun) (FIRST (args));
  case prim_2:
    return (*fun) (FIRST (args), SECOND (args));
  case prim_3:
    return (*fun) (FIRST (args), SECOND (args), THIRD (args));
  case prim_4:
    return (*fun) (FIRST (args), SECOND (args), THIRD (args), FOURTH (args));
  case prim_5:
    return (*fun) (FIRST (args), SECOND (args), THIRD (args), FOURTH (args), FIFTH (args));
  case prim_0_1:
    switch (marlais_list_length (args)) {
    case 0:
      return (*fun) (NULL);
    case 1:
      return (*fun) (FIRST (args));
    default:
      return marlais_error ("incorrect number of args to primitive", prim, NULL);
    }
  case prim_0_2:
    switch (marlais_list_length (args)) {
    case 0:
      return (*fun) (NULL, NULL);
    case 1:
      return (*fun) (FIRST (args), NULL);
    case 2:
      return (*fun) (FIRST (args), SECOND (args));
    default:
      return marlais_error ("incorrect number of args to primitive", prim, NULL);
    }
  case prim_0_3:
    switch (marlais_list_length (args)) {
    case 0:
      return (*fun) (NULL, NULL, NULL);
    case 1:
      return (*fun) (FIRST (args), NULL, NULL);
    case 2:
      return (*fun) (FIRST (args), SECOND (args), NULL);
    case 3:
      return (*fun) (FIRST (args), SECOND (args), THIRD (args));
    default:
      return marlais_error ("incorrect number of args to primitive", prim, NULL);
    }
  case prim_1_1:
    switch (marlais_list_length (args)) {
    case 1:
      return (*fun) (FIRST (args), NULL);
    case 2:
      return (*fun) (FIRST (args), SECOND (args));
    default:
      return marlais_error ("incorrect number of args to primitive", prim, NULL);
    }
  case prim_2_1:
    switch (marlais_list_length (args)) {
    case 2:
      return (*fun) (FIRST (args), SECOND (args), NULL);
    case 3:
      return (*fun) (FIRST (args), SECOND (args), THIRD (args));
    default:
      return marlais_error ("incorrect number of args to primitive", prim, NULL);
    }
  case prim_0_rest:
    return (*fun) (args);
  case prim_1_rest:
    return (*fun) (CAR (args), CDR (args));
  case prim_2_rest:
    return (*fun) (FIRST (args), SECOND (args), CDR (CDR (args)));
  default:
    return marlais_error ("unknown primitive type", NULL);
  }
}
