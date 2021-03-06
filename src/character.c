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

#include "character.h"

#include "alloc.h"
#include "number.h"
#include "prim.h"

/* Primitives */

static Object integer_to_character (Object i);
static Object character_to_integer (Object ch);

static struct primitive char_prims[] =
{
    {"%integer->character", prim_1, integer_to_character},
    {"%character->integer", prim_1, character_to_integer},
};

/* Exported functions */

void
marlais_register_character (void)
{
    int num = sizeof (char_prims) / sizeof (struct primitive);
    init_prims (num, char_prims);
}

Object
marlais_make_character (char ch)
{
#ifdef SMALL_OBJECTS
    return (MAKE_CHAR (ch));
#else
    Object obj;
    obj = marlais_allocate_object (Character, sizeof (struct object));
    CHARVAL (obj) = ch;
    return (obj);
#endif
}

/* Static functions */

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
