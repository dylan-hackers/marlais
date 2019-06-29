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

#include "deque.h"

#include "alloc.h"
#include "error.h"
#include "list.h"
#include "prim.h"
#include "symbol.h"
#include "sequence.h"

/* primitives */

static Object push (Object d, Object new);
static Object pop (Object d);
static Object push_last (Object d, Object new);
static Object pop_last (Object d);
static Object deque_first (Object d, Object default_ob);
static Object deque_last (Object d, Object default_ob);
static Object deque_element (Object d, Object i, Object default_ob);
static Object deque_element_setter (Object d, Object i, Object new);
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
  {"%push", prim_2, push},
  {"%pop", prim_1, pop},
  {"%push-last", prim_2, push_last},
  {"%pop-last", prim_1, pop_last},
  {"%deque-first", prim_2, deque_first},
  {"%deque-last", prim_2, deque_last},
  {"%deque-element", prim_3, deque_element},
  {"%deque-element-setter", prim_3, deque_element_setter},
  {"%deque-initial-state", prim_1, deque_initial_state},
  {"%deque-next-state", prim_2, deque_next_state},
  {"%deque-final-state", prim_1, deque_final_state},
  {"%deque-previous-state", prim_2, deque_previous_state},
  {"%deque-current-element", prim_2, deque_current_element},
  {"%deque-current-element-setter", prim_3, deque_current_element_setter},
};

void
init_deque_prims (void)
{
  int num = sizeof (deque_prims) / sizeof (struct primitive);
  init_prims (num, deque_prims);
}

Object
make_deque (void)
{
  Object obj = marlais_allocate_object (Deque, sizeof (struct deque));

  DEQUEFIRST (obj) = make_empty_list ();
  DEQUELAST (obj) = make_empty_list ();
  return (obj);
}

Object
make_deque_entry (Object prev, Object value, Object next)
{
  Object obj = marlais_allocate_object (DequeEntry, sizeof (struct deque_entry));

  DEPREV (obj) = prev;
  DEVALUE (obj) = value;
  DENEXT (obj) = next;
  return (obj);
}

Object
make_deque_driver (Object args)
{
  int size;
  Object size_obj, fill_obj, first, last, deq;

  make_sequence_driver(args, &size, &size_obj, &fill_obj, "<deque>");

  deq = make_deque ();
  /* actually fabricate the list representing the deque */
  if (size--) {
    first = last = make_deque_entry (make_empty_list (), fill_obj,
				     make_empty_list ());
    DEQUEFIRST (deq) = first;
    while (size--) {
      DENEXT (last) = make_deque_entry (last, fill_obj, NULL);
      last = DENEXT (last);
    }
    DENEXT (last) = make_empty_list ();
    DEQUELAST (deq) = last;
  } else {
    DEQUEFIRST (deq) = DEQUELAST (deq) = DENEXT (deq) = make_empty_list ();
  }
  return (deq);
}

/* primitives */

static Object
push (Object d, Object new)
{
  Object new_entry = make_deque_entry(make_empty_list (), new, DEQUEFIRST (d));
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    DEQUEFIRST (d) = DEQUELAST (d) = new_entry;
  } else {
    DEPREV (DEQUEFIRST (d)) = new_entry;
    DEQUEFIRST (d) = new_entry;
  }
  return (d);
}

static Object
pop (Object d)
{
  Object ret;

  if (EMPTYLISTP (DEQUEFIRST (d))) {
    error ("pop: cannot pop empty <deque>", d, NULL);
  }
  ret = DEVALUE (DEQUEFIRST (d));
  DEQUEFIRST (d) = DENEXT (DEQUEFIRST (d));
  if (!EMPTYLISTP (DEQUEFIRST (d))) {
    DEPREV (DEQUEFIRST (d)) = make_empty_list ();
  }
  return (ret);
}

static Object
push_last (Object d, Object new)
{
  Object new_entry = make_deque_entry (DEQUELAST (d), new, make_empty_list ());
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
pop_last (Object d)
{
  Object res;

  if (EMPTYLISTP (DEQUEFIRST (d))) {
    error ("pop-list: cannot pop empty <deque>", d, NULL);
  }
  res = DEVALUE (DEQUELAST (d));
  if (DEQUEFIRST (d) == DEQUELAST (d)) {
    DEQUEFIRST (d) = DEQUELAST (d) = make_empty_list ();
  } else {
    DEQUELAST (d) = DEPREV (DEQUELAST (d));
    if (!EMPTYLISTP (DEQUELAST (d))) {
      DENEXT (DEQUELAST (d)) = make_empty_list ();
    }
  }
  return (res);
}

static Object
deque_first (Object d, Object default_ob)
{
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    if (default_ob == default_object) {
      error ("first: empty <deque>", d, NULL);
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
      error ("last: empty <deque>", d, NULL);
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
	error ("element: out of range", index, d, NULL);
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
    error ("attempt to set element of empty deque", NULL);
  }
  while (i) {
    i--;
    el = DENEXT (el);
    if (EMPTYLISTP (el)) {
      error ("element: out of range", index, d, NULL);
    }
  }
  DEVALUE (el) = new;
  return (unspecified_object);
}

static Object
deque_initial_state (Object d)
{
  if (EMPTYLISTP (DEQUEFIRST (d))) {
    return (false_object);
  } else {
    return (DEQUEFIRST (d));
  }
}

static Object
deque_next_state (Object d, Object s)
{
  if (EMPTYLISTP (DENEXT (s))) {
    return (false_object);
  } else {
    return (DENEXT (s));
  }
}

static Object
deque_final_state (Object d)
{
  if (EMPTYLISTP (DEQUELAST (d))) {
    return (false_object);
  } else {
    return (DEQUELAST (d));
  }
}

static Object
deque_previous_state (Object d, Object s)
{
  if (EMPTYLISTP (DEPREV (s))) {
    return (false_object);
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
