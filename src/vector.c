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

#include <marlais/vector.h>

#include <marlais/alloc.h>
#include <marlais/list.h>
#include <marlais/number.h>
#include <marlais/prim.h>
#include <marlais/sequence.h>
#include <marlais/symbol.h>

/* Primitives */

static Object prim_vector_size (Object vec);
static Object prim_vector_size_setter (Object vec, Object size);
static Object prim_vector_element (Object vec, Object index, Object def);
static Object prim_vector_element_setter (Object vec, Object index, Object val);

static struct primitive vector_prims[] =
{
    {"%vector-size", prim_1, prim_vector_size},
    {"%vector-size-setter", prim_1, prim_vector_size_setter},
    {"%vector-element", prim_3, prim_vector_element},
    {"%vector-element-setter", prim_3, prim_vector_element_setter},
    {"%vector->list", prim_1, marlais_vector_to_list},
    {"%list->vector", prim_1, marlais_list_to_vector},
};

/* Exported functions */

void
marlais_register_vector (void)
{
  int num = sizeof (vector_prims) / sizeof (struct primitive);
  marlais_register_prims (num, vector_prims);
}

Object
marlais_make_vector (int size, Object fill_obj)
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

Object
marlais_make_vector_entrypoint (Object args)
{
  int size;
  Object size_obj, fill_obj;

  marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<vector>");

  // XXX check size

  return marlais_make_vector (size, fill_obj);
}

Object
marlais_list_to_vector (Object list)
{
  Object obj, els;
  int size, i;

  /* XXX improve this - count first, allocate, overwrite */

  obj = marlais_allocate_object (SimpleObjectVector, sizeof (struct simple_object_vector));

  size = 0;
  els = list;
  while (PAIRP (els)) {
    size++;
    els = CDR (els);
  }
  SOVSIZE (obj) = size;
  SOVELS (obj) = (Object *) marlais_allocate_memory (size * sizeof (Object));

  els = list;
  i = 0;
  while (PAIRP (els)) {
    SOVELS (obj)[i++] = CAR (els);
    els = CDR (els);
  }
  return (obj);
}

Object
marlais_vector_to_list (Object vec)
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

/* Primitives */

static Object
prim_vector_size (Object vec)
{
  return (marlais_make_integer (SOVSIZE (vec)));
}

static Object
prim_vector_size_setter (Object vec, Object size)
{
  return NULL;
}

static Object
prim_vector_element (Object vec, Object index, Object default_ob)
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
prim_vector_element_setter (Object vec, Object index, Object val)
{
  int i, size;

  i = INTVAL (index);
  size = SOVSIZE (vec);
  if ((i < 0) || (i >= size)) {
    marlais_error ("element-setter: index out of range", vec, index, NULL);
  }
  return (SOVELS (vec)[i] = val);
}
