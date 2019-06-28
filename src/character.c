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

#include "number.h"
#include "prim.h"

static Object integer_to_character (Object i);
static Object character_to_integer (Object ch);

static struct primitive char_prims[] =
{
    {"%integer->character", prim_1, integer_to_character},
    {"%character->integer", prim_1, character_to_integer},
};

void
init_char_prims (void)
{
    int num;

    num = sizeof (char_prims) / sizeof (struct primitive);

    init_prims (num, char_prims);
}

#ifndef SMALL_OBJECTS
Object
make_character (char ch)
{
    Object obj;

    obj = allocate_object (sizeof (struct object));

    TYPE (obj) = Character;
    CHARVAL (obj) = ch;
    return (obj);
}
#else
Object
make_character (char ch)
{
    return (MAKE_CHAR (ch));
}
#endif

static Object
integer_to_character (Object i)
{
    return (make_character (INTVAL (i)));
}

static Object
character_to_integer (Object ch)
{
    return (make_integer (CHARVAL (ch)));
}
