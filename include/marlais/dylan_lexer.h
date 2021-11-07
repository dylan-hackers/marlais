/*
   dylan_lexer.h
 */

#ifndef MARLAIS_DYLAN_LEXER_H
#define MARLAIS_DYLAN_LEXER_H

#include <marlais/common.h>
#include <marlais/yystype.h>

void init_reserved_word_symbols (void);

extern FILE *yyin;
void yy_restart (FILE * fp);
void yyrestart (FILE * new_file);
void yy_skip_ws (void);
int charready (FILE * fp);

int yylex (void);

#endif
