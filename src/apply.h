/* apply.h -- see COPYRIGHT for use */

#ifndef APPLY_H
#define APPLY_H

#include "common.h"

/* global data */
extern int trace_functions;
extern int trace_level;
extern Object hash_values_symbol;
extern Object ResultValueStack;
Object default_result_value (void);

/* external functions */
void init_apply_prims (void);
Object apply (Object fun, Object args);
Object apply_internal (Object fun, Object args);
Object apply_method (Object meth,
		     Object args,
		     Object rest_methods,
		     Object generic_apply);
Object construct_return_values (Object ret,
				Object required_values,
				Object rest_values);

#endif
