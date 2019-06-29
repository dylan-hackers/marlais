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

#include <string.h>
#include <ctype.h>

#include "symbol.h"

#include "alloc.h"
#include "bytestring.h"

/* local function prototypes
 */
static Object intern_symbol (char *name);

#ifdef NO_STRCASECMP
int strcasecmp (unsigned char *s1, unsigned char *s2);

#endif

/* local data
 */

/* If SYMTAB_SIZE is not a power of 2, see change required below. */
#define SYMTAB_SIZE 1024
struct symtab *symbol_table[SYMTAB_SIZE];
unsigned char chartable[1 << sizeof (char) * 8];

/* function definitions
 */
void
init_symbol_prims (void)
{
  int c;

  for (c = 0; c < (1 << sizeof (char) * 8); c++) {
    chartable[c] = tolower (c);
  }
}

Object
make_symbol (char *name)
{
  Object obj;

  obj = intern_symbol (name);
  return (obj);
}

Object
make_keyword (char *name)
{
  Object obj;

  obj = intern_symbol (name);
  SYMBOLTYPE (obj) = Keyword;
  return (obj);
}


Object
make_setter_symbol (Object sym)
{
  size_t namelen;
  char *name;

  namelen = 1 + strlen(SYMBOLNAME(sym)) + strlen("-setter");
  name = MARLAIS_ALLOCATE_STRING(namelen);
  strcpy (name, SYMBOLNAME (sym));
  strcat (name, "-setter");

  return (make_symbol (name));
}

#ifdef NO_STRCASECMP
int
strcasecmp (unsigned char *s1, unsigned char *s2)
{
  while ((chartable[*s1] == chartable[*s2++])) {
    if (!chartable[*s1++])
      return 0;
  }
  return (chartable[*s1] - chartable[*--s2]);
}
#endif

static Object
intern_symbol (char *name)
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
#ifdef NO_STRCASECMP
    if (strcasecmp ((unsigned char *) name,
		    (unsigned char *) SYMBOLNAME (entry->sym)) == 0) {
#else
    if (strcasecmp (name, SYMBOLNAME (entry->sym)) == 0) {
#endif
      return (entry->sym);
    }
    entry = entry->next;
  }

    /* not found, create new entry for it. */
  sym = marlais_allocate_object (Symbol, sizeof (struct symbol));

  SYMBOLNAME (sym) = marlais_allocate_strdup (name);

  entry = MARLAIS_ALLOCATE_STRUCT (struct symtab);
  entry->sym = sym;
  entry->next = symbol_table[h];

  symbol_table[h] = entry;

  return (sym);
}
