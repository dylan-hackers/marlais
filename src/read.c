/*

   read.c

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
   0.6 Revisions Copyright 2001, Douglas M. Auclair.  All rights reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include <stdio.h>
#include <ctype.h>
#ifdef sun
#include <floatingpoint.h>
#endif

#include "read.h"

#include "bytestring.h"
#include "character.h"
#include "dylan_lexer.h"
#include "dylan.tab.h"
#include "error.h"
#include "file.h"
#include "list.h"
#include "number.h"
#include "parse.h"
#include "prim.h"
#include "symbol.h"
#include "vector.h"

#ifdef BIG_INTEGERS
#include "biginteger.h"
#endif

extern FILE *yyin;
extern Object yylval;

/* local function prototypes */

static Object infix_read_object (FILE * fp);
static Object infix_decode_token (int yychar, FILE * fp);

static Object read_wrapper (Object stream);
static Object read_char (Object stream);

/* primitives */

static struct primitive read_prims[] =
{
  {"%read", prim_0_1, read_wrapper},
  {"%read-char", prim_0_1, read_char},
};

/* function definitions */

void
init_read_prims (void)
{
  int num = sizeof (read_prims) / sizeof (struct primitive);
  init_prims (num, read_prims);
}

static Object
infix_read_object (FILE * fp)
{
  int yychar;
  int old_load_file_context = load_file_context;

  load_file_context = 1;

  if (yyin != fp) {
    reset_parser (fp);
  }
  if ((yychar = yylex ()) < 0)
    yychar = 0;

  load_file_context = old_load_file_context;
  return infix_decode_token (yychar, fp);
}

static Object
infix_decode_token (int yychar, FILE * fp)
{
  switch (yychar) {
  case EOF_TOKEN:
    return (eof_object);
  case LITERAL:
  case STRING:
  case HASH_T:
  case HASH_F:
    return (yylval);
  case HASH_BRACKET:
    {
      Object new_list = make_empty_list ();
      Object *new_list_ptr = &new_list;
      
      do {
	if ((yychar = yylex ()) < 0)
	  yychar = 0;
	if (yychar == ']')
	  return new_list;
	
	*new_list_ptr = cons (infix_decode_token (yychar, fp),
			      make_empty_list ());
	new_list_ptr = &CDR (*new_list_ptr);
	
	if ((yychar = yylex ()) < 0)
	  yychar = 0;
      } while (yychar == ',');
      
      if (yychar != ']') {
	return error ("Malformed vector.  Expected a ']'", NULL);
      }
      return make_sov (new_list);
    }			/* case HASH_BRACKET */
  case HASH_PAREN:
    {
      Object new_list = make_empty_list ();
      Object *new_list_ptr = &new_list;
      
      do {
	if ((yychar = yylex ()) < 0)
	  yychar = 0;
	if (yychar == ')')
	  return new_list;
	
	*new_list_ptr = cons (infix_decode_token (yychar, fp),
			      make_empty_list ());
	new_list_ptr = &CDR (*new_list_ptr);

	if ((yychar = yylex ()) < 0)
	  yychar = 0;
      } while (yychar == ',');

      if (yychar == '.') {
	*new_list_ptr = infix_read_object (fp);
	if ((yychar = yylex ()) < 0)
	  yychar = 0;
      }
      if (yychar != ')') {
	return error ("Malformed list.  Expected a ')'", NULL);
      }
      return new_list;
    }			/* case HASH_PAREN */
  case KEYWORD:
    return yylval;
  default:
    return error ("read couldn't find a literal", NULL);
  }
}

static Object
read_wrapper (Object stream)
{
  FILE *fp;

  if (stream) {
    fp = STREAMFP (stream);
  } else {
    fp = stdin;
  }
  return infix_read_object(fp);
}

static Object
read_char (Object stream)
{
  FILE *fp;
  int ch;

  if (stream) {
    fp = STREAMFP (stream);
  } else {
    fp = stdin;
  }
  ch = getc (fp);
  if (ch == EOF) {
    return (eof_object);
  } else {
    return (make_character (ch));
  }
}
