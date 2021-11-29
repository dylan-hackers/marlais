/* file.c -- see COPYRIGHT for use */

#include <marlais/file.h>

#include <marlais/env.h>
#include <marlais/eval.h>
#include <marlais/foreign_ptr.h>
#include <marlais/lexer.h>
#include <marlais/list.h>
#include <marlais/parser.h>
#include <marlais/prim.h>
#include <marlais/print.h>
#include <marlais/read.h>

#include "lexer.gen.h"

/* Primitives */

static struct primitive file_prims[] =
{
    {"load", prim_1, marlais_load},
};

/* Internal function declarations */

static FILE * open_file (Object filename);
static void close_file (FILE * fp);

/* Exported functions */

void
marlais_register_file (void)
{
  int num = sizeof (file_prims) / sizeof (struct primitive);
  marlais_register_prims (num, file_prims);
}

static Object
load_internal (Object filename)
{
    FILE *fp;
    Object obj, res;
    struct module_binding *old_module;
    Object expr_list = MARLAIS_NIL;
    Object *expr_list_ptr;
    int old_load_file_context;

    /* save current states. */
    old_load_file_context = load_file_context;
    load_file_context = 1;
    old_module = marlais_current_module ();

    fp = open_file (filename);

    /*
     * Cons up the list of expressions in the file, then eval them.
     */

    expr_list_ptr = &expr_list;
    marlais_parser_prepare_stream (fp, 0);
    while ((obj = marlais_parse_object ()) && (obj != MARLAIS_EOF)) {
	*expr_list_ptr = marlais_cons (obj, MARLAIS_NIL);
	expr_list_ptr = &CDR (*expr_list_ptr);
    }

    close_file (fp);

    while (PAIRP (expr_list)) {
	res = marlais_eval (CAR (expr_list));
	expr_list = CDR (expr_list);
    }

#ifdef NO_LOADING_LIBRARIES
    marlais_set_module (old_module);
#endif

    load_file_context = old_load_file_context;

    return (res);
}

Object
marlais_load (Object filename)
{
  Object res;
  FILE *old_yyin = marlais_yyin;

  res = load_internal (filename);
  if (old_yyin == stdin) {
    clearerr (stdin);
    fflush (stdin);
    marlais_yyrestart (stdin);
  }
  return res;
}

void
marlais_close_open_files (void)
{
  while (PAIRP (open_file_list)) {
    fclose ((FILE *) FOREIGNPTR (CAR (open_file_list)));
    open_file_list = CDR (open_file_list);
  }
}

/* Internal functions */

static FILE *
open_file (Object filename)
{
  char *str;
  FILE *fp;

  if (!BYTESTRP (filename)) {
    marlais_error ("load: argument must be a filename", filename, NULL);
  }
  str = BYTESTRVAL (filename);
  fprintf(stderr, "Loading %s...\n", str);
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
        marlais_error ("load: cannot open file", filename, NULL);
      }
    }
  }
  open_file_list = marlais_cons (marlais_make_foreign_ptr (fp), open_file_list);
  return fp;
}

static void
close_file (FILE * fp)
{
  if ((FILE *) FOREIGNPTR (CAR (open_file_list)) != fp) {
    marlais_error ("close-file called with bogus fp (BUG)", NULL);
  }
  fclose (fp);
  open_file_list = CDR (open_file_list);
}
