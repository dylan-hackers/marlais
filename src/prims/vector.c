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

#include <marlais/object/prim.h>
#include <marlais/object/sequence.h>

/* Forward declarations */

static Object prim_vector (Object rest);
static Object prim_vector_size (Object vec);
static Object prim_vector_element (Object vec, Object index, Object def);
static Object prim_vector_element_setter (Object vec, Object index, Object val);
static Object prim_vector_to_list (Object vec);
static Object prim_list_to_vector (Object lst);

/* Primitive definitions */

static struct primitive vector_prims[] =
{
  {"vector",                 prim_0_rest, prim_vector},
  {"%vector-size",           prim_1,      prim_vector_size},
  {"%vector-element",        prim_3,      prim_vector_element},
  {"%vector-element-setter", prim_3,      prim_vector_element_setter},
  {"%vector->list",          prim_1,      prim_vector_to_list},
  {"%list->vector",          prim_1,      prim_list_to_vector},
};

/* Exported functions */

void
marlais_register_vector (void)
{
  MARLAIS_REGISTER_PRIMS (vector_prims);
}

/* Primitives */

static Object
prim_vector (Object rest)
{
  return marlais_list_to_vector (rest);
}

static Object
prim_vector_size (Object vec)
{
  return marlais_make_integer (marlais_vector_size (vec));
}

static Object
prim_vector_element (Object vec, Object idx, Object def)
{
  marlais_index_t i = marlais_get_int (idx);
  return marlais_vector_get(vec,i,def);
}

static Object
prim_vector_element_setter (Object vec, Object idx, Object val)
{
  marlais_index_t i = marlais_get_int (idx);
  return marlais_vector_set(vec, i, val);
}

static Object
prim_vector_to_list (Object vec)
{
  return marlais_vector_to_list (vec);
}

static Object
prim_list_to_vector (Object lst)
{
  return marlais_list_to_vector (lst);
}
