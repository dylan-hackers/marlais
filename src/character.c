/*

   character.c

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

#include <marlais/character.h>

#include <marlais/alloc.h>
#include <marlais/number.h>
#include <marlais/prim.h>

#include <ctype.h>

#ifdef MARLAIS_ENABLE_WCHAR
#include <wctype.h>
#endif

/* Internal variables */

#ifdef MARLAIS_OBJECT_MODEL_LARGE

#if MARLAIS_CONFIG_CHARACTER_CACHE > 0
static Object character_cache[MARLAIS_CONFIG_CHARACTER_CACHE];
#endif

#ifdef MARLAIS_ENABLE_WCHAR
#if MARLAIS_CONFIG_WCHAR_CACHE > 0
static Object wchar_cache[MARLAIS_CONFIG_WCHAR_CACHE];
#endif
#endif

#ifdef MARLAIS_ENABLE_UCHAR
#if MARLAIS_CONFIG_UCHAR_CACHE > 0
static Object uchar_cache[MARLAIS_CONFIG_UCHAR_CACHE];
#endif
#endif

#endif /* MARLAIS_OBJECT_MODEL_LARGE */

/* Primitives */

static Object integer_to_character (Object i);
static Object character_to_integer (Object ch);
static Object character_to_lowercase (Object ch);
static Object character_to_uppercase (Object ch);
static Object character_alphabetic_p (Object ch);
static Object character_alphanumeric_p (Object ch);
static Object character_control_p (Object ch);
static Object character_graphic_p (Object ch);
static Object character_printable_p (Object ch);
static Object character_whitespace_p (Object ch);
static Object character_decimal_p (Object ch);
static Object character_hexadecimal_p (Object ch);
static Object character_octal_p (Object ch);
static Object character_lowercase_p (Object ch);
static Object character_uppercase_p (Object ch);

#ifdef MARLAIS_ENABLE_WCHAR
static Object integer_to_wchar (Object i);
static Object wchar_to_integer (Object ch);
static Object wchar_to_lowercase (Object ch);
static Object wchar_to_uppercase (Object ch);
static Object wchar_alphabetic_p (Object ch);
static Object wchar_alphanumeric_p (Object ch);
static Object wchar_control_p (Object ch);
static Object wchar_graphic_p (Object ch);
static Object wchar_printable_p (Object ch);
static Object wchar_whitespace_p (Object ch);
static Object wchar_decimal_p (Object ch);
static Object wchar_hexadecimal_p (Object ch);
static Object wchar_octal_p (Object ch);
static Object wchar_lowercase_p (Object ch);
static Object wchar_uppercase_p (Object ch);
#endif

#ifdef MARLAIS_ENABLE_UCHAR
static Object integer_to_uchar (Object i);
static Object uchar_to_integer (Object ch);
static Object uchar_to_lowercase (Object ch);
static Object uchar_to_uppercase (Object ch);
static Object uchar_alphabetic_p (Object ch);
static Object uchar_alphanumeric_p (Object ch);
static Object uchar_control_p (Object ch);
static Object uchar_graphic_p (Object ch);
static Object uchar_printable_p (Object ch);
static Object uchar_whitespace_p (Object ch);
static Object uchar_decimal_p (Object ch);
static Object uchar_hexadecimal_p (Object ch);
static Object uchar_octal_p (Object ch);
static Object uchar_lowercase_p (Object ch);
static Object uchar_uppercase_p (Object ch);
#endif

static struct primitive char_prims[] =
{
  {"%integer->character", prim_1, integer_to_character},
  {"%character->integer", prim_1, character_to_integer},
  {"%character-to-lowercase", prim_1, character_to_lowercase},
  {"%character-to-uppercase", prim_1, character_to_uppercase},
  {"%character-alphabetic?", prim_1, character_alphabetic_p},
  {"%character-alphanumeric?", prim_1, character_alphanumeric_p},
  {"%character-control?", prim_1, character_control_p},
  {"%character-graphic?", prim_1, character_graphic_p},
  {"%character-printable?", prim_1, character_printable_p},
  {"%character-whitespace?", prim_1, character_whitespace_p},
  {"%character-decimal?", prim_1, character_decimal_p},
  {"%character-hexadecimal?", prim_1, character_hexadecimal_p},
/*{"%character-octal?", prim_1, character_octal_p},*/
  {"%character-lowercase?", prim_1, character_lowercase_p},
  {"%character-uppercase?", prim_1, character_uppercase_p},

#ifdef MARLAIS_ENABLE_WCHAR
  {"%integer->wchar", prim_1, integer_to_wchar},
  {"%wchar->integer", prim_1, wchar_to_integer},
  {"%wchar-to-lowercase", prim_1, wchar_to_lowercase},
  {"%wchar-to-uppercase", prim_1, wchar_to_uppercase},
  {"%wchar-alphabetic?", prim_1, wchar_alphabetic_p},
  {"%wchar-alphanumeric?", prim_1, wchar_alphanumeric_p},
  {"%wchar-control?", prim_1, wchar_control_p},
  {"%wchar-graphic?", prim_1, wchar_graphic_p},
  {"%wchar-printable?", prim_1, wchar_printable_p},
  {"%wchar-whitespace?", prim_1, wchar_whitespace_p},
  {"%wchar-decimal?", prim_1, wchar_decimal_p},
  {"%wchar-hexadecimal?", prim_1, wchar_hexadecimal_p},
/*{"%wchar-octal?", prim_1, wchar_octal_p},*/
  {"%wchar-lowercase?", prim_1, wchar_lowercase_p},
  {"%wchar-uppercase?", prim_1, wchar_uppercase_p},
#endif

#ifdef MARLAIS_ENABLE_UCHAR
  {"%integer->uchar", prim_1, integer_to_uchar},
  {"%uchar->integer", prim_1, uchar_to_integer},
  {"%uchar-to-lowercase", prim_1, uchar_to_lowercase},
  {"%uchar-to-uppercase", prim_1, uchar_to_uppercase},
  {"%uchar-alphabetic?", prim_1, uchar_alphabetic_p},
  {"%uchar-alphanumeric?", prim_1, uchar_alphanumeric_p},
  {"%uchar-control?", prim_1, uchar_control_p},
  {"%uchar-graphic?", prim_1, uchar_graphic_p},
  {"%uchar-printable?", prim_1, uchar_printable_p},
  {"%uchar-whitespace?", prim_1, uchar_whitespace_p},
  {"%uchar-decimal?", prim_1, uchar_decimal_p},
  {"%uchar-hexadecimal?", prim_1, uchar_hexadecimal_p},
/*{"%uchar-octal?", prim_1, uchar_octal_p},*/
  {"%uchar-lowercase?", prim_1, uchar_lowercase_p},
  {"%uchar-uppercase?", prim_1, uchar_uppercase_p},
#endif

};

/* Exported functions */

void
marlais_register_character (void)
{
  int num = sizeof (char_prims) / sizeof (struct primitive);
  marlais_register_prims (num, char_prims);
}

#ifdef MARLAIS_OBJECT_MODEL_LARGE
/* small version is inline in marlais/character.h */
Object
marlais_make_character (char ch)
{
  Object obj;

#if MARLAIS_CONFIG_CHARACTER_CACHE > 0
  unsigned int uc = (unsigned int)ch;
  if(uc < MARLAIS_CONFIG_CHARACTER_CACHE) {
    obj = character_cache[uc];
    if(obj != NULL) {
      return obj;
    }
  }
#endif

  obj = marlais_allocate_object (Character, sizeof (struct character));
  CHARVAL (obj) = ch;

#if MARLAIS_CONFIG_CHARACTER_CACHE > 0
  if(uc < MARLAIS_CONFIG_CHARACTER_CACHE) {
    character_cache[uc] = obj;
  }
#endif

  return (obj);
}
#endif


#ifdef MARLAIS_OBJECT_MODEL_LARGE
#ifdef MARLAIS_ENABLE_WCHAR
/* small version is inline in marlais/character.h */
Object
marlais_make_wchar (wchar_t ch)
{
  Object obj;

#if MARLAIS_CONFIG_WCHAR_CACHE > 0
  unsigned int uc = (unsigned int)ch;
  if(uc < MARLAIS_CONFIG_WCHAR_CACHE) {
    obj = wchar_cache[uc];
    if(obj != NULL) {
      return obj;
    }
  }
#endif

  obj = marlais_allocate_object (WideCharacter, sizeof (struct wide_character));
  WCHARVAL (obj) = ch;

#if MARLAIS_CONFIG_WCHAR_CACHE > 0
  if(uc < MARLAIS_CONFIG_WCHAR_CACHE) {
    wchar_cache[uc] = obj;
  }
#endif

  return (obj);
}
#endif
#endif

/* Primitives on <byte-character> */

static Object
integer_to_character (Object i)
{
  return (marlais_make_character (INTVAL (i)));
}

static Object
character_to_integer (Object ch)
{
  return (marlais_make_integer (CHARVAL (ch)));
}

static Object
character_to_lowercase (Object ch)
{
  return (marlais_make_character (tolower(CHARVAL(ch))));
}

static Object
character_to_uppercase (Object ch)
{
  return (marlais_make_character (toupper(CHARVAL(ch))));
}

#define DEFINE_CTYPE_PREDICATE(_NAME,_PRED)             \
  static Object character_ ## _NAME ## _p (Object ch) { \
    return (marlais_make_boolean(_PRED(CHARVAL(ch))));  \
  }
DEFINE_CTYPE_PREDICATE(alphabetic,   isalpha);
DEFINE_CTYPE_PREDICATE(alphanumeric, isalnum);
DEFINE_CTYPE_PREDICATE(control,      iscntrl);
DEFINE_CTYPE_PREDICATE(graphic,      isgraph);
DEFINE_CTYPE_PREDICATE(printable,    isprint);
DEFINE_CTYPE_PREDICATE(whitespace,   isspace);
DEFINE_CTYPE_PREDICATE(decimal,      isdigit);
DEFINE_CTYPE_PREDICATE(hexadecimal,  isxdigit);
//DEFINE_CTYPE_PREDICATE(octal,        isdigit); /* XXX not implemented */
DEFINE_CTYPE_PREDICATE(lowercase,    islower);
DEFINE_CTYPE_PREDICATE(uppercase,    isupper);
#undef DEFINE_CTYPE_PREDICATE

/* Primitives on <wide-character> */

#ifdef MARLAIS_ENABLE_WCHAR

static Object
integer_to_wchar (Object i)
{
  return (marlais_make_wchar (INTVAL (i)));
}

static Object
wchar_to_integer (Object ch)
{
  return (marlais_make_integer (WCHARVAL (ch)));
}

static Object
wchar_to_lowercase (Object ch)
{
  return (marlais_make_wchar (towlower(WCHARVAL(ch))));
}

static Object
wchar_to_uppercase (Object ch)
{
  return (marlais_make_wchar (towupper(WCHARVAL(ch))));
}

#define DEFINE_WCTYPE_PREDICATE(_NAME,_PRED)            \
  static Object wchar_ ## _NAME ## _p (Object ch) {     \
    return (marlais_make_boolean(_PRED(CHARVAL(ch))));  \
  }
DEFINE_WCTYPE_PREDICATE(alphabetic,   iswalpha);
DEFINE_WCTYPE_PREDICATE(alphanumeric, iswalnum);
DEFINE_WCTYPE_PREDICATE(control,      iswcntrl);
DEFINE_WCTYPE_PREDICATE(graphic,      iswgraph);
DEFINE_WCTYPE_PREDICATE(printable,    iswprint);
DEFINE_WCTYPE_PREDICATE(whitespace,   iswspace);
DEFINE_WCTYPE_PREDICATE(decimal,      iswdigit);
DEFINE_WCTYPE_PREDICATE(hexadecimal,  iswxdigit);
//DEFINE_WCTYPE_PREDICATE(octal,      iswodigit); /* XXX not implemented */
DEFINE_WCTYPE_PREDICATE(lowercase,    iswlower);
DEFINE_WCTYPE_PREDICATE(uppercase,    iswupper);
#undef DEFINE_WCTYPE_PREDICATE

#endif /* MARLAIS_ENABLE_WCHAR */
