
#include <marlais/context.h>

#include <marlais/marlais.h>

#include <marlais/gc.h>
#include <marlais/lexer.h>
#include <marlais/parser.h>
#include <marlais/unicode.h>

#define DYLAN_INIT_FILE "dylan/init.dylan"
#define COMMON_INIT_FILE "common/init.dylan"

void
marlais_initialize (void)
{
  int err;
  jmp_buf errbuf;
  const char *dylan_init, *common_init;

  /* intialize garbage collector */
  marlais_initialize_gc ();

  /* initialize GMP memory management */
#ifdef MARLAIS_ENABLE_GMP
  marlais_initialize_gmp ();
#endif

  /* initialize ICU memory management */
#ifdef MARLAIS_ENABLE_ICU
  marlais_initialize_icu ();
#endif

  /* intialize core constants */
#ifdef MARLAIS_OBJECT_MODEL_LARGE
  marlais_true = marlais_allocate_object (True, sizeof (struct empty));
  marlais_false = marlais_allocate_object (False, sizeof (struct empty));
  marlais_nil = marlais_allocate_object (EmptyList, sizeof (struct empty));
  marlais_eof = marlais_allocate_object (EndOfFile, sizeof (struct empty));
  marlais_unspecified = marlais_allocate_object (UnspecifiedValue, sizeof (struct empty));
  marlais_uninitialized = marlais_allocate_object (UninitializedValue, sizeof (struct empty));
#endif

  /* initialize additional constants */
  marlais_default = marlais_cons (MARLAIS_FALSE, MARLAIS_FALSE);
  marlais_empty_string = marlais_make_bytestring ("");

  /* initialize interpreter state */
  marlais_results = MARLAIS_NIL;
  marlais_error_jump = NULL;
  marlais_tail_jump = NULL;
  marlais_loading = false;
  marlais_loading_files = MARLAIS_NIL;

  /* create core modules */
  all_symbol = marlais_make_name ("all");
  dylan_symbol = marlais_make_name ("dylan");
  dylan_user_symbol = marlais_make_name ("dylan-user");
  marlais_initialize_module();
  marlais_module_dylan = marlais_make_module (dylan_symbol);
  marlais_module_dylan_user = marlais_make_module (dylan_user_symbol);

  /* Switch to the dylan module */
  marlais_set_current_module (marlais_module_dylan);

  /* Export all bindings from dylan module */
  MODULE(marlais_get_current_module ())->exported_bindings = all_symbol;

  /* initialize the initial streams */
  marlais_initialize_stream ();

  /* initialize symbols */
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
  unwind_symbol = marlais_make_name ("%unwind");
  next_method_symbol = marlais_make_name ("next-method");
  initialize_symbol = marlais_make_name ("initialize");
  equal_hash_symbol = marlais_make_name ("=hash");
  standard_input_symbol = marlais_make_name ("*standard-input*");
  standard_output_symbol = marlais_make_name ("*standard-output*");
  standard_error_symbol = marlais_make_name ("*standard-error*");
  quasiquote_symbol = marlais_make_name ("quasiquote");
  unquote_symbol = marlais_make_name ("unquote");
  unquote_splicing_symbol = marlais_make_name ("unquote-splicing");
  element_symbol = marlais_make_name ("element");
  element_setter_symbol = marlais_make_name ("element-setter");
  signal_symbol = marlais_make_name ("signal");
  concatenate_symbol = marlais_make_name ("concatenate");
  cond_symbol = marlais_make_name ("cond");
  instance_symbol = marlais_make_name ("instance");
  class_symbol = marlais_make_name ("class");
  each_subclass_symbol = marlais_make_name ("each-subclass");
  inherited_symbol = marlais_make_name ("inherited");
  constant_symbol = marlais_make_name ("constant");
  virtual_symbol = marlais_make_name ("virtual");
  object_class_symbol = marlais_make_name ("object-class");
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
  seal_symbol = marlais_make_name ("seal");
  set_bang_symbol = marlais_make_name ("set!");
  singleton_symbol = marlais_make_name ("singleton");
  sealed_symbol = marlais_make_name ("sealed");
  open_symbol = marlais_make_name ("open");
  dynamism_keyword = marlais_make_symbol ("dynamism:");
  negative_symbol = marlais_make_name ("negative");
  list_symbol = marlais_make_name ("list");
  pair_symbol = marlais_make_name ("pair");
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
  instance_slots_symbol = marlais_make_name ("%instance-slots");
  class_slots_symbol = marlais_make_name ("%class-slots");
  each_subclass_slots_symbol = marlais_make_name ("%each-subclass-slots");
  x_symbol = marlais_make_name ("x");
  define_test_symbol = marlais_make_name ("define-test");
  test_symbol = marlais_make_name ("test");
  aref_symbol = marlais_make_name ("aref");
  print_symbol = marlais_make_name ("print-object");
  princ_symbol = marlais_make_name ("object-princ");
  obj_sym = marlais_make_name ("obj");
  slot_val_sym = marlais_make_name ("slot-value");
  set_slot_value_sym = marlais_make_name ("set-slot-value!");
  val_sym = marlais_make_name ("val");
  initial_state_sym = marlais_make_name ("initial-state");
  next_state_sym = marlais_make_name ("next-state");
  current_element_sym = marlais_make_name ("current-element");
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

  /* initialize keywords */
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
  variable_keyword = marlais_make_symbol ("variable:");
  module_keyword = marlais_make_symbol ("module:");
  import_keyword = marlais_make_symbol ("import:");
  exclude_keyword = marlais_make_symbol ("exclude:");
  prefix_keyword = marlais_make_symbol ("prefix:");
  rename_keyword = marlais_make_symbol ("rename:");
  export_keyword = marlais_make_symbol ("export:");

  /* initialize table of syntax operators and functions */
  marlais_initialize_syntax ();
  marlais_initialize_lexer ();

  /* initialize builtin classes */
  marlais_initialize_class ();

  /* export core constants */
  marlais_add_export (marlais_make_name ("%unspecified"),
                      MARLAIS_UNSPECIFIED,
                      1);
  marlais_add_export (marlais_make_name ("%uninitialized-slot-value"),
                      MARLAIS_UNINITIALIZED,
                      1);
  marlais_add_export (marlais_make_name ("%default-object"),
                      marlais_default,
                      1);

  /* determine default string and character classes */
#if defined(MARLAIS_STANDARD_CHARACTER_BYTE)
  standard_character_class = byte_character_class;
  standard_string_class = byte_string_class;
#elif defined(MARLAIS_STANDARD_CHARACTER_WIDE)
  standard_character_class = wide_character_class;
  standard_string_class = wide_string_class;
#elif defined(MARLAIS_STANDARD_CHARACTER_UNICODE)
  standard_character_class = unicode_character_class;
  standard_string_class = unicode_string_class;
#else
#error Unknown standard character class.
#endif

  /* export the standard character classes */
  marlais_add_export (marlais_make_name ("<standard-character>"), standard_character_class, 1);
  marlais_add_export (marlais_make_name ("<standard-string>"), standard_string_class, 1);

  /* initialize primitives */
  marlais_register_module ();
  marlais_register_list ();
  marlais_register_symbol ();
  marlais_register_type ();
  marlais_register_class ();
  marlais_register_slot ();
  marlais_register_file ();
  marlais_register_function ();
  marlais_register_values ();
  marlais_register_print ();
  marlais_register_number ();
  marlais_register_float ();
  marlais_register_apply ();
  marlais_register_boolean ();
  marlais_register_string ();
  marlais_register_bytevector ();
  marlais_register_vector ();
  marlais_register_error ();
  marlais_register_stream ();
  marlais_register_table ();
  marlais_register_character ();
  marlais_register_deque ();
  marlais_register_array ();
  marlais_register_sys ();
  marlais_register_gc ();

#ifdef MARLAIS_ENABLE_GMP
  marlais_register_gmp ();
#endif
#ifdef MARLAIS_ENABLE_ICU
  marlais_register_icu ();
#endif

  /* error catch for initialization code */
  err = setjmp (&errbuf);
  if (err) {
    printf ("error in initialization code -- exiting.\n");
    exit (1);
  }
  marlais_error_jump = &errbuf;

  /* load init code */
  dylan_init = getenv ("MARLAIS_DYLAN_INIT");
  if(!dylan_init) {
    dylan_init = DYLAN_INIT_FILE;
  }
  common_init = getenv ("MARLAIS_COMMON_INIT");
  if(!common_init) {
    common_init = COMMON_INIT_FILE;
  }
  marlais_load(marlais_make_bytestring (dylan_init));
  marlais_load(marlais_make_bytestring (common_init));

  /* Switch to dylan-user */
  marlais_set_current_module (marlais_module_dylan_user);

  /* Export all bindings from the dylan-user module */
  MODULE(marlais_get_current_module ())->exported_bindings = all_symbol;

  /* Make dylan-user use dylan */
  marlais_use_module (dylan_symbol,
                      all_symbol,
                      MARLAIS_NIL,
                      marlais_empty_string,
                      MARLAIS_NIL,
                      all_symbol);
}
