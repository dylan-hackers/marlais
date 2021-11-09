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


struct modules {
    int size;
    struct module_binding **bindings;
};

struct module_binding {
    Object sym;
    struct frame *namespace;
    Object exported_bindings;
};

struct frame {
    int size;
    Object owner;
    struct binding **bindings;
    struct frame *next;
    struct binding **top_level_env;
};

extern void marlais_register_env (void);

extern Object marlais_make_environment (struct frame *env);
extern struct frame *marlais_current_environment (void);

extern void marlais_push_scope (Object owner);
extern void marlais_pop_scope (void);

/* Warning!!! - you can't mix calls to add_locals() and
   add_local() within the same frame.  Things will get
   hopelessly screwed up.
 */
extern void marlais_add_locals (Object syms, Object vals, int constant,
				struct frame *to_frame);
extern void marlais_add_local (Object sym, Object val, int constant,
			       struct frame *to_frame);

extern struct module_binding *marlais_get_module (Object module_name);
extern struct module_binding *marlais_new_module (Object module_name);
extern struct module_binding *marlais_set_module (struct module_binding *module);
extern struct module_binding *marlais_current_module (void);

extern void marlais_add_binding(Object sym, Object val, int constant);
extern void marlais_add_export(Object sym, Object val, int constant);
extern void marlais_change_binding (Object sym, Object new_val);

extern Object marlais_use_module (Object module_name,
				  Object imports,
				  Object exclusions,
				  Object prefix,
				  Object renames,
				  Object exports);

extern Object marlais_user_current_module (void);
extern Object marlais_user_set_module (Object args);

extern Object marlais_symbol_value (Object sym);
extern void marlais_modify_value (Object sym, Object new_val);

extern struct binding *marlais_symbol_binding (Object sym);
extern struct binding *marlais_symbol_binding_top_level (Object sym);



void fill_table_from_property_set (Object the_table, Object the_set);
Object print_env (struct frame *env);
Object show_bindings (Object args);
int unwind_to_exit (Object exit_sym);
struct frame *module_namespace ();


#endif
