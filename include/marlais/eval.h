/*

   eval.h

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

#ifndef MARLAIS_EVAL_H
#define MARLAIS_EVAL_H

#include <marlais/common.h>
#include <marlais/env.h>

struct eval_stack {
    struct eval_stack *next;
    Object context;
    struct environment *frame;
};

extern struct eval_stack *eval_stack;

/* Apply arguments to function */
extern Object marlais_apply (Object fun, Object args);

/* Evaluate an expression */
extern Object marlais_eval (Object obj);

extern Object marlais_tail_eval (Object obj);

extern void marlais_pop_eval_stack (void);
extern void marlais_push_eval_stack (Object obj);
extern Object marlais_print_stack (void);

#endif
