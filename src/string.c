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

#include <marlais/string.h>

#include <marlais/alloc.h>
#include <marlais/character.h>
#include <marlais/number.h>
#include <marlais/prim.h>
#include <marlais/sequence.h>
#include <marlais/symbol.h>

#include <ctype.h>

#ifdef MARLAIS_ENABLE_WCHAR
#include <wctype.h>
#endif

/* Internal function declarations */

static Object make_string(char *str, size_t len);
#ifdef MARLAIS_ENABLE_WCHAR
static Object make_wstring(wchar_t *str, size_t len);
#endif
#ifdef MARLAIS_ENABLE_UCHAR
static Object make_ustring(UChar *str, size_t len);
#endif

/* Primitives */

static Object string_element (Object string, Object index, Object default_ob);
static Object string_element_setter (Object string, Object index, Object val);
static Object string_size (Object string);
/* static Object string_size_setter (Object size, Object string); */
static Object string_append2 (Object str1, Object str2);
static Object string_lessthan (Object str1, Object str2);
static Object string_equal (Object str1, Object str2);
static Object string_as_lowercase (Object string);
static Object string_as_uppercase (Object string);
static Object string_as_lowercase_bang (Object string);
static Object string_as_uppercase_bang (Object string);

#ifdef MARLAIS_ENABLE_WCHAR
static Object wstring_element (Object string, Object index, Object default_ob);
static Object wstring_element_setter (Object string, Object index, Object val);
static Object wstring_size (Object string);
static Object wstring_size_setter (Object size, Object string);
static Object wstring_append2 (Object str1, Object str2);
static Object wstring_lessthan (Object str1, Object str2);
static Object wstring_equal (Object str1, Object str2);
static Object wstring_as_lowercase (Object string);
static Object wstring_as_uppercase (Object string);
static Object wstring_to_bstring (Object string);
static Object bstring_to_wstring (Object string);
#endif

#ifdef MARLAIS_ENABLE_UCHAR
static Object ustring_element (Object string, Object index, Object default_ob);
static Object ustring_element_setter (Object string, Object index, Object val);
static Object ustring_size (Object string);
static Object ustring_size_setter (Object size, Object string);
static Object ustring_append2 (Object str1, Object str2);
static Object ustring_lessthan (Object str1, Object str2);
static Object ustring_equal (Object str1, Object str2);
static Object ustring_as_lowercase (Object string);
static Object ustring_as_uppercase (Object string);
static Object ustring_as_titlecase (Object string);
static Object ustring_to_bstring (Object string);
static Object ustring_to_wstring (Object string);
#endif

static struct primitive string_prims[] =
{
    {"%string-element", prim_3, string_element},
    {"%string-element-setter", prim_3, string_element_setter},
    {"%string-size", prim_1, string_size},
/*  {"%string-size-setter", prim_2, string_size_setter}, */
    {"%string-append2", prim_2, string_append2},
    {"%string<", prim_2, string_lessthan},
    {"%string=", prim_2, string_equal},
    {"%string-as-lowercase", prim_1, string_as_lowercase},
    {"%string-as-uppercase", prim_1, string_as_uppercase},
    {"%string-as-lowercase!", prim_1, string_as_lowercase_bang},
    {"%string-as-uppercase!", prim_1, string_as_uppercase_bang},

#ifdef MARLAIS_ENABLE_WCHAR
    {"%wstring-element", prim_3, wstring_element},
    {"%wstring-element-setter", prim_3, wstring_element_setter},
    {"%wstring-size", prim_1, wstring_size},
    {"%wstring<", prim_2, wstring_lessthan},
    {"%wstring=", prim_2, wstring_equal},
    {"%wstring-as-lowercase", prim_1, wstring_as_lowercase},
    {"%wstring-as-uppercase", prim_1, wstring_as_uppercase},
#if 0
    {"%wstring->bstring", prim1, wstring_to_bstring},
    {"%bstring->wstring", prim1, bstring_to_wstring},
#endif
#endif

#ifdef MARLAIS_ENABLE_UCHAR
    {"%ustring-element", prim_3, ustring_element},
    {"%ustring-element-setter", prim_3, ustring_element_setter},
    {"%ustring-size", prim_1, ustring_size},
    {"%ustring<", prim_2, ustring_lessthan},
    {"%ustring=", prim_2, ustring_equal},
    {"%ustring-as-lowercase", prim_1, wstring_as_lowercase},
    {"%ustring-as-uppercase", prim_1, wstring_as_uppercase},
#if 0
    {"%ustring->bstring", prim1, ustring_to_bstring},
    {"%bstring->ustring", prim1, bstring_to_ustring},
    {"%ustring->wstring", prim1, ustring_to_wstring},
    {"%wstring->ustring", prim1, wstring_to_ustring},
#endif
#endif
};

/* Exported functions */

void
marlais_register_string (void)
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

  marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<byte-string>");

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

#ifdef MARLAIS_ENABLE_WCHAR

Object
marlais_make_wstring (const wchar_t *str)
{
    int size;
    wchar_t *new;

    size = wcslen (str);

    new = MARLAIS_ALLOCATE_WSTRING (size + 1);

    wcsncpy (new, str, size);
    new[size] = 0;

    return (make_wstring (new, size));
}

Object
marlais_make_wstring_entrypoint (Object args)
{
  int size, i;
  Object size_obj, fill_obj;
  wchar_t fill = ' ';
  wchar_t *new;

  marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<wide-string>");

  if (fill_obj != MARLAIS_FALSE) {
    if (!WCHARP (fill_obj)) {
      marlais_error ("make: value of fill: must be a <wide-character> for <wide-string> class",
                     fill_obj, NULL);
    }
    fill = WCHARVAL (fill_obj);
  }

  new = MARLAIS_ALLOCATE_WSTRING (size + 1);

  for(i = 0; i < size; i++) {
    new[i] = fill;
  }
  new[size] = 0;

  return (make_wstring (new, size));
}

#endif /* MARLAIS_ENABLE_WCHAR */

#ifdef MARLAIS_ENABLE_UCHAR

Object
marlais_make_ustring (const UChar *str)
{
    int size;
    UChar *new;

    size = u_strlen (str);

    new = MARLAIS_ALLOCATE_USTRING (size + 1);

    // XXX
    //wcsncpy (new, str, size);
    //new[size] = 0;

    return (make_ustring (new, size));
}

Object
marlais_make_ustring_entrypoint (Object args)
{
  int size, i;
  Object size_obj, fill_obj;
  UChar32 fill = ' ';
  UChar *new;

  marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<unicode-string>");

  if (fill_obj != MARLAIS_FALSE) {
    if (!UCHARP (fill_obj)) {
      marlais_error ("make: value of fill: must be a <unicode-character> for <unicode-string> class",
                     fill_obj, NULL);
    }
    fill = UCHARVAL (fill_obj);
  }

  new = MARLAIS_ALLOCATE_USTRING (size + 1);

  for(i = 0; i < size; i++) {
    new[i] = fill;
  }
  new[size] = 0;

  return (make_ustring (new, size));
}

#endif /* MARLAIS_ENABLE_UCHAR */

/* Internal functions */

static Object
make_string(char *str, size_t len) {
  Object obj;

  obj = marlais_allocate_object (ByteString, sizeof (struct byte_string));
  BYTESTRVAL (obj) = str;
  BYTESTRSIZE (obj) = len;

  return obj;
}

#ifdef MARLAIS_ENABLE_WCHAR
static Object
make_wstring(wchar_t *str, size_t len) {
  Object obj;

  obj = marlais_allocate_object (WideString, sizeof (struct wide_string));
  WIDESTRVAL (obj) = str;
  WIDESTRSIZE (obj) = len;

  return obj;
}
#endif

#ifdef MARLAIS_ENABLE_WCHAR
static Object
make_ustring(UChar *str, size_t len) {
  Object obj;

  obj = marlais_allocate_object (UnicodeString, sizeof (struct unicode_string));
  USTRVAL (obj) = str;
  USTRSIZE (obj) = len;

  return obj;
}
#endif

static Object
string_element (Object string, Object index, Object default_ob)
{
    int i;

    i = INTVAL (index);
    if ((i < 0) || (i >= BYTESTRSIZE (string))) {
      if (default_ob == marlais_default) {
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
    return (marlais_unspecified);
}

static Object
string_size (Object string)
{
    return (marlais_make_integer (BYTESTRSIZE (string)));
}

#if 0
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
#endif

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

#ifdef MARLAIS_ENABLE_WCHAR

static Object
wstring_element (Object string, Object index, Object default_ob)
{
    int i;

    i = INTVAL (index);
    if ((i < 0) || (i >= WIDESTRSIZE (string))) {
      if (default_ob == marlais_default) {
        marlais_error ("element: argument out of range", string, index, NULL);
      } else {
        return default_ob;
      }
    }
    return (marlais_make_wchar (WIDESTRVAL (string)[i]));
}

static Object
wstring_element_setter (Object string, Object index, Object val)
{
    int i;

    i = INTVAL (index);
    if ((i < 0) || (i >= WIDESTRSIZE (string))) {
      marlais_error ("element-setter: argument out of range", string, index, NULL);
    }
    WIDESTRVAL (string)[i] = WCHARVAL (val);
    return (marlais_unspecified);
}

static Object
wstring_size (Object string)
{
    return (marlais_make_integer (WIDESTRSIZE (string)));
}

static Object
wstring_lessthan (Object str1, Object str2) {
  return marlais_make_boolean(wcscmp (WIDESTRVAL (str1), WIDESTRVAL (str2)) < 0);
}

static Object
wstring_equal (Object str1, Object str2) {
  return marlais_make_boolean(wcscmp (WIDESTRVAL (str1), WIDESTRVAL (str2)) == 0);
}

/* XXX reimplement using transform */
static Object
wstring_as_lowercase (Object string)
{
    int len, i;
    wchar_t *oldstr, *newstr;

    len = WIDESTRSIZE (string);
    oldstr = WIDESTRVAL (string);
    newstr = MARLAIS_ALLOCATE_WSTRING (len + 1);

    for(i = 0; i < len; i++) {
      newstr[i] = towlower (oldstr[i]);
    }
    newstr[len] = 0;

    return (make_wstring (newstr, len));
}

/* XXX reimplement using transform */
static Object
wstring_as_uppercase (Object string)
{
    int len, i;
    wchar_t *oldstr, *newstr;

    len = WIDESTRSIZE (string);
    oldstr = WIDESTRVAL (string);
    newstr = MARLAIS_ALLOCATE_WSTRING (len + 1);

    for(i = 0; i < len; i++) {
      newstr[i] = towupper (oldstr[i]);
    }
    newstr[len] = 0;

    return (make_wstring (newstr, len));
}

#endif


#ifdef MARLAIS_ENABLE_WCHAR

static Object
ustring_element (Object string, Object index, Object default_ob)
{
    int i;

    i = INTVAL (index);
    if ((i < 0) || (i >= WIDESTRSIZE (string))) {
      if (default_ob == marlais_default) {
        marlais_error ("element: argument out of range", string, index, NULL);
      } else {
        return default_ob;
      }
    }
    return (marlais_make_wchar (WIDESTRVAL (string)[i]));
}

static Object
ustring_element_setter (Object string, Object index, Object val)
{
    int i;

    i = INTVAL (index);
    if ((i < 0) || (i >= WIDESTRSIZE (string))) {
      marlais_error ("element-setter: argument out of range", string, index, NULL);
    }
    WIDESTRVAL (string)[i] = WCHARVAL (val);
    return (marlais_unspecified);
}

static Object
ustring_size (Object string)
{
    return (marlais_make_integer (USTRSIZE (string)));
}

static Object
ustring_lessthan (Object str1, Object str2) {
  return marlais_make_boolean(u_strcmp (USTRVAL (str1), USTRVAL (str2)) < 0);
}

static Object
ustring_equal (Object str1, Object str2) {
  return marlais_make_boolean(u_strcmp (USTRVAL (str1), USTRVAL (str2)) == 0);
}

/* XXX reimplement using transform */
static Object
ustring_as_lowercase (Object string)
{
    int len, i;
    UChar *oldstr, *newstr;

    len = USTRSIZE (string);
    oldstr = USTRVAL (string);
    newstr = MARLAIS_ALLOCATE_USTRING (len + 1);

    for(i = 0; i < len; i++) {
      newstr[i] = u_tolower (oldstr[i]);
    }
    newstr[len] = 0;

    return (make_ustring (newstr, len));
}

/* XXX reimplement using transform */
static Object
ustring_as_uppercase (Object string)
{
    int len, i;
    UChar *oldstr, *newstr;

    len = USTRSIZE (string);
    oldstr = USTRVAL (string);
    newstr = MARLAIS_ALLOCATE_USTRING (len + 1);

    for(i = 0; i < len; i++) {
      newstr[i] = u_toupper (oldstr[i]);
    }
    newstr[len] = 0;

    return (make_ustring (newstr, len));
}

/* XXX reimplement using transform */
static Object
ustring_as_titlecase (Object string)
{
    int len, i;
    UChar *oldstr, *newstr;

    len = USTRSIZE (string);
    oldstr = USTRVAL (string);
    newstr = MARLAIS_ALLOCATE_USTRING (len + 1);

    for(i = 0; i < len; i++) {
      newstr[i] = u_totitle (oldstr[i]);
    }
    newstr[len] = 0;

    return (make_ustring (newstr, len));
}

#endif
