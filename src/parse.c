/*

   parse.c

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

   Copyright, 1994, Joseph N. Wilson.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include <stdio.h>

#include "parse.h"

#include "error.h"
#include "print.h"
#include "yystype.h"

Object *parse_value_ptr;
extern FILE *yyin;
extern Object standard_error_stream;

void yy_restart (FILE * new_file);

extern int yydebug;
extern int echo_prefix;
extern int yychar;
extern int yylineno;
extern int load_file_context;

extern Object eof_object;

Object
parse_object (FILE * fp, int debug)
{
    Object parse_value;

    if (!yyin) {
	yyin = fp;
    } else if (yyin && fp != yyin) {
	reset_parser (fp);
    }
    yydebug = debug;
    parse_value_ptr = &parse_value;
    if (yyparse () == 0) {
	if (echo_prefix && !load_file_context) {
	    fprintf (stderr, "==> ");
	    print_object (standard_error_stream, parse_value, 1);
	    fprintf (stderr, "\n");
	    fflush (stderr);
	}
	return parse_value;
    } else {
	warning ("Parser failed in inexplicable way", parse_value, NULL);
	return eof_object;
    }
}

void
reset_parser (FILE * fp)
{
    fflush (stderr);
    yy_restart (fp);
    yylineno = 1;
}
