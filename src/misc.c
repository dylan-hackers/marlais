/*

   misc.c

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

#include "misc.h"

#include "alloc.h"
#include "prim.h"

static Object quit (void);

static struct primitive misc_prims[] =
{
    {"quit", prim_0, quit},
    {"bye", prim_0, quit},
};

void
init_misc_prims (void)
{
    int num;

    num = sizeof (misc_prims) / sizeof (struct primitive);

    init_prims (num, misc_prims);
}

Object
make_eof_object (void)
{
#ifndef SMALL_OBJECTS
    Object obj;

    obj = allocate_object (sizeof (struct object));

    TYPE (obj) = EndOfFile;
    return (obj);
#else
    return (EOFVAL);
#endif
}

Object
make_unspecified_object (void)
{
    Object obj;


    obj = allocate_object (sizeof (struct values));

    VALUESTYPE (obj) = Values;
    VALUESNUM (obj) = 0;
    VALUESELS (obj) = NULL;

    return obj;

#if 0
#ifndef SMALL_OBJECTS
    Object obj;
    obj = allocate_object (sizeof (struct object));

    TYPE (obj) = Unspecified;
    return (obj);
#else
    return (UNSPECVAL);
#endif
#endif
}

Object
make_uninit_slot (void)
{
#ifndef SMALL_OBJECTS
    Object obj;

    obj = allocate_object (sizeof (struct object));

    TYPE (obj) = UninitializedSlotValue;
    return (obj);
#else
    return (UNINITVAL);
#endif
}

/* jump buffer must be filled in by
   calling routine. */
Object
make_exit (Object sym)
{
    Object obj;

    obj = allocate_object (sizeof (struct exitproc));

    EXITTYPE (obj) = Exit;
    EXITSYM (obj) = sym;
    EXITRET (obj) = (jmp_buf *) checking_malloc (sizeof (jmp_buf));
    return (obj);
}

Object
make_unwind (Object body)
{
    Object obj;

    obj = allocate_object (sizeof (struct unwind));

    UNWINDTYPE (obj) = Unwind;
    UNWINDBODY (obj) = body;
    return (obj);
}

static Object
quit (void)
{
    exit (0);
}
