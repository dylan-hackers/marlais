/*

   sys.c

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

   Copyright, 1994, Joseph N. Wilson.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#if defined(MACOS) || defined (_WIN32)
#include <time.h>


#else
#include <sys/time.h>
#endif

#include "sys.h"

#include "bytestring.h"
#include "error.h"
#include "number.h"
#include "prim.h"
#include "values.h"

#define POSITIVE_SMALL_INT_MASK	01777777777

static struct primitive sys_prims[] =
{
    {"ctime", prim_0, get_ctime},
    {"time", prim_0, get_time},
    {"clock", prim_0, get_clock},
    {"system", prim_1, user_system},
};

void
init_sys_prims (void)
{
    int num;

    num = sizeof (sys_prims) / sizeof (struct primitive);

    init_prims (num, sys_prims);
}

Object
get_ctime ()
{
    time_t time_loc;

    (void) time (&time_loc);
    return make_byte_string (ctime (&time_loc));
}

Object
get_time ()
{
    time_t time_loc;

    (void) time (&time_loc);

#ifndef SMALL_OBJECTS
    time_loc = abs ((int) time_loc);
#else
    time_loc &= POSITIVE_SMALL_INT_MASK;
#endif

    return make_integer (time_loc);
}

Object
get_clock ()
{
    clock_t clock_val;

    clock_val = clock ();

#ifndef SMALL_OBJECTS
    clock_val = abs ((int) clock_val);
#else
    clock_val = abs ((int) (clock_val << 2)) >> 2;
#endif

    return make_integer (clock_val);
}

Object
user_system (Object string)
{
    if (!BYTESTRTYPE (string)) {
	return error ("system: argument must be a string", string, NULL);
    } else {
	return make_integer (system (BYTESTRVAL (string)));

    }
}
