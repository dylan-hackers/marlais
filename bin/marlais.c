/* main.c -- see COPYRIGHT for use */

#include <marlais/marlais.h>

#include <marlais/gc.h>
#include <marlais/parser.h>

#include <getopt.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>

static char *optstring = "dehnpsv";

static bool opt_debug = 0;
static bool opt_expressions = 0;
static bool opt_noinit = 0;
static bool opt_stay = 0;


char *prompt = "? ";
char *prompt_continuation = "> ";
char *current_prompt;
int sequence_num = 0;

static void print_top_level_constant(Object obj, int bind_p)
{
  Object symbol;
  char symbol_name[12];

  if (obj == MARLAIS_UNSPECIFIED) return;

  if(bind_p) {
    snprintf (symbol_name, 12, "$%i", sequence_num);
    symbol = marlais_make_name (symbol_name);
    marlais_add_export (symbol, obj, 1);
    fprintf (stdout, " $%i = ", sequence_num);
    sequence_num++;
  }

  marlais_apply (marlais_eval (print_symbol),
                 marlais_make_list (obj,
                                    marlais_eval(standard_output_symbol),
                                    NULL));

  fprintf (stdout, "\n");
}

static int read_eval_print(FILE* f, int bind_constant_p)
{
  int x, vals;
  Object obj;

  /* <pcb> needs to be cleared after loading file */
  marlais_parser_prepare_string("", opt_debug);

  if ((obj = marlais_parse_object ()) && (obj != MARLAIS_EOF)) {
    obj = marlais_eval (obj);
    if(POINTERP(obj) && POINTERTYPE(obj) == Values) {
      vals = VALUESNUM(obj);
    }
    else {
      obj = marlais_values_args(1, obj);
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
  printf("Marlais %s -- a Dylan Language Interactor\n\n", MARLAIS_VERSION);
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
      opt_debug = true;
      break;
    case 'e':
      opt_expressions = true;
      break;
    case 'n':
      opt_noinit = true;
      break;
    case 's':
      opt_stay = true;
      break;

    case 'p':
      prompt_continuation = "";
      break;

    case 'h':
      show_help();
      break;
    case 'v':
      printf("Marlais, version %s\n", MARLAIS_VERSION);
      exit(0);
    default:
      printf("Unrecognized option '-%c'\n", c);
      exit(1);
    }
  }
}

int
main (int argc, char *argv[])
{
  char *dylan_init, *common_init;
  int err;
  jmp_buf errjmp;
  extern int optind;
  struct environment *cache_env;
  int maybe_quit = 0;

  /* initialize locales */
  setlocale(LC_ALL, "");

  /* initialize marlais */
  marlais_initialize ();

  marlais_trace_bindings = 0;
  marlais_trace_functions = 0;
  marlais_trace_primitives = 0;
  marlais_trace_level = 0;

  /* parse arguments */
  parse_args(argc, argv);

  /* load any source files specified on command line */
  while (optind < argc) {
    marlais_load (marlais_make_bytestring (argv[optind]));
    maybe_quit = 1;
    optind++;
  }

  if(opt_expressions) {
    /* put in a ; in case the user forgets */
    char command[256]; // win32 MSVC++ requires a constant here
    sprintf(command, "%s;", argv[optind]);
    marlais_parser_prepare_string(command, opt_debug);
    read_eval_print(stdin, 0);
    if(!opt_stay) exit(0);
    optind++;
  }

  if(maybe_quit && !opt_stay) exit(0);

  printf("Marlais %s\n", MARLAIS_VERSION);

  cache_env = the_env;
  current_prompt = prompt;

  /* errors reset to here */
  err = setjmp (errjmp);
  /* things to do on an error reset */
  if (err) {
    marlais_close_open_files ();
    fflush (stdout);
    clearerr (stdin);
    if (marlais_trace_functions) {
      printf ("; reset\n");
      marlais_trace_level = 0;
    }
    marlais_loading = 0;
    the_env = cache_env;
    eval_stack = 0;
    marlais_push_eval_stack (MODULE(marlais_get_current_module ())->sym);
    prompt = "? ";
    current_prompt = prompt;
  }
  marlais_error_jump = &errjmp;

  while(read_eval_print(stdin, 1)) {
    cache_env = the_env;
    current_prompt = prompt;
  }

  printf("\n");
  return (0);
}
