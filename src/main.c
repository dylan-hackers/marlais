/* main.c -- see COPYRIGHT for use */

#include <marlais/marlais.h>

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

void initialize_reservered_word_symbols (void);
void initialize_marlais (void);
extern Object binding_stack;

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
    add_top_level_binding (symbol, obj, 1);
    fprintf (stdout, " $%i = ", sequence_num);
    sequence_num++;
  }

  marlais_apply (eval (print_symbol),
	 listem (obj, eval(standard_output_symbol), NULL));

  fprintf (stdout, "\n");
}

static int read_eval_print(FILE* f, int bind_constant_p)
{
  int x, vals;
  Object obj;

  if ((obj = marlais_parse_object ()) && (obj != eof_object)) {
    obj = eval (obj);
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
    load(marlais_make_bytestring (init_file));
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
    char* common_dylan = getenv("MARLAIS_LIB_DIR");
    char file[256];
    if(!common_dylan) {
      common_dylan = COMMON_DYLAN_LIB_DIR;
    }
    sprintf(file, "%s/common-dylan.dylan", common_dylan);
    load(marlais_make_bytestring(file));
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
    load (marlais_make_bytestring (argv[optind]));
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
    close_open_files ();
    fflush (stdout);
    clearerr (stdin);
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

  while(read_eval_print(stdin, 1)) {
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
  marlais_initialize_gc ();

  dylan_symbol = marlais_make_name ("dylan");
  dylan_user_symbol = marlais_make_name ("dylan-user");

  set_module (new_module (dylan_symbol));

  all_symbol = marlais_make_name ("all");
  (current_module ())->exported_bindings = all_symbol;

  /* intialize global objects */
  marlais_initialize_boolean ();

  initialize_empty_list ();

  marlais_initialize_stream ();

  empty_string = marlais_make_bytestring ("");
  equal_symbol = marlais_make_name ("=");

  apply_symbol = marlais_make_name ("apply");
  key_symbol = marlais_make_name ("#key");
  keyword_symbol = marlais_make_name ("keyword");
  required_symbol = marlais_make_name ("required");
  allkeys_symbol = marlais_make_name ("#all-keys");
  hash_rest_symbol = marlais_make_name ("#rest");
  next_symbol = marlais_make_name ("#next");
  values_symbol = marlais_make_name ("values");
  hash_values_symbol = marlais_make_name ("#values");
  quote_symbol = marlais_make_name ("quote");
  eof_object = make_eof_object ();
  unwind_symbol = marlais_make_name ("%unwind");
  next_method_symbol = marlais_make_name ("next-method");
  initialize_symbol = marlais_make_name ("initialize");
  equal_hash_symbol = marlais_make_name ("=hash");
  uninit_slot_object = make_uninit_slot ();
  standard_input_symbol = marlais_make_name ("*standard-input*");
  standard_output_symbol = marlais_make_name ("*standard-output*");
  standard_error_symbol = marlais_make_name ("*standard-error*");

/* this looks like lisp -- should it go? */
  quasiquote_symbol = marlais_make_name ("quasiquote");
  unquote_symbol = marlais_make_name ("unquote");
  unquote_splicing_symbol = marlais_make_name ("unquote-splicing");

  element_symbol = marlais_make_name ("element");
  element_setter_symbol = marlais_make_name ("element-setter");
  signal_symbol = marlais_make_name ("signal");
  concatenate_symbol = marlais_make_name ("concatenate");
  cond_symbol = marlais_make_name ("cond");

  /* often used keywords */
  getter_keyword = marlais_make_symbol ("getter:");
  setter_keyword = marlais_make_symbol ("setter:");
  until_keyword = marlais_make_symbol ("until:");
  while_keyword = marlais_make_symbol ("while:");
  else_keyword = marlais_make_symbol ("else:");
  type_keyword = marlais_make_symbol ("type:");
  deferred_type_keyword = marlais_make_symbol ("deferred-type:");
  init_value_keyword = marlais_make_symbol ("init-value:");
  init_function_keyword = marlais_make_symbol ("init-function:");
  init_keyword_keyword = marlais_make_symbol ("init-keyword:");
  required_init_keyword_keyword = marlais_make_symbol ("required-init-keyword:");
  allocation_keyword = marlais_make_symbol ("allocation:");
  super_classes_keyword = marlais_make_symbol ("superclasses:");
  slots_keyword = marlais_make_symbol ("slots:");
  abstract_p_keyword = marlais_make_symbol ("abstract?:");
  debug_name_keyword = marlais_make_symbol ("debug-name:");
  size_keyword = marlais_make_symbol ("size:");
  fill_keyword = marlais_make_symbol ("fill:");
  dim_keyword = marlais_make_symbol ("dimensions:");
  min_keyword = marlais_make_symbol ("min:");
  max_keyword = marlais_make_symbol ("max:");

  /* often used symbols */
  instance_symbol = marlais_make_name ("instance");
  class_symbol = marlais_make_name ("class");
  each_subclass_symbol = marlais_make_name ("each-subclass");
  inherited_symbol = marlais_make_name ("inherited");
  constant_symbol = marlais_make_name ("constant");
  virtual_symbol = marlais_make_name ("virtual");
  object_class_symbol = marlais_make_name ("object-class");

  obj_sym = marlais_make_name ("obj");
  slot_val_sym = marlais_make_name ("slot-value");
  set_slot_value_sym = marlais_make_name ("set-slot-value!");
  val_sym = marlais_make_name ("val");
  initial_state_sym = marlais_make_name ("initial-state");
  next_state_sym = marlais_make_name ("next-state");
  current_element_sym = marlais_make_name ("current-element");

  colon_equal_symbol = marlais_make_name (":=");
  not_equal_symbol = marlais_make_name ("~=");
  not_equal_equal_symbol = marlais_make_name ("~==");
  equal_equal_symbol = marlais_make_name ("==");
  greater_equal_symbol = marlais_make_name (">=");
  lesser_equal_symbol = marlais_make_name ("<=");
  or_symbol = marlais_make_name ("|");
  and_symbol = marlais_make_name ("&");
  greater_symbol = marlais_make_name (">");
  lesser_symbol = marlais_make_name ("<");
  exponent_symbol = marlais_make_name ("^");
  divides_symbol = marlais_make_name ("/");
  times_symbol = marlais_make_name ("*");
  minus_symbol = marlais_make_name ("-");
  plus_symbol = marlais_make_name ("+");
  not_symbol = marlais_make_name ("~");

  /** dma -- I think these are classic syntax constructs */
  local_bind_symbol = marlais_make_name ("\"local-bind");
  local_bind_rec_symbol = marlais_make_name ("\"local-bind-rec");
  unbinding_begin_symbol = marlais_make_name ("\"unbinding-begin");
  define_variable_symbol = marlais_make_name ("define-variable");
  define_constant_symbol = marlais_make_name ("define-constant");
  define_class_symbol = marlais_make_name ("define-class");
  define_generic_function_symbol = marlais_make_name ("define-generic-function");
  define_method_symbol = marlais_make_name ("define-method");
  define_function_symbol = marlais_make_name ("define-function");

  seal_symbol = marlais_make_name ("seal");
  set_bang_symbol = marlais_make_name ("set!");
  singleton_symbol = marlais_make_name ("singleton");
  sealed_symbol = marlais_make_name ("sealed");
  open_symbol = marlais_make_name ("open");
  dynamism_keyword = marlais_make_symbol ("dynamism:");
  negative_symbol = marlais_make_name ("negative");
  list_symbol = marlais_make_name ("list");
  pair_symbol = marlais_make_name ("pair");
  variable_keyword = marlais_make_symbol ("variable:");
  to_symbol = marlais_make_name ("to");
  above_symbol = marlais_make_name ("above");
  below_symbol = marlais_make_name ("below");
  by_symbol = marlais_make_name ("by");
  range_keyword = marlais_make_symbol ("range:");
  collection_keyword = marlais_make_symbol ("collection:");
  forward_iteration_protocol_symbol =
    marlais_make_name ("forward-iteration-protocol");
  plus_symbol = marlais_make_name ("+");
  bind_symbol = marlais_make_name ("bind");
  bind_exit_symbol = marlais_make_name ("bind-exit");
  exit_symbol = marlais_make_name ("exit");
  unwind_protect_symbol = marlais_make_name ("unwind-protect");
  vals_symbol = marlais_make_name ("\"vals");
  modifiers_keyword = marlais_make_symbol ("modifiers:");
  abstract_symbol = marlais_make_name ("abstract");
  concrete_symbol = marlais_make_name ("concrete");
  primary_symbol = marlais_make_name ("primary");
  free_symbol = marlais_make_name ("free");
  use_symbol = marlais_make_name ("use");
  export_symbol = marlais_make_name ("export");
  create_symbol = marlais_make_name ("create");
  module_symbol = marlais_make_name ("module");
  define_module_symbol = marlais_make_name ("define-module");
  module_keyword = marlais_make_symbol ("module:");
  import_keyword = marlais_make_symbol ("import:");
  exclude_keyword = marlais_make_symbol ("exclude:");
  prefix_keyword = marlais_make_symbol ("prefix:");
  rename_keyword = marlais_make_symbol ("rename:");
  export_keyword = marlais_make_symbol ("export:");

  /* infrequently used, but important symbols */
  instance_slots_symbol = marlais_make_name ("%instance-slots");
  class_slots_symbol = marlais_make_name ("%class-slots");
  each_subclass_slots_symbol = marlais_make_name ("%each-subclass-slots");
  x_symbol = marlais_make_name ("x");

  /* initialize table of syntax operators and functions */
  marlais_initialize_syntax ();
  marlais_initialize_lexer ();
  define_test_symbol = marlais_make_name ("define-test");
  test_symbol = marlais_make_name ("test");
  aref_symbol = marlais_make_name ("aref");
  print_symbol = marlais_make_name ("print-object");
  princ_symbol = marlais_make_name ("object-princ");

  /* initialize builtin classes */
  marlais_initialize_class ();

  unspecified_object = make_unspecified_object ();

  /* make the unspecified object available */
  add_top_level_binding (marlais_make_name ("%unspecified"),
			 unspecified_object,
			 1);

  /* make the uninitialize slot value available */
  add_top_level_binding (marlais_make_name ("%uninitialized-slot-value"),
			 uninit_slot_object,
			 1);

  /* make default object */
  default_object = cons (MARLAIS_FALSE, MARLAIS_FALSE);
  add_top_level_binding (marlais_make_name ("%default-object"), default_object, 1);

  binding_stack = cons (marlais_make_integer (0), make_empty_list ());

  /* initialize primitives */
  init_env_prims ();
  init_list_prims ();
  marlais_register_symbol ();
  marlais_register_class ();
  marlais_register_slot ();
  init_file_prims ();
  init_function_prims ();
  marlais_register_values ();
  marlais_register_print ();
  marlais_register_number ();
  marlais_register_apply ();
  marlais_register_boolean ();
  marlais_register_bytestring ();
  marlais_register_vector ();
  marlais_register_error ();
  marlais_register_stream ();
  marlais_register_read ();
  marlais_register_table ();
  marlais_register_character ();
  marlais_register_deque ();
  marlais_register_array ();
  init_sys_prims ();
}
