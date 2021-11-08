/* error.c -- see COPYRIGHT for use */

#include <marlais/error.h>

#include <marlais/alloc.h>
#include <marlais/apply.h>
#include <marlais/bytestring.h>
#include <marlais/class.h>
#include <marlais/env.h>
#include <marlais/eval.h>
#include <marlais/list.h>
#include <marlais/number.h>
#include <marlais/parser.h>
#include <marlais/prim.h>
#include <marlais/print.h>
#include <marlais/read.h>
#include <marlais/stream.h>

#include <signal.h>

extern Object print_symbol;
extern char* prompt;
extern char* current_prompt;
extern int sequence_num;

static char prompt_buf[20];

#define NUMSIGNALS 32
#define IGNORE  0
#define ERROR   1
#define DEFAULT 2

#if (!defined __NetBSD__) && (!defined __linux__) && (!defined _HP_UX) && (!defined __hpux) && (!defined _WIN32) && (!defined __APPLE__)
extern char *sys_siglist[];

#else
#if (defined _HP_UX) || (defined __hpux) || defined (_WIN32)
char *sys_siglist[32] =
    {"",
     "hangup",
     "interrupt",
     "quit",
     "illegal instruction (not reset when caught)",
     "trace trap (not reset when caught)",
     "IOT instruction",
     "EMT instruction",
     "floating point exception",
     "kill (cannot be caught or ignored)",
     "bus error",
     "segmentation violation",
     "bad argument to system call",
     "write on a pipe with no one to read it",
     "alarm clock",
     "software termination signal from kill",
     "urgent condition on IO channel",
     "sendable stop signal not from tty",
     "stop signal from tty",
     "continue a stopped process",
     "to parent on child stop or exit",
     "to readers pgrp upon background tty read",
     "like TTIN for output if (tp->t_local&LTOSTOP)",
     "input/output possible signal",
     "exceeded CPU time limit",
     "exceeded file size limit",
     "virtual time alarm",
     "profiling time alarm",
     "window changed",
     "resource lost (eg, record-lock lost)",
     "user defined signal 1",
     "user defined signal 2",
    };

#endif
#endif
extern Object signal_symbol;
extern Object simple_error_class;

int signal_response[32] =
    {IGNORE,
     ERROR,				/* hangup */
     DEFAULT,			/* interrupt */
     DEFAULT,			/* quit */
     DEFAULT,		/* illegal instruction (not reset when caught) */
     DEFAULT,			/* trace trap (not reset when caught) */
     DEFAULT,			/* IOT instruction */
     DEFAULT,			/* EMT instruction */
     ERROR,				/* floating point exception */
     IGNORE,			/* kill (cannot be caught or ignored) */
     DEFAULT,			/* bus error */
     DEFAULT,			/* segmentation violation */
     DEFAULT,			/* bad argument to system call */
     ERROR,			/* write on a pipe with no one to read it */
     IGNORE,			/* alarm clock */
     ERROR,			/* software termination signal from kill */
     DEFAULT,			/* urgent condition on IO channel */
     DEFAULT,			/* sendable stop signal not from tty */
     DEFAULT,			/* stop signal from tty */
     DEFAULT,			/* continue a stopped process */
     DEFAULT,			/* to parent on child stop or exit */
     DEFAULT,			/* to readers pgrp upon background tty read */
     DEFAULT,		/* like TTIN for output if (tp->t_local&LTOSTOP) */
     ERROR,				/* input/output possible signal */
     ERROR,				/* exceeded CPU time limit */
     ERROR,				/* exceeded file size limit */
     ERROR,				/* virtual time alarm */
     ERROR,				/* profiling time alar */
     DEFAULT,				/* window changed */
     DEFAULT,			/* resource lost (eg, record-lock lost) */
     ERROR,				/* user defined signal 1 */
     ERROR				/* user defined signal 2 */
    };

struct jmp_buf_stack {
    jmp_buf buf;
    struct jmp_buf_stack *next;
};

struct jmp_buf_stack *error_ok_return = 0;

static jmp_buf *error_ok_return_pop (void);
static jmp_buf *error_ok_return_push (void);
int num_debug_contexts;
int NoDebug;

static void signal_handler (int sig);
static Object my_print_env (void);

/* primitives */

static void signal_handler_init (void);

Object print_env_symbol;
Object print_stack_symbol;
Object show_bindings_symbol;
Object help_symbol;
Object return_symbol;
Object fail_symbol;
Object debugger_symbol;

static Object return_value (Object args);
static Object fail_function (void);
static Object help_function (void);

static Object dylan_error (Object msg_str, Object rest);
static Object dylan_warning (Object msg_str, Object rest);
static Object signal_error_jump ();
static Object enter_debugger (void);

static struct primitive error_prims[] =
{
  {"%error", prim_1_rest, dylan_error},
  {"%warning", prim_1_rest, dylan_warning},
  {"%signal-error-jump", prim_0, signal_error_jump},
  {"%debugger", prim_0, enter_debugger},
};

/* Exported functions */

void
marlais_register_error (void)
{
  int num;

  print_env_symbol = make_symbol ("print-env");
  print_stack_symbol = make_symbol ("print-stack");
  show_bindings_symbol = make_symbol ("show-bindings");
  help_symbol = make_symbol ("help");
  return_symbol = make_symbol ("return");
  fail_symbol = make_symbol ("fail");
  debugger_symbol = make_symbol ("<<Debugger>>");

  num = sizeof (error_prims) / sizeof (struct primitive);
  marlais_register_prims (num, error_prims);
  signal_handler_init ();
}

static void
signal_handler_init ()
{
  int i;

  for (i = 0; i < NUMSIGNALS; i++) {
    switch (signal_response[i]) {
    case IGNORE:
      signal (i, SIG_IGN);
      break;
    case ERROR:
      signal (i, signal_handler);
      break;
    case DEFAULT:
      ;
    }
  }
}

static void
print_dylan_error_helper(const char* kind, Object msg_str, Object rest)
{
  fprintf (stderr, "%s: %s", kind, BYTESTRVAL (msg_str));
  if (!EMPTYLISTP (rest)) {
    fprintf (stderr, ": ");
  }
  while (!EMPTYLISTP (rest)) {
    marlais_print_object (marlais_standard_error, CAR (rest), 0);
    rest = CDR (rest);
    if (!EMPTYLISTP (rest)) {
      fprintf (stderr, ", ");
    }
  }
  fprintf (stderr, ".\n");
}

static Object
dylan_error (Object msg_str, Object rest)
{
  print_dylan_error_helper("error", msg_str, rest);
  longjmp (error_return, 1);
}

static Object
dylan_warning (Object msg_str, Object rest)
{
  print_dylan_error_helper("warning", msg_str, rest);
  return unspecified_object;
}

void
marlais_fatal (char *msg)
{
  fprintf (stderr, "%s.\n", msg);
  abort();
}

Object
marlais_error (char *msg,...)
{
  va_list args;
  Object obj, signal_value, ret;
  jmp_buf *jmp_buf_ptr;
  static int message_printed = 0;

  va_start (args, msg);
  fprintf (stderr, "error: %s", msg);
  obj = va_arg (args, Object);

  if (obj) {
    fprintf (stderr, ": ");
  }
  while (obj) {
    marlais_print_object (marlais_standard_error, obj, 0);
    obj = va_arg (args, Object);

    if (obj) {
      fprintf (stderr, ", ");
    }
  }
  fprintf (stderr, ".\n");

  jmp_buf_ptr = error_ok_return_push ();
  ret = (Object) setjmp (*jmp_buf_ptr);
  if (!NoDebug) {
    /* debugger eval loop */
    if (!ret) {
      push_scope (debugger_symbol);

      /* Put debugging functions this frame */
      add_binding (print_env_symbol,
		   marlais_make_primitive ("print-env", prim_0, my_print_env),
		   1,
		   the_env);
      add_binding (print_stack_symbol,
		   marlais_make_primitive ("print-stack", prim_0, print_stack),
		   1,
		   the_env);
      add_binding (show_bindings_symbol,
		   marlais_make_primitive ("show-bindings",
				   prim_0_rest,
				   show_bindings),
		   1,
		   the_env);
      add_binding (help_symbol,
		   marlais_make_primitive ("help",
				   prim_0_rest,
				   help_function),
		   1,
		   the_env);

      add_binding (return_symbol,
		   marlais_make_primitive ("return", prim_0_rest, return_value),
		   1,
		   the_env);
      add_binding (fail_symbol,
		   marlais_make_primitive ("fail", prim_0, fail_function),
		   1,
		   the_env);

      if (!message_printed) {
	help_function ();
	message_printed = 1;
      }
      yy_restart (stdin);
      prompt = prompt_buf;
      current_prompt = prompt;
      while ((obj = marlais_parse_object (stdin, 0)) && (obj != eof_object)) {
	obj = eval (obj);
	if (obj != unspecified_object) {
	  Object symbol;
	  char symbol_name[12];

	  snprintf (symbol_name, 12, "$%i", sequence_num);
	  symbol = make_symbol (symbol_name);
	  add_top_level_binding (symbol, obj, 1);
	  fprintf (stdout, " $%i = ", sequence_num);
	  sequence_num++;
	}
	if (object_type (obj) == Values) {
	  marlais_print_obj (marlais_standard_output, obj);
	  if (VALUESNUM (obj)) {
	    fprintf (stdout, "\n");
	  }
	} else {
	  marlais_apply (eval(print_symbol), listem(obj, marlais_standard_output, NULL));
	  fprintf (stdout, "\n");
	}
	current_prompt = prompt;
	fflush (stdout);
      }
      fprintf (stderr, "\n");
      prompt = "? ";
      pop_scope ();
      error_ok_return_pop ();
    } else {
      return ret;
    }
  }
  signal_value = symbol_value (signal_symbol);
  if (signal_value) {
    marlais_apply (signal_value,
	   cons (marlais_make (simple_error_class, make_empty_list ()),
		 make_empty_list ()));
  } else {
    longjmp (error_return, 1);
  }
}

static Object
signal_error_jump ()
{
  longjmp (error_return, 1);
}

static Object
enter_debugger (void)
{
  return marlais_error ("entering debugger", NULL);
}

Object
marlais_warning (char *msg,...)
{
  va_list args;
  Object obj;

  va_start (args, msg);
  fprintf (stderr, "warning: %s", msg);
  obj = va_arg (args, Object);

  if (obj) {
    fprintf (stderr, ": ");
  }
  while (obj) {
    marlais_print_object (marlais_standard_error, obj, 0);
    obj = va_arg (args, Object);

    if (obj) {
      fprintf (stderr, ", ");
    }
  }
  fprintf (stderr, ".\n");
  return unspecified_object;
}

static void
signal_handler (int sig)
{
#ifdef __SunOS_5__
  error ((char *) _sys_siglist[sig], NULL);
#else
  error (sys_siglist[sig], NULL);
#endif
}

static Object
return_value (Object args)
{
  jmp_buf *buf;

  if (list_length (args) != 1) {
    fprintf (stderr, "return: Requires one argument\n");
  }
  pop_scope ();
  pop_eval_stack ();
  buf = error_ok_return_pop ();
  longjmp (*buf, (int) (CAR (args)));
}

static Object
help_function (void)
{
  fprintf (stderr, "\n");
  fprintf (stderr, "** Debugger **\n\n");
  fprintf (stderr, "  debugging functions:\n\n");
  fprintf (stderr, "    print-stack () => () "
	   "// print numbered entries in the runtime stack\n");
  fprintf (stderr, "    print-env () => ()   "
	   "// print numbered frames in static environment\n");
  fprintf (stderr, "    show-bindings (frame-number :: <integer>) => ()\n");
  fprintf (stderr, "                         "
	   "// show variable bindings in specified frame\n");
  fprintf (stderr, "    return (value) => () "
	   "// return to error context with specified value\n");
  fprintf (stderr, "                         "
	   "// return will almost always fail at this time\n");
  fprintf (stderr, "    help() => ()         // print this message\n");
  fprintf (stderr, "    fail() => ()         "
	   "// or ^D returns to the read-eval-print loop\n");
  return unspecified_object;
}

static jmp_buf
*
error_ok_return_pop (void)
{
  jmp_buf *ret = &(error_ok_return->buf);

  num_debug_contexts--;
  error_ok_return = error_ok_return->next;

  if(num_debug_contexts) {
    snprintf(prompt_buf, 20, "Debug[%d]> ", num_debug_contexts);
    prompt = prompt_buf;
  } else {
    prompt = "? ";
  }
  return ret;
}

static jmp_buf *
error_ok_return_push ()
{
  struct jmp_buf_stack *tmp =
    (struct jmp_buf_stack *) marlais_allocate_memory (sizeof (struct jmp_buf_stack));

  num_debug_contexts++;
  snprintf(prompt_buf, 20, "Debug[%d]> ", num_debug_contexts);
  prompt = prompt_buf;
  tmp->next = error_ok_return;
  error_ok_return = tmp;
  return &(error_ok_return->buf);
}

static Object
fail_function (void)
{
  longjmp (error_return, 1);
}

static Object
my_print_env (void)
{
  return print_env (the_env);
}
