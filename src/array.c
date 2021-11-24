/*

   array.c

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

#include <marlais/array.h>

#include <marlais/alloc.h>
#include <marlais/list.h>
#include <marlais/number.h>
#include <marlais/prim.h>
#include <marlais/symbol.h>

/* Internal function declarations */

static Object array_make (Object dims, Object fill);
static int    array_index (Object arr, Object indices, Object default_ob);

/* Primitives */

static Object array_size (Object arr);
static Object array_ref (Object arr, Object indices, Object default_ob);
static Object array_ref_setter (Object arr, Object indices, Object new_val);
static Object array_element (Object arr, Object index, Object default_ob);
static Object array_element_setter (Object arr, Object index, Object new_val);
static Object array_dimensions (Object arr);
static Object array_initial_state (Object arr);
static Object array_next_state (Object arr, Object state);
static Object array_current_element (Object arr, Object state);
static Object array_row_major_index (Object arr, Object indices);

static struct primitive array_prims[] =
{
  {"%array-size", prim_1, array_size},
  {"%array-ref", prim_3, array_ref},
  {"%array-ref-setter", prim_3, array_ref_setter},
  {"%array-element", prim_3, array_element},
  {"%array-element-setter", prim_3, array_element_setter},
  {"%array-dimensions", prim_1, array_dimensions},
  {"%array-initial-state", prim_1, array_initial_state},
  {"%array-next-state", prim_2, array_next_state},
  {"%array-current-element", prim_2, array_current_element},
  {"%array-row-major-index", prim_2, array_row_major_index},
};

/* Exported functions */

void
marlais_register_array (void)
{
  int num = sizeof (array_prims) / sizeof (struct primitive);
  marlais_register_prims (num, array_prims);
}

Object
marlais_make_array_entrypoint (Object args)
{
  Object dim_obj, fill_obj, res;

  dim_obj = NULL;
  fill_obj = NULL;

  while (!EMPTYLISTP (args)) {
    if (FIRST (args) == dim_keyword) {
      dim_obj = SECOND (args);
    } else if (FIRST (args) == fill_keyword) {
      fill_obj = SECOND (args);
    } else {
      marlais_error ("make: unsupported keyword for <array> class",
                     FIRST (args), NULL);
    }
    args = CDR (CDR (args));
  }
  if (dim_obj) {
    if (!LISTP (dim_obj)) {
      marlais_error ("make: value of dimensions: argument must be a list of integers",
                     dim_obj, NULL);
    }
  } else {
    marlais_error ("make: dimensions: must be specified for <array>", args, NULL);
  }
  if (!fill_obj) {
    fill_obj = MARLAIS_FALSE;
  }
  /* actually fabricate the array */
  res = array_make (dim_obj, fill_obj);
  return (res);
}

/* Static functions */

static Object
array_make (Object dims, Object fill)
{
  Object obj, dl, val;
  unsigned int size, i;

  obj = marlais_allocate_object (Array, sizeof (struct array));

  ARRDIMS (obj) = dims;
  dl = dims;
  size = 1;
  while (!EMPTYLISTP (dl)) {
    val = CAR (dl);
    if (!INTEGERP (val)) {
      marlais_error ("make: array dimensions must be integers", dims, NULL);
    }
    size *= INTVAL (val);
    dl = CDR (dl);
  }
  ARRELS (obj) = (Object *) marlais_allocate_memory (sizeof (Object) * size);

  ARRSIZE (obj) = size;
  for (i = 0; i < size; ++i) {
    ARRELS (obj)[i] = fill;
  }
  return (obj);
}

static int
array_index (Object arr, Object indices, Object default_ob)
{
  Object dims, inds, ind, dim;
  unsigned int offset, dim_val;
  int ind_val;
  unsigned int index_stride = 1;

  dims = marlais_list_reverse (ARRDIMS (arr));
  inds = marlais_list_reverse (indices);
  offset = 0;

  while (!EMPTYLISTP (dims) && !EMPTYLISTP (inds)) {
    if (EMPTYLISTP (dims)) {
      marlais_error ("element: too many indices for array", arr, indices, NULL);
    }
    if (EMPTYLISTP (inds)) {
      marlais_error ("element: not enough indices given", arr, indices, NULL);
    }
    dim = CAR (dims);
    ind = CAR (inds);
    if (!INTEGERP (ind)) {
      marlais_error ("element: array indices must be integers", ind, NULL);
    }
    dim_val = INTVAL (dim);
    ind_val = INTVAL (ind);
    if ((ind_val < 0) || (ind_val >= dim_val)) {
      if (default_ob == marlais_default) {
        marlais_error ("element: array indices out of range", indices,
                       ARRDIMS (arr), NULL);
      } else {
        return -1;
      }
    }
    offset += (ind_val * index_stride);
    index_stride *= dim_val;
    dims = CDR (dims);
    inds = CDR (inds);
  }
  if (!EMPTYLISTP (dims)) {
    marlais_error ("element: not enough indices for array", arr, indices, NULL);
  }
  if (!EMPTYLISTP (inds)) {
    marlais_error ("element: too many indices given", arr, indices, NULL);
  }
  return offset;
}

static Object
array_size (Object arr)
{
  return marlais_make_integer (ARRSIZE (arr));
}

/*
 * Note that the key of an array collection is the list of indices.
 */

static Object
array_ref (Object arr, Object indices, Object default_ob)
{
  return (ARRELS (arr)[array_index (arr, indices, default_ob)]);
}

static Object
array_ref_setter (Object arr, Object indices, Object new_val)
{
  ARRELS (arr)[array_index (arr, indices, marlais_default)] = new_val;
  return (marlais_unspecified);
}

static Object
array_element (Object arr, Object index, Object default_ob)
{
  int ind_val = INTVAL (index);

  if ((ind_val < 0) || (ind_val >= ARRSIZE (arr))) {
    if (default_ob == marlais_default) {
      return marlais_error ("element: array index out of range", index,
                            ARRDIMS (arr), NULL);
    } else {
      return default_ob;
    }
  } else {
    return ARRELS (arr)[ind_val];
  }
}

static Object
array_element_setter (Object arr, Object index, Object new_val)
{
  int ind_val = INTVAL (index);

  if ((ind_val < 0) || (ind_val >= ARRSIZE (arr))) {
    marlais_error ("element_setter: array index out of range", index,
                   ARRDIMS (arr), NULL);
  }
  ARRELS (arr)[ind_val] = new_val;
  return (marlais_unspecified);
}

static Object
array_dimensions (Object arr)
{
  return (ARRDIMS (arr));
}

static Object
array_initial_state (Object arr)
{
  return (marlais_make_integer (0));
}

static Object
array_next_state (Object arr, Object state)
{
  Object dims, dim;
  unsigned int total_size, state_val;

  total_size = 1;
  dims = ARRDIMS (arr);
  while (!EMPTYLISTP (dims)) {
    dim = CAR (dims);
    total_size *= INTVAL (dim);
    dims = CDR (dims);
  }
  state_val = INTVAL (state);
  state_val++;
  if (state_val >= total_size) {
    return (MARLAIS_FALSE);
  } else {
    return (marlais_make_integer (state_val));
  }
}

static Object
array_current_element (Object arr, Object state)
{
  return (ARRELS (arr)[INTVAL (state)]);
}

static Object
array_row_major_index (Object arr, Object indices)
{
  return marlais_make_integer (array_index (arr, indices, marlais_default));
}
