
#include <marlais/debug.h>

#include <marlais/alloc.h>
#include <marlais/eval.h>
#include <marlais/parser.h>
#include <marlais/print.h>
#include <marlais/prim.h>

/* Internal data types */

struct jmp_buf_stack {
    jmp_buf buf;
    struct jmp_buf_stack *next;
};

/* Internal variables */

extern char* prompt;
extern char* current_prompt;
extern int sequence_num;

static int debug_depth;
static Object debug_results;
static char prompt_buf[20];

static struct jmp_buf_stack *error_ok_return = 0;

/* Local symbols */

static Object debugger_symbol;
static Object show_bindings_symbol;
static Object show_frames_symbol;
static Object show_stack_symbol;
static Object help_symbol;
static Object return_symbol;
static Object fail_symbol;

/* Internal functions */

static Object debug_main (void);

/* Debugger functions */

static Object debug_show_bindings (Object rest);
static Object debug_show_frames (void);
static Object debug_show_stack (void);
static Object debug_fail (void);
static Object debug_return (Object values);
static Object debug_help (void);

/* Primitives */

static Object prim_debugger (void);

static struct primitive debug_prims[] =
{
 {"debugger", prim_0, prim_debugger},
};

/* Exported functions */

void
marlais_initialize_debug(void)
{
  int num = sizeof (debug_prims) / sizeof (struct primitive);
  marlais_register_prims (num, debug_prims);

  debugger_symbol = marlais_make_name ("<<debugger>>");
  show_bindings_symbol = marlais_make_name ("show-bindings");
  show_frames_symbol = marlais_make_name ("show-frames");
  show_stack_symbol = marlais_make_name ("show-stack");
  help_symbol = marlais_make_name ("help");
  return_symbol = marlais_make_name ("return");
  fail_symbol = marlais_make_name ("fail");
}

Object
marlais_debugger(void)
{
  int res;
  Object ret;
  jmp_buf *jmp_buf_ptr;
  marlais_push_scope (debugger_symbol);
  jmp_buf_ptr = marlais_push_error ();
  res = setjmp (*jmp_buf_ptr);
  if (res) {
    ret = debug_results;
  } else {
    ret = debug_main();
    marlais_pop_error ();
  }
  marlais_pop_scope ();
  return ret;
}

jmp_buf *
marlais_push_error (void)
{
  struct jmp_buf_stack *tmp =
    (struct jmp_buf_stack *) marlais_allocate_memory (sizeof (struct jmp_buf_stack));

  debug_depth++;
  snprintf(prompt_buf, 20, "Debug[%d]> ", debug_depth);
  prompt = prompt_buf;
  tmp->next = error_ok_return;
  error_ok_return = tmp;
  return &(error_ok_return->buf);
}

jmp_buf *
marlais_pop_error (void)
{
  jmp_buf *ret = &(error_ok_return->buf);

  debug_depth--;
  error_ok_return = error_ok_return->next;

  if(debug_depth) {
    snprintf(prompt_buf, 20, "Debug[%d]> ", debug_depth);
    prompt = prompt_buf;
  } else {
    prompt = "? ";
  }
  return ret;
}

/* Primitives */

static Object
prim_debugger (void)
{
  return marlais_error ("entering debugger", NULL);
}

/* Internal functions */

static Object
debug_main(void)
{
  Object obj;
  static bool message_printed = 0;

  /* Put debugging functions this frame */
  marlais_add_local (show_bindings_symbol,
                     marlais_make_primitive ("show-bindings",
                                             prim_0_rest,
                                             debug_show_bindings),
                     1,
                     the_env);
  marlais_add_local (show_frames_symbol,
                     marlais_make_primitive ("show-frames",
                                             prim_0,
                                             debug_show_frames),
                     1,
                     the_env);
  marlais_add_local (show_stack_symbol,
                     marlais_make_primitive ("show-stack",
                                             prim_0,
                                             debug_show_stack),
                     1,
                     the_env);
  marlais_add_local (return_symbol,
                     marlais_make_primitive ("return",
                                             prim_0_rest,
                                             debug_return),
                     1,
                     the_env);
  marlais_add_local (fail_symbol,
                     marlais_make_primitive ("fail",
                                             prim_0,
                                             debug_fail),
                     1,
                     the_env);
  marlais_add_local (help_symbol,
                     marlais_make_primitive ("help",
                                             prim_0,
                                             debug_help),
                     1,
                     the_env);

  if (!message_printed) {
    debug_help ();
    message_printed = 1;
  }
  prompt = prompt_buf;
  current_prompt = prompt;
  marlais_parser_prepare_string("", 0);
  while ((obj = marlais_parse_object ()) && (obj != MARLAIS_EOF)) {
    obj = marlais_eval (obj);
    if (obj != MARLAIS_UNSPECIFIED) {
      Object symbol;
      char symbol_name[12];

      /* is this sequence num a global? */
      snprintf (symbol_name, 12, "$%i", sequence_num);
      symbol = marlais_make_name (symbol_name);
      // this is the current module or what?
      // shouldn't it be the user module?
      marlais_add_export (symbol, obj, 1);
      fprintf (stdout, " $%i = ", sequence_num);
      sequence_num++;
    }
    if (object_type (obj) == Values) {
      marlais_print_obj (marlais_standard_output, obj);
      if (VALUESNUM (obj)) {
        fprintf (stdout, "\n");
      }
    } else {
      marlais_apply (marlais_eval(print_symbol), marlais_make_list(obj, marlais_standard_output, NULL));
      fprintf (stdout, "\n");
    }
    current_prompt = prompt;
    fflush (stdout);
  }
  fprintf (stderr, "\n");
  prompt = "? ";

  return MARLAIS_UNSPECIFIED;
}

/* Debugger functions */

static Object
debug_help (void)
{
  fprintf (stderr, "\n");
  fprintf (stderr, "** Debugger **\n\n");
  fprintf (stderr, "  debugging functions:\n\n");
  fprintf (stderr, "    show-bindings (frame :: <integer>) => ()\n");
  fprintf (stderr, "                         "
           "// show variable bindings in specified frame\n");
  fprintf (stderr, "    show-frames () => () "
           "// print numbered frames in static environment\n");
  fprintf (stderr, "    show-stack () => ()  "
           "// print numbered entries in the runtime stack\n");
  fprintf (stderr, "    return (value) => () "
           "// return to error context with specified value\n");
  fprintf (stderr, "                         "
           "// return will almost always fail at this time\n");
  fprintf (stderr, "    fail() => ()         "
           "// or ^D returns to the read-eval-print loop\n");
  fprintf (stderr, "    help() => ()         // print this message\n");
  return MARLAIS_UNSPECIFIED;
}

static Object
debug_fail (void)
{
  longjmp (*marlais_error_jump, 1);
}

static Object
debug_return (Object args)
{
  jmp_buf *buf;

  marlais_pop_eval_stack ();
  buf = marlais_pop_error ();
  debug_results = marlais_values (args);
  longjmp (*buf, 1);
}

static Object
debug_show_bindings (Object args)
{
  struct environment *frame;
  int i;
  int slot;
  struct binding **bindings, *binding;
  int frame_number;

  if (marlais_list_length (args) != 1 || !INTEGERP (CAR (args))) {
    marlais_error ("show_bindings: requires a single <integer> argument", NULL);
  }
  frame_number = INTVAL (CAR (args));

  for (frame = the_env, i = frame_number;
       i > 0 && frame != NULL;
       frame = frame->next, i--) ;
  if (i != 0) {
    fprintf (stderr, "Frame number %d does not exist\n",
             frame_number);
  } else {
    fprintf (stderr, "** Bindings for frame %d [",
             frame_number);
    marlais_print_object (marlais_standard_error, frame->owner, 1);
    fprintf (stderr, "]\n");
    /*
     * Print the bindings in all the frame slots.
     */
    for (bindings = frame->bindings, slot = 0;
         slot < frame->size;
         slot++) {
      /*
       * Print the bindings in one slot
       */
      for (binding = frame->bindings[slot];
           binding != NULL;
           binding = binding->next) {
        fprintf (stderr, "   ");
        marlais_print_object (marlais_standard_error, binding->sym, 1);
        if (binding->type != object_class) {
          fprintf (stderr, " :: ");
          marlais_print_object (marlais_standard_error, binding->type, 1);
        }
        fprintf (stderr, " = ");
        marlais_print_object (marlais_standard_error, *(binding->val), 1);
        fprintf (stderr, "\n");
      }
    }

  }
  return MARLAIS_UNSPECIFIED;
}

static Object
debug_show_frames (void)
{
  struct environment *frame;
  int i;

  for (i = 0, frame = the_env; frame != NULL; frame = frame->next, i++) {
    fprintf (stdout, "#%d ", i);
    marlais_print_object(marlais_standard_output, frame->owner, 1);
    fprintf (stdout, "\n");
  }

  return MARLAIS_UNSPECIFIED;
}

static Object
debug_show_stack (void)
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
