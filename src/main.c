/* main.c -- see COPYRIGHT for use */

#include <stdlib.h>
#include <stdio.h>

#include "alloc.h"
#include "apply.h"
#include "array.h"
#include "boolean.h"
#include "bytestring.h"
#include "character.h"
#include "class.h"
#include "deque.h"
#include "dylan_lexer.h"
#include "env.h"
#include "error.h"
#include "eval.h"
#include "file.h"
#include "function.h"
#include "globals.h"
#include "keyword.h"
#include "list.h"
#include "misc.h"
#include "number.h"
#include "object.h"
#include "parse.h"
#include "print.h"
#include "read.h"
#include "slot.h"
#include "symbol.h"
#include "syntax.h"
#include "stream.h"
#include "sys.h"
#include "table.h"
#include "values.h"
#include "vector.h"

void initialize_reservered_word_symbols (void);
void initialize_marlais (void);
extern Object binding_stack;
extern FILE *yyin;
extern int yylineno;

#ifdef MACOS
int getopt (int argc, char *argv[], const char *options);

#endif

#ifndef INIT_FILE
#define INIT_FILE "init.dylan"
#endif

#ifndef VERSION
#define VERSION "0.6.4a"
#endif

#ifdef MACOS
#define main marlais_main
int marlais_main (int argc, char *argv[]);
extern void init_mac_prims (void);
extern void yy_restart (FILE * fp);

// support for asynch loading of a file.
Boolean theLoadFileFlag = false;
char thePathToLoad[256];
int thePromptDirty = false;

// support for general message handling.
#include "MacMessages.h"
#include "marlais_utils.h"
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
    symbol = make_symbol (symbol_name);
    add_top_level_binding (symbol, obj, 1);
    fprintf (stdout, " $%i = ", sequence_num);
    sequence_num++;
  }
  apply (eval (print_symbol),
	 listem (obj, standard_output_stream, NULL));
  fprintf (stdout, "\n");
}

static int read_eval_print(FILE* f, int dbg_lvl, int bind_constant_p)
{
  int x, vals;
  Object obj;

  if ((obj = parse_object (f, dbg_lvl)) && (obj != eof_object)) {
    obj = eval (obj);
    if(TYPE(obj) == Values) {
      vals = VALUESNUM(obj);
    }
    else {
      obj = construct_values(1, obj);
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
      fatal ("Marlais fatal error: unrecognized option");
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

  /* initialization */
  initialize_marlais ();
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
    load(make_byte_string (init_file));
  }

  set_module (new_module (dylan_user_symbol));
  current_module ()->exported_bindings = all_symbol;

  use_module (dylan_symbol,
	      all_symbol,
	      make_empty_list (),
	      empty_string,
	      make_empty_list (),
	      all_symbol);

#ifdef DO_NOT_LOAD_COMMON_DYLAN_SPEC
#else
  {
#define COMMON_DYLAN_LIB_DIR "common"
    char* lib_dir = getenv("MARLAIS_LIB_DIR");
    char common_dylan[256];
    if(!lib_dir) {
      lib_dir = COMMON_DYLAN_LIB_DIR;
    }
    sprintf(common_dylan, "%s/common-dylan.dylan", lib_dir);
    load(make_byte_string(common_dylan));
  }
#endif

  if(execute) {
    /* put in a ; in case the user forgets */
    char command[256]; // win32 MSVC++ requires a constant here
    sprintf(command, "%s;", argv[optind]);
    yy_restart(stdin);
    yy_scan_string(command);
    read_eval_print(stdin, debug, 0);
    if(!stay) exit(0);
    optind++;
  }

  /* load any source files specified on command line */
  while (optind < argc) {
    load (make_byte_string (argv[optind]));
    maybe_quit = 1;
    optind++;
  }

  if(maybe_quit && !stay) exit(0);
  printf("Marlais %s\n", VERSION);

  load_file_context = 0;
	/* <pcb> needs to be cleared after loading file */
  yy_restart(stdin);
  yy_scan_string("");

  cache_env = the_env;
  current_prompt = prompt;

  /* errors reset to here */
  err = setjmp (error_return);
  /* things to do on an error reset */
  if (err) {
    close_open_files ();
#ifdef MACOS
    fflush (stdout);

    /* so parser/scanner won't get confused. */
    clearerr (stdin);
    fflush (stdin);
    yy_restart (stdin);

    /* is this a request to load a file? */
    if (theLoadFileFlag) {
      theLoadFileFlag = false;
      load (make_byte_string (thePathToLoad));
    }
    /* some event caused the prompt to be dirty. */
    if (thePromptDirty) {
      printf (prompt);
      fflush (stdout);
      thePromptDirty = 0;
    }
#else
    fflush (stdout);
    clearerr (stdin);
#endif
    if (trace_functions) {
      printf ("; reset\n");
      trace_level = 0;
    }
    load_file_context = 0;
    the_env = cache_env;
    eval_stack = 0;
    push_eval_stack (current_module ()->sym);
    num_debug_contexts = 0;
    prompt = "? ";
    current_prompt = prompt;
  }

  while(read_eval_print(stdin, debug, 1)) {
    cache_env = the_env;
    current_prompt = prompt;
  }

  printf("\n");
  return (0);
}

void
initialize_marlais (void)
{
  /* intialize garbage collector */
  initialize_gc ();

  dylan_symbol = make_symbol ("dylan");
  dylan_user_symbol = make_symbol ("dylan-user");

  set_module (new_module (dylan_symbol));

  all_symbol = make_symbol ("all");
  (current_module ())->exported_bindings = all_symbol;

  /* initialize symbol table primitives -- MUST BE DONE EARLY!! */
  init_symbol_prims ();

  /* intialize global objects */
  initialize_empty_list ();

  empty_string = make_byte_string ("");
  equal_symbol = make_symbol ("=");

  true_object = make_true ();
  false_object = make_false ();

  apply_symbol = make_symbol ("apply");
  key_symbol = make_symbol ("#key");
  keyword_symbol = make_symbol ("keyword");
  required_symbol = make_symbol ("required");
  allkeys_symbol = make_symbol ("#all-keys");
  hash_rest_symbol = make_symbol ("#rest");
  next_symbol = make_symbol ("#next");
  values_symbol = make_symbol ("values");
  hash_values_symbol = make_symbol ("#values");
  quote_symbol = make_symbol ("quote");
  eof_object = make_eof_object ();
  unwind_symbol = make_symbol ("%unwind");
  next_method_symbol = make_symbol ("next-method");
  initialize_symbol = make_symbol ("initialize");
  equal_hash_symbol = make_symbol ("=hash");
  uninit_slot_object = make_uninit_slot ();

#ifndef COMMON_DYLAN_SPEC
  standard_input_stream = make_stream (Input, stdin);
  standard_output_stream = make_stream (Output, stdout);
  standard_error_stream = make_stream (Output, stderr);
#endif

/* this looks like lisp -- should it go? */
  quasiquote_symbol = make_symbol ("quasiquote");
  unquote_symbol = make_symbol ("unquote");
  unquote_splicing_symbol = make_symbol ("unquote-splicing");

  element_symbol = make_symbol ("element");
  element_setter_symbol = make_symbol ("element-setter");
  signal_symbol = make_symbol ("signal");
  concatenate_symbol = make_symbol ("concatenate");
  cond_symbol = make_symbol ("cond");

  /* often used keywords */
  getter_keyword = make_keyword ("getter:");
  setter_keyword = make_keyword ("setter:");
  until_keyword = make_keyword ("until:");
  while_keyword = make_keyword ("while:");
  else_keyword = make_keyword ("else:");
  type_keyword = make_keyword ("type:");
  deferred_type_keyword = make_keyword ("deferred-type:");
  init_value_keyword = make_keyword ("init-value:");
  init_function_keyword = make_keyword ("init-function:");
  init_keyword_keyword = make_keyword ("init-keyword:");
  required_init_keyword_keyword = make_keyword ("required-init-keyword:");
  allocation_keyword = make_keyword ("allocation:");
  super_classes_keyword = make_keyword ("superclasses:");
  slots_keyword = make_keyword ("slots:");
  abstract_p_keyword = make_keyword ("abstract?:");
  debug_name_keyword = make_keyword ("debug-name:");
  size_keyword = make_keyword ("size:");
  fill_keyword = make_keyword ("fill:");
  dim_keyword = make_keyword ("dimensions:");
  min_keyword = make_keyword ("min:");
  max_keyword = make_keyword ("max:");

  /* often used symbols */
  instance_symbol = make_symbol ("instance");
  class_symbol = make_symbol ("class");
  each_subclass_symbol = make_symbol ("each-subclass");
  inherited_symbol = make_symbol ("inherited");
  constant_symbol = make_symbol ("constant");
  virtual_symbol = make_symbol ("virtual");
  object_class_symbol = make_symbol ("object-class");

  obj_sym = make_symbol ("obj");
  slot_val_sym = make_symbol ("slot-value");
  set_slot_value_sym = make_symbol ("set-slot-value!");
  val_sym = make_symbol ("val");
  initial_state_sym = make_symbol ("initial-state");
  next_state_sym = make_symbol ("next-state");
  current_element_sym = make_symbol ("current-element");

  colon_equal_symbol = make_symbol (":=");
  not_equal_symbol = make_symbol ("~=");
  not_equal_equal_symbol = make_symbol ("~==");
  equal_equal_symbol = make_symbol ("==");
  greater_equal_symbol = make_symbol (">=");
  lesser_equal_symbol = make_symbol ("<=");
  or_symbol = make_symbol ("|");
  and_symbol = make_symbol ("&");
  greater_symbol = make_symbol (">");
  lesser_symbol = make_symbol ("<");
  exponent_symbol = make_symbol ("^");
  divides_symbol = make_symbol ("/");
  times_symbol = make_symbol ("*");
  minus_symbol = make_symbol ("-");
  plus_symbol = make_symbol ("+");
  not_symbol = make_symbol ("~");

  /** dma -- I think these are classic syntax constructs */
  local_bind_symbol = make_symbol ("\"local-bind");
  local_bind_rec_symbol = make_symbol ("\"local-bind-rec");
  unbinding_begin_symbol = make_symbol ("\"unbinding-begin");
  define_variable_symbol = make_symbol ("define-variable");
  define_constant_symbol = make_symbol ("define-constant");
  define_class_symbol = make_symbol ("define-class");
  define_generic_function_symbol = make_symbol ("define-generic-function");
  define_method_symbol = make_symbol ("define-method");
  define_function_symbol = make_symbol ("define-function");

  seal_symbol = make_symbol ("seal");
  set_bang_symbol = make_symbol ("set!");
  singleton_symbol = make_symbol ("singleton");
  sealed_symbol = make_symbol ("sealed");
  open_symbol = make_symbol ("open");
  dynamism_keyword = make_keyword ("dynamism:");
  negative_symbol = make_symbol ("negative");
  list_symbol = make_symbol ("list");
  pair_symbol = make_symbol ("pair");
  variable_keyword = make_keyword ("variable:");
  to_symbol = make_symbol ("to");
  above_symbol = make_symbol ("above");
  below_symbol = make_symbol ("below");
  by_symbol = make_symbol ("by");
  range_keyword = make_keyword ("range:");
  collection_keyword = make_keyword ("collection:");
  forward_iteration_protocol_symbol = 
    make_symbol ("forward-iteration-protocol");
  plus_symbol = make_symbol ("+");
  bind_symbol = make_symbol ("bind");
  bind_exit_symbol = make_symbol ("bind-exit");
  exit_symbol = make_symbol ("exit");
  unwind_protect_symbol = make_symbol ("unwind-protect");
  vals_symbol = make_symbol ("\"vals");
  modifiers_keyword = make_keyword ("modifiers:");
  abstract_symbol = make_symbol ("abstract");
  concrete_symbol = make_symbol ("concrete");
  primary_symbol = make_symbol ("primary");
  free_symbol = make_symbol ("free");
  use_symbol = make_symbol ("use");
  export_symbol = make_symbol ("export");
  create_symbol = make_symbol ("create");
  module_symbol = make_symbol ("module");
  define_module_symbol = make_symbol ("define-module");
  module_keyword = make_keyword ("module:");
  import_keyword = make_keyword ("import:");
  exclude_keyword = make_keyword ("exclude:");
  prefix_keyword = make_keyword ("prefix:");
  rename_keyword = make_keyword ("rename:");
  export_keyword = make_keyword ("export:");

  /* infrequently used, but important symbols */
  instance_slots_symbol = make_symbol ("%instance-slots");
  class_slots_symbol = make_symbol ("%class-slots");
  each_subclass_slots_symbol = make_symbol ("%each-subclass-slots");
  x_symbol = make_symbol ("x");

  /* initialize table of syntax operators and functions */
  init_syntax_table ();
  init_reserved_word_symbols ();
  define_test_symbol = make_symbol ("define-test");
  test_symbol = make_symbol ("test");
  description_symbol = make_keyword ("description:");
  aref_symbol = make_symbol ("aref");
  print_symbol = make_symbol ("print-object");
  princ_symbol = make_symbol ("object-princ");

  /* initialize builtin classes */
  init_class_hierarchy ();

  unspecified_object = make_unspecified_object ();

  /* make the unspecified object available */
  add_top_level_binding (make_symbol ("%unspecified"),
			 unspecified_object,
			 1);

  /* make the uninitialize slot value available */
  add_top_level_binding (make_symbol ("%uninitialized-slot-value"),
			 uninit_slot_object,
			 1);

  /* make default object */
  default_object = cons (false_object, false_object);
  add_top_level_binding (make_symbol ("%default-object"), default_object, 1);

  binding_stack = cons (make_integer (0), make_empty_list ());

  /* initialize primitives */
  init_env_prims ();
  init_list_prims ();
  init_class_prims ();
  init_slot_prims ();
  init_file_prims ();
  init_function_prims ();
  init_values_prims ();
  init_print_prims ();
  init_number_prims ();
  init_apply_prims ();
  init_boolean_prims ();
  init_keyword_prims ();
  init_string_prims ();
  init_vector_prims ();
  init_error_prims ();
  init_stream_prims ();
  init_read_prims ();
  init_table_prims ();
  init_char_prims ();
  init_deque_prims ();
  init_array_prims ();
  init_sys_prims ();
#ifdef MACOS
  init_mac_prims ();
#endif
}
