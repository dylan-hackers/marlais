/*

   prim.c

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

#include <string.h>

#include "prim.h"

#include "alloc.h"
#include "bytestring.h"
#include "error.h"
#include "list.h"
#include "symbol.h"

void
init_prims (int num, struct primitive prims[])
{
  int i;
  Object sym, prim;

  for (i = 0; i < num; ++i) {
    sym = make_symbol (prims[i].name);
    prim = make_primitive (prims[i].name, prims[i].prim_type, prims[i].fun);
    /* just for now, don't add %foo to be exported */
    add_top_lvl_binding1(sym, prim, 0, prims[i].name[0] != '%');
  }
}

Object
make_primitive (char *name, enum primtype type, Object (*fun) ())
{
  Object obj;

#ifndef SMALL_OBJECTS
  obj = allocate_object (sizeof (struct primitive));
#else
  obj = allocate_object (sizeof (struct prim));
#endif

  PRIMTYPE (obj) = Primitive;
  PRIMNAME (obj) = checking_strdup (name);
  PRIMPTYPE (obj) = type;
  PRIMFUN (obj) = fun;
  return (obj);
}

Object
apply_prim (Object prim, Object args)
{
#if defined(MACOS) && defined(THINK_C)
  Object (*fun) (...);
#else
  Object (*fun) ();
#endif

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
  case prim_0_1:
    switch (list_length (args)) {
    case 0:
      return (*fun) (NULL);
    case 1:
      return (*fun) (FIRST (args));
    default:
      error ("incorrect number of args to primitive", prim, NULL);
    }
  case prim_0_2:
    switch (list_length (args)) {
    case 0:
      return (*fun) (NULL, NULL);
    case 1:
      return (*fun) (FIRST (args), NULL);
    case 2:
      return (*fun) (FIRST (args), SECOND (args));
    default:
      error ("incorrect number of args to primitive", prim, NULL);
    }
  case prim_0_3:
    switch (list_length (args)) {
    case 0:
      return (*fun) (NULL, NULL, NULL);
    case 1:
      return (*fun) (FIRST (args), NULL, NULL);
    case 2:
      return (*fun) (FIRST (args), SECOND (args), NULL);
    case 3:
      return (*fun) (FIRST (args), SECOND (args), THIRD (args));
    default:
      error ("incorrect number of args to primitive", prim, NULL);
    }
  case prim_1_1:
    switch (list_length (args)) {
    case 1:
      return (*fun) (FIRST (args), NULL);
    case 2:
      return (*fun) (FIRST (args), SECOND (args));
    default:
      error ("incorrect number of args to primitive", prim, NULL);
    }
  case prim_2_1:
    switch (list_length (args)) {
    case 2:
      return (*fun) (FIRST (args), SECOND (args), NULL);
    case 3:
      return (*fun) (FIRST (args), SECOND (args), THIRD (args));
    default:
      return error ("incorrect number of args to primitive", prim, 0);
    }
  case prim_0_rest:
    return (*fun) (args);
  case prim_1_rest:
    return (*fun) (CAR (args), CDR (args));
  case prim_2_rest:
    return (*fun) (FIRST (args), SECOND (args), CDR (CDR (args)));
  default:
    return error ("cannot handle primitive type", prim, NULL);
  }
}
