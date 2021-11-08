%{ /*  Emacs: -*- Fundamental -*- */
/*   Lexical analyzer for Dylan interm report tokens
     see COPYRIGHT for use */

#include <marlais/common.h>

#include <marlais/alloc.h>
#include <marlais/bytestring.h>
#include <marlais/character.h>
#include <marlais/list.h>
#include <marlais/number.h>
#include <marlais/env.h>
#include <marlais/symbol.h>
#include <marlais/yystype.h>

#include "parser.gen.h"

#include <sys/time.h>

#ifdef MARLAIS_ENABLE_READLINE
#define _FUNCTION_DEF
#include <readline/readline.h>
#include <readline/history.h>
#endif

#define mydebug  marlais_yydebug
#define mylval   marlais_yylval

extern char* prompt_continuation;
extern char* current_prompt;
extern int load_file_context;

Object header_key;
Object header_val;

/* Internal types */

struct resword
{
	char *word;
	int token;
	Object *symbol;
};

/* #define NUM_COREWORDS 9 */
#define NUM_COREWORDS 8

struct resword coreword_table [NUM_COREWORDS] =
{
	{"define", DEFINE, &define_symbol},
	{"end", END, &end_symbol},
	{"generic", GENERIC, &generic_symbol},
	{"handler", HANDLER, &handler_symbol},
	{"let", LET, &let_symbol},
	{"local", LOCAL, &local_symbol},
	{"method", METHOD, &method_symbol},
	/* {"function", FUNCTION, &function_symbol},   DMA */
	{"otherwise", OTHERWISE, &otherwise_symbol},
};

#define NUM_BEGIN_WORDS	9

struct resword begin_word_table [NUM_BEGIN_WORDS] =
{
	{"begin", BEGIN_TOKEN, &begin_symbol},
	{"block", BLOCK, &block_symbol},
	{"case", CASE, &case_symbol},
	{"for", FOR, &for_symbol},
	{"if", IF, &if_symbol},
	{"select", SELECT, &select_symbol},
	{"unless", UNLESS, &unless_symbol},
	{"until", UNTIL, &until_symbol},
	{"while", WHILE, &while_symbol},
};

#define NUM_DEFINING_WORDS	5

struct resword defining_word_table [NUM_DEFINING_WORDS] =
{
	{"class", CLASS, &class_symbol},
	{"constant", CONSTANT, &constant_symbol},
	{"library", LIBRARY, &library_symbol},
	{"module", MODULE, &module_symbol},
	{"variable", VARIABLE, &variable_symbol},
};


/* intermediate word struct and tables */

struct intermediate_word_struct {
    int num_words;
    struct resword *reswords;
    struct intermediate_word_struct *next;
}
*intermediate_words;

#define NUM_IF_INTERMEDIATE_WORDS	2
struct resword if_intermediate_word_table [NUM_IF_INTERMEDIATE_WORDS] =
{
	{"else", ELSE, &else_symbol},
	{"elseif", ELSEIF, &elseif_symbol},
};

#define NUM_SELECT_INTERMEDIATE_WORDS	1
struct resword select_intermediate_word_table [NUM_SELECT_INTERMEDIATE_WORDS] =
{
	{"by", BY, &by_symbol},
};

#define NUM_CLASS_INTERMEDIATE_WORDS	1
struct resword class_intermediate_word_table [NUM_CLASS_INTERMEDIATE_WORDS] =
{
	{"slot", SLOT, &slot_symbol},
};

#define NUM_FOR_INTERMEDIATE_WORDS	1
struct resword for_intermediate_word_table [NUM_FOR_INTERMEDIATE_WORDS] =
{
	{"finally", FINALLY, &finally_symbol},
};

#define NUM_FOR_CLAUSE_WORDS		7
struct resword for_clause_word_table [NUM_FOR_CLAUSE_WORDS] =
{
    {"then", THEN, &then_symbol},
    {"in", IN, &in_symbol},
    {"from", FROM, &from_symbol},
    {"to", TO, &to_symbol},
    {"above", ABOVE, &above_symbol},
    {"below", BELOW, &below_symbol},
    {"by", BY, &by_symbol},
};

#define NUM_BLOCK_INTERMEDIATE_WORDS	3
struct resword block_intermediate_word_table [NUM_BLOCK_INTERMEDIATE_WORDS] =
{
	{"afterwards", AFTERWARDS, &afterwards_symbol},
	{"cleanup", CLEANUP, &cleanup_symbol},
	{"exception", EXCEPTION, &exception_symbol},
};

#define NUM_MODULE_INTERMEDIATE_WORDS	3
struct resword module_intermediate_word_table [NUM_MODULE_INTERMEDIATE_WORDS] =
{
	{"use", USE, &use_symbol},
	{"export", EXPORT, &export_symbol},
	{"create", CREATE, &create_symbol},
};

/* Internal function declarations */

static int search_for_poundword (char *string, YYSTYPE *obj_ptr);
static int symbol_or_resword (char *string, YYSTYPE *obj_ptr);
static struct resword *search_for_resword (char *string,
					   struct resword *resword_table,
					   int num_ords);
static int resword_compare (struct resword *c1, struct resword *c2);

static struct resword *search_intermediate_word (char *string);

static void process_unrecognized_character (char *yytext);
static int which_operator (char *string, int length);

static void make_header_key (void);
static void make_header_end (void);
static void make_header_val (void);

static char *get_nonws_symbol (char *text);
static char expand_escaped_character (char ch);

static Object make_expanded_byte_string (char *str);

static int countlines(char *str);

%}
%pointer

EXP		([Ee][\+\-]?[0-9][0-9]*)?
anyCHAR		[a-zA-Z0-9\!&\*\<=\>\|\^\$%@_\-\+~\?\/]
alphaCHAR	[a-zA-Z]
leadALPHA	[a-zA-Z]{anyCHAR}*
leadNUMERIC	[0-9]{anyCHAR}*
leadGRAPHIC	[!&\*<=>\|\^\$%@_]{anyCHAR}*
OPSYMS		[-~\+\*\/\^=\<\>&\|]|([\<\>=:~]=)|(~==)
SYMBOL		{leadALPHA}|({leadNUMERIC}[a-zA-Z]{leadALPHA})|([!&\*<=>\|\^\$%@_]{anyCHAR}*{leadALPHA})|(\${anyCHAR}*)
STRING		\"([^\\\"]|(\\(.|\n)))*\"

%x INI KEY VAL ETC
/* start contexts courtesy of Roger Critchlow */

%%

<INITIAL>.	{ BEGIN(INI);
		  yyless(0);
		}

<INITIAL,INI,KEY,VAL,ETC><<EOF>>	{ mylval = eof_object;
					  return EOF_TOKEN;
					}

<INI>.		{ BEGIN(ETC);
		  yyless(0);
		}

<INI>^#!.*\n	{ BEGIN(KEY);
		  /* warn(line_count, "ignoring initial #! interpreter comment\n"); */
		  /* line_count++; */
		  ++yylineno;
		}

<INI,KEY>^[A-Za-z][-A-Za-z0-9]*:	{ BEGIN(VAL);
				  make_header_key();
				  return yylex();
				}

<INI,KEY>^[\ \t\f\v\r]*\n	{ BEGIN(ETC);
			  ++yylineno;
			  make_header_end ();
			  return yylex ();
			}

<VAL>.*\n([\ \t\f\v\r]+.+\n)*	{ BEGIN(KEY);
				  yylineno += countlines (yytext);
				  make_header_val ();
				  return yylex ();
				}

<ETC>[\ \t\f\v\r]	{ }

<ETC>[\n]	{ /* Bogus hack! */
		  ++yylineno;
#if 0
		  if (yyerrflag == 3) {
		      return ';';
		  }
#endif
                }

<ETC>#b[01][01]*	{ /* binary integer */
		  mylval = marlais_make_integer (strtol (yytext+2, NULL, 2));
		  return (LITERAL);
		}

<ETC>#o[0-7][0-7]*	{ /* octal-integer */
		  mylval = marlais_make_integer (strtol (yytext+2, NULL, 8));
		  return (LITERAL);
		}

<ETC>[+-]?[0-9][0-9]*	{ /* decimal integer */
#ifdef BIG_INTEGERS
			if (strlen(yytext) >= 10)
				mylval = make_big_integer_str(yytext, 10);
			else
				mylval = marlais_make_integer (strtol (yytext, NULL, 10));
#else
    		  mylval = marlais_make_integer (strtol (yytext, NULL, 10));
#endif
		  return (LITERAL);
		}

<ETC>#x[0-9A-Fa-f][0-9A-Fa-f]* { /* hex-integer */
			    mylval = marlais_make_integer (strtol (yytext+2, NULL, 16));
			    return (LITERAL);
			  }

<ETC>[+-]?[0-9][0-9]*\/[0-9][0-9]*	{ /* ratio */
				  char *ptr;
				  long numerator, denominator;
				  numerator = strtol (yytext, &ptr, 10);
				  denominator = strtol (ptr + 1, NULL, 10);
				  mylval = marlais_make_ratio (numerator, denominator);
				  return (LITERAL);
				}

<ETC>[+-]?[0-9]*\.[0-9][0-9]*{EXP}	{ mylval = marlais_make_dfloat (strtod (yytext, NULL));
					  return (LITERAL);
					}
<ETC>[+-]?[0-9][0-9]*\.[0-9]*{EXP}	{ mylval = marlais_make_dfloat (strtod (yytext, NULL));
				  return (LITERAL);
				}
<ETC>[+-]?[0-9][0-9]*{EXP}	{ mylval = marlais_make_dfloat (strtod (yytext, NULL));
				  return (LITERAL);
				}

<ETC>\'([^\\\']|(\\.))\' { char ch = yytext [yyleng-2];
			   mylval =
			    marlais_make_character(yytext[1] == '\\' ?
					   expand_escaped_character(ch) : ch );
			   return (LITERAL);
			 }


<ETC>{OPSYMS}		{ /* OPERATOR  SYMBOL */
			  return which_operator(yytext, yyleng);
			}

<ETC>[\(\)\[\]\{\}\.\,\;\~\?]	{ /* return char as token */
				  mylval = (Object)0;
				  return *yytext;
				}

<ETC>=>		{ mylval = equal_arrow_symbol;
		  return (EQUAL_ARROW);
		}

<ETC>::		{ mylval = colon_colon_symbol;
		  return (COLON_COLON);
		 }

<ETC>#\(		{ mylval = NULL;
		  return (HASH_PAREN);
		 }

<ETC>#\[		{ mylval = NULL;
		  return (HASH_BRACKET);
		 }

<ETC>\?\?		{ mylval = NULL;
		  return (QUESTION_QUESTION);
		 }

<ETC>\.\.\.		{ mylval = NULL;
		  return (ELLIPSIS);
		 }

<ETC>#{STRING}	{ /* Do some nasty business with yytext */
		  yytext[yyleng-1] = ':';
		  mylval = make_keyword (yytext + 2);
		  return (KEYWORD);
		}

<ETC>{SYMBOL}:	{
		  mylval = make_keyword (yytext);
		  if (mydebug) {
			printf("yydebug: got symbol [%s]\n",  yytext);
			}
		  return (KEYWORD);
		 }

<ETC>#[a-zA-Z][a-zA-Z\-]*	{ return search_for_poundword (yytext, &mylval);
			}


<ETC>{SYMBOL}	{ int tmp = symbol_or_resword (yytext, &mylval);
		  if (mydebug && tmp == SYMBOL) {
			printf ("yydebug: got symbol [%s]\n", yytext);
			}
		  return tmp;
		}

<ETC>\\{OPSYMS}	{ which_operator (yytext+1, yyleng-1);
		  return SYMBOL;
		}

<INI,KEY,VAL,ETC>.	{ process_unrecognized_character (yytext);
			}

<ETC>\/\/[^\n]*	{ }

<ETC>"/*"	{ int ch;
		  loop:
			do {
				ch = input();
				if (ch == '\n')
					++yylineno;
				else
				if (ch == EOF)
					break;
			} while (ch != '*');
		  inner:
		      	switch (input()) {
			case EOF:
			case '/': break;
			case '*': goto inner;
			case '\n': ++yylineno;
			default: goto loop;
		      }
		}

<ETC>{STRING}	{ yytext[yyleng-1] = '\0';
		  mylval = make_expanded_byte_string (yytext+1);
		  return (STRING);
		}

%%

int yywrap() {
#define BUFSIZE 1024
  char buffer[BUFSIZE];
  char* input;

  if(load_file_context) return 1;

#ifdef MARLAIS_ENABLE_READLINE
  input = readline(current_prompt);
  if(input) {
    yy_delete_buffer( YY_CURRENT_BUFFER );
    if(*input) {
      add_history(input);
      snprintf(buffer, BUFSIZE, "%s\n", input);
      yy_scan_string(buffer);
      current_prompt = prompt_continuation;
    } else {
      yy_scan_string("\n");
    }
    free(input);
    return 0;
  } else {
    return 1;
  }

#else
  printf("%s", current_prompt);
  fflush(stdout);
  input = fgets(buffer, BUFSIZE, stdin);
  if(input) {
    yy_delete_buffer( YY_CURRENT_BUFFER );
    if(*input) {
      yy_scan_string(input);
      current_prompt = prompt_continuation;
    } else {
      yy_scan_string("\n");
    }
    return 0;
  } else {
    return 1;
  }
#endif
}

void
marlais_initialize_lexer (void)
{
    int i;

    intermediate_words = NULL;

    for (i = 0; i < NUM_COREWORDS; i++) {
	*(coreword_table[i].symbol) = make_symbol(coreword_table[i].word);
    }
    for (i = 0; i < NUM_BEGIN_WORDS; i++) {
	*(begin_word_table[i].symbol) =
	    make_symbol (begin_word_table[i].word);
    }
    for (i = 0; i < NUM_DEFINING_WORDS; i++) {
	*(defining_word_table[i].symbol) =
	    make_symbol (defining_word_table[i].word);
    }
    for (i = 0; i < NUM_IF_INTERMEDIATE_WORDS; i++) {
	*(if_intermediate_word_table[i].symbol) =
	    make_symbol (if_intermediate_word_table[i].word);
    }
    for (i = 0; i < NUM_SELECT_INTERMEDIATE_WORDS; i++) {
	*(select_intermediate_word_table[i].symbol) =
	    make_symbol (select_intermediate_word_table[i].word);
    }
    for (i = 0; i < NUM_CLASS_INTERMEDIATE_WORDS; i++) {
	*(class_intermediate_word_table[i].symbol) =
	    make_symbol (class_intermediate_word_table[i].word);
    }
    for (i = 0; i < NUM_FOR_INTERMEDIATE_WORDS; i++) {
	*(for_intermediate_word_table[i].symbol) =
	    make_symbol (for_intermediate_word_table[i].word);
    }
    for (i = 0; i < NUM_FOR_CLAUSE_WORDS; i++) {
	*(for_clause_word_table[i].symbol) =
	    make_symbol (for_clause_word_table[i].word);
    }
   for (i = 0; i < NUM_BLOCK_INTERMEDIATE_WORDS; i++) {
	*(block_intermediate_word_table[i].symbol) =
	    make_symbol (block_intermediate_word_table[i].word);
    }
   for (i = 0; i < NUM_MODULE_INTERMEDIATE_WORDS; i++) {
	*(module_intermediate_word_table[i].symbol) =
	    make_symbol (module_intermediate_word_table[i].word);
    }
    equal_arrow_symbol = make_symbol("=>");
    colon_colon_symbol = make_symbol("::");
}

void
marlais_lexer_reset (FILE *new_file)
{
	yylineno = 1;
	/* yyrestart(new_file); */
	BEGIN(INI);
}

void
marlais_lexer_push_intermediate_words(Object begin_word)
{
    struct intermediate_word_struct *new_table;

    new_table = (struct intermediate_word_struct *)
	marlais_allocate_memory (sizeof (struct intermediate_word_struct));

    if (begin_word == if_symbol) {
	new_table->num_words = NUM_IF_INTERMEDIATE_WORDS;
	new_table->reswords = if_intermediate_word_table;
    } else if (begin_word == select_symbol) {
	new_table->num_words = NUM_SELECT_INTERMEDIATE_WORDS;
	new_table->reswords = select_intermediate_word_table;
    } else if(begin_word == class_symbol) {
	new_table->num_words = NUM_CLASS_INTERMEDIATE_WORDS;
	new_table->reswords = class_intermediate_word_table;
    } else if (begin_word == for_symbol) {
	new_table->num_words = NUM_FOR_INTERMEDIATE_WORDS;
	new_table->reswords = for_intermediate_word_table;
	new_table->next = intermediate_words;
	intermediate_words = new_table;
	new_table = (struct intermediate_word_struct *)
	    marlais_allocate_memory (sizeof (struct intermediate_word_struct));
	new_table->num_words = NUM_FOR_CLAUSE_WORDS;
	new_table->reswords = for_clause_word_table;
    } else if (begin_word == block_symbol) {
	new_table->num_words = NUM_BLOCK_INTERMEDIATE_WORDS;
	new_table->reswords = block_intermediate_word_table;
    } else if (begin_word == module_symbol) {
	new_table->num_words = NUM_MODULE_INTERMEDIATE_WORDS;
	new_table->reswords = module_intermediate_word_table;
    }
    new_table->next = intermediate_words;
    intermediate_words = new_table;
}

void
marlais_lexer_pop_intermediate_words()
{
    intermediate_words = intermediate_words->next;
}

static int
search_for_poundword (char *string, YYSTYPE *obj_ptr)
{
    switch (string[1]) {
    case 't':
    case 'T':
	if (yyleng == 2) {
	    *obj_ptr = MARLAIS_TRUE;
	    return HASH_T;
	}
	break;
    case 'f':
    case 'F':
	if (yyleng == 2) {
	    *obj_ptr = MARLAIS_FALSE;
	    return HASH_F;
	}
	break;
    case 'n':
    case 'N':
	if (strcasecmp (string, "#next") == 0) {
	    *obj_ptr = next_symbol;
	    return HASH_NEXT;
	}
	break;
    case 'r':
    case 'R':
	if (strcasecmp (string, "#rest") == 0) {
	    *obj_ptr = hash_rest_symbol;
	    return HASH_REST;
	}
	break;
    case 'k':
    case 'K':
	if (strcasecmp (string, "#key") == 0) {
	    *obj_ptr = key_symbol;
	    return HASH_KEY;
	}
	break;
    case 'a':
    case 'A':
	if (strcasecmp (string, "#all-keys") == 0) {
	    *obj_ptr = allkeys_symbol;
	    return HASH_ALL_KEYS;
	}
	break;
    }
    obj_ptr = NULL;
    return UNRECOGNIZED;
}

static int
symbol_or_resword (char *string, YYSTYPE *obj_ptr)
{
    struct resword *result, target;

    target.word = string;

    result = search_for_resword (string, coreword_table, NUM_COREWORDS);
    if (result) {
	*obj_ptr = *(result->symbol);
	return result->token;
    }
    /* 	Check for simple begin word */
    result = search_for_resword (string, begin_word_table, NUM_BEGIN_WORDS);
    if (result) {
	*obj_ptr = *(result->symbol);
	return result->token;
    }
    result = search_intermediate_word (string);
    if (result) {
	*obj_ptr = *(result->symbol);
	return result->token;
    }
    result = search_for_resword (string, defining_word_table,
				 NUM_DEFINING_WORDS);
    if (result) {
	*obj_ptr = *(result->symbol);
	return result->token;
    }
    *obj_ptr = make_symbol(string);
    return SYMBOL;
}

static struct resword *
search_for_resword(char *string, struct resword *table, int num_words)
{
    struct resword target;

    target.word = string;
    return (struct resword *)bsearch ((const void *)(&target),
	    (const void *) table,
	    num_words, sizeof (struct resword),
            (int (*)(const void *, const void *))resword_compare);
}

static struct resword *
search_intermediate_word (char *string)
{
    int i;

    if (intermediate_words) {
	for (i = 0; i < intermediate_words->num_words; i++) {
	    if (0 == strcasecmp(string,
				(intermediate_words->reswords)[i].word)) {
		return &((intermediate_words->reswords)[i]);
	    }
	}
    }
    return NULL;
}

static int
resword_compare (struct resword *r1, struct resword *r2)
{
	return strcasecmp (r1->word, r2->word);
}

static void
process_unrecognized_character (char *yytext)
{
	unsigned c = *yytext;
	fprintf(stderr, "Unrecognized character '%c' (0x%02x).\n", c, c);
}

static int
which_operator( char *string, int length)
{
  if (length == 1) {
    switch (*string) {
	case '+':
	    mylval = plus_symbol;
	    break;
	case '-':
	    mylval = minus_symbol;
	    break;
	case '*':
	    mylval = times_symbol;
	    break;
	case '/':
	    mylval = divides_symbol;
	    break;
	case '^':
	    mylval = exponent_symbol;
	    break;
	case '<':
	    mylval = lesser_symbol;
	    break;
	case '>':
	    mylval = greater_symbol;
	    break;
	case '=':
	    mylval = equal_symbol;
	    break;
	case '&':
	    mylval = and_symbol;
	    break;
	case '|':
	    mylval = or_symbol;
	    break;
	case '~':
	    mylval = not_symbol;
	    break;
	}
	return *string;
    } else if (length == 2) {
	switch (*string) {
	case '<':
	    mylval = lesser_equal_symbol;
	    return LESSER_EQUAL;
	case '>':
	    mylval = greater_equal_symbol;
	    return GREATER_EQUAL;
	case '=':
	    mylval = equal_equal_symbol;
	    return EQUAL_EQUAL;
	case '~':
	    mylval = not_equal_symbol;
	    return NOT_EQUAL;
	case ':':
	    mylval = colon_equal_symbol;
	    return COLON_EQUAL;
	}
    } else if(length == 3) {
      mylval = not_equal_equal_symbol;
      return NOT_EQUAL_EQUAL;
    }

    /*
     * If control gets here, we're really in trouble.
     */
    abort();
}

static void
make_header_key()
{
	header_key = make_keyword (yytext);
}

static void
make_header_end()
{

}

static char *
get_nonws_symbol (char *text)
{
	char *buffer, *buf_ptr, *start_ptr, *end_ptr;

	for (start_ptr = text;
	     *start_ptr == ' ' || *start_ptr == '\t' || *start_ptr == '\f'
		|| *start_ptr == '\v' || *start_ptr == '\r';
	     start_ptr++);
	for (end_ptr = start_ptr;
	     *end_ptr != ' ' && *end_ptr != '\t' && *end_ptr != '\f'
		&& *end_ptr != '\v' && *end_ptr != '\r' && *end_ptr != '\n';
             end_ptr++);
	buf_ptr = buffer =
		(char *) marlais_allocate_memory((end_ptr - start_ptr + 1) * sizeof (char));
	for(; start_ptr < end_ptr; *buf_ptr++ = *start_ptr++);
	*buf_ptr = '\0';
	return buffer;
}

static void
make_header_val()
{
	if (header_key == module_keyword) {
		Object symbol = make_symbol (get_nonws_symbol (yytext));
		struct module_binding *module = module_binding (symbol);
		set_module (module);
	}
}

static char
expand_escaped_character (char ch)
{
	switch (ch) {
	case 'a':
		return '\a';
	case 'b':
		return '\b';
	case 'f':
		return '\f';
	case 'n':
		return '\n';
	case 'r':
		return '\r';
	case 't':
		return '\t';
	case 'v':
		return '\v';
	}
	return ch;
}

static Object make_expanded_byte_string(char* str)
{
	char* backslash = strchr(str, '\\');
	if (backslash) {
		char* exp_str;
		Object obj;

		exp_str = marlais_allocate_strdup (str);
		exp_str[0] = '\0';
		while (backslash) {
			backslash[0] = expand_escaped_character(backslash[1]);
			backslash[1] = '\0';
			strcat(exp_str, str);
			str = backslash + 2;
			backslash = strchr(str, '\\');
		}
		strcat(exp_str, str);

		obj = marlais_allocate_object (ByteString, sizeof (struct byte_string));
		BYTESTRSIZE(obj) = strlen (exp_str);
		BYTESTRVAL(obj) = exp_str;
		return (obj);
	}
	return marlais_make_bytestring(str);
}

static int countlines(char *str)
{
	int lines = 0;
	char c = *str++;
	while (c) {
		if (c == '\n') ++lines;
		c = *str++;
	}
	return lines;
}
