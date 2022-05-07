/*

   bytevector.h

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

#ifndef MARLAIS_BYTEVECTOR_H
#define MARLAIS_BYTEVECTOR_H

#include <marlais/common.h>

/* Data structures */

struct byte_vector {
    ObjectHeader header;
    int size;
    uint8_t *els;
};

#define BYTEVSIZE(obj)      (((struct byte_vector *)obj)->size)
#define BYTEVELS(obj)       (((struct byte_vector *)obj)->els)

/* Function declarations */

extern void marlais_register_bytevector (void);

/* Make a <byte-vector> */
extern Object marlais_make_bytevector (int size, uint8_t fill);
/* Entrypoint for make(<byte-vector>) */
extern Object marlais_make_bytevector_entrypoint (Object args);

#endif /* !MARLAIS_BYTEVECTOR_H */
