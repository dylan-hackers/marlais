/* file.c -- see COPYRIGHT for use */

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

#ifdef NO_LOADING_LIBRARIES
    set_module (old_module);
#endif

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
    {
#define COMMON_DYLAN_LIB_DIR "common"
      char* lib_dir = getenv("MARLAIS_LIB_DIR");
      char file_name[256];
      if(!lib_dir) {
        lib_dir = COMMON_DYLAN_LIB_DIR;
      }
      sprintf(file_name, "%s/%s", lib_dir, str);
      fp = fopen(file_name, "r");
      if(!fp) {
        error ("load: cannot open file", filename, NULL);
      }
    }
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
