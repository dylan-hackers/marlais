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

#ifndef MARLAIS_ALLOC_H
#define MARLAIS_ALLOC_H

#include <marlais/common.h>

/* Allocate memory (that may contain pointers) */
extern void *marlais_malloc_general (size_t size);
/* Allocate memory (that will contain no pointers) */
extern void *marlais_malloc_atomic (size_t size);

/* Reallocate memmory */
extern void *marlais_realloc_general (void *old_obj, size_t new_size);
/* Reallocate memmory */
extern void *marlais_realloc_atomic (void *old_obj, size_t new_size);

/* Free memory */
extern void  marlais_free (void *obj);

/* Allocate copy of a zero-terminated string */
extern char *marlais_strdup (const char *str);

/* Allocate a structure with casting */
#define MARLAIS_MALLOC_GENERAL(_type)                           \
  ((_type *)marlais_malloc_general(sizeof(_type)))
/* Allocate a structure with casting */
#define MARLAIS_MALLOC_ATOMIC(_type)                            \
  ((_type *)marlais_malloc_atomic(sizeof(_type)))
/* Allocate an array with casting */
#define MARLAIS_MALLOC_ARRAY_GENERAL(_nmemb,_type)              \
  ((_type *)marlais_malloc_general((_nmemb)*sizeof(_type)))
/* Allocate an array with casting */
#define MARLAIS_MALLOC_ARRAY_ATOMIC(_nmemb,_type)               \
  ((_type *)marlais_malloc_atomic((_nmemb)*sizeof(_type)))
/* Reallocate an array with casting */
#define MARLAIS_REALLOC_ARRAY_GENERAL(_oldptr,_nmemb,_type)             \
  ((_type *)marlais_realloc_general(((void*)(_oldptr)),(_nmemb)*sizeof(_type)))
/* Reallocate an array with casting */
#define MARLAIS_REALLOC_ARRAY_ATOMIC(_oldptr,_nmemb,_type)             \
  ((_type *)marlais_realloc_atomic(((void*)(_oldptr)),(_nmemb)*sizeof(_type)))

#endif
