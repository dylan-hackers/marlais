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

#include <marlais/object/values.h>

/* Exported functions */

Object
marlais_values_alloc (int num)
{
  Object obj = marlais_allocate_object(Values, sizeof(struct marlais_values) + (num * sizeof(Object)));
  VALUESNUM(obj) = num;
  VALUESELS(obj) = (Object*)(((void*)obj)+sizeof(struct marlais_values));
  return obj;
}

Object
marlais_values_args (int num,...)
{
  va_list args;
  Object obj;
  int i;

  if(num == 0) {
    obj = MARLAIS_UNSPECIFIED;
#if 0
  } else if(num == 1) {
    va_start(args,num);
    obj = va_arg(args,Object);
    va_end(args);
#endif
  } else {
    obj = marlais_values_alloc(num);
    va_start(args, num);
    for (i = 0; i < num; ++i) {
      VALUESELS(obj)[i] = va_arg(args, Object);
    }
    va_end(args);
  }

  return(obj);
}

Object
marlais_values_list (Object vals)
{
  Object obj;
  int num, i;

  num = marlais_list_length(vals);

  if(EMPTYLISTP(vals)) {
    obj = MARLAIS_UNSPECIFIED;
#if 0
  } else if(num == 1) {
    obj = CAR(vals);
#endif
  } else {
    obj = marlais_values_alloc(num);
    for (i = 0; i < VALUESNUM(obj); ++i) {
      VALUESELS(obj)[i] = CAR(vals);
      vals = CDR(vals);
    }
  }

  return(obj);
}

Object
marlais_values (Object rest)
{
  if (EMPTYLISTP(rest)) {
    return MARLAIS_UNSPECIFIED;
  } else if (PAIRP(CDR(rest))) {
    return marlais_values_list(rest);
  } else {
    return (CAR(rest));
  }
}

Object
marlais_devalue (Object val)
{
  if (VALUESP(val)) {
    if (VALUESNUM(val)) {
      return VALUESELS(val)[0];
    } else {
      return marlais_error ("Null values construct used in an invalid context", NULL);
    }
  } else {
    return val;
  }
}
