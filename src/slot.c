/*

   slot.c

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

#include "slot.h"

#include "apply.h"
#include "class.h"
#include "error.h"
#include "eval.h"
#include "globaldefs.h"
#include "keyword.h"
#include "list.h"
#include "prim.h"
#include "vector.h"

/* primitives */
static Object instance_slots (Object instance);
static Object class_slots (Object class);

static struct primitive slot_prims[] =
{
    {"slot-value", prim_2, slot_value},
    {"set-slot-value!", prim_3, set_slot_value},
    {"%instance-slots", prim_1, instance_slots},
    {"%class-slots", prim_1, class_slots},
};

/* function definitions */

void
init_slot_prims (void)
{
    int num;

    num = sizeof (slot_prims) / sizeof (struct primitive);

    init_prims (num, slot_prims);
}


Object
make_slot_descriptor (unsigned char properties, Object getter, Object setter,
		      Object type, Object init, Object init_keyword,
		      Object allocation, Object dynamism)
{
    Object obj;

    obj = allocate_object (sizeof (struct slot_descriptor));

    SLOTDTYPE (obj) = SlotDescriptor;
    SLOTDPROPS (obj) = properties;
    SLOTDGETTER (obj) = getter;
    SLOTDSETTER (obj) = setter;
    SLOTDSLOTTYPE (obj) = type;
    SLOTDINIT (obj) = init;
    SLOTDINITKEYWORD (obj) = init_keyword;
    SLOTDALLOCATION (obj) = allocation;
    SLOTDDYNAMISM (obj) = dynamism;

    return obj;
}

Object
slot_name (Object slot)
{
    if (!PAIRP (slot)) {
	return (slot);
    } else {
	if (SYMBOLP (CAR (slot))) {
	    return (CAR (slot));
	} else {
	    error ("Slot has no name but needs one", slot, NULL);
	    return NULL;
	}
    }
}

Object
slot_getter (Object slot)
{
    if (!PAIRP (slot)) {
	return (NULL);
    } else {
	return (find_keyword_val (getter_keyword, slot));
    }
}

Object
slot_setter (Object slot)
{
    if (!PAIRP (slot)) {
	return (NULL);
    } else {
	return (find_keyword_val (setter_keyword, slot));
    }
}

Object
slot_type (Object slot)
{
    if (!PAIRP (slot)) {
	return (NULL);
    } else {
	return (find_keyword_val (type_keyword, slot));
    }
}

Object
slot_init_value (Object slotd)
{
    if (SLOTDINITFUNCTION (slotd)) {
	return eval (cons (listem (quote_symbol, SLOTDINIT (slotd), NULL),
			   make_empty_list ()));
    } else {
	return SLOTDINIT (slotd);
    }
}

Object
slot_init_function (Object slot)
{
    if (!PAIRP (slot)) {
	return (NULL);
    } else {
	return (find_keyword_val (init_function_keyword, slot));
    }
}

Object
slot_init_keyword (Object slot)
{
    if (!PAIRP (slot)) {
	return (NULL);
    } else {
	return (find_keyword_val (init_keyword_keyword, slot));
    }
}

Object
slot_required_init_keyword (Object slot)
{
    if (!PAIRP (slot)) {
	return (NULL);
    } else {
	return (find_keyword_val (required_init_keyword_keyword, slot));
    }
}

Object
slot_allocation (Object slot)
{
    if (!PAIRP (slot)) {
	return (NULL);
    } else {
	return (find_keyword_val (allocation_keyword, slot));
    }
}

Object
slot_value (Object instance, Object slot_num)
{
    return CAR (INSTSLOTS (instance)[INTVAL (slot_num)]);
}

Object
set_slot_value (Object instance, Object slot_num, Object val)
{
    CAR (INSTSLOTS (instance)[INTVAL (slot_num)]) = val;
    return val;
}

static Object
instance_slots (Object instance)
{
    return (Object) (&INSTSLOTS (instance));
}

static Object
class_slots (Object class)
{
    return (Object) (CLASSCSLOTS (class));
}
