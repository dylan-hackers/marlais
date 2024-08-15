/*

   compare.c

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

#include <marlais/core/compare.h>

/* Exported functions */

bool
marlais_identical_p (Object obj1, Object obj2)
{
  if (obj1 == obj2) {
    return true;
  } else if (marlais_is_sfloat_p (obj1) && marlais_is_sfloat_p (obj2)) {
    return (SFLOATVAL (obj1) == SFLOATVAL (obj2));
  } else if (marlais_is_dfloat_p (obj1) && marlais_is_dfloat_p (obj2)) {
    return (DFLOATVAL (obj1) == DFLOATVAL (obj2));

#ifdef MARLAIS_ENABLE_EFLOAT

  } else if (marlais_is_efloat_p (obj1) && marlais_is_efloat_p (obj2)) {
    return (EFLOATVAL (obj1) == EFLOATVAL (obj2));

#endif /* MARLAIS_ENABLE_EFLOAT */

#ifdef MARLAIS_OBJECT_MODEL_BOXED

  } else if (marlais_is_integer_p (obj1) && marlais_is_integer_p (obj2)) {
    return (marlais_get_int (obj1) == marlais_get_int (obj2));
  } else if (marlais_is_bchar_p (obj1) && marlais_is_bchar_p (obj2)) {
    return (CHARVAL (obj1) == CHARVAL (obj2));

#ifdef MARLAIS_ENABLE_WCHAR
  } else if (marlais_is_wchar_p (obj1) && marlais_is_wchar_p (obj2)) {
    return (WCHARVAL (obj1) == WCHARVAL (obj2));
#endif /* MARLAIS_ENABLE_WCHAR */

#ifdef MARLAIS_ENABLE_UCHAR
  } else if (marlais_is_uchar_p (obj1) && marlais_is_uchar_p (obj2)) {
    return (UCHARVAL (obj1) == UCHARVAL (obj2));
#endif /* MARLAIS_ENABLE_UCHAR */

#endif /* MARLAIS_OBJECT_MODEL_BOXED */

  } else {
    return false;
  }
}
