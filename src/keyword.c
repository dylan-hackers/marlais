/* keyword.c -- see COPYRIGHT for use */

#include <marlais/keyword.h>

#include <marlais/bytestring.h>
#include <marlais/list.h>
#include <marlais/prim.h>
#include <marlais/symbol.h>

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
  marlais_register_prims (num, keyword_prims);
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
