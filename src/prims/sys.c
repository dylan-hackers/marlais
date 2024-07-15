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

#include <marlais/sys.h>

#include <marlais/prim.h>
#include <marlais/values.h>

#include <time.h>

/* XXX this must be fixed */
#define POSITIVE_SMALL_INT_MASK 01777777777

static struct primitive sys_prims[] =
{
    {"ctime", prim_0, marlais_get_ctime},
    {"time", prim_0, marlais_get_time},
    {"clock", prim_0, marlais_get_clock},
    {"system", prim_1, marlais_user_system},
};

void
marlais_register_sys (void)
{
  MARLAIS_REGISTER_PRIMS (sys_prims);
}

Object
marlais_get_ctime ()
{
    time_t time_loc;

    (void) time (&time_loc);
    return marlais_make_bytestring (ctime (&time_loc));
}

Object
marlais_get_time ()
{
    time_t time_loc;

    (void) time (&time_loc);

#ifndef MARLAIS_OBJECT_MODEL_SMALL
    time_loc = abs ((int) time_loc);
#else
    time_loc &= POSITIVE_SMALL_INT_MASK;
#endif

    return marlais_make_integer (time_loc);
}

Object
marlais_get_clock ()
{
    clock_t clock_val;

    clock_val = clock ();

#ifndef MARLAIS_OBJECT_MODEL_SMALL
    clock_val = abs ((int) clock_val);
#else
    clock_val = abs ((int) (clock_val << 2)) >> 2;
#endif

    return marlais_make_integer (clock_val);
}

Object
marlais_user_system (Object string)
{
    if (!BYTESTRP (string)) {
      return marlais_error ("system: argument must be a string", string, NULL);
    } else {
      return marlais_make_integer (system (BYTESTRVAL (string)));
    }
}
