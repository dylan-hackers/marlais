/*

   bytestring.c

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

#include "bytestring.h"

#include "alloc.h"
#include "character.h"
#include "error.h"
#include "number.h"
#include "prim.h"
#include "symbol.h"
#include "sequence.h"

/* Primitives */

static Object string_element (Object string, Object index, Object default_ob);
static Object string_element_setter (Object string, Object index, Object val);
static Object string_size (Object string);
static Object string_size_setter (Object size, Object string);
static Object string_append2 (Object str1, Object str2);
static Object string_lessthan (Object str1, Object str2);
static Object string_equal (Object str1, Object str2);

static struct primitive string_prims[] =
{
    {"%string-element", prim_3, string_element},
    {"%string-element-setter", prim_3, string_element_setter},
    {"%string-size", prim_1, string_size},
    {"%string-size-setter", prim_2, string_size_setter},
    {"%string-append2", prim_2, string_append2},
    {"%string<", prim_2, string_lessthan},
    {"%string=", prim_2, string_equal},
};

/* Exported functions */

void
marlais_register_bytestring (void)
{
    int num = sizeof (string_prims) / sizeof (struct primitive);
    init_prims (num, string_prims);
}

Object
marlais_make_bytestring (char *str)
{
    Object obj = marlais_allocate_object (ByteString, sizeof (struct byte_string));

    BYTESTRSIZE (obj) = strlen (str);
    BYTESTRVAL (obj) = marlais_allocate_strdup (str);
    return (obj);
}

Object
marlais_make_bytestring_entry (Object args)
{
  int size, i;
  char fill;
  Object size_obj, fill_obj, res;

  make_sequence_driver(args, &size, &size_obj, &fill_obj, "<string>");

  if (fill_obj != MARLAIS_FALSE) {
    if (!CHARP (fill_obj)) {
      marlais_error ("make: value of fill: must be a character for <string> class",
	     fill_obj, NULL);
    }
    fill = CHARVAL (fill_obj);
  } else {
    fill = ' ';
  }

  /* actually fabricate the string */
  res = marlais_allocate_object (ByteString, sizeof (struct byte_string));

  BYTESTRSIZE (res) = size;
  BYTESTRVAL (res) = MARLAIS_ALLOCATE_STRING (size + 1);
  for (i = 0; i < size; ++i) {
    BYTESTRVAL (res)[i] = fill;
  }
  BYTESTRVAL (res)[i] = '\0';

  return (res);
}

/* Static functions */

static Object
string_element (Object string, Object index, Object default_ob)
{
    int i;

    i = INTVAL (index);
    if ((i < 0) || (i >= BYTESTRSIZE (string))) {
	if (default_ob == default_object) {
	    marlais_error ("element: argument out of range", string, index, NULL);
	} else {
	    return default_ob;
	}
    }
    return (marlais_make_character (BYTESTRVAL (string)[i]));
}

static Object
string_element_setter (Object string, Object index, Object val)
{
    int i;

    i = INTVAL (index);
    if ((i < 0) || (i >= BYTESTRSIZE (string))) {
	marlais_error ("element-setter: argument out of range", string, index, NULL);
    }
    BYTESTRVAL (string)[i] = CHARVAL (val);
    return (unspecified_object);
}

static Object
string_size (Object string)
{
    return (marlais_make_integer (BYTESTRSIZE (string)));
}

static Object
string_size_setter (Object size, Object string)
{
    int new_size = INTVAL (size);

    if ((new_size < 0) || (new_size >= BYTESTRSIZE (string))) {
	marlais_error ("size-setter: new size out of range", new_size, string, NULL);
    }
    BYTESTRSIZE (string) = new_size;
    BYTESTRVAL (string)[new_size] = '\0';
    return size;
}

static Object
string_append2 (Object str1, Object str2)
{
    char *new_str;
    int new_size;

    new_size = BYTESTRSIZE (str1) + BYTESTRSIZE (str2);
    new_str = (char *) marlais_allocate_memory ((new_size * sizeof (char)) + 1);

    strcpy (new_str, BYTESTRVAL (str1));
    strcat (new_str, BYTESTRVAL (str2));
    return (marlais_make_bytestring (new_str));
}

static Object
string_lessthan (Object str1, Object str2)
{
    return (strcmp (BYTESTRVAL (str1), BYTESTRVAL (str2)) < 0) ?
	MARLAIS_TRUE : MARLAIS_FALSE;
}

static Object
string_equal (Object str1, Object str2)
{
    return (strcmp (BYTESTRVAL (str1), BYTESTRVAL (str2)) == 0) ?
	MARLAIS_TRUE : MARLAIS_FALSE;
}
