/* main.c -- see COPYRIGHT for use */

#include <marlais/marlais.h>

#include <marlais/gc.h>
#include <marlais/parser.h>

#include <getopt.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>


#ifndef INIT_FILE
#define INIT_FILE "init.dylan"
#endif

#ifndef VERSION
#define VERSION "0.6.4-io-beta"
#endif

static int do_not_load_init_file = 0;
static char *optstring = "dehnpsv";
char *prompt = "? ";
char *prompt_continuation = "> ";
char *current_prompt;
static int debug = 0;
int execute = 0;
int stay = 0;
int sequence_num = 0;

static void print_top_level_constant(Object obj, int bind_p)
{
  Object symbol;
  char symbol_name[12];

  if (obj == unspecified_object) return;

  if(bind_p) {
    snprintf (symbol_name, 12, "$%i", sequence_num);
    symbol = marlais_make_name (symbol_name);
    marlais_add_export (symbol, obj, 1);
    fprintf (stdout, " $%i = ", sequence_num);
    sequence_num++;
  }

  marlais_apply (marlais_eval (print_symbol),
	 listem (obj, marlais_eval(standard_output_symbol), NULL));

  fprintf (stdout, "\n");
}

static int read_eval_print(FILE* f, int bind_constant_p)
{
  int x, vals;
  Object obj;

  if ((obj = marlais_parse_object ()) && (obj != eof_object)) {
    obj = marlais_eval (obj);
    if(POINTERP(obj) && POINTERTYPE(obj) == Values) {
      vals = VALUESNUM(obj);
    }
    else {
      obj = marlais_construct_values(1, obj);
      vals = 1;
    }
    for(x = 0; x < vals; x++) {
      Object elt = VALUESELS(obj)[x];
      print_top_level_constant(elt, bind_constant_p);
    }
    fflush (stdout);
    return 1;
    /*    cache_env = the_env;
	  current_prompt = prompt; */
  }
  return 0;
}

static void show_help()
{
  printf("Marlais %s -- a Dylan Language Interactor\n\n", VERSION);
  printf("marlais [-dhnpsv] [-e '<Dylan expression>'|<file.dylan> ...]\n\n"
  	 "Options:\n  -d -- Provide debugging information\n"
  	 "  -e -- Execute Dylan expression, print return values and "
	 "optionally exit\n"
  	 "  -h -- Show help information on Marlais\n"
  	 "  -n -- Do not load bootstrap init.dylan file\n"
	 "  -p -- Do not show continuation prompt for incomplete "
	 "expressions\n"
	 "  -s -- Stay in interactor after executing file or expression\n"
	 "  -v -- print version number\n\n"
	 "If Marlais receives files to execute, it will do so, in order, and\n"
	 "then (optionally) exit.\n\n"
	 "To quit the interactor, send it the EOF or BRK character at the\n"
	 "prompt (EOF is <CTRL>-D for Unix and <CTRL>-Z for Windows, BRK is\n"
         "<CTRL-C>).\n");
  exit(0);
}

static void parse_args(int argc, char* argv[])
{
  char c;

  /* process command line parameters except source files */
  while ((c = getopt (argc, argv, optstring)) != EOF) {
    switch (c) {
    case 'd':
      debug = 1;
      break;
    case 'e':
      execute = 1;
      break;
    case 'h':
      show_help();
      break;
    case 'n':
      do_not_load_init_file = 1;
      break;
    case 'p':
      prompt_continuation = "";
      break;
    case 's':
      stay = 1;
      break;
    case 'v':
      printf("Marlais, version %s\n", VERSION);
      exit(0);
    default:
      marlais_fatal ("fatal error: unrecognized option");
    }
  }
}

int
main (int argc, char *argv[])
{
  char *init_file;
  int err;
  extern int optind;
  struct frame *cache_env;
  int maybe_quit = 0;

  setlocale(LC_ALL, "");

  /* initialization */
  marlais_initialize ();
  open_file_list = make_empty_list ();
  parse_args(argc, argv);

  /* error catch for initialization code */
  err = setjmp (error_return);
  if (err) {
    printf ("error in initialization code -- exiting.\n");
    exit (1);
  }

  /* load initialization code */
  if (!do_not_load_init_file) {
    init_file = getenv ("MARLAIS_INIT");
    if(!init_file) {
      init_file = INIT_FILE;
    }
    marlais_load(marlais_make_bytestring (init_file));
  }

  marlais_set_module (marlais_new_module (dylan_user_symbol));
  marlais_current_module ()->exported_bindings = all_symbol;

  marlais_use_module (dylan_symbol,
	      all_symbol,
	      make_empty_list (),
	      empty_string,
	      make_empty_list (),
	      all_symbol);

#ifdef DO_NOT_LOAD_COMMON_DYLAN_SPEC
#else
  {
#define COMMON_DYLAN_LIB_DIR "common"
    char* common_dylan = getenv("MARLAIS_LIB_DIR");
    char file[256];
    if(!common_dylan) {
      common_dylan = COMMON_DYLAN_LIB_DIR;
    }
    sprintf(file, "%s/common-dylan.dylan", common_dylan);
    marlais_load(marlais_make_bytestring(file));
  }
#endif

  if(execute) {
    /* put in a ; in case the user forgets */
    char command[256]; // win32 MSVC++ requires a constant here
    sprintf(command, "%s;", argv[optind]);
    marlais_parser_prepare_string(command, debug);
    read_eval_print(stdin, 0);
    if(!stay) exit(0);
    optind++;
  }

  /* load any source files specified on command line */
  while (optind < argc) {
    marlais_load (marlais_make_bytestring (argv[optind]));
    maybe_quit = 1;
    optind++;
  }

  if(maybe_quit && !stay) exit(0);
  printf("Marlais %s\n", VERSION);

  load_file_context = 0;
  /* <pcb> needs to be cleared after loading file */
  marlais_parser_prepare_string("", debug);

  cache_env = the_env;
  current_prompt = prompt;

  /* errors reset to here */
  err = setjmp (error_return);
  /* things to do on an error reset */
  if (err) {
    marlais_close_open_files ();
    fflush (stdout);
    clearerr (stdin);
    if (trace_functions) {
      printf ("; reset\n");
      trace_level = 0;
    }
    load_file_context = 0;
    the_env = cache_env;
    eval_stack = 0;
    marlais_push_eval_stack (marlais_current_module ()->sym);
    num_debug_contexts = 0;
    prompt = "? ";
    current_prompt = prompt;
  }

  while(read_eval_print(stdin, 1)) {
    cache_env = the_env;
    current_prompt = prompt;
  }

  printf("\n");
  return (0);
}
