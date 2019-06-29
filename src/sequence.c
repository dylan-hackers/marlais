/*
   sequence.c

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
   0.6 Revisions Copyright 2001, Douglas M. Auclair.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

/* general operations that all sequences (vectors, deques, etc) use */ 

#include "sequence.h"
#include "globals.h"
#include "error.h"

void make_sequence_driver(Object args, 
	int* size, Object* size_obj, Object* fill_obj,
	const char* type)
{
  *size = 0;
  *size_obj = NULL;
  *fill_obj = false_object;

  while (!EMPTYLISTP (args)) {
    if (FIRST (args) == size_keyword) {
      *size_obj = SECOND (args);
    } else if (FIRST (args) == fill_keyword) {
      *fill_obj = SECOND (args);
    } else {
      char err_msg[80];
      sprintf(err_msg, "make: unsupported keyword for %s class", type);
      error (err_msg, FIRST (args), NULL);
    }
    args = CDR (CDR (args));
  }
  if (*size_obj) {
    if (!INTEGERP (*size_obj)) {
      error ("make: value of size: argument must be an integer",
	     size_obj, NULL);
    }
    *size = INTVAL (*size_obj);
  }
}
