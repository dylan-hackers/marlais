/*

   table.h

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

#ifndef MARLAIS_TABLE_H
#define MARLAIS_TABLE_H

#include <marlais/common.h>

/* Data structures */

struct marlais_table {
    ObjectHeader header;
    int size;
    Object *the_table;
};

#define TABLESIZE(obj)    (((struct marlais_table *)obj)->size)
#define TABLETABLE(obj)   (((struct marlais_table *)obj)->the_table)

struct marlais_table_entry {
    ObjectHeader header;
    int row;
    Object key;
    Object value;
    Object next;
};

#define TEROW(obj)        (((struct marlais_table_entry *)obj)->row)
#define TEKEY(obj)        (((struct marlais_table_entry *)obj)->key)
#define TEVALUE(obj)      (((struct marlais_table_entry *)obj)->value)
#define TENEXT(obj)       (((struct marlais_table_entry *)obj)->next)

/* Function declarations */

/* Register table primitives */
extern void marlais_register_table (void);
/* Make a new table of given size */
extern Object marlais_make_table (int size);
/* Entrypoint for make(<table>) */
extern Object marlais_make_table_entrypoint (Object rest);
/* Get element by key */
extern Object marlais_table_element (Object table, Object key, Object default_val);
/* Set element by key */
extern Object marlais_table_element_setter (Object table, Object key, Object val);
/* Get element by vector key */
extern Object *marlais_table_element_by_vector (Object table, Object key);
/* Set element by vector key */
extern Object marlais_table_element_setter_by_vector (Object table, Object key, Object val);
/* Fill table from properties */
extern Object marlais_table_fill_properties (Object the_table, Object the_set);

#endif
