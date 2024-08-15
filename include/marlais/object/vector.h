/*

   vector.h

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

#ifndef MARLAIS_VECTOR_H
#define MARLAIS_VECTOR_H

#include <marlais/common.h>

/* Data structures */

struct marlais_vector {
  marlais_header_t header;
  marlais_size_t   vector_size;
  Object           vector_elements[] __marlais_counted_by(vector_size);
};

/* Function declarations */

/* Register vector primitives */
extern void marlais_register_vector (void);

/* Make a <vector> */
extern Object marlais_make_vector (marlais_size_t size, Object fill);
/* Entrypoint for make(<vector>) */
extern Object marlais_make_vector_entrypoint (Object args);

/* Get the size of the the vector */
static inline marlais_size_t marlais_vector_size (Object vec) {
  return ((struct marlais_vector*)vec)->vector_size;
}
/* Get an element from the vector */
static inline Object marlais_vector_get (Object vec, marlais_index_t idx, Object def) {
  struct marlais_vector *v = (struct marlais_vector *)vec;
  size_t size = v->vector_size;
  if (idx >= size) {
    if (def == marlais_default) {
      marlais_error ("element: index out of range", vec, marlais_make_integer(idx), NULL);
    } else {
      return def;
    }
  }
  return v->vector_elements[idx];
}
/* Set an element in the vector */
static inline Object marlais_vector_set (Object vec, marlais_index_t idx, Object val) {
  struct marlais_vector *v = (struct marlais_vector *)vec;
  size_t size = v->vector_size;
  if (idx >= size) {
    marlais_error ("element-setter: index out of range", vec, marlais_make_integer(idx), NULL);
  }
  v->vector_elements[idx] = val;
  return val;
}

/* Get a const pointer to the elements array of a vector */
static inline const Object* marlais_vector_ref_const (Object vec) {
  return ((struct marlais_vector*)vec)->vector_elements;
}
/* Get a mutable pointer to the elements array of a vector */
static inline Object* marlais_vector_ref_mutable (Object vec) {
  return ((struct marlais_vector*)vec)->vector_elements;
}

/* Make a vector from a list */
extern Object marlais_list_to_vector (Object list);
/* Convert a vector to a list */
extern Object marlais_vector_to_list (Object vec);

#endif /* !MARLAIS_VECTOR_H */
