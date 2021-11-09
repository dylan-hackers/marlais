
#include <marlais/context.h>

#include <marlais/marlais.h>

extern Object binding_stack;

void
marlais_initialize (void)
{
  /* intialize garbage collector */
  marlais_initialize_gc ();

  dylan_symbol = marlais_make_name ("dylan");
  dylan_user_symbol = marlais_make_name ("dylan-user");

  marlais_set_module (marlais_new_module (dylan_symbol));

  all_symbol = marlais_make_name ("all");
  (marlais_current_module ())->exported_bindings = all_symbol;

  /* intialize global objects */
  marlais_initialize_boolean ();
  initialize_empty_list ();
  marlais_initialize_stream ();
  empty_string = marlais_make_bytestring ("");
  eof_object = make_eof_object ();
  unspecified_object = make_unspecified_object ();

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
  uninit_slot_object = make_uninit_slot ();
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

  /* make the unspecified object available */
  marlais_add_export (marlais_make_name ("%unspecified"),
                         unspecified_object,
                         1);

  /* make the uninitialize slot value available */
  marlais_add_export (marlais_make_name ("%uninitialized-slot-value"),
                         uninit_slot_object,
                         1);

  /* make default object */
  default_object = cons (MARLAIS_FALSE, MARLAIS_FALSE);
  marlais_add_export (marlais_make_name ("%default-object"), default_object, 1);

  binding_stack = cons (marlais_make_integer (0), make_empty_list ());

  /* initialize primitives */
  marlais_register_env ();
  init_list_prims ();
  marlais_register_symbol ();
  marlais_register_class ();
  marlais_register_slot ();
  marlais_register_file ();
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
