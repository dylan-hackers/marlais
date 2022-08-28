
#include <marlais/module.h>

#include <marlais/alloc.h>
#include <marlais/class.h>
#include <marlais/eval.h>
#include <marlais/function.h>
#include <marlais/prim.h>
#include <marlais/print.h>
#include <marlais/stream.h>
#include <marlais/table.h>

/* Local variables */

static Object marlais_all_modules;
static Object marlais_current_module;

/* Internal functions */

static void marlais_import_module_binding (struct binding *import_binding,
                                           struct binding **bindings,
                                           int all_imports);
static void marlais_add_module_binding(Object sym, Object val,
                                       int constant, int exported);
static void marlais_fill_imports_table_from_property_set (Object imports_table,
                                                          Object imports_set,
                                                          Object renames_table);

/* Primitives */

static Object prim_current_module (void);
static Object prim_set_module (Object mod_or_sym);
static Object prim_find_module (Object sym);
static Object prim_library_name (Object lib);
static Object prim_library_modules (Object lib);
static Object prim_module_name (Object mod);
static Object prim_module_environment (Object mod);

static struct primitive module_prims[] =
{
  {"current-module",      prim_0, prim_current_module},
  {"%set-module",         prim_1, prim_set_module},
  {"%find-module",        prim_1, prim_find_module},
#if 0
  {"%library-name",       prim_1, prim_library_name},
  {"%library-modules",    prim_1, prim_library_modules},
#endif
  {"%module-name",        prim_1, prim_module_name},
  {"%module-environment", prim_1, prim_module_environment},
};

/* Exported functions */

void
marlais_initialize_module (void)
{
  marlais_all_modules = MARLAIS_NIL;
  marlais_current_module = MARLAIS_FALSE;
}

void
marlais_register_module (void)
{
  MARLAIS_REGISTER_PRIMS (module_prims);
}

Object
marlais_get_current_module (void)
{
  return marlais_current_module;
}

Object
marlais_set_current_module (Object new_module)
{
  struct module *m = MODULE(new_module);
  Object old_module = marlais_get_current_module ();

  the_env = m->namespace;
  if (eval_stack && eval_stack->next == 0) {
    eval_stack = 0;
  }
  marlais_push_eval_stack (m->sym);
  eval_stack->frame = the_env;

  marlais_current_module = new_module;

  return old_module;
}

Object
marlais_make_module (Object module_name)
{
  struct module *module;

  /* construct the module */
  module = MARLAIS_ALLOCATE_OBJECT (Module, struct module);
  module->sym = module_name;
  module->namespace = marlais_make_toplevel (module);
  module->exported_bindings = marlais_make_table (DEFAULT_TABLE_SIZE);

  /* add to global list */
  marlais_all_modules = marlais_cons((Object)module, marlais_all_modules);

  /* return the module */
  return module;
}

Object
marlais_find_module (Object module_name)
{
  Object l;

  l = marlais_all_modules;
  while(!EMPTYLISTP(l)) {
    Object m = CAR(l);
    if (MODULE(m)->sym == module_name) {
      return m;
    }
    l = CDR(l);
  }

  return MARLAIS_FALSE;
}

void
marlais_add_binding (Object sym, Object val, int constant)
{
  marlais_add_module_binding(sym, val, constant, 0);
}

void
marlais_add_export (Object sym, Object val, int constant)
{
  marlais_add_module_binding(sym, val, constant, 1);
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
  } else if (marlais_instance_p (new_val, binding->type)) {
    *(binding->val) = new_val;
  } else {
    marlais_error ("attempt to assign variable an incompatible object",
                   sym, new_val, NULL);
  }
}

Object
marlais_use_module (Object module_name,
                    Object imports,
                    Object exclusions,
                    Object prefix,
                    Object renames,
                    Object exports)
{
  struct environment *frame;
  struct binding *binding;
  struct binding *bindings = NULL;
  struct binding *old_binding;
  struct module *import_module;
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
    marlais_fill_imports_table_from_property_set (imports_table,
                                                  imports,
                                                  renames_table);
  } else {
    all_imports = 1;
  }

  exclusions_table = marlais_make_table (DEFAULT_TABLE_SIZE);
  marlais_table_fill_properties (exclusions_table, exclusions);

  marlais_table_fill_properties (renames_table, renames);
  if (exports != all_symbol) {
    exports_table = marlais_make_table (DEFAULT_TABLE_SIZE);
    marlais_table_fill_properties (exports_table, exports);
  }
  if (prefix == marlais_empty_string) { /* is this check correct? */
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

    import_module = marlais_find_module (module_name);
    if (import_module == MARLAIS_FALSE) {
      marlais_fatal("Could not find import module", module_name);
    }
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
          marlais_import_module_binding (binding,
                                         &bindings,
                                         all_imports);
          /*
           * See what the bindings name needs to be.
           */
          if ((new_sym = marlais_table_element (renames_table,
                                                binding->sym,
                                                MARLAIS_FALSE))
              == MARLAIS_FALSE) {
            new_sym = prefix_string ? marlais_make_prefix_symbol
                                        (prefix_string, binding->sym)
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
                marlais_add_method (*(old_binding->val),
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
  return MARLAIS_UNSPECIFIED;
}

/* Primitives */

static Object prim_current_module (void)
{
  return marlais_current_module;
}

static Object prim_set_module (Object mod_or_sym)
{
  Object mod;
  if (SYMBOLP(mod_or_sym)) {
    mod = marlais_find_module (marlais_symbol_to_name (mod_or_sym));
  } else if (MODULEP(mod_or_sym)) {
    mod = mod_or_sym;
  } else {
    marlais_fatal("%set-module: Invalid argument", mod_or_sym, NULL);
  }
  return marlais_set_current_module (mod);
}

static Object prim_find_module (Object sym)
{
  return marlais_find_module (marlais_symbol_to_name (sym));
}

static Object prim_module_name (Object module)
{
  return marlais_name_to_symbol (MODULE(module)->sym);
}

static Object prim_module_environment (Object module)
{
  return (Object)MODULE(module)->namespace;
}

/* Internal functions */

static void
marlais_import_module_binding (struct binding *import_binding,
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

static void
marlais_add_module_binding(Object sym, Object val, int constant, int exported)
{
  struct binding *binding, *old_binding;
  int i;
  unsigned h;
  char *str;

  binding = MARLAIS_ALLOCATE_STRUCT (struct binding);

  if (PAIRP (sym)) {
    binding->sym = CAR (sym);
    binding->type = marlais_eval (SECOND (sym));
  } else {
    binding->sym = sym;
    binding->type = marlais_class_object;
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

  if (marlais_trace_bindings) {
    fprintf(stderr, "%s %s\n", (exported?"Export":"Binding"), str);
  }
}

/*
 * Like fill_table_from..., but stores variable renamings in renames_table.
 */
static void
marlais_fill_imports_table_from_property_set (Object imports_table,
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
