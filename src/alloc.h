/*

   alloc.h

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

#ifndef ALLOC_H
#define ALLOC_H

#include "object.h"
#include "env.h"

/* initialize the garbage collector */
extern void  marlais_initialize_gc (void);

/* allocate memory */
extern void *marlais_allocate_memory (size_t size);
/* reallocate memory */
extern void *marlais_reallocate_memory (void *old_obj, size_t new_size);
/* allocate atomic memory (no pointers) */
extern void *marlais_allocate_atomic (size_t size);

/* allocate an object */
extern Object marlais_allocate_object (size_t size);

struct frame *allocate_frame (void);
struct binding *allocate_binding (void);
struct module_binding *allocate_module_binding (void);
struct symtab *allocate_symtab (void);
void *checking_malloc (size_t size);
void *checking_realloc (void *ptr, size_t size);
char *checking_strdup (char *str);
char *allocate_string (size_t size);

#endif
