/*

   core.c

   This software is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this software; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Original copyright notice follows:

   Copyright, 1993, Brent Benson.  All Rights Reserved.
   0.4 & 0.5 Revisions Copyright 1994, Joseph N. Wilson.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include <marlais/object/values.h>

#include <marlais/object/prim.h>

#include <marlais/core/apply.h>
#include <marlais/core/eval.h>

/* Forward declarations */

static Object prim_identity (Object obj);
static Object prim_values(Object rest);
static Object prim_not (Object obj);
static Object prim_eq (Object obj1, Object obj2, Object rest);
static Object prim_not_eq (Object obj1, Object obj2, Object rest);
static Object prim_instance_p (Object obj, Object class);
static Object prim_subtype_p (Object class1, Object class2);

/* Primitive definitions */

static struct primitive core_prims[] =
{
  {"identity", prim_1, prim_identity},
  {"values", prim_0_rest, prim_values},
  {"~", prim_1, prim_not},
  {"==", prim_2_rest, prim_eq},
  {"~==", prim_2_rest, prim_not_eq},
  {"%instance?", prim_2, prim_instance_p},
  {"%subtype?", prim_2, prim_subtype_p},
  {"%make-limited-integer", prim_1, marlais_make_limited_integer},
  {"%make-singleton", prim_1, marlais_make_singleton},
  {"%make-subclass", prim_1, marlais_make_subclass},
  {"%make-union", prim_1, marlais_make_union},
  {"%symbol->string", prim_1, marlais_symbol_to_string},
  {"%string->symbol", prim_1, marlais_string_to_symbol},
  {"%symbol->name", prim_1, marlais_symbol_to_name},
  {"%name->symbol", prim_1, marlais_name_to_symbol},
  {"%apply", prim_2, marlais_apply},
  {"%eval", prim_1, marlais_eval},
  {"%print", prim_2, marlais_print_obj},
  {"%princ", prim_2, marlais_print_obj_escaped},
};

/* Exported functions */

void
marlais_register_core (void)
{
  MARLAIS_REGISTER_PRIMS(core_prims);
}

/* Primitive implementations */

static Object
prim_identity (Object obj)
{
  return obj;
}

static Object
prim_values (Object rest)
{
  return marlais_values(rest);
}

static Object
prim_not (Object obj)
{
  if (obj == MARLAIS_FALSE) {
    return (MARLAIS_TRUE);
  } else {
    return (MARLAIS_FALSE);
  }
}

static Object
prim_eq (Object obj1, Object obj2, Object rest)
{
  /* check fixed arguments first */
  if(!marlais_identical_p(obj1, obj2)) {
    return MARLAIS_FALSE;
  }

  /* iterate through the rest */
  while (!EMPTYLISTP (rest)) {
    obj1 = obj2;
    obj2 = CAR(rest);
    rest = CDR(rest);

    if(!marlais_identical_p(obj1, obj2)) {
      return MARLAIS_FALSE;
    }
  }

  /* if we get here they are all identical */
  return MARLAIS_TRUE;
}

static Object
prim_not_eq (Object obj1, Object obj2, Object rest)
{
  return prim_not (prim_eq (obj1, obj2, rest));
}

static Object
prim_instance_p (Object obj, Object type)
{
  return marlais_make_boolean (marlais_instance_p (obj, type));
}

static Object
prim_subtype_p (Object type1, Object type2)
{
  return marlais_make_boolean (marlais_subtype_p (type1, type2));
}
