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

#include <marlais/object/sequence.h>
#include <marlais/object/vector.h>

/* Forward declarations */

static Object deque_make_entry (Object prev, Object value, Object next);

/* Exported functions */

Object
marlais_make_deque (void)
{
  Object obj = marlais_allocate_object (ObjectDeque, sizeof (struct marlais_deque));
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
  Object v = marlais_make_vector (n, MARLAIS_FALSE);
  Object de = DEQUEFIRST (deq);
  while(!EMPTYLISTP (de)) {
    SOVELS(v)[i++] = DEVALUE (de);
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
  Object obj = marlais_allocate_object (ObjectDequeEntry, sizeof (struct marlais_deque_entry));

  DEPREV (obj) = prev;
  DEVALUE (obj) = value;
  DENEXT (obj) = next;
  return (obj);
}
