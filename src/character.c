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

/* Internal variables */

#ifndef MARLAIS_OBJECT_MODEL_SMALL
#if MARLAIS_CONFIG_CHARACTER_CACHE > 0
static Object character_cache[MARLAIS_CONFIG_CHARACTER_CACHE];
#endif
#endif

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
  //{"%character-octal?", prim_1, character_octal_p},
  {"%character-lowercase?", prim_1, character_lowercase_p},
  {"%character-uppercase?", prim_1, character_uppercase_p},
};

/* Exported functions */

void
marlais_register_character (void)
{
  int num = sizeof (char_prims) / sizeof (struct primitive);
  marlais_register_prims (num, char_prims);
}

#ifndef MARLAIS_OBJECT_MODEL_SMALL
/* small version is inline in marlais/character.h */
Object
marlais_make_character (char ch)
{
  Object obj;

#if MARLAIS_CONFIG_CHARACTER_CACHE > 0
  if(ch >= 0 && ch < MARLAIS_CONFIG_CHARACTER_CACHE) {
    if(character_cache[ch] != NULL) {
      return character_cache[ch];
    }
  }
#endif

  obj = marlais_allocate_object (Character, sizeof (struct character));
  CHARVAL (obj) = ch;

#if MARLAIS_CONFIG_CHARACTER_CACHE > 0
  if(ch >= 0 && ch < MARLAIS_CONFIG_CHARACTER_CACHE) {
    character_cache[ch] = obj;
  }
#endif

  return (obj);
}
#endif

/* Internal functions */

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

#define DEFINE_CTYPE_PREDICATE(_NAME,_PRED)		\
  static Object character_ ## _NAME ## _p (Object ch) { \
    return (marlais_make_boolean(_PRED(CHARVAL(ch))));	\
  }
DEFINE_CTYPE_PREDICATE(alphabetic,   isalpha);
DEFINE_CTYPE_PREDICATE(alphanumeric, isalnum);
DEFINE_CTYPE_PREDICATE(control,      iscntrl);
DEFINE_CTYPE_PREDICATE(graphic,      isgraph);
DEFINE_CTYPE_PREDICATE(printable,    isprint);
DEFINE_CTYPE_PREDICATE(whitespace,   isspace);
DEFINE_CTYPE_PREDICATE(decimal,      isdigit);
DEFINE_CTYPE_PREDICATE(hexadecimal,  isxdigit);
//DEFINE_CTYPE_PREDICATE(octal,        isdigit); /* XXX wrong */
DEFINE_CTYPE_PREDICATE(lowercase,    islower);
DEFINE_CTYPE_PREDICATE(uppercase,    isupper);
#undef DEFINE_CTYPE_PREDICATE
