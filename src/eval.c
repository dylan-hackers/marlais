/* eval.c -- see COPYRIGHT for use */

#include "eval.h"

#include "alloc.h"
#include "apply.h"
#include "env.h"
#include "list.h"
#include "number.h"
#include "print.h"
#include "stream.h"
#include "syntax.h"

struct eval_stack *eval_stack = 0;

/* local function prototypes */
Object eval_combination (Object obj, int do_apply);

/* function definitions */

Object
eval (Object obj)
{
    Object val;

#ifdef SMALL_OBJECTS
    if (INTEGERP (obj) || IMMEDP (obj)) {
	return (obj);
    }
#endif

#ifdef SMALL_OBJECTS
    switch (POINTERTYPE (obj))
#else
    switch (object_type (obj))
#endif
    {
    case True:
    case False:
    case Integer:
#ifdef BIG_INTEGERS
    case BigInteger:
#endif
    case Ratio:
    case SingleFloat:
    case DoubleFloat:
    case ByteString:
    case SimpleObjectVector:
    case Keyword:
    case Character:
    case EndOfFile:
    case EmptyList:		/* is this right? */
    case ForeignPtr:		/* <pcb> */
	return (obj);
    case Values:
	if (obj == unspecified_object) {
	    return obj;
	} else {
	    return marlais_error ("Trying to eval a values object (this is a bug)",
			  obj,
			  NULL);
	}
    case Symbol:
	val = symbol_value (obj);
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

jmp_buf *the_eval_context = NULL;
static Object the_eval_obj = NULL;

extern struct frame *the_env;

Object
tail_eval (Object obj)
{

#ifdef OPTIMIZE_TAIL_CALLS
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
    return eval (obj);
}

/* <pcb> moved apply here to permit safe tail recursion. */

Object
apply (Object fun, Object args)
{
    return eval_combination (cons (fun, args), 1);
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

    ResultValueStack = cons (default_result_value (), ResultValueStack);

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
	push_eval_stack (fun);
	ret = apply_internal (fun, args);
	pop_eval_stack ();
    } else {
	op = CAR (obj);
	sf = marlais_syntax_function (op);
	if (sf) {
	    push_eval_stack (op);
	    ret = (*sf) (obj);
	    pop_eval_stack ();
	} else {
	    fun = eval (CAR (obj));
	    push_eval_stack (fun);
	    args = map (eval, CDR (obj));
	    ret = apply_internal (fun, args);
	    pop_eval_stack ();
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

    ret = construct_return_values (ret,
				   tail_required_values,
				   tail_rest_values);
    return ret;
}

void
pop_eval_stack (void)
{
    eval_stack = eval_stack->next;
}

void
push_eval_stack (Object obj)
{
    struct eval_stack *tmp =
    (struct eval_stack *) marlais_allocate_memory (sizeof (struct eval_stack));

    tmp->next = eval_stack;
    tmp->context = obj;
    tmp->frame = the_env;
    eval_stack = tmp;
}

Object
print_stack (void)
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
    return unspecified_object;
}
