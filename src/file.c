/*

   file.c

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

#include <stdio.h>

#include "file.h"

#include "dylan_lexer.h"
#include "env.h"
#include "eval.h"
#include "error.h"
#include "foreign_ptr.h"
#include "list.h"
#include "parse.h"
#include "prim.h"
#include "read.h"

extern Object open_file_list;

static struct primitive file_prims[] =
{
    {"load", prim_1, load},
};

/* function definitions */
static FILE * open_file (Object filename);
static void close_file (FILE * fp);

void
init_file_prims (void)
{
  int num = sizeof (file_prims) / sizeof (struct primitive);
  init_prims (num, file_prims);
}

Object
i_load (Object filename)
{
    FILE *fp;
    Object obj, res;
    struct module_binding *old_module;
    Object expr_list = make_empty_list ();
    Object *expr_list_ptr;
    int old_load_file_context;

    /* save current states. */
    old_load_file_context = load_file_context;
    load_file_context = 1;
    old_module = current_module ();

    fp = open_file (filename);

    reset_parser (fp);

    /*
     * Cons up the list of expressions in the file, then eval them.
     */

    expr_list_ptr = &expr_list;
    while ((obj = parse_object (fp, 0)) && (obj != eof_object)) {
	*expr_list_ptr = cons (obj, make_empty_list ());
	expr_list_ptr = &CDR (*expr_list_ptr);
    }

    close_file (fp);

    while (PAIRP (expr_list)) {
	res = eval (CAR (expr_list));
	expr_list = CDR (expr_list);
    }

    set_module (old_module);
    load_file_context = old_load_file_context;

    return (res);
}

Object
load (Object filename)
{
  Object res;
  FILE *old_yyin = yyin;

  res = i_load (filename);
  if (old_yyin == stdin) {
    clearerr (stdin);
    fflush (stdin);
    yyrestart (stdin);
  }
  return res;
}

static FILE *
open_file (Object filename)
{
  char *str;
  FILE *fp;

  if (!BYTESTRP (filename)) {
    error ("load: argument must be a filename", filename, NULL);
  }
  str = BYTESTRVAL (filename);
  fp = fopen (str, "r");
  if (!fp) {
    error ("load: cannot open file", filename, NULL);
  }
  open_file_list = cons (make_foreign_ptr (fp), open_file_list);
  return fp;
}

static void
close_file (FILE * fp)
{
  if ((FILE *) FOREIGNPTR (CAR (open_file_list)) != fp) {
    error ("close-file called with bogus fp (BUG)", NULL);
  }
  fclose (fp);
  open_file_list = CDR (open_file_list);
}

void
close_open_files (void)
{
  while (PAIRP (open_file_list)) {
    fclose ((FILE *) FOREIGNPTR (CAR (open_file_list)));
    open_file_list = CDR (open_file_list);
  }
}
