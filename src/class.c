/* class.c -- see COPYRIGHT for use */

#include <marlais/class.h>

#include <marlais/alloc.h>
#include <marlais/apply.h>
#include <marlais/array.h>
#include <marlais/boolean.h>
#include <marlais/bytestring.h>
#include <marlais/classprec.h>
#include <marlais/deque.h>
#include <marlais/env.h>
#include <marlais/eval.h>
#include <marlais/function.h>
#include <marlais/keyword.h>
#include <marlais/list.h>
#include <marlais/number.h>
#include <marlais/prim.h>
#include <marlais/slot.h>
#include <marlais/symbol.h>
#include <marlais/table.h>
#include <marlais/values.h>
#include <marlais/vector.h>

extern struct binding *symbol_binding (Object sym);

int last_class_index = 0;
static Object class_slots_class;

/* primitives */
static Object class_debug_name (Object class);
static Object class_precedence_list (Object class);

static Object make_limited_int_type (Object args);
static Object make_union_type (Object typelist);

static struct primitive class_prims[] =
{
    {"%object-class", prim_1, marlais_object_class},

    {"%make", prim_2, marlais_make},
    {"%instance?", prim_2, marlais_instance_p},
    {"%subtype?", prim_2, marlais_subtype_p},

    {"%singleton", prim_1, marlais_singleton},
    {"%union-type", prim_1, marlais_make_union_type},

    {"%direct-superclasses", prim_1, marlais_direct_superclasses},
    {"%direct-subclasses", prim_1, marlais_direct_subclasses},
    {"%seal", prim_1, marlais_make_class_sealed},
    {"%limited-integer", prim_1, make_limited_int_type},
    {"%all-superclasses", prim_1, class_precedence_list},
    {"%class-debug-name", prim_1, class_debug_name},
};

/* Internal function prototypes */

static Object make_instance (Object class, Object *initializers);
static Object make_class_driver (Object args);
static Object initialize_slots (Object descriptors, Object initializers);
static int member_2 (Object obj1, Object obj2, Object obj_list);
static Object make_builtin_class (char *name, Object superclasses);
static void add_slot_descriptor_names (Object sd_list, Object *sg_names_ptr);
static void append_slot_descriptors (Object sd_list,
				     Object **new_sd_list_insert_ptr,
				     Object *sg_names_ptr);
static void append_one_slot_descriptor (Object sd,
					Object **new_sd_list_insert_ptr,
					Object *sg_names_ptr);
static void make_getters_setters (Object class, Object slots);
static Object make_getter_method (Object getter_name,
				  Object class,
				  int slot_num);
static Object make_setter_method (Object slot,
				  Object class,
				  int slot_num);
static Object pair_list_reverse (Object lst);
static Object replace_slotd_init (Object init_slotds, Object keyword,
				  Object init);
static void initialize_slotds (Object class);
static void eval_slotds (Object slotds);
static Object merge_sorted_precedence_lists (Object class, Object supers);
static Object merge_class_lists (Object left, Object right);

/* Exported functions */

void
marlais_initialize_class (void)
{
  object_class = make_builtin_class ("<object>", make_empty_list ());

  /* fix up the binding for object_class so that it is correct */
  {
    struct binding *binding;

    binding = symbol_binding (CLASSNAME (object_class));
    binding->type = object_class;
  }
  boolean_class = make_builtin_class ("<boolean>", object_class);

  /* Numeric classes */
  number_class = make_builtin_class ("<number>", object_class);
  complex_class = make_builtin_class ("<complex>", number_class);
  real_class = make_builtin_class ("<real>", complex_class);
  rational_class = make_builtin_class ("<rational>", real_class);
  integer_class = make_builtin_class ("<integer>", rational_class);
  small_integer_class = make_builtin_class ("<small-integer>",
					    integer_class);
  big_integer_class = make_builtin_class ("<big-integer>", integer_class);

  ratio_class = make_builtin_class ("<ratio>", rational_class);
  float_class = make_builtin_class ("<float>", real_class);
  single_float_class = make_builtin_class ("<single-float>", float_class);
  double_float_class = make_builtin_class ("<double-float>", float_class);

  /* Collection classes */
  collection_class = make_builtin_class ("<collection>", object_class);
  explicit_key_collection_class =
    make_builtin_class ("<explicit-key-collection>",
			collection_class);
  stretchy_collection_class =
    make_builtin_class ("<stretchy-collection>", collection_class);
  mutable_collection_class =
    make_builtin_class ("<mutable-collection>", collection_class);
  sequence_class =
    make_builtin_class ("<sequence>", collection_class);
  mutable_explicit_key_collection_class =
    make_builtin_class ("<mutable-explicit-key-collection>",
			listem (explicit_key_collection_class,
				mutable_collection_class,
				NULL));
  mutable_sequence_class =
    make_builtin_class ("<mutable-sequence>",
			listem (mutable_collection_class,
				sequence_class,
				NULL));
  table_class =
    make_builtin_class ("<table>",
			listem (mutable_explicit_key_collection_class,
				stretchy_collection_class,
				NULL));

  object_table_class =
    make_builtin_class ("<object-table>", table_class);

  deque_class =
    make_builtin_class ("<deque>",
			listem (mutable_sequence_class,
				stretchy_collection_class,
				NULL));
  array_class =
      make_builtin_class ("<array>", mutable_sequence_class);
  list_class = make_builtin_class ("<list>", mutable_sequence_class);
  empty_list_class = make_builtin_class ("<empty-list>", list_class);
  pair_class = make_builtin_class ("<pair>", list_class);
  string_class = make_builtin_class ("<string>", mutable_sequence_class);
  vector_class = make_builtin_class ("<vector>", array_class);
  stretchy_vector_class = make_builtin_class("<stretchy-vector>",
					     listem(vector_class,
						    stretchy_collection_class,
						    NULL));


  byte_string_class =
    make_builtin_class ("<byte-string>",
			listem (string_class,
				vector_class,
				NULL));
  unicode_string_class =
    make_builtin_class ("<unicode-string>",
			listem (string_class,
				vector_class,
				NULL));
  simple_object_vector_class =
    make_builtin_class ("<simple-object-vector>", vector_class);

  /* Condition classes */
  condition_class = make_builtin_class ("<condition>", object_class);
  serious_condition_class = make_builtin_class ("<serious-condition>",
						condition_class);
  warning_class = make_builtin_class ("<warning>", condition_class);
  simple_warning_class = make_builtin_class ("<simple-warning>",
					     warning_class);
  restart_class = make_builtin_class ("<restart>", condition_class);
  simple_restart_class = make_builtin_class ("<simple-restart>",
					     restart_class);
  abort_class = make_builtin_class ("<abort>", restart_class);
  error_class = make_builtin_class ("<error>", condition_class);
  simple_error_class = make_builtin_class ("<simple-error>",
					   error_class);
  type_error_class = make_builtin_class ("<type-error>",
					 error_class);
  sealed_object_error_class =
    make_builtin_class ("<sealed-object-error>", error_class);
  symbol_class = make_builtin_class ("<variable-name>", object_class);
  keyword_class = make_builtin_class ("<symbol>", object_class);
  character_class = make_builtin_class ("<character>", object_class);
  function_class = make_builtin_class ("<function>", object_class);
  primitive_class = make_builtin_class ("<primitive>", function_class);
  generic_function_class =
    make_builtin_class ("<generic-function>", function_class);
  method_class = make_builtin_class ("<method>", function_class);
  exit_function_class =
    make_builtin_class ("<exit-function>", function_class);
  unwind_protect_function_class =
    make_builtin_class ("<unwind-protect-function>", function_class);
  type_class = make_builtin_class ("<type>", object_class);
  singleton_class = make_builtin_class ("<singleton>", type_class);
  class_class = make_builtin_class ("<class>", type_class);

  table_entry_class = make_builtin_class ("<table-entry>", object_class);
  deque_entry_class = make_builtin_class ("<deque-entry>", object_class);

  class_slots_class =
    make_builtin_class ("<class-slots-class>", object_class);

  object_handle_class =
    make_builtin_class ("<object-handle>", object_class);

  foreign_pointer_class =
    make_builtin_class ("<foreign-pointer>", object_class);	/* <pcb> */

  marlais_make_class_sealed (integer_class);
  marlais_make_class_sealed (ratio_class);
  marlais_make_class_sealed (rational_class);
  marlais_make_class_sealed (single_float_class);
  marlais_make_class_sealed (double_float_class);
  marlais_make_class_sealed (float_class);
  marlais_make_class_sealed (real_class);
  marlais_make_class_sealed (empty_list_class);
  marlais_make_class_sealed (pair_class);
  marlais_make_class_sealed (list_class);
  marlais_make_class_sealed (byte_string_class);
  marlais_make_class_sealed (unicode_string_class);
  marlais_make_class_sealed (simple_object_vector_class);

  /* here, need to make things like sequence_class uninstantiable */

  marlais_make_class_uninstantiable (object_class); /* DMA, August 20, 2001 */

  marlais_make_class_uninstantiable (collection_class);
  marlais_make_class_uninstantiable (explicit_key_collection_class);
  marlais_make_class_uninstantiable (stretchy_collection_class);
  marlais_make_class_uninstantiable (mutable_collection_class);
  marlais_make_class_uninstantiable (sequence_class);
  marlais_make_class_uninstantiable (mutable_explicit_key_collection_class);
  marlais_make_class_uninstantiable (mutable_sequence_class);

  marlais_make_class_uninstantiable (number_class);
  marlais_make_class_uninstantiable (complex_class);

  marlais_make_class_uninstantiable (condition_class);
  marlais_make_class_uninstantiable (serious_condition_class);
  marlais_make_class_uninstantiable (warning_class);
  marlais_make_class_uninstantiable (restart_class);
  marlais_make_class_uninstantiable (error_class);
}

void
marlais_register_class (void)
{
  int num = sizeof (class_prims) / sizeof (struct primitive);
  marlais_register_prims (num, class_prims);
}

Object
marlais_object_class (Object obj)
{
  switch (object_type (obj)) {
  case Integer:
    return (small_integer_class);
  case BigInteger:
    return (big_integer_class);
  case True:
  case False:
    return (boolean_class);
    break;
  case Ratio:
    return (ratio_class);
  case SingleFloat:
    return (single_float_class);
  case DoubleFloat:
    return (double_float_class);
  case EmptyList:
    return (empty_list_class);
  case Pair:
    return (pair_class);
  case ByteString:
    return (byte_string_class);
  case SimpleObjectVector:
    return (simple_object_vector_class);
  case ObjectTable:
    return (object_table_class);
  case Deque:
    return (deque_class);
  case Array:
    return (array_class);
  case Condition:
    return (condition_class);
  case Symbol:
    return (symbol_class);
  case Keyword:
    return (keyword_class);
  case Character:
    return (character_class);
  case NextMethod:
    return (method_class);
  case Class:
    return (class_class);
  case Instance:
    return (INSTCLASS (obj));

    /* need to check the following two cases */
  case LimitedIntType:
    return (type_class);
  case UnionType:
    return (type_class);

  case Primitive:
    return (primitive_class);
  case GenericFunction:
    return (generic_function_class);
  case Method:
    return (method_class);
  case Exit:
    return (exit_function_class);
  case Unwind:
    return (unwind_protect_function_class);
  case Unspecified:
    return (object_class);
  case EndOfFile:
    return (object_class);
  case TableEntry:
    return (table_entry_class);
  case DequeEntry:
    return (deque_entry_class);
  case Singleton:
    return (singleton_class);
  case ObjectHandle:
    return (object_handle_class);
  case ForeignPtr:
    return (foreign_pointer_class);		/* <pcb> */
  case UninitializedSlotValue:
    return (object_class);
  default:
    return marlais_error ("object-class: don't know class of object", obj, NULL);
  }
}

Object
marlais_instance_p (Object obj, Object type)
{
  return (marlais_instance (obj, type) ? MARLAIS_TRUE : MARLAIS_FALSE);
}

int
marlais_instance (Object obj, Object type)
{
  Object objtype;

  if (SINGLETONP (type)) {
    return marlais_identical_p (obj, SINGLEVAL (type));
  } else if (LIMINTP (type)) {
    if (INTEGERP (obj) &&
	((!LIMINTHASMIN (type)) ||
	 INTVAL (obj) >= LIMINTMIN (type)) &&
	((!LIMINTHASMAX (type)) ||
	 INTVAL (obj) <= LIMINTMAX (type))) {
      return 1;
    } else {
      return 0;
    }
  } else if (LIMINTP (obj)) {
    /* not sure on this one.  jnw */
    return marlais_subtype (type_class, type);
  } else if (UNIONP (type)) {
    Object ptr;

    for (ptr = UNIONLIST (type); PAIRP (ptr); ptr = CDR (ptr)) {
      if (marlais_instance (obj, (CAR (ptr)))) {
	return 1;
      }
    }
    return 0;
  }
  objtype = marlais_object_class (obj);
  if (objtype == type) {
    return 1;
  } else {
    return (marlais_subtype (objtype, type));
  }
}

Object
marlais_subtype_p (Object type1, Object type2)
{
  return (marlais_subtype (type1, type2) ? MARLAIS_TRUE : MARLAIS_FALSE);
}

int
marlais_subtype (Object type1, Object type2)
{
  Object supers;

  if (type1 == type2) {
    return 1;
  } else if (SINGLETONP (type1)) {
    return (marlais_instance (SINGLEVAL (type1), type2));
  } else if (LIMINTP (type1)) {
    if (LIMINTP (type2)) {
      if (((!LIMINTHASMIN (type2)) ||
	   (LIMINTHASMIN (type1) &&
	    (LIMINTMIN (type1) >= LIMINTMIN (type2))))
	  &&
	  ((!LIMINTHASMAX (type2)) ||
	   (LIMINTHASMAX (type1) &&
	    (LIMINTMAX (type1) <= LIMINTMAX (type2))))) {
	return 1;
      } else {
	return 0;
      }
    } else {
      return (marlais_subtype (integer_class, type2));
    }
  } else if (UNIONP (type1)) {
    Object ptr;

    for (ptr = UNIONLIST (type1); PAIRP (ptr); ptr = CDR (ptr)) {
      if (!marlais_subtype (CAR (ptr), type2)) {
	return 0;
      }
    }
    return 1;
  } else if (UNIONP (type2)) {
    Object ptr;

    for (ptr = UNIONLIST (type2); PAIRP (ptr); ptr = CDR (ptr)) {
      if (marlais_subtype (type1, CAR (ptr))) {
	return 1;
      }
    }
    return 0;
  } else {
    supers = CLASSSUPERS (type1);
    if (!supers) {
      return 0;
    }
    while (!EMPTYLISTP (supers)) {
      if (marlais_subtype (CAR (supers), type2)) {
	return 1;
      }
      supers = CDR (supers);
    }
    return 0;
  }
}

Object
marlais_same_class_p (Object class1, Object class2)
{
  if (class1 == class2) {
    return (MARLAIS_TRUE);
  } else if ((POINTERTYPE (class1) == Singleton) &&
	     (POINTERTYPE (class2) == Singleton)) {
    if (marlais_identical_p (SINGLEVAL(class1), SINGLEVAL(class2))) {
      return MARLAIS_TRUE;
    } else {
      return MARLAIS_FALSE;
    }
  } else {
    return MARLAIS_FALSE;
  }
}

Object
marlais_direct_subclasses (Object class)
{
  return CLASSSUBS (class);
}

Object
marlais_direct_superclasses (Object class)
{
  if (!SEALEDP (class)) {
    return CLASSSUPERS (class);
  } else {
    return make_empty_list ();
  }
}

Object
marlais_make (Object class, Object rest)
{
  Object ret, initialize_fun;

  if (!INSTANTIABLE (class)) {
    marlais_error ("make: class uninstantiable", class, NULL);
    return MARLAIS_FALSE;
  }
  /* special case the builtin classes */
  if (class == pair_class) {
    ret = make_pair_driver (rest);
  } else if (class == empty_list_class) {
    ret = make_empty_list ();
  } else if (class == list_class) {
    ret = make_list_driver (rest);
  } else if ((class == vector_class) ||
	     (class == simple_object_vector_class)) {
    ret = marlais_make_vector_entry (rest);
  } else if ((class == string_class) || (class == byte_string_class)) {
    ret = marlais_make_bytestring_entry (rest);
  } else if (class == generic_function_class) {
    ret = make_generic_function_driver (rest);
  } else if ((class == table_class) || (class == object_table_class)) {
    ret = marlais_make_table_driver (rest);
  } else if (class == deque_class) {
    ret = marlais_make_deque_entry (rest);
  } else if (class == array_class) {
    ret = marlais_make_array_entry (rest);
  } else if (class == class_class) {
    ret = make_class_driver (rest);
  } else {
    ret = make_instance (class, &rest);
  }
  initialize_fun = symbol_value (initialize_symbol);
  if (initialize_fun) {
    marlais_apply (initialize_fun, cons (ret, rest));
  } else {
    marlais_warning ("make: no `initialize' generic function", class, NULL);
  }
  return (ret);
}

Object
marlais_singleton (Object val)
{
  Object obj;

  obj = marlais_allocate_object (Singleton, sizeof (struct singleton));

  SINGLEVAL (obj) = val;
  return (obj);
}

/*
 * Incredibly speculative!
 */
Object
marlais_make_union_type (Object typelist)
{
  Object obj, ptr, qtr, union_types;

  obj = marlais_allocate_object (UnionType, sizeof (struct union_type));

  union_types = make_empty_list ();

  for (ptr = typelist; PAIRP (ptr); ptr = CDR (ptr)) {
    if (UNIONP (CAR (ptr))) {
      for (qtr = UNIONLIST (CAR (ptr)); PAIRP (qtr); qtr = CDR (qtr)) {
	union_types = cons (CAR (qtr), union_types);
      }
    } else {
      union_types = cons (CAR (ptr), union_types);
    }
  }
  UNIONLIST (obj) = union_types;

  return obj;
}

Object
marlais_make_class (Object obj,
		    Object supers,
		    Object slot_descriptors,
		    Object abstract_p,
		    char *debug_name)
{
  Object allsuperclasses, super;
  Object tmp, slot;
  Object sg_names;
  Object *i_tmp_ptr;
  Object *s_tmp_ptr;
  Object *cl_tmp_ptr;
  Object *es_tmp_ptr;
  Object *co_tmp_ptr;
  Object *vi_tmp_ptr;

  CLASSINDEX (obj) = NEWCLASSINDEX;
  CLASSENV (obj) = the_env;
  if(abstract_p == MARLAIS_FALSE) CLASSPROPS (obj) |= CLASSINSTANTIABLE;

  /* allow a single value for supers, make it into a list
   */
  if (!LISTP (supers)) {
    CLASSSUPERS (obj) = cons (supers, make_empty_list ());
  } else {
    CLASSSUPERS (obj) = supers;
  }
  CLASSSORTEDPRECS (obj) =
    merge_sorted_precedence_lists (obj, CLASSSUPERS (obj));
  CLASSNUMPRECS (obj) = list_length (CLASSSORTEDPRECS (obj));
  CLASSPRECLIST (obj) = marlais_compute_class_precedence_list (obj);

  /* first find slot descriptors for this class */

  CLASSINSLOTDS (obj) = make_empty_list ();
  CLASSSLOTDS (obj) = make_empty_list ();
  CLASSCSLOTDS (obj) = make_empty_list ();
  CLASSESSLOTDS (obj) = make_empty_list ();
  CLASSCONSTSLOTDS (obj) = make_empty_list ();
  CLASSVSLOTDS (obj) = make_empty_list ();

  /* Process superclasses.  This includes:
   *  1. add the slots of the superclasses
   *  2. add this class to the subclass list of each superclass
   */

  if (!LISTP (supers)) {
    /* only one superclass */
    CLASSSUBS (supers) = cons (obj, CLASSSUBS (supers));
  } else {
    while (PAIRP (supers)) {
      super = CAR (supers);
      CLASSSUBS (super) = cons (obj, CLASSSUBS (super));
      supers = CDR (supers);
    }
  }

  /*    update_slot_descriptors (class); */

  i_tmp_ptr = &CLASSINSLOTDS (obj);
  s_tmp_ptr = &CLASSSLOTDS (obj);
  cl_tmp_ptr = &CLASSCSLOTDS (obj);
  es_tmp_ptr = &CLASSESSLOTDS (obj);
  co_tmp_ptr = &CLASSCONSTSLOTDS (obj);
  vi_tmp_ptr = &CLASSVSLOTDS (obj);

  allsuperclasses = list_reverse (CDR (CLASSPRECLIST (obj)));

  sg_names = make_empty_list ();
  while (!EMPTYLISTP (allsuperclasses)) {
    /* check for sealed superclass */
    if (SEALEDP (CAR (allsuperclasses))) {
      marlais_error ("Cannot create subclass of sealed class",
	     CAR (allsuperclasses), NULL);
    }
    super = CAR (allsuperclasses);
    append_slot_descriptors (CLASSSLOTDS (super), &i_tmp_ptr, &sg_names);
    append_slot_descriptors (CLASSESSLOTDS (super), &es_tmp_ptr,
			     &sg_names);
    append_slot_descriptors (CLASSCONSTSLOTDS (super),
			     &co_tmp_ptr, &sg_names);
    add_slot_descriptor_names (CLASSVSLOTDS (super), &sg_names);
    allsuperclasses = CDR (allsuperclasses);
  }
  CLASSSUBS (obj) = make_empty_list ();

  for (tmp = slot_descriptors; PAIRP (tmp); tmp = CDR (tmp)) {
    slot = CAR (tmp);
    if (SLOTDALLOCATION (slot) == instance_symbol) {
      append_one_slot_descriptor (slot, &s_tmp_ptr, &sg_names);
    } else if (SLOTDALLOCATION (slot) == class_symbol) {
      append_one_slot_descriptor (slot, &cl_tmp_ptr, &sg_names);
    } else if (SLOTDALLOCATION (slot) == each_subclass_symbol) {
      append_one_slot_descriptor (slot, &es_tmp_ptr, &sg_names);
    } else if (SLOTDALLOCATION (slot) == constant_symbol) {
      append_one_slot_descriptor (slot, &co_tmp_ptr, &sg_names);
    } else if (SLOTDALLOCATION (slot) == virtual_symbol) {
      append_one_slot_descriptor (slot, &vi_tmp_ptr, &sg_names);
    }
  }

  if (!CLASSNAME (obj)) {
    CLASSNAME (obj) = make_symbol (debug_name);
    // XXX what is this!?
    //marlais_warning ("Making class name", CLASSNAME(obj), NULL);
  }
  /* initialize class and each-subclass slot objects */
  CLASSCSLOTS (obj) = marlais_allocate_object (Instance, sizeof (struct instance));

  INSTCLASS (CLASSCSLOTS (obj)) = class_slots_class;

  /*
   * Note - CLASSCSLOTDS must precede CLASSESSLOTDS for
   * print_class_slot_values (print.c) to work correctly.
   */
  INSTSLOTS (CLASSCSLOTS (obj)) =
    (Object *) (VALUESELS (initialize_slots (append (CLASSCSLOTDS (obj),
						     CLASSESSLOTDS (obj)),
					     make_empty_list ()))[0]);

  return (obj);
}

void
marlais_make_class_primary (Object class)
{
  /* Need to add some semantics here.  Requires field in class object rep. */
}

Object
marlais_make_class_sealed (Object class)
{
  CLASSPROPS (class) |= CLASSSEAL;
  return class;
}

void
marlais_make_class_uninstantiable (Object class)
{
  CLASSPROPS (class) &= ~CLASSINSTANTIABLE;
}

Object
marlais_make_slot_descriptor_list (Object slots, int do_eval)
{
  Object slot;
  Object getter, setter;
  Object type, init;
  Object init_keyword, allocation;
  int type_seen, init_seen, allocation_seen, dynamism_seen, getter_seen;
  int inherited_slot;
  unsigned char properties;
  Object descriptors;
  Object *desc_ptr;
  Object slotelt;
  Object dynamism;

  descriptors = make_empty_list ();
  desc_ptr = &descriptors;
  while (PAIRP (slots)) {
    slot = CAR (slots);

    getter = NULL;
    setter = NULL;
    type = CLASSNAME (object_class);
    init = uninit_slot_object;
    init_keyword = NULL;
    allocation = instance_symbol;
    dynamism = open_symbol;
    type_seen = 0;
    init_seen = 0;
    allocation_seen = 0;
    dynamism_seen = 0;
    getter_seen = 0;
    properties = 0;
    inherited_slot = 0;

    if (SYMBOLP (slot)) {
      /* simple slot descriptor */
      getter = slot;
    } else {
      if (SYMBOLP (CAR (slot))) {
	/* first elt is getter name */
	getter = CAR (slot);
	slot = CDR (slot);
	getter_seen = 1;
      }
      while (PAIRP (slot)) {
	slotelt = CAR (slot);
	/* parse keyword-value pairs for slot initialization */
	if (!KEYWORDP (slotelt) || EMPTYLISTP (CDR (slot))) {
	  marlais_error ("malformed slot descriptor", slot, NULL);
	} else if (slotelt == getter_keyword) {
	  if (getter_seen) {
	    marlais_error ("redundant getter specified", SECOND (slot),
		   NULL);
	  }
	  getter_seen = 1;
	  getter = SECOND (slot);
	} else if (slotelt == setter_keyword) {
	  if (setter != NULL) {
	    marlais_error ("redundant specification for slot setter name",
		   SECOND (slot), NULL);
	  }
	  setter = SECOND (slot);
	} else if (slotelt == allocation_keyword) {
	  if (allocation_seen) {
	    marlais_error ("redundant specification for allocation",
		   SECOND (slot), NULL);
	  }
	  allocation_seen = 1;
	  allocation = SECOND (slot);
	  if (marlais_identical_p (allocation, inherited_symbol)) {
	    inherited_slot = 1;
	  }
	} else if (slotelt == type_keyword) {
	  if (type_seen) {
	    marlais_error ("redundant specification for type", SECOND (slot), NULL);
	  }
	  type_seen = 1;
	  /*
	   * type_keyword indicates eval this slot!
	   */
	  type = SECOND (slot);
	} else if (slotelt == deferred_type_keyword) {
	  if (type_seen) {
	    marlais_error ("redundant specification for type",
		   SECOND (slot), NULL);
	  }
	  type_seen = 1;
	  type = SECOND (slot);
	  properties |= SLOTDDEFERREDTYPEMASK;
	} else if (slotelt == init_value_keyword) {
	  if (init_seen) {
	    marlais_error ("redundant specification for initializer",
		   SECOND (slot), NULL);
	  }
	  init_seen = 1;
	  init = SECOND (slot);
	} else if (slotelt == init_function_keyword) {
	  if (init_seen) {
	    marlais_error ("redundant specification for initializer",
		   SECOND (slot), NULL);
	  }
	  init_seen = 1;
	  init = do_eval ? eval (SECOND (slot)) : SECOND (slot);
	  properties |= SLOTDINITFUNCTIONMASK;
	} else if (slotelt == init_keyword_keyword) {
	  if (init_keyword) {
	    marlais_error ("redundant init-keyword: specification",
		   SECOND (slot), NULL);
	  }
	  init_keyword = SECOND (slot);
	  if (!KEYWORDP (init_keyword)) {
	    marlais_error ("init-keyword: value is not a keyword", init_keyword, NULL);
	  }
	} else if (slotelt == required_init_keyword_keyword) {
	  if (init_keyword) {
	    marlais_error ("redundant required-init-keyword: specification",
		   SECOND (slot), NULL);
	  }
	  init_keyword = SECOND (slot);
	  if (!KEYWORDP (init_keyword)) {
	    marlais_error ("required-init-keyword: value is not a keyword",
		   init_keyword, NULL);
	  }
	  properties |= SLOTDKEYREQMASK;
	} else if (slotelt == dynamism_keyword) {
	  if (dynamism_seen) {
	    marlais_error ("Dynamism of slot specified twice",
		   SECOND (slot), NULL);
	  }
	  dynamism = SECOND (slot);
	} else {
	  marlais_error ("unknown slot keyword initializer", slotelt, NULL);
	}
	slot = CDR (CDR (slot));
      }
    }

#if 0
    if (!(getter || inherited_slot))
#else
    if (!getter)
#endif
    {
      marlais_error ("Slot has no getter", CAR (slots), NULL);
    }
    if (allocation == constant_symbol) {
      if (init == NULL || properties & SLOTDINITFUNCTIONMASK) {
	marlais_error ("Bad initialization for constant slot",
	       CAR (slots), NULL);
      }
    }
    if (properties & SLOTDKEYREQMASK) {
      if (init != uninit_slot_object) {
	marlais_error ("required-init-keyword should not have initial value",
	       CAR (slots), NULL);
      }
    }
    *desc_ptr =
      cons (marlais_make_slot_descriptor (properties, getter, setter, type,
				  init, init_keyword, allocation,
				  dynamism),
	    make_empty_list ());
    desc_ptr = &CDR (*desc_ptr);

    slots = CDR (slots);
  }
  return descriptors;
}

void
marlais_make_getter_setter_gfs (Object slotds)
{
  Object getter, setter;

  while (PAIRP (slotds)) {

    /* Fix up the getter first */

    getter = SLOTDGETTER (CAR (slotds));
    if (SYMBOLP (getter)) {
      if (NULL == symbol_value (getter)) {
	SLOTDGETTER (CAR (slotds)) =
	  make_generic_function (getter,
				 listem (x_symbol,
					 hash_rest_symbol,
					 x_symbol,
					 NULL),
				 make_empty_list ());
	add_top_level_binding (getter, SLOTDGETTER (CAR (slotds)), 1);
      } else if (!GFUNP (symbol_value (getter))) {
	marlais_error ("Getter symbol not bound to a generic function",
	       getter,
	       symbol_value (getter),
	       NULL);
      } else {
	SLOTDGETTER (CAR (slotds)) = symbol_value (getter);
      }
    } else {
      /* getter is not a symbol */
      marlais_error ("Getter name is not a symbol", getter, NULL);
    }

    /* Now fix up the setter */

    if (SLOTDALLOCATION (CAR (slotds)) != constant_symbol) {
      setter = SLOTDSETTER (CAR (slotds));
      if (NULL == setter) {
	/* Manufacture the setter name */
	setter =
	  SLOTDSETTER (CAR (slotds)) =
	  make_setter_symbol (getter);
      }
      if (SYMBOLP (setter)) {
	if (NULL == symbol_value (setter)) {
	  SLOTDSETTER (CAR (slotds)) =
	    make_generic_function (setter,
				   listem (x_symbol,
					   x_symbol,
					   hash_rest_symbol,
					   x_symbol,
					   NULL),
				   make_empty_list ());
	  add_top_level_binding (setter,
				 SLOTDSETTER (CAR (slotds)),
				 1);
	} else if (!GFUNP (symbol_value (setter))) {
	  marlais_error ("Setter symbol not bound to a generic function",
		 setter,
		 symbol_value (setter),
		 NULL);
	} else {
	  SLOTDSETTER (CAR (slotds)) = symbol_value (setter);
	}
      } else if (setter == MARLAIS_FALSE) {
	SLOTDSETTER (CAR (slotds)) = setter;
      } else {
	/* setter is not a symbol */
	marlais_error ("Setter name is not a symbol", setter, NULL);
      }

    }
    slotds = CDR (slotds);
  }
}

/* Internal functions */

/* Destructively modifies second parameter to include default initializations. */
static Object
make_instance (Object class, Object *initializers)
{
  Object obj, ret;

  obj = marlais_allocate_object (Instance, sizeof (struct instance));

  INSTCLASS (obj) = class;
  initialize_slotds (class);
  ret = initialize_slots (append (CLASSINSLOTDS (class), CLASSSLOTDS (class)),
			  *initializers);
  INSTSLOTS (obj) = (Object *) (VALUESELS (ret)[0]);
  *initializers = VALUESELS (ret)[1];

  return (obj);
}

static Object
make_class_driver (Object args)
{
  Object supers_obj, slots_obj, debug_obj, abstract_obj;
  Object obj;

  supers_obj = object_class;
  slots_obj = make_empty_list ();
  debug_obj = NULL;
  abstract_obj = MARLAIS_FALSE;

  while (!EMPTYLISTP (args)) {
    if (FIRST (args) == super_classes_keyword) {
      supers_obj = SECOND (args);
    } else if (FIRST (args) == slots_keyword) {
      slots_obj = marlais_make_slot_descriptor_list (SECOND (args), 0);
    } else if (FIRST (args) == debug_name_keyword) {
      debug_obj = SECOND (args);
    } else if (FIRST (args) == abstract_p_keyword) {
      abstract_obj = SECOND (args);
    } else {
      marlais_error ("make: unsupported keyword for <class> class", FIRST (args), NULL);
    }
    args = CDR (CDR (args));
  }
  if (!debug_obj) {
    marlais_warning ("make <class> no debug-name specified", NULL);
    debug_obj = empty_string;
  } else if (!BYTESTRP (debug_obj)) {
    marlais_error ("make <class> debug-name: must be a string", NULL);
  }
  if (EMPTYLISTP (supers_obj)) {
    supers_obj = object_class;
  }
  obj = marlais_allocate_object (Class, sizeof (struct clas));

  CLASSNAME (obj) = make_symbol (BYTESTRVAL (debug_obj));
  CLASSPROPS (obj) |= CLASSSLOTSUNINIT;
  return marlais_make_class (obj, supers_obj, slots_obj, abstract_obj, debug_obj);
}

/*
 * initialize_slots (slot_descriptors, initializers)
 *
 * Given
 *  i) a list of slot descriptors for a particular object class, and
 *  ii) a keyword-value association list of initializers
 *
 * Return a 2 element value object with elements
 *  i) a newly initialized vector of bindings representing the appropriately
 *     initialized slots, and
 *  ii) a keyword-value association list of initializers for the object
 *      including pairs for keyword initializable slots with init-values
 *      that were not listed in initializers
 */
static Object
initialize_slots (Object slot_descriptors, Object initializers)
{
  int i;
  Object slotd, init_slotds, tmp_slotds;
  Object *slots;
  Object default_initializers, initializer, *def_ptr;
  Object extra_initializers = make_empty_list ();
  Object extra;

  /* create defaulted initialization arguments */

  /* Create a copy (init_slotds) of the slot descriptors for this object
   * and fill in the init values with the appropriate values as
   * specified by keywords.
   */

  /* Note that we reverse the initializers list of keyword-value pairs
   * so they get the right binding if there are duplicates.
   */
  initializers = pair_list_reverse (initializers);

  if (PAIRP (initializers)) {
    init_slotds = copy_list (slot_descriptors);
    while (!EMPTYLISTP (initializers)) {
      initializer = CAR (initializers);
      if (KEYWORDP (initializer) && !EMPTYLISTP (CDR (initializers))) {
	extra = replace_slotd_init (init_slotds,
				    initializer,
				    SECOND (initializers));
	if (extra != NULL) {
	  extra_initializers = append (extra, extra_initializers);
	}
      } else {
	/* Should check for class or subclass initializer and
	 * take appropriate action.  Perhaps memoize the init
	 * and perform below.
	 */
	marlais_error ("Bad slot initializers", initializer, NULL);
      }
      initializers = CDR (CDR (initializers));
    }
  } else {
    init_slotds = copy_list (slot_descriptors);
  }

  default_initializers = make_empty_list ();
  def_ptr = &default_initializers;

  /*
   * Turn the list of modified slot descriptors (init_slotds)
   * into the corresponding key-value association list (default_initializers)
   * that may be passed to initialize.
   */
  for (tmp_slotds = init_slotds;
       !EMPTYLISTP (tmp_slotds);
       tmp_slotds = CDR (tmp_slotds)) {
    slotd = CAR (tmp_slotds);
    if (SLOTDINITKEYWORD (slotd)) {
      if (SLOTDINIT (slotd) != uninit_slot_object) {
	*def_ptr = listem (SLOTDINITKEYWORD (slotd),
			   SLOTDINIT (slotd),
			   NULL);
	def_ptr = &CDR (CDR (*def_ptr));
      } else if (SLOTDKEYREQ (slotd)) {
	marlais_error ("Required keyword not specified",
	       SLOTDINITKEYWORD (slotd), NULL);
      }
    }
  }

  /*
   * Create a vector of slot values (slots)
   * from the list of modified slot descriptors (init_slotds)
   */
  slots = (Object *) marlais_allocate_memory (list_length (init_slotds) *
				      sizeof (Object));

  tmp_slotds = init_slotds;
  for (i = 0; PAIRP (tmp_slotds); tmp_slotds = CDR (tmp_slotds), i++) {
    slotd = CAR (tmp_slotds);
    slots[i] = listem (marlais_slot_init_value (slotd), SLOTDSLOTTYPE (slotd), NULL);
  }
  return marlais_construct_values (2, slots, append (default_initializers,
						     extra_initializers));
}

static void
add_slot_descriptor_names (Object sd_list, Object *sg_names_ptr)
{
  Object sd;

  while (!EMPTYLISTP (sd_list)) {
    sd = CAR (sd_list);
    if (SLOTDSETTER (sd) != MARLAIS_FALSE) {
      if (member_2 (SLOTDGETTER (sd), SLOTDSETTER (sd), *sg_names_ptr)) {
	marlais_error ("slot getter or setter appears in superclass", sd, NULL);
      }
    } else {
      if (member (SLOTDGETTER (sd), *sg_names_ptr))
	marlais_error ("slot getter appears in superclass", sd, NULL);
    }
  }
}

static void
append_slot_descriptors (Object sd_list, Object **new_sd_list_insert_ptr,
			 Object *sg_names_ptr)
{
  while (!EMPTYLISTP (sd_list)) {
    append_one_slot_descriptor (CAR (sd_list),
				new_sd_list_insert_ptr,
				sg_names_ptr);
    sd_list = CDR (sd_list);
  }
}

/*
 * Given a slot descriptor (sd),
 * a pointer to the tail insertion point in a new slot descriptor list
 *  (new_sd_list_insert_ptr),
 * a pointer to a setter-getter names list (sg_names_ptr),
 *
 * This checks the setter and getter of sd for appearance
 * in the sg_names_ptr list.  If either appears already, that's an error.
 *
 * It inserts the slot descriptor in sd_list into the new slot descriptor
 * list (at the end) and updates the tail insertion point appropriately.
 */
static void
append_one_slot_descriptor (Object sd, Object **new_sd_list_insert_ptr,
			    Object *sg_names_ptr)
{
  if (member_2 (SLOTDGETTER (sd), SLOTDSETTER (sd), *sg_names_ptr)) {
    marlais_error ("slot getter or setter appears in superclass", sd, NULL);
  }
  *sg_names_ptr = cons (SLOTDGETTER (sd), *sg_names_ptr);
  if (SLOTDSETTER (sd)) {
    *sg_names_ptr = cons (SLOTDSETTER (sd), *sg_names_ptr);
  }
  **new_sd_list_insert_ptr = cons (sd, **new_sd_list_insert_ptr);
  *new_sd_list_insert_ptr = &CDR (**new_sd_list_insert_ptr);
}

static Object
replace_slotd_init (Object init_slotds, Object keyword, Object init)
{
  Object slotd;
  Object new_slotd;

  while (PAIRP (init_slotds)) {
    slotd = CAR (init_slotds);

    if (SLOTDINITKEYWORD (slotd) == keyword) {
      new_slotd = marlais_allocate_object (SlotDescriptor, sizeof (struct slot_descriptor));

      CAR (init_slotds) = new_slotd;
      SLOTDPROPS (new_slotd) = SLOTDPROPS (slotd) & ~SLOTDINITFUNCTIONMASK;
      SLOTDGETTER (new_slotd) = SLOTDGETTER (slotd);
      SLOTDSETTER (new_slotd) = SLOTDSETTER (slotd);
      SLOTDSLOTTYPE (new_slotd) = SLOTDSLOTTYPE (slotd);
      SLOTDINITKEYWORD (new_slotd) = SLOTDINITKEYWORD (slotd);
      SLOTDALLOCATION (new_slotd) = SLOTDALLOCATION (slotd);
      SLOTDDYNAMISM (new_slotd) = SLOTDDYNAMISM (slotd);

      SLOTDINIT (new_slotd) = init;
      return NULL;
    }
    init_slotds = CDR (init_slotds);
  }
/*
 *  If you get to here, the keyword did not match a slot init-keyword
 *  Return the list containing the keyword and initial value to
 *  signify this fact.
 */
  return listem (keyword, init, NULL);
}

static Object
pair_list_reverse (Object lst)
{
  Object result;

  result = make_empty_list ();
  while (PAIRP (lst) && PAIRP (CDR (lst))) {
    result = cons (CAR (lst), cons (SECOND (lst), result));
    lst = CDR (CDR (lst));
  }
  return result;
}

/*
 * Largely speculative.  Probably will change all around.
 */
static Object
make_limited_int_type (Object args)
{
  Object obj;

  obj = marlais_allocate_object (LimitedIntType, sizeof (struct limited_int_type));

  while (!EMPTYLISTP (args)) {
    if (FIRST (args) == min_keyword) {
      if (LIMINTHASMIN (obj)) {
	marlais_error ("Minimum value for limited type specified twice", NULL);
      } else {
	LIMINTMIN (obj) = INTVAL (SECOND (args));
	LIMINTPROPS (obj) |= LIMMINMASK;
      }
    } else if (FIRST (args) == max_keyword) {
      if (LIMINTHASMAX (obj)) {
	marlais_error ("Maximum value for limited type specified twice", NULL);
      } else {
	LIMINTMAX (obj) = INTVAL (SECOND (args));
	LIMINTPROPS (obj) |= LIMMAXMASK;
      }
    } else {
      marlais_error ("make: unsupported keyword for limited integer type",
	     FIRST (args), NULL);
    }
    args = CDR (CDR (args));
  }

  return (obj);
}

static void
initialize_slotds (Object class)
{
  struct frame *old_env = the_env;
  Object superclasses;

  if (!CLASSUNINITIALIZED (class))
    return;

    /*
     * Check initialization status of superclasses.
     * This may seem odd, but sometimes, a superclass may not have been
     * initialized the first time a subclass object is created.
     * (e.g. it might be abstract)
     */
  for (superclasses = CLASSSUPERS (class);
       PAIRP (superclasses);
       superclasses = CDR (superclasses)) {
    if (CLASSUNINITIALIZED (CAR (superclasses))) {
      initialize_slotds (CAR (superclasses));
    }
  }

  the_env = CLASSENV (class);
  eval_slotds (CLASSSLOTDS (class));
  make_getters_setters (class, append (CLASSINSLOTDS (class),
				       CLASSSLOTDS (class)));

  eval_slotds (CLASSESSLOTDS (class));
  eval_slotds (CLASSCSLOTDS (class));
  make_getters_setters (class, append (CLASSCSLOTDS (class),
				       CLASSESSLOTDS (class)));

  make_getters_setters (class, CLASSCONSTSLOTDS (class));

  eval_slotds (CLASSVSLOTDS (class));
  make_getters_setters (class, CLASSVSLOTDS (class));
  CLASSPROPS (class) &= ~CLASSSLOTSUNINIT;
  the_env = old_env;
}

static void
eval_slotds (Object slotds)
{
  Object slotd;

  while (PAIRP (slotds)) {
    slotd = CAR (slotds);
    SLOTDSLOTTYPE (slotd) = eval (SLOTDSLOTTYPE (slotd));
    if (SLOTDDEFERREDTYPE (slotd)) {
      SLOTDSLOTTYPE (slotd) = marlais_apply_method (eval (SLOTDSLOTTYPE (slotd)),
						    make_empty_list (),
						    make_empty_list (),
						    NULL);
    }
    slotds = CDR (slotds);
  }
}

static Object
class_precedence_list (Object class)
{
  if (SEALEDP (class)) {
    return make_empty_list ();
  } else {
    return CLASSPRECLIST (class);
  }
}

static Object
class_debug_name (Object class)
{
  return CLASSNAME (class);
}

static int
member_2 (Object obj1, Object obj2, Object obj_list)
{
  while (PAIRP (obj_list)) {
    if (obj1 == CAR (obj_list) || obj2 == CAR (obj_list)) {
      return 1;
    }
    obj_list = CDR (obj_list);
  }
  return 0;
}

static Object
make_builtin_class (char *name, Object supers)
{
  Object obj;

  obj = marlais_allocate_object (Class, sizeof (struct clas));

  CLASSNAME (obj) = make_symbol (name);
  CLASSPROPS (obj) &= ~CLASSSLOTSUNINIT;
  add_top_level_binding (CLASSNAME (obj), obj, 1);
  return marlais_make_class (obj, supers, make_empty_list (), MARLAIS_FALSE, NULL);
}

static void
make_getters_setters (Object class, Object slotds)
{
  Object slotd;
  int slot_num = 0;

  while (!EMPTYLISTP (slotds)) {
    slotd = CAR (slotds);
    make_getter_method (slotd, class, slot_num);
    if (SLOTDALLOCATION (slotd) != constant_symbol) {
      make_setter_method (slotd, class, slot_num);
    }
    slotds = CDR (slotds);
    slot_num++;
  }
}

/*

  params = ((obj <class>))
  body = (slot-value obj 'slot)

 */
static Object
make_getter_method (Object slot, Object class, int slot_num)
{
  Object params, body, slot_location, allocation;
  Object class_location;

  if (!GFUNP (SLOTDGETTER (slot))) {
    marlais_error ("Slot getter is not a generic function", SLOTDGETTER (slot), NULL);
  }
  if (CLASSNAME (class)) {
    class_location = CLASSNAME (class);
  } else {
    class_location = listem (quote_symbol, class, NULL);
  }
  params = listem (listem (obj_sym, class_location, NULL), NULL);

  allocation = SLOTDALLOCATION (slot);
  if (allocation == instance_symbol) {
    slot_location = obj_sym;
  } else if (allocation == class_symbol ||
	     allocation == each_subclass_symbol) {
    slot_location = listem (class_slots_symbol,
			    listem (quote_symbol, class, NULL), NULL);
  } else if (allocation == virtual_symbol) {
    return SLOTDGETTER (slot);
  } else if (allocation != constant_symbol) {
    marlais_error ("Bad slot allocation ", allocation, NULL);
  }
  if (allocation == constant_symbol) {
    body = cons (SLOTDINIT (slot), make_empty_list ());
  } else {
    body = listem (listem (slot_val_sym,
			   slot_location,
			   marlais_make_integer (slot_num),
			   NULL),
		   NULL);
  }
  return (make_method (GFNAME (SLOTDGETTER (slot)),
		       params, body, the_env, 1));
}

/*

   params = ((obj <class>) val)
   body = (set-slot-value! obj 'slot val)

 */
static Object
make_setter_method (Object slot, Object class, int slot_num)
{
  Object params, body, slot_location, allocation;
  Object class_location;

  if (NULL == SLOTDSETTER (slot) || MARLAIS_FALSE == SLOTDSETTER (slot)) {
    return NULL;
  }
  if (!GFUNP (SLOTDSETTER (slot))) {
    marlais_error ("Slot setter is not a generic function",
	   SLOTDSETTER (slot),
	   NULL);
  }
  if (CLASSNAME (class)) {
    class_location = CLASSNAME (class);
  } else {
    class_location = listem (quote_symbol, class, NULL);
  }
  params = listem (listem (val_sym,
			   listem (quote_symbol,
				   SLOTDSLOTTYPE (slot),
				   NULL),
			   NULL),
		   listem (obj_sym, class_location, NULL),
		   NULL);
  allocation = SLOTDALLOCATION (slot);
  if (allocation == instance_symbol) {
    slot_location = obj_sym;
  } else if (allocation == class_symbol ||
	     allocation == each_subclass_symbol) {
    slot_location = listem (class_slots_symbol,
			    listem (quote_symbol, class, NULL),
				NULL);
  } else if (allocation == constant_symbol) {
    marlais_error ("BUG - attempt to allocate setter for constant slot",
	   slot, NULL);
  } else if (allocation == virtual_symbol) {
    return SLOTDSETTER (slot);
  } else {
    marlais_error ("Bad slot allocation ", allocation, NULL);
  }
  body = listem (listem (set_slot_value_sym,
			 slot_location,
			 marlais_make_integer (slot_num),
			 val_sym,
			 NULL),
		 NULL);
  return (make_method (GFNAME (SLOTDSETTER (slot)),
		       params, body, the_env, 1));
}

static Object
merge_sorted_precedence_lists (Object class, Object supers)
{
  Object new_list;

  /*
   * copying of supers is not strictly relied upon, but tail sharing
   * in merged lists occurs below.
   */
  if (!EMPTYLISTP (supers)) {
    new_list = CLASSSORTEDPRECS (CAR (supers));
    supers = CDR (supers);
  } else {
    new_list = make_empty_list ();
    supers = make_empty_list ();
  }

  while (!EMPTYLISTP (supers)) {
    new_list = merge_class_lists (new_list,
				  CLASSSORTEDPRECS (CAR (supers)));
    supers = CDR (supers);
  }
  return merge_class_lists (new_list, cons (class, make_empty_list ()));
}

static Object
merge_class_lists (Object left, Object right)
{
  Object new_list;
  Object *new_list_ptr;

  new_list_ptr = &new_list;

  while (!EMPTYLISTP (left) && !EMPTYLISTP (right)) {
    if (CLASSINDEX (CAR (left)) < CLASSINDEX (CAR (right))) {
      *new_list_ptr = cons (CAR (left), make_empty_list ());
      left = CDR (left);
      new_list_ptr = &CDR (*new_list_ptr);
    } else if (CLASSINDEX (CAR (left)) == CLASSINDEX (CAR (right))) {
      *new_list_ptr = cons (CAR (left), make_empty_list ());
      left = CDR (left);
      right = CDR (right);
      new_list_ptr = &CDR (*new_list_ptr);
    } else {
      *new_list_ptr = cons (CAR (right), make_empty_list ());
      right = CDR (right);
      new_list_ptr = &CDR (*new_list_ptr);
    }
  }

  if (!EMPTYLISTP (left)) {
    *new_list_ptr = left;
  }
  if (!EMPTYLISTP (right)) {
    *new_list_ptr = right;
  }
  return new_list;
}
