/* print.c -- see COPYRIGHT for use */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "print.h"
#include "stream.h"

#include "apply.h"
#include "character.h"
#include "error.h"
#include "eval.h"
#include "globals.h"
#include "list.h"
#include "number.h"
#include "prim.h"
#include "slot.h"

#ifdef BIG_INTEGERS
#include "biginteger.h"
#endif

extern Object print_symbol;
extern Object princ_symbol;

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
};

/* function definitions */

Object generic_function_methods (Object gen);

static int DebugPrint = 0;
static FILE* file_from_fd(Object);

static void
apply_print (Object fd, Object obj, int escaped)
{
    if (DebugPrint || trace_functions) {
	print_obj (fd, obj);
    } else {
      Object stream;
      file_from_fd(fd);
      if(INTVAL(fd) == 1) {
        stream = standard_output_stream;
      } else {
        stream = standard_error_stream;
      }
	if (0 == escaped) {
	    apply (eval (princ_symbol),
		   listem (eval(stream), obj, NULL));
	} else {
	    apply (eval (print_symbol),
		   listem (obj, eval(stream), NULL));
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
	print_object (make_integer(STDERR), obj, 1);
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

FILE* file_from_fd(Object fd)
{
    switch(INTVAL(fd)) {
      case 0:
	error ("print_object: cannot send output to input-stream", fd, NULL);
        break;
      case 1:
        return stdout;
      case 2:
        return stderr;
      default:
        error("Don't handle printing objects to >2 fds yet!", fd, NULL);
    }
	return NULL;
}

void
print_object (Object fd, Object obj, int escaped)
{
    FILE* fp = file_from_fd(fd);

    switch (object_type (obj)) {
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
	print_pair (fd, obj, escaped);
	break;
    case Character:
	print_character (fd, obj, escaped);
	break;
    case SimpleObjectVector:
	print_vector (fd, obj, escaped);
	break;
    case ByteString:
	print_string (fd, obj, escaped);
	break;
    case ObjectTable:
	fprintf (fp, "{table}");
	break;
    case Deque:
	fprintf (fp, "{deque}");
	break;
    case Array:
	print_array (fd, obj, escaped);
	break;
    case Primitive:
	fprintf (fp, "{primitive function %s}", PRIMNAME (obj));
	break;
    case GenericFunction:
	print_generic_function (fd, obj, escaped);
	break;
    case Method:
	print_method (fd, obj, escaped);
	break;
    case NextMethod:
	fprintf (fp, "next-method()");
	break;
    case Class:
	print_class (fd, obj, escaped);
	break;
    case Instance:
	print_instance (fd, obj, escaped);
	break;
    case Singleton:
	fprintf (fp, "{the singleton ");
	print_object (fd, SINGLEVAL (obj), escaped);
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
		print_object (fd, CAR (ptr), escaped);
	    }
	    fprintf (fp, "}");
	}
	break;
    case SlotDescriptor:
	print_slot_descriptor (fd, obj, escaped);
	break;
    case EndOfFile:
	fprintf (fp, "{end of file}");
	break;
    case Values:
	print_values (fd, obj, escaped);
	break;
    case Unspecified:
	break;
    case Exit:
	fprintf (fp, "{exit procedure}");
	break;
    case Unwind:
	fprintf (fp, "{unwind protect}");
	break;
    case TableEntry:
	fprintf (fp, "{table entry}");
	break;
    case UninitializedSlotValue:
	fprintf (fp, "{uninitialized slot value}");
	break;
    case DequeEntry:
	fprintf (fp, "{deque entry ");
	print_object (fd, DEVALUE (obj), escaped);
	fprintf (fp, "}");
	break;
    case ObjectHandle:
	fprintf (fp, "{object handle ");
	print_object (fd, HDLOBJ (obj), escaped);
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
print_obj (Object fd, Object obj)
{
    print_object (fd, obj, 1);
#if 0
    if (object_type (obj) != Values || VALUESNUM (obj)) {
	printf ("\n");
    }
#endif
    return (unspecified_object);
}

static Object
print_obj_escaped (Object fd, Object obj)
{
    print_object (fd, obj, 0);
#if 0
    if (object_type (obj) != Values || VALUESNUM (obj)) {
	printf ("\n");
    }
#endif
    return (unspecified_object);
}

void
print_err (Object obj)
{
    apply_print (make_integer(STDERR), obj, 1);
    fflush (stderr);
}

static void
print_pair (Object fd, Object pair, int escaped)
{
    Object cdr;
    FILE *fp = file_from_fd(fd);

    fprintf (fp, "#(");
    apply_print (fd, CAR (pair), escaped);
    cdr = CDR (pair);
    while (PAIRP (cdr)) {
	fprintf (fp, ", ");
	apply_print (fd, CAR (cdr), escaped);
	cdr = CDR (cdr);
    }
    if (!EMPTYLISTP (cdr)) {
	fprintf (fp, " . ");
	apply_print (fd, cdr, escaped);
    }
    fprintf (fp, ")");
}

static void
print_character (Object fd, Object c, int escaped)
{
    char ch;
    FILE *fp = file_from_fd(fd);

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
print_vector (Object fd, Object vec, int escaped)
{
    int i;
    FILE *fp = file_from_fd(fd);

    fprintf (fp, "#[");
    for (i = 0; i < SOVSIZE (vec); ++i) {
	apply_print (fd, SOVELS (vec)[i], escaped);
	if (i < (SOVSIZE (vec) - 1)) {
	    fprintf (fp, ", ");
	}
    }
    fprintf (fp, "]");
}

static void
print_slot_values (Object fd, Object instance, Object slotds, int escaped)
{
    int i;
    FILE *fp = file_from_fd(fd);

    if (EMPTYLISTP (slotds))
	return;

    for (i = 0;
	 PAIRP (slotds);
	 i++, slotds = CDR (slotds)) {
	fprintf (fp, ", ");
	print_object (fd, GFNAME (SLOTDGETTER (CAR (slotds))), escaped);
	fprintf (fp, " = ");
	apply_print (fd, CAR (INSTSLOTS (instance)[i]), escaped);
    }
}

static void
print_constant_slot_values (Object fd, Object const_slotds, int escaped)
{
    Object slotd;
    int i;
    FILE *fp = file_from_fd(fd);

    if (EMPTYLISTP (const_slotds))
	return;

    for (i = 0;
	 PAIRP (const_slotds);
	 i++, const_slotds = CDR (const_slotds)) {
	slotd = CAR (const_slotds);
	fprintf (fp, ", ");
	print_object (fd, SLOTDGETTER (slotd), escaped);
	fprintf (fp, " = ");
	apply_print (fd, SLOTDINIT (slotd), escaped);
    }
}

#if 0
/* I suppose we don't want to print virtual slots? -- dma */
static void
print_virtual_slot_values (Object fd, Object instance, Object slotds,
			   int escaped)
{
    Object slotd;
    FILE *fp = file_from_fd(fd);

    if (EMPTYLISTP (slotds))
	return;

    for (slotd = CAR (slotds);
	 !EMPTYLISTP (slotds);
	 slotds = CDR (slotds)) {
	fprintf (fp, ", ");
	print_object (fd, SLOTDGETTER (slotd), escaped);
	fprintf (fp, " = ");
	apply_print (fd, apply_internal (SLOTDGETTER (slotd),
				       cons (instance, make_empty_list ())),
		     escaped);
    }
}

#endif

static void
print_class_slot_values (Object fd, Object class, int escaped, int first)
{
    Object supers;

    print_slot_values (fd, CLASSCSLOTS (class),
		       (first ? append (CLASSCSLOTDS (class),
					CLASSESSLOTDS (class))
			: CLASSCSLOTDS (class)),
		       escaped);

    for (supers = CLASSSUPERS (class);
	 PAIRP (supers);
	 supers = CDR (supers)) {
	print_class_slot_values (fd, CAR (supers), escaped, 0);
    }
}

/* changing this to be like other Dylan implementations -- if you
 * want slot values, write a print-object method on the class you're
 * interested in -- dma */
static void
print_instance (Object fd, Object inst, int escaped)
{
    Object class;
    FILE *fp = file_from_fd(fd);

    fprintf (fp, "{instance of class %s",
	     SYMBOLNAME (CLASSNAME (INSTCLASS (inst))));
    class = INSTCLASS (inst);
    fprintf (fp, "}");
}

static void
print_values (Object fd, Object vals, int escaped)
{
    int i, num;
    FILE *fp = file_from_fd(fd);

    num = VALUESNUM (vals);
/*  fprintf (fp, "#<"); */
    for (i = 0; i < num; ++i) {
	apply_print (fd, VALUESELS (vals)[i], escaped);
	if (i < (num - 1)) {
	    fprintf (fp, "\n");
	}
    }
/*  fprintf (fp, ">"); */
}

static void
print_param (Object fd, Object param, int escaped)
{
    FILE *fp = file_from_fd(fd);

    if (SECOND (param) != object_class
    /* || CAR (param) == unspecified_object */
	) {
	print_object (fd, CAR (param), escaped);
	fprintf (fp, " :: ");
	print_type_name (fd, SECOND (param), escaped);
    } else {
	print_object (fd, CAR (param), escaped);
    }

}

/* param_list and unparen_list are the same function */
static void
print_list_helper(Object fd, Object members, int escaped,
	          void (*print_fn)(Object, Object, int), const char* separator)
{
    FILE *fp = file_from_fd(fd);

    if (PAIRP (members)) {
	print_fn (fd, CAR (members), escaped);
	members = CDR (members);
	while (PAIRP (members)) {
	    fprintf (fp, "%s", separator);
	    print_fn (fd, CAR (members), escaped);
	    members = CDR (members);
	}
    }
}

static void
print_param_list (Object fd, Object params, int escaped)
{
  print_list_helper(fd, params, escaped, print_param, ", ");
}

static void
print_unparenthesized_list (Object fd, Object pair, int escaped)
{
  print_list_helper(fd, pair, escaped, print_object, " ");
}

static void
print_generic_function (Object fd, Object gf, int escaped)
{
    int some_args_printed = 0;
    FILE *fp = file_from_fd(fd);

    if (SYMBOLP (GFNAME (gf))) {
	fprintf (fp, "{the generic function %s (", SYMBOLNAME (GFNAME (gf)));
    } else {
	fprintf (fp, "{an anonymous generic function (");
    }

    if (PAIRP (GFREQPARAMS (gf))) {
	print_param_list (fd, GFREQPARAMS (gf), escaped);
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
	print_unparenthesized_list (fd, GFKEYPARAMS (gf), escaped);
	if (GFALLKEYS (gf)) {
	    fprintf (fp, " #, all-keys");
	}
    }
    fprintf (fp, ")}");
}

static void
print_method (Object fd, Object method, int escaped)
{
    int some_args_printed = 0;
    FILE *fp = file_from_fd(fd);

    if (METHNAME (method)) {
	fprintf (fp, "{method %s (", SYMBOLNAME (METHNAME (method)));
    } else {
	fprintf (fp, "{an anonymous method (");
    }
    if (PAIRP (METHREQPARAMS (method))) {
	print_param_list (fd, METHREQPARAMS (method), escaped);
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
	print_unparenthesized_list (fd, METHKEYPARAMS (method), escaped);
	if (METHALLKEYS (method)) {
	    fprintf (fp, ", #all-keys");
	}
    }
    fprintf (fp, ")");

#if 1
    print_unparenthesized_list (fd, METHBODY (method), escaped);
#endif

    fprintf (fp, "}");
}

static void
print_class (Object fd, Object class, int escaped)
{
    FILE *fp = file_from_fd(fd);

    if (!SYMBOLNAME (CLASSNAME (class))) {
	fprintf (fp, "{an anonymous class");
    } else {
	fprintf (fp, "{the class %s", SYMBOLNAME (CLASSNAME (class)));
    }

    fprintf (fp, " (%d)}", CLASSINDEX (class));
}

static void
print_slot_descriptor (Object fd, Object slotd, int escaped)
{
    FILE *fp = file_from_fd(fd);

    fprintf (fp, "{slot descriptor ");
    print_object (fd, SLOTDGETTER (slotd), escaped);
    if (SLOTDALLOCATION (slotd) != instance_symbol) {
	fprintf (fp, " allocation: ");
	print_object (fd, SLOTDALLOCATION (slotd), escaped);
    }
    if (SLOTDSETTER (slotd)) {
	fprintf (fp, " setter: ");
	print_object (fd, SLOTDSETTER (slotd), escaped);
    }
    if (SLOTDSLOTTYPE (slotd) != object_class) {
	if (SLOTDDEFERREDTYPE (slotd)) {
	    fprintf (fp, " deferred-type: ");
	} else {
	    fprintf (fp, " type: ");
	}
	print_object (fd, SLOTDSLOTTYPE (slotd), escaped);
    }
    if (SLOTDINIT (slotd) != uninit_slot_object) {
	if (SLOTDINITFUNCTION (slotd)) {
	    fprintf (fp, " init-function: ");
	} else {
	    fprintf (fp, " init: ");
	}
	print_object (fd, SLOTDINIT (slotd), escaped);
    }
    if (SLOTDINITKEYWORD (slotd)) {
	if (SLOTDKEYREQ (slotd)) {
	    fprintf (fp, " required-init-keyword: ");
	} else {
	    fprintf (fp, " init-keyword: ");
	}
	print_object (fd, SLOTDINITKEYWORD (slotd), escaped);
    }
    fprintf (fp, "}");
}

static int cur_el;
static void
print_array_help (Object fd, Object dims, Object *els, int escaped);

static void
print_array (Object fd, Object array, int escaped)
{
    Object dims, *els;
    FILE *fp = file_from_fd(fd);

    dims = ARRDIMS (array);
    els = ARRELS (array);

    cur_el = 0;
    fprintf (fp, "#%da", list_length (dims));
    print_array_help (fd, dims, els, escaped);
}

static void
print_array_help (Object fd, Object dims, Object *els, int escaped)
{
    int dim_val, i;
    FILE *fp = file_from_fd(fd);

    fprintf (fp, "#(");
    if (EMPTYLISTP (dims)) {
	apply_print (fd, els[cur_el++], escaped);
	return;
    }
    dim_val = INTVAL (CAR (dims));
    if (EMPTYLISTP (CDR (dims))) {
	for (i = 0; i < dim_val; ++i) {
	    apply_print (fd, els[cur_el++], escaped);
	    if (i < (dim_val - 1)) {
		fprintf (fp, " ");
	    }
	}
    } else {
	for (i = 0; i < dim_val; ++i) {
	    print_array_help (fd, CDR (dims), els, escaped);
	}
    }
    fprintf (fp, ")");
}

static void
print_string (Object fd, Object str, int escaped)
{
    FILE *fp = file_from_fd(fd);

    if (escaped) {
	fprintf (fp, "\"%s\"", BYTESTRVAL (str));
    } else {
	fprintf (fp, "%s", BYTESTRVAL (str));
    }
}

static void
print_type_name (Object fd, Object obj, int escaped)
{
    FILE *fp = file_from_fd(fd);

    switch (object_type (obj)) {
    case Class:
	fprintf (fp, "%s", SYMBOLNAME (CLASSNAME (obj)));
	break;
    case LimitedIntType:
    case UnionType:
    case Singleton:
	print_object (fd, obj, escaped);
	break;
    default:
	error ("print_type_name: object is not a type", obj);
    }
}
