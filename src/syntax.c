/*

   syntax.c

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

#include <string.h>

#include "syntax.h"

#include "alloc.h"
#include "apply.h"
#include "boolean.h"
#include "bytestring.h"
#include "class.h"
#include "env.h"
#include "error.h"
#include "eval.h"
#include "keyword.h"
#include "list.h"
#include "function.h"
#include "misc.h"
#include "number.h"
#include "print.h"
#include "symbol.h"
#include "table.h"
#include "values.h"

extern Object dylan_symbol;
extern Object dylan_user_symbol;
extern Object modifiers_keyword;
extern Object abstract_symbol;
extern Object concrete_symbol;
extern Object primary_symbol;
extern Object free_symbol;
extern Object open_symbol;
extern Object sealed_symbol;
extern Object description_symbol;
extern Object error_class;
extern Object unwind_protect_symbol;

extern Object eval_combination (Object obj, int do_apply);

/* data structures */

struct syntax_entry {
    Object sym;
    syntax_fun fun;
    struct syntax_entry *next;
};

#define SYNTAX_TABLE_SIZE 1024
struct syntax_entry *syntax_table[SYNTAX_TABLE_SIZE];

/* local variables and functions */

void install_syntax_entry (char *name, syntax_fun fun);
static void bind_variables (Object init_list,
			    int top_level,
			    int constant,
			    struct frame *to_frame);
void add_variable_binding (Object var,
			   Object val,
			   int top_level,
			   int constant,
			   struct frame *to_frame);

/* functions emobodying evaluation rules for forms */

static Object and_eval (Object form);
static Object car (Object lst);
static Object begin_eval (Object form);
static Object bind_eval (Object form);
static Object bind_exit_eval (Object form);
static Object bind_methods_eval (Object form);
static Object boundp_eval (Object form);
static Object case_eval (Object form);
static Object cond_eval (Object form);
static Object define_eval (Object form);
static Object define_constant_eval (Object form);
static Object define_class_eval (Object form);
static Object define_generic_function_eval (Object form);
static Object define_method_eval (Object form);
static Object define_module_eval (Object form);
static Object define_test_eval (Object form);
static Object dotimes_eval (Object form);
static Object for_eval (Object form);
static Object get_variable (Object var_spec);
static void get_vars_and_inits (Object var_forms,
				Object *clause_types_ptr,
				Object *vars_ptr,
				Object *inits_ptr);
static void initialize_step_and_numeric_vars (Object clause_types,
					      Object vars,
					      Object inits);
static void initialize_collection_inits (Object clause_types,
					 Object vars,
					 Object inits);
static int exhausted_numeric_or_collection_clauses (Object clause_types,
						    Object vars,
						    Object inits,
						    int init_call);
static void initialize_collection_variables (Object clause_types,
					     Object vars,
					     Object inits);
static void update_explicit_and_numeric_clauses (Object clause_types,
						 Object vars,
						 Object inits);
static void update_collection_variables (Object clause_types,
					 Object vars,
					 Object inits);
static Object for_each_eval (Object form);
static Object if_eval (Object form);
static Object method_eval (Object form);
static Object or_eval (Object form);
static Object quasiquote_eval (Object form);
static Object quote_eval (Object form);
static Object select_eval (Object form);
static Object set_eval (Object form);
static Object set_module_eval (Object form);
static Object unless_eval (Object form);
static Object until_eval (Object form);
static Object unwind_protect_eval (Object form);
static Object when_eval (Object form);
static Object while_eval (Object form);
static Object local_bind_eval (Object form);
static Object local_bind_rec_eval (Object form);
static Object unbinding_begin_eval (Object form);

static Object process_test_result (Object name, Object options,
				   Object doc_string, Object result);
static Object record_failure (Object name, Object doc_string, Object result);
static Object record_success (Object name, Object doc_string, Object result);
static Object record_disabled (Object name, Object doc_string);


static char *syntax_operators[] =
{
    "and",
    "&",
    "begin",
    "bind",
    "bind-exit",
    "bind-methods",
    "bound?",
    "case",
    "cond",
    "define",
    "define-variable",
    "define-class",
    "define-constant",
    "define-generic-function",
    "define-method",
    "define-module",
    "define-test",
    "dotimes",
    "for",
    "for-each",
    "if",
    "method",
    "or",
    "|",
    "quasiquote",
    "quote",
    "select",
    "set!",
    "set-module",
    "unless",
    "until",
    "unwind-protect",
    "when",
    "while",
    "\"local-bind",
    "\"local-bind-rec",
    "\"unbinding-begin",
};

static syntax_fun syntax_functions[] =
{
    and_eval,
    and_eval,
    begin_eval,
    bind_eval,
    bind_exit_eval,
    bind_methods_eval,
    boundp_eval,
    case_eval,
    cond_eval,
    define_eval,
    define_eval,
    define_class_eval,
    define_constant_eval,
    define_generic_function_eval,
    define_method_eval,
    define_module_eval,
    define_test_eval,
    dotimes_eval,
    for_eval,
    for_each_eval,
    if_eval,
    method_eval,
    or_eval,
    or_eval,
    quasiquote_eval,
    quote_eval,
    select_eval,
    set_eval,
    set_module_eval,
    unless_eval,
    until_eval,
    unwind_protect_eval,
    when_eval,
    while_eval,
    local_bind_eval,
    local_bind_rec_eval,
    unbinding_begin_eval,
};

void
init_syntax_table (void)
{
    int numops, i;

    numops = sizeof (syntax_operators) / sizeof (char *);

    for (i = 0; i < numops; ++i) {
	install_syntax_entry (syntax_operators[i],
			      syntax_functions[i]);
    }
}

syntax_fun
syntax_function (Object sym)
{
    struct syntax_entry *entry;
    int h;

    h = ((int) sym) % SYNTAX_TABLE_SIZE;
    entry = syntax_table[h];
    while (entry) {
	if (entry->sym == sym) {
	    return (entry->fun);
	}
	entry = entry->next;
    }
    return (NULL);
}

void
install_syntax_entry (char *name, syntax_fun fun)
{
    struct syntax_entry *entry;
    Object sym;
    int h;

    sym = make_symbol (name);
    h = ((int) sym) % SYNTAX_TABLE_SIZE;
    entry = (struct syntax_entry *)
	checking_malloc (sizeof (struct syntax_entry));

    entry->sym = sym;
    entry->fun = fun;
    entry->next = syntax_table[h];
    syntax_table[h] = entry;
}

/* <pcb> a single function to evaluate bodies. uses tail_eval. */

static Object
eval_body (Object body, Object null_body_result_value)
{
    Object result = null_body_result_value;

    while (!EMPTYLISTP (body)) {
	Object next = CDR (body);

	if (EMPTYLISTP (next)) {
	    result = tail_eval (CAR (body));
	} else {
	    result = eval (CAR (body));
	}
	body = next;
    }

    return result;
}

/* functions that perform the special evaluation
   rules for syntax forms. */

static Object
and_eval (Object form)
{
    Object clauses, ret;

    clauses = CDR (form);
    while (!EMPTYLISTP (clauses)) {
	ret = eval (CAR (clauses));
	if (VALUESP (ret)) {
	    if (PAIRP (CDR (clauses))) {
		ret = FIRSTVAL (ret);
	    } else {
		return ret;
	    }
	}
	if (ret == false_object) {
	    return (false_object);
	}
	clauses = CDR (clauses);
    }
    return (ret);
}

static Object
begin_eval (Object form)
{
    return eval_body (CDR (form), unspecified_object);
}

static Object
bind_eval (Object form)
{
    Object bindings, body, result;
    struct frame *initial_env;
    struct frame *enclosing_env;
    struct frame *binding_env;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed bind form", form, NULL);
    }
    bindings = SECOND (form);
    body = CDR (CDR (form));

    /* <pcb> rather than pop the environements, just restore at the end. */
    initial_env = the_env;

    while (!EMPTYLISTP (bindings)) {
	/* <pcb> some hackery to make bind work correctly. */
	enclosing_env = the_env;
	push_scope (CAR (form));
	binding_env = the_env;
	the_env = enclosing_env;

	bind_variables (CAR (bindings), 0, 0, binding_env);
	bindings = CDR (bindings);

	/* begin the next loop iteration in this new scope. */
	the_env = binding_env;
    }

    result = eval_body (body, unspecified_object);

    /* restore the environment to its initial state. */
    the_env = initial_env;

    return (result);

}

static Object
local_bind_eval (Object form)
{
    Object bindings;
    struct frame *enclosing_env;
    struct frame *binding_env;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed local binding", form, NULL);
    }
    bindings = SECOND (form);

    /* <pcb> some hackery to make local bind work correctly. */
    enclosing_env = the_env;
    push_scope (CAR (form));
    binding_env = the_env;
    the_env = enclosing_env;

    while (!EMPTYLISTP (bindings)) {
	bind_variables (CAR (bindings), 0, 0, binding_env);
	bindings = CDR (bindings);
    }

    the_env = binding_env;
    return unspecified_object;
}

static Object
local_bind_rec_eval (Object form)
{
    Object bindings;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed local binding", form, NULL);
    }
    bindings = SECOND (form);

    push_scope (CAR (form));

    while (!EMPTYLISTP (bindings)) {
	bind_variables (CAR (bindings), 0, 0, the_env);
	bindings = CDR (bindings);
    }

    return unspecified_object;
}

/*
 * Hacked together to make infix begin work with strange "let" forms
 */
static Object
unbinding_begin_eval (Object form)
{
    int i;
    Object res;

    if (list_length (form) < 2) {
	error ("Bad unbinding-begin form", form, NULL);
    }
    i = INTVAL (SECOND (form));

    res = unspecified_object;
    form = CDR (CDR (form));
    while (PAIRP (form)) {
	Object next_form = CDR (form);

	if (EMPTYLISTP (next_form)) {
	    res = tail_eval (CAR (form));
	} else {
	    res = eval (CAR (form));
	}
	form = next_form;
    }

    while (i-- > 0) {
	pop_scope ();
    }

    return res;
}

static Object
bind_exit_eval (Object form)
{
    Object exit_obj, sym, body, ret, sec;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed bind-exit form", form, NULL);
    }
    sec = SECOND (form);
    if (!PAIRP (sec)) {
	error ("bind-exit: second argument must be a list containing a symbol", sec, NULL);
    }
    sym = CAR (sec);
    body = CDR (CDR (form));
    if (!SYMBOLP (sym)) {
	error ("bind-exit: bad exit procedure name", sym, NULL);
    }
    exit_obj = make_exit (sym);

    push_scope (CAR (form));
    add_binding (sym, exit_obj, 1, the_env);
    EXITBINDING (exit_obj) = the_env->bindings[0];
    ret = (Object) setjmp (*EXITRET (exit_obj));


    if (!ret) {
#if 1
	ret = false_object;
	while (!EMPTYLISTP (body)) {
	    ret = eval (CAR (body));
	    body = CDR (body);
	}
#else
	eval_body (body, false_object);
#endif
	pop_scope ();
	return (ret);
    } else {
	pop_scope ();
	return (ret);
    }
}

static Object
bind_methods_eval (Object form)
{
    Object specs, body, spec, ret;
    Object name, params, method_body, method;

    if (EMPTYLISTP (CDR (form))) {
	error ("bind-methods: bad form", form, NULL);
    }
    specs = SECOND (form);
    body = CDR (CDR (form));

    push_scope (CAR (form));
    /* first bind method names to dummy values */
    if (!PAIRP (specs)) {
	error ("bind-methods: First argument must be a list of method bindings",
	       specs,
	       NULL);
    }
    while (!EMPTYLISTP (specs)) {
	spec = CAR (specs);
	name = FIRST (spec);
	add_binding (name, false_object, 0, the_env);
	specs = CDR (specs);
    }

    /* now, actually make the methods */
    specs = SECOND (form);
    while (!EMPTYLISTP (specs)) {
	spec = CAR (specs);
	name = FIRST (spec);
	if (EMPTYLISTP (CDR (spec))) {
	    error ("bind-methods: incomplete method specification",
		   spec,
		   NULL);
	}
	params = SECOND (spec);
	method_body = CDR (CDR (spec));
	method = make_method (name, params, method_body, the_env, 0);
	modify_value (name, method);
	specs = CDR (specs);
    }

    ret = eval_body (body, unspecified_object);
    pop_scope ();
    return (ret);
}

static Object
boundp_eval (Object form)
{
    Object cdr = CDR (form);
    Object sym;

    if (EMPTYLISTP (cdr)) {
	error ("bound?: missing symbol", form, NULL);
    }
    sym = CAR (cdr);
    if (!SYMBOLP (sym)) {
	error ("bound?: argument must be a symbol", sym, NULL);
    }
    return (symbol_value (sym) == NULL ? false_object : true_object);
}

static Object
case_eval (Object form)
{
    Object target_form, branches, branch;
    Object match_list, consequents, ret;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed case", form, NULL);
    }
    target_form = eval (CAR (CDR (form)));

    if (EMPTYLISTP (CDR (CDR (form)))) {
	error ("malformed case", form, NULL);
    }
    branches = CDR (CDR (form));
    while (!EMPTYLISTP (branches)) {
	branch = CAR (branches);
	if (!PAIRP (branch)) {
	    error ("case: malformed branch", branch, NULL);
	}
	match_list = CAR (branch);
	if ((match_list == true_object) || (match_list == else_keyword)) {
	    consequents = CDR (branch);
	    ret = false_object;
	    while (!EMPTYLISTP (consequents)) {
		ret = eval (CAR (consequents));
		consequents = CDR (consequents);
	    }
	    return (ret);
	}
	if (!PAIRP (match_list)) {
	    error ("select: malformed test expression", match_list, NULL);
	}
	while (!EMPTYLISTP (match_list)) {
	    if (id_p (CAR (match_list), target_form, make_empty_list ())
		!= false_object) {
		consequents = CDR (branch);
		ret = false_object;
		while (!EMPTYLISTP (consequents)) {
		    ret = eval (CAR (consequents));
		    consequents = CDR (consequents);
		}
		return (ret);
	    }
	    match_list = CDR (match_list);
	}
	branches = CDR (branches);
    }
    return error ("case: no matching clause", target_form, NULL);
}

static Object
cond_eval (Object form)
{
    Object clauses, clause, test, ret;

    clauses = CDR (form);
    while (!EMPTYLISTP (clauses)) {
	clause = CAR (clauses);
	test = CAR (clause);
	ret = eval (test);
	if (VALUESP (ret)) {
	    ret = FIRSTVAL (ret);
	}
	if (ret != false_object) {
	    clause = CDR (clause);
	    return eval_body (clause, ret);
	}
	clauses = CDR (clauses);
    }
    return (false_object);
}

static Object
define_eval (Object form)
{
    if (EMPTYLISTP (CDR (form)) || EMPTYLISTP (CDR (CDR (form)))) {
	error ("DEFINE form requires at least two args: (define {<var>} <init>)", form, NULL);
    } else {
	bind_variables (CDR (form), 1, 0, the_env);
    }
    return unspecified_object;
}


static Object
define_constant_eval (Object form)
{
    if (EMPTYLISTP (CDR (form)) || EMPTYLISTP (CDR (CDR (form)))) {
	error ("DEFINE form requires at least two args: (define {<var>} <init>)", form, NULL);
    } else {
	bind_variables (CDR (form), 1, 1, the_env);
    }
    return SECOND (form);
}

static void
bind_variables (Object init_list,
		int top_level,
		int constant,
		struct frame *to_frame)
{
    Object variable, variables, init, val;
    Object first, last, new;
    int i, value_count;

    if (!PAIRP (init_list) || EMPTYLISTP (CDR (init_list))) {
	error ("Initializer list requires at least two elements",
	       init_list, NULL);
    }
    variables = init = init_list;
    while (!EMPTYLISTP (CDR (init))) {
	init = CDR (init);
    }
    val = eval (CAR (init));
    if (VALUESP (val)) {
	value_count = 0;
	while (variables != init) {
	    variable = CAR (variables);
	    if (variable == hash_rest_symbol) {
		variable = SECOND (variables);
		last = NULL;
		first = make_empty_list ();
		/* bind rest values */
		for (i = value_count; i < VALUESNUM (val); ++i) {
		    new = cons (VALUESELS (val)[i], make_empty_list ());
		    if (last) {
			CDR (last) = new;
		    } else {
			first = new;
		    }
		    last = new;
		}
		if (top_level) {
		    add_top_level_binding (variable, first, constant);
		} else {
		    add_binding (variable, first, constant, to_frame);
		}
		/* check for no variables after #rest */
		if (CDR (CDR (variables)) != init) {
		    error ("Badly placed #rest specifier", init_list, NULL);
		}
		/* finished with bindings */
		break;
	    } else {
		/* check for not enough inits */
		if (value_count < VALUESNUM (val)) {
		    new = VALUESELS (val)[value_count];
		} else {
		    new = false_object;
		}
		add_variable_binding (variable,
				      new,
				      top_level,
				      constant,
				      to_frame);
		value_count++;
	    }
	    variables = CDR (variables);
	}
    } else {
	/* init is not a values object */
	if (CAR (variables) == hash_rest_symbol) {
	    add_variable_binding (SECOND (variables),
				  cons (val, make_empty_list ()),
				  top_level,
				  constant,
				  to_frame);
	} else {
	    add_variable_binding (CAR (variables),
				  val,
				  top_level,
				  constant,
				  to_frame);
	    for (variables = CDR (variables);
		 variables != init;
		 variables = CDR (variables)) {
		add_variable_binding (CAR (variables),
				      false_object,
				      top_level,
				      constant,
				      to_frame);
	    }
	}
    }
}

void
add_variable_binding (Object var,
		      Object val,
		      int top_level,
		      int constant,
		      struct frame *to_frame)
{
    Object type;

    if (PAIRP (var)) {
	if (!PAIRP (CDR (var))) {
	    error ("badly formed variable", var, NULL);
	}
	type = eval (SECOND (var));
	if (!instance (type, type_class)) {
	    error ("badly formed variable", var, NULL);
	}
    } else {
	type = object_class;
    }
    if (top_level) {
	/* add_top_level_binding can't easily check type match.
	 * do it here.
	 */
	if (!instance (val, type)) {
	    error ("initial value does not satisfy type constraint",
		   val,
		   type,
		   NULL);
	}
	add_top_level_binding (var, val, constant);
    } else {
	add_binding (var, val, constant, to_frame);
    }
}

static Object
define_class_eval (Object form)
{
    Object name, supers, slots, class, obj, modifiers, modifier;

    /*
     * Assume a class to be concrete, sealed, and free unless otherwise
     * specified
     */

    Object tmp_form = form;
    int abstract_class = 0, abstract_concrete_seen = 0;
    int open_class = 0, open_sealed_seen = 0;
    int primary_class = 0, primary_free_seen = 0;

    if (EMPTYLISTP (CDR (tmp_form))) {
	error ("malfored define-class (no arguments)", form, NULL);
    }
    tmp_form = CDR (tmp_form);
    if (PAIRP (CAR (tmp_form))) {
	modifiers = CAR (tmp_form);
	if (CAR (modifiers) != modifiers_keyword) {
	    error ("malformed define-class (bad modifiers)", form, NULL);
	}
	for (modifiers = CDR (modifiers);
	     PAIRP (modifiers);
	     modifiers = CDR (modifiers)) {
	    modifier = CAR (modifiers);
	    if (modifier == abstract_symbol || modifier == concrete_symbol) {
		if (abstract_concrete_seen) {
		    error ("redundant or conflicting modifier given to define-class",
			   modifier, NULL);
		}
		abstract_concrete_seen = 1;
		abstract_class = (modifier == abstract_symbol);
	    }
	    if (modifier == primary_symbol || modifier == free_symbol) {
		if (primary_free_seen) {
		    error ("redundant or conflicting modifier given to define-class",
			   modifier, NULL);
		}
		primary_free_seen = 1;
		primary_class = (modifier == primary_symbol);
	    }
	    if (modifier == open_symbol || modifier == sealed_symbol) {
		if (open_sealed_seen) {
		    error ("redundant or conflicting modifier given to define-class",
			   modifier, NULL);
		}
		open_sealed_seen = 1;
		open_class = (modifier == open_symbol);
	    }
	}
	tmp_form = CDR (tmp_form);

    }
    name = CAR (tmp_form);
    tmp_form = CDR (tmp_form);
    if (EMPTYLISTP (tmp_form)) {
	error ("malformed define-class (no superclass)", form, NULL);
    }
    /*
     * Must introduce binding for the class before eval'ing the slot definitions.
     */
    obj = allocate_object (sizeof (struct class));

    CLASSTYPE (obj) = Class;
    CLASSNAME (obj) = name;
    add_top_level_binding (name, obj, 0);
    supers = map (eval, CAR (tmp_form));
    slots = slot_descriptor_list (CDR (tmp_form), 1);
    make_getter_setter_gfs (slots);
    class = make_class (obj, supers, slots, NULL);

    /* kludge to put these here.  Better to add a param to make_class. */
    CLASSPROPS (class) |= CLASSSLOTSUNINIT;

    if (abstract_class) {
	make_uninstantiable (class);
    }
    if (!open_class) {
/*
 * Need to address sealed vs. open classes with library additions.
 */
/*
   seal (class);
 */
    }
    if (primary_class) {
	make_primary (class);
    }
    return (name);
}

static Object
define_generic_function_eval (Object form)
{
    Object name, params, gf;

    if (EMPTYLISTP (CDR (form))) {
	error ("define-generic-function: missing name", form, NULL);
    }
    name = SECOND (form);
    if (EMPTYLISTP (CDR (CDR (form)))) {
	error ("define-generic-function: missing parameters", form, NULL);
    }
    params = THIRD (form);

    gf = make_generic_function (name, params, make_empty_list ());
    add_top_level_binding (name, gf, 0);
    return (unspecified_object);
}

static Object
define_method_eval (Object form)
{
    Object name, params, body, method;

    if (EMPTYLISTP (CDR (form))) {
	error ("define-method: missing name", form, NULL);
    }
    name = SECOND (form);
    if (!SYMBOLP (name)) {
	error ("define-method: first argument must be a symbol", name, NULL);
    }
    if (EMPTYLISTP (CDR (CDR (form)))) {
	error ("define-method: missing parameter list", form, NULL);
    }
    params = THIRD (form);
    if (!LISTP (params)) {
	error ("define-method: second argument must be a parameter list", params, NULL);
    }
    body = CDR (CDR (CDR (form)));
    method = make_method (name, params, body, the_env, 1);
    return (name);
}

static Object
define_module_eval (Object form)
{
    Object clauses, clause;
    Object module_name;
    Object option;
    struct module_binding *the_module;

    /* Bogus for now */
    if (PAIRP (form) && list_length (form) >= 2 && SYMBOLP (SECOND (form))) {
	the_module = new_module (SECOND (form));
	clauses = CDR (CDR (form));

	while (PAIRP (clauses)) {
	    clause = CAR (clauses);
	    if (PAIRP (clause)) {
		if (CAR (clause) == use_symbol) {
		    Object imports = all_symbol;
		    Object exclusions = make_empty_list ();
		    Object prefix = empty_string;
		    Object renames = make_empty_list ();
		    Object exports = make_empty_list ();

		    int imports_specified = 0;
		    int exclusions_specified = 0;
		    int prefix_specified = 0;
		    int renames_specified = 0;
		    int exports_specified = 0;
		    struct module_binding *old_module;

		    if (list_length (clause) >= 2) {
			module_name = SECOND (clause);
			clause = CDR (CDR (clause));
			while (PAIRP (clause)) {
			    option = CAR (clause);
			    if (PAIRP (option)) {
				if (CAR (option) == import_keyword &&
				    !imports_specified) {
				    imports = CDR (option);
				    imports_specified = 1;
				} else if (CAR (option) == exclude_keyword &&
					   !exclusions_specified) {
				    exclusions = CDR (option);
				    exclusions_specified = 1;
				} else if (CAR (option) == prefix_keyword &&
					   !prefix_specified) {
				    prefix = CDR (option);
				    prefix_specified = 1;
				} else if (CAR (option) == rename_keyword &&
					   !renames_specified) {
				    renames = CDR (option);
				    renames_specified = 1;
				} else if (CAR (option) == export_keyword &&
					   !exports_specified) {
				    exports = CDR (option);
				    exports_specified = 1;
				} else {
				    error ("use clause: unknown option",
					   option,
					   NULL);
				}

			    } else {
				error ("use clause: poorly formed option",
				       CAR (clause),
				       NULL);
			    }
			    clause = CDR (clause);
			}
			if (imports_specified && exclusions_specified) {
			    error ("Define module: Can't specify both imports: and exclusions:",
				   clause,
				   NULL);
			}
			old_module = set_module (the_module);
			use_module (module_name,
				    imports,
				    exclusions,
				    prefix,
				    renames,
				    exports);
			set_module (old_module);
		    } else {
			error ("define-module: Bad use clause",
			       clause,
			       NULL);
		    }
		} else if (CAR (clause) == export_symbol) {
		    fill_table_from_property_set (the_module->exported_bindings,
						  CDR (clause));
		} else if (CAR (clause) == create_symbol) {
		    /*
		     * Aside from this, it's not clear to me (jnw) what
		     * needs to be done for create clause.
		     */
		    fill_table_from_property_set (the_module->exported_bindings,
						  CDR (clause));
		} else {
		    error ("define-module: Bad clause",
			   clause,
			   NULL);
		}
	    } else {
		error ("define-module: Bad clause",
		       clause,
		       NULL);
	    }
	    clauses = CDR (clauses);
	}
    } else {
	error ("define-module: Bad argument list",
	       form,
	       NULL);
    }
    return unspecified_object;
}

static Object
dotimes_eval (Object form)
{
    Object clause, var, intval, resform, body, res;
    int i;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed dotimes expression", form, NULL);
    }
    clause = CAR (CDR (form));
    if (!PAIRP (clause)) {
	error ("second arg to dotimes must be a list", clause, NULL);
    }
    var = CAR (clause);
    if (!SYMBOLP (var)) {
	error ("dotimes: first value in spec clause must be a symbol", var, NULL);
    }
    if (EMPTYLISTP (CDR (clause))) {
	error ("dotimes: must specifiy an upper bound", form, NULL);
    }
    intval = eval (CAR (CDR (clause)));
    if (!INTEGERP (intval)) {
	error ("dotimes: upper bound must an integer", intval, NULL);
    }
    if (!EMPTYLISTP (CDR (CDR (clause)))) {
	resform = CAR (CDR (CDR (clause)));
    } else {
	resform = NULL;
    }

    push_scope (CAR (form));
    add_binding (var, false_object, 0, the_env);
    for (i = 0; i < INTVAL (intval); ++i) {
	change_binding (var, make_integer (i));
	body = CDR (CDR (form));
	while (!EMPTYLISTP (body)) {
	    res = eval (CAR (body));
	    body = CDR (body);
	}
    }
    if (resform) {
	res = eval (resform);
    } else {
	res = false_object;
    }
    pop_scope ();
    return (res);
}



static Object
for_eval (Object form)
{
    /* The classic syntax of a for is

     *  (for ((var-name-1 init-1 step-1) ... (var-name-2 init-2 step-2))
     *       (test result-1 ... result-n)
     *       expr-1 ... expr-n)
     *
     * We have to get the new syntax into a mild variant of this
     * original form.  The new syntax is
     *
     *  'for' ( clauses [{'until' | 'while'} end-test] )
     *    body
     *    ['finally' result-body]
     *  'end' ['for'] into this form.
     *
     * Where each of the `clauses' is of one of the following forms:
     *
     *  variable '=' init-value 'then' next-value
     *  variable 'in' collection
     *  variable 'from' start [{ 'to' | 'above' | 'below'} bound]
     *                        ['by' increment]
     *
     * Note that a variable is either
     *  variable-name or
     *  '(' variable-name '::' type ')'
     *
     * I decided to change the underlying form as follows:
     *
     * ('for' (clause-1 ... clause-n)
     *        (test result-1 result-n)
     *        expr-1 ... expr-n)
     *
     * Where clause-1 ... clause-n are each of the form
     *
     *  (variable init step)
     *  ('collection:' variable collection)
     *  ('range:' variable start [{'to' | 'above' | 'below'} bound]
     *                           ['by' increment])
     *
     * A 'while' specification will cause the end-test to be logically
     * negated in the generated for.
     * The result-body of the 'finally' specification shall provide the
     * single result expression.
     *
     * Some extra work is done to achieve compliance with the rules concerning
     * evaluation order of the expressions and availability of the variables
     * in the result-body.
     */

    Object var_forms, test_form, return_forms;
    Object clause_types, vars, inits, body, ret;

    if ((!PAIRP (CDR (form))) ||
	(!PAIRP (CDR (CDR (form)))) ||
	(!PAIRP (THIRD (form)))) {
	error ("malformed FOR", form, NULL);
    }
    test_form = FIRST (THIRD (form));
    return_forms = CDR (THIRD (form));

    var_forms = SECOND (form);

    /* IRM Pg. 33 Step 1 */

    clause_types = vars = inits = make_empty_list ();

    get_vars_and_inits (var_forms, &clause_types, &vars, &inits);

    /* IRM Step 2 */
    push_scope (CAR (form));
    initialize_step_and_numeric_vars (clause_types, vars, inits);

    /* IRM Step 3 */
    initialize_collection_inits (clause_types, vars, inits);
    if (!exhausted_numeric_or_collection_clauses (clause_types,
						  vars,
						  inits,
						  1)) {

	/* IRM Step 4 */

	push_scope (CAR (form));
	initialize_collection_variables (clause_types, vars, inits);

	do {
	    /* IRM Step 5 */
	    if (eval (test_form) != false_object) {
		break;
	    }
	    /* IRM Step 6 */
	    body = CDR (CDR (CDR (form)));
	    while (!EMPTYLISTP (body)) {
		eval (CAR (body));
		body = CDR (body);
	    }

	    /* IRM Steps 7 and 8 */
	    update_explicit_and_numeric_clauses (clause_types,
						 vars,
						 inits);

	    /* IRM Step 3 (again) */
	    if (exhausted_numeric_or_collection_clauses (clause_types,
							 vars,
							 inits,
							 0)) {
		break;
	    }
	    update_collection_variables (clause_types, vars, inits);
	} while (1);
	pop_scope ();		/* To get rid of collection variables */
    }
    if (!PAIRP (return_forms)) {
	ret = false_object;
    } else {
	while (PAIRP (return_forms)) {
	    ret = eval (CAR (return_forms));
	    return_forms = CDR (return_forms);
	}
    }
    pop_scope ();
    return ret;
}

static Object
get_variable (Object var_spec)
{
    if ((PAIRP (var_spec) && (list_length (var_spec) != 2)) &&
	(!SYMBOLP (var_spec))) {
	error ("Bad variable specification", var_spec, NULL);
    }
    return var_spec;
}

static Object
variable_name (Object var_spec)
{
    return (PAIRP (var_spec)) ? CAR (var_spec) : var_spec;
}

static void
get_vars_and_inits (Object var_forms,
		    Object *clause_types_ptr,
		    Object *vars_ptr,
		    Object *inits_ptr)
{
    Object var_form, var_spec;
    Object clause_type, var, init;
    Object rest, by, start, termination, bound, negative;

    while (PAIRP (var_forms)) {
	var_form = CAR (var_forms);
	var_spec = CAR (var_form);
	if (PAIRP (var_spec) || SYMBOLP (var_spec)) {

	    /* Explicit Step Clause: init is of form
	     *    (init-value . next-value)
	     */

	    clause_type = variable_keyword;
	    var = get_variable (var_spec);
	    if (list_length (var_form) != 3) {
		error ("for: Bad variable initialization",
		       var_form,
		       NULL);
	    }
	    init = cons (eval (SECOND (var_form)), THIRD (var_form));
	} else if (var_spec == range_keyword) {

	    /* Numeric Clause: init is of form
	     * (start-value increment negative termination bound)
	     *
	     * negative is #t is increment < 0, #f otherwise
	     * termination is to:, above:, or below:
	     */

	    clause_type = range_keyword;
	    if (list_length (var_form) < 3) {
		error ("for: Bad numeric clause specification", var_form, NULL);
	    }
	    var = get_variable (SECOND (var_form));
	    rest = CDR (CDR (var_form));

	    by = make_integer (1);
	    termination = false_object;
	    start = eval (CAR (rest));
	    rest = CDR (rest);
	    bound = false_object;
	    if (PAIRP (rest)) {
		termination = CAR (rest);
		if (PAIRP (CDR (rest)) &&
		 (termination == to_symbol || termination == above_symbol ||
		  termination == below_symbol)) {
		    bound = CAR (CDR (rest));
		    rest = CDR (CDR (rest));
		} else {
		    error ("for: badly formed numeric clause", var_form, NULL);
		}
	    }
	    if (PAIRP (rest)) {
		if (PAIRP (CDR (rest)) && CAR (rest) == by_symbol) {
		    by = eval (CAR (CDR (rest)));
		} else {
		    error ("for: badly formed numeric clause", var_form, NULL);
		}
	    }
	    switch (TYPE (by)) {
	    case Integer:
		negative = (INTVAL (by) >= 0) ? false_object : true_object;
		break;
	    case DoubleFloat:
		negative = (DFLOATVAL (by) >= 0) ? false_object : true_object;
		break;
	    default:
		error ("for: numeric clause has unsupported increment type",
		       by,
		       NULL);
	    }

	    init = listem (start, by, negative, termination, bound, NULL);

	} else if (var_spec == collection_keyword) {

	    /* Collection Clause: init value is
	     *  (protocol collection state)
	     */

	    clause_type = collection_keyword;
	    if (list_length (var_form) != 3) {
		error ("for: Bad collection clause specification",
		       var_form,
		       NULL);
	    }
	    var = get_variable (SECOND (var_form));
	    init = listem (make_empty_list (),
			   eval (THIRD (var_form)),
			   make_empty_list (),
			   NULL);
	}
	*clause_types_ptr = cons (clause_type, make_empty_list ());
	*vars_ptr = cons (var, make_empty_list ());
	*inits_ptr = cons (init, make_empty_list ());

	clause_types_ptr = &CDR (*clause_types_ptr);
	vars_ptr = &CDR (*vars_ptr);
	inits_ptr = &CDR (*inits_ptr);

	var_forms = CDR (var_forms);
    }
}

static void
initialize_step_and_numeric_vars (Object clause_types,
				  Object vars,
				  Object inits)
{
    while (PAIRP (clause_types)) {
	if (CAR (clause_types) == variable_keyword) {
	    /* explicit step clause */
	    add_binding (CAR (vars), CAR (CAR (inits)), 0, the_env);
	} else if (CAR (clause_types) == range_keyword) {
	    add_binding (CAR (vars), CAR (CAR (inits)), 0, the_env);
	}
	clause_types = CDR (clause_types);
	vars = CDR (vars);
	inits = CDR (inits);
    }
}

/*
 * Surgically alters `inits'!
 * Stuffs the forward-iteration-protocol for the numeric and collection
 * clauses into the CAR of the corresponding init.
 */
static void
initialize_collection_inits (Object clause_types,
			     Object vars,
			     Object inits)
{
    Object clause_type, protocol;

    while (PAIRP (clause_types)) {
	clause_type = CAR (clause_types);
	if (clause_type == collection_keyword) {
	    protocol = eval (cons (forward_iteration_protocol_symbol,
				   cons (cons (quote_symbol,
					       cons (SECOND (CAR (inits)),
						     make_empty_list ())),
					 make_empty_list ())));
	    CAR (CAR (inits)) = protocol;
	    THIRD (CAR (inits)) = VALUESELS (protocol)[0];
	}
	clause_types = CDR (clause_types);
	vars = CDR (vars);
	inits = CDR (inits);
    }
}


/*
 * Evaluates finished-state (value 3) applied to collection
 * initial-state (value 0) and limit (value 0) to determine whether or
 * not the collection is exhausted.
 */


static int
exhausted_numeric_or_collection_clauses (Object clause_types,
					 Object vars,
					 Object inits,
					 int init_call)
{
    Object clause_type;
    Object protocol;
    Object init, current, increment, negative, termination, bound;

    while (PAIRP (clause_types)) {
	clause_type = CAR (clause_types);
	if (clause_type == collection_keyword) {
	    protocol = FIRST (CAR (inits));

	    /* (finished-state? collection state limit) */
	    if (!init_call) {
		/* Bump to the next state to see if it exists */
		THIRD (CAR (inits)) = apply (VALUESELS (protocol)[2],
					     cons (SECOND (CAR (inits)),
						   cons (THIRD (CAR (inits)),
						      make_empty_list ())));
	    }
	    if (true_object == apply (VALUESELS (protocol)[3],
				      cons (SECOND (CAR (inits)),
					    cons (THIRD (CAR (inits)),
					      cons (VALUESELS (protocol)[1],
						    make_empty_list ()))))) {
		return 1;
	    }
	} else if (clause_type == range_keyword) {

	    init = CAR (inits);
	    current = CAR (init);	/* FIRST */
	    init = CDR (init);
	    increment = CAR (init);	/* SECOND */
	    init = CDR (init);
	    negative = CAR (init);	/* THIRD */
	    init = CDR (init);
	    termination = CAR (init);	/* FOURTH */
	    init = CDR (init);
	    bound = CAR (init);	/* FIFTH */

	    if (termination == false_object) {
		/* do nothing */
	    } else if (termination == to_symbol) {
		if (negative == true_object) {
		    if (true_object == eval (listem (lesser_symbol,
						     current,
						     bound,
						     NULL))) {
			return 1;
		    }
		} else if (true_object == eval (listem (greater_symbol,
							current,
							bound,
							NULL))) {
		    return 1;
		}
	    } else if (termination == above_symbol) {
		if (true_object == eval (listem (lesser_equal_symbol,
						 current,
						 bound,
						 NULL))) {
		    return 1;
		}
	    } else if (termination == below_symbol) {
		if (true_object == eval (listem (greater_equal_symbol,
						 current,
						 bound,
						 NULL))) {
		    return 1;
		}
	    }
	}
	clause_types = CDR (clause_types);
	vars = CDR (vars);
	inits = CDR (inits);
    }
    return 0;
}


static void
initialize_collection_variables (Object clause_types,
				 Object vars,
				 Object inits)
{
    Object protocol;

    while (PAIRP (clause_types)) {
	if (CAR (clause_types) == collection_keyword) {
	    protocol = FIRST (CAR (inits));

	    /* (set! var (current-element collection state)) */
	    add_binding (CAR (vars),
			 apply (VALUESELS (protocol)[5],
				cons (SECOND (CAR (inits)),
				      cons (THIRD (CAR (inits)),
					    make_empty_list ()))),
			 0,
			 the_env);
	}
	clause_types = CDR (clause_types);
	vars = CDR (vars);
	inits = CDR (inits);
    }
}

static void
update_explicit_and_numeric_clauses (Object clause_types,
				     Object vars,
				     Object inits)
{
    Object vars_copy, new_values, *new_values_ptr, new_value;
    Object clause_type;

    vars_copy = vars;
    new_values_ptr = &new_values;

    while (PAIRP (clause_types)) {
	new_value = make_empty_list ();
	clause_type = CAR (clause_types);
	if (clause_type == variable_keyword) {
	    new_value = eval (CDR (CAR (inits)));
	} else if (clause_type == range_keyword) {
	    /* Set new of var generated by range to
	     *  (+ var increment)
	     */
	    new_value = eval (listem (plus_symbol,
				      variable_name (CAR (vars)),
				      SECOND (CAR (inits)),
				      NULL));

	    FIRST (CAR (inits)) = new_value;
	}
	*new_values_ptr = cons (new_value, make_empty_list ());

	new_values_ptr = &CDR (*new_values_ptr);
	clause_types = CDR (clause_types);
	vars = CDR (vars);
	inits = CDR (inits);

    }

    /* Do the bindings */
    while (PAIRP (vars_copy)) {
	if (!EMPTYLISTP (new_values)) {
	    modify_value (variable_name (CAR (vars_copy)),
			  CAR (new_values));
	    vars_copy = CDR (vars_copy);
	    new_values = CDR (new_values);
	}
    }
}

static void
update_collection_variables (Object clause_types,
			     Object vars,
			     Object inits)
{
    Object protocol;

    while (PAIRP (clause_types)) {
	if (CAR (clause_types) == collection_keyword) {
	    protocol = FIRST (CAR (inits));

	    /* (set! var (current-element collection state)) */
	    modify_value (CAR (vars),
			  apply (VALUESELS (protocol)[5],
				 cons (SECOND (CAR (inits)),
				       cons (THIRD (CAR (inits)),
					     make_empty_list ()))));
	}
	clause_types = CDR (clause_types);
	vars = CDR (vars);
	inits = CDR (inits);
    }
}

/*
   The iteration is terminated if any collection is exhausted 
   (in which case #f is returned) or if the end-test evaluates 
   to #t (in which case the result forms are evaluated and the
   value of the last is returned).
 */
static Object
for_each_eval (Object form)
{
    Object test_form, return_forms, var_forms;
    Object vars, collections, states, vals, body, ret, temp_vars;
    Object init_state_fun, next_state_fun, cur_el_fun;

    init_state_fun = symbol_value (initial_state_sym);
    if (!init_state_fun) {
	error ("for-each: no initial-state function defined", NULL);
    }
    next_state_fun = symbol_value (next_state_sym);
    if (!next_state_fun) {
	error ("for-each: no next-state function defined", NULL);
    }
    cur_el_fun = symbol_value (current_element_sym);
    if (!cur_el_fun) {
	error ("for-each: no current-element function defined", NULL);
    }
    if (EMPTYLISTP (CDR (form))) {
	error ("malformed FOR-EACH", form, NULL);
    }
    if (EMPTYLISTP (CDR (CDR (form)))) {
	error ("malformed FOR-EACH", form, NULL);
    }
    test_form = FIRST (THIRD (form));
    return_forms = CDR (THIRD (form));

    var_forms = SECOND (form);
    vars = map (car, var_forms);
    collections = map (second, var_forms);
    collections = map (eval, collections);
    states = list_map1 (init_state_fun, collections);

    if (member (false_object, states)) {
	return (false_object);
    }
    vals = list_map2 (cur_el_fun, collections, states);
    push_scope (CAR (form));
    add_bindings (vars, vals, 0, the_env);

    while (eval (test_form) == false_object) {
	body = CDR (CDR (CDR (form)));
	while (!EMPTYLISTP (body)) {
	    eval (CAR (body));
	    body = CDR (body);
	}
	states = list_map2 (next_state_fun, collections, states);
	if (member (false_object, states)) {
	    pop_scope ();
	    return (false_object);
	}
	vals = list_map2 (cur_el_fun, collections, states);

	/* modify bindings */
	temp_vars = vars;
	while (!EMPTYLISTP (temp_vars)) {
	    modify_value (CAR (temp_vars), CAR (vals));
	    temp_vars = CDR (temp_vars);
	    vals = CDR (vals);
	}
    }

    if (EMPTYLISTP (return_forms)) {
	return (false_object);
    } else {
	ret = eval_body (return_forms, false_object);
    }
    pop_scope ();
    return (ret);
}

static Object
if_eval (Object form)
{
    Object testval, thenform, elseform;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed if expression", form, NULL);
    }
    testval = SECOND (form);
    if (EMPTYLISTP (CDR (CDR (form)))) {
	error ("malformed if expression", form, NULL);
    }
    thenform = THIRD (form);
    if (EMPTYLISTP (CDR (CDR (CDR (form))))) {
	error ("if expression must have else clause", form, NULL);
    }
    elseform = FOURTH (form);
    if (!EMPTYLISTP (CDR (CDR (CDR (CDR (form)))))) {
	error ("if: too many arguments", NULL);
    }
    testval = eval (testval);

    if (testval == false_object) {
	return tail_eval (elseform);
    } else {
	return tail_eval (thenform);
    }
}

static Object
method_eval (Object form)
{
    Object params, body, method;

    if (EMPTYLISTP (CDR (form))) {
	error ("method: missing parameters", form, NULL);
    }
    params = SECOND (form);
    body = CDR (CDR (form));
    method = make_method (NULL, params, body, the_env, 0);
    return (method);
}

static Object
or_eval (Object form)
{
    Object clauses, ret;

    clauses = CDR (form);
    while (!EMPTYLISTP (clauses)) {
	if (EMPTYLISTP (CDR (clauses))) {
	    return tail_eval (CAR (clauses));
	}
	ret = eval (CAR (clauses));
	if (VALUESP (ret)) {
	    if (PAIRP (CDR (clauses))) {
		ret = FIRSTVAL (ret);
	    } else {
		return (ret);
	    }
	}
	if (ret != false_object) {
	    return (ret);
	}
	clauses = CDR (clauses);
    }
    return (false_object);
}

static Object qq_help (Object skel);

static Object
quasiquote_eval (Object form)
{
    return qq_help (SECOND (form));
}

static Object
qq_help (Object skel)
{
    Object head, tmp, tail;

    if (EMPTYLISTP (skel) || SYMBOLP (skel) || !PAIRP (skel)) {
	return skel;
    } else {
	head = skel;
	tail = CDR (skel);
	if (CAR (head) == unquote_symbol) {
	    if (!EMPTYLISTP (tail)) {
		if (!EMPTYLISTP (CDR (tail))) {
		    error ("Too many arguments to unquote", NULL);
		}
		return eval (CAR (tail));
	    } else {
		return error ("missing argument to unquote", NULL);
	    }
	} else if (PAIRP (CAR (head))
		   && CAR (CAR (head)) == unquote_splicing_symbol) {

	    if (!EMPTYLISTP (CDR (CAR (head)))) {
		tmp = eval (CAR (CDR (CAR (head))));
		CAR (head) = CAR (tmp);
		CDR (head) = CDR (tmp);
		tmp = head;
		while (!EMPTYLISTP (CDR (tmp))) {
		    tmp = CDR (tmp);
		}
		CDR (tmp) = qq_help (tail);
		return head;
	    } else {
		return error ("missing argument to unquote_splicing", NULL);
	    }
	} else {
	    return cons (qq_help (CAR (head)), qq_help (tail));
	}
    }
}

static Object
quote_eval (Object form)
{
    return (SECOND (form));
}

static Object
select_eval (Object form)
{
    Object target_form, test, branches, branch;
    Object match_list, consequents, ret;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed select", form, NULL);
    }
    target_form = eval (CAR (CDR (form)));

    if (EMPTYLISTP (CDR (CDR (form)))) {
	error ("malformed select", form, NULL);
    }
    test = eval (CAR (CDR (CDR (form))));

    if (EMPTYLISTP (CDR (CDR (CDR (form))))) {
	error ("malformed select", form, NULL);
    }
    branches = CDR (CDR (CDR (form)));
    while (!EMPTYLISTP (branches)) {
	branch = CAR (branches);
	if (!PAIRP (branch)) {
	    error ("select: malformed branch", branch, NULL);
	}
	match_list = CAR (branch);
	if ((match_list == true_object) || (match_list == else_keyword)) {
	    consequents = CDR (branch);
	    while (!EMPTYLISTP (consequents)) {
		ret = eval (CAR (consequents));
		consequents = CDR (consequents);
	    }
	    return (ret);
	}
	if (!PAIRP (match_list)) {
	    error ("select: malformed test expression", match_list, NULL);
	}
	while (!EMPTYLISTP (match_list)) {
	    ret = false_object;
	    if (apply (test, listem (target_form, eval (CAR (match_list)),
				     NULL)) != false_object) {
		consequents = CDR (branch);
		while (!EMPTYLISTP (consequents)) {
		    ret = eval (CAR (consequents));
		    consequents = CDR (consequents);
		}
		return (ret);
	    }
	    match_list = CDR (match_list);
	}
	branches = CDR (branches);
    }
    return (false_object);
}

static Object
set_eval (Object form)
{
    Object sym, val;

    if (EMPTYLISTP (CDR (form))) {
	error ("set!: missing forms", form, NULL);
    }
    sym = SECOND (form);

    if (PAIRP (sym)) {
	/*
	 * <pcb> let's keep things in the spirit of the old language.
	 * (set! (slot obj ...) new-value) should become
	 * (slot-setter new-value obj ...)
	 */
	return eval (cons (make_setter_symbol (CAR (sym)),
			   devalue (cons (THIRD (form), CDR (sym)))));

    }
    if (EMPTYLISTP (CDR (CDR (form)))) {
	error ("set!: missing forms", form, NULL);
    }
    val = devalue (eval (THIRD (form)));
    modify_value (sym, val);
    return (val);
}

static Object
set_module_eval (Object form)
{
    if (PAIRP (form) && list_length (form) == 2 && KEYWORDP (SECOND (form))) {
	return user_set_module (devalue (CDR (form)));
    } else {
	error ("set_module: argument list not a single symbol",
	       form,
	       NULL);
    }
    return unspecified_object;
}

static Object
unless_eval (Object form)
{
    Object test, body;

    if (EMPTYLISTP (CDR (form))) {
	error ("unless: missing forms", form, NULL);
    }
    test = SECOND (form);
    body = CDR (CDR (form));
    if (eval (test) == false_object) {
	return (eval_body (body, false_object));
    }
    return (false_object);
}

static Object
until_eval (Object form)
{
    Object test, body, forms;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed until statment", form, NULL);
    }
    test = CAR (CDR (form));
    body = CDR (CDR (form));

    while (eval (test) == false_object) {
	forms = body;
	while (!EMPTYLISTP (forms)) {
	    eval (CAR (forms));
	    forms = CDR (forms);
	}
    }
    return (false_object);
}

static Object
unwind_protect_eval (Object form)
{
    Object protected, cleanups, unwind, ret;

    if (EMPTYLISTP (CDR (form))) {
	error ("unwind-protect: missing forms", form, NULL);
    }
    protected = SECOND (form);
    cleanups = CDR (CDR (form));
    unwind = make_unwind (cleanups);


    push_scope (CAR (form));

    add_binding (unwind_symbol, unwind, 1, the_env);

    ret = eval (protected);

    pop_scope ();
    return (ret);
}

static Object
when_eval (Object form)
{
    Object test, body;

    if (EMPTYLISTP (CDR (form))) {
	error ("when: missing forms", form, NULL);
    }
    test = SECOND (form);
    body = CDR (CDR (form));
    if (eval (test) != false_object) {
	return eval_body (body, false_object);
    }
    return (false_object);
}

static Object
while_eval (Object form)
{
    Object test, body, forms;

    if (EMPTYLISTP (CDR (form))) {
	error ("malformed while statment", form, NULL);
    }
    test = CAR (CDR (form));
    body = CDR (CDR (form));

    while (eval (test) != false_object) {
	forms = body;
	while (!EMPTYLISTP (forms)) {
	    eval (CAR (forms));
	    forms = CDR (forms);
	}
    }
    return (false_object);
}

static Object ___passed_test_list;
static Object ___failed_test_list;
static Object ___disabled_test_list;
static Object ___failure_format_string;
static Object ___success_format_string;
static Object ___disabled_format_string;
static Object ___fail_symbol;
static Object ___pass_symbol;
static Object ___disabled_symbol;
static Object ___no_handler_symbol;
static Object ___signal_symbol;

static Object
define_test_eval (Object form)
{
    Object test_name, test_options, doc_string, test_form;
    Object exit_obj, ret;
    int old_no_debug = NoDebug;
    Object cache_env = the_env;

    NoDebug = 1;

    if (___passed_test_list == NULL) {
	___passed_test_list = make_symbol ("*passed-test-list*");
	add_top_level_binding (___passed_test_list, make_empty_list (), 0);

	___failed_test_list = make_symbol ("*failed-test-list*");
	add_top_level_binding (___failed_test_list, make_empty_list (), 0);

	___disabled_test_list = make_symbol ("*disabled-test-list*");
	add_top_level_binding (___disabled_test_list, make_empty_list (), 0);

	___failure_format_string =
	    make_byte_string ("\nFailed:  %= %= with result %=.");
	___success_format_string =
	    make_byte_string ("\nPassed:  %= %= with result %=.");
	___disabled_format_string =
	    make_byte_string ("\nDisabled:  %= %=.");
	___disabled_symbol = make_symbol ("disabled");
	___fail_symbol = make_symbol ("fail");
	___pass_symbol = make_symbol ("pass");

	___disabled_symbol = make_keyword ("disabled:");
	___no_handler_symbol = make_keyword ("no-handler:");
	___signal_symbol = make_keyword ("signal:");
    }
    if (list_length (form) != 5) {
	error ("define-test: bad argument list", form);
    }
    form = CDR (form);
    test_name = CAR (form);
    form = CDR (form);
    test_options = CAR (form);
    form = CDR (form);
    doc_string = CAR (form);
    form = CDR (form);
    test_form = CAR (form);

    if (!SYMBOLP (test_name)) {
	error ("define-test: first argument must be the test name",
	       test_name, NULL);
    }
    if (!LISTP (test_options)) {
	error ("define-test: second argument must be a list of options",
	       test_options, NULL);
    }
    if (!BYTESTRP (doc_string)) {
	error ("define-test: third argument must be documenting string",
	       doc_string, NULL);
    }
    if (doc_string == empty_string) {
	doc_string = find_keyword_val (description_symbol, test_options);
	if (doc_string == NULL)
	    doc_string = empty_string;
    }
    if (member (___disabled_symbol, test_options)) {
	return record_disabled (test_name, doc_string);
    } else if (member (___no_handler_symbol, test_options)) {
	return process_test_result (test_name, map (eval, test_options),
				    doc_string, eval (test_form));
    } else {
	exit_obj = make_exit (signal_symbol);
	ret = (Object) setjmp (*EXITRET (exit_obj));
	push_scope (CAR (form));
	add_binding (signal_symbol, exit_obj, 1, the_env);
	EXITBINDING (exit_obj) = the_env->bindings[0];
	if (!ret) {
	    ret = eval (test_form);
	    pop_scope ();
	} else {
	    pop_scope ();
	}
	the_env = cache_env;
	NoDebug = old_no_debug;
	return process_test_result (test_name, map (eval, test_options),
				    doc_string, ret);
    }
}

static Object
process_test_result (Object name, Object options, Object doc_string,
		     Object result)
{
    Object signal_opt;

    signal_opt = find_keyword_val (___signal_symbol, options);
    if (signal_opt) {
	if (!instance (result, signal_opt)) {
	    warning ("Signalled error class incorrect", signal_opt, NULL);
	}
	if (instance (result, error_class)) {
	    return record_success (name, doc_string, result);
	}
	return record_failure (name, doc_string, result);
    } else if (result == true_object) {
	return record_success (name, doc_string, result);
    } else {
	return record_failure (name, doc_string, result);
    }
}


static Object
record_failure (Object name, Object doc_string, Object result)
{
    struct frame *old_env = the_env;

    the_env = module_binding (dylan_user_symbol)->namespace;

    format (true_object, ___failure_format_string,
	    listem (name, doc_string, result, NULL));
    modify_value (___failed_test_list,
		  cons (name, symbol_value (___failed_test_list)));
    the_env = old_env;
    return ___fail_symbol;
}

static Object
record_success (Object name, Object doc_string, Object test_result)
{
    struct frame *old_env = the_env;

    the_env = module_binding (dylan_user_symbol)->namespace;

    format (true_object, ___success_format_string,
	    listem (name, doc_string, test_result, NULL));
    modify_value (___passed_test_list,
		  cons (name, symbol_value (___passed_test_list)));
    the_env = old_env;
    return ___pass_symbol;
}

static Object
record_disabled (Object name, Object doc_string)
{
    struct frame *old_env = the_env;

    the_env = module_binding (dylan_user_symbol)->namespace;

    format (true_object, ___disabled_format_string,
	    listem (name, doc_string, NULL));
    modify_value (___disabled_test_list,
		  cons (name, symbol_value (___disabled_test_list)));
    the_env = old_env;
    return ___disabled_symbol;
}

static Object
car (Object lst)
{
    return CAR (lst);
}
