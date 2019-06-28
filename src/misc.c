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
   0.6 revisions copyright 2001, Douglas M. Auclair. All Rights Reserved.

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

Object
make_eof_object (void)
{
#ifndef SMALL_OBJECTS
    Object obj = marlais_allocate_object (EndOfFile, sizeof (struct object));
    return (obj);
#else
    return (EOFVAL);
#endif
}

Object
make_unspecified_object (void)
{
    Object obj = marlais_allocate_object (Values, sizeof (struct values));

    VALUESNUM (obj) = 0;
    VALUESELS (obj) = NULL;

    return obj;
}

Object
make_uninit_slot (void)
{
#ifndef SMALL_OBJECTS
    Object obj = marlais_allocate_object (UninitializedSlotValue, sizeof (struct object));
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
    Object obj = marlais_allocate_object (Exit, sizeof (struct exitproc));
    EXITSYM (obj) = sym;
    EXITRET (obj) = (jmp_buf *) checking_malloc (sizeof (jmp_buf));
    return (obj);
}

Object
make_unwind (Object body)
{
    Object obj = marlais_allocate_object (Unwind, sizeof (struct unwind));
    UNWINDBODY (obj) = body;
    return (obj);
}
