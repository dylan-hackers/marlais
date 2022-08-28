/* globaldefs.h -- see COPYRIGHT for use */

/* These are the master definitions for all globals */

#ifndef GLOBAL
#error globaldefs.h should not be included directly
#endif

/* core constants */
#ifdef MARLAIS_OBJECT_MODEL_LARGE
GLOBAL Object marlais_true;
GLOBAL Object marlais_false;
GLOBAL Object marlais_nil;
GLOBAL Object marlais_eof;
GLOBAL Object marlais_unspecified;
GLOBAL Object marlais_uninitialized;
#endif

/* additional constants */
GLOBAL Object marlais_default;
GLOBAL Object marlais_empty_string;

/* streams */
GLOBAL Object marlais_standard_input;
GLOBAL Object marlais_standard_output;
GLOBAL Object marlais_standard_error;

/* modules */
GLOBAL Object marlais_module_dylan;
GLOBAL Object marlais_module_dylan_user;

/* kernel state */
GLOBAL struct environment *the_env;
GLOBAL struct eval_stack *eval_stack;
GLOBAL Object the_eval_obj;
GLOBAL Object   marlais_results;
GLOBAL jmp_buf *marlais_error_jump;
GLOBAL jmp_buf *marlais_tail_jump;

/* load state */
GLOBAL bool     marlais_loading;
GLOBAL Object   marlais_loading_files;

/* trace flags */
GLOBAL int marlais_trace_bindings;
GLOBAL int marlais_trace_functions;
GLOBAL int marlais_trace_primitives;
GLOBAL int marlais_trace_level;

/* important objects */
GLOBAL Object apply_symbol;
GLOBAL Object size_keyword;
GLOBAL Object fill_keyword;
GLOBAL Object key_symbol;
GLOBAL Object keyword_symbol;
GLOBAL Object required_symbol;
GLOBAL Object allkeys_symbol;
GLOBAL Object hash_rest_symbol;
GLOBAL Object next_symbol;
GLOBAL Object all_symbol;
GLOBAL Object values_symbol;
GLOBAL Object hash_values_symbol;
GLOBAL Object quote_symbol;
GLOBAL Object getter_keyword;
GLOBAL Object setter_keyword;
GLOBAL Object else_keyword;
GLOBAL Object type_keyword;
GLOBAL Object init_value_keyword;
GLOBAL Object init_function_keyword;
GLOBAL Object deferred_type_keyword;
GLOBAL Object init_keyword_keyword;
GLOBAL Object required_init_keyword_keyword;
GLOBAL Object allocation_keyword;
GLOBAL Object unwind_symbol;
GLOBAL Object next_method_symbol;
GLOBAL Object initialize_symbol;
GLOBAL Object equal_hash_symbol;
GLOBAL Object object_class_symbol;
GLOBAL Object quasiquote_symbol;
GLOBAL Object unquote_symbol;
GLOBAL Object unquote_splicing_symbol;
GLOBAL Object super_classes_keyword;
GLOBAL Object slots_keyword;
GLOBAL Object abstract_p_keyword;
GLOBAL Object debug_name_keyword;
GLOBAL Object min_keyword;
GLOBAL Object max_keyword;
GLOBAL Object dim_keyword;

GLOBAL Object instance_symbol;
GLOBAL Object class_symbol;
GLOBAL Object each_subclass_symbol;
GLOBAL Object inherited_symbol;
GLOBAL Object constant_symbol;
GLOBAL Object virtual_symbol;
GLOBAL Object obj_sym;
GLOBAL Object slot_val_sym;
GLOBAL Object set_slot_value_sym;
GLOBAL Object val_sym;
GLOBAL Object initial_state_sym;
GLOBAL Object next_state_sym;
GLOBAL Object current_element_sym;
GLOBAL Object element_symbol;
GLOBAL Object element_setter_symbol;
GLOBAL Object signal_symbol;
GLOBAL Object concatenate_symbol;

GLOBAL Object not_symbol;
GLOBAL Object colon_equal_symbol;
GLOBAL Object not_equal_symbol;
GLOBAL Object not_equal_equal_symbol;
GLOBAL Object equal_equal_symbol;
GLOBAL Object greater_equal_symbol;
GLOBAL Object lesser_equal_symbol;
GLOBAL Object or_symbol;
GLOBAL Object and_symbol;
GLOBAL Object equal_symbol;
GLOBAL Object greater_symbol;
GLOBAL Object lesser_symbol;
GLOBAL Object exponent_symbol;
GLOBAL Object divides_symbol;
GLOBAL Object times_symbol;
GLOBAL Object minus_symbol;
GLOBAL Object plus_symbol;

GLOBAL Object instance_slots_symbol;
GLOBAL Object class_slots_symbol;
GLOBAL Object each_subclass_slots_symbol;
GLOBAL Object statement_symbol;
GLOBAL Object cond_symbol;
GLOBAL Object equal_arrow_symbol;
GLOBAL Object colon_colon_symbol;

GLOBAL Object standard_input_symbol;
GLOBAL Object standard_output_symbol;
GLOBAL Object standard_error_symbol;

/* builtin classes */
GLOBAL Object marlais_class_object;
GLOBAL Object marlais_class_name;
GLOBAL Object marlais_class_boolean;
GLOBAL Object marlais_class_number;
GLOBAL Object marlais_class_complex;
GLOBAL Object marlais_class_real;
GLOBAL Object marlais_class_rational;
GLOBAL Object marlais_class_integer;
GLOBAL Object marlais_class_small_integer;
GLOBAL Object marlais_class_ratio;
GLOBAL Object marlais_class_float;
GLOBAL Object marlais_class_single_float;
GLOBAL Object marlais_class_double_float;
GLOBAL Object marlais_class_extended_float;
GLOBAL Object marlais_class_collection;
GLOBAL Object marlais_class_mutable_sequence;
GLOBAL Object marlais_class_list;
GLOBAL Object marlais_class_empty_list;
GLOBAL Object marlais_class_pair;
GLOBAL Object marlais_class_string;
GLOBAL Object marlais_class_byte_string;
GLOBAL Object marlais_class_wide_string;
GLOBAL Object marlais_class_unicode_string;
GLOBAL Object marlais_class_vector;
GLOBAL Object marlais_class_simple_vector;
GLOBAL Object marlais_class_stretchy_vector;
GLOBAL Object marlais_class_simple_object_vector;
GLOBAL Object marlais_class_byte_vector;
GLOBAL Object marlais_class_explicit_key_collection;
GLOBAL Object marlais_class_mutable_explicit_key_collection;
GLOBAL Object marlais_class_stretchy_collection;
GLOBAL Object marlais_class_mutable_collection;
GLOBAL Object marlais_class_sequence;
GLOBAL Object marlais_class_table;
GLOBAL Object marlais_class_object_table;
GLOBAL Object marlais_class_deque;
GLOBAL Object marlais_class_object_deque;
GLOBAL Object marlais_class_array;
GLOBAL Object marlais_class_object_array;
GLOBAL Object marlais_class_condition;
GLOBAL Object marlais_class_serious_condition;
GLOBAL Object marlais_class_error;
GLOBAL Object marlais_class_simple_error;
GLOBAL Object marlais_class_type_error;
GLOBAL Object marlais_class_sealed_object_error;
GLOBAL Object marlais_class_warning;
GLOBAL Object marlais_class_simple_warning;
GLOBAL Object marlais_class_restart;
GLOBAL Object marlais_class_simple_restart;
GLOBAL Object marlais_class_abort;
GLOBAL Object marlais_class_symbol;
GLOBAL Object marlais_class_character;
GLOBAL Object marlais_class_byte_character;
GLOBAL Object marlais_class_wide_character;
GLOBAL Object marlais_class_unicode_character;
GLOBAL Object marlais_class_function;
GLOBAL Object marlais_class_primitive;
GLOBAL Object marlais_class_generic_function;
GLOBAL Object marlais_class_method;
GLOBAL Object marlais_class_exit_function;
GLOBAL Object marlais_class_unwind_protect_function;
GLOBAL Object marlais_class_class;
GLOBAL Object marlais_class_class_slots;
GLOBAL Object marlais_class_object_table_entry;
GLOBAL Object marlais_class_object_deque_entry;
GLOBAL Object marlais_class_limited_int;
GLOBAL Object marlais_class_singleton;
GLOBAL Object marlais_class_subclass;
GLOBAL Object marlais_class_union;
GLOBAL Object marlais_class_type;
GLOBAL Object marlais_class_limited_type;
GLOBAL Object marlais_class_limited_integer;
GLOBAL Object marlais_class_object_handle;
GLOBAL Object marlais_class_foreign_pointer;
GLOBAL Object marlais_class_environment;
GLOBAL Object marlais_class_module;
GLOBAL Object marlais_class_stdio_handle;
GLOBAL Object marlais_class_big_float;
GLOBAL Object marlais_class_big_ratio;
GLOBAL Object marlais_class_big_integer;

/* standard character classes */

GLOBAL Object standard_character_class;
GLOBAL Object standard_string_class;

/* reserved word symbols */
GLOBAL Object above_symbol;
GLOBAL Object afterwards_symbol;
GLOBAL Object begin_symbol;
GLOBAL Object below_symbol;
GLOBAL Object block_symbol;
GLOBAL Object by_symbol;
GLOBAL Object case_symbol;
GLOBAL Object class_symbol;
GLOBAL Object cleanup_symbol;
GLOBAL Object constant_symbol;
GLOBAL Object define_symbol;
GLOBAL Object else_symbol;
GLOBAL Object elseif_symbol;
GLOBAL Object end_symbol;
GLOBAL Object exception_symbol;
GLOBAL Object finally_symbol;
GLOBAL Object for_symbol;
GLOBAL Object from_symbol;
GLOBAL Object generic_symbol;
GLOBAL Object handler_symbol;
GLOBAL Object if_symbol;
GLOBAL Object in_symbol;
GLOBAL Object let_symbol;
GLOBAL Object library_symbol;
GLOBAL Object local_symbol;
GLOBAL Object method_symbol;
GLOBAL Object function_symbol;
GLOBAL Object module_symbol;
GLOBAL Object otherwise_symbol;
GLOBAL Object select_symbol;
GLOBAL Object slot_symbol;
GLOBAL Object then_symbol;
GLOBAL Object to_symbol;
GLOBAL Object unless_symbol;
GLOBAL Object until_symbol;
GLOBAL Object until_keyword;
GLOBAL Object variable_symbol;
GLOBAL Object while_symbol;
GLOBAL Object while_keyword;
GLOBAL Object local_bind_symbol;
GLOBAL Object local_bind_rec_symbol;
GLOBAL Object unbinding_begin_symbol;
GLOBAL Object define_variable_symbol;
GLOBAL Object define_constant_symbol;
GLOBAL Object define_class_symbol;
GLOBAL Object define_generic_function_symbol;
GLOBAL Object define_method_symbol;
GLOBAL Object define_function_symbol;
GLOBAL Object seal_symbol;
GLOBAL Object set_bang_symbol;
GLOBAL Object singleton_symbol;
GLOBAL Object open_symbol;
GLOBAL Object sealed_symbol;
GLOBAL Object dynamism_keyword;
GLOBAL Object negative_symbol;
GLOBAL Object list_symbol;
GLOBAL Object pair_symbol;
GLOBAL Object variable_keyword;
GLOBAL Object to_symbol;
GLOBAL Object above_symbol;
GLOBAL Object below_symbol;
GLOBAL Object by_symbol;
GLOBAL Object range_keyword;
GLOBAL Object collection_keyword;
GLOBAL Object forward_iteration_protocol_symbol;
GLOBAL Object plus_symbol;
GLOBAL Object bind_symbol;
GLOBAL Object bind_exit_symbol;
GLOBAL Object exit_symbol;
GLOBAL Object unwind_protect_symbol;
GLOBAL Object vals_symbol;
GLOBAL Object modifiers_keyword;
GLOBAL Object abstract_symbol;
GLOBAL Object concrete_symbol;
GLOBAL Object primary_symbol;
GLOBAL Object free_symbol;
GLOBAL Object use_symbol;
GLOBAL Object export_symbol;
GLOBAL Object create_symbol;
GLOBAL Object define_module_symbol;
GLOBAL Object module_keyword;
GLOBAL Object import_keyword;
GLOBAL Object exclude_keyword;
GLOBAL Object prefix_keyword;
GLOBAL Object rename_keyword;
GLOBAL Object export_keyword;
GLOBAL Object dylan_symbol;
GLOBAL Object dylan_user_symbol;
GLOBAL Object define_test_symbol;
GLOBAL Object test_symbol;
GLOBAL Object x_symbol;

GLOBAL Object aref_symbol;
GLOBAL Object print_symbol;
GLOBAL Object princ_symbol;
