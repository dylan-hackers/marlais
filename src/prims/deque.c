/*

   deque.c

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

#include <marlais/object/deque.h>

#include <marlais/object/prim.h>
#include <marlais/object/sequence.h>
#include <marlais/object/vector.h>

/* Internal functions */

static Object deque_make_entry (Object prev, Object value, Object next);

/* Primitives */

static Object deque_size (Object d);
static Object deque_first (Object d, Object default_ob);
static Object deque_last (Object d, Object default_ob);
static Object deque_element (Object d, Object i, Object default_ob);
static Object deque_element_setter (Object d, Object i, Object new);

/*static Object deque_remove_bang (Object d, Object v, Object test, Object count);*/

static Object deque_first_entry (Object d);
static Object deque_last_entry (Object d);

static Object deque_entry_next (Object de);
static Object deque_entry_previous (Object de);
static Object deque_entry_value (Object de);
static Object deque_entry_value_setter (Object de,
                                        Object new_value);

static struct primitive deque_prims[] = {
  {"%deque-size",      prim_1, deque_size},

  {"%deque-push",      prim_2, marlais_deque_push},
  {"%deque-pop",       prim_1, marlais_deque_pop},
  {"%deque-push-last", prim_2, marlais_deque_push_last},
  {"%deque-pop-last",  prim_1, marlais_deque_pop_last},

  {"%deque->list",     prim_1, marlais_deque_to_list},
  {"%deque->vector",   prim_1, marlais_deque_to_vector},
  {"%list->deque",     prim_1, marlais_list_to_deque},
  {"%vector->deque",   prim_1, marlais_vector_to_deque},

  {"%deque-first", prim_2, deque_first},
  {"%deque-last", prim_2, deque_last},
  {"%deque-element", prim_3, deque_element},
  {"%deque-element-setter", prim_3, deque_element_setter},

/*{"%deque-remove!", prim_4, deque_remove_bang},*/

  {"%deque-first-entry", prim_1, deque_first_entry},
  {"%deque-last-entry", prim_1, deque_last_entry},

  {"%deque-entry-next", prim_1, deque_entry_next},
  {"%deque-entry-previous", prim_1, deque_entry_previous},
  {"%deque-entry-value", prim_1, deque_entry_value},
  {"%deque-entry-value-setter", prim_2, deque_entry_value_setter},
};

/* Exported functions */

void
marlais_register_deque (void)
{
  int num = sizeof (deque_prims) / sizeof (struct primitive);
  marlais_register_prims (num, deque_prims);
}

/* Primitives */

static Object
deque_size (Object d)
{
  return marlais_make_integer(marlais_deque_size(d));
}

static Object
deque_first (Object d, Object default_ob)
{
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    if (default_ob == marlais_default) {
      marlais_error ("first: empty <deque>", d, NULL);
    } else {
      return default_ob;
    }
  }
  return (DEVALUE (DEQUEFIRST (d)));
}

static Object
deque_last (Object d, Object default_ob)
{
  if (EMPTYLISTP (DEQUELAST (d))) {
    if (default_ob == marlais_default) {
      marlais_error ("last: empty <deque>", d, NULL);
    } else {
      return default_ob;
    }
  }
  return (DEVALUE (DEQUELAST (d)));
}

static Object
deque_element (Object d, Object index, Object default_ob)
{
  int i;
  Object el;

  i = INTVAL (index);
  el = DEQUEFIRST (d);
  while (i) {
    i--;
    el = DENEXT (el);
    if (EMPTYLISTP (el)) {
      if (default_ob == marlais_default) {
        marlais_error ("element: out of range", index, d, NULL);
      } else {
        return default_ob;
      }
    }
  }
  return (DEVALUE (el));
}

static Object
deque_element_setter (Object d, Object index, Object new)
{
  int i;
  Object el;

  i = INTVAL (index);
  el = DEQUEFIRST (d);
  if (EMPTYLISTP (el)) {
    marlais_error ("attempt to set element of empty deque", NULL);
  }
  while (i) {
    i--;
    el = DENEXT (el);
    if (EMPTYLISTP (el)) {
      marlais_error ("element: out of range", index, d, NULL);
    }
  }
  DEVALUE (el) = new;
  return (MARLAIS_UNSPECIFIED);
}

static Object
deque_first_entry (Object d)
{
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    return (MARLAIS_FALSE);
  } else {
    return (DEQUEFIRST (d));
  }
}

static Object
deque_last_entry (Object d)
{
  if (EMPTYLISTP (DEQUELAST (d))) {
    return (MARLAIS_FALSE);
  } else {
    return (DEQUELAST (d));
  }
}

static Object
deque_entry_next (Object de)
{
  if (EMPTYLISTP (DENEXT (de))) {
    return (MARLAIS_FALSE);
  } else {
    return (DENEXT (de));
  }
}

static Object
deque_entry_previous (Object de)
{
  if (EMPTYLISTP (DEPREV (de))) {
    return (MARLAIS_FALSE);
  } else {
    return (DEPREV (de));
  }
}

static Object
deque_entry_value (Object de)
{
  return (DEVALUE (de));
}

static Object
deque_entry_value_setter (Object de, Object new_value)
{
  return (DEVALUE (de) = new_value);
}
