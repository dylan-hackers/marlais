#ifndef MARLAIS_MODULE_H
#define MARLAIS_MODULE_H

#include <marlais/common.h>

struct modules {
    int size;
    struct module_binding **bindings;
};

struct module_binding {
    Object sym;
    struct frame *namespace;
    Object exported_bindings;
};

extern struct module_binding *marlais_get_module (Object module_name);
extern struct module_binding *marlais_new_module (Object module_name);
extern struct module_binding *marlais_set_module (struct module_binding *module);
extern struct module_binding *marlais_current_module (void);

extern Object marlais_use_module (Object module_name,
                                  Object imports,
                                  Object exclusions,
                                  Object prefix,
                                  Object renames,
                                  Object exports);

extern Object marlais_user_current_module (void);
extern Object marlais_user_set_module (Object args);

extern void marlais_add_binding(Object sym, Object val, int constant);
extern void marlais_add_export(Object sym, Object val, int constant);
extern void marlais_change_binding (Object sym, Object new_val);

extern struct binding *marlais_symbol_binding_top_level (Object sym);

#endif
