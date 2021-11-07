/* syntax.h -- see COPYRIGHT for use */

#ifndef SYNTAX_H
#define SYNTAX_H

#include "common.h"

/* global objects */
extern Object type_class, initial_state_sym, next_state_sym;
extern Object current_element_sym;
extern Object signal_symbol, variable_keyword, range_keyword;
extern Object collection_keyword;
extern Object to_symbol, above_symbol, below_symbol, by_symbol;
extern Object forward_iteration_protocol_symbol;
extern Object lesser_symbol, greater_symbol;
extern Object lesser_equal_symbol, greater_equal_symbol;
extern Object plus_symbol;
extern Object object_class_symbol;
extern Object import_keyword;
extern Object exclude_keyword;
extern Object prefix_keyword;
extern Object rename_keyword;
extern Object export_keyword;
extern Object use_symbol;
extern Object export_symbol;
extern Object create_symbol;
extern Object empty_string;

typedef Object (*syntax_fun) (Object);

extern void marlais_register_syntax (void);
extern syntax_fun marlais_syntax_function (Object sym);

#endif
