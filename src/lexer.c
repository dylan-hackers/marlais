
#include <marlais/lexer.h>

#include <marlais/alloc.h>

#include "parser.gen.h"
#include "lexer.gen.h"

/* Internal function declarations */

static char expand_escaped_character (char ch);

/* Exported functions */

Object
marlais_lexer_expand_char (char *str)
{
  char c;
  size_t len = strlen(str);
  /* be checky */
  if((len == 3) && (str[0] == '\'') && (str[2] == '\'')) {
    /* unescaped case */
    c = str[1];
  } else if((len == 4) && (str[0] == '\'')
            && (str[1] == '\\') && (str[3] == '\'')) {
    /* escaped case */
    c = expand_escaped_character(str[2]);
  } else {
    return marlais_error("Expanding malformed character literal", NULL);
  }
  /* return result */
  return marlais_make_character(c);
}

Object
marlais_lexer_expand_string (char *str)
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

    obj = marlais_allocate_object (ByteString, sizeof (struct marlais_byte_string));
    BYTESTRSIZE(obj) = strlen (exp_str);
    BYTESTRVAL(obj) = exp_str;
    return (obj);
  }
  return marlais_make_bytestring(str);
}

/* Internal functions */

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
#if 0
    /* we are not bold enough for this */
  case '0':
    return '0';
#endif
  }
  return ch;
}
