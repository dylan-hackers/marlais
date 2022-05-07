/*

   string.h

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

#ifndef MARLAIS_STRING_H
#define MARLAIS_STRING_H

#include <marlais/common.h>

/* Data structures */

struct byte_string {
    ObjectHeader header;
    int size;
    char *val;
};

#define BYTESTRSIZE(obj)  (((struct byte_string *)obj)->size)
#define BYTESTRVAL(obj)   (((struct byte_string *)obj)->val)

#ifdef MARLAIS_ENABLE_WCHAR
struct wide_string {
    ObjectHeader header;
    int size;
    wchar_t *val;
};

#define WIDESTRSIZE(obj)  (((struct wide_string *)obj)->size)
#define WIDESTRVAL(obj)   (((struct wide_string *)obj)->val)
#endif

#ifdef MARLAIS_ENABLE_UCHAR
struct unicode_string {
    ObjectHeader header;
    int size;
    UChar *val;
};

#define USTRSIZE(obj)  (((struct unicode_string *)obj)->size)
#define USTRVAL(obj)   (((struct unicode_string *)obj)->val)
#endif

/* Function declarations */

/* allocate a char string */
#define MARLAIS_ALLOCATE_STRING(_size) \
  ((char *)marlais_allocate_atomic(_size))

#ifdef MARLAIS_ENABLE_WCHAR
/* allocate a wchar_t string */
#define MARLAIS_ALLOCATE_WSTRING(_size) \
  ((wchar_t *)marlais_allocate_atomic((_size) * sizeof(wchar_t)))
#endif

#ifdef MARLAIS_ENABLE_UCHAR
/* allocate a UChar string */
#define MARLAIS_ALLOCATE_USTRING(_size) \
  ((UChar *)marlais_allocate_atomic((_size) * sizeof(UChar)))
#endif

/* Register string primitives */
extern void marlais_register_string (void);

/* Make a <byte-string> */
extern Object marlais_make_bytestring (const char *str);
/* Entrypoint for make(<byte-string>) */
extern Object marlais_make_bytestring_entrypoint (Object args);

#ifdef MARLAIS_ENABLE_WCHAR
/* Make a <wide-string> */
extern Object marlais_make_wstring (const wchar_t *str);
/* Entrypoint for make(<wide-string>) */
extern Object marlais_make_wstring_entrypoint (Object args);
#endif

#ifdef MARLAIS_ENABLE_UCHAR
/* Make a <unicode-string> */
extern Object marlais_make_ustring (const UChar *str);
/* Entrypoint for make(<unicode-string>) */
extern Object marlais_make_ustring_entrypoint (Object args);
#endif

#endif
