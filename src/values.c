/*

   values.c

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

#include "values.h"

#include "alloc.h"
#include "error.h"
#include "list.h"
#include "prim.h"

/* primitives */

static struct primitive values_prims[] =
{
  {"values", prim_0_rest, values},
};

/* function definitions */

void
init_values_prims (void)
{
  int num = sizeof (values_prims) / sizeof (struct primitive);
  init_prims (num, values_prims);
}

Object
make_values (Object vals)
{
  Object obj;
  int i;

  if (EMPTYLISTP (vals)) {
    return unspecified_object;
  } else {
    obj = allocate_object (sizeof (struct values));

    VALUESTYPE (obj) = Values;
    VALUESNUM (obj) = list_length (vals);
    VALUESELS (obj) = (Object *)
      checking_malloc (VALUESNUM (obj) * sizeof (Object));

    for (i = 0; i < VALUESNUM (obj); ++i) {
      VALUESELS (obj)[i] = CAR (vals);
      vals = CDR (vals);
    }
    return (obj);
  }
}

Object
construct_values (int num,...)
{
  Object obj;
  int i;
  va_list args;

  obj = allocate_object (sizeof (struct values));
  
  VALUESTYPE (obj) = Values;
  VALUESNUM (obj) = num;
  VALUESELS (obj) = (Object *) checking_malloc (num * sizeof (Object));

  va_start (args, num);
  for (i = 0; i < num; ++i) {
    VALUESELS (obj)[i] = va_arg (args, Object);
  }
  va_end (args);
  return (obj);
}

Object
values (Object rest)
{
  if (EMPTYLISTP (rest) || PAIRP (CDR (rest))) {
    return make_values (rest);
  } else {
    return (CAR (rest));
  }
}

Object
devalue (Object val)
{
  if (VALUESP (val)) {
    if (VALUESNUM (val)) {
      return VALUESELS (val)[0];
    } else {
      return error ("Null values construct used in an invalid context", NULL);
    }
  } else {
    return val;
  }
}
