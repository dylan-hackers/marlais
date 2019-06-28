/* env.c -- see COPYRIGHT for use */

#include <string.h>

#include "env.h"

#include "alloc.h"
#include "bytestring.h"
#include "class.h"
#include "error.h"
#include "eval.h"
#include "function.h"
#include "keyword.h"
#include "list.h"
#include "number.h"
#include "prim.h"
#include "print.h"
#include "stream.h"
#include "table.h"

extern Object dylan_symbol;
extern Object dylan_user_symbol;
extern Object empty_string;
extern Object unwind_protect_symbol;
extern jmp_buf *the_eval_context;

int trace_bindings = 0;

/* the environment */
struct frame *the_env;

struct module_binding *the_current_module;

struct modules {
    int size;
    struct module_binding **bindings;
};

struct modules modules =
{0, NULL};

/* the top level environment */
#define BIND_ALLOC_CHUNK 4

/* If TOP_LEVEL_SIZE is not a power of two, see change required below */
#define TOP_LEVEL_SIZE 1024
/* struct binding *top_level_env[TOP_LEVEL_SIZE]; */

/* local function prototypes */
struct binding *symbol_binding (Object sym);
static Object concat_prefix (char *prefix_string, Object sym);
static void fill_imports_table_from_property_set (Object imports_table,
						  Object imports_set,
						  Object renames_table);

/* function definitions */

static struct frame *
initialize_namespace (Object owner)
{
  struct frame *frame;

  frame = (struct frame *) allocate_frame ();
  frame->size = TOP_LEVEL_SIZE;
  frame->owner = owner;
  frame->bindings =
    (struct binding **) checking_malloc (TOP_LEVEL_SIZE * sizeof (struct binding));

  frame->next = NULL;
  frame->top_level_env = frame->bindings;
  return frame;
}

void add_top_level_binding(Object sym, Object val, int constant)
{
  add_top_lvl_binding1(sym, val, constant, 1);
}

void
add_top_lvl_binding1(Object sym, Object val, int constant, int exported)
{
  struct binding *binding, *old_binding;
  int i;
  unsigned h;
  char *str;

  binding = (struct binding *) allocate_binding ();
  if (PAIRP (sym)) {
    binding->sym = CAR (sym);
    binding->type = eval (SECOND (sym));
  } else {
    binding->sym = sym;
    binding->type = object_class;
  }

  binding->props &= !IMPORTED_BINDING;

  /* Just for now, hide all bindings starting with '%' */
  if(exported)
    binding->props |= EXPORTED_BINDING;

  if (constant) {
    binding->props |= CONSTANT_BINDING;
  }
  old_binding = symbol_binding_top_level (binding->sym);
  if (old_binding != NULL) {
    warning ("Symbol already defined. Previous value", sym,
	     *(old_binding->val), NULL);
  }
  binding->val = (Object *) marlais_allocate_memory (sizeof (Object *));

  *(binding->val) = val;

  i = h = 0;
  str = SYMBOLNAME (binding->sym);
  while (str[i]) {
    h += str[i++];
  }
/*
   h = h % TOP_LEVEL_SIZE;
 */

  /* Works only if TOP_LEVEL_SIZE is a power of 2 */
  h &= (TOP_LEVEL_SIZE - 1);

  binding->next = the_env->top_level_env[h];
  the_env->top_level_env[h] = binding;

  if (trace_bindings) {
    print_obj (make_integer(STDERR), sym);
  }
}

void
push_scope (Object owner)
{
  struct frame *frame;

  /* push a new frame */
  frame = (struct frame *) allocate_frame ();
  frame->owner = owner;
  frame->size = 0;
  frame->bindings = NULL;
  frame->next = the_env;
  frame->top_level_env = the_env->top_level_env;
  the_env = frame;
  eval_stack->frame = frame;
}

void
pop_scope (void)
{
  the_env = the_env->next;
}

void
add_bindings (Object syms, Object vals, int constant, struct frame *to_frame)
{
  struct frame *frame;
  struct binding *binding;
  int num_bindings, i;
  Object sym_list;

  sym_list = syms;
  num_bindings = 0;
  while (!EMPTYLISTP (sym_list)) {
    num_bindings++;
    sym_list = CDR (sym_list);
  }

  frame = to_frame;

  frame->bindings = (struct binding **)
    checking_realloc (frame->bindings,
		      (frame->size + num_bindings) * sizeof(struct binding *));

  for (i = 0; i < num_bindings; ++i) {
    if ((!syms) || (!vals)) {
      error ("mismatched number of symbols and values", NULL);
    }
    binding = (struct binding *) allocate_binding ();
    binding->sym = CAR (syms);
    /* ??? */
    binding->type = object_class;
    binding->val = (Object *) marlais_allocate_memory (sizeof (Object *));

    *(binding->val) = CAR (vals);

    binding->props &= !IMPORTED_BINDING;
    /* Just for now */
    binding->props |= EXPORTED_BINDING;
    if (constant) {
      binding->props |= CONSTANT_BINDING;
    }
    frame->bindings[i + frame->size] = binding;

    syms = CDR (syms);
    vals = CDR (vals);
  }
  frame->size += num_bindings;
}

void
add_binding (Object sym, Object val, int constant, struct frame *to_frame)
{
    struct frame *frame;
    struct binding *binding;

    binding = (struct binding *) allocate_binding ();
    if (PAIRP (sym)) {
	binding->sym = CAR (sym);
	binding->type = eval (SECOND (sym));
    } else {
	binding->sym = sym;
	binding->type = object_class;
    }
    binding->val = (Object *) marlais_allocate_memory (sizeof (Object *));

    if (!instance (val, binding->type)) {
	error ("add_binding: value does not satisfy type constraint",
	       val,
	       binding->type,
	       NULL);
    }
    *(binding->val) = val;
    binding->props &= !IMPORTED_BINDING;
    /* Just for now */
    binding->props |= EXPORTED_BINDING;
    if (constant) {
	binding->props |= CONSTANT_BINDING;
    }
    frame = to_frame;

    if ((frame->size % BIND_ALLOC_CHUNK) == 0) {
	frame->bindings = (struct binding **)
	    checking_realloc (frame->bindings,
	      (frame->size + BIND_ALLOC_CHUNK) * sizeof (struct binding *));
    }
    frame->bindings[frame->size] = binding;
    frame->size++;
}

/* Change the binding of the symbol in top-most frame.
   Return 1 on success.  If there is no such binding,
   return 0.

   This isn't correct.  It uses symbol_binding() which
   checks *all* bindings of the symbol, not just the
   top level.
 */
int
change_binding (Object sym, Object val)
{
    struct binding *binding;

    binding = symbol_binding (sym);
    if (!binding) {
	return (0);
    } else {
/*
   if ( ! instance (val, binding->type)) {
 */
	*(binding->val) = val;
	return 1;
/*
   } else {
   error("attempt to assign binding of wrong type", sym, val,NULL);
   return 0;
   }
 */
    }
}

Object
symbol_value (Object sym)
{
    struct binding *binding;

    binding = symbol_binding (sym);
    if (!binding) {
	return (NULL);
    }
    return (*(binding->val));
}

void
modify_value (Object sym, Object new_val)
{
    struct binding *binding;

    binding = symbol_binding (sym);
    if (!binding) {
	error ("attempt to modify value of unbound symbol", sym, NULL);
    } else if (IS_CONSTANT_BINDING (binding)) {
	error ("attempt to modify value of a constant", sym, NULL);
    } else if (instance (new_val, binding->type)) {
	*(binding->val) = new_val;
    } else {
	error ("attempt to assign variable an incompatible object",
	       sym, new_val, NULL);
    }
}

struct frame *
current_env (void)
{
    return (the_env);
}

/* primitives */

Object user_current_module (void);
Object user_set_module (Object args);

#if 0
static Object reset_module (Object args);

#endif

static struct primitive env_prims[] =
{
    {"current-module", prim_0, user_current_module},
    {"set-module", prim_0_rest, user_set_module},
#if 0
    {"reset-module", prim_0_rest, reset_module},
#endif
};

/* functions */

void
init_env_prims (void)
{
    int num = sizeof (env_prims) / sizeof (struct primitive);
    init_prims (num, env_prims);
}

/* local functions */

/* made symbol_binding non local to be able to fix <object> binding */
struct binding *
symbol_binding (Object sym)
{
    struct frame *frame;
    struct binding *binding;
    int i;

    frame = the_env;
    while (frame->bindings != frame->top_level_env) {
	for (i = 0; i < frame->size; ++i) {
	    binding = frame->bindings[i];
	    if (binding->sym == sym) {
		return (binding);
	    }
	}
	frame = frame->next;
	if (!frame)
	    break;		/* <pcb> I/'ve observed this to be nil in a special case. */
    }
    /* can't find binding in frames, look at top_level */
    return (symbol_binding_top_level (sym));
}

struct binding *
symbol_binding_top_level (Object sym)
{
    struct binding *binding;
    int h, i;
    char *str;

    i = h = 0;
    str = SYMBOLNAME (sym);
    while (str[i]) {
	h += str[i++];
    }
    h = h % TOP_LEVEL_SIZE;

    binding = the_env->top_level_env[h];
    while (binding) {
	if (binding->sym == sym) {
	    return (binding);
	}
	binding = binding->next;
    }
    return (NULL);
}

/* Unwind the eval stack until we reach a context having a frame
 * with exit_sym as its only binding.  Perform unwind-protect
 * cleanups when we find them.
 */
int
unwind_to_exit (Object exit_proc)
{
    struct frame *frame;
    Object body;
    struct eval_stack *tmp_eval_stack, *save_eval_stack;

    /* pop the current frame off the stack.  It can't be right. */
    save_eval_stack = eval_stack;
    tmp_eval_stack = eval_stack->next;

    while (tmp_eval_stack) {
	frame = tmp_eval_stack->frame;
	if (frame->bindings) {
	    if (frame->bindings[0] == EXITBINDING (exit_proc)) {
		the_env = tmp_eval_stack->frame;
		eval_stack = tmp_eval_stack;
		return 1;
	    }
	    if (tmp_eval_stack->context == unwind_protect_symbol) {
		body = UNWINDBODY (*(frame->bindings[0]->val));
		the_env = tmp_eval_stack->frame;
		while (!EMPTYLISTP (body)) {
		    eval (CAR (body));
		    body = CDR (body);
		}
	    }
	}
	save_eval_stack = tmp_eval_stack;
	tmp_eval_stack = tmp_eval_stack->next;
    }

    the_env = save_eval_stack->frame;
    eval_stack = save_eval_stack;
    error ("unwound to end of stack without finding exit context",
	   exit_proc,
	   NULL);
    return 0;
}

struct module_binding *
new_module (Object module_name)
{
    struct module_binding *this_module;

    this_module = (struct module_binding *) allocate_module_binding ();
    this_module->sym = module_name;
    this_module->namespace = initialize_namespace (module_name);
    this_module->exported_bindings = make_table (DEFAULT_TABLE_SIZE);


    modules.bindings = (struct module_binding **)
	checking_realloc (modules.bindings,
			  (modules.size + BIND_ALLOC_CHUNK) *
			  sizeof (struct module_binding *));

    modules.bindings[modules.size] = this_module;
    modules.size++;

    return this_module;
}


struct module_binding *
module_binding (Object module_name)
{
    struct module_binding *binding;
    int i;

    for (i = 0; i < modules.size; ++i) {
	binding = modules.bindings[i];
	if (binding->sym == module_name) {
	    return (binding);
	}
    }
    error ("Unable to find binding for module", module_name, NULL);
}

Object
user_set_module (Object args)
{
    Object module_name;

    if (list_length (args) != 1) {
	return error ("set-module: requires a single argument", NULL);
    } else {
	module_name = CAR (args);
    }
    if (KEYWORDP (module_name)) {
	return
	    symbol_to_keyword (set_module
			       (module_binding
				(keyword_to_symbol (module_name)))
			       ->sym);
    } else {
	return error ("set-module: argument should be a symbol",
		      module_name,
		      NULL);
    }
}

struct module_binding *
set_module (struct module_binding *new_module)
{
    struct module_binding *old_module = current_module ();

    the_env = new_module->namespace;
    if (eval_stack && eval_stack->next == 0) {
	eval_stack = 0;
    }
    push_eval_stack (new_module->sym);
    eval_stack->frame = the_env;

    the_current_module = new_module;
    return old_module;
}

static void
import_top_level_binding (struct binding *import_binding,
			  struct binding **bindings,
			  int all_imports)
{
    struct binding *binding;

    binding = (struct binding *) allocate_binding ();

    binding->type = import_binding->type;
/*    binding->props |= import_binding->props & CONSTANT_BINDING; */
    binding->props |= import_binding->props;


    /* Share storage with old binding */
    binding->val = import_binding->val;
    binding->next = *bindings;
    *bindings = binding;
}

Object
use_module (Object module_name,
	    Object imports,
	    Object exclusions,
	    Object prefix,
	    Object renames,
	    Object exports)
{
    struct frame *frame;
    struct binding *binding;
    struct binding *bindings = NULL;
    struct binding *old_binding;
    struct module_binding *import_module;
    unsigned i;
    int all_imports;
    char *prefix_string;
    Object imports_table, exclusions_table, renames_table, exports_table;
    Object new_sym;

    /*
     * Store property sets in tables for quick reference.
     */
    renames_table = make_table (DEFAULT_TABLE_SIZE);
    if (imports != all_symbol) {
	imports_table = make_table (DEFAULT_TABLE_SIZE);
	fill_imports_table_from_property_set (imports_table,
					      imports,
					      renames_table);
    } else {
	all_imports = 1;
    }

    exclusions_table = make_table (DEFAULT_TABLE_SIZE);
    fill_table_from_property_set (exclusions_table, exclusions);

    fill_table_from_property_set (renames_table, renames);
    if (exports != all_symbol) {
	exports_table = make_table (DEFAULT_TABLE_SIZE);
	fill_table_from_property_set (exports_table, exports);
    }
    if (prefix == empty_string) {
	prefix_string = 0;
    } else {
	prefix_string = BYTESTRVAL (prefix);
    }

    /*
     * Now inspect all the bindings from the imported module.
     * If imports == all, this is probably a good idea.  On the other
     * hand, if imports != all, we might want to just look at the
     * symbols to be imported.
     */
    if (SYMBOLP (module_name)) {

	import_module = module_binding (module_name);
	frame = import_module->namespace;
	/* Look at each has location */
	for (i = 0; i < TOP_LEVEL_SIZE; i++) {
	    binding = frame->top_level_env[i];

	    /* Look at each bucket in a hash location */
	    while (binding) {
		if (IS_EXPORTED_BINDING (binding) &&
		    (import_module->exported_bindings == all_symbol ||
		     table_element (import_module->exported_bindings,
				    binding->sym,
				    false_object) != false_object) &&
		    (all_imports ||
		     (table_element (imports_table, binding, false_object)
		      != false_object))) {

		    /*
		     * This binding is importable.  Go for it.
		     */
		    import_top_level_binding (binding,
					      &bindings,
					      all_imports);
		    /*
		     * See what the bindings name needs to be.
		     */
		    if ((new_sym = table_element (renames_table,
						  binding->sym,
						  false_object))
			== false_object) {
			new_sym = prefix_string ? concat_prefix (prefix_string,
							       binding->sym)
			    : binding->sym;
		    }
		    /*
		     * See if we've already got this binding.
		     * Two possibilities here:
		     *  1. We've got a conflict with a symbol in our current
		     *     module (Squawk but don't die).
		     *  2. We're importing the same symbol again (Say nothing).
		     */
		    old_binding = symbol_binding_top_level (new_sym);
		    if (old_binding != NULL) {
			if (GFUNP (*(old_binding->val))
			    && GFUNP (*(bindings->val))) {
			    Object new_methods;

			    warning ("Adding methods to generic function", NULL);
			    /* Add methods to generic function */
			    for (new_methods = GFMETHODS (*(bindings->val));
				 !EMPTYLISTP (new_methods);
				 new_methods = CDR (new_methods)) {
				add_method (*(old_binding->val),
					    CAR (new_methods));
			    }
			} else if (old_binding->val != bindings->val) {
			    warning ("Ignoring import that conflicts with defined symbol",
				     binding->sym,
				     NULL);
			}
			bindings = bindings->next;
			binding = binding->next;
			continue;
		    }
		    /*
		     * Associate the new binding with this sym and determine
		     * if it is exportable.
		     */
		    bindings->sym = new_sym;
		    if (exports != all_symbol &&
			(table_element (exports_table,
					new_sym,
					false_object)
			 == false_object)) {
			/* This binding can't be exported */
			bindings->props &= !EXPORTED_BINDING;
			bindings->props |= IMPORTED_BINDING;
		    } else {
			bindings->props |= EXPORTED_BINDING;
			bindings->props |= IMPORTED_BINDING;
		    }
		}
		binding = binding->next;
	    }

	    /* Now put the bindings in place. */
	    while (bindings != NULL) {
		int h, i;
		char *str;

		binding = bindings;
		bindings = bindings->next;

		i = h = 0;
		str = SYMBOLNAME (binding->sym);
		while (str[i]) {
		    h += str[i++];
		}
		/*
		   h = h % TOP_LEVEL_SIZE;
		 */

		/* Works only if TOP_LEVEL_SIZE is a power of 2 */
		h &= (TOP_LEVEL_SIZE - 1);

		binding->next = the_env->top_level_env[h];
		the_env->top_level_env[h] = binding;
	    }
	}
    } else {
	error ("use: argument should be a symbol", module_name, NULL);
    }
    return unspecified_object;
}

Object
user_current_module ()
{
    return symbol_to_keyword (current_module ()->sym);
}

struct module_binding *
current_module ()
{
    return the_current_module;
}

Object
print_env (struct frame *env)
{
    struct frame *frame;
    int i;

    for (i = 0, frame = env; frame != NULL; frame = frame->next, i++) {
	fprintf (stderr, "#%d ", i);
	print_object(make_integer(STDERR), frame->owner, 1);
	fprintf (stderr, "\n");
    }
    return unspecified_object;
}

Object
show_bindings (Object args)
{
    struct frame *frame;
    int i;
    int slot;
    struct binding **bindings, *binding;
    int frame_number;

    if (list_length (args) != 1 || !INTEGERP (CAR (args))) {
	error ("show_bindings: requires a single <integer> argument", NULL);
    }
    frame_number = INTVAL (CAR (args));

    for (frame = the_env, i = frame_number;
	 i > 0 && frame != NULL;
	 frame = frame->next, i--) ;
    if (i != 0) {
	fprintf (stderr, "Frame number %d does not exist\n",
		 frame_number);
    } else {
	fprintf (stderr, "** Bindings for frame %d [",
		 frame_number);
	print_object (make_integer(STDERR), frame->owner, 1);
	fprintf (stderr, "]\n");
	/*
	 * Print the bindings in all the frame slots.
	 */
	for (bindings = frame->bindings, slot = 0;
	     slot < frame->size;
	     slot++) {
	    /*
	     * Print the bindings in one slot
	     */
	    for (binding = frame->bindings[slot];
		 binding != NULL;
		 binding = binding->next) {
		fprintf (stderr, "   ");
		print_object (make_integer(STDERR), binding->sym, 1);
		if (binding->type != object_class) {
		    fprintf (stderr, " :: ");
		print_object (make_integer(STDERR), binding->type, 1);
		}
		fprintf (stderr, " = ");
		print_object (make_integer(STDERR), *(binding->val), 1);
		fprintf (stderr, "\n");
	    }
	}

    }
    return unspecified_object;
}

Object
make_environment (struct frame *env)
{
    Object obj;

    obj = marlais_allocate_object (sizeof (struct environment));

    ENVIRONMENTTYPE (obj) = Environment;
    ENVIRONMENT (obj) = env;
    return obj;
}

static Object
concat_prefix (char *prefix_string, Object sym)
{
    char *new_str, *old_str = SYMBOLNAME (sym);
    int prefix_len = strlen (prefix_string);

    new_str = (char *) allocate_string (prefix_len + strlen (old_str) + 1);
    strcpy (new_str, prefix_string);
    strcpy (new_str + prefix_len, old_str);
    return make_symbol (new_str);
}

/*
 * Stores entry for each property in the_set in the_table.  If
 * property is an atom, the key and value are both the atom.
 * If property is a pair, the key is the CAR and the value is the CDR.
 */
void
fill_table_from_property_set (Object the_table, Object the_set)
{
    Object the_element;

    while (!EMPTYLISTP (the_set)) {
	the_element = CAR (the_set);
	if (PAIRP (the_element)) {
	    table_element_setter (the_table,
				  CAR (the_element),
				  CDR (the_element));
	} else {
	    table_element_setter (the_table, the_element, the_element);
	}
	the_set = CDR (the_set);
    }
}

/*
 * Like fill_table_from..., but stores variable renamings in renames_table.
 */
static void
fill_imports_table_from_property_set (Object imports_table,
				      Object imports_set,
				      Object renames_table)
{
    Object the_element;

    while (!EMPTYLISTP (imports_set)) {
	the_element = CAR (imports_set);
	if (PAIRP (the_element)) {
	    table_element_setter (imports_table, the_element, the_element);
	    table_element_setter (renames_table,
				  CAR (the_element),
				  CDR (the_element));
	} else {
	    table_element_setter (imports_table, the_element, the_element);
	}
	imports_set = CDR (imports_set);
    }
}
