/* print.c -- see COPYRIGHT for use */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "print.h"

#include "apply.h"
#include "character.h"
#include "error.h"
#include "eval.h"
#include "list.h"
#include "prim.h"
#include "slot.h"

#ifdef BIG_INTEGERS
#include "biginteger.h"
#endif

extern Object print_symbol;
extern Object princ_symbol;
extern Object standard_output_stream;
extern Object standard_error_stream;

/* local function prototypes */

static Object print_obj_escaped (Object stream, Object obj);
static void print_pair (Object stream, Object pair, int escaped);
static void print_character (Object stream, Object c, int escaped);
static void print_vector (Object stream, Object vec, int escaped);
static void print_values (Object stream, Object vals, int escaped);
static void print_string (Object stream, Object str, int escaped);
static void print_instance (Object stream, Object inst, int escaped);
static void print_generic_function (Object stream, Object gf, int escaped);
static void print_method (Object stream, Object method, int escaped);
static void print_slot_descriptor (Object stream, Object slotd, int escaped);
static void print_class (Object stream, Object class, int escaped);
static void print_array (Object stream, Object array, int escaped);
static void print_stream (Object out_stream, Object stream);
static void print_type_name (Object stream, Object class, int escaped);

/* primitives */

static struct primitive print_prims[] =
{
    {"%print", prim_2, print_obj},
    {"%princ", prim_2, print_obj_escaped},
    {"%format", prim_3, format},
};

/* function definitions */

Object generic_function_methods (Object gen);

static int DebugPrint = 0;

static void
apply_print (Object stream, Object obj, int escaped)
{
    if (DebugPrint || trace_functions) {
	print_obj (stream, obj);
    } else {
	if (0 == escaped) {
	    apply (eval (princ_symbol),
		   listem (stream, obj, NULL));
	} else {
	    apply (eval (print_symbol),
		   listem (obj, stream, NULL));
	}
    }
}

/*
 * pr (obj)
 *
 * Use this when debugging to print an object.  Example:
 *
 *      (gdb) print pr(cache_tail)
 *      #(#({object handle {method foo (obj)1}}))
 *      $11 = (void *) 0xa2d50
 *      (gdb)
 *
 * This helps make debugging almost tolerable.
 */
void
pr (Object obj)
{
    if (obj == 0) {
	fprintf (stderr, "*NULL POINTER*\n");
    } else {
	DebugPrint++;
	print_object (standard_error_stream, obj, 1);
	fprintf (stderr, "\n");
	fflush (stderr);
	DebugPrint--;
    }
}

void
init_print_prims (void)
{
  int num = sizeof (print_prims) / sizeof (struct primitive);
  init_prims (num, print_prims);
}

void
print_object (Object stream, Object obj, int escaped)
{
    FILE *fp;

    if (stream == true_object) {
	fp = stdout;
	stream = standard_output_stream;
    } else if (OUTPUTSTREAMP (stream)) {
	fp = STREAMFP (stream);
    } else {
	error ("print_object: cannot send output to non-stream", stream, NULL);
    }

    switch (TYPE (obj)) {
    case True:
	fprintf (fp, "#t");
	break;
    case False:
	fprintf (fp, "#f");
	break;
    case EmptyList:
	fprintf (fp, "#()");
	break;
    case Integer:
	fprintf (fp, "%d", INTVAL (obj));
	break;
#ifdef BIG_INTEGERS
    case BigInteger:
	print_big_integer (fp, obj);
	break;
#endif
    case Ratio:
	fprintf (fp, "%d/%d", RATIONUM (obj), RATIODEN (obj));
	break;
    case DoubleFloat:
	fprintf (fp, "%f", DFLOATVAL (obj));
	break;
    case Symbol:
	fprintf (fp, "%s", SYMBOLNAME (obj));
	break;
    case Keyword:
	{
	    int i;
	    char *cp;

	    fprintf (fp, "#\"");
	    cp = KEYNAME (obj);
	    for (i = strlen (cp) - 1; i > 0; i--) {
		fputc (*(cp++), fp);
	    }
	    fputc ('"', fp);
	}
	break;
    case Pair:
	print_pair (stream, obj, escaped);
	break;
    case Character:
	print_character (stream, obj, escaped);
	break;
    case SimpleObjectVector:
	print_vector (stream, obj, escaped);
	break;
    case ByteString:
	print_string (stream, obj, escaped);
	break;
    case ObjectTable:
	fprintf (fp, "{table}");
	break;
    case Deque:
	fprintf (fp, "{deque}");
	break;
    case Array:
	print_array (stream, obj, escaped);
	break;
    case Primitive:
	fprintf (fp, "{primitive function %s}", PRIMNAME (obj));
	break;
    case GenericFunction:
	print_generic_function (stream, obj, escaped);
	break;
    case Method:
	print_method (stream, obj, escaped);
	break;
    case NextMethod:
	fprintf (fp, "next-method()");
	break;
    case Class:
	print_class (stream, obj, escaped);
	break;
    case Instance:
	print_instance (stream, obj, escaped);
	break;
    case Singleton:
	fprintf (fp, "{the singleton ");
	print_object (stream, SINGLEVAL (obj), escaped);
	fprintf (fp, "}");
	break;
    case LimitedIntType:
	fprintf (fp, "{limited <integer>");
	if (LIMINTHASMIN (obj)) {
	    fprintf (fp, " min: %d", LIMINTMIN (obj));
	}
	if (LIMINTHASMAX (obj)) {
	    fprintf (fp, " max: %d", LIMINTMAX (obj));
	}
	fprintf (fp, "}");
	break;
    case UnionType:
	fprintf (fp, "{union");
	{
	    Object ptr;

	    for (ptr = UNIONLIST (obj); PAIRP (ptr); ptr = CDR (ptr)) {
		fprintf (fp, " ");
		print_object (stream, CAR (ptr), escaped);
	    }
	    fprintf (fp, "}");
	}
	break;
    case SlotDescriptor:
	print_slot_descriptor (stream, obj, escaped);
	break;
    case EndOfFile:
	fprintf (fp, "{end of file}");
	break;
    case Values:
	print_values (stream, obj, escaped);
	break;
    case Unspecified:
	break;
    case Exit:
	fprintf (fp, "{exit procedure}");
	break;
    case Unwind:
	fprintf (fp, "{unwind protect}");
	break;
    case Stream:
	print_stream (stream, obj);
	break;
    case TableEntry:
	fprintf (fp, "{table entry}");
	break;
    case UninitializedSlotValue:
	fprintf (fp, "{uninitialized slot value}");
	break;
    case DequeEntry:
	fprintf (fp, "{deque entry ");
	print_object (stream, DEVALUE (obj), escaped);
	fprintf (fp, "}");
	break;
    case ObjectHandle:
	fprintf (fp, "{object handle ");
	print_object (stream, HDLOBJ (obj), escaped);
	fprintf (fp, "}");
	break;
    case ForeignPtr:		/* <pcb> my foreign pointer type. */
	fprintf (fp, "{foreign pointer 0x%08x}", (int) FOREIGNPTR (obj));
	break;
    case Environment:
	fprintf (fp, "{environment object ");
	print_env (ENVIRONMENT (obj));
	fprintf (fp, "}");
	break;
    default:
	error ("print: unknown object type", NULL);

    }
}

Object
print_obj (Object stream, Object obj)
{
    print_object (stream, obj, 1);
#if 0
    if (TYPE (obj) != Values || VALUESNUM (obj)) {
	printf ("\n");
    }
#endif
    return (unspecified_object);
}

static Object
print_obj_escaped (Object stream, Object obj)
{
    print_object (stream, obj, 0);
#if 0
    if (TYPE (obj) != Values || VALUESNUM (obj)) {
	printf ("\n");
    }
#endif
    return (unspecified_object);
}

void
print_err (Object obj)
{
    apply_print (standard_error_stream, obj, 1);
    fflush (stderr);
}

static void
print_pair (Object stream, Object pair, int escaped)
{
    Object cdr;
    FILE *fp = STREAMFP (stream);

    fprintf (fp, "#(");
    apply_print (stream, CAR (pair), escaped);
    cdr = CDR (pair);
    while (PAIRP (cdr)) {
	fprintf (fp, ", ");
	apply_print (stream, CAR (cdr), escaped);
	cdr = CDR (cdr);
    }
    if (!EMPTYLISTP (cdr)) {
	fprintf (fp, " . ");
	apply_print (stream, cdr, escaped);
    }
    fprintf (fp, ")");
}

static void
print_character (Object stream, Object c, int escaped)
{
    char ch;
    FILE *fp = STREAMFP (stream);

    ch = CHARVAL (c);
    if (escaped) {
	    switch (ch) {
	    case '\b':
		fprintf (fp, "'\\b'");
		break;
	    case '\f':
		fprintf (fp, "'\\f'");
		break;
	    case '\n':
		fprintf (fp, "'\\n'");
		break;
	    case '\r':
		fprintf (fp, "'\\r'");
		break;
	    case '\t':
		fprintf (fp, "'\\r'");
		break;
	    default:
		fprintf (fp, "'%c'", ch);
		break;
	    }
    } else {
	fprintf (fp, "%c", ch);
    }
}

static void
print_vector (Object stream, Object vec, int escaped)
{
    int i;
    FILE *fp = STREAMFP (stream);

    fprintf (fp, "#[");
    for (i = 0; i < SOVSIZE (vec); ++i) {
	apply_print (stream, SOVELS (vec)[i], escaped);
	if (i < (SOVSIZE (vec) - 1)) {
	    fprintf (fp, ", ");
	}
    }
    fprintf (fp, "]");
}

static void
print_slot_values (Object stream, Object instance, Object slotds, int escaped)
{
    int i;
    FILE *fp = STREAMFP (stream);

    if (EMPTYLISTP (slotds))
	return;

    for (i = 0;
	 PAIRP (slotds);
	 i++, slotds = CDR (slotds)) {
	fprintf (fp, ", ");
	print_object (stream, GFNAME (SLOTDGETTER (CAR (slotds))), escaped);
	fprintf (fp, " = ");
	apply_print (stream, CAR (INSTSLOTS (instance)[i]), escaped);
    }
}

static void
print_constant_slot_values (Object stream, Object const_slotds, int escaped)
{
    Object slotd;
    int i;
    FILE *fp = STREAMFP (stream);

    if (EMPTYLISTP (const_slotds))
	return;

    for (i = 0;
	 PAIRP (const_slotds);
	 i++, const_slotds = CDR (const_slotds)) {
	slotd = CAR (const_slotds);
	fprintf (fp, ", ");
	print_object (stream, SLOTDGETTER (slotd), escaped);
	fprintf (fp, " = ");
	apply_print (stream, SLOTDINIT (slotd), escaped);
    }
}

#if 0

static void
print_virtual_slot_values (Object stream, Object instance, Object slotds,
			   int escaped)
{
    Object slotd;
    FILE *fp = STREAMFP (stream);

    if (EMPTYLISTP (slotds))
	return;

    for (slotd = CAR (slotds);
	 !EMPTYLISTP (slotds);
	 slotds = CDR (slotds)) {
	fprintf (fp, ", ");
	print_object (stream, SLOTDGETTER (slotd), escaped);
	fprintf (fp, " = ");
	apply_print (stream, apply_internal (SLOTDGETTER (slotd),
				       cons (instance, make_empty_list ())),
		     escaped);
    }
}

#endif

static void
print_class_slot_values (Object stream, Object class, int escaped, int first)
{
    Object supers;

    print_slot_values (stream, CLASSCSLOTS (class),
		       (first ? append (CLASSCSLOTDS (class),
					CLASSESSLOTDS (class))
			: CLASSCSLOTDS (class)),
		       escaped);

    for (supers = CLASSSUPERS (class);
	 PAIRP (supers);
	 supers = CDR (supers)) {
	print_class_slot_values (stream, CAR (supers), escaped, 0);
    }
}

static void
print_instance (Object stream, Object inst, int escaped)
{
    Object class, instslotds;
    FILE *fp = STREAMFP (stream);

    fprintf (fp, "{instance of class %s",
	     SYMBOLNAME (CLASSNAME (INSTCLASS (inst))));
    class = INSTCLASS (inst);

    instslotds = append (CLASSINSLOTDS (class), CLASSSLOTDS (class));
    print_slot_values (stream, inst, instslotds, escaped);
/*
 *    print_virtual_slot_values (stream, inst, CLASSVSLOTDS (class), escaped);
 */
    print_class_slot_values (stream, class, escaped, 1);
    print_constant_slot_values (stream, CLASSCONSTSLOTDS (class), escaped);
    fprintf (fp, "}");
}

static void
print_values (Object stream, Object vals, int escaped)
{
    int i, num;
    FILE *fp = STREAMFP (stream);

    num = VALUESNUM (vals);
/*  fprintf (fp, "#<"); */
    for (i = 0; i < num; ++i) {
	apply_print (stream, VALUESELS (vals)[i], escaped);
	if (i < (num - 1)) {
	    fprintf (fp, "\n");
	}
    }
/*  fprintf (fp, ">"); */
}

static void
print_param (Object stream, Object param, int escaped)
{
    FILE *fp = STREAMFP (stream);

    if (SECOND (param) != object_class
    /* || CAR (param) == unspecified_object */
	) {
	print_object (stream, CAR (param), escaped);
	fprintf (fp, " :: ");
	print_type_name (stream, SECOND (param), escaped);
    } else {
	print_object (stream, CAR (param), escaped);
    }

}
static void
print_param_list (Object stream, Object params, int escaped)
{
    FILE *fp = STREAMFP (stream);

    if (PAIRP (params)) {
	print_param (stream, CAR (params), escaped);
	params = CDR (params);
	while (PAIRP (params)) {
	    fprintf (fp, ", ");
	    print_param (stream, CAR (params), escaped);
	    params = CDR (params);
	}
    }
}

static void
print_unparenthesized_list (Object stream, Object pair, int escaped)
{
    FILE *fp = STREAMFP (stream);

    if (PAIRP (pair)) {
	print_object (stream, CAR (pair), escaped);
	pair = CDR (pair);
	while (PAIRP (pair)) {
	    fprintf (fp, " ");
	    print_object (stream, CAR (pair), escaped);
	    pair = CDR (pair);
	}
    }
}

static void
print_generic_function (Object stream, Object gf, int escaped)
{
    int some_args_printed = 0;
    FILE *fp = STREAMFP (stream);

    if (SYMBOLP (GFNAME (gf))) {
	fprintf (fp, "{the generic function %s (", SYMBOLNAME (GFNAME (gf)));
    } else {
	fprintf (fp, "{an anonymous generic function (");
    }

    if (PAIRP (GFREQPARAMS (gf))) {
	print_param_list (stream, GFREQPARAMS (gf), escaped);
	some_args_printed = 1;
    }
    if (GFRESTPARAM (gf)) {
	if (some_args_printed) {
	    fprintf (fp, ", #rest %s", SYMBOLNAME (GFRESTPARAM (gf)));
	} else {
	    fprintf (fp, "#rest %s", SYMBOLNAME (GFRESTPARAM (gf)));
	}
	some_args_printed = 1;
    }
    if (PAIRP (GFKEYPARAMS (gf))) {
	if (some_args_printed) {
	    fprintf (fp, ", #key ");
	} else {
	    fprintf (fp, "#key ");
	}
	print_unparenthesized_list (stream, GFKEYPARAMS (gf), escaped);
	if (GFALLKEYS (gf)) {
	    fprintf (fp, " #, all-keys");
	}
    }
    fprintf (fp, ")}");
}

static void
print_method (Object stream, Object method, int escaped)
{
    int some_args_printed = 0;
    FILE *fp = STREAMFP (stream);

    if (METHNAME (method)) {
	fprintf (fp, "{method %s (", SYMBOLNAME (METHNAME (method)));
    } else {
	fprintf (fp, "{an anonymous method (");
    }
    if (PAIRP (METHREQPARAMS (method))) {
	print_param_list (stream, METHREQPARAMS (method), escaped);
	some_args_printed = 1;
    }
    if (METHRESTPARAM (method)) {
	if (some_args_printed) {
	    fprintf (fp, ", #rest %s", SYMBOLNAME (METHRESTPARAM (method)));
	} else {
	    fprintf (fp, "#rest %s", SYMBOLNAME (METHRESTPARAM (method)));
	}
	some_args_printed = 1;
    }
    if (PAIRP (METHKEYPARAMS (method)) || METHALLKEYS (method)) {
	if (some_args_printed) {
	    fprintf (fp, ", #key ");
	} else {
	    fprintf (fp, "#key ");
	}
	print_unparenthesized_list (stream, METHKEYPARAMS (method), escaped);
	if (METHALLKEYS (method)) {
	    fprintf (fp, ", #all-keys");
	}
    }
    fprintf (fp, ")");

#if 1
    print_unparenthesized_list (stream, METHBODY (method), escaped);
#endif

    fprintf (fp, "}");
}

static void
print_class (Object stream, Object class, int escaped)
{
    FILE *fp = STREAMFP (stream);

    if (!SYMBOLNAME (CLASSNAME (class))) {
	fprintf (fp, "{an anonymous class");
    } else {
	fprintf (fp, "{the class %s", SYMBOLNAME (CLASSNAME (class)));
    }

    fprintf (fp, " (%d)", CLASSINDEX (class));
    /*
       fprintf (fp, " (");
       print_unparenthesized_list (stream, CLASSSUPERS (class), escaped);
       fprintf (fp, ")");

       print_slot_values (stream, CLASSCSLOTS(class), append (CLASSCSLOTDS (class),
       CLASSESSLOTDS (class)),
       escaped);
     */

    fprintf (fp, "}");

}

static void
print_slot_descriptor (Object stream, Object slotd, int escaped)
{
    FILE *fp = STREAMFP (stream);

    fprintf (fp, "{slot descriptor ");
    print_object (stream, SLOTDGETTER (slotd), escaped);
    if (SLOTDALLOCATION (slotd) != instance_symbol) {
	fprintf (fp, " allocation: ");
	print_object (stream, SLOTDALLOCATION (slotd), escaped);
    }
    if (SLOTDSETTER (slotd)) {
	fprintf (fp, " setter: ");
	print_object (stream, SLOTDSETTER (slotd), escaped);
    }
    if (SLOTDSLOTTYPE (slotd) != object_class) {
	if (SLOTDDEFERREDTYPE (slotd)) {
	    fprintf (fp, " deferred-type: ");
	} else {
	    fprintf (fp, " type: ");
	}
	print_object (stream, SLOTDSLOTTYPE (slotd), escaped);
    }
    if (SLOTDINIT (slotd) != uninit_slot_object) {
	if (SLOTDINITFUNCTION (slotd)) {
	    fprintf (fp, " init-function: ");
	} else {
	    fprintf (fp, " init: ");
	}
	print_object (stream, SLOTDINIT (slotd), escaped);
    }
    if (SLOTDINITKEYWORD (slotd)) {
	if (SLOTDKEYREQ (slotd)) {
	    fprintf (fp, " required-init-keyword: ");
	} else {
	    fprintf (fp, " init-keyword: ");
	}
	print_object (stream, SLOTDINITKEYWORD (slotd), escaped);
    }
    fprintf (fp, "}");
}

#if 0
static void
print_array (Object stream, Object array, int escaped)
{
    FILE *fp = STREAMFP (stream);

    fprintf (fp, "{array ");
    print_object (stream, ARRDIMS (array), escaped);
    fprintf (fp, "}");
}

#endif

static int cur_el;
static void print_array_help (Object stream, Object dims, Object *els, int escaped);

static void
print_array (Object stream, Object array, int escaped)
{
    Object dims, *els;
    FILE *fp = STREAMFP (stream);

    dims = ARRDIMS (array);
    els = ARRELS (array);

    cur_el = 0;
    fprintf (fp, "#%da", list_length (dims));
    print_array_help (stream, dims, els, escaped);
}

static void
print_array_help (Object stream, Object dims, Object *els, int escaped)
{
    int dim_val, i;
    FILE *fp = STREAMFP (stream);

    fprintf (fp, "#(");
    if (EMPTYLISTP (dims)) {
	apply_print (stream, els[cur_el++], escaped);
	return;
    }
    dim_val = INTVAL (CAR (dims));
    if (EMPTYLISTP (CDR (dims))) {
	for (i = 0; i < dim_val; ++i) {
	    apply_print (stream, els[cur_el++], escaped);
	    if (i < (dim_val - 1)) {
		fprintf (fp, " ");
	    }
	}
    } else {
	for (i = 0; i < dim_val; ++i) {
	    print_array_help (stream, CDR (dims), els, escaped);
	}
    }
    fprintf (fp, ")");
}



#if 0
static void
print_array (Object stream, Object array, int escaped)
{
    Object dims;
    unsigned int dim_val, offset, i;
    int rank;
    FILE *fp = STREAMFP (stream);

    dims = ARRDIMS (array);
    rank = list_length (dims);
    fprintf (fp, "#%da", rank);
    offset = 0;
    while (!EMPTYLISTP (dims)) {
	fprintf (fp, "(");
	if (EMPTYLISTP (CDR (dims))) {
	    dim_val = INTVAL (CAR (dims));
	    for (i = 0; i < dim_val; ++i) {
		apply_print (stream, ARRELS (array)[offset], escaped);
		offset++;
	    }
	}
	dims = CDR (dims);
    }

    while (!EMPTYLISTP (dims)) {
	fprintf (fp, "(");
	if (EMPTYLISTP (CDR (dims))) {
	    dim_val = INTVAL (CAR (dims));
	    for (i = 0; i < dim_val; ++i) {
		apply_print (stream, ARRELS (array)[offset], escaped);
		offset++;
	    }
	}
	dims = CDR (dims);
    }
}
#endif

static void
print_string (Object stream, Object str, int escaped)
{
    FILE *fp = STREAMFP (stream);

    if (escaped) {
	fprintf (fp, "\"%s\"", BYTESTRVAL (str));
    } else {
	fprintf (fp, "%s", BYTESTRVAL (str));
    }
}

Object
format (Object stream, Object str, Object rest)
{
    Object obj;
    FILE *fp;
    char *cstr;
    int i;

/** DMA -- tightening requirements on format
    * if (stream == true_object) {
	* fp = stdout;
	* stream = standard_output_stream;
    } else ***/
    if (OUTPUTSTREAMP (stream)) {
	fp = STREAMFP (stream);
    } else {
	error ("format: cannot send output to non-stream", stream, NULL);
    }
    cstr = BYTESTRVAL (str);

    i = 0;
    while (cstr[i]) {
	if (cstr[i] == '%') {
	    i++;
	    switch (cstr[i]) {
	    case '=':
		if (EMPTYLISTP (rest)) {
		    error ("format: not enough args for format string", str, NULL);
		}
		obj = CAR (rest);
		rest = CDR (rest);
		apply_print (stream, obj, 0);
		break;

	    case 'd':
	    case 'D':
		if (EMPTYLISTP (rest)) {
		    error ("format: not enough args for format string", str, NULL);
		}
		obj = CAR (rest);
		if (!INTEGERP (obj)) {
		    error ("format: argument to %d must be an integer", obj, NULL);
		}
		rest = CDR (rest);
#if 0
		if (isdigit (cstr[i - 1])) {
		    j = i - 1;
		    while (isdigit (cstr[j])) {
			j--;
		    }
		    j++;
		    sscanf (cstr[j], "%d", &arg);
		    fprintf (fp, "%");
		} else
#endif
		{
		    fprintf (fp, "%d", INTVAL (obj));
		}
		break;
	    case 'b':
	    case 'B':
		if (EMPTYLISTP (rest)) {
		    error ("format: not enough args for format string", str, NULL);
		}
		obj = CAR (rest);
		if (!INTEGERP (obj)) {
		    error ("format: argument to %b must be an integer", obj, NULL);
		}
		rest = CDR (rest);
		{
		    int val = INTVAL (obj);

		    while (val > 0)
			val <<= 1;
		    do
			fputc (val < 0 ? '1' : '0', fp);
		    while ((val <<= 1) != 0);
		}
		break;

	    case 'o':
	    case 'O':
		if (EMPTYLISTP (rest)) {
		    error ("format: not enough args for format string", str, NULL);
		}
		obj = CAR (rest);
		if (!INTEGERP (obj)) {
		    error ("format: argument to %o must be an integer", obj, NULL);
		}
		rest = CDR (rest);
		fprintf (fp, "%o", INTVAL (obj));
		break;

	    case 'x':
	    case 'X':
		if (EMPTYLISTP (rest)) {
		    error ("format: not enough args for format string", str, NULL);
		}
		obj = CAR (rest);
		if (!INTEGERP (obj)) {
		    error ("format: argument to %x must be an integer", obj, NULL);
		}
		rest = CDR (rest);
		fprintf (fp, "%x", INTVAL (obj));
		break;

	    case 'c':
	    case 'C':
		if (EMPTYLISTP (rest)) {
		    error ("format: not enough args for format string", str, NULL);
		}
		obj = CAR (rest);
		if (!CHARP (obj)) {
		    error ("format: argument to %c must be an integer", obj, NULL);
		}
		rest = CDR (rest);
		apply_print (stream, obj, 0);
		break;

	    case 's':
	    case 'S':
		if (EMPTYLISTP (rest)) {
		    error ("format: not enough args for format string", str, NULL);
		}
		obj = CAR (rest);
		rest = CDR (rest);
		apply_print (stream, obj, 0);
		break;

	    case '%':
		fprintf (fp, "%%");
		break;

	    default:
		/* skip over digits.  individuals branches
		   handle there own arguments. */
		if (isdigit (cstr[i])) {
		    while (isdigit (cstr[i])) {
			i++;
		    }
		    break;
		}
		error ("format: bad escape character", make_character (cstr[i]), NULL);
	    }
	} else {
	    fprintf (fp, "%c", cstr[i]);
	}
	i++;
    }
    if (!EMPTYLISTP (rest)) {
	error ("format: too many arguments for format string", CAR (rest), NULL);
    }
    return (unspecified_object);
}

static void
print_stream (Object out_stream, Object stream)
{
    FILE *fp = STREAMFP (out_stream);

    switch (STREAMSTYPE (stream)) {
    case Input:
	fprintf (fp, "{input stream}");
	break;
    case Output:
	fprintf (fp, "{output stream}");
	break;
    default:
	error ("trying to print stream of unknown type", NULL);
    }
}

static void
print_type_name (Object stream, Object obj, int escaped)
{
    FILE *fp = STREAMFP (stream);

    switch (TYPE (obj)) {
    case Class:
	fprintf (fp, "%s", SYMBOLNAME (CLASSNAME (obj)));
	break;
    case LimitedIntType:
    case UnionType:
    case Singleton:
	print_object (stream, obj, escaped);
	break;
    default:
	error ("print_type_name: object is not a type", obj);
    }
}
