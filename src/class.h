/*

   class.h

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

#ifndef CLASS_H
#define CLASS_H

#include "symbol.h"
#include "object.h"

/* globals */

extern int last_class_index;

#define NEWCLASSINDEX (++last_class_index)

void init_class_prims (void);
void init_class_hierarchy (void);
Object make_class (Object class_object, Object supers, Object slot_descriptors,
		   char *debug_name);
void make_uninstantiable (Object class);
void make_primary (Object class);
Object make_instance (Object class, Object *initializers);
Object make_singleton (Object val);
Object make (Object class, Object rest);
Object instance_p (Object obj, Object class);
int instance (Object obj, Object class);
Object make_union_type (Object typelist);

#define GENERIC_INSTANCE_P(obj, class)  (apply (eval(make_symbol("instance?")), listem (obj, class, NULL)))
Object subtype_p (Object class1, Object class2);
int subtype (Object class1, Object class2);
Object objectclass (Object obj);
Object singleton (Object val);
Object direct_superclasses (Object class);
Object direct_subclasses (Object class);
Object same_class_p (Object class1, Object class2);
Object eval_slots (Object slots);
Object eval_slot (Object slot);
Object eval_slot_key_values (Object slot_tail);
Object find_getter_name (Object slot);
Object seal (Object class);
Object slot_descriptor_list (Object slots, int do_eval);
void make_getter_setter_gfs (Object slotds);

#endif
