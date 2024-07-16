/*

   float.h

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

#ifndef MARLAIS_FLOAT_H
#define MARLAIS_FLOAT_H

#include <marlais/common.h>

/* Types */

typedef float marlais_sfloat_t;
typedef double marlais_dfloat_t;
typedef long double marlais_efloat_t;

/* Data structures */

struct marlais_sfloat {
    marlais_header_t header;
    marlais_sfloat_t sfloat_value;
};

#define SFLOATVAL(obj)    (((struct marlais_sfloat *)obj)->sfloat_value)

struct marlais_dfloat {
    marlais_header_t header;
    marlais_dfloat_t dfloat_value;
};

#define DFLOATVAL(obj)    (((struct marlais_dfloat *)obj)->dfloat_value)

struct marlais_efloat {
    marlais_header_t header;
    marlais_efloat_t efloat_value;
};

#define EFLOATVAL(obj)    (((struct marlais_efloat *)obj)->efloat_value)

/* Function declarations */

/* Register number primitives */
extern void marlais_register_float (void);
/* Make a <single-float> */
extern Object marlais_make_sfloat (marlais_sfloat_t value);
/* Make a <double-float> */
extern Object marlais_make_dfloat (marlais_dfloat_t value);
#ifdef MARLAIS_ENABLE_EFLOAT
/* Make an <extended-float> */
extern Object marlais_make_efloat (marlais_efloat_t value);
#endif

#endif
