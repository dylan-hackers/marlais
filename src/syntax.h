/*

   syntax.h

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

#ifndef SYNTAX_H
#define SYNTAX_H

#include "object.h"

/* global objects */
extern Object type_class, initial_state_sym, next_state_sym;
extern Object current_element_sym;
extern Object signal_symbol, variable_keyword, range_keyword;
extern Object collection_keyword;
extern Object to_symbol, above_symbol, below_symbol, by_symbol;
extern Object forward_iteration_protocol_symbol;
extern Object lesser_symbol, greater_symbol;
extern Object lesser_equal_symbol, greater_equal_symbol;
extern Object plus_symbol;
extern Object object_class_symbol;
extern Object import_keyword;
extern Object exclude_keyword;
extern Object prefix_keyword;
extern Object rename_keyword;
extern Object export_keyword;
extern Object use_symbol;
extern Object export_symbol;
extern Object create_symbol;
extern Object empty_string;

typedef Object (*syntax_fun) (Object);
void init_syntax_table (void);
syntax_fun syntax_function (Object sym);
Object eval_slots (Object slots);

#endif
