/* classprec-ll.c -- see COPYRIGHT for use */

#include <marlais/classprec.h>

#include <marlais/alloc.h>
#include <marlais/list.h>
#include <marlais/number.h>
#include <marlais/print.h>
#include <marlais/stream.h>

#ifdef NO_COMMON_DYLAN_SPEC
extern Object standard_error_stream;
#endif

/* Internal types */

typedef struct prec_graph {
    Object *class_vec;
    int *succ_vec;
    int succ_size;
    int num_classes;
} prec_graph;

typedef struct l_list {
    int size;
    int *vec;
} l_list;

/* Internal function Declarations */

static void print_graph (prec_graph graph);
static void print_l_list (l_list arg, prec_graph graph);

static prec_graph build_l_graph (Object class, Object direct_superclasses);
static int get_class_index (Object class, prec_graph graph);
static void concatenate_successor (prec_graph graph,
				   int class_index,
				   int successor_class_index);
static Object loops (Object class, prec_graph graph);
static l_list step (int class_index, int *subs, prec_graph graph);
static l_list loops_concatenate (l_list list1, l_list list2, prec_graph graph);
static Object l_list_to_class_list (l_list this_l_list, prec_graph graph);

/* Internal macros */

#define NO_MORE_SUCCS(x)	(-1 == x)

/* Exported functions */

Object
marlais_compute_class_precedence_list (Object class)
{
    Object cpl;
    prec_graph graph;

    graph = build_l_graph (class, CLASSSUPERS (class));

    cpl = loops (class, graph);
    return cpl;
}

/* Internal functions */

static void
print_graph (prec_graph graph)
{
    int i, j, index;

    printf ("graph\n");
    index = 0;
    for (i = 0; i < graph.num_classes; i++) {
	printf ("  class: ");
	marlais_print_obj (marlais_standard_error, graph.class_vec[i]);
	printf ("   successors:\n");
	for (j = 0; j < graph.succ_size; j++, index++) {
	    if (graph.succ_vec[index] != -1) {
		printf ("              ");
		marlais_print_obj (marlais_standard_error,
				   graph.class_vec[graph.succ_vec[index]]);
		printf ("\n");
	    }
	}
    }
}

static void
print_l_list (l_list arg, prec_graph graph)
{
    int i;

    printf ("l-list :size = %d", arg.size);
    for (i = 0; i < arg.size; i++) {
	printf ("       [%d] = ", i);
	marlais_print_obj (marlais_standard_error, graph.class_vec[arg.vec[i]]);
	printf ("\n");
    }
}

static prec_graph
build_l_graph (Object class,
	       Object direct_superclasses)
{
    int num_classes;
    Object *class_vec;
    prec_graph graph;
    int i;

    num_classes = CLASSNUMPRECS (class);

    {				/* Initialize class vector */
	Object sorted_supers;

	class_vec = (Object *) marlais_allocate_memory (num_classes * sizeof (Object));

	sorted_supers = CLASSSORTEDPRECS (class);

	for (i = 0; i < num_classes; i++) {
	    class_vec[i] = CAR (sorted_supers);
	    sorted_supers = CDR (sorted_supers);
	}
    }
    graph.class_vec = class_vec;
    graph.succ_size = list_length (direct_superclasses);
    graph.succ_vec = (int *) marlais_allocate_memory (num_classes * graph.succ_size
					      * sizeof (int));

    graph.num_classes = num_classes;
    for (i = 0; i < num_classes * graph.succ_size; i++) {
	graph.succ_vec[i] = -1;

    }

    {				/* Fill in successor in graph */
	Object direct_superclass;
	Object superclasses;

	int class_index = get_class_index (class, graph);
	int direct_superclass_index;

	while (!EMPTYLISTP (direct_superclasses)) {
	    direct_superclass = CAR (direct_superclasses);
	    direct_superclass_index = get_class_index (direct_superclass,
						       graph);
	    concatenate_successor (graph,
				   class_index,
				   direct_superclass_index);
	    superclasses = CLASSPRECLIST (direct_superclass);
	    {
		int current_class_index;
		int next_class_index = get_class_index (CAR (superclasses),
							graph);

		superclasses = CDR (superclasses);
		while (!EMPTYLISTP (superclasses)) {
		    current_class_index = next_class_index;
		    next_class_index = get_class_index (CAR (superclasses),
							graph);
		    concatenate_successor (graph,
					   current_class_index,
					   next_class_index);
		    superclasses = CDR (superclasses);
		}
	    }
	    direct_superclasses = CDR (direct_superclasses);
	}
    }
    return graph;
}

static int
get_class_index (Object class, prec_graph graph)
{
    int key;
    int low_index;
    int high_index;
    int mid_point;

    key = CLASSINDEX (class);

    low_index = 0;
    high_index = graph.num_classes - 1;
    while (low_index <= high_index) {
	mid_point = (low_index + high_index) / 2.0;
	if (CLASSINDEX (graph.class_vec[mid_point]) < key) {
	    low_index = mid_point + 1;
	} else if (CLASSINDEX (graph.class_vec[mid_point]) == key) {
	    return mid_point;
	} else {
	    high_index = mid_point - 1;
	}
    }
    marlais_fatal ("Unable to find index of class in cpl computation");
}

static void
concatenate_successor (prec_graph graph,
		       int class_index,
		       int successor_class_index)
{
    int i;
    int *vec = graph.succ_vec + class_index * graph.succ_size;

    for (i = 0; i < graph.succ_size; i++) {
	if (NO_MORE_SUCCS (vec[i])) {
	    break;
	}
	if (successor_class_index == vec[i]) {
	    return;
	}
    }

    if (i < graph.succ_size) {
	vec[i] = successor_class_index;
	return;
    }
    marlais_fatal ("Too many successors for class");
}

static Object
loops (Object class,
       prec_graph graph)
{
    int i, j;
    int *subs;
    int *vec;
    l_list new_list;

    subs = (int *) marlais_allocate_memory (graph.num_classes * sizeof (int));

    for (i = 0; i < graph.num_classes; i++) {
	subs[i] = 0;
    }

    for (i = 0; i < graph.num_classes; i++) {
	vec = graph.succ_vec + graph.succ_size * i;
	for (j = 0; j < graph.succ_size; j++) {
	    if (NO_MORE_SUCCS (vec[j])) {
		break;
	    }
	    subs[vec[j]]++;
	}
    }

    new_list = step (get_class_index (class, graph), subs, graph);

    if (new_list.size < graph.num_classes) {
	/* At least one class didn't make it into the precedence list */
	marlais_error ("Unable to construct precedence list for class",
	       class,
	       NULL);
    }
    return l_list_to_class_list (new_list, graph);
}

static l_list
step (int class_index, int *subs, prec_graph graph)
{
    int i;
    int *vec;
    l_list this_l_list;

    vec = graph.succ_vec + class_index * graph.succ_size;
    for (i = 0; i < graph.succ_size; i++) {
	if (NO_MORE_SUCCS (vec[i])) {
	    break;
	}
	subs[vec[i]] = subs[vec[i]] - 1;
    }

/*    l_list = cons (graph.class_vec[class_index], make_empty_list()); */

    this_l_list.size = 1;
    this_l_list.vec = (int *) marlais_allocate_memory (sizeof (int));

    this_l_list.vec[0] = class_index;

    for (i = 0; i < graph.succ_size && !NO_MORE_SUCCS (vec[i]); i++) {
	if (0 == subs[vec[i]]) {
	    this_l_list = loops_concatenate (this_l_list,
					     step (vec[i], subs, graph),
					     graph);
	}
    }

    return this_l_list;
}

static l_list
loops_concatenate (l_list l_list1, l_list l_list2, prec_graph graph)
{
    l_list new_l_list;

#if defined (__GNUC__)
    int used_classes[graph.num_classes];

#else
    int *used_classes = (int *) marlais_allocate_memory (graph.num_classes * sizeof (int));

#endif
    int i, j;

    for (i = 0; i < graph.num_classes; i++) {
	used_classes[i] = 0;
    }

    new_l_list.vec = (int *) marlais_allocate_memory ((l_list1.size + l_list2.size)
					      * sizeof (int));

    for (i = 0; i < l_list1.size; i++) {
	new_l_list.vec[i] = l_list1.vec[i];
	used_classes[new_l_list.vec[i]] = 1;
    }

    for (j = 0; j < l_list2.size; j++) {
	if (!used_classes[l_list2.vec[j]]) {
	    new_l_list.vec[i++] = l_list2.vec[j];
	    used_classes[l_list2.vec[j]] = 1;
	}
    }

    new_l_list.size = i;
    return new_l_list;
}

static Object
l_list_to_class_list (l_list this_l_list, prec_graph graph)
{
    Object new_list;
    Object *list_ptr;
    int i;

    list_ptr = &new_list;

    for (i = 0; i < this_l_list.size; i++) {
	*list_ptr = cons (graph.class_vec[this_l_list.vec[i]],
			  make_empty_list ());
	list_ptr = &CDR (*list_ptr);
    }
    return new_list;
}
