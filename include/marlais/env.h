/*

   env.h

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

#ifndef MARLAIS_ENV_H
#define MARLAIS_ENV_H

#include <marlais/common.h>

/* If TOP_LEVEL_SIZE is not a power of two, see change required below */
#define TOP_LEVEL_SIZE 1024

struct binding {
    Object sym, *val, type;
    int props;
    struct binding *next; /* in top_level_env hash chain */
};

#define CONSTANT_BINDING 0x01
#define IMPORTED_BINDING 0x02
#define EXPORTED_BINDING 0x04

#define IS_IMPORTED_BINDING(binding) (binding->props & IMPORTED_BINDING)
#define IS_CONSTANT_BINDING(binding) (binding->props & CONSTANT_BINDING)
#define IS_EXPORTED_BINDING(binding) (binding->props & EXPORTED_BINDING)

extern Object marlais_make_toplevel (Object owner);

extern void marlais_push_scope (Object owner);
extern void marlais_pop_scope (void);

/* Warning!!! - you can't mix calls to add_locals() and
   add_local() within the same frame.  Things will get
   hopelessly screwed up.
 */
extern void marlais_add_locals (Object syms, Object vals, int constant,
				struct environment *to_frame);
extern void marlais_add_local (Object sym, Object val, int constant,
			       struct environment *to_frame);

extern Object marlais_symbol_value (Object sym);
extern void marlais_modify_value (Object sym, Object new_val);

extern struct binding *marlais_symbol_binding (Object sym);
extern struct binding *marlais_symbol_binding_top_level (Object sym);

/* TODO namespace */
int unwind_to_exit (Object exit_sym);
struct environment *module_namespace ();

#endif
