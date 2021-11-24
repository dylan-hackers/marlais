/* eval.c -- see COPYRIGHT for use */

#include <marlais/eval.h>

#include <marlais/alloc.h>
#include <marlais/apply.h>
#include <marlais/env.h>
#include <marlais/list.h>
#include <marlais/number.h>
#include <marlais/print.h>
#include <marlais/stream.h>
#include <marlais/syntax.h>

struct eval_stack *eval_stack = 0;
jmp_buf *the_eval_context = NULL;
static Object the_eval_obj = NULL;

/* local function prototypes */
Object eval_combination (Object obj, int do_apply);

/* function definitions */

Object
marlais_eval (Object obj)
{
    Object val;

#ifdef MARLAIS_OBJECT_MODEL_SMALL
    if (INTEGERP (obj) || IMMEDP (obj)) {
        return (obj);
    }
#endif

#ifdef MARLAIS_OBJECT_MODEL_SMALL
    switch (POINTERTYPE (obj))
#else
    switch (object_type (obj))
#endif
    {
    case True:
    case False:
    case Integer:
#ifdef MARLAIS_ENABLE_BIG_INTEGERS
    case BigInteger:
#endif
    case Ratio:
    case SingleFloat:
    case DoubleFloat:
    case ByteString:
    case SimpleObjectVector:
    case Symbol:
    case Character:
    case EndOfFile:
    case EmptyList:
    case ForeignPtr:
    case UnspecifiedValue:
        return (obj);
    case Values:
        return marlais_error ("Trying to eval a values object (this is a bug)",
                              obj,
                              NULL);
    case Name:
        val = marlais_symbol_value (obj);
        if (!val) {
            return marlais_error ("unbound variable", obj, NULL);
        }
        return (val);
    case Pair:
        return (eval_combination (obj, 0));
    default:
        return marlais_error ("eval: do not know how to eval object", obj, NULL);
    }
}

Object
marlais_tail_eval (Object obj)
{

#ifdef MARLAIS_ENABLE_TAIL_CALL_OPTIMIZATION
    if (trace_functions) {
	marlais_warning ("in tail eval context, parent context",
		 eval_stack->context,
		 eval_stack->next->context,
		 0);
    }
    if (PAIRP (obj)) {
	the_eval_obj = obj;
	if (the_eval_context == NULL) {
	    marlais_error ("tail_eval called without a prior eval in progress.", NULL);
	}
	longjmp (*the_eval_context, 1);
    }
#endif
    /* if it's not a <pair>, then call good old eval. */
    return marlais_eval (obj);
}

/* <pcb> moved apply here to permit safe tail recursion. */

Object
marlais_apply (Object fun, Object args)
{
    return eval_combination (marlais_cons (fun, args), 1);
}

Object
eval_combination (Object obj, int do_apply)
{
    Object op;
    syntax_fun sf;
    Object fun, args, ret;
    struct frame *old_env;
    struct eval_stack *old_stack;
    jmp_buf *old_context;
    jmp_buf this_context;
    int is_tail_call = 0;
    Object tail_required_values;
    Object tail_rest_values;

    ResultValueStack = marlais_cons (marlais_default_result_value (), ResultValueStack);

    old_env = the_env;
    old_stack = eval_stack;

    /* save a place for tail_eval to longjmp to later. */
    old_context = the_eval_context;
    the_eval_context = &this_context;
    if (setjmp (this_context) != 0) {
	obj = the_eval_obj;

	eval_stack = old_stack;	/* restore the state of the "eval" stack. */

	is_tail_call = 1;	/* a tail call occurred. */
	do_apply = 0;		/* tail_eval called from apply. */
    }
    if (do_apply) {
	fun = CAR (obj);
	args = CDR (obj);
	marlais_push_eval_stack (fun);
	ret = marlais_apply_internal (fun, args);
	marlais_pop_eval_stack ();
    } else {
	op = CAR (obj);
	sf = marlais_syntax_function (op);
	if (sf) {
	    marlais_push_eval_stack (op);
	    ret = (*sf) (obj);
	    marlais_pop_eval_stack ();
	} else {
	    fun = marlais_eval (CAR (obj));
	    marlais_push_eval_stack (fun);
	    args = marlais_map1 (marlais_eval, CDR (obj));
	    ret = marlais_apply_internal (fun, args);
	    marlais_pop_eval_stack ();
	}
    }

    /* restore previous frame's context. */
    the_eval_context = old_context;

    /* here we restore the environment since is not restored via tail calls. */
    if (is_tail_call)
	the_env = old_env;

    tail_required_values = CAR (CAR (ResultValueStack));
    tail_rest_values = CDR (CAR (ResultValueStack));
    ResultValueStack = CDR (ResultValueStack);

    ret = marlais_construct_return_values (ret,
					   tail_required_values,
					   tail_rest_values);
    return ret;
}

void
marlais_pop_eval_stack (void)
{
    eval_stack = eval_stack->next;
}

void
marlais_push_eval_stack (Object obj)
{
    struct eval_stack *tmp =
    (struct eval_stack *) marlais_allocate_memory (sizeof (struct eval_stack));

    tmp->next = eval_stack;
    tmp->context = obj;
    tmp->frame = the_env;
    eval_stack = tmp;
}

Object
marlais_print_stack (void)
{
    struct eval_stack *entry;
    int i;

    for (i = 0, entry = eval_stack->next;
	 entry != NULL;
	 entry = entry->next, i++) {
	fprintf (stderr, "#%d ", i);
	marlais_print_object (marlais_standard_error, entry->context, 1);
	fprintf (stderr, "\n");
    }
    return MARLAIS_UNSPECIFIED;
}
