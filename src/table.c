/*

   table.c

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
   0.6 Revisions Copyright 2001, Douglas M. Auclair. All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include <string.h>

#include "table.h"

#include "alloc.h"
#include "apply.h"
#include "env.h"
#include "list.h"
#include "number.h"
#include "prim.h"
#include "symbol.h"

extern Object dylan_symbol;

/* local function prototypes */

static Object make_table_entry (int row, Object key, Object value, Object next);
static Object *table_element_handle (Object table,
				     Object key,
				     Object *default_val);
static int table_vectors_equal (Object vec1, Object vec2);
static Object table_initial_state (Object table);
static Object table_next_state (Object table, Object state);
static Object table_current_element (Object table, Object state);
static Object table_current_key (Object table, Object state);
static Object table_current_element_setter (Object table, Object state, Object value);
static Object equal_hash (Object key);
static Object hash_pair (Object pair);
static Object hash_deque (Object deq);
static Object hash_string (Object string);
static Object hash_vector (Object vector);

static Object table_default = NULL;

/* primitives */

static struct primitive table_prims[] =
{
  {"%table-element", prim_3, marlais_table_element},
  {"%table-element-setter", prim_3, marlais_table_element_setter},
  {"%table-initial-state", prim_1, table_initial_state},
  {"%table-next-state", prim_2, table_next_state},
  {"%table-current-element", prim_2, table_current_element},
  {"%table-current-key", prim_2, table_current_key},
  {"%table-current-element-setter", prim_3, table_current_element_setter},
  {"%=hash", prim_1, equal_hash},
};

void
marlais_register_table (void)
{
  int num = sizeof (table_prims) / sizeof (struct primitive);
  marlais_register_prims (num, table_prims);
  table_default = cons (MARLAIS_FALSE, MARLAIS_FALSE);
}

Object
marlais_make_table (int size)
{
  Object obj = marlais_allocate_object (ObjectTable, sizeof (struct table));

  TABLESIZE (obj) = size;
  TABLETABLE (obj) = (Object *) marlais_allocate_memory (sizeof (Object) * size);
  memset (TABLETABLE (obj), 0, sizeof (Object) * size);

  return (obj);
}

Object
marlais_make_table_driver (Object rest)
{
  Object size;

  if (EMPTYLISTP (rest)) {
    return (marlais_make_table (DEFAULT_TABLE_SIZE));
  } else if (CAR (rest) == size_keyword) {
    rest = CDR (rest);
    if (EMPTYLISTP (rest)) {
      marlais_error ("make: no argument given to size keyword", NULL);
    }
    size = CAR (rest);
    if (!INTEGERP (size)) {
      marlais_error ("make: argument to size keyword must be an integer", size, NULL);
    }
    return (marlais_make_table (INTVAL (size)));
  } else {
    return marlais_error ("make: bad keywords or arguments", rest, NULL);
  }
}

Object
marlais_table_element (Object table, Object key, Object default_val)
{
  return *table_element_handle (table, key, &default_val);
}

Object
marlais_table_element_setter (Object table, Object key, Object val)
{
  Object hval, entry;
  Object *element_handle;
  int h;

  if ((element_handle = table_element_handle (table, key, &table_default))
      != &table_default) {
    *element_handle = val;
  } else {
    hval = equal_hash (key);
    h = abs (INTVAL (hval)) % TABLESIZE (table);
    entry = make_table_entry (h, key, val, TABLETABLE (table)[h]);
    TABLETABLE (table)[h] = entry;
  }
  return (unspecified_object);
}

Object *
marlais_table_element_by_vector (Object table, Object key)
{
  Object hval, entry;
  int h;

  hval = equal_hash (key);
  h = abs (INTVAL (hval)) % TABLESIZE (table);
  entry = TABLETABLE (table)[h];

  while (entry) {
    if (table_vectors_equal (key, TEKEY (entry))) {
      return (TEVALUE (entry));
    }
    entry = TENEXT (entry);
  }
  return (NULL);
}

Object
marlais_table_element_setter_by_vector (Object table, Object key, Object val)
{
  Object hval, entry;
  Object *element_handle;
  int h;

  element_handle = marlais_table_element_by_vector (table, key);
  if (element_handle) {
    *element_handle = val;
  } else {
    hval = equal_hash (key);
    h = abs (INTVAL (hval)) % TABLESIZE (table);

    entry = make_table_entry (h, key, val, TABLETABLE (table)[h]);
    TABLETABLE (table)[h] = entry;
  }
  return (unspecified_object);
}

/* local functions */

static Object
make_table_entry (int row, Object key, Object value, Object next)
{
  Object obj = marlais_allocate_object (TableEntry, sizeof (struct table_entry));

  TEROW (obj) = row;
  TEKEY (obj) = key;
  TEVALUE (obj) = value;
  TENEXT (obj) = next;
  return (obj);
}

static Object *
table_element_handle (Object table, Object key, Object *default_val)
{
  Object hval, equal_fun, entry;
  int h;
  struct frame *old_env;

  hval = equal_hash (key);
  h = abs (INTVAL (hval)) % TABLESIZE (table);
  entry = TABLETABLE (table)[h];

  old_env = the_env;
  the_env = module_binding (dylan_symbol)->namespace;
  equal_fun = symbol_value (equal_symbol);
  the_env = old_env;

  while (entry) {
    if (apply (equal_fun,
	       cons (TEKEY (entry), cons (key, make_empty_list ())))
	!= MARLAIS_FALSE) {
      return &(TEVALUE (entry));
    }
    entry = TENEXT (entry);
  }
  if (*default_val != default_object) {
    return default_val;
  } else {
    return marlais_error ("element: no element matching key", key, NULL);
  }
}

static int
table_vectors_equal (Object vec1, Object vec2)
{
  int i;

  if (SOVSIZE (vec1) != SOVSIZE (vec2))
    return (0);
  for (i = 0; i < SOVSIZE (vec1); i++) {
    if (SOVELS (vec1)[i] != SOVELS (vec2)[i])
      return (0);
  }
  return (1);
}

/* iteration protocol */

static Object
table_initial_state (Object table)
{
  int i;

  for (i = 0; i < TABLESIZE (table); ++i) {
    if (TABLETABLE (table)[i]) {
      return (TABLETABLE (table)[i]);
    }
  }
  return (MARLAIS_FALSE);
}

static Object
table_next_state (Object table, Object state)
{
  int i;

  if (TENEXT (state)) {
    return (TENEXT (state));
  }
  for (i = (TEROW (state) + 1); i < TABLESIZE (table); ++i) {
    if (TABLETABLE (table)[i]) {
      return (TABLETABLE (table)[i]);
    }
  }
  return (MARLAIS_FALSE);
}

static Object
table_current_element (Object table, Object state)
{
  return (TEVALUE (state));
}

static Object
table_current_key (Object table, Object state)
{
  return (TEKEY (state));
}

static Object
table_current_element_setter (Object table, Object state, Object value)
{
  TEVALUE (state) = value;
  return (unspecified_object);
}

static Object
equal_hash (Object key)
{
  Object hashfun;

  if (INSTANCEP (key)) {
    hashfun = symbol_value (equal_hash_symbol);
    /*
     * Need to be able to hash arbitrary instances here!
     */
    if (!hashfun) {
      marlais_error ("no =hash method defined for key class", key, NULL);
    }
    return (apply (hashfun, cons (key, make_empty_list ())));
  } else {
    if (INTEGERP (key)) {
      return (key);
    } else if (CHARP (key)) {
      return (marlais_make_integer (CHARVAL (key)));
    } else if (TRUEP (key)) {
      return (marlais_make_integer (1));
    } else if (FALSEP (key)) {
      return (marlais_make_integer (0));
    } else if (EMPTYLISTP (key)) {
      return (marlais_make_integer (2));
    } else if (PAIRP (key)) {
      return (hash_pair (key));
    } else if (DEQUEP (key)) {
      return (hash_deque (key));
    } else if (BYTESTRP (key)) {
      return (hash_string (key));
    } else if (SOVP (key)) {
      return (hash_vector (key));
    } else if (SYMBOLP (key) || KEYWORDP (key)) {
      return (marlais_make_integer ((DyInteger)key));
    } else {
      /* marlais_error ("=hash: don't know how to hash object", key, NULL);  */
      return (marlais_make_integer ((DyInteger)key));
    }
  }
}

static Object
hash_pair (Object pair)
{
  int h = INTVAL (equal_hash (CAR (pair))) + INTVAL (equal_hash (CDR (pair)));
  return (marlais_make_integer (h));
}

static Object
hash_deque (Object deq)
{
  int h = 0;
  Object entry;

  entry = DEQUEFIRST (deq);
  while (!EMPTYLISTP (entry)) {
    h += INTVAL (equal_hash (DEVALUE (entry)));
    entry = DENEXT (entry);
  }
  return (marlais_make_integer (h));
}

static Object
hash_string (Object string)
{
  int i, h;

  h = 0;
  for (i = 0; i < BYTESTRSIZE (string); ++i) {
    h += BYTESTRVAL (string)[i];
  }
  return (marlais_make_integer (h));
}

static Object
hash_vector (Object vector)
{
  int i, h = 0;
  for (i = 0; i < SOVSIZE (vector); ++i) {
    h += INTVAL (equal_hash (SOVELS (vector)[i]));
  }
  return (marlais_make_integer (h));
}
