/* syntax.c -- see COPYRIGHT for use */

#include <marlais/syntax.h>

#include <marlais/alloc.h>
#include <marlais/apply.h>
#include <marlais/boolean.h>
#include <marlais/bytestring.h>
#include <marlais/class.h>
#include <marlais/env.h>
#include <marlais/eval.h>
#include <marlais/function.h>
#include <marlais/list.h>
#include <marlais/misc.h>
#include <marlais/number.h>
#include <marlais/print.h>
#include <marlais/stream.h>
#include <marlais/symbol.h>
#include <marlais/table.h>
#include <marlais/values.h>

extern Object dylan_symbol;
extern Object dylan_user_symbol;
extern Object modifiers_keyword;
extern Object abstract_symbol;
extern Object concrete_symbol;
extern Object primary_symbol;
extern Object free_symbol;
extern Object open_symbol;
extern Object sealed_symbol;
extern Object description_keyword;
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

static void install_syntax_entry (char *name, syntax_fun fun);
static void bind_variables (Object init_list,
                            int top_level,
                            int constant,
                            struct frame *to_frame);
static void add_variable_binding (Object var,
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
static Object define_function_eval (Object form);
static Object define_module_eval (Object form);
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

static Object while_eval (Object form);
static Object local_bind_eval (Object form);
static Object local_bind_rec_eval (Object form);
static Object unbinding_begin_eval (Object form);

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
    "define-function",
    "define-module",
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
    define_function_eval,
    define_module_eval,
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
    while_eval,
    local_bind_eval,
    local_bind_rec_eval,
    unbinding_begin_eval,
};

void
marlais_initialize_syntax (void)
{
    int numops, i;

    numops = sizeof (syntax_operators) / sizeof (char *);

    for (i = 0; i < numops; ++i) {
      install_syntax_entry (syntax_operators[i],
                            syntax_functions[i]);
    }
}

syntax_fun
marlais_syntax_function (Object sym)
{
    struct syntax_entry *entry;
    int h;

    h = ((DyInteger) sym) % SYNTAX_TABLE_SIZE;
    entry = syntax_table[h];
    while (entry) {
      if (entry->sym == sym) {
        return (entry->fun);
      }
      entry = entry->next;
    }
    return (NULL);
}

/* <pcb> a single function to evaluate bodies. uses tail_eval. */

static void
install_syntax_entry (char *name, syntax_fun fun)
{
    struct syntax_entry *entry;
    Object sym;
    int h;

    sym = marlais_make_name (name);
    h = ((DyInteger) sym) % SYNTAX_TABLE_SIZE;
    entry = (struct syntax_entry *)
      marlais_allocate_memory (sizeof (struct syntax_entry));

    entry->sym = sym;
    entry->fun = fun;
    entry->next = syntax_table[h];
    syntax_table[h] = entry;
}

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
      if (ret == MARLAIS_FALSE) {
        return (MARLAIS_FALSE);
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
      marlais_error ("malformed bind form", form, NULL);
    }
    bindings = SECOND (form);
    body = CDR (CDR (form));

    /* <pcb> rather than pop the environements, just restore at the end. */
    initial_env = the_env;

    while (!EMPTYLISTP (bindings)) {
      /* <pcb> some hackery to make bind work correctly. */
      enclosing_env = the_env;
      marlais_push_scope (CAR (form));
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
      marlais_error ("malformed local binding", form, NULL);
    }
    bindings = SECOND (form);

    /* <pcb> some hackery to make local bind work correctly. */
    enclosing_env = the_env;
    marlais_push_scope (CAR (form));
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
      marlais_error ("malformed local binding", form, NULL);
    }
    bindings = SECOND (form);

    marlais_push_scope (CAR (form));

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
      marlais_error ("Bad unbinding-begin form", form, NULL);
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
      marlais_pop_scope ();
    }

    return res;
}

static Object
bind_exit_eval (Object form)
{
    Object exit_obj, sym, body, ret, sec;

    if (EMPTYLISTP (CDR (form))) {
      marlais_error ("malformed bind-exit form", form, NULL);
    }
    sec = SECOND (form);
    if (!PAIRP (sec)) {
      marlais_error ("bind-exit: second argument must be a list containing a symbol", sec, NULL);
    }
    sym = CAR (sec);
    body = CDR (CDR (form));
    if (!NAMEP (sym)) {
      marlais_error ("bind-exit: bad exit procedure name", sym, NULL);
    }
    exit_obj = make_exit (sym);

    marlais_push_scope (CAR (form));
    marlais_add_binding (sym, exit_obj, 1, the_env);
    EXITBINDING (exit_obj) = the_env->bindings[0];
    ret = (Object) setjmp (*EXITRET (exit_obj));


    if (!ret) {
#if 1
      ret = MARLAIS_FALSE;
      while (!EMPTYLISTP (body)) {
        ret = eval (CAR (body));
        body = CDR (body);
      }
#else
      eval_body (body, MARLAIS_FALSE);
#endif
      marlais_pop_scope ();
      return (ret);
    } else {
      marlais_pop_scope ();
      return (ret);
    }
}

static Object
bind_methods_eval (Object form)
{
  Object specs, body, spec, ret;
  Object name, params, method_body, method;

  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("bind-methods: bad form", form, NULL);
  }
  specs = SECOND (form);
  body = CDR (CDR (form));

  marlais_push_scope (CAR (form));
  /* first bind method names to dummy values */
  if (!PAIRP (specs)) {
    marlais_error ("bind-methods: First argument must be a list of method bindings",
                   specs,
                   NULL);
  }
  while (!EMPTYLISTP (specs)) {
    spec = CAR (specs);
    name = FIRST (spec);
    marlais_add_binding (name, MARLAIS_FALSE, 0, the_env);
    specs = CDR (specs);
  }

  /* now, actually make the methods */
  specs = SECOND (form);
  while (!EMPTYLISTP (specs)) {
    spec = CAR (specs);
    name = FIRST (spec);
    if (EMPTYLISTP (CDR (spec))) {
      marlais_error ("bind-methods: incomplete method specification", spec, NULL);
    }
    params = SECOND (spec);
    method_body = CDR (CDR (spec));
    method = make_method (name, params, method_body, the_env, 0);
    modify_value (name, method);
    specs = CDR (specs);
  }

  ret = eval_body (body, unspecified_object);
  marlais_pop_scope ();
  return (ret);
}

static Object
boundp_eval (Object form)
{
  Object cdr = CDR (form);
  Object sym;

  if (EMPTYLISTP (cdr)) {
    marlais_error ("bound?: missing symbol", form, NULL);
  }
  sym = CAR (cdr);
  if (!NAMEP (sym)) {
    marlais_error ("bound?: argument must be a symbol", sym, NULL);
  }
  return (symbol_value (sym) == NULL ? MARLAIS_FALSE : MARLAIS_TRUE);
}

static Object
case_eval (Object form)
{
  Object target_form, branches, branch;
  Object match_list, consequents, ret;

  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("malformed case", form, NULL);
  }
  target_form = eval (CAR (CDR (form)));

  if (EMPTYLISTP (CDR (CDR (form)))) {
    marlais_error ("malformed case", form, NULL);
  }
  branches = CDR (CDR (form));
  while (!EMPTYLISTP (branches)) {
    branch = CAR (branches);
    if (!PAIRP (branch)) {
      marlais_error ("case: malformed branch", branch, NULL);
    }
    match_list = CAR (branch);
    if ((match_list == MARLAIS_TRUE) || (match_list == else_keyword)) {
      consequents = CDR (branch);
      ret = MARLAIS_FALSE;
      while (!EMPTYLISTP (consequents)) {
        ret = eval (CAR (consequents));
        consequents = CDR (consequents);
      }
      return (ret);
    }
    if (!PAIRP (match_list)) {
      marlais_error ("select: malformed test expression", match_list, NULL);
    }
    while (!EMPTYLISTP (match_list)) {
      if (marlais_identical_p (CAR (match_list), target_form)) {
        consequents = CDR (branch);
        ret = MARLAIS_FALSE;
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
  return marlais_error ("case: no matching clause", target_form, NULL);
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
    if (ret != MARLAIS_FALSE) {
      clause = CDR (clause);
      return eval_body (clause, ret);
    }
    clauses = CDR (clauses);
  }
  return (MARLAIS_FALSE);
}

static void define_eval_helper(Object form, int top_level, int constant)
{
  if (EMPTYLISTP (CDR (form)) || EMPTYLISTP (CDR (CDR (form)))) {
    marlais_error ("DEFINE form requires at least two args: (define {<var>} <init>)",
                   form, NULL);
  } else {
    bind_variables (CDR (form), top_level, constant, the_env);
  }
}

static Object
define_eval (Object form)
{
  define_eval_helper(form, 1, 0);
  return unspecified_object;
}

static Object
define_constant_eval (Object form)
{
  define_eval_helper(form, 1, 1);
  return unspecified_object;
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
    marlais_error ("Initializer list requires at least two elements", init_list, NULL);
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
          marlais_module_export (variable, first, constant);
        } else {
          marlais_add_binding (variable, first, constant, to_frame);
        }
        /* check for no variables after #rest */
        if (CDR (CDR (variables)) != init) {
          marlais_error ("Badly placed #rest specifier", init_list, NULL);
        }
        /* finished with bindings */
        break;
      } else {
        /* check for not enough inits */
        if (value_count < VALUESNUM (val)) {
          new = VALUESELS (val)[value_count];
        } else {
          new = MARLAIS_FALSE;
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
                              MARLAIS_FALSE,
                              top_level,
                              constant,
                              to_frame);
      }
    }
  }
}

static void
add_variable_binding (Object var,
                      Object val,
                      int top_level,
                      int constant,
                      struct frame *to_frame)
{
  Object type;

  if (PAIRP (var)) {
    if (!PAIRP (CDR (var))) {
      marlais_error ("badly formed variable", var, NULL);
    }
    type = eval (SECOND (var));
    if (!marlais_instance (type, type_class)) {
      marlais_error ("badly formed variable", var, NULL);
    }
  } else {
    type = object_class;
  }
  if (top_level) {
    /* marlais_module_export can't easily check type match.
     * do it here.
     */
    if (!marlais_instance (val, type)) {
      marlais_error ("initial value does not satisfy type constraint",
                     val,
                     type,
                     NULL);
    }
    marlais_module_export (var, val, constant);
  } else {
    marlais_add_binding (var, val, constant, to_frame);
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
    marlais_error ("malfored define-class (no arguments)", form, NULL);
  }
  tmp_form = CDR (tmp_form);
  if (PAIRP (CAR (tmp_form))) {
    modifiers = CAR (tmp_form);
    if (CAR (modifiers) != modifiers_keyword) {
      marlais_error ("malformed define-class (bad modifiers)", form, NULL);
    }
    for (modifiers = CDR (modifiers);
         PAIRP (modifiers);
         modifiers = CDR (modifiers)) {
      modifier = CAR (modifiers);
      if (modifier == abstract_symbol || modifier == concrete_symbol) {
        if (abstract_concrete_seen) {
          marlais_error ("redundant or conflicting modifier given to define-class",
                         modifier, NULL);
        }
        abstract_concrete_seen = 1;
        abstract_class = (modifier == abstract_symbol);
      }
      if (modifier == primary_symbol || modifier == free_symbol) {
        if (primary_free_seen) {
          marlais_error ("redundant or conflicting modifier given to define-class",
                         modifier, NULL);
        }
        primary_free_seen = 1;
        primary_class = (modifier == primary_symbol);
      }
      if (modifier == open_symbol || modifier == sealed_symbol) {
        if (open_sealed_seen) {
          marlais_error ("redundant or conflicting modifier given to define-class",
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
    marlais_error ("malformed define-class (no superclass)", form, NULL);
  }
  /*
   * Must introduce binding for the class before eval'ing
   * the slot definitions.
   */
  obj = marlais_allocate_object (Class, sizeof (struct clas));

  CLASSNAME (obj) = name;
  marlais_module_export (name, obj, 0);
  supers = map (eval, CAR (tmp_form));
  if(EMPTYLISTP(supers)) supers = cons(object_class, make_empty_list());
  slots = marlais_make_slot_descriptor_list (CDR (tmp_form), 1);
  marlais_make_getter_setter_gfs (slots);
  class = marlais_make_class (obj, supers, slots,
                              (abstract_class ? MARLAIS_TRUE : MARLAIS_FALSE), NULL);

  /* kludge to put these here.  Better to add a param to make_class. */
  CLASSPROPS (class) |= CLASSSLOTSUNINIT;

  /*
    if (abstract_class) {
    marlais_make_class_uninstantiable (class);
    }
  */
  if (!open_class) {
    /*
     * Need to address sealed vs. open classes with library additions.
     */
    /*
      marlais_make_class_sealed (class);
    */
  }
  if (primary_class) {
    marlais_make_class_primary (class);
  }
  return (name);
}

static void
check_function_syntax (Object form, Object* name, Object* params, char* def)
{
  char err_msg[80];
  strcpy(err_msg, def);
  strcat(err_msg, ": ");

  if (EMPTYLISTP (CDR (form))) {
    strcat(err_msg, "missing name");
    marlais_error (err_msg, form, NULL);
  }
  *name = SECOND (form);
  if (EMPTYLISTP (CDR (CDR (form)))) {
    strcat(err_msg, "missing parameters");
    marlais_error (err_msg, form, NULL);
  }
  *params = THIRD (form);
  if (!LISTP (*params)) {
    strcat(err_msg, "second argument must be a parameter list");
    marlais_error (err_msg, params, NULL);
  }
}

static Object
define_generic_function_eval (Object form)
{
  Object name, params, gf;

  check_function_syntax(form, &name, &params, "define-generic-function");
  gf = make_generic_function (name, params, make_empty_list ());
  marlais_module_export (name, gf, 0);
  return (unspecified_object);
}

static Object
define_method_eval_helper (Object form, int do_generic_p)
{
  Object name, params, body, method;

  check_function_syntax(form, &name, &params, "define-method");
  body = CDR (CDR (CDR (form)));
  method = make_method (name, params, body, the_env, do_generic_p);
  return (name);
}

static Object
define_method_eval (Object form)
{
  return define_method_eval_helper(form, 1);
}

static Object
define_function_eval (Object form)
{
  return define_method_eval_helper(form, 0);
}

static Object
define_module_eval (Object form)
{
  Object clauses, clause;
  Object module_name;
  Object option;
  struct module_binding *the_module;

  /* Bogus for now */
  if (PAIRP (form) && list_length (form) >= 2 && NAMEP (SECOND (form))) {
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
                  marlais_error ("use clause: unknown option", option, NULL);
                }

              } else {
                marlais_error ("use clause: poorly formed option", CAR (clause), NULL);
              }
              clause = CDR (clause);
            }
            if (imports_specified && exclusions_specified) {
              marlais_error ("Define module: Can't specify both imports: "
                             "and exclusions:", clause, NULL);
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
            marlais_error ("define-module: Bad use clause", clause, NULL);
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
          marlais_error ("define-module: Bad clause", clause, NULL);
        }
      } else {
        marlais_error ("define-module: Bad clause", clause, NULL);
      }
      clauses = CDR (clauses);
    }
  } else {
    marlais_error ("define-module: Bad argument list", form, NULL);
  }
  return unspecified_object;
}

static Object
dotimes_eval (Object form)
{
  Object clause, var, intval, resform, body, res;
  int i;

  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("malformed dotimes expression", form, NULL);
  }
  clause = CAR (CDR (form));
  if (!PAIRP (clause)) {
    marlais_error ("second arg to dotimes must be a list", clause, NULL);
  }
  var = CAR (clause);
  if (!NAMEP (var)) {
    marlais_error ("dotimes: first value in spec clause must be a symbol", var, NULL);
  }
  if (EMPTYLISTP (CDR (clause))) {
    marlais_error ("dotimes: must specifiy an upper bound", form, NULL);
  }
  intval = eval (CAR (CDR (clause)));
  if (!INTEGERP (intval)) {
    marlais_error ("dotimes: upper bound must an integer", intval, NULL);
  }
  if (!EMPTYLISTP (CDR (CDR (clause)))) {
    resform = CAR (CDR (CDR (clause)));
  } else {
    resform = NULL;
  }

  marlais_push_scope (CAR (form));
  marlais_add_binding (var, MARLAIS_FALSE, 0, the_env);
  for (i = 0; i < INTVAL (intval); ++i) {
    change_binding (var, marlais_make_integer (i));
    body = CDR (CDR (form));
    while (!EMPTYLISTP (body)) {
      res = eval (CAR (body));
      body = CDR (body);
    }
  }
  if (resform) {
    res = eval (resform);
  } else {
    res = MARLAIS_FALSE;
  }
  marlais_pop_scope ();
  return (res);
}

static Object
for_eval (Object form)
{
  /* The classic syntax of a for is
   *
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
    marlais_error ("malformed FOR", form, NULL);
  }
  test_form = FIRST (THIRD (form));
  return_forms = CDR (THIRD (form));

  var_forms = SECOND (form);

  /* IRM Pg. 33 Step 1 */

  clause_types = vars = inits = make_empty_list ();

  get_vars_and_inits (var_forms, &clause_types, &vars, &inits);

  /* IRM Step 2 */
  marlais_push_scope (CAR (form));
  initialize_step_and_numeric_vars (clause_types, vars, inits);

  /* IRM Step 3 */
  initialize_collection_inits (clause_types, vars, inits);
  if (!exhausted_numeric_or_collection_clauses (clause_types,
                                                vars,
                                                inits,
                                                1)) {

    /* IRM Step 4 */

    marlais_push_scope (CAR (form));
    initialize_collection_variables (clause_types, vars, inits);

    do {
      /* IRM Step 5 */
      if (eval (test_form) != MARLAIS_FALSE) {
        break;
      }
      /* IRM Step 6 */
      body = CDR (CDR (CDR (form)));
      while (!EMPTYLISTP (body)) {
        eval (CAR (body));
        body = CDR (body);
      }

      /* IRM Steps 7 and 8 */
      update_explicit_and_numeric_clauses (clause_types, vars, inits);

      /* IRM Step 3 (again) */
      if (exhausted_numeric_or_collection_clauses (clause_types,
                                                   vars,
                                                   inits,
                                                   0)) {
        break;
      }
      update_collection_variables (clause_types, vars, inits);
    } while (1);
    marlais_pop_scope (); /* To get rid of collection variables */
  }
  if (!PAIRP (return_forms)) {
    ret = MARLAIS_FALSE;
  } else {
    while (PAIRP (return_forms)) {
      ret = eval (CAR (return_forms));
      return_forms = CDR (return_forms);
    }
  }
  marlais_pop_scope ();
  return ret;
}

static Object
get_variable (Object var_spec)
{
  if ((PAIRP (var_spec) && (list_length (var_spec) != 2)) &&
      (!NAMEP (var_spec))) {
    marlais_error ("Bad variable specification", var_spec, NULL);
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
    if (PAIRP (var_spec) || NAMEP (var_spec)) {

      /* Explicit Step Clause: init is of form
       *    (init-value . next-value)
       */

      clause_type = variable_keyword;
      var = get_variable (var_spec);
      if (list_length (var_form) != 3) {
        marlais_error ("for: Bad variable initialization", var_form, NULL);
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
        marlais_error ("for: Bad numeric clause specification", var_form, NULL);
      }
      var = get_variable (SECOND (var_form));
      rest = CDR (CDR (var_form));

      by = marlais_make_integer (1);
      termination = MARLAIS_FALSE;
      start = eval (CAR (rest));
      rest = CDR (rest);
      bound = MARLAIS_FALSE;
      if (PAIRP (rest)) {
        termination = CAR (rest);
        if (PAIRP (CDR (rest)) &&
            (termination == to_symbol || termination == above_symbol ||
             termination == below_symbol)) {
          bound = CAR (CDR (rest));
          rest = CDR (CDR (rest));
        } else {
          marlais_error ("for: badly formed numeric clause", var_form, NULL);
        }
      }
      if (PAIRP (rest)) {
        if (PAIRP (CDR (rest)) && CAR (rest) == by_symbol) {
          by = eval (CAR (CDR (rest)));
        } else {
          marlais_error ("for: badly formed numeric clause", var_form, NULL);
        }
      }
      switch (object_type (by)) {
      case Integer:
        negative = (INTVAL (by) >= 0) ? MARLAIS_FALSE : MARLAIS_TRUE;
        break;
      case DoubleFloat:
        negative = (DFLOATVAL (by) >= 0) ? MARLAIS_FALSE : MARLAIS_TRUE;
        break;
      default:
        marlais_error ("for: numeric clause has unsupported increment type", by, NULL);
      }

      init = listem (start, by, negative, termination, bound, NULL);

    } else if (var_spec == collection_keyword) {

      /* Collection Clause: init value is
       *  (protocol collection state)
       */

      clause_type = collection_keyword;
      if (list_length (var_form) != 3) {
        marlais_error ("for: Bad collection clause specification", var_form, NULL);
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
      marlais_add_binding (CAR (vars), CAR (CAR (inits)), 0, the_env);
    } else if (CAR (clause_types) == range_keyword) {
      marlais_add_binding (CAR (vars), CAR (CAR (inits)), 0, the_env);
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
        THIRD (CAR (inits)) = marlais_apply (VALUESELS (protocol)[2],
                                             cons (SECOND (CAR (inits)),
                                                   cons (THIRD (CAR (inits)),
                                                         make_empty_list ())));
      }
      if (MARLAIS_TRUE == marlais_apply (VALUESELS (protocol)[3],
                                         cons (SECOND (CAR (inits)),
                                               cons (THIRD (CAR (inits)),
                                                     cons (VALUESELS (protocol)[1],
                                                           make_empty_list ()))))) {
        return 1;
      }
    } else if (clause_type == range_keyword) {

      init = CAR (inits);
      current = CAR (init); /* FIRST */
      init = CDR (init);
      increment = CAR (init); /* SECOND */
      init = CDR (init);
      negative = CAR (init); /* THIRD */
      init = CDR (init);
      termination = CAR (init); /* FOURTH */
      init = CDR (init);
      bound = CAR (init); /* FIFTH */

      if (termination == MARLAIS_FALSE) {
        /* do nothing */
      } else if (termination == to_symbol) {
        if (negative == MARLAIS_TRUE) {
          if (MARLAIS_TRUE == eval (listem (lesser_symbol,
                                            current,
                                            bound,
                                            NULL))) {
            return 1;
          }
        } else if (MARLAIS_TRUE == eval (listem (greater_symbol,
                                                 current,
                                                 bound,
                                                 NULL))) {
          return 1;
        }
      } else if (termination == above_symbol) {
        if (MARLAIS_TRUE == eval (listem (lesser_equal_symbol,
                                          current,
                                          bound,
                                          NULL))) {
          return 1;
        }
      } else if (termination == below_symbol) {
        if (MARLAIS_TRUE == eval (listem (greater_equal_symbol,
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
      marlais_add_binding (CAR (vars),
                   marlais_apply (VALUESELS (protocol)[5],
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
                    marlais_apply (VALUESELS (protocol)[5],
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
    marlais_error ("for-each: no initial-state function defined", NULL);
  }
  next_state_fun = symbol_value (next_state_sym);
  if (!next_state_fun) {
    marlais_error ("for-each: no next-state function defined", NULL);
  }
  cur_el_fun = symbol_value (current_element_sym);
  if (!cur_el_fun) {
    marlais_error ("for-each: no current-element function defined", NULL);
  }
  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("malformed FOR-EACH", form, NULL);
  }
  if (EMPTYLISTP (CDR (CDR (form)))) {
    marlais_error ("malformed FOR-EACH", form, NULL);
  }
  test_form = FIRST (THIRD (form));
  return_forms = CDR (THIRD (form));

  var_forms = SECOND (form);
  vars = map (car, var_forms);
  collections = map (second, var_forms);
  collections = map (eval, collections);
  states = list_map1 (init_state_fun, collections);

  if (member (MARLAIS_FALSE, states)) {
    return (MARLAIS_FALSE);
  }
  vals = list_map2 (cur_el_fun, collections, states);
  marlais_push_scope (CAR (form));
  marlais_add_bindings (vars, vals, 0, the_env);

  while (eval (test_form) == MARLAIS_FALSE) {
    body = CDR (CDR (CDR (form)));
    while (!EMPTYLISTP (body)) {
      eval (CAR (body));
      body = CDR (body);
    }
    states = list_map2 (next_state_fun, collections, states);
    if (member (MARLAIS_FALSE, states)) {
      marlais_pop_scope ();
      return (MARLAIS_FALSE);
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
    return (MARLAIS_FALSE);
  } else {
    ret = eval_body (return_forms, MARLAIS_FALSE);
  }
  marlais_pop_scope ();
  return (ret);
}

static Object
if_eval (Object form)
{
  Object testval, thenform, elseform;

  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("malformed if expression", form, NULL);
  }
  testval = SECOND (form);
  if (EMPTYLISTP (CDR (CDR (form)))) {
    marlais_error ("malformed if expression", form, NULL);
  }
  thenform = THIRD (form);
  if (EMPTYLISTP (CDR (CDR (CDR (form))))) {
    marlais_error ("if expression must have else clause", form, NULL);
  }
  elseform = FOURTH (form);
  if (!EMPTYLISTP (CDR (CDR (CDR (CDR (form)))))) {
    marlais_error ("if: too many arguments", NULL);
  }
  testval = eval (testval);

  if (testval == MARLAIS_FALSE) {
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
    marlais_error ("method: missing parameters", form, NULL);
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
    if (ret != MARLAIS_FALSE) {
      return (ret);
    }
    clauses = CDR (clauses);
  }
  return (MARLAIS_FALSE);
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

  if (EMPTYLISTP (skel) || NAMEP (skel) || !PAIRP (skel)) {
    return skel;
  } else {
    head = skel;
    tail = CDR (skel);
    if (CAR (head) == unquote_symbol) {
      if (!EMPTYLISTP (tail)) {
        if (!EMPTYLISTP (CDR (tail))) {
          marlais_error ("Too many arguments to unquote", NULL);
        }
        return eval (CAR (tail));
      } else {
        return marlais_error ("missing argument to unquote", NULL);
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
        return marlais_error ("missing argument to unquote_splicing", NULL);
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
    marlais_error ("malformed select", form, NULL);
  }
  target_form = eval (CAR (CDR (form)));

  if (EMPTYLISTP (CDR (CDR (form)))) {
    marlais_error ("malformed select", form, NULL);
  }
  test = eval (CAR (CDR (CDR (form))));

  if (EMPTYLISTP (CDR (CDR (CDR (form))))) {
    marlais_error ("malformed select", form, NULL);
  }
  branches = CDR (CDR (CDR (form)));
  while (!EMPTYLISTP (branches)) {
    branch = CAR (branches);
    if (!PAIRP (branch)) {
      marlais_error ("select: malformed branch", branch, NULL);
    }
    match_list = CAR (branch);
    if ((match_list == MARLAIS_TRUE) || (match_list == else_keyword)) {
      consequents = CDR (branch);
      while (!EMPTYLISTP (consequents)) {
        ret = eval (CAR (consequents));
        consequents = CDR (consequents);
      }
      return (ret);
    }
    if (!PAIRP (match_list)) {
      marlais_error ("select: malformed test expression", match_list, NULL);
    }
    while (!EMPTYLISTP (match_list)) {
      ret = MARLAIS_FALSE;
      if (marlais_apply (test, listem (target_form, eval (CAR (match_list)),
                                       NULL)) != MARLAIS_FALSE) {
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
  return (MARLAIS_FALSE);
}

static Object
set_eval (Object form)
{
  Object sym, val;

  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("set!: missing forms", form, NULL);
  }
  sym = SECOND (form);

  if (PAIRP (sym)) {
    /*
     * <pcb> let's keep things in the spirit of the old language.
     * (set! (slot obj ...) new-value) should become
     * (slot-setter new-value obj ...)
     */
    return eval (cons (marlais_make_setter_symbol (CAR (sym)),
                       marlais_devalue (cons (THIRD (form), CDR (sym)))));

  }
  if (EMPTYLISTP (CDR (CDR (form)))) {
    marlais_error ("set!: missing forms", form, NULL);
  }
  val = marlais_devalue (eval (THIRD (form)));
  modify_value (sym, val);
  return (val);
}

static Object
set_module_eval (Object form)
{
  if (PAIRP (form) && list_length (form) == 2 && SYMBOLP (SECOND (form))) {
    return user_set_module (marlais_devalue (CDR (form)));
  } else {
    marlais_error ("set_module: argument list not a single symbol", form, NULL);
  }
  return unspecified_object;
}

static Object
unless_eval (Object form)
{
  Object test, body;

  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("unless: missing forms", form, NULL);
  }
  test = SECOND (form);
  body = CDR (CDR (form));
  if (eval (test) == MARLAIS_FALSE) {
    return (eval_body (body, MARLAIS_FALSE));
  }
  return (MARLAIS_FALSE);
}

static Object
until_eval (Object form)
{
  Object test, body, forms;

  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("malformed until statment", form, NULL);
  }
  test = CAR (CDR (form));
  body = CDR (CDR (form));

  while (eval (test) == MARLAIS_FALSE) {
    forms = body;
    while (!EMPTYLISTP (forms)) {
      eval (CAR (forms));
      forms = CDR (forms);
    }
  }
  return (MARLAIS_FALSE);
}

static Object
unwind_protect_eval (Object form)
{
  Object protected, cleanups, unwind, ret;

  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("unwind-protect: missing forms", form, NULL);
  }
  protected = SECOND (form);
  cleanups = CDR (CDR (form));
  unwind = make_unwind (cleanups);

  marlais_push_scope (CAR (form));

  marlais_add_binding (unwind_symbol, unwind, 1, the_env);

  ret = eval (protected);

  marlais_pop_scope ();
  return (ret);
}

static Object
while_eval (Object form)
{
  Object test, body, forms;

  if (EMPTYLISTP (CDR (form))) {
    marlais_error ("malformed while statment", form, NULL);
  }
  test = CAR (CDR (form));
  body = CDR (CDR (form));

  while (eval (test) != MARLAIS_FALSE) {
    forms = body;
    while (!EMPTYLISTP (forms)) {
      eval (CAR (forms));
      forms = CDR (forms);
    }
  }
  return (MARLAIS_FALSE);
}

static Object
car (Object lst)
{
  return CAR (lst);
}
