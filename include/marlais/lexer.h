/*
   dylan_lexer.h
 */

#ifndef MARLAIS_LEXER_H
#define MARLAIS_LEXER_H

#include <marlais/common.h>

extern void marlais_initialize_lexer (void);

extern void marlais_lexer_reset (void);

static void marlais_lexer_push_intermediate_words (Object begin_word);
static void marlais_lexer_pop_intermediate_words (void);

#endif
