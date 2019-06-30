/*

   vector.c

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
   0.6 Revisions Copyright 2001, Douglas M. Auclair.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include "vector.h"

#include "alloc.h"
#include "error.h"
#include "list.h"
#include "number.h"
#include "prim.h"
#include "symbol.h"
#include "sequence.h"

/* primitives */

static Object vector_element_setter (Object vec, Object index, Object val);
static Object vector_size (Object vec);

static struct primitive vector_prims[] =
{
    {"%vector-element", prim_3, vector_element},
    {"%vector-element-setter", prim_3, vector_element_setter},
    {"%vector-size", prim_1, vector_size},
    {"%vector", prim_1, make_sov},
};

/* function definitions */
void
init_vector_prims (void)
{
  int num = sizeof (vector_prims) / sizeof (struct primitive);
  init_prims (num, vector_prims);
}

Object
make_sov (Object el_list)
{
  Object obj, els;
  int size, i;

  obj = marlais_allocate_object (SimpleObjectVector, sizeof (struct simple_object_vector));

  size = 0;
  els = el_list;
  while (PAIRP (els)) {
    size++;
    els = CDR (els);
  }
  SOVSIZE (obj) = size;
  SOVELS (obj) = (Object *) marlais_allocate_memory (size * sizeof (Object));

  els = el_list;
  i = 0;
  while (PAIRP (els)) {
    SOVELS (obj)[i++] = CAR (els);
    els = CDR (els);
  }
  return (obj);
}

Object
make_vector (int size, Object fill_obj)
{
  Object res;
  int i;

  /* actually fabricate the vector */
  res = marlais_allocate_object (SimpleObjectVector, sizeof (struct simple_object_vector));

  SOVSIZE (res) = size;
  SOVELS (res) = (Object *) marlais_allocate_memory (size * sizeof (Object));

  for (i = 0; i < size; ++i) {
    SOVELS (res)[i] = fill_obj;
  }
  return (res);
}

/* Called with args to make */
Object
make_vector_driver (Object args)
{
  int size;
  Object size_obj, fill_obj;

  make_sequence_driver(args, &size, &size_obj, &fill_obj, "<vector>");
  return make_vector (size, fill_obj);
}

Object
vector_to_list (Object vec)
{
  int i;
  Object first, cur, acons;

  cur = make_empty_list ();
  for (i = 0; i < SOVSIZE (vec); ++i) {
    acons = cons (SOVELS (vec)[i], make_empty_list ());
    if (!EMPTYLISTP (cur)) {
      CDR (cur) = acons;
    } else {
      first = acons;
    }
    cur = acons;
  }
  return (first);
}

/* primitives */

Object
vector_element (Object vec, Object index, Object default_ob)
{
  int i, size;

  i = INTVAL (index);
  size = SOVSIZE (vec);
  if ((i < 0) || (i >= size)) {
    if (default_ob == default_object) {
      marlais_error ("element: index out of range", vec, index, NULL);
    } else {
      return default_ob;
    }
  }
  return (SOVELS (vec)[i]);
}

static Object
vector_element_setter (Object vec, Object index, Object val)
{
  int i, size;

  i = INTVAL (index);
  size = SOVSIZE (vec);
  if ((i < 0) || (i >= size)) {
    marlais_error ("element-setter: index out of range", vec, index, NULL);
  }
  return (SOVELS (vec)[i] = val);
}

static Object
vector_size (Object vec)
{
  return (marlais_make_integer (SOVSIZE (vec)));
}
