/* env.c -- see COPYRIGHT for use */

#include <marlais/env.h>

#include <marlais/alloc.h>
#include <marlais/string.h>
#include <marlais/class.h>
#include <marlais/eval.h>
#include <marlais/function.h>
#include <marlais/prim.h>
#include <marlais/print.h>
#include <marlais/stream.h>
#include <marlais/table.h>

/* Local constants */

/* Chunking size for single-binding frames */
#define BINDING_ALLOC_CHUNK 8

/* Exported functions */

static struct environment *
marlais_make_environment (int size, Object owner) {
  struct environment *frame;
  struct binding **bindings = NULL;

  if(size > 0) {
    bindings = marlais_allocate_memory (size * sizeof (struct binding *));
  }

  frame = MARLAIS_ALLOCATE_OBJECT (Environment, struct environment);
  frame->size = size;
  frame->owner = owner;
  frame->bindings = bindings;
  frame->next = NULL;
  frame->top_level_env = NULL;

  return (Object)frame;
}

Object
marlais_make_toplevel (Object owner)
{
  struct environment *frame;

  /* make a new frame */
  frame = marlais_make_environment (TOP_LEVEL_SIZE, owner);

  /* toplevel frames reference their own bindings */
  frame->top_level_env = frame->bindings;

  /* return it */
  return frame;
}

void
marlais_push_scope (Object owner)
{
  struct environment *frame;

  /* make a new frame with no bindings */
  frame = marlais_make_environment (0, owner);

  /* reference the toplevel environment of the parent */
  frame->top_level_env = the_env->top_level_env;

  /* push it on the stack */
  frame->next = the_env;
  the_env = frame;
  eval_stack->frame = frame;
}

void
marlais_pop_scope (void)
{
  the_env = the_env->next;
}

void
marlais_add_locals (Object syms, Object vals, int constant, struct environment *to_frame)
{
  struct environment *frame;
  struct binding *binding;
  int num_bindings, i;
  Object sym_list;

  sym_list = syms;
  num_bindings = 0;
  while (!EMPTYLISTP (sym_list)) {
    num_bindings++;
    sym_list = CDR (sym_list);
  }

  frame = to_frame;

  frame->bindings = (struct binding **)
    marlais_reallocate_memory (frame->bindings,
                               (frame->size + num_bindings) * sizeof(struct binding *));

  for (i = 0; i < num_bindings; ++i) {
    if ((!syms) || (!vals)) {
      marlais_error ("mismatched number of symbols and values", NULL);
    }
    binding = MARLAIS_ALLOCATE_STRUCT (struct binding);
    binding->sym = CAR (syms);
    /* ??? */
    binding->type = object_class;
    binding->val = (Object *) marlais_allocate_memory (sizeof (Object *));

    *(binding->val) = CAR (vals);

    binding->props &= !IMPORTED_BINDING;
    /* Just for now */
    binding->props |= EXPORTED_BINDING;
    if (constant) {
      binding->props |= CONSTANT_BINDING;
    }
    frame->bindings[i + frame->size] = binding;

    syms = CDR (syms);
    vals = CDR (vals);
  }
  frame->size += num_bindings;
}

void
marlais_add_local (Object sym, Object val, int constant, struct environment *to_frame)
{
  struct environment *frame;
  struct binding *binding;

  binding = MARLAIS_ALLOCATE_STRUCT (struct binding);
  if (PAIRP (sym)) {
    binding->sym = CAR (sym);
    binding->type = marlais_eval (SECOND (sym));
  } else {
    binding->sym = sym;
    binding->type = object_class;
  }
  binding->val = (Object *) marlais_allocate_memory (sizeof (Object *));

  if (!marlais_instance_p (val, binding->type)) {
    marlais_error ("add_local: value does not satisfy type constraint",
                   val,
                   binding->type,
                   NULL);
  }
  *(binding->val) = val;
  binding->props &= !IMPORTED_BINDING;
  /* Just for now */
  binding->props |= EXPORTED_BINDING;
  if (constant) {
    binding->props |= CONSTANT_BINDING;
  }
  frame = to_frame;

  if ((frame->size % BINDING_ALLOC_CHUNK) == 0) {
    frame->bindings = (struct binding **)
      marlais_reallocate_memory (frame->bindings,
                                 (frame->size + BINDING_ALLOC_CHUNK) * sizeof (struct binding *));
  }
  frame->bindings[frame->size] = binding;
  frame->size++;
}

Object
marlais_symbol_value (Object sym)
{
  struct binding *binding;

  binding = marlais_symbol_binding (sym);
  if (!binding) {
    return (NULL);
  }
  return (*(binding->val));
}

void
marlais_modify_value (Object sym, Object new_val)
{
  struct binding *binding;

  binding = marlais_symbol_binding (sym);
  if (!binding) {
    marlais_error ("attempt to modify value of unbound symbol", sym, NULL);
  } else if (IS_CONSTANT_BINDING (binding)) {
    marlais_error ("attempt to modify value of a constant", sym, NULL);
  } else if (marlais_instance_p (new_val, binding->type)) {
    *(binding->val) = new_val;
  } else {
    marlais_error ("attempt to assign variable an incompatible object",
                   sym, new_val, NULL);
  }
}

/* made symbol_binding non local to be able to fix <object> binding */
struct binding *
marlais_symbol_binding (Object sym)
{
  struct environment *frame;
  struct binding *binding;
  int i;

  frame = the_env;
  while (frame->bindings != frame->top_level_env) {
    for (i = 0; i < frame->size; ++i) {
      binding = frame->bindings[i];
      if (binding->sym == sym) {
        return (binding);
      }
    }
    frame = frame->next;
    if (!frame)
      break; /* <pcb> I/'ve observed this to be nil in a special case. */
  }
  /* can't find binding in frames, look at top_level */
  return (marlais_symbol_binding_top_level (sym));
}

struct binding *
marlais_symbol_binding_top_level (Object sym)
{
  struct binding *binding;
  int h, i;
  char *str;

  i = h = 0;
  str = SYMBOLNAME (sym);
  while (str[i]) {
    h += str[i++];
  }
  h = h % TOP_LEVEL_SIZE;

  binding = the_env->top_level_env[h];
  while (binding) {
    if (binding->sym == sym) {
      return (binding);
    }
    binding = binding->next;
  }
  return (NULL);
}

/* Internal functions */

/* Unwind the eval stack until we reach a context having a frame
 * with exit_sym as its only binding.  Perform unwind-protect
 * cleanups when we find them.
 */
int
unwind_to_exit (Object exit_proc)
{
  struct environment *frame;
  Object body;
  struct eval_stack *tmp_eval_stack, *save_eval_stack;

  /* pop the current frame off the stack.  It can't be right. */
  save_eval_stack = eval_stack;
  tmp_eval_stack = eval_stack->next;

  while (tmp_eval_stack) {
    frame = tmp_eval_stack->frame;
    if (frame->bindings) {
      if (frame->bindings[0] == EXITBINDING (exit_proc)) {
        the_env = tmp_eval_stack->frame;
        eval_stack = tmp_eval_stack;
        return 1;
      }
      if (tmp_eval_stack->context == unwind_protect_symbol) {
        body = UNWINDBODY (*(frame->bindings[0]->val));
        the_env = tmp_eval_stack->frame;
        while (!EMPTYLISTP (body)) {
          marlais_eval (CAR (body));
          body = CDR (body);
        }
      }
    }
    save_eval_stack = tmp_eval_stack;
    tmp_eval_stack = tmp_eval_stack->next;
  }

  the_env = save_eval_stack->frame;
  eval_stack = save_eval_stack;
  marlais_error ("unwound to end of stack without finding exit context",
                 exit_proc,
                 NULL);
  return 0;
}
