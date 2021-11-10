/* apply.h -- see COPYRIGHT for use */

#ifndef MARLAIS_APPLY_H
#define MARLAIS_APPLY_H

#include <marlais/common.h>

/* Register apply primitives */
extern void marlais_register_apply (void);
/* Create the default result value */
extern Object marlais_default_result_value (void);
/* Internal version of apply (real one is in eval.c) */
extern Object marlais_apply_internal (Object fun, Object args);
/* Apply a specific method */
extern Object marlais_apply_method (Object meth,
                                    Object args,
                                    Object rest_methods,
                                    Object generic_apply);
/* Construct and check return values */
extern Object marlais_construct_return_values (Object ret,
                                               Object required_values,
                                               Object rest_values);

#endif
