%{ /*  Emacs: -*- Fundamental -*- */
/*   Lexical analyzer for Dylan interm report tokens
     see COPYRIGHT for use */

#include <marlais/common.h>

#include <marlais/alloc.h>
#include <marlais/bytestring.h>
#include <marlais/character.h>
#include <marlais/lexer.h>
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

#define NUM_BEGIN_WORDS 9

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

#define NUM_DEFINING_WORDS      5

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

#define NUM_IF_INTERMEDIATE_WORDS       2
struct resword if_intermediate_word_table [NUM_IF_INTERMEDIATE_WORDS] =
{
        {"else", ELSE, &else_symbol},
        {"elseif", ELSEIF, &elseif_symbol},
};

#define NUM_SELECT_INTERMEDIATE_WORDS   1
struct resword select_intermediate_word_table [NUM_SELECT_INTERMEDIATE_WORDS] =
{
        {"by", BY, &by_symbol},
};

#define NUM_CLASS_INTERMEDIATE_WORDS    1
struct resword class_intermediate_word_table [NUM_CLASS_INTERMEDIATE_WORDS] =
{
        {"slot", SLOT, &slot_symbol},
};

#define NUM_FOR_INTERMEDIATE_WORDS      1
struct resword for_intermediate_word_table [NUM_FOR_INTERMEDIATE_WORDS] =
{
        {"finally", FINALLY, &finally_symbol},
};

#define NUM_FOR_CLAUSE_WORDS            7
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

#define NUM_BLOCK_INTERMEDIATE_WORDS    3
struct resword block_intermediate_word_table [NUM_BLOCK_INTERMEDIATE_WORDS] =
{
        {"afterwards", AFTERWARDS, &afterwards_symbol},
        {"cleanup", CLEANUP, &cleanup_symbol},
        {"exception", EXCEPTION, &exception_symbol},
};

#define NUM_MODULE_INTERMEDIATE_WORDS   3
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

static char *get_nonws_symbol (char *text);

static void make_header_key (void);
static void make_header_val (void);
static void make_header_end (void);

static int countlines(char *str);

%}
%pointer

/******************************************************************************
 * START OF DEFINITIONS                                                       *
 ******************************************************************************/

/* Names, Symbols and Keywords
*/

/*
alphabetic-character:
                one of a b c d e f g h i j k l m n o p q r s t u v w x y z
*/

alphaCHAR       [a-zA-Z]

/*
numeric-character:
                one of 0 1 2 3 4 5 6 7 8 9
*/

numericCHAR     [0-9]

/*
graphic-character:
                one of ! & * < > | ^ $ % @ _
*/

graphicCHAR     [\!\&\*\<\>\|\^\$\%\@\_]

/*
special-character:
                one of - + ~ ? / =
*/

specialCHAR     [\-\+\~\?\/\=]

/*
any-character:
                alphabetic-character
                numeric-character
                graphic-character
                special-character
*/

anyCHAR         {alphaCHAR}|{numericCHAR}|{graphicCHAR}|{specialCHAR}

/*
word-character-not-alphabetic:
                numeric-character
                graphic-character
                special-character
*/

wordNonALPHA    {numericCHAR}|{graphicCHAR}|{specialCHAR}

/*
word-character-not-double-alphabetic:
                alphabetic-character word-character-not-alphabetic
                numeric-character
                graphic-character
                special-character
*/

wordNonDALPHA   ({alphaCHAR}{wordNonALPHA})|{wordNonALPHA}

/*
leading-alphabetic:
                alphabetic-character
                leading-alphabetic any-character
*/

leadALPHA       {alphaCHAR}{anyCHAR}*

/*
leading-numeric:
                numeric-character
                leading-numeric word-character-not-double-alphabetic
*/

leadNUMERIC     {numericCHAR}{wordNonDALPHA}*

/*
leading-graphic:
                graphic-character
                leading-graphic word-character-not-alphabetic
*/
leadGRAPHIC     {graphicCHAR}{wordNonALPHA}*

/*
word:
                leading-alphabetic
                leading-numeric alphabetic-character leading-alphabetic
                leading-graphic leading-alphabetic
*/

WORD            {leadALPHA}|({leadNUMERIC}{alphaCHAR}{leadALPHA})|({leadGRAPHIC}{leadALPHA})

/* Operators
*/

/*
unary-operator:
                one of - ~
*/

unaryOPERATOR   [\-\~]

/*
binary-operator:
                one of + - * / ^ = == ~= ~== < <= > >= & | :=
*/

binaryOPERATOR  [\+\-\*\/\^\=\<\>\&\|]|([\<\>=:~]=)|(~==)

/*
operator:       # this definition is custom
                # operators are passed to parser with no regard for arity
                unary-operator
                binary-operator
*/

OPERATOR        {unaryOPERATOR}|{binaryOPERATOR}

/* Character and String Literals
*/

/*
escape-character:
                one of \ ' " a b e f n r t 0
                < hex-digits >
string-character:
                any printing character (including space) except for " or \
                \ escape-character
more-string:
                string-character more-string
                "
string:
                " more-string
*/

/*              # we ignore this restriction
                # and allow all escape characters
escapeCHAR      [\\\'\"abefnrt0]|{hexDIGIT}
*/

STRING          \"([^\\\"]|(\\(.|\n)))*\"

/* Numbers
*/

/*
binary-digit:
                one of 0 1
octal-digit:
                one of 0 1 2 3 4 5 6 7
decimal-digit:
                one of 0 1 2 3 4 5 6 7 8 9
hex-digit:      # we extend this to accept lowercase
                one of 0 1 2 3 4 5 6 7 8 9 A B C D E F
*/

octDIGIT        [0-7]
decDIGIT        [0-9]
hexDIGIT        [0-9a-fA-f]

EXP             ([Ee][\+\-]?[0-9][0-9]*)?

/* Custom definitions
*/

WHITESPACE      [\ \t\f\v\r]

/******************************************************************************/
/* END OF DEFINITIONS                                                         */
/******************************************************************************/

/******************************************************************************/
/* BEGINING OF RULES                                                          */
/******************************************************************************/

%x INI KEY VAL ETC
/* start contexts courtesy of Roger Critchlow */

%%

 /* Start the INI context when the parse begins */

<INITIAL>.      { BEGIN(INI);
                  yyless(0);
                }

 /* Emit EOF token on end-of-file in any state */

<INITIAL,INI,KEY,VAL,ETC><<EOF>>        { mylval = eof_object;
                                          return EOF_TOKEN;
                                        }

 /* Match for interpreter command at beginning of file */

<INI>^#!.*\n    { BEGIN(KEY);
                  /* warn(line_count, "ignoring initial #! interpreter comment\n"); */
                  /* line_count++; */
                  ++yylineno;
                }

 /* Otherwise start the main context */

<INI>.          { BEGIN(ETC);
                  yyless(0);
                }

 /* Header keys */

<INI,KEY>^[A-Za-z][-A-Za-z0-9]*:        { BEGIN(VAL);
                                          make_header_key();
                                          return yylex();
                                        }

 /* Header values */

<VAL>.*\n([\ \t\f\v\r]+.+\n)*   { BEGIN(KEY);
                                  yylineno += countlines (yytext);
                                  make_header_val ();
                                  return yylex ();
                                }

 /* An empty line ends the header section */

<INI,KEY>^[\ \t\f\v\r]*\n       { BEGIN(ETC);
                                  ++yylineno;
                                  make_header_end ();
                                  return yylex ();
                                }

 /* Ignore whitespace */

<ETC>{WHITESPACE}       { }

 /* Ignore line comments */

<ETC>\/\/[^\n]*         { }

 /* Ignore block comments */

<ETC>"/*"       { int ch;
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

 /* Count newlines */

<ETC>[\n]               { ++yylineno; /* Bogus hack! */ }

 /* Binary integers */

<ETC>#b[01][01]*        { mylval = marlais_make_integer (strtol (yytext+2, NULL, 2));
                          return (LITERAL);
                        }

 /* Octal integers */

<ETC>#o[0-7][0-7]*      { mylval = marlais_make_integer (strtol (yytext+2, NULL, 8));
                          return (LITERAL);
                        }

 /* Decimal integers */

<ETC>[+-]?[0-9][0-9]*   { mylval = marlais_make_integer (strtol (yytext, NULL, 10));
                          return (LITERAL);
                        }

 /* Hexadecimal integers */

<ETC>#x[0-9A-Fa-f][0-9A-Fa-f]* { mylval = marlais_make_integer (strtol (yytext+2, NULL, 16));
                                 return (LITERAL);
                               }

 /* Ratios */

<ETC>[+-]?[0-9][0-9]*\/[0-9][0-9]*      {
                                  char *ptr;
                                  long numerator, denominator;
                                  numerator = strtol (yytext, &ptr, 10);
                                  denominator = strtol (ptr + 1, NULL, 10);
                                  mylval = marlais_make_ratio (numerator, denominator);
                                  return (LITERAL);
                                }

 /* Floats */

<ETC>[+-]?[0-9]*\.[0-9][0-9]*{EXP}      { mylval = marlais_make_dfloat (strtod (yytext, NULL));
                                          return (LITERAL);
                                        }
<ETC>[+-]?[0-9][0-9]*\.[0-9]*{EXP}      { mylval = marlais_make_dfloat (strtod (yytext, NULL));
                                          return (LITERAL);
                                        }
<ETC>[+-]?[0-9][0-9]*{EXP}              { mylval = marlais_make_dfloat (strtod (yytext, NULL));
                                          return (LITERAL);
                                        }

 /* Characters */

<ETC>\'([^\\\']|(\\.))\' { mylval = marlais_lexer_expand_char (yytext);
                           return (LITERAL);
                         }

 /* Strings */

<ETC>{STRING}   { yytext[yyleng-1] = '\0';
                  mylval = marlais_lexer_expand_string (yytext+1);
                  return (STRING);
                }

 /* Symbols */

<ETC>#{STRING}  { /* Do some nasty business with yytext */
                  yytext[yyleng-1] = ':';
                  mylval = marlais_make_symbol (yytext + 2);
                  return (SYMBOL);
                }

<ETC>{WORD}:    {
                  mylval = marlais_make_symbol (yytext);
                  if (mydebug) {
                        printf("yydebug: got symbol [%s]\n",  yytext);
                        }
                  return (SYMBOL);
                 }

 /* Operators are treated as specific tokens */

<ETC>{OPERATOR} { return which_operator(yytext, yyleng);
                }

 /* Escaped operators are treated as names */

<ETC>\\{OPERATOR}       { which_operator (yytext+1, yyleng-1);
                          return NAME;
                        }

 /* Words are either reserved words or binding references */

<ETC>{WORD}     { int tmp = symbol_or_resword (yytext, &mylval);
                  if (mydebug && tmp == SYMBOL) {
                        printf ("yydebug: got symbol [%s]\n", yytext);
                        }
                  return tmp;
                }

 /* Poundwords serve various syntactic purposes, including various literals */

<ETC>#[a-zA-Z][a-zA-Z\-]*       { return search_for_poundword (yytext, &mylval);
                                }

 /* Various special tokens */

<ETC>[\(\)\[\]\{\}\.\,\;\~\?]   { /* return char as token */
                                  mylval = (Object)0;
                                  return *yytext;
                                }

<ETC>=>         { mylval = equal_arrow_symbol;
                  return (EQUAL_ARROW);
                }

<ETC>::         { mylval = colon_colon_symbol;
                  return (COLON_COLON);
                }

<ETC>#\(        { mylval = NULL;
                  return (HASH_PAREN);
                }

<ETC>#\[        { mylval = NULL;
                  return (HASH_BRACKET);
                }

<ETC>\?\?       { mylval = NULL;
                  return (QUESTION_QUESTION);
                }

<ETC>\.\.\.     { mylval = NULL;
                  return (ELLIPSIS);
                }

 /* Handle unrecognized characters */

<INI,KEY,VAL,ETC>.      { process_unrecognized_character (yytext);
                        }

%%

/******************************************************************************/
/* END OF RULES                                                               */
/******************************************************************************/

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
        *(coreword_table[i].symbol) = marlais_make_name(coreword_table[i].word);
    }
    for (i = 0; i < NUM_BEGIN_WORDS; i++) {
        *(begin_word_table[i].symbol) =
            marlais_make_name (begin_word_table[i].word);
    }
    for (i = 0; i < NUM_DEFINING_WORDS; i++) {
        *(defining_word_table[i].symbol) =
            marlais_make_name (defining_word_table[i].word);
    }
    for (i = 0; i < NUM_IF_INTERMEDIATE_WORDS; i++) {
        *(if_intermediate_word_table[i].symbol) =
            marlais_make_name (if_intermediate_word_table[i].word);
    }
    for (i = 0; i < NUM_SELECT_INTERMEDIATE_WORDS; i++) {
        *(select_intermediate_word_table[i].symbol) =
            marlais_make_name (select_intermediate_word_table[i].word);
    }
    for (i = 0; i < NUM_CLASS_INTERMEDIATE_WORDS; i++) {
        *(class_intermediate_word_table[i].symbol) =
            marlais_make_name (class_intermediate_word_table[i].word);
    }
    for (i = 0; i < NUM_FOR_INTERMEDIATE_WORDS; i++) {
        *(for_intermediate_word_table[i].symbol) =
            marlais_make_name (for_intermediate_word_table[i].word);
    }
    for (i = 0; i < NUM_FOR_CLAUSE_WORDS; i++) {
        *(for_clause_word_table[i].symbol) =
            marlais_make_name (for_clause_word_table[i].word);
    }
   for (i = 0; i < NUM_BLOCK_INTERMEDIATE_WORDS; i++) {
        *(block_intermediate_word_table[i].symbol) =
            marlais_make_name (block_intermediate_word_table[i].word);
    }
   for (i = 0; i < NUM_MODULE_INTERMEDIATE_WORDS; i++) {
        *(module_intermediate_word_table[i].symbol) =
            marlais_make_name (module_intermediate_word_table[i].word);
    }
    equal_arrow_symbol = marlais_make_name("=>");
    colon_colon_symbol = marlais_make_name("::");
}

void
marlais_lexer_reset (void)
{
        yylineno = 1;
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
    /*  Check for simple begin word */
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
    *obj_ptr = marlais_make_name(string);
    return NAME;
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
make_header_key()
{
        header_key = marlais_make_symbol (yytext);
}

static void
make_header_val()
{
        if (header_key == module_keyword) {
                Object symbol = marlais_make_name (get_nonws_symbol (yytext));
                struct module_binding *module = marlais_get_module (symbol);
                marlais_set_module (module);
        }
}

static void
make_header_end()
{

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
