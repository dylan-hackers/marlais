
#include <marlais/signal.h>

#include <signal.h>

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

static void
signal_handler (int sig)
{
  marlais_fatal (sys_siglist[sig], NULL);
}

void
marlais_initialize_signal(void)
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
