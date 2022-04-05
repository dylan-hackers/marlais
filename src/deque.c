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

#include <marlais/deque.h>

#include <marlais/alloc.h>
#include <marlais/prim.h>
#include <marlais/sequence.h>
#include <marlais/vector.h>

/* Internal functions */

static Object deque_make_entry (Object prev, Object value, Object next);

/* Primitives */

static Object deque_size (Object d);
static Object deque_first (Object d, Object default_ob);
static Object deque_last (Object d, Object default_ob);
static Object deque_element (Object d, Object i, Object default_ob);
static Object deque_element_setter (Object d, Object i, Object new);
/*static Object deque_remove_bang (Object d, Object v, Object test, Object count);*/
static Object deque_initial_state (Object d);
static Object deque_next_state (Object d, Object s);
static Object deque_final_state (Object d);
static Object deque_previous_state (Object d, Object s);
static Object deque_current_element (Object d, Object s);
static Object deque_current_element_setter (Object d,
                                            Object s,
                                            Object new_value);

static struct primitive deque_prims[] =
{
  {"%deque-size",      prim_1, deque_size},

  {"%deque-push",      prim_2, marlais_deque_push},
  {"%deque-pop",       prim_1, marlais_deque_pop},
  {"%deque-push-last", prim_2, marlais_deque_push_last},
  {"%deque-pop-last",  prim_1, marlais_deque_pop_last},

  {"%deque-first", prim_2, deque_first},
  {"%deque-last", prim_2, deque_last},
  {"%deque-element", prim_3, deque_element},
  {"%deque-element-setter", prim_3, deque_element_setter},
/*{"%deque-remove!", prim_4, deque_remove_bang},*/
  {"%deque-initial-state", prim_1, deque_initial_state},
  {"%deque-next-state", prim_2, deque_next_state},
  {"%deque-final-state", prim_1, deque_final_state},
  {"%deque-previous-state", prim_2, deque_previous_state},
  {"%deque-current-element", prim_2, deque_current_element},
  {"%deque-current-element-setter", prim_3, deque_current_element_setter},

  {"%deque->list",   prim_1, marlais_deque_to_list},
  {"%deque->vector", prim_1, marlais_deque_to_vector},
  {"%list->deque",   prim_1, marlais_list_to_deque},
  {"%vector->deque", prim_1, marlais_vector_to_deque},
};

/* Exported functions */

void
marlais_register_deque (void)
{
  int num = sizeof (deque_prims) / sizeof (struct primitive);
  marlais_register_prims (num, deque_prims);
}

Object
marlais_make_deque (void)
{
  Object obj = marlais_allocate_object (ObjectDeque, sizeof (struct deque));

  DEQUEFIRST (obj) = MARLAIS_NIL;
  DEQUELAST (obj) = MARLAIS_NIL;
  return (obj);
}

Object
marlais_make_deque_entrypoint (Object args)
{
  int size;
  Object size_obj, fill_obj, first, last, deq;

  marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<object-deque>");

  deq = marlais_make_deque ();
  /* actually fabricate the list representing the deque */
  if (size--) {
    first = last = deque_make_entry (MARLAIS_NIL, fill_obj,
                                     MARLAIS_NIL);
    DEQUEFIRST (deq) = first;
    while (size--) {
      DENEXT (last) = deque_make_entry (last, fill_obj, NULL);
      last = DENEXT (last);
    }
    DENEXT (last) = MARLAIS_NIL;
    DEQUELAST (deq) = last;
  } else {
    DEQUEFIRST (deq) = DEQUELAST (deq) = DENEXT (deq) = MARLAIS_NIL;
  }
  return (deq);
}

int
marlais_deque_size (Object d)
{
  int res = 0;
  Object de = DEQUEFIRST(d);
  while(!EMPTYLISTP(de)) {
    res++;
    de = DENEXT (de);
  }
  return res;
}


Object
marlais_deque_push (Object d, Object new)
{
  Object new_entry = deque_make_entry(MARLAIS_NIL, new, DEQUEFIRST (d));
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    DEQUEFIRST (d) = DEQUELAST (d) = new_entry;
  } else {
    DEPREV (DEQUEFIRST (d)) = new_entry;
    DEQUEFIRST (d) = new_entry;
  }
  return (d);
}

Object
marlais_deque_pop (Object d)
{
  Object ret;

  if (EMPTYLISTP (DEQUEFIRST (d))) {
    marlais_error ("pop: cannot pop empty <deque>", d, NULL);
  }
  ret = DEVALUE (DEQUEFIRST (d));
  DEQUEFIRST (d) = DENEXT (DEQUEFIRST (d));
  if (!EMPTYLISTP (DEQUEFIRST (d))) {
    DEPREV (DEQUEFIRST (d)) = MARLAIS_NIL;
  }
  return (ret);
}

Object
marlais_deque_push_last (Object d, Object new)
{
  Object new_entry = deque_make_entry (DEQUELAST (d), new, MARLAIS_NIL);
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    DEQUEFIRST (d) = DEQUELAST (d) = new_entry;
  } else {
    DENEXT (DEQUELAST (d)) = new_entry;
    DEPREV (new_entry) = DEQUELAST (d);
    DEQUELAST (d) = new_entry;
  }
  return (d);
}

Object
marlais_deque_pop_last (Object d)
{
  Object res;

  if (EMPTYLISTP (DEQUEFIRST (d))) {
    marlais_error ("pop-list: cannot pop empty <deque>", d, NULL);
  }
  res = DEVALUE (DEQUELAST (d));
  if (DEQUEFIRST (d) == DEQUELAST (d)) {
    DEQUEFIRST (d) = DEQUELAST (d) = MARLAIS_NIL;
  } else {
    DEQUELAST (d) = DEPREV (DEQUELAST (d));
    if (!EMPTYLISTP (DEQUELAST (d))) {
      DENEXT (DEQUELAST (d)) = MARLAIS_NIL;
    }
  }
  return (res);
}

Object
marlais_deque_to_list (Object deq)
{
  Object l = MARLAIS_NIL;
  Object de = DEQUELAST(deq);
  while(!EMPTYLISTP (de)) {
    l = marlais_cons (DEVALUE (de), l);
    de = DEPREV (de);
  }
  return l;
}

Object
marlais_deque_to_vector (Object deq)
{
  int n = marlais_deque_size (deq), i = 0;
  Object v = marlais_make_vector (n, MARLAIS_NIL);
  Object de = DEQUEFIRST (deq);
  while(!EMPTYLISTP (de)) {
    SOVELS(v)[i] = DEVALUE (de);
    de = DENEXT (de);
  }
  return v;
}

Object
marlais_list_to_deque (Object lst)
{
  Object d = marlais_make_deque ();
  Object l = lst;
  while(!EMPTYLISTP (l)) {
    marlais_deque_push_last (d, CAR (l));
    l = CDR (l);
  }
  return d;
}

Object
marlais_vector_to_deque (Object vec)
{
  Object d = marlais_make_deque ();
  int n = SOVSIZE (vec), i;
  for(i = 0; i < n; i++) {
    marlais_deque_push_last (d, SOVELS(vec)[i]);
  }
  return d;
}

/* Internal functions */

static Object
deque_make_entry (Object prev, Object value, Object next)
{
  Object obj = marlais_allocate_object (ObjectDequeEntry, sizeof (struct deque_entry));

  DEPREV (obj) = prev;
  DEVALUE (obj) = value;
  DENEXT (obj) = next;
  return (obj);
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
deque_initial_state (Object d)
{
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    return (MARLAIS_FALSE);
  } else {
    return (DEQUEFIRST (d));
  }
}

static Object
deque_next_state (Object d, Object s)
{
  if (EMPTYLISTP (DENEXT (s))) {
    return (MARLAIS_FALSE);
  } else {
    return (DENEXT (s));
  }
}

static Object
deque_final_state (Object d)
{
  if (EMPTYLISTP (DEQUELAST (d))) {
    return (MARLAIS_FALSE);
  } else {
    return (DEQUELAST (d));
  }
}

static Object
deque_previous_state (Object d, Object s)
{
  if (EMPTYLISTP (DEPREV (s))) {
    return (MARLAIS_FALSE);
  } else {
    return (DEPREV (s));
  }
}

static Object
deque_current_element (Object d, Object s)
{
  return (DEVALUE (s));
}

static Object
deque_current_element_setter (Object d, Object s, Object new_value)
{
  return (DEVALUE (s) = new_value);
}
