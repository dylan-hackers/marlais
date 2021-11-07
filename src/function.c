/*

   function.c

   This software is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this software; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Original copyright notice follows:

   Copyright, 1993, Brent Benson.  All Rights Reserved.
   0.4 & 0.5 Revisions Copyright 1994, Joseph N. Wilson.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include <string.h>

#include "function.h"

#include "alloc.h"
#include "apply.h"
#include "class.h"
#include "env.h"
#include "eval.h"
#include "keyword.h"
#include "list.h"
#include "number.h"
#include "prim.h"
#include "symbol.h"
#include "table.h"
#include "values.h"
#include "vector.h"

extern Object x_symbol;

/* local function prototypes */

static Object generic_function_make (Object arglist);
static Object generic_function_mandatory_keywords (Object generic);
static Object function_values (Object func);
static Object make_specializers_from_params (Object);
static Object function_arguments (Object fun);
static int possible_method (Object meth, Object class_list);
static Object user_applicable_method_p (Object fun, Object sample_args);
static Object split_sorted_methods (Object methods, Object sample_args);
static Object build_sorted_handles (Object methods, Object current_group);
static Object sort_methods (Object methods, Object sample_args);
static int sort_driver (Object *pmeth1, Object *pmeth2);
static int same_specializers (Object s1, Object s2);
static int specializer_compare (Object spec1, Object spec2);
static Object find_method (Object generic, Object spec_list);
static Object remove_method (Object generic, Object method);
static Object debug_name_setter (Object method, Object name);
static int is_param_name (Object parameter_name);
static Object param_name_to_keyword (Object param_name);


/* primitives */

static struct primitive function_prims[] =
{
    {"%add-method", prim_2, add_method},
    {"%generic-function-make", prim_1, generic_function_make},
    {"%generic-function-methods", prim_1, generic_function_methods},
    {"%generic-function-mandatory-keywords", prim_1,
     generic_function_mandatory_keywords},
    {"%function-specializers", prim_1, function_specializers},
    {"%function-values", prim_1, function_values},
    {"%function-arguments", prim_1, function_arguments},
    {"%applicable-method?", prim_1_rest, user_applicable_method_p},
    {"%sorted-applicable-methods", prim_1_rest, sorted_applicable_methods},
    {"%find-method", prim_2, find_method},
    {"%remove-method", prim_2, remove_method},
    {"%debug-name-setter", prim_2, debug_name_setter},
};

/* function definitions */


void
init_function_prims (void)
{
    int num;

    num = sizeof (function_prims) / sizeof (struct primitive);

    marlais_register_prims (num, function_prims);
}

static void
keyword_list_insert (Object *list, Object key_binding)
{
    Object *tmp_ptr;
    int compare;
    char *key_name;

    key_name = SYMBOLNAME (CAR (key_binding));
    /* Search for insert point, then break */

    tmp_ptr = list;
    while (PAIRP (*tmp_ptr)) {
	compare = strcmp (key_name, SYMBOLNAME (CAR (CAR (*tmp_ptr))));
	if (compare < 0) {
	    tmp_ptr = &CDR (*tmp_ptr);
	} else if (compare > 0) {
	    break;
	} else {
	    marlais_error ("keyword specified twice", CAR (key_binding),
		   CAR (CAR (*tmp_ptr)), NULL);
	    return;
	}
    }
    *tmp_ptr = cons (key_binding, *tmp_ptr);
}

static int
next_parameter_is(Object params, Object type)
{
  return PAIRP(params) && (CAR(params) == type);
}

static void
parse_function_required_parameters (Object *params, Object *tmp_ptr)
{
  Object entry;

  *tmp_ptr = make_empty_list ();

  while (PAIRP (*params)) {	/* CONTAINS BREAK! */
    entry = CAR (*params);
    if (entry == hash_rest_symbol || entry == key_symbol ||
	entry == hash_values_symbol || entry == next_symbol) {
      break;
    }
    if (PAIRP (entry)) {
      (*tmp_ptr) = cons (listem (CAR (entry),
				 eval (SECOND (entry)),
				 NULL),
			 make_empty_list ());
    } else {
      *tmp_ptr = cons (listem (entry, object_class, NULL),
		       make_empty_list ());
    }
    tmp_ptr = &CDR (*tmp_ptr);
    *params = CDR (*params);
  }
}

static void
parse_method_next_parameter (Object meth_obj, Object *params)
{
  if (next_parameter_is(*params, next_symbol)) {
    *params = CDR (*params);
    if (PAIRP (*params)) {
      METHNEXTMETH (meth_obj) = CAR (*params);
      *params = CDR (*params);
    } else {
      marlais_error ("method #next designator not followed by a parameter", NULL);
    }
  } else {
    METHNEXTMETH (meth_obj) = next_method_symbol;
  }
}

static void
parse_function_rest_parameter (Object fn_obj, Object *params,
			       void (*assign_fn)(Object, Object))
{
  if (next_parameter_is(*params, hash_rest_symbol)) {
    *params = CDR (*params);
    if (PAIRP (*params)) {
      assign_fn (fn_obj, CAR (*params));
      *params = CDR (*params);
    } else {
      marlais_error ("generic function #rest designator not followed by a parameter",
	     NULL);
    }
  } else {
    assign_fn (fn_obj, NULL);
  }
}

static void
gf_rest_assign(Object gf_obj, Object val)
{
  GFRESTPARAM (gf_obj) = val;
}

static void
gf_rest_return_assign(Object gf_obj, Object params)
{
  if(params == NULL) return;
  if (PAIRP (CAR (params))) {
    GFRESTVALUES (gf_obj) = eval (SECOND (CAR (params)));
  } else {
    GFRESTVALUES (gf_obj) = object_class;
  }
}

static void
method_rest_assign(Object meth_obj, Object val)
{
  METHRESTPARAM (meth_obj) = val;
}

static void
method_rest_return_assign(Object meth_obj, Object params)
{
  if(params == NULL) return;
  if (PAIRP (CAR (params))) {
    METHRESTVALUES (meth_obj) = eval (SECOND (CAR (params)));
  } else {
    METHRESTVALUES (meth_obj) = object_class;
  }
}

static void
parse_function_return_parameters(Object functor, Object* params,
				 Object* tmp_ptr)
{
  Object entry, result_type;

  *params = CDR (*params);
  *tmp_ptr = make_empty_list ();

  while (PAIRP (*params)) {	/* CONTAINS BREAK! */
    entry = CAR (*params);
    if (entry == hash_rest_symbol) {
      break;
    }
    if (PAIRP (entry)) {
      result_type = eval (SECOND (entry));
    } else {
      result_type = object_class;
    }

    (*tmp_ptr) = cons (result_type, make_empty_list ());
    tmp_ptr = &CDR (*tmp_ptr);
    *params = CDR (*params);
  }
}

static void
parse_function_key_parameters(Object functor, Object* params,
			      Object* (*get_params_fn)(Object),
			      void (*bit_mask_fn)(Object, int),
			      void (*xform_key_fn)(Object, Object))
{
  Object entry;

  *get_params_fn (functor) = make_empty_list ();
  if (PAIRP (*params) && CAR (*params) == key_symbol) {
    bit_mask_fn(functor, 0);
    *params = CDR (*params);
    while (PAIRP (*params) && (CAR (*params) != hash_values_symbol)) {
      /* CONTAINS BREAK! */
      entry = CAR (*params);
      if (entry == allkeys_symbol) {
	break;
      }
      /* get a keyword-parameter pair */
      if (SYMBOLP (entry)) {
	keyword_list_insert (get_params_fn (functor),
			     listem (param_name_to_keyword (entry),
				     entry,
				     MARLAIS_FALSE,
				     NULL));
      } else if (PAIRP (entry) && is_param_name (CAR (entry)) &&
		 list_length (entry) == 2) {
	keyword_list_insert (get_params_fn (functor),
			     listem (param_name_to_keyword (CAR (entry)),
				     CAR (entry),
				     SECOND (entry),
				     NULL));
      } else if (PAIRP (entry) && KEYWORDP (CAR (entry))) {
	xform_key_fn(functor, entry);
      }
      *params = CDR(*params);
    }
    if (PAIRP (*params) && CAR (*params) == allkeys_symbol) {
      bit_mask_fn(functor, 1);
      *params = CDR (*params);
      if (PAIRP (*params) && CAR (*params) != hash_values_symbol) {
	marlais_error ("parameters follow #all-keys", *params);
      }
    }
  }
}

static Object*
method_keys(Object meth_obj)
{
  return &METHKEYPARAMS(meth_obj);
}

static Object*
gf_keys(Object gf_obj)
{
  return &GFKEYPARAMS(gf_obj);
}

static void
do_method_key(Object meth_obj, int all_p)
{
  if(all_p) {
    METHPROPS (meth_obj) |= METHALLKEYSMASK;
  }
}

static void
do_gf_key(Object gf_obj, int all_p)
{
  GFPROPS (gf_obj) |= (all_p ? GFALLKEYSMASK : GFKEYSMASK);
}

static void
xform_method_key_param(Object meth_obj, Object entry)
{
  int entry_length = list_length (entry);

  if (entry_length == 3) {
    /* key: key-name value */
    keyword_list_insert (&METHKEYPARAMS (meth_obj), entry);
  } else if (entry_length == 2) {
    /* key: key-name */
    keyword_list_insert (&METHKEYPARAMS (meth_obj),
			 listem (CAR (entry),
				 SECOND (entry),
				 MARLAIS_FALSE,
				 NULL));
  }
}

static void
xform_gf_key_param(Object gf_obj, Object entry)
{
  if(list_length (entry) == 3) {
    keyword_list_insert (&GFKEYPARAMS (gf_obj), entry);
  }
}

static void
parse_generic_function_parameters (Object gf_obj, Object params)
{
  Object entry, *tmp_ptr;

  tmp_ptr = &GFREQPARAMS (gf_obj);

  parse_function_required_parameters(&params, tmp_ptr);
  parse_function_rest_parameter(gf_obj, &params, gf_rest_assign);
  parse_function_key_parameters(gf_obj, &params, gf_keys, do_gf_key,
				xform_gf_key_param);

  if(next_parameter_is(params, hash_values_symbol)) {
    GFRESTVALUES (gf_obj) = NULL;
    tmp_ptr = &GFREQVALUES (gf_obj);
    parse_function_return_parameters(gf_obj, &params, tmp_ptr);
    parse_function_rest_parameter(gf_obj, &params, gf_rest_return_assign);
  } else {			/* no values specified */
    GFREQVALUES (gf_obj) = make_empty_list ();
    GFRESTVALUES (gf_obj) = object_class;
  }

    if (PAIRP (params)) {
	marlais_error ("objects encountered after parameter list", params, NULL);
    }
    if (trace_functions) {
	marlais_warning ("Got GF", GFNAME (gf_obj), NULL);
	marlais_warning (" Required parameters", GFREQPARAMS (gf_obj), NULL);
	marlais_warning (" Rest parameter", GFRESTPARAM (gf_obj), NULL);
	if (GFHASKEYS (gf_obj)) {
	    marlais_warning (" Has keys", NULL);
	    marlais_warning (" Key parameters", GFKEYPARAMS (gf_obj), NULL);
	}
	if (GFALLKEYS (gf_obj)) {
	    marlais_warning (" All Keys specified", NULL);
	}
	marlais_warning (" Required return values", GFREQVALUES (gf_obj), NULL);
	marlais_warning (" Rest return value type", GFRESTVALUES (gf_obj), NULL);
    }
}

Object
make_generic_function (Object name, Object params, Object methods)
{
    Object obj;

    obj = marlais_allocate_object (GenericFunction, sizeof (struct generic_function));

    GFNAME (obj) = name;
    parse_generic_function_parameters (obj, params);
    GFMETHODS (obj) = methods;

#ifdef MARLAIS_ENABLE_METHOD_CACHING
    GFCACHE (obj) = marlais_make_table (50);	/* Maybe make 50 a define? */
    GFACTIVENM (obj) = make_empty_list ();
#endif

    return (obj);
}

static void
parse_method_parameters (Object meth_obj, Object params)
{
  Object entry, *tmp_ptr, result_type;

  tmp_ptr = &METHREQPARAMS (meth_obj);

  parse_function_required_parameters(&params, tmp_ptr);
  parse_method_next_parameter(meth_obj, &params);
  parse_function_rest_parameter(meth_obj, &params, method_rest_assign);
  parse_function_key_parameters(meth_obj, &params, method_keys, do_method_key,
				xform_method_key_param);

  if(next_parameter_is(params, hash_values_symbol)) {
    METHRESTVALUES (meth_obj) = NULL;
    tmp_ptr = &METHREQVALUES (meth_obj);
    parse_function_return_parameters(meth_obj, &params, tmp_ptr);
    parse_function_rest_parameter(meth_obj, &params,method_rest_return_assign);
  } else {
    METHREQVALUES (meth_obj) = make_empty_list ();
    METHRESTVALUES (meth_obj) = object_class;
  }

  if (PAIRP (params)) {
    marlais_error ("objects encountered after parameter list", params, NULL);
  }
  if (trace_functions) {
    marlais_warning ("Got Method", METHNAME (meth_obj), NULL);
    marlais_warning (" Required parameters", METHREQPARAMS (meth_obj), NULL);
    marlais_warning (" Rest parameter", METHRESTPARAM (meth_obj), NULL);
    marlais_warning (" Key parameters", METHKEYPARAMS (meth_obj), NULL);
    if (METHALLKEYS (meth_obj)) {
      marlais_warning ("All Keys specified", NULL);
    }
    marlais_warning (" Required return values", METHREQVALUES (meth_obj), NULL);
    marlais_warning (" Rest return value type", METHRESTVALUES (meth_obj), NULL);
  }
}

static Object
create_generic_parameters (Object params)
{
    Object entry, gf_params;

    gf_params = make_empty_list ();

    /* first get required params */
    while (PAIRP (params)) {	/* CONTAINS BREAK! */
	entry = CAR (params);

	if (entry == hash_rest_symbol) {	/* skip #rest */
	    params = CDR (params);
	    if (PAIRP (params)) {
		params = CDR (params);
	    } else {
		marlais_error ("method #rest designator not followed by a parameter", NULL);
	    }
	    break;
	}
	/* don't convert #key or #value */
	if (entry == key_symbol || entry == hash_values_symbol) {
	    break;
	}
	if (PAIRP (entry)) {
	    entry = CAR (entry);
	}
	gf_params = append (gf_params, listem (entry, NULL));

	params = CDR (params);
    }

    /* next add generic rest parameter */
    gf_params = append (gf_params, listem (hash_rest_symbol, NULL));
    gf_params = append (gf_params, listem (x_symbol, NULL));

    /* I believe that all other parts of the generic function parameters
       ** should be the same as the initial method's
     */
    if (PAIRP (params)) {
	gf_params = append (gf_params, params);
    }
    return (gf_params);
}

Object
make_method (Object name, Object params, Object body, struct frame *env, int do_generic)
{
    Object obj, gf;

    obj = marlais_allocate_object (Method, sizeof (struct method));

    if (name) {
	METHNAME (obj) = name;
    } else {
	METHNAME (obj) = NULL;
    }
    parse_method_parameters (obj, params);
    METHBODY (obj) = body;
    METHENV (obj) = env;

#ifdef MARLAIS_ENABLE_METHOD_CACHING
    /* create my handle (will redo if replacing existing method) */
    METHHANDLE (obj) = make_handle (obj);
#endif

    if (do_generic && name) {
	gf = symbol_value (name);
	if (!gf) {
	    gf = make_generic_function (name,
					create_generic_parameters (params),
					make_empty_list ());
	    add_top_level_binding (name, gf, 0);
	}
	add_method (gf, obj);
	return (gf);
    } else {
	return (obj);
    }
}

Object
make_next_method (Object generic, Object rest_methods, Object args)
{
    Object obj;

    obj = marlais_allocate_object (NextMethod, sizeof (struct next_method));

#ifdef MARLAIS_ENABLE_METHOD_CACHING
    NMGF (obj) = generic;
    NMMETH (obj) = CAR (rest_methods);
    NMREST (obj) = CDR (rest_methods);
#else
    NMREST (obj) = rest_methods;
#endif
    NMARGS (obj) = args;
    return (obj);
}

static Object
generic_function_make (Object arglist)
{
    Object obj;
    Object required, rest, key, allkeys;
    Object ptr;

    required = FIRST (arglist);
    arglist = CDR (arglist);
    rest = FIRST (arglist);
    arglist = CDR (arglist);
    key = FIRST (arglist);
    arglist = CDR (arglist);
    allkeys = FIRST (arglist);

    for (ptr = required; PAIRP (ptr); ptr = CDR (ptr)) {
	if (!CLASSP (CAR (ptr))) {
	    marlais_error ("make: generic function specializer is not a class",
		   CAR (ptr),
		   NULL);
	} else {
	    CAR (ptr) = listem (unspecified_object, CAR (ptr), NULL);
	}
    }

    obj = marlais_allocate_object (GenericFunction, sizeof (struct generic_function));

    GFNAME (obj) = unspecified_object;
    GFREQPARAMS (obj) = required;

    if (rest != MARLAIS_FALSE) {
	GFRESTPARAM (obj) = rest;
    } else {
	GFRESTPARAM (obj) = NULL;
    }
    GFKEYPARAMS (obj) = key;
    if (allkeys == MARLAIS_FALSE) {
	GFPROPS (obj) &= !GFALLKEYSMASK;
    } else {
	GFPROPS (obj) |= GFALLKEYSMASK;
    }
    GFMETHODS (obj) = make_empty_list ();

#ifdef MARLAIS_ENABLE_METHOD_CACHING
    GFCACHE (obj) = marlais_make_table (50);	/* Maybe make 50 a define? */
    GFACTIVENM (obj) = make_empty_list ();
#endif

    return (obj);

}

Object
make_generic_function_driver (Object args)
{
    return marlais_error ("make: not implemented for generic functions", NULL);
}

/* local functions */

/* compare specializer lists s1 and s2 to see if each specializer in s1
 * is a subclass of the corresponding specializer in s2
 * list lengths are also compared.
 */
static int
sub_specializers (Object s1, Object s2)
{
    while (!EMPTYLISTP (s1) && !EMPTYLISTP (s2)) {
	if (!subtype (CAR (s1), CAR (s2))) {
	    return (0);
	}
	s1 = CDR (s1);
	s2 = CDR (s2);
    }

    if (!EMPTYLISTP (s1) || !EMPTYLISTP (s2))
	return (0);

    return (1);
}

/* add a method, replacing one with matching parameters
 * if it's already there
 */
Object
add_method (Object generic, Object method)
{
    Object new_specs, old_specs, methods, last, old_method, next_meth_list;

    new_specs = function_specializers (method);

/* check method for fit with generic specializers */
    old_specs = function_specializers (generic);

#ifdef MARLAIS_ENABLE_METHOD_CACHING
    /* invalidate next methods when new method added. */
    next_meth_list = GFACTIVENM (generic);
    while (PAIRP (next_meth_list)) {
	NMREST (CAR (next_meth_list)) = cons (MARLAIS_FALSE,
					      make_empty_list ());
	next_meth_list = CDR (next_meth_list);
    }
#endif

    if (!sub_specializers (new_specs, old_specs)) {
	marlais_error ("add-method: method specializers must be subtypes of generic func. specs.", method, NULL);
    }
    if (!GFRESTPARAM (generic) && METHRESTPARAM (method)) {
	marlais_error ("add-method: generic must have #rest parameters if method does",
	       method,
	       NULL);
    }
    methods = GFMETHODS (generic);
    last = 0;
    while (!EMPTYLISTP (methods)) {
	old_specs = function_specializers (CAR (methods));
	if (same_specializers (new_specs, old_specs)) {
	    old_method = CAR (methods);

#ifdef MARLAIS_ENABLE_METHOD_CACHING
	    METHHANDLE (method) = METHHANDLE (old_method);
	    HDLOBJ (METHHANDLE (method)) = method;
#endif

	    if (!last) {
		GFMETHODS (generic) = cons (method, CDR (methods));
		return (marlais_construct_values (2, method, old_method));
	    } else {
		CDR (last) = cons (method, CDR (methods));
		return (marlais_construct_values (2, method, old_method));
	    }
	}
	last = methods;
	methods = CDR (methods);
    }
    GFMETHODS (generic) = cons (method, GFMETHODS (generic));

#ifdef MARLAIS_ENABLE_METHOD_CACHING
    /* Invalidate the method cache */
    GFCACHE (generic) = marlais_make_table (50);
#endif

    return (marlais_construct_values (2, method, MARLAIS_FALSE));
}

Object
generic_function_methods (Object gen)
{
    if (!GFUNP (gen)) {
	marlais_error ("generic-function-methods: argument must be a generic function", gen, NULL);
    }
    return (GFMETHODS (gen));
}

static Object
generic_function_mandatory_keywords (Object generic)
{
    return (GFKEYPARAMS (generic));
}

Object
function_specializers (Object func)
{
    Object params;

    if (METHODP (func)) {
	params = METHREQPARAMS (func);
    } else if (GFUNP (func)) {
	params = GFREQPARAMS (func);
    } else {
	marlais_error ("function-specializers: arg. must be a method or generic function",
	       func,
	       NULL);
    }
    return make_specializers_from_params (params);
}

static Object
function_values (Object func)
{
    Object vals, rest;

    if (METHODP (func)) {
	vals = METHREQVALUES (func);
	rest = METHRESTVALUES (func);
    } else if (GFUNP (func)) {
	vals = GFREQVALUES (func);
	rest = GFRESTVALUES (func);
    } else {
	marlais_error ("function-values: arg. must be a method or generic function",
	       func,
	       NULL);
    }
    return marlais_construct_values (2,
				     vals,
				     rest == NULL ? MARLAIS_FALSE : rest);

}

static Object
make_specializers_from_params (Object params)
{
    Object specs, *tmp_ptr;

    for (specs = make_empty_list (), tmp_ptr = &specs;
	 PAIRP (params);
	 tmp_ptr = &CDR (*tmp_ptr), params = CDR (params)) {
	*tmp_ptr = cons (SECOND (CAR (params)), make_empty_list ());

    }
    return (specs);

}

/*
   returns three values:
   1) number of required parameters
   2) #t if takes rest, #f otherwise
   3) sequence of keywords or #f if no keywords
 */

static Object
function_arguments (Object fun)
{
    Object params, keywords;
    Object has_rest;

    switch (POINTERTYPE (fun)) {
    case GenericFunction:
	params = GFREQPARAMS (fun);
	if (GFALLKEYS (fun)) {
	    keywords = all_symbol;
	} else {
	    keywords = GFKEYPARAMS (fun);
	}
	if (GFRESTPARAM (fun)) {
	    has_rest = MARLAIS_TRUE;
	} else {
	    has_rest = MARLAIS_FALSE;
	}
	break;
    case Method:
	params = METHREQPARAMS (fun);
	if (METHALLKEYS (fun)) {
	    keywords = all_symbol;
	} else {
	    keywords = METHKEYPARAMS (fun);
	}
	if (METHRESTPARAM (fun)) {
	    has_rest = MARLAIS_TRUE;
	} else {
	    has_rest = MARLAIS_FALSE;
	}
	break;
    case Primitive:
	marlais_fatal ("function-arguments: cannot query arguments of a primitive");
    default:
	marlais_fatal ("function-arguments: bad argument");
    }
    return (marlais_construct_values (3, list_length_int (params), has_rest, keywords));
}

static int
find_keyword_in_list (Object keyword, Object keyword_list)
{
    if (keyword_list == all_symbol) {
	return 1;
    } else {
	while (PAIRP (keyword_list)) {
	    if (keyword == CAR (CAR (keyword_list))) {
		return 1;
	    }
	    keyword_list = CDR (keyword_list);
	}
    }
    return 0;
}

static Object
user_applicable_method_p (Object argfun, Object sample_args)
{
    return applicable_method_p (argfun, sample_args, 1);
}

/*
 * In applicable_method_p, strict_check is true if we should complain
 * about extra keyword arguments.  It should be set to 0 for internal
 * tests for generic function dispatch, etc.
 */
Object
applicable_method_p (Object argfun, Object sample_args, int strict_check)
{
    Object args, specs, samples, keywords;
    int num_required, i, check_keywords = 1;
    Object funs, fun;

    if (!METHODP (argfun) && !GFUNP (argfun)) {
	marlais_fatal ("applicable-method?: first argument must be a generic function or method");
    }
    if (METHODP (argfun)) {
	funs = cons (argfun, make_empty_list ());
    } else {
	strict_check = 0;
	funs = GFMETHODS (argfun);
    }

  fail:
    if (EMPTYLISTP (funs)) {
	return MARLAIS_FALSE;
    }
    while (PAIRP (funs)) {
	fun = CAR (funs);
	funs = CDR (funs);
	args = function_arguments (fun);
	specs = function_specializers (fun);

	/* Are there more sample args than required args? */
	num_required = INTVAL (FIRSTVAL (args));
	if (list_length (sample_args) < num_required) {
	    return (MARLAIS_FALSE);
	}
	/* Do the types of the required args match the
	   types of the sample args? */
	samples = sample_args;
	for (i = 0; i < num_required; ++i) {
	    if (!instance (CAR (samples), CAR (specs))) {
		goto fail;
/*              return (MARLAIS_FALSE); */
	    }
	    samples = CDR (samples);
	    specs = CDR (specs);
	}

	if (PAIRP (samples)) {
	  keywords = THIRDVAL (args);
	  /* If the method accepts keywords, make sure supplied keywords match */
	    if (PAIRP (keywords) || keywords == all_symbol) {
		if (keywords == all_symbol) {
		    check_keywords = 0;
		}
		/* If keywords != all_symbol, make sure rest of sample_args
		 * are keyword specified, and that all keywords
		 * in sample_args are in the keyword list
		 */
		while (PAIRP (samples)) {
		    if (!KEYWORDP (CAR (samples)) ||
			EMPTYLISTP (CDR (samples))) {
			/* Has non keyword where our method needs one */
			goto fail;
/*                      return (MARLAIS_FALSE); */
		    } else if (check_keywords) {
			if (strict_check &&
			  !find_keyword_in_list (CAR (samples), keywords)) {
			    /* Has a keyword not in the method */
			    goto fail;
/*                          return (MARLAIS_FALSE); */
			}
		    }
		    samples = CDR (CDR (samples));
		}
	    } else if (SECONDVAL (args) == MARLAIS_FALSE) {
		/* We have no rest parameter.  If there are more arguments, this
		 * ain't a match.
		 */
		return (MARLAIS_FALSE);
	    }
	}
    }
    /* We passed all of the tests. */
    return (MARLAIS_TRUE);
}

#ifdef MARLAIS_ENABLE_METHOD_CACHING
Object
recalc_next_methods (Object fun, Object meth, Object sample_args)
/* this function recalculates the end of the applicable methods list. */
{
    Object methods, app_methods, sorted_methods, method;
    int current_method_added;

    current_method_added = 0;
    methods = GFMETHODS (fun);
    app_methods = make_empty_list ();

    /* add all applicable methods */
    while (!EMPTYLISTP (methods)) {
	method = CAR (methods);
	if (applicable_method_p (method, sample_args, 0) != MARLAIS_FALSE) {
	    if (meth == method) {
		current_method_added = 1;
		/* detect add of current method */
	    }
	    app_methods = cons (method, app_methods);
	}
	methods = CDR (methods);
    }
    if (EMPTYLISTP (app_methods)) {
	return (make_empty_list ());
    }
    /* add current method if not there (could have been deleted? */
    if (!current_method_added) {
	app_methods = cons (meth, app_methods);
    }
    sorted_methods = FIRSTVAL (split_sorted_methods (app_methods, sample_args));

    /* blow away list up to first method after current method */
    while (!EMPTYLISTP (sorted_methods)) {
	if (meth == (CAR (sorted_methods))) {
	    sorted_methods = CDR (sorted_methods);
	    break;
	}
	sorted_methods = CDR (sorted_methods);
    }
    /* need to check for replacement of myself leaving my new self in the list
     * (If you can dig that) */
    if (!EMPTYLISTP (sorted_methods)) {
	/* my handle should point to my new self (if there is one) */
	if (HDLOBJ (METHHANDLE (meth)) == (CAR (sorted_methods))) {
	    sorted_methods = CDR (sorted_methods);
	}
    }
    return (sorted_methods);
}

static Object
build_sorted_handles (Object methods, Object current_group)
/* recursively build the cache entry */
{
    /* end case */
    if (EMPTYLISTP (methods)) {
	if (EMPTYLISTP (current_group)) {
	    return (make_empty_list ());
	} else {
	    return (cons (current_group, make_empty_list ()));
	}
    }
    /* add to current group or build new group into list */
    if (EMPTYLISTP (current_group) ||
	specializer_compare (function_specializers (HDLOBJ (CAR (current_group))),
			     function_specializers (CAR (methods))) == 0) {
	return (build_sorted_handles (CDR (methods),
			 cons (METHHANDLE (CAR (methods)), current_group)));
    } else {
	return (cons (current_group, build_sorted_handles (CDR (methods),
		   cons (METHHANDLE (CAR (methods)), make_empty_list ()))));
    }
}

static Object
broad_class (Object obj)
/* return the class that the specializer falls into */
{
    Object class_list, union_types;

    if (SINGLETONP (obj)) {
	return (objectclass (SINGLEVAL (obj)));
    } else if (LIMINTP (obj)) {
	return (integer_class);
    } else if (UNIONP (obj)) {
	class_list = make_empty_list ();
	union_types = UNIONLIST (obj);
	while (!EMPTYLISTP (union_types)) {
	    class_list = cons (broad_class (CAR (union_types)), class_list);
	    union_types = CDR (union_types);
	}
	return (make_union_type (class_list));
    } else {
	return (obj);
    }
}

static int
possible_method (Object meth, Object class_list)
/* detect whether this is a possible method for this list of classes of
   specializers */
{
    Object args, specs, samples;
    int num_required, i;

    args = function_arguments (meth);
    specs = function_specializers (meth);

    /* Are there more sample args than required args?
     */
    num_required = INTVAL (FIRSTVAL (args));
    if (list_length (class_list) < num_required) {
	return (0);
    }
    /* Are the classes of the required args supertypes of the
     * class list? */
    samples = class_list;
    for (i = 0; i < num_required; ++i) {
	if (!subtype (CAR (samples), broad_class (CAR (specs)))) {
	    return (0);
	}
	samples = CDR (samples);
	specs = CDR (specs);
    }

    /* We passed all of the tests. */
    return (1);
}

static Object
make_class_list (Object args, int count)
/* recursively build list of classes to match possible methods */
{
    if (EMPTYLISTP (args) || !count) {
	return (make_empty_list ());
    }
    return (cons (objectclass (CAR (args)), make_class_list (CDR (args), count - 1)));
}

Object
sorted_possible_method_handles (Object fun, Object sample_args)
/* build a cache entry, a list of lists of mutually ambiguous methods ordered
   overall by subtype */
{
    Object methods, maybe_methods, sorted_methods, sorted_handles, method,
      class_list;

    class_list = make_class_list (sample_args,
			      INTVAL (FIRSTVAL (function_arguments (fun))));
    methods = GFMETHODS (fun);
    maybe_methods = make_empty_list ();
    while (!EMPTYLISTP (methods)) {
	method = CAR (methods);
	if (possible_method (method, class_list)) {
	    maybe_methods = cons (method, maybe_methods);
	}
	methods = CDR (methods);
    }
    if (EMPTYLISTP (maybe_methods)) {
	return marlais_error ("No applicable methods", fun, sample_args, NULL);
    }
    sorted_methods = sort_methods (maybe_methods, sample_args);

    sorted_handles = build_sorted_handles (sorted_methods, make_empty_list ());
    return (sorted_handles);
}

#endif

Object
sorted_applicable_methods (Object fun, Object sample_args)
{
    Object methods, app_methods, method;

    methods = GFMETHODS (fun);
    app_methods = make_empty_list ();
    while (!EMPTYLISTP (methods)) {
	method = CAR (methods);
	if (applicable_method_p (method, sample_args, 0) != MARLAIS_FALSE) {
	    app_methods = cons (method, app_methods);
	}
	methods = CDR (methods);
    }
    if (EMPTYLISTP (app_methods)) {
	return marlais_error ("No applicable methods", fun, sample_args, NULL);
    }
    return split_sorted_methods (app_methods, sample_args);
}


static Object
split_sorted_methods (Object methods, Object sample_args)
/* split off two values for sorted_applicable_methods() */
{
    Object *prev_ptr, next;

    methods = sort_methods (methods, sample_args);

    for (prev_ptr = &methods, next = CDR (methods);
	 PAIRP (next);
	 prev_ptr = &CDR (*prev_ptr), next = CDR (next)) {
	if (specializer_compare (function_specializers (CAR (*prev_ptr)),
				 function_specializers (CAR (next))) == 0) {
	    next = *prev_ptr;
	    *prev_ptr = make_empty_list ();
	    break;
	}
    }
    return marlais_construct_values (2, methods, next);
}

/* See KLUDGE ALERT below */
Object sort_driver_args____;

static Object
sort_methods (Object methods, Object sample_args)
{
    Object method_vector;
    typedef int (*sortfun) ();

    /* KLUDGE ALERT!! Due to lack of closures in C, the following
     * is included as a public service to code readers.
     * We need the comparator for the sort to know about the
     * sample arguments.  These are stored in the static global
     * sort_driver_args____.
     */
    sort_driver_args____ = sample_args;

    if (PAIRP (CDR (methods))) {
	method_vector = marlais_make_sov (methods);
	qsort (SOVELS (method_vector),
	       SOVSIZE (method_vector),
	       sizeof (Object),
	         (sortfun) sort_driver);

	methods = marlais_vector_to_list (method_vector);
    }
    return (methods);
}

static int
sort_driver (Object *pmeth1, Object *pmeth2)
{
    Object specs1, specs2;

    specs1 = function_specializers (*pmeth1);
    specs2 = function_specializers (*pmeth2);
    return specializer_compare (specs1, specs2);
}

/* It is assumed that s1 and s2 have the same length. */
static int
same_specializers (Object s1, Object s2)
{
    while (!EMPTYLISTP (s1) && !EMPTYLISTP (s2)) {
	if (same_class_p (CAR (s1), CAR (s2)) == MARLAIS_FALSE) {
	    return (0);
	}
	s1 = CDR (s1);
	s2 = CDR (s2);
    }
    if (!EMPTYLISTP (s1) || !EMPTYLISTP (s2))
	return (0);
    return (1);
}

static int
specializer_compare (Object s1, Object s2)
{
    Object spec1, spec2, arg, specs1, specs2, args, class_list;
    int ret = 0;

    specs1 = s1;
    specs2 = s2;
    args = sort_driver_args____;

    while (!EMPTYLISTP (specs1)) {
	spec1 = CAR (specs1);
	spec2 = CAR (specs2);
	arg = CAR (args);

	if (spec1 == spec2) {
	    /* No help from this specializer */
	} else if (subtype (spec1, spec2)) {
	    /* This suggests less than */
	    if (ret <= 0) {
		ret = -1;
	    } else {
		/*
		 * We previously saw an indication of greater than.
		 * Thus, these two methods are unordered!
		 */
		return 0;
	    }
	} else if (subtype (spec2, spec1)) {
	    /* This suggests greater than */
	    if (ret >= 0) {
		ret = 1;
	    } else {
		/* We previously saw an indication of less than. */
		return 0;
	    }
	} else if (CLASSP (spec1) && CLASSP (spec2)) {
	    for (class_list = CLASSPRECLIST (objectclass (arg));
		 PAIRP (class_list);
		 class_list = CDR (class_list)) {
		if (spec1 == CAR (class_list)) {
		    if (ret <= 0) {
			ret = -1;
			break;
		    } else {
			return 0;
		    }
		} else if (spec2 == CAR (class_list)) {
		    if (ret >= 0) {
			ret = 1;
			break;
		    } else {
			return 0;
		    }
		}
	    }
	} else if (instance (arg, spec1)
		   && instance (arg, spec2)
		   && (!subtype (spec1, spec2))
		   && (!subtype (spec2, spec1))) {
	    /* These are ambiguous according to Design Note 8 */
	    return 0;
	}
	specs1 = CDR (specs1);
	specs2 = CDR (specs2);
	args = CDR (args);
    }
    return ret;
}

static Object
find_method (Object generic, Object spec_list)
{
    Object methods;

    for (methods = GFMETHODS (generic);
	 PAIRP (methods);
	 methods = CDR (methods)) {
	if (same_specializers (function_specializers (CAR (methods)),
			       spec_list)) {
	    return CAR (methods);
	}
    }
    return MARLAIS_FALSE;
}

static Object
remove_method (Object generic, Object method)
{
    Object *tmp_ptr;

    for (tmp_ptr = &GFMETHODS (generic);
	 PAIRP (*tmp_ptr);
	 tmp_ptr = &CDR (*tmp_ptr)) {
	/* need to add test for sealed function, when available */
	if (method == CAR (*tmp_ptr)) {
	    *tmp_ptr = CDR (*tmp_ptr);
	    return method;
	}
    }
    return marlais_error ("remove-method: generic function does not contain method",
		  generic, method, NULL);
}

static Object
debug_name_setter (Object method, Object name)
{
    METHNAME (method) = name;
    return (name);
}

static int
is_param_name (Object parameter_name)
{
    return SYMBOLP (parameter_name) ||
	(PAIRP (parameter_name) && SYMBOLP (CAR (parameter_name)));
}

static Object
param_name_to_keyword (Object param_name)
{
    return symbol_to_keyword (SYMBOLP (param_name) ? param_name
			      : CAR (param_name));
}
