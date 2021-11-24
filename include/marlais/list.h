/*

   list.h

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

#ifndef MARLAIS_LIST_H
#define MARLAIS_LIST_H

#include <marlais/common.h>

/* Initialize list constants */
extern void marlais_initialize_list (void);
/* Register list primitives */
extern void marlais_register_list (void);

/* Entrypoint for make(<pair>) */
extern Object marlais_make_pair_entrypoint (Object args);
/* Entrypoint for make(<list>) */
extern Object marlais_make_list_entrypoint (Object args);

#ifdef MARLAIS_OBJECT_MODEL_LARGE
/* Returns the empty list */
static inline Object marlais_make_nil (void) {
  assert(marlais_mil); // TODO remove
  return marlais_nil;
}
#else
/* Returns the empty list */
static inline Object marlais_make_nil (void) {
  return EMPTYLISTVAL;
}
#endif

/* Make a <pair> */
extern Object marlais_cons (Object car, Object cdr);

extern Object marlais_car (Object lst);
extern Object marlais_cdr (Object lst);

extern Object marlais_second (Object lst);
extern Object marlais_third (Object lst);

extern Object marlais_map1 (Object (*fun) (Object), Object lst);
extern Object marlais_map2 (Object (*fun) (Object, Object), Object l1, Object l2);

extern Object marlais_map_apply1 (Object fun, Object lst);
extern Object marlais_map_apply2 (Object fun, Object l1, Object l2);

Object append (Object l1, Object l2);
Object append_bang (Object l1, Object l2);
int member (Object obj, Object lst);
Object member_p (Object obj, Object lst, Object test);
Object listem (Object car,...);
Object list_reduce (Object fun, Object init, Object lst);
Object list_reduce1 (Object fun, Object lst);
Object list_length_int (Object lst);
int list_length (Object lst);
int list_equal (Object l1, Object l2);
Object list_reverse (Object lst);
Object list_reverse_bang (Object lst);
Object copy_list (Object lst);

Object add_new_at_end (Object *lst, Object elt);
Object list_sort (Object lst, Object test);
Object list_sort_bang (Object lst, Object test);

#endif
