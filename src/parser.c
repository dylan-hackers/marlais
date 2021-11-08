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

#include <marlais/parser.h>

#include <marlais/lexer.h>
#include <marlais/print.h>
#include <marlais/yystype.h>

#include "lexer.gen.h"
#include "parser.gen.h"

Object *parse_value_ptr;

extern int load_file_context;

void
marlais_parser_reset (FILE * new_file)
{
  marlais_lexer_reset (new_file);
}

Object
marlais_parse_object (FILE * fp, int debug)
{
  Object parse_value;

  if (!marlais_yyin) {
    marlais_yyin = fp;
  } else if (marlais_yyin && fp != marlais_yyin) {
    marlais_parser_reset (fp);
  }
  marlais_yydebug = debug;
  parse_value_ptr = &parse_value;
  if (marlais_yyparse () == 0) {
    return parse_value;
  } else {
    marlais_warning ("Parser failed in inexplicable way", parse_value, NULL);
    return eof_object;
  }
}
