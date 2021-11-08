/* read.c -- see COPYRIGHT for use */

/* so, like, this whole file is to be reworked */

#ifdef NO_COMMON_DYLAN_SPEC

#include <marlais/read.h>

#include "bytestring.h"
#include "character.h"
#include "dylan_lexer.h"
#include "dylan.tab.h"
#include "file.h"
#include "list.h"
#include "number.h"
#include "parse.h"
#include "prim.h"
#include "symbol.h"
#include "vector.h"

#include <stdio.h>
#include <ctype.h>

#ifdef MARLAIS_ENABLE_BIG_INTEGERS
#include "biginteger.h"
#endif

extern FILE *yyin;
extern Object yylval;

/* local function prototypes */

static Object infix_read_object (FILE * fp);
static Object infix_decode_token (int yychar, FILE * fp);

static Object read_wrapper (Object stream);
static Object read_char (Object stream);

/* primitives */

static struct primitive read_prims[] =
{
  {"%read", prim_0_1, read_wrapper},
  {"%read-char", prim_0_1, read_char},
};

/* function definitions */

#endif
void
marlais_register_read (void)
{
#ifdef NO_COMMON_DYLAN_SPEC
  int num = sizeof (read_prims) / sizeof (struct primitive);
  marlais_register_prims (num, read_prims);
#endif
}
#ifdef NO_COMMON_DYLAN_SPEC

static Object
infix_read_object (FILE * fp)
{
  int yychar;
  int old_load_file_context = load_file_context;

  load_file_context = 1;

  if (yyin != fp) {
    reset_parser (fp);
  }
  if ((yychar = yylex ()) < 0)
    yychar = 0;

  load_file_context = old_load_file_context;
  return infix_decode_token (yychar, fp);
}

static Object
infix_decode_token (int yychar, FILE * fp)
{
  switch (yychar) {
  case EOF_TOKEN:
    return (eof_object);
  case LITERAL:
  case STRING:
  case HASH_T:
  case HASH_F:
    return (yylval);
  case HASH_BRACKET:
    {
      Object new_list = make_empty_list ();
      Object *new_list_ptr = &new_list;

      do {
        if ((yychar = yylex ()) < 0)
          yychar = 0;
        if (yychar == ']')
          return new_list;

        *new_list_ptr = cons (infix_decode_token (yychar, fp),
                              make_empty_list ());
        new_list_ptr = &CDR (*new_list_ptr);

        if ((yychar = yylex ()) < 0)
          yychar = 0;
      } while (yychar == ',');

      if (yychar != ']') {
        return marlais_error ("Malformed vector.  Expected a ']'", NULL);
      }
      return marlais_make_sov (new_list);
    } /* case HASH_BRACKET */
  case HASH_PAREN:
    {
      Object new_list = make_empty_list ();
      Object *new_list_ptr = &new_list;

      do {
        if ((yychar = yylex ()) < 0)
          yychar = 0;
        if (yychar == ')')
          return new_list;

        *new_list_ptr = cons (infix_decode_token (yychar, fp),
                              make_empty_list ());
        new_list_ptr = &CDR (*new_list_ptr);

        if ((yychar = yylex ()) < 0)
          yychar = 0;
      } while (yychar == ',');

      if (yychar == '.') {
        *new_list_ptr = infix_read_object (fp);
        if ((yychar = yylex ()) < 0)
          yychar = 0;
      }
      if (yychar != ')') {
        return marlais_error ("Malformed list.  Expected a ')'", NULL);
      }
      return new_list;
    } /* case HASH_PAREN */
  case KEYWORD:
    return yylval;
  default:
    return marlais_error ("read couldn't find a literal", NULL);
  }
}

static Object
read_wrapper (Object stream)
{
  FILE *fp;

  if (stream) {
    fp = STREAMFP (stream);
  } else {
    fp = stdin;
  }
  return infix_read_object(fp);
}

static Object
read_char (Object stream)
{
  FILE *fp;
  int ch;

  if (stream) {
    fp = STREAMFP (stream);
  } else {
    fp = stdin;
  }
  ch = getc (fp);
  if (ch == EOF) {
    return (eof_object);
  } else {
    return (marlais_make_character (ch));
  }
}

#endif
