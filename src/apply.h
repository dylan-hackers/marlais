/*

   apply.h

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

#ifndef APPLY_H
#define APPLY_H

#include "object.h"

/* global data */
extern int trace_functions;
extern int trace_level;
extern Object hash_values_symbol;
extern Object ResultValueStack;
Object default_result_value (void);

/* external functions */
void init_apply_prims (void);
Object apply (Object fun, Object args);
Object apply_internal (Object fun, Object args);
Object apply_method (Object meth,
		     Object args,
		     Object rest_methods,
		     Object generic_apply);
Object construct_return_values (Object ret,
				Object required_values,
				Object rest_values);

#endif
