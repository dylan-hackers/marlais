/*

   read.c

   This software is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this software; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Original copyright notice follows:

   Copyright, 1993, Brent Benson.  All Rights Reserved.
   0.4 & 0.5 Revisions Copyright 1994, Joseph N. Wilson.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include <stdio.h>
#include <ctype.h>
#ifdef sun
#include <floatingpoint.h>
#endif

#include "read.h"

#include "bytestring.h"
#include "character.h"
#include "dylan_lexer.h"
#include "dylan.tab.h"
#include "error.h"
#include "file.h"
#include "list.h"
#include "number.h"
#include "parse.h"
#include "prim.h"
#include "symbol.h"
#include "vector.h"

#ifdef BIG_INTEGERS
#include "biginteger.h"
#endif

extern FILE *yyin;
extern Object yylval;

/* local function prototypes */

static Object infix_read_object (FILE * fp);
static Object infix_decode_token (int yychar, FILE * fp);

static Object read_wrapper (Object stream);
static Object read_char (Object stream);

static Object read_list (FILE * fp);
static Object read_string (FILE * fp);
static Object read_quote (FILE * fp);
static Object read_vector (FILE * fp);
static Object read_number (FILE * fp);
static Object read_symbol (FILE * fp);
static Object read_character (FILE * fp);
static Object read_quasiquote (FILE * fp);
static Object read_unquote (FILE * fp);
static Object read_unquote_splicing (FILE * fp);

static void skip_whitespace_comments (FILE * fp);
static char peek_char (FILE * fp);
static int match_chars (FILE * fp, char *str);

/* primitives */

static struct primitive read_prims[] =
{
    {"%read", prim_0_1, read_wrapper},
    {"%read-char", prim_0_1, read_char},
};

/* function definitions */

void
init_read_prims (void)
{
    int num;

    num = sizeof (read_prims) / sizeof (struct primitive);

    init_prims (num, read_prims);
}

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
		return error ("Malformed vector.  Expected a ']'", NULL);
	    }
	    return make_sov (new_list);
	}			/* case HASH_BRACKET */
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
		return error ("Malformed list.  Expected a ')'", NULL);
	    }
	    return new_list;
	}			/* case HASH_PAREN */
    case KEYWORD:
	return yylval;
    default:
	return error ("read couldn't find a literal", NULL);
    }
}

Object
read_object (FILE * fp)
{
    signed char ch;

    if (!classic_syntax) {
	return infix_read_object (fp);
    }
  start_over:
    skip_whitespace_comments (fp);
    ch = getc (fp);
    switch (ch) {
    case EOF:
	return (eof_object);
    case ')':
	error ("read: unexpected ')'", NULL);
    case '(':
	return (read_list (fp));
    case '"':
	return (read_string (fp));
    case '\'':
	return (read_quote (fp));
    case '`':
	return (read_quasiquote (fp));
    case ',':
	if (peek_char (fp) == '@') {
	    ch = getc (fp);
	    return (read_unquote_splicing (fp));
	} else {
	    return (read_unquote (fp));
	}
    case ';':
	while ((ch = getc (fp)) != '\n') {
	    if (ch == EOF) {
		return (eof_object);
	    }
	}
	goto start_over;
	break;
    case '+':
    case '-':
	if (isdigit (peek_char (fp))) {
	    ungetc (ch, fp);
	    return (read_number (fp));
	} else {
	    ungetc (ch, fp);
	    return (read_symbol (fp));
	}
    case '#':
	ch = getc (fp);
	switch (ch) {
	case 'a':
	    if (((ch = getc (fp) != 'l') || ((ch = getc (fp)) != 'l')
		 || ((ch = getc (fp)) != '-') || ((ch = getc (fp)) != 'k')
		 || ((ch = getc (fp)) != 'e') || ((ch = getc (fp)) != 'y')
		 || ((ch = getc (fp)) != 's'))) {
		error ("read: regular symbol cannot begin with '#'", NULL);
	    }
	    return (allkeys_symbol);

	case 'v':
	    if (((ch = getc (fp)) != 'a') || ((ch = getc (fp)) != 'l')
		|| ((ch = getc (fp)) != 'u') || ((ch = getc (fp)) != 'e')
		|| ((ch = getc (fp)) != 's')) {
		error ("read: regular symbol cannot begin with '#'", NULL);
	    }
	    return (hash_values_symbol);
	case 'k':
	    if (((ch = getc (fp)) != 'e') || ((ch = getc (fp)) != 'y')) {
		error ("read: regular symbol cannot begin with `#'", NULL);
	    }
	    return (key_symbol);
	case 'r':
	    if (((ch = getc (fp)) != 'e') || ((ch = getc (fp)) != 's')
		|| ((ch = getc (fp)) != 't')) {
		error ("read: regular symbol cannot begin with `#'", NULL);
	    }
	    return (hash_rest_symbol);
	case 'n':
	    if (((ch = getc (fp)) != 'e') || ((ch = getc (fp)) != 'x')
		|| ((ch = getc (fp)) != 't')) {
		error ("read: regular symbol cannot begin with `#'", NULL);
	    }
	    return (next_symbol);
	case '(':
	    return (read_vector (fp));
	case '\\':
	    return (read_character (fp));
	case 't':
	    return (true_object);
	case 'f':
	    return (false_object);
	case 'x':
	case 'b':
	case 'o':
	    error ("read: hex, binary and octal number reading not supported", NULL);
	case '|':
	    do {
		ch = getc (fp);
		if (ch == EOF) {
		    error ("read: end of file in #| comment", NULL);
		}
		if ((ch == '|') && (peek_char (fp) == '#')) {
		    ch = getc (fp);
		    goto start_over;
		}
	    }
	    while (1);
	    break;
	default:
	    error ("read: unexpected `#'", NULL);
	}
    default:
	/* may need to look ahead one further if have a '.' here */
	if (isdigit (ch) || ch == '.') {
	    ungetc (ch, fp);
	    return (read_number (fp));
	} else {
	    ungetc (ch, fp);
	    return (read_symbol (fp));
	}
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
    return (read_object (fp));
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
	return (make_character (ch));
    }
}

/* "(" has already been read */
static Object
read_list (FILE * fp)
{
    Object car, cdr;
    char ch;

    skip_whitespace_comments (fp);
    ch = getc (fp);
    if (ch == ')')
	return (make_empty_list ());
    if (ch == EOF)
	error ("read: unexpected EOF while reading a list", NULL);
    ungetc (ch, fp);
    car = read_object (fp);
    skip_whitespace_comments (fp);
    ch = getc (fp);
    if (ch == ')') {
	cdr = make_empty_list ();
    } else if (ch == '.' && isspace (peek_char (fp))) {
	cdr = read_object (fp);
	skip_whitespace_comments (fp);
	ch = getc (fp);
	if (ch != ')') {
	    error ("read: malformed list", NULL);
	}
    } else {
	ungetc (ch, fp);
	cdr = read_list (fp);
    }

    return (cons (car, cdr));
}

/* '"' has already been read */
static Object
read_string (FILE * fp)
{
    char ch, buf[MAX_STRING_SIZE];
    int i;

    i = 0;
    while ((ch = getc (fp)) != '"') {
	if (i > MAX_STRING_SIZE) {
	    error ("read: string to long for reader", NULL);
	}
	if (ch == '\\') {
	    ch = getc (fp);
	}
	buf[i++] = ch;
    }
    buf[i] = '\0';
    return (make_byte_string (buf));
}

/* "'" has been read */
static Object
read_quote (FILE * fp)
{
    Object obj;

    obj = read_object (fp);
    return (cons (quote_symbol, cons (obj, make_empty_list ())));
}

/* "#(" has been read */
static Object
read_vector (FILE * fp)
{
    Object obj;

    obj = read_list (fp);
    return (make_sov (obj));
}

/* nothing has been read */
static Object
read_number (FILE * fp)
{
    char ch, buf[MAX_NUMBER_SIZE];
    int i, is_float, is_negative;

    i = 0;
    is_negative = is_float = 0;
    ch = getc (fp);
    if (ch == '+') {
	ch = getc (fp);
    } else if (ch == '-') {
	is_negative = 1;
	ch = getc (fp);
    }
    do {
	if (i > MAX_NUMBER_SIZE) {
	    error ("read: number too long for reader", NULL);
	}
	if (ch == '.') {
	    is_float = 1;
	}
	buf[i++] = ch;
    }
    while (isdigit (ch = getc (fp)) || (ch == '.'));
    ungetc (ch, fp);
    buf[i] = '\0';
    if (is_float) {
	double d;

	d = strtod (buf, NULL);
	if (is_negative) {
	    d = -d;
	}
	return (make_dfloat (d));
    } else {
#ifdef BIG_INTEGERS
	if (i >= 10) {
	    return make_big_integer_str (buf, 10);
	} else {
	    i = atoi (buf);
	    if (is_negative)
		i = -i;
	    return make_integer (i);
	}
#else
	int i;

	i = atoi (buf);
	if (is_negative) {
	    i = -i;
	}
	return (make_integer (i));
#endif
    }
}

/* nothing has been read */
static Object
read_symbol (FILE * fp)
{
    signed char ch, buf[MAX_SYMBOL_SIZE];
    int i;

    i = 0;
    while ((!isspace (ch = getc (fp)))
	   && (ch != '(')
	   && (ch != ')')
	   && (ch != '#')
	   && (ch != '\'')
	   && (ch != '`')) {
	if (ch == EOF) {
	    return (eof_object);
	}
	buf[i++] = ch;
    }
    ungetc (ch, fp);
    buf[i] = '\0';
    if (buf[i - 1] == ':') {
	return (make_keyword ((char *) &buf));
    } else {
	return (make_symbol ((char *) &buf));
    }
}

/* "#\" has been read */
/*
 * Really this is broken.  It only ignores case on the first char
 * of each symbolic character name.
 */
static Object
read_character (FILE * fp)
{
    char ch;

    ch = getc (fp);
    switch (ch) {
    case 'n':			/* maybe `newline' */
    case 'N':
	if (peek_char (fp) == 'e') {
	    if (!match_chars (fp, "ewline")) {
		error ("read: bad character constant", NULL);
	    } else {
		return (make_character ('\n'));
	    }
	} else {
	    return (make_character ('n'));
	}
    case 's':			/* maybe `space' */
    case 'S':
	if (peek_char (fp) == 'p') {
	    if (!match_chars (fp, "pace")) {
		error ("read: bad character constant", NULL);
	    } else {
		return (make_character (' '));
	    }
	} else {
	    return (make_character ('s'));
	}
    case 'r':			/* maybe `rubout' */
    case 'R':
	if (peek_char (fp) == 'u') {
	    if (!match_chars (fp, "ubout")) {
		error ("read: bad character constant", NULL);
	    } else {
		return (make_character (0x7f));
	    }
	} else if (peek_char (fp) == 'e') {
	    if (!match_chars (fp, "eturn")) {
		error ("read: bad character constant", NULL);
	    } else {
		return (make_character ('\r'));
	    }
	} else {
	    return (make_character ('r'));
	}
    case 'p':			/* maybe `page' */
    case 'P':
	if (peek_char (fp) == 'a') {
	    if (!match_chars (fp, "age")) {
		error ("read: bad character constant", NULL);
	    } else {
		return (make_character ('\f'));
	    }
	} else {
	    return (make_character ('p'));
	}
    case 't':			/* maybe `tab' */
    case 'T':
	if (peek_char (fp) == 'e') {
	    if (!match_chars (fp, "ab")) {
		error ("read: bad character constant", NULL);
	    } else {
		return (make_character ('\t'));
	    }
	} else {
	    return (make_character ('t'));
	}
    case 'b':			/* maybe `backspace' */
    case 'B':
	if (peek_char (fp) == 'e') {
	    if (!match_chars (fp, "ackspace")) {
		error ("read: bad character constant", NULL);
	    } else {
		return (make_character ('\b'));
	    }
	} else {
	    return (make_character ('b'));
	}
    case 'l':			/* maybe `linefeed' */
    case 'L':
	if (peek_char (fp) == 'e') {
	    if (!match_chars (fp, "inefeed")) {
		error ("read: bad character constant", NULL);
	    } else {
		return (make_character ('\n'));
	    }
	} else {
	    return (make_character ('l'));
	}
    default:
	return (make_character (ch));
    }
}

/* "`" has been read */
static Object
read_quasiquote (FILE * fp)
{
    Object quoted_obj, ret;

    quoted_obj = read_object (fp);
    ret = cons (quasiquote_symbol, cons (quoted_obj, make_empty_list ()));
    return (ret);
}

/* "," has been read */
static Object
read_unquote (FILE * fp)
{
    Object obj, ret;

    obj = read_object (fp);
    ret = cons (unquote_symbol, cons (obj, make_empty_list ()));
    return (ret);
}

/* ",@" has been read */
static Object
read_unquote_splicing (FILE * fp)
{
    Object obj, ret;

    obj = read_object (fp);
    ret = cons (unquote_splicing_symbol, cons (obj, make_empty_list ()));
    return (ret);
}

/* utilities */

static void
skip_whitespace_comments (FILE * fp)
{
    int ch;

  start_over:
    while (isspace (ch = getc (fp))) ;
    if (ch == ';') {
	while ((ch = getc (fp)) != '\n') ;
	goto start_over;
    }
    if ((ch == '#') && (peek_char (fp) == '|')) {
	do {
	    ch = getc (fp);
	    if (ch == EOF) {
		error ("read: end of file in #| comment", NULL);
	    }
	    if ((ch == '|') && (peek_char (fp) == '#')) {
		ch = getc (fp);
		goto start_over;
	    }
	}
	while (1);
    }
    ungetc (ch, fp);
}

static char
peek_char (FILE * fp)
{
    char ch;

    ch = getc (fp);
    ungetc (ch, fp);
    return (ch);
}

static int
match_chars (FILE * fp, char *str)
{
    int i;
    char ch;

    i = 0;
    while (str[i]) {
	ch = getc (fp);
	if (ch != str[i]) {
	    return (0);
	}
	i++;
    }
    return (1);
}
