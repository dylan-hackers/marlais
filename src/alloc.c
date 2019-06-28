/*
   alloc.c

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

#include <string.h>

#include <gc.h>

#include "alloc.h"

/* for throwing errors */
#include "error.h"
/* for struct sizes */
#include "env.h"
#include "symbol.h"

/* function definitions */

void
marlais_initialize_gc (void)
{
    GC_init ();
    return;
}

void *
marlais_allocate_memory (size_t size)
{
    void *obj;
	obj = GC_malloc (size);
	if (!obj) {
        fatal ("internal error: memory allocation failure.");
	}
	return obj;
}

void *
marlais_reallocate_memory(void *old_obj, size_t new_size)
{
    void *obj;
	obj = GC_realloc (old_obj, new_size);
	if (!obj) {
        fatal ("internal error: memory allocation failure.");
	}
	return obj;
}

void *
marlais_allocate_atomic (size_t size)
{
    void *obj;
	obj = GC_malloc_atomic (size);
	if (!obj) {
        fatal ("internal error: memory allocation failure.");
	}
	return obj;
}


Object
allocate_object (size_t size)
{
    Object obj;

#ifndef SMALL_OBJECTS
    obj = (Object) GC_malloc (sizeof (struct object));
#else
    obj = (Object) GC_malloc (size);
#endif
    if (!obj) {
        fatal ("internal error: memory allocation failure.");
    }
    return (obj);
}

struct frame *
allocate_frame (void)
{
    struct frame *frame;

    frame = (struct frame *) GC_malloc (sizeof (struct frame));

    if (!frame) {
        fatal ("internal error: memory allocation failure.");
    }
    return (frame);
}

struct binding *
allocate_binding (void)
{
    struct binding *binding;

    binding = (struct binding *) GC_malloc (sizeof (struct binding));

    if (!binding) {
        fatal ("internal error: memory allocation failure.");
    }
    return (binding);
}

struct module_binding *
allocate_module_binding (void)
{
    struct module_binding *module_binding;

    module_binding =
    (struct module_binding *) GC_malloc (sizeof (struct module_binding));

    if (!module_binding) {
        fatal ("internal error: memory allocation failure.");
    }
    return (module_binding);
}

struct symtab *
allocate_symtab (void)
{
    struct symtab *entry;

    entry = (struct symtab *) GC_malloc (sizeof (struct symtab));

    if (!entry) {
        fatal ("internal error: memory allocation failure.");
    }
    return (entry);
}

void *
checking_malloc (size_t size)
{
    void *ptr;

    ptr = (void *) GC_malloc (size);
    if (!ptr) {
        fatal ("internal error: memory allocation failure");
    }
    return (ptr);
}

void *
checking_realloc (void *ptr, size_t total_size)
{
    ptr = (void *) GC_realloc (ptr, total_size);
    if (!ptr) {
        fatal ("internal error: memory allocation failure");
    }
    return (ptr);
}

char *
checking_strdup (char *str)
{
    int size;
    char *copied_str;

    size = strlen (str) + 1;
    copied_str = (char *) GC_malloc_atomic (size);
    if (!copied_str) {
        fatal ("internal error: memory allocation failure");
    }
    strcpy (copied_str, str);
    return (copied_str);
}

char *
allocate_string (size_t size)
{
    char *str;

    str = (char *) GC_malloc_atomic (size);
    if (!str) {
        fatal ("internal error: memory allocation failure.");
    }
    return (str);
}
