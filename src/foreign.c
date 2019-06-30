/*

   foreign.c - foreign function interface

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

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include "foreign.h"
#include "prim.h"

static Object load_foreign (Object name);

static struct primitive foreign_prims[] =
{
    {"%load_foreign", prim_1, load_foreign},
};

void
init_foreign_prims (void)
{
    int num;

    num = sizeof (foreign_prims) / sizeof (struct primitive);

    init_prims (num, foreign_prims);
}

#ifdef DLOPEN
#include <dlfcn.h>

static Object
load_foreign (Object name)
{
    char *filename;
    void *handle;

    filename = BYTESTRVAL (name);
    handle = dlopen (filename, RTLD_LAZY);
    if (!handle) {
	marlais_error ("load-foreign: could not load file", name, NULL);
    }
    return (marlais_make_integer ((int) handle));
}

#endif /* DLOPEN */

#ifdef powerc
// load a function from a container with code fragment manager.

static Object
load_foreign (Object name)
{
    char *filename;
    void *handle;

    marlais_error ("load-foreign: not implemented yet ", name, NULL);

#if 0
    filename = BYTESTRVAL (name);
    handle = dlopen (filename, RTLD_LAZY);
    if (!handle) {
    }
    return (marlais_make_integer ((int) handle));
#endif

    return unspecified_object;
}

#endif
