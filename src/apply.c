/* apply.c -- see COPYRIGHT for use */

#include <marlais/apply.h>

#include <marlais/alloc.h>
#include <marlais/class.h>
#include <marlais/env.h>
#include <marlais/eval.h>
#include <marlais/function.h>
#include <marlais/keyword.h>
#include <marlais/list.h>
#include <marlais/number.h>
#include <marlais/print.h>
#include <marlais/prim.h>
#include <marlais/stream.h>
#include <marlais/symbol.h>
#include <marlais/syntax.h>
#include <marlais/table.h>
#include <marlais/values.h>
#include <marlais/vector.h>

/* Global data */
int trace_functions = 0;
int trace_only_user_funs = 0;
int trace_level = 0;
Object ResultValueStack;
static Object user_keyword;

/* Internal functions */

static Object apply_generic (Object gen, Object args);
static void narrow_value_types (Object *values_list,
                                Object new_values_list,
                                Object *rest_type,
                                Object new_rest_type);
static Object apply_exit (Object exit_proc, Object args);
static Object apply_next_method (Object next_method, Object args);
static void devalue_args (Object args);

/* Primitives */

static Object set_trace (Object bool);

static struct primitive apply_prims[] =
{
    {"%apply", prim_2, marlais_apply},
    {"%trace", prim_1, set_trace},
    {"%eval", prim_1, eval},
};

/* Exported functions */

void
marlais_register_apply (void)
{
    int num;

    num = sizeof (apply_prims) / sizeof (struct primitive);

    marlais_register_prims (num, apply_prims);

    user_keyword = make_keyword ("user:");
    ResultValueStack = make_empty_list ();
}

Object
marlais_default_result_value (void)
{
    return cons (make_empty_list (), object_class);
}

Object
marlais_apply_internal (Object fun, Object args)
{
    Object ret;

    if (trace_functions) {
	int i;

	if ((!trace_only_user_funs) || (!PRIMP (fun))) {
	    printf ("; ");
	    for (i = 0; i < trace_level; ++i) {
		putchar ('-');
	    }
	    marlais_print_object (marlais_standard_output, fun, 1);
	    printf (" called with ");
	    marlais_print_object (marlais_standard_output, args, 1);
	    printf ("\n");
	    trace_level++;
	}
    }
#ifdef MARLAIS_OBJECT_MODEL_SMALL
    if (!POINTERP (fun)) {
	return marlais_error ("apply: cannot apply this object", fun, NULL);
    }
#endif

    devalue_args (args);
    switch (POINTERTYPE (fun)) {
    case Primitive:
	ret = marlais_apply_prim (fun, args);
	break;
    case Method:
	ret = marlais_apply_method (fun, args, make_empty_list (), NULL);
	break;
    case GenericFunction:
	ret = apply_generic (fun, args);
	break;
    case NextMethod:
	ret = apply_next_method (fun, args);
	break;
    case Exit:
	ret = apply_exit (fun, args);
	break;
    default:
	marlais_error ("apply: cannot apply this object", fun, NULL);
    }
    if (trace_functions && trace_level) {
	int i;

	if ((!trace_only_user_funs) || (!PRIMP (fun))) {
	    trace_level--;
	    printf ("; ");
	    for (i = 0; i < trace_level; ++i) {
		printf ("-");
	    }
	    printf ("returned: ");
	    marlais_print_object(marlais_standard_output, ret, 1);
	    printf ("\n");
	}
    }
    return (ret);
}

/*
 * It seems to me that apply method has gotten a little big.
 * It could benefit from modularizing in a rewrite.
 *              -jnw
 */
Object
marlais_apply_method (Object meth, Object args, Object rest_methods, Object generic_apply)
{
    Object params, param, sym, val, body, ret;
    Object dup_list;
    Object rest_var, class, keyword, keys;
    Object *tmp_ptr;
    int hit_rest, hit_key, hit_values;
    struct frame *old_env;

    if (trace_functions && trace_level) {
	int i;

	if (!trace_only_user_funs) {
	    printf ("; ");
	    for (i = 0; i < trace_level; ++i) {
		putchar ('-');
	    }
	    printf ("apply-method applying ");
	    marlais_print_object (marlais_standard_output, meth, 1);
	    printf (" with args ");
	    marlais_print_object (marlais_standard_output, args, 1);
	    printf ("\n");
	}
    }
    ret = unspecified_object;
    params = METHREQPARAMS (meth);
    body = METHBODY (meth);


    /* remember current environment and subsitute with
       environment present at method creation time */
    old_env = the_env;
    the_env = METHENV (meth);

    push_scope (meth);

#ifdef MARLAIS_ENABLE_METHOD_CACHING
    /* next-method stuff only applies to generic function method */
    if (generic_apply) {

	/* re-calculate next methods if invalidated. */
	if (PAIRP (rest_methods) && CAR (rest_methods) == MARLAIS_FALSE) {
	    rest_methods = recalc_next_methods (generic_apply, meth, args);
	}
#endif

	/* install of next method object if there are next methods */
	if (PAIRP (rest_methods)) {
	    /* check use of empty_list vs. NULL!! */
	    Object next_method;

	    /* make next-method and push it into the GF list */
	    next_method = make_next_method (generic_apply, rest_methods, args);

#ifdef MARLAIS_ENABLE_METHOD_CACHING
	    /* push next method on active list */
	    GFACTIVENM (generic_apply) =
		cons (next_method, GFACTIVENM (generic_apply));
#endif

	    /* make constant binding for next method */
	    add_binding (METHNEXTMETH (meth), next_method, 1, the_env);
	}
#ifdef MARLAIS_ENABLE_METHOD_CACHING
    }
#endif
    hit_rest = hit_key = hit_values = 0;

    /* first process required parameters */
    while ((PAIRP (params) && PAIRP (args))
	   && (!hit_rest) && (!hit_key) && !(hit_values)) {
	param = CAR (params);
	if (param == hash_rest_symbol) {
	    hit_rest = 1;
	} else if (param == key_symbol) {
	    hit_key = 1;
	} else if (param == hash_values_symbol) {
	    hit_values = 1;
	} else {
	    val = CAR (args);
	    if (SYMBOLP (param)) {
		sym = param;
	    } else {
		sym = FIRST (param);
		class = SECOND (param);
		if (!marlais_instance (val, class)) {
		    marlais_error ("apply: argument doesn't match method specializer",
			   val, class, meth, NULL);
		}
	    }
	    add_binding (sym, val, 0, the_env);
	    args = CDR (args);
	    params = CDR (params);
	}
    }
    /* now process #rest and #key parameters */

    if ((rest_var = METHRESTPARAM (meth)) != NULL) {
	add_binding (rest_var, args, 0, the_env);
    }
    if (PAIRP (METHKEYPARAMS (meth))) {
	/* copy keys */
	keys = copy_list (METHKEYPARAMS (meth));
	dup_list = make_empty_list ();	/* For duplicate keywords */

	/* Bind each of the keyword args that is present. */
	while (!EMPTYLISTP (args)) {
	    keyword = FIRST (args);
	    if (!KEYWORDP (keyword)) {
		/* jnw -- check this out! */
		if (!rest_var) {
		    marlais_error ("apply: argument to method must be keyword", meth, keyword, NULL);
		} else {
		    args = CDR (args);
		    continue;
		}
	    }
	    val = SECOND (args);

	    /* if keyword is in the keys list then
	     * 1) add a binding for keyword to val
	     * 2) remove the keyword entry from keys
	     */

	    for (tmp_ptr = &keys;
		 PAIRP (*tmp_ptr);
		 tmp_ptr = &CDR (*tmp_ptr)) {
		if (CAR (CAR (*tmp_ptr)) == keyword) {
		    break;
		}
	    }
	    if (EMPTYLISTP (*tmp_ptr)) {
		if (member (keyword, dup_list)) {
		    marlais_warning ("Duplicate keyword value ignored",
			     keyword,
			     val,
			     0);
		} else if (!METHALLKEYS (meth)) {
		    marlais_error ("apply: Keyword argument not in parameter list",
			   keyword,
			   0);
		}
	    } else {
		add_binding (SECOND (CAR (*tmp_ptr)), val, 0, the_env);
		dup_list = cons (keyword, dup_list);
		*tmp_ptr = CDR (*tmp_ptr);
	    }
	    args = CDR (CDR (args));
	}
	/* Bind the missing keyword args to default_object */
	while (PAIRP (keys)) {
	    add_binding (SECOND (CAR (keys)),
			 eval (THIRD (CAR (keys))),
			 0,
			 the_env);
	    keys = CDR (keys);
	}

    }
    if (PAIRP (args) && !rest_var) {
	/*
	 * Shouldn't check for all args used if applying method through
	 * a generic function or as a next method.
	 * Must check if applying directly.
	 */
	if (METHALLKEYS (meth)) {
	    /* skip rest of parameters if they are keywords */
	    while (PAIRP (args)) {
		if (!KEYWORDP (CAR (args))) {
		    marlais_error ("apply: keyword argument expected", CAR (args),
			   NULL);
		} else if (!PAIRP (CDR (args))) {
		    marlais_error ("apply: keyword has no associated argument value",
			   CAR (args), NULL);
		}
		args = CDR (CDR (args));
	    }
	} else {
	    marlais_error ("Arguments have no matching parameters", args, NULL);
	}
    }
    if (PAIRP (params)) {
	marlais_error ("Required parameters have no matching arguments", params,
	       NULL);
    }
    while (!EMPTYLISTP (body)) {
	Object form = CAR (body);

#ifdef MARLAIS_ENABLE_TAIL_CALL_OPTIMIZATION
	/* when in tail form, we use tail_eval */
	if (EMPTYLISTP (CDR (body))) {
	    if (trace_functions) {
		if (!trace_only_user_funs)
		    marlais_warning ("tail position: ", form, NULL);
		if (trace_level)
		    --trace_level;
	    }
	    /* tail recursion optimization. */

	    /* If return values of this method are narrower types
	     * than what is currently on top of the ResultValueStack,
	     * trim it down to match.
	     */


	    narrow_value_types (&CAR (CAR (ResultValueStack)),
				METHREQVALUES (meth),
				&CDR (CAR (ResultValueStack)),
				METHRESTVALUES (meth));

	    ret = tail_eval (form);
	} else {
#endif

	    ret = marlais_construct_return_values (eval (form),
					   METHREQVALUES (meth),
					   METHRESTVALUES (meth));
#ifdef MARLAIS_ENABLE_TAIL_CALL_OPTIMIZATION
	}
#endif

	body = CDR (body);
    }

#ifdef MARLAIS_ENABLE_METHOD_CACHING
    /* pop out the next method that I put in the GF. */
    if (generic_apply && PAIRP (rest_methods)) {

	/* case controlling push */
	GFACTIVENM (generic_apply) = CDR (GFACTIVENM (generic_apply));
    }
#endif

    /* When the_env disappears, we'll need this pop_scope()! */
    pop_scope ();

    /* re-assert environment present at the beginning of this function
     */
    the_env = old_env;

    return ret;
}

/* Internal functions */

static void
narrow_value_types (Object *values_list_ptr,
		    Object new_values_list,
		    Object *rest_type,
		    Object new_rest_type)
{
    Object values_list;

    /* First check each value common to both lists.
     * If a new value is a subtype, substitute it.
     */
    for (; !EMPTYLISTP (*values_list_ptr);
	 values_list_ptr = &CDR (*values_list_ptr),
	 new_values_list = CDR (new_values_list)) {
	if (EMPTYLISTP (new_values_list)) {
	    break;
	}
	if (marlais_subtype (CAR (new_values_list), CAR (*values_list_ptr))) {
	    CAR (*values_list_ptr) = CAR (new_values_list);
	}
    }

    if (EMPTYLISTP (*values_list_ptr)) {
	/* We had enough values in the new list to match all the old ones */

	/* If there were more new_values than old.
	 * They must match the rest type of the old list, and must
	 * be added to the list.
	 */
	while (!EMPTYLISTP (new_values_list)) {
	    if (marlais_subtype (CAR (new_values_list), *rest_type)) {
		*values_list_ptr = cons (CAR (new_values_list),
					 make_empty_list ());
	    } else {
		*values_list_ptr = cons (*rest_type, make_empty_list ());
	    }
	    values_list_ptr = &CDR (*values_list_ptr);
	    new_values_list = CDR (new_values_list);
	}
    } else {
	/* We didn't match all the values.
	 * Make sure the remaining values are equally as narrow as
	 * new_rest_values
	 */
	if (new_rest_type == NULL) {
	    marlais_error ("Incompatible value specification in call", NULL);
	}
	values_list = *values_list_ptr;
	while (!EMPTYLISTP (values_list)) {
	    if (marlais_subtype (new_rest_type, CAR (values_list))) {
		CAR (values_list) = new_rest_type;
	    }
	    values_list = CDR (values_list);
	}
    }
    if (new_rest_type == NULL) {
	/* No rest values are allowed to be returned */
	*rest_type = NULL;
    } else if (*rest_type == NULL || marlais_subtype (*rest_type, new_rest_type)) {
	*rest_type = new_rest_type;
    }
}

Object
marlais_construct_return_values (Object ret,
				 Object required_values,
				 Object rest_values)
{
    int i, j;
    Object newret;

    /* To save effort, I make sure the return is a VALUES object.
     * This is a waste of effort and really ought to be fixed.
     * <pcb> could at least wrap it in a stack variable to avoid an alloc.
     */

    ResultValueStack = cons (marlais_default_result_value (), ResultValueStack);

    if (!ret) {
	/*
	 * This happened on an sgi, but I haven't tracked down the problem yet.
	 * It seems like an internal error, but I need to find its source.
	 */
	marlais_error ("return value is invalid", NULL);
    }
    if (!VALUESP (ret)) {
	ret = marlais_make_values (listem (ret, NULL));
    }
    /* check return values (not done for non VALUESTYPE values yet */
    for (i = 0;
	 i < VALUESNUM (ret) && PAIRP (required_values);
	 i++, required_values = CDR (required_values)) {
	if (!marlais_instance (VALUESELS (ret)[i], CAR (required_values))) {
	    marlais_error ("in value return: return value is not of correct type",
		   VALUESELS (ret)[i], CAR (required_values), NULL);
	}
    }
    if (i < VALUESNUM (ret)) {
	/* We have more return values than specific return types.
	 * Check them against the #rest value return type
	 */
	if (rest_values != NULL) {
	    for (; i < VALUESNUM (ret); i++) {
		if (!marlais_instance (VALUESELS (ret)[i],
			       rest_values)) {
		    marlais_error ("in value return: return value is not of correct type",
			   VALUESELS (ret)[i],
			   rest_values,
			   NULL);
		}
	    }
	} else {
	    /* Discard the extra values by ignoring them. */
	    VALUESNUM (ret) = i;
	}
    } else if (PAIRP (required_values)) {
	/* Add default values */
	for (j = 0; PAIRP (required_values); j++, required_values = CDR (required_values)) {
	    if (!marlais_instance (MARLAIS_FALSE, CAR (required_values))) {
		marlais_error ("in value return: default value doesn't match return type",
		       CAR (required_values),
		       NULL);
	    }
	}
	newret = marlais_allocate_object (Values, sizeof (struct values));

	VALUESNUM (newret) = i + j;
	VALUESELS (newret) = (Object *)
	    marlais_allocate_memory (VALUESNUM (newret) * sizeof (Object));

	for (i = 0; i < VALUESNUM (ret); i++) {
	    VALUESELS (newret)[i] = VALUESELS (ret)[i];
	}
	for (; i < VALUESNUM (newret); i++) {
	    VALUESELS (newret)[i] = MARLAIS_FALSE;
	}
	ret = newret;
    }
    /* turn stupid multiple value into single value */
    if (VALUESNUM (ret) == 1) {
	ret = VALUESELS (ret)[0];
    }
    ResultValueStack = CDR (ResultValueStack);
    return (ret);
}

#ifdef MARLAIS_ENABLE_METHOD_CACHING
static Object
get_specializers (Object gen, Object args)
/* Construct vector of classes of agruments */
{
    int length, i;
    Object result, tmp;

    tmp = args;
    length = list_length (function_specializers (gen));
    result = marlais_make_vector (length, NULL);
    for (i = 0; i < length; i++) {
	if (EMPTYLISTP (tmp)) {
	    marlais_error ("Missing Required Arguments", gen, args, NULL);
	}
	SOVELS (result)[i] = marlais_object_class (CAR (tmp));
	tmp = CDR (tmp);
    }
    return (result);
}

static Object
getCacheEntry (Object gen, Object args)
{
    Object arg_vec, cacheEntry;

    arg_vec = get_specializers (gen, args);
    cacheEntry = marlais_table_element_by_vector (GFCACHE (gen), arg_vec);
    return (cacheEntry);
}

static Object
add_method_cache (Object gen, Object args)
{
    Object arg_vec, new_item;

    arg_vec = get_specializers (gen, args);
    new_item = sorted_possible_method_handles (gen, args);
    marlais_table_element_setter_by_vector (GFCACHE (gen), arg_vec, new_item);
    return (new_item);
}

static Object
build_rest_methods (Object cache_tail, Object args)
    /* build the next methods list */
{
    Object method_found, method_group;

    if (EMPTYLISTP (cache_tail)) {
	return (make_empty_list ());
    }
    /* possible ambiguous point - put recalc signal here */
    method_found = 0;
    method_group = CAR (cache_tail);
    while (!EMPTYLISTP (method_group)) {
	if (method_found) {
	    return make_empty_list ();
	}
	method_found = HDLOBJ (CAR (method_group));
	/* Drew - check the next line.  I think you forgot to cdr down
	 * the method_group list
	 * jnw (5 June 1995)
	 */
	method_group = CDR (method_group);
    }
    if (!method_found) {
	return cons (MARLAIS_FALSE, make_empty_list ());
    } else {
	return cons (method_found, build_rest_methods (CDR (cache_tail), args));
    }
}
#endif

Object
apply_generic (Object gen, Object args)
{

#ifndef MARLAIS_ENABLE_METHOD_CACHING
    Object sorted_methods;
    Object methods;

#endif
    Object cacheEntry;
    Object currentGroup;
    Object method;
    Object rest_methods;

#ifdef MARLAIS_ENABLE_METHOD_CACHING
    /* try the cache first */
    cacheEntry = getCacheEntry (gen, args);
    if (!cacheEntry) {
	/* add the cache entry if it isn't there */
	cacheEntry = add_method_cache (gen, args);
    }
    method = NULL;
    /* find the first applicable method */
    while (!EMPTYLISTP (cacheEntry)) {
	currentGroup = CAR (cacheEntry);
	while (!EMPTYLISTP (currentGroup)) {
	    if (applicable_method_p (HDLOBJ (CAR (currentGroup)), args, 0)
		== MARLAIS_TRUE) {
		if (method) {
		    marlais_error ("Ambiguous methods in apply generic function", gen, args, NULL);
		} else {
		    method = HDLOBJ (CAR (currentGroup));
		}
	    }
	    currentGroup = CDR (currentGroup);
	}
	cacheEntry = CDR (cacheEntry);
	if (method)
	    break;
    }
    if (!method) {
	marlais_error ("No applicable methods", gen, args, NULL);
    }
    rest_methods = build_rest_methods (cacheEntry, args);
    return marlais_apply_method (method, args, rest_methods, gen);
#else
    methods = GFMETHODS (gen);
    sorted_methods = FIRSTVAL (sorted_applicable_methods (gen, args));
    if (EMPTYLISTP (sorted_methods)) {
	marlais_error ("Ambiguous methods in apply generic function", gen, args, NULL);
    } else {
	return marlais_apply_method (CAR (sorted_methods),
				     args,
				     CDR (sorted_methods),
				     gen);
    }
#endif
}

static Object
apply_exit (Object exit_proc, Object args)
{
    if (unwind_to_exit (exit_proc)) {
      /* XXX casts are wrong and must be fixed */
	switch (list_length (args)) {
	case 0:
	    longjmp (*EXITRET (exit_proc), (int) (unspecified_object));
	case 1:
	    longjmp (*EXITRET (exit_proc), (int) FIRST (args));
	default:
	    longjmp (*EXITRET (exit_proc), (int) (marlais_values (args)));
	}
    } else {
	return marlais_error ("No exit procedure binding -- returning", 0);
    }
}

static Object
apply_next_method (Object next_method, Object args)
{
    Object method, rest_methods, real_args;

    rest_methods = NMREST (next_method);
#ifdef MARLAIS_ENABLE_METHOD_CACHING
    method = NMMETH (next_method);
#else
    method = CAR (rest_methods);
    rest_methods = CDR (rest_methods);
#endif

    if (EMPTYLISTP (args)) {
	real_args = NMARGS (next_method);
    } else {
	real_args = args;
    }
    return marlais_apply_method (method, real_args, rest_methods, NMGF (next_method));
}

static Object
set_trace (Object flag)
{
    if (flag == MARLAIS_FALSE) {
	trace_functions = 0;
	trace_only_user_funs = 0;
    } else {
	trace_functions = 1;
	if (flag == user_keyword) {
	    trace_only_user_funs = 1;
	}
    }
    return (flag);
}

static void
devalue_args (Object args)
{
    while (!EMPTYLISTP (args)) {
	Object arg = CAR (args);

	if (VALUESP (arg)) {
	    if (VALUESNUM (arg) > 0) {
		CAR (args) = VALUESELS (arg)[0];
	    } else {
		marlais_error ("Null values construct used as an argument", NULL);
	    }
	}
	args = CDR (args);
    }
}
