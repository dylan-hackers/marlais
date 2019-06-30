/* keyword.c -- see COPYRIGHT for use */

#include <string.h>

#include "keyword.h"

#include "bytestring.h"
#include "list.h"
#include "prim.h"
#include "symbol.h"

/* primitives */

static Object symbol_to_string (Object sym);
static Object string_to_symbol (Object str);

static struct primitive keyword_prims[] =
{
    {"%keyword->symbol", prim_1, keyword_to_symbol},
    {"%symbol->keyword", prim_1, symbol_to_keyword},
    {"%symbol->string", prim_1, symbol_to_string},
    {"%string->symbol", prim_1, string_to_symbol},
};

/* function definitions */

void
init_keyword_prims (void)
{
  int num = sizeof (keyword_prims) / sizeof (struct primitive);
  init_prims (num, keyword_prims);
}

/* take a "foo:" keyword and return the symbol "foo" */
Object
keyword_to_symbol (Object keyword)
{
    char name[MAX_SYMBOL_SIZE];
    int size;

    strcpy (name, KEYNAME (keyword));
    size = strlen (name);
    name[size - 1] = '\0';
    return (make_symbol (name));
}

Object
symbol_to_keyword (Object symbol)
{
    char name[MAX_SYMBOL_SIZE];

    strcpy (name, SYMBOLNAME (symbol));
    strcat (name, ":");
    return (make_keyword (name));
}

/* Return the value of a keyword from a list, e.g., 

   (find-keyword foo: '(wagga fooga :bar 3 :foo 6 :baz 8)) => 6 */
Object
find_keyword_val (Object keyword, Object lst)
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

/* Return all keywords in the given list. */
Object
all_keywords (Object lst)
{
    Object l, keywords;

    l = lst;
    keywords = make_empty_list ();
    while (!EMPTYLISTP (l)) {
	if (KEYWORDP (CAR (l))) {
	    keywords = cons (CAR (l), keywords);
	}
	l = CDR (l);
    }
    return (keywords);
}

/* locals */

static Object
symbol_to_string (Object sym)
{
    return (marlais_make_bytestring (SYMBOLNAME (sym)));
}

static Object
string_to_symbol (Object str)
{
    return (make_symbol (BYTESTRVAL (str)));
}
