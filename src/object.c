/*

   object.c

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

#include "object.h"

#include "error.h"
#include "number.h"

#ifdef SMALL_OBJECTS

enum objtype
object_type (Object obj)
{
    if (POINTERP (obj)) {
	return (PAIRTYPE (obj));
    } else if (IMMEDP (obj)) {
	switch (SUBPART (obj)) {
	case TRUESUB:
	    return (True);
	case FALSESUB:
	    return (False);
	case EMPTYSUB:
	    return (EmptyList);
	case CHARSUB:
	    return (Character);
	case EOFSUB:
	    return (EndOfFile);
	case UNSPECSUB:
	    return (Unspecified);
	case UNINITSUB:
	    return (UninitializedSlotValue);
	default:
	    error ("object with unknown immediate tag",
		   make_integer (SUBPART (obj)),
		   0);
	}
    } else {
	return (Integer);
    }
}

#endif

Object
make_handle (Object an_object)
{
    Object new_handle;

    new_handle = allocate_object (sizeof (struct object_handle));

    HDLTYPE (new_handle) = ObjectHandle;
    HDLOBJ (new_handle) = an_object;
    return (new_handle);
}
