/*

   deque.h

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

#ifndef MARLAIS_DEQUE_H
#define MARLAIS_DEQUE_H

#include <marlais/common.h>


/* Data structures */

struct marlais_deque {
    ObjectHeader header;
    Object first, last;
};

#define DEQUEFIRST(obj)   (((struct marlais_deque *)obj)->first)
#define DEQUELAST(obj)    (((struct marlais_deque *)obj)->last)

struct marlais_deque_entry {
    ObjectHeader header;
    Object value;
    Object prev, next;
};

#define DEVALUE(obj)      (((struct marlais_deque_entry *)obj)->value)
#define DEPREV(obj)       (((struct marlais_deque_entry *)obj)->prev)
#define DENEXT(obj)       (((struct marlais_deque_entry *)obj)->next)


/* Function declarations */

/* Register deque primitives */
extern void marlais_register_deque (void);

/* Make an empty <deque> */
extern Object marlais_make_deque (void);
/* Entrypoint for make(<deque>) */
extern Object marlais_make_deque_entrypoint (Object args);

/* Get an element from a deque */
extern int marlais_deque_size (Object deq);
/* Get an element from a deque */
extern Object marlais_deque_element (Object deq, int idx, Object def);
/* Set an element in a deque */
extern Object marlais_deque_element_setter (Object deq, int idx, Object val);

/* Push an element at the start of a deque */
extern Object marlais_deque_push (Object deq, Object new);
/* Pop an element at the start of a deque */
extern Object marlais_deque_pop (Object deq);
/* Push an element at the end of a deque */
extern Object marlais_deque_push_last (Object deq, Object new);
/* Pop an element at the end of a deque */
extern Object marlais_deque_pop_last (Object deq);

/* Convert a deque to a list */
extern Object marlais_deque_to_list (Object deq);
/* Convert a deque to a vector */
extern Object marlais_deque_to_vector (Object deq);
/* Convert a list to a deque */
extern Object marlais_list_to_deque (Object lst);
/* Convert a vector to a deque */
extern Object marlais_vector_to_deque (Object vec);

#endif /* !MARLAIS_DEQUE_H */
