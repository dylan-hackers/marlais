#ifndef MARLAIS_MODULE_H
#define MARLAIS_MODULE_H

#include <marlais/common.h>

struct marlais_module {
    marlais_header_t header;
    Object sym;
    struct environment *namespace;
    Object exported_bindings;
};

#define MODULE(obj) ((struct marlais_module *)obj)

/* TODO do we want these exposed? */
extern Object marlais_all_modules;
extern Object marlais_current_module;

extern void marlais_initialize_module (void);
extern void marlais_register_module (void);

extern Object marlais_get_current_module (void);
extern Object marlais_set_current_module (Object new_module);

extern Object marlais_make_module (Object module_name);
extern Object marlais_find_module (Object module_name);

extern Object marlais_use_module (Object module_name,
                                  Object imports,
                                  Object exclusions,
                                  Object prefix,
                                  Object renames,
                                  Object exports);

extern void marlais_add_binding(Object sym, Object val, int constant);
extern void marlais_add_export(Object sym, Object val, int constant);
extern void marlais_change_binding (Object sym, Object new_val);

#endif
