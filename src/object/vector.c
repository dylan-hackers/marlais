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

#include <marlais/object/vector.h>

#include <marlais/object/sequence.h>

/* Exported functions */

Object
marlais_make_vector (marlais_size_t size, Object fill_obj)
{
  struct marlais_vector *res;
  int i;

  /* allocate the object and the vector */
  res = MARLAIS_ALLOCATE_OBJECT_EXTRA (ObjectVector, struct marlais_vector, size*sizeof(Object));
  res->vector_size = size;

  /* fill the vector */
  for (i = 0; i < size; ++i) {
    res->vector_elements[i] = fill_obj;
  }

  /* return the result */
  return (res);
}

Object
marlais_make_vector_entrypoint (Object args)
{
  int size;
  Object size_obj, fill_obj;

  marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<vector>");

  /* TODO check size */

  return marlais_make_vector (size, fill_obj);
}

Object
marlais_list_to_vector (Object list)
{
  marlais_size_t size;
  marlais_index_t i;
  struct marlais_vector *vec;
  Object els;

  /* TODO improve this - count first, allocate, overwrite */
  size = 0;
  els = list;
  while (marlais_is_pair_p (els)) {
    size++;
    els = CDR (els);
  }

  vec = MARLAIS_ALLOCATE_OBJECT_EXTRA (ObjectVector, struct marlais_vector, size*sizeof(Object));
  vec->vector_size = size;

  els = list;
  i = 0;
  while (marlais_is_pair_p (els)) {
    vec->vector_elements[i++] = CAR (els);
    els = CDR (els);
  }
  return (vec);
}

Object
marlais_vector_to_list (Object vec)
{
  int i;
  Object first = MARLAIS_NIL, cur = MARLAIS_NIL, acons;

  for (i = 0; i < marlais_vector_size (vec); ++i) {
    acons = marlais_cons (marlais_vector_get (vec, i, NULL), MARLAIS_NIL);
    if (!marlais_is_nil_p (cur)) {
      CDR (cur) = acons;
    } else {
      first = acons;
    }
    cur = acons;
  }
  return (first);
}
