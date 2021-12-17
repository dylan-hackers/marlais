/*

   function.h

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

#ifndef MARLAIS_FUNCTION_H
#define MARLAIS_FUNCTION_H

#include <marlais/common.h>

extern void marlais_register_function (void);

extern Object marlais_function_specializers (Object meth);
extern Object marlais_generic_methods (Object generic);

extern Object marlais_make_generic (Object name, Object params, Object methods);
extern Object marlais_make_generic_entrypoint (Object args);
extern Object marlais_make_method (Object name, Object params, Object body, struct environment *env, int do_generic);
extern Object marlais_add_method (Object generic, Object method);
extern Object marlais_make_next_method (Object generic, Object rest_methods, Object args);

extern Object marlais_applicable_method_p (Object fun, Object sample_args, int strict_check);
extern Object marlais_sorted_applicable_methods (Object fun, Object sample_args);


#ifdef MARLAIS_ENABLE_METHOD_CACHING
extern Object marlais_recalc_next_methods (Object fun, Object meth, Object sample_args);
extern Object marlais_sorted_possible_method_handles (Object fun, Object sample_args);
#endif

#endif
