
#include <marlais/object/module.h>

#include <marlais/core/eval.h>
#include <marlais/core/print.h>
#include <marlais/object/class.h>
#include <marlais/object/function.h>
#include <marlais/object/prim.h>
#include <marlais/object/stream.h>
#include <marlais/object/table.h>

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
marlais_register_module (void)
{
  MARLAIS_REGISTER_PRIMS (module_prims);
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
