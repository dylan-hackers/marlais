/* slot.c -- see COPYRIGHT for use */

#include <marlais/slot.h>

#include <marlais/alloc.h>
#include <marlais/apply.h>
#include <marlais/class.h>
#include <marlais/eval.h>
#include <marlais/list.h>
#include <marlais/prim.h>
#include <marlais/vector.h>

/* Internal function declarations */

static Object find_keyword (Object keyword, Object lst);

/* Primitives */

static Object instance_slots (Object instance);
static Object class_slots (Object class);

static struct primitive slot_prims[] =
{
    {"slot-value", prim_2, marlais_slot_value},
    {"set-slot-value!", prim_3, marlais_set_slot_value},
    {"%instance-slots", prim_1, instance_slots},
    {"%class-slots", prim_1, class_slots},
};

/* Exported functions */

void
marlais_register_slot (void)
{
    int num;

    num = sizeof (slot_prims) / sizeof (struct primitive);

    marlais_register_prims (num, slot_prims);
}

Object
marlais_slot_name (Object slot)
{
  if (!PAIRP (slot)) {
    return (slot);
  } else {
    if (NAMEP (CAR (slot))) {
      return (CAR (slot));
    } else {
      marlais_error ("Slot has no name but needs one", slot, NULL);
      return NULL;
    }
  }
}

Object
marlais_slot_getter (Object slot)
{
  if (!PAIRP (slot)) {
    return (NULL);
  } else {
    return (find_keyword (getter_keyword, slot));
  }
}

Object
marlais_slot_setter (Object slot)
{
  if (!PAIRP (slot)) {
    return (NULL);
  } else {
    return (find_keyword (setter_keyword, slot));
  }
}

Object
marlais_slot_type (Object slot)
{
  if (!PAIRP (slot)) {
    return (NULL);
  } else {
    return (find_keyword (type_keyword, slot));
  }
}

Object
marlais_slot_init_value (Object slotd)
{
  if (SLOTDINITFUNCTION (slotd)) {
    return eval (cons (listem (quote_symbol, SLOTDINIT (slotd), NULL),
                       make_empty_list ()));
  } else {
    return SLOTDINIT (slotd);
  }
}

Object
marlais_slot_init_function (Object slot)
{
  if (!PAIRP (slot)) {
    return (NULL);
  } else {
    return (find_keyword (init_function_keyword, slot));
  }
}

Object
marlais_slot_init_keyword (Object slot)
{
  if (!PAIRP (slot)) {
    return (NULL);
  } else {
    return (find_keyword (init_keyword_keyword, slot));
  }
}

Object
marlais_slot_required_init_keyword (Object slot)
{
  if (!PAIRP (slot)) {
    return (NULL);
  } else {
    return (find_keyword (required_init_keyword_keyword, slot));
  }
}

Object
marlais_slot_allocation (Object slot)
{
  if (!PAIRP (slot)) {
    return (NULL);
  } else {
    return (find_keyword (allocation_keyword, slot));
  }
}

Object
marlais_slot_value (Object instance, Object slot_num)
{
    return CAR (INSTSLOTS (instance)[INTVAL (slot_num)]);
}

Object
marlais_set_slot_value (Object instance, Object slot_num, Object val)
{
    CAR (INSTSLOTS (instance)[INTVAL (slot_num)]) = val;
    return val;
}

Object
marlais_make_slot_descriptor (unsigned char properties, Object getter, Object setter,
                              Object type, Object init, Object init_keyword,
                              Object allocation, Object dynamism)
{
    Object obj;

    obj = marlais_allocate_object (SlotDescriptor, sizeof (struct slot_descriptor));

    SLOTDPROPS (obj) = properties;
    SLOTDGETTER (obj) = getter;
    SLOTDSETTER (obj) = setter;
    SLOTDSLOTTYPE (obj) = type;
    SLOTDINIT (obj) = init;
    SLOTDINITKEYWORD (obj) = init_keyword;
    SLOTDALLOCATION (obj) = allocation;
    SLOTDDYNAMISM (obj) = dynamism;

    return obj;
}

/* Internal functions */

static Object
instance_slots (Object instance)
{
    return (Object) (&INSTSLOTS (instance));
}

static Object
class_slots (Object class)
{
    return (Object) (CLASSCSLOTS (class));
}

static Object
find_keyword (Object keyword, Object lst)
{
  if (!LISTP (lst)) {
    return (NULL);
  }
  while (!EMPTYLISTP (lst)) {
    if (CAR (lst) == keyword && !EMPTYLISTP (CDR (lst))) {
      return SECOND (lst);
    }
    lst = CDR (lst);
  }
  return (NULL);
}
