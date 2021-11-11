/*

   symbol.c

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

#include <marlais/symbol.h>

#include <marlais/alloc.h>
#include <marlais/string.h>
#include <marlais/prim.h>

#include <ctype.h>

/* Internal types */

struct symtab {
    Object sym;
    struct symtab *next;
};

/* Internal variables */

/* If SYMTAB_SIZE is not a power of 2, see change required below. */
#define SYMTAB_SIZE 1024
struct symtab *symbol_table[SYMTAB_SIZE];

/* Internal function declarations */

static Object intern_symbol (ObjectType type, const char *name);

/* Primitives */

static struct primitive symbol_prims[] =
{
    {"%symbol->string", prim_1, marlais_symbol_to_string},
    {"%string->symbol", prim_1, marlais_string_to_symbol},
    {"%symbol->name", prim_1, marlais_symbol_to_name},
    {"%name->symbol", prim_1, marlais_name_to_symbol},
};

/* Exported functions */

void
marlais_register_symbol (void)
{
  int num = sizeof (symbol_prims) / sizeof (struct primitive);
  marlais_register_prims (num, symbol_prims);
}

Object
marlais_make_name (const char *name)
{
  Object obj;

  obj = intern_symbol (Name, name);
  return (obj);
}

Object
marlais_make_symbol (const char *name)
{
  Object obj;

  obj = intern_symbol (Symbol, name);
  return (obj);
}

Object
marlais_make_setter_symbol (Object sym)
{
  size_t namelen;
  char *name;

  namelen = 1 + strlen(SYMBOLNAME(sym)) + strlen("-setter");
  name = MARLAIS_ALLOCATE_STRING(namelen);
  strcpy (name, SYMBOLNAME (sym));
  strcat (name, "-setter");

  return (marlais_make_name (name));
}

Object
marlais_symbol_to_string (Object sym)
{
  return (marlais_make_bytestring (SYMBOLNAME (sym)));
}

Object
marlais_string_to_symbol (Object str)
{
  return (marlais_make_name (BYTESTRVAL (str)));
}

Object
marlais_symbol_to_name (Object keyword)
{
  char name[MAX_SYMBOL_SIZE];
  int size;

  strcpy (name, NAMENAME (keyword));
  size = strlen (name);
  name[size - 1] = '\0';
  return (marlais_make_name (name));
}

Object
marlais_name_to_symbol (Object symbol)
{
  char name[MAX_SYMBOL_SIZE];

  strcpy (name, SYMBOLNAME (symbol));
  strcat (name, ":");
  return (marlais_make_symbol (name));
}

/* Internal functions */

static Object
intern_symbol (ObjectType type, const char *name)
{
  int i;
  unsigned h;
  struct symtab *entry;
  Object sym;

  h = i = 0;
  while (name[i]) {
    h += tolower (name[i++]);
  }
/*
   h = h % SYMTAB_SIZE;
 */

  /* Works only if SYMTAB_SIZE is a power of 2 */
  h &= (SYMTAB_SIZE - 1);

  entry = symbol_table[h];
  while (entry) {
    if (strcasecmp (name, SYMBOLNAME (entry->sym)) == 0) {
      return (entry->sym);
    }
    entry = entry->next;
  }

    /* not found, create new entry for it. */
  sym = marlais_allocate_object (type, sizeof (struct symbol));

  SYMBOLNAME (sym) = marlais_allocate_strdup (name);

  entry = MARLAIS_ALLOCATE_STRUCT (struct symtab);
  entry->sym = sym;
  entry->next = symbol_table[h];

  symbol_table[h] = entry;

  return (sym);
}
