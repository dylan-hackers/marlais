/*

   boolean.c

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

#include <marlais/boolean.h>

#include <marlais/alloc.h>
#include <marlais/prim.h>

/* Primitives */

static Object prim_not (Object obj);
static Object prim_identical_p (Object obj1, Object obj2, Object rest);

static struct primitive boolean_prims[] =
{
  {"%not", prim_1, prim_not},
  {"%identical?", prim_2_rest, prim_identical_p},
};

/* Exported functions */

void
marlais_register_boolean (void)
{
  int num = sizeof (boolean_prims) / sizeof (struct primitive);
  marlais_register_prims (num, boolean_prims);
}

bool
marlais_identical_p (Object obj1, Object obj2)
{
  if (obj1 == obj2) {
    return true;
  } else if (SFLOATP (obj1) && SFLOATP (obj2)) {
    return (SFLOATVAL (obj1) == SFLOATVAL (obj2));
  } else if (DFLOATP (obj1) && DFLOATP (obj2)) {
    return (DFLOATVAL (obj1) == DFLOATVAL (obj2));

#ifdef MARLAIS_OBJECT_MODEL_LARGE

  } else if (INTEGERP (obj1) && INTEGERP (obj2)) {
    return (INTVAL (obj1) == INTVAL (obj2));
  } else if (CHARP (obj1) && CHARP (obj2)) {
    return (CHARVAL (obj1) == CHARVAL (obj2));

#ifdef MARLAIS_ENABLE_WCHAR
  } else if (WCHARP (obj1) && WCHARP (obj2)) {
    return (WCHARVAL (obj1) == WCHARVAL (obj2));
#endif

#ifdef MARLAIS_ENABLE_UCHAR
  } else if (UCHARP (obj1) && UCHARP (obj2)) {
    return (UCHARVAL (obj1) == UCHARVAL (obj2));
#endif

#endif /* MARLAIS_OBJECT_MODEL_LARGE */

  } else {
    return false;
  }
}

/* Primitives */

static Object
prim_not (Object obj)
{
  if (obj == MARLAIS_FALSE) {
    return (MARLAIS_TRUE);
  } else {
    return (MARLAIS_FALSE);
  }
}

static Object
prim_identical_p (Object obj1, Object obj2, Object rest)
{
  /* check fixed arguments first */
  if(!marlais_identical_p(obj1, obj2)) {
    return MARLAIS_FALSE;
  }

  /* iterate through the rest */
  while (!EMPTYLISTP (rest)) {
    obj1 = obj2;
    obj2 = CAR(rest);
    rest = CDR(rest);

    if(!marlais_identical_p(obj1, obj2)) {
      return MARLAIS_FALSE;
    }
  }

  /* if we get here they are all identical */
  return MARLAIS_TRUE;
}
