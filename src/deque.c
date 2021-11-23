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
#include <marlais/list.h>
#include <marlais/prim.h>
#include <marlais/sequence.h>
#include <marlais/symbol.h>

/* Static declarations */

static Object deque_make_entry (Object prev, Object value, Object next);

/* Primitives */

static Object deque_push (Object d, Object new);
static Object deque_pop (Object d);
static Object deque_push_last (Object d, Object new);
static Object deque_pop_last (Object d);
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
  {"%deque-push", prim_2, deque_push},
  {"%deque-pop", prim_1, deque_pop},
  {"%deque-push-last", prim_2, deque_push_last},
  {"%deque-pop-last", prim_1, deque_pop_last},
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
  Object obj = marlais_allocate_object (Deque, sizeof (struct deque));

  DEQUEFIRST (obj) = marlais_make_nil ();
  DEQUELAST (obj) = marlais_make_nil ();
  return (obj);
}

Object
marlais_make_deque_entrypoint (Object args)
{
  int size;
  Object size_obj, fill_obj, first, last, deq;

  marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<deque>");

  deq = marlais_make_deque ();
  /* actually fabricate the list representing the deque */
  if (size--) {
    first = last = deque_make_entry (marlais_make_nil (), fill_obj,
                                     marlais_make_nil ());
    DEQUEFIRST (deq) = first;
    while (size--) {
      DENEXT (last) = deque_make_entry (last, fill_obj, NULL);
      last = DENEXT (last);
    }
    DENEXT (last) = marlais_make_nil ();
    DEQUELAST (deq) = last;
  } else {
    DEQUEFIRST (deq) = DEQUELAST (deq) = DENEXT (deq) = marlais_make_nil ();
  }
  return (deq);
}

/* Internal functions */

static Object
deque_make_entry (Object prev, Object value, Object next)
{
  Object obj = marlais_allocate_object (DequeEntry, sizeof (struct deque_entry));

  DEPREV (obj) = prev;
  DEVALUE (obj) = value;
  DENEXT (obj) = next;
  return (obj);
}

static Object
deque_push (Object d, Object new)
{
  Object new_entry = deque_make_entry(marlais_make_nil (), new, DEQUEFIRST (d));
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    DEQUEFIRST (d) = DEQUELAST (d) = new_entry;
  } else {
    DEPREV (DEQUEFIRST (d)) = new_entry;
    DEQUEFIRST (d) = new_entry;
  }
  return (d);
}

static Object
deque_pop (Object d)
{
  Object ret;

  if (EMPTYLISTP (DEQUEFIRST (d))) {
    marlais_error ("pop: cannot pop empty <deque>", d, NULL);
  }
  ret = DEVALUE (DEQUEFIRST (d));
  DEQUEFIRST (d) = DENEXT (DEQUEFIRST (d));
  if (!EMPTYLISTP (DEQUEFIRST (d))) {
    DEPREV (DEQUEFIRST (d)) = marlais_make_nil ();
  }
  return (ret);
}

static Object
deque_push_last (Object d, Object new)
{
  Object new_entry = deque_make_entry (DEQUELAST (d), new, marlais_make_nil ());
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    DEQUEFIRST (d) = DEQUELAST (d) = new_entry;
  } else {
    DENEXT (DEQUELAST (d)) = new_entry;
    DEPREV (new_entry) = DEQUELAST (d);
    DEQUELAST (d) = new_entry;
  }
  return (d);
}

static Object
deque_pop_last (Object d)
{
  Object res;

  if (EMPTYLISTP (DEQUEFIRST (d))) {
    marlais_error ("pop-list: cannot pop empty <deque>", d, NULL);
  }
  res = DEVALUE (DEQUELAST (d));
  if (DEQUEFIRST (d) == DEQUELAST (d)) {
    DEQUEFIRST (d) = DEQUELAST (d) = marlais_make_nil ();
  } else {
    DEQUELAST (d) = DEPREV (DEQUELAST (d));
    if (!EMPTYLISTP (DEQUELAST (d))) {
      DENEXT (DEQUELAST (d)) = marlais_make_nil ();
    }
  }
  return (res);
}

static Object
deque_first (Object d, Object default_ob)
{
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    if (default_ob == default_object) {
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
    if (default_ob == default_object) {
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
      if (default_ob == default_object) {
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
  return (unspecified_object);
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
