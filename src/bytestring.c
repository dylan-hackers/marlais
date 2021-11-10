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

#include <marlais/bytestring.h>

#include <marlais/alloc.h>
#include <marlais/character.h>
#include <marlais/number.h>
#include <marlais/prim.h>
#include <marlais/sequence.h>
#include <marlais/symbol.h>

#include <ctype.h>

/* Internal function declarations */

static Object make_string(char *str, size_t len);

/* Primitives */

static Object string_element (Object string, Object index, Object default_ob);
static Object string_element_setter (Object string, Object index, Object val);
static Object string_size (Object string);
static Object string_size_setter (Object size, Object string);
static Object string_append2 (Object str1, Object str2);
static Object string_lessthan (Object str1, Object str2);
static Object string_equal (Object str1, Object str2);
static Object string_as_lowercase (Object string);
static Object string_as_uppercase (Object string);
static Object string_as_lowercase_bang (Object string);
static Object string_as_uppercase_bang (Object string);

static struct primitive string_prims[] =
{
    {"%string-element", prim_3, string_element},
    {"%string-element-setter", prim_3, string_element_setter},
    {"%string-size", prim_1, string_size},
    {"%string-size-setter", prim_2, string_size_setter},
    {"%string-append2", prim_2, string_append2},
    {"%string<", prim_2, string_lessthan},
    {"%string=", prim_2, string_equal},
    {"%string-as-lowercase", prim_1, string_as_lowercase},
    {"%string-as-uppercase", prim_1, string_as_uppercase},
    {"%string-as-lowercase!", prim_1, string_as_lowercase_bang},
    {"%string-as-uppercase!", prim_1, string_as_uppercase_bang},
};

/* Exported functions */

void
marlais_register_bytestring (void)
{
    int num = sizeof (string_prims) / sizeof (struct primitive);
    marlais_register_prims (num, string_prims);
}

Object
marlais_make_bytestring (const char *str)
{
    int size;
    char *new;

    size = strlen (str);

    new = MARLAIS_ALLOCATE_STRING (size + 1);

    strncpy (new, str, size);
    new[size] = 0;

    return (make_string (new, size));
}

Object
marlais_make_bytestring_entrypoint (Object args)
{
  int size;
  Object size_obj, fill_obj;
  char fill = ' ';
  char *new;

  marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<string>");

  if (fill_obj != MARLAIS_FALSE) {
    if (!CHARP (fill_obj)) {
      marlais_error ("make: value of fill: must be a character for <string> class",
                     fill_obj, NULL);
    }
    fill = CHARVAL (fill_obj);
  }

  new = MARLAIS_ALLOCATE_STRING (size + 1);

  memset (new, fill, size);
  new[size] = 0;

  return (make_string (new, size));
}

/* Internal functions */

static Object
make_string(char *str, size_t len) {
  Object obj;

  obj = marlais_allocate_object (ByteString, sizeof (struct byte_string));
  BYTESTRVAL (obj) = str;
  BYTESTRSIZE (obj) = len;

  return obj;
}

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
    int len1, len2, newlen;
    char *newstr;

    len1 = BYTESTRSIZE (str1);
    len2 = BYTESTRSIZE (str2);
    newlen = len1 + len2;

    newstr = MARLAIS_ALLOCATE_STRING (newlen + 1);

    strncpy (newstr, str1, len1);
    strncpy (newstr + len1, str2, len2);
    newstr[newlen] = 0;

    return (make_string (newstr, newlen));
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

static Object
string_as_lowercase (Object string)
{
    int len, i;
    char *oldstr, *newstr;

    len = BYTESTRSIZE (string);
    oldstr = BYTESTRVAL (string);
    newstr = MARLAIS_ALLOCATE_STRING (len + 1);

    for(i = 0; i < len; i++) {
      newstr[i] = tolower (oldstr[i]);
    }
    newstr[len] = 0;

    return (make_string (newstr, len));
}

static Object
string_as_uppercase (Object string)
{
    int len, i;
    char *oldstr, *newstr;

    len = BYTESTRSIZE (string);
    oldstr = BYTESTRVAL (string);
    newstr = MARLAIS_ALLOCATE_STRING (len + 1);

    for(i = 0; i < len; i++) {
      newstr[i] = toupper (oldstr[i]);
    }
    newstr[len] = 0;

    return (make_string (newstr, len));
}

static Object
string_as_lowercase_bang (Object string)
{
    int len, i;
    char *str;

    len = BYTESTRSIZE (string);
    str = BYTESTRVAL (string);

    for(i = 0; i < len; i++) {
      str[i] = tolower (str[i]);
    }
    str[len] = 0;

    return (string);
}

static Object
string_as_uppercase_bang (Object string)
{
    int len, i;
    char *str;

    len = BYTESTRSIZE (string);
    str = BYTESTRVAL (string);

    for(i = 0; i < len; i++) {
      str[i] = toupper (str[i]);
    }
    str[len] = 0;

    return (string);
}
