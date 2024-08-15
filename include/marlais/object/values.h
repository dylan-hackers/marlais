/*

   values.h

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

#ifndef MARLAIS_VALUES_H
#define MARLAIS_VALUES_H

#include <marlais/common.h>

struct marlais_values {
    marlais_header_t header;
    int     values_size;
    Object *values_elts;
};

#define VALUESNUM(obj)    (((struct marlais_values *)obj)->values_size)
#define VALUESELS(obj)    (((struct marlais_values *)obj)->values_elts)


#define FIRSTVALP(vals)  (VALUESNUM(vals)>0)
#define SECONDVALP(vals) (VALUESNUM(vals)>1)
#define THIRDVALP(vals)  (VALUESNUM(vals)>2)
#define FOURTHVALP(vals) (VALUESNUM(vals)>3)
#define FIFTHVALP(vals)  (VALUESNUM(vals)>4)

#define FIRSTVAL(vals)  (VALUESELS(vals)[0])
#define SECONDVAL(vals) (VALUESELS(vals)[1])
#define THIRDVAL(vals)  (VALUESELS(vals)[2])
#define FOURTHVAL(vals) (VALUESELS(vals)[3])
#define FIFTHVAL(vals)  (VALUESELS(vals)[4])

/* Iterate multiple values */
#define MARLAIS_VALUES_FOREACH(_values,_idxvar,_valvar)                 \
  for(_idxvar = 0,                                                      \
        _valvar = (marlais_is_values_p((_values))                                   \
                   ? VALUESELS((_values))[_idxvar]                      \
                   : (_values));                                        \
      (marlais_is_values_p((_values)) && _idxvar < VALUESNUM((_values)))            \
        ||((!marlais_is_values_p((_values))) && _idxvar < 1);                       \
      _idxvar++,                                                        \
        _valvar=marlais_is_values_p((_values))                                      \
        ? VALUESELS((_values))[_idxvar]                                 \
        : MARLAIS_UNSPECIFIED)

/* Register vector primitives */
extern void marlais_register_values (void);

/* Allocate values object */
extern Object marlais_values_alloc (int num);
/* Make values from arguments */
extern Object marlais_values_args (int num,...);
/* Make values from list */
extern Object marlais_values_list (Object vals);
/* Make values (primitive/rest form) */
extern Object marlais_values (Object rest);
/* Reduce to single value */
extern Object marlais_devalue (Object val);

#endif
