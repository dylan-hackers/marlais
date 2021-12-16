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

/* The parser knows about these */
Object marlais_yybindings;
Object marlais_yyresult;

void
marlais_parser_reset (void)
{
  /* clear the result */
  marlais_yyresult = MARLAIS_UNSPECIFIED;
  /* initialize the binding stack */
  marlais_yybindings = marlais_cons (marlais_make_integer (0), MARLAIS_NIL);
  /* reset the lexer */
  marlais_lexer_reset ();
  /* reset the parser */
  marlais_yyrestart (NULL);
}

void
marlais_parser_prepare_stream (FILE *fp, int debug)
{
  /* reset the parser */
  marlais_parser_reset ();
  /* configure debugging */
  marlais_yydebug = debug;
  /* prepare the parser */
  marlais_yyrestart (fp);
}

void
marlais_parser_prepare_string (const char *str, int debug)
{
  /* reset the parser */
  marlais_parser_reset ();
  /* configure debugging */
  marlais_yydebug = debug;
  /* prepare the parser */
  marlais_yy_scan_string (str);
}

Object
marlais_parse_object (void)
{
  Object res;
  /* run the parser */
  if (marlais_yyparse () == 0) {
    /* get the result */
    res = marlais_yyresult;
  } else {
    /* parse error */
    marlais_warning ("Parser failed in inexplicable way", marlais_yyresult, NULL);
    res = MARLAIS_EOF;
  }
  /* destroy stray reference */
  marlais_yyresult = MARLAIS_UNSPECIFIED;
  /* return whatever result */
  return res;
}
