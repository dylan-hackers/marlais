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

#include <marlais/object/array.h>

/* Exported functions */

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
      marlais_error ("make: unsupported keyword for <object-array> class",
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
    marlais_error ("make: dimensions: must be specified for <object-array>", args, NULL);
  }
  if (!fill_obj) {
    fill_obj = MARLAIS_FALSE;
  }
  /* actually fabricate the array */
  res = marlais_make_array (dim_obj, fill_obj);
  return (res);
}


/* Static functions */

Object
marlais_make_array (Object dims, Object fill)
{
  Object obj, dl, val;
  unsigned int size, i;

  obj = marlais_allocate_object (ObjectArray, sizeof (struct marlais_array));

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
  ARRELS (obj) = MARLAIS_MALLOC_ARRAY_GENERAL (size, Object);

  ARRSIZE (obj) = size;
  for (i = 0; i < size; ++i) {
    ARRELS (obj)[i] = fill;
  }
  return (obj);
}

int
marlais_array_size (Object arr)
{
  return ARRSIZE (arr);
}

int
marlais_array_index (Object arr, Object indices, Object default_ob)
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
