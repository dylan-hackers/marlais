/* env.c -- see COPYRIGHT for use */

#include <marlais/env.h>

#include <marlais/alloc.h>
#include <marlais/bytestring.h>
#include <marlais/class.h>
#include <marlais/eval.h>
#include <marlais/function.h>
#include <marlais/list.h>
#include <marlais/number.h>
#include <marlais/prim.h>
#include <marlais/print.h>
#include <marlais/stream.h>
#include <marlais/table.h>


/* XXX remaining global */
struct modules modules;

/* the top level environment */
#define BIND_ALLOC_CHUNK 4

/* If TOP_LEVEL_SIZE is not a power of two, see change required below */
#define TOP_LEVEL_SIZE 1024
/* struct binding *top_level_env[TOP_LEVEL_SIZE]; */

/* Internal function declarations */

static struct frame *initialize_namespace (Object owner);
static void add_module_binding(Object sym, Object val,
			       int constant, int exported);

static Object concat_prefix (char *prefix_string, Object sym);
static void fill_imports_table_from_property_set (Object imports_table,
						  Object imports_set,
						  Object renames_table);

/* Primitives */

static struct primitive env_prims[] =
{
    {"current-module", prim_0, marlais_user_current_module},
    {"set-module", prim_0_rest, marlais_user_set_module},
#if 0
    {"reset-module", prim_0_rest, reset_module},
#endif
};

/* Exported functions */

void
marlais_register_env (void)
{
  int num = sizeof (env_prims) / sizeof (struct primitive);
  marlais_register_prims (num, env_prims);
}

Object
marlais_make_environment (struct frame *env)
{
  Object obj;
  obj = marlais_allocate_object (Environment, sizeof (struct environment));
  ENVIRONMENT (obj) = env;
  return obj;
}

struct frame *
marlais_current_environment (void)
{
  return (the_env);
}

void
marlais_push_scope (Object owner)
{
  struct frame *frame;

  /* push a new frame */
  frame = MARLAIS_ALLOCATE_STRUCT (struct frame);
  frame->owner = owner;
  frame->size = 0;
  frame->bindings = NULL;
  frame->next = the_env;
  frame->top_level_env = the_env->top_level_env;

  the_env = frame;
  eval_stack->frame = frame;
}

void
marlais_pop_scope (void)
{
  the_env = the_env->next;
}

void
marlais_add_locals (Object syms, Object vals, int constant, struct frame *to_frame)
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
    marlais_reallocate_memory (frame->bindings,
                               (frame->size + num_bindings) * sizeof(struct binding *));

  for (i = 0; i < num_bindings; ++i) {
    if ((!syms) || (!vals)) {
      marlais_error ("mismatched number of symbols and values", NULL);
    }
    binding = MARLAIS_ALLOCATE_STRUCT (struct binding);
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
marlais_add_local (Object sym, Object val, int constant, struct frame *to_frame)
{
  struct frame *frame;
  struct binding *binding;

  binding = MARLAIS_ALLOCATE_STRUCT (struct binding);
  if (PAIRP (sym)) {
    binding->sym = CAR (sym);
    binding->type = eval (SECOND (sym));
  } else {
    binding->sym = sym;
    binding->type = object_class;
  }
  binding->val = (Object *) marlais_allocate_memory (sizeof (Object *));

  if (!marlais_instance (val, binding->type)) {
    marlais_error ("add_local: value does not satisfy type constraint",
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
      marlais_reallocate_memory (frame->bindings,
                                 (frame->size + BIND_ALLOC_CHUNK) * sizeof (struct binding *));
  }
  frame->bindings[frame->size] = binding;
  frame->size++;
}

struct module_binding *
marlais_get_module (Object module_name)
{
  struct module_binding *binding;
  int i;

  for (i = 0; i < modules.size; ++i) {
    binding = modules.bindings[i];
    if (binding->sym == module_name) {
      return (binding);
    }
  }
  marlais_fatal ("Unable to find binding for module");
}

struct module_binding *
marlais_new_module (Object module_name)
{
  struct module_binding *this_module;

  this_module = MARLAIS_ALLOCATE_STRUCT (struct module_binding);
  this_module->sym = module_name;
  this_module->namespace = initialize_namespace (module_name);
  this_module->exported_bindings = marlais_make_table (DEFAULT_TABLE_SIZE);

  modules.size++;
  modules.bindings = (struct module_binding **)
    marlais_reallocate_memory (modules.bindings,
                               (modules.size * sizeof (struct module_binding *)));

  modules.bindings[modules.size - 1] = this_module;

  return this_module;
}

struct module_binding *
marlais_set_module (struct module_binding *new_module)
{
  struct module_binding *old_module = marlais_current_module ();

  the_env = new_module->namespace;
  if (eval_stack && eval_stack->next == 0) {
    eval_stack = 0;
  }
  push_eval_stack (new_module->sym);
  eval_stack->frame = the_env;

  the_current_module = new_module;
  return old_module;
}

struct module_binding *
marlais_current_module ()
{
  return the_current_module;
}

void
marlais_add_binding (Object sym, Object val, int constant)
{
  add_module_binding(sym, val, constant, 0);
}

void
marlais_add_export (Object sym, Object val, int constant)
{
  add_module_binding(sym, val, constant, 1);
}

void
marlais_change_binding (Object sym, Object new_val)
{
  struct binding *binding;

  binding = marlais_symbol_binding_top_level (sym);
  if (!binding) {
    marlais_error ("attempt to modify value of unbound symbol", sym, NULL);
  } else if (IS_CONSTANT_BINDING (binding)) {
    marlais_error ("attempt to modify value of a constant", sym, NULL);
  } else if (marlais_instance (new_val, binding->type)) {
    *(binding->val) = new_val;
  } else {
    marlais_error ("attempt to assign variable an incompatible object",
                   sym, new_val, NULL);
  }
}

static void
import_top_level_binding (struct binding *import_binding,
                          struct binding **bindings,
                          int all_imports)
{
  struct binding *binding;

  binding = MARLAIS_ALLOCATE_STRUCT (struct binding);
  binding->type = import_binding->type;
  /* binding->props |= import_binding->props & CONSTANT_BINDING; */
  binding->props |= import_binding->props;

  /* Share storage with old binding */
  binding->val = import_binding->val;
  binding->next = *bindings;
  *bindings = binding;
}

Object
marlais_use_module (Object module_name,
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
  renames_table = marlais_make_table (DEFAULT_TABLE_SIZE);
  if (imports != all_symbol) {
    imports_table = marlais_make_table (DEFAULT_TABLE_SIZE);
    fill_imports_table_from_property_set (imports_table,
                                          imports,
                                          renames_table);
  } else {
    all_imports = 1;
  }

  exclusions_table = marlais_make_table (DEFAULT_TABLE_SIZE);
  fill_table_from_property_set (exclusions_table, exclusions);

  fill_table_from_property_set (renames_table, renames);
  if (exports != all_symbol) {
    exports_table = marlais_make_table (DEFAULT_TABLE_SIZE);
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
  if (NAMEP (module_name)) {

    import_module = marlais_get_module (module_name);
    frame = import_module->namespace;
    /* Look at each has location */
    for (i = 0; i < TOP_LEVEL_SIZE; i++) {
      binding = frame->top_level_env[i];

      /* Look at each bucket in a hash location */
      while (binding) {
        if (IS_EXPORTED_BINDING (binding) &&
            (import_module->exported_bindings == all_symbol ||
             marlais_table_element (import_module->exported_bindings,
                                    binding->sym,
                                    MARLAIS_FALSE) != MARLAIS_FALSE) &&
            (all_imports ||
             (marlais_table_element (imports_table, binding, MARLAIS_FALSE)
              != MARLAIS_FALSE))) {

          /*
           * This binding is importable.  Go for it.
           */
          import_top_level_binding (binding,
                                    &bindings,
                                    all_imports);
          /*
           * See what the bindings name needs to be.
           */
          if ((new_sym = marlais_table_element (renames_table,
                                                binding->sym,
                                                MARLAIS_FALSE))
              == MARLAIS_FALSE) {
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
          old_binding = marlais_symbol_binding_top_level (new_sym);
          if (old_binding != NULL) {
            if (GFUNP (*(old_binding->val))
                && GFUNP (*(bindings->val))) {
              Object new_methods;

              marlais_warning ("Adding methods to generic function", NULL);
              /* Add methods to generic function */
              for (new_methods = GFMETHODS (*(bindings->val));
                   !EMPTYLISTP (new_methods);
                   new_methods = CDR (new_methods)) {
                add_method (*(old_binding->val),
                            CAR (new_methods));
              }
            } else if (old_binding->val != bindings->val) {
              marlais_warning ("Ignoring import that conflicts with defined symbol",
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
              (marlais_table_element (exports_table,
                                      new_sym,
                                      MARLAIS_FALSE)
               == MARLAIS_FALSE)) {
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
    marlais_error ("use: argument should be a symbol", module_name, NULL);
  }
  return unspecified_object;
}

Object
marlais_user_set_module (Object args)
{
  Object module_name;

  if (list_length (args) != 1) {
    return marlais_error ("set-module: requires a single argument", NULL);
  } else {
    module_name = CAR (args);
  }
  if (SYMBOLP (module_name)) {
    return
      marlais_name_to_symbol (marlais_set_module
                              (marlais_get_module
                               (marlais_symbol_to_name (module_name)))
                              ->sym);
  } else {
    return marlais_error ("set-module: argument should be a symbol",
                          module_name,
                          NULL);
  }
}

Object
marlais_user_current_module ()
{
  return marlais_name_to_symbol (marlais_current_module ()->sym);
}

Object
marlais_symbol_value (Object sym)
{
  struct binding *binding;

  binding = marlais_symbol_binding (sym);
  if (!binding) {
    return (NULL);
  }
  return (*(binding->val));
}

void
marlais_modify_value (Object sym, Object new_val)
{
  struct binding *binding;

  binding = marlais_symbol_binding (sym);
  if (!binding) {
    marlais_error ("attempt to modify value of unbound symbol", sym, NULL);
  } else if (IS_CONSTANT_BINDING (binding)) {
    marlais_error ("attempt to modify value of a constant", sym, NULL);
  } else if (marlais_instance (new_val, binding->type)) {
    *(binding->val) = new_val;
  } else {
    marlais_error ("attempt to assign variable an incompatible object",
                   sym, new_val, NULL);
  }
}

/* made symbol_binding non local to be able to fix <object> binding */
struct binding *
marlais_symbol_binding (Object sym)
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
      break; /* <pcb> I/'ve observed this to be nil in a special case. */
  }
  /* can't find binding in frames, look at top_level */
  return (marlais_symbol_binding_top_level (sym));
}

struct binding *
marlais_symbol_binding_top_level (Object sym)
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

/* Internal functions */

static struct frame *
initialize_namespace (Object owner)
{
  struct frame *frame;

  frame = MARLAIS_ALLOCATE_STRUCT (struct frame);
  frame->size = TOP_LEVEL_SIZE;
  frame->owner = owner;
  frame->bindings =
    (struct binding **) marlais_allocate_memory (TOP_LEVEL_SIZE * sizeof (struct binding));
  frame->next = NULL;
  frame->top_level_env = frame->bindings;

  return frame;
}

static void
add_module_binding(Object sym, Object val, int constant, int exported)
{
  struct binding *binding, *old_binding;
  int i;
  unsigned h;
  char *str;

  binding = MARLAIS_ALLOCATE_STRUCT (struct binding);

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
  old_binding = marlais_symbol_binding_top_level (binding->sym);
  if (old_binding != NULL) {
    marlais_warning ("Symbol already defined. Previous value", sym,
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
    marlais_print_obj (marlais_standard_error, sym);
  }
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
  marlais_error ("unwound to end of stack without finding exit context",
                 exit_proc,
                 NULL);
  return 0;
}

Object
print_env (struct frame *env)
{
  struct frame *frame;
  int i;

  for (i = 0, frame = env; frame != NULL; frame = frame->next, i++) {
    fprintf (stderr, "#%d ", i);
    marlais_print_object(marlais_standard_error, frame->owner, 1);
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
    marlais_error ("show_bindings: requires a single <integer> argument", NULL);
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
    marlais_print_object (marlais_standard_error, frame->owner, 1);
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
        marlais_print_object (marlais_standard_error, binding->sym, 1);
        if (binding->type != object_class) {
          fprintf (stderr, " :: ");
          marlais_print_object (marlais_standard_error, binding->type, 1);
        }
        fprintf (stderr, " = ");
        marlais_print_object (marlais_standard_error, *(binding->val), 1);
        fprintf (stderr, "\n");
      }
    }

  }
  return unspecified_object;
}

/* XXX move to symbol.c */
static Object
concat_prefix (char *prefix_string, Object sym)
{
  char *new_str, *old_str = SYMBOLNAME (sym);
  int prefix_len = strlen (prefix_string);
  size_t new_len = prefix_len + strlen(old_str) + 1;

  new_str = MARLAIS_ALLOCATE_STRING(new_len);
  strcpy (new_str, prefix_string);
  strcpy (new_str + prefix_len, old_str);

  return marlais_make_name (new_str);
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
      marlais_table_element_setter (the_table,
                                    CAR (the_element),
                                    CDR (the_element));
    } else {
      marlais_table_element_setter (the_table, the_element, the_element);
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
      marlais_table_element_setter (imports_table, the_element, the_element);
      marlais_table_element_setter (renames_table,
                                    CAR (the_element),
                                    CDR (the_element));
    } else {
      marlais_table_element_setter (imports_table, the_element, the_element);
    }
    imports_set = CDR (imports_set);
  }
}
