%{ /* Emacs: -*- Fundamental -*- */

/*
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

   Original authorship and copyright notices follows

 */

/* interim-dylan.y -- dylan phrase grammar from 5/12/94 interim report */

/*
 * this grammar is, i believe, an accurate translation of the
 * BNF grammar in the appendix to the 5/12/94 interim report
 * into yacc-friendly format.  the differences are
 *
 *	+ where the BNF used ellipses to indicate repitition,
 *	  (right) recursive productions have been added.
 *
 *	+ the grammar has been rearranged to eliminate ambiguities
 *	  (what yacc calls conflicts);  this grammar should have
 *	  no shift/reduce or reduce/reduce conflicts.  occasional
 *	  comments explain what had to be done in the dicier of
 *	  situations.
 *
 *	+ according to notes from the partners' mailing list, a
 *	  few productions were omitted from the grammar in the
 *	  report;  these are flagged with the comment "omitted"
 *	  in this grammar.
 *
 * paul haahr
 * 22 may 1994
 */

/*
   Marlais Revisions Copyright 1994, Joseph N. Wilson.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.
*/

/* 
 * Modified this to have built-in rules for control constructs.
 * I'll save macro implementation for a little later.
 * 
 * jnw@cis.ufl.edu
 * 15 July 1994
 */

#ifdef MACOS
#define YYDEBUG 0
#endif

/* #define OPTIMIZE_SPECIALIZERS */

#include "boolean.h"
#include "bytestring.h"
#include "error.h"
#include "gc.h"
#include "globaldefs.h"
#include "list.h"
#include "number.h"
#include "object.h"
#include "symbol.h"
#include "table.h"
#include "vector.h"
#include "yystype.h"

#define alloca GC_malloc

void yyerror (char *);
static Object append_bang (Object l1, Object l2);
static Object nelistem (Object car,...);

extern char *yytext;
extern Object *parse_value_ptr;

extern int yylineno;

void push_intermediate_words (Object begin_word);
void pop_intermediate_words (void);

Object binding_stack;
Object symtab;
Object methnames, methdefs;
static void push_bindings();
static void pop_bindings();
static Object bindings_top ();
static void bindings_increment ();
static Object gensym(int i);
static Object make_setter_expr (Object place, Object value);
static int allocation_word (Object word);
%}

/*
 * Inappropriate start symbol for an interpreter
 *
	%start dylan_program
 */

%start evaluable_constituent

%token	SYMBOL				/* identifier */
%token	KEYWORD				/* identifier: */
%token	LITERAL				/* #"..." */
%token	STRING				/* "..." */

%token	'(' ')' '[' ']' '{' '}'

%right	COLON_EQUAL	/* := */
%left	'&' '|'
%left	GREATER_EQUAL LESSER_EQUAL '>' '<' NOT_EQUAL EQUAL_EQUAL '='
%left	'-' '+'
%left	'/' '*'
%left	'^'

%token	'.' ',' ';' '~'

%token	EQUAL_ARROW	/* => */
%token	COLON_COLON	/* :: */

/* For macros */
%token	QUESTION_QUESTION	/* ?? */
%token	ELLIPSIS		/* ... */

/* for signalling errors from lexical analyzer */
%token UNRECOGNIZED
%token EOF_TOKEN

%token	HASH_T HASH_F HASH_BRACKET HASH_PAREN
%token	HASH_NEXT HASH_REST HASH_KEY HASH_ALL_KEYS

%token	DEFINE END GENERIC HANDLER LET LOCAL METHOD OTHERWISE

/*
 * Wait for macros to include this general approach
 * %token BLOCK_BEGIN_WORD EXPR_BEGIN_WORD SIMPLE_BEGIN_WORD
 * %token INTERMEDIATE_WORD EXPR_INTERMEDIATE_WORD SIMPLE_INTERMEDIATE_WORD
 */	

/*   %token	DEFINING_WORD */
%token CLASS CONSTANT LIBRARY MODULE VARIABLE TEST FUNCTION

/* CLASS intermediate words */
%token SLOT
%token BEGIN_TOKEN CASE IF UNLESS UNTIL WHILE BLOCK FOR SELECT

/* MODULE intermediate words */
%token USE EXPORT CREATE

/* IF intermediate words */
%token ELSE ELSEIF

/* SELECT intermediate words */
%token BY

/* FOR intermediate words */
%token FINALLY
%token THEN IN FROM TO ABOVE BELOW BY

/* BLOCK intermediate words */
%token AFTERWARDS CLEANUP EXCEPTION
%%



defining_word
	: CLASS
	| CONSTANT
	| LIBRARY
	| MODULE
	| VARIABLE
	| TEST
	| FUNCTION

/* Program Structure */

/*
dylan_program
	: body
*/

evaluable_constituent
	: ';'	{ *parse_value_ptr = unspecified_object; YYACCEPT; }

	| defining_form	';'
		{ *parse_value_ptr = $1; YYACCEPT; }

	| expression ';'
		{ *parse_value_ptr = $1; YYACCEPT; }

	| local_declaration
		{ *parse_value_ptr = unspecified_object;
		  warning("local binding outside of block ignored", NULL);
		  YYACCEPT;
	        }

/*	| constituent ';'
		{ *parse_value_ptr = $1; YYACCEPT; }
*/

	| EOF_TOKEN	{ *parse_value_ptr = eof_object; YYACCEPT; }

	| error ';'
		{ yyerrok;
		  *parse_value_ptr = unspecified_object;
		  YYACCEPT;
		}
	| error EOF_TOKEN
		{ yyerrok;
		  *parse_value_ptr = eof_object;
		  YYACCEPT;
		}

body
	:		{ $$ = unspecified_object; }
	| nonempty_body	{ $$ = $1; }

nonempty_body
	: 	{ push_bindings (); }

	  nonempty_constituents

	  {
	   if (INTVAL (bindings_top ()) > 0) {
	       $$ = cons(unbinding_begin_symbol,
			 cons (bindings_top(), $2));
	   } else if (list_length ($2) > 1) {
	       $$ = cons (begin_symbol, $2);
	   } else {
	       $$ = FIRST ($2);
	   }
	   pop_bindings ();
	}


nonempty_constituents
	: constituent			{ $$ = cons ($1, make_empty_list()); }
	| constituent ';' constituents	{ $$ = cons ($1, $3); }

constituents
	:		 		{ $$ = make_empty_list(); }
	| constituent 			{ $$ = cons ($1, make_empty_list()); }
	| constituent ';' constituents	{ $$ = cons ($1, $3); }
/*
	| local_declaration_block	{ $$ = cons ($1, make_empty_list()); }
*/


constituent
	: defining_form		{ $$ = $1; }
	| local_declaration	{ $$ = $1; } 
	| expression		{ $$ = $1; }
/*
	| error	';'		{ yyerrok; $$ = unspecified_object; }
 */

/* Expressions */

expression
	: binary_operand	{ $$ = $1; }
/*	| unparenthesized_operand COLON_EQUAL expression */
	| expression COLON_EQUAL expression
           { if (SYMBOLP ( $1)) {
		 $$ = listem (set_bang_symbol, $1, $3, NULL);
	     } else {
		 $$ = make_setter_expr ($1, $3);
	     }
	   }
	| expression '&' expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression '|' expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression GREATER_EQUAL expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression LESSER_EQUAL expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression '<' expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression '>' expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression NOT_EQUAL expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression EQUAL_EQUAL expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression '=' expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression '-' expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression '+' expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression '/' expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression '*' expression
		{ $$ = listem ($2, $1, $3, NULL); }
	| expression '^' expression
		{ $$ = listem ($2, $1, $3, NULL); }
/*
	| error { yyerrok; }
*/


binary_operand
	: KEYWORD			{ $$ = $1; }
	| keyless_binary_operand	{ $$ = $1; }

keyless_binary_operand
	: operand			{ $$ = $1; }
	| unary_operator operand
		{ $$ = cons ($1, cons ($2, make_empty_list())); }

unary_operator
	: '-'				{ $$ = negative_symbol; }
	| '~'				{ $$ = $1; }

operand	: operand '(' arguments_opt ')'

	  {   
#ifdef OPTIMIZE_SPECIALIZERS
	      $$ = cons ($1, $3);
#else
	      $$ = cons ($1, $3);
#endif
	  }
	| operand '[' arguments_opt ']'	/* array ref!!! */
    		{ if (list_length ($3) == 1) {
			$$ = cons (element_symbol, cons ($1, $3));
		  } else {
			$$ = cons (aref_symbol, cons ($1, $3));
		  }
		}
	| operand '.' variable_name
		{ $$ = cons ($3, cons ($1, make_empty_list())); }
	| leaf				{ $$ = $1; }

unparenthesized_operand
	: unparenthesized_operand '(' arguments_opt ')'
		{   
#ifdef OPTIMIZE_SPECIALIZERS
		    $$ = cons ($1, $3);
#else
		    $$ = cons ($1, $3);
#endif
		}
	| unparenthesized_operand '[' arguments ']'
    		{ $$ = cons (element_symbol, cons ($1, $3)); }
	| unparenthesized_operand '.' variable_name
		{ $$ = cons ($3, cons ($1, make_empty_list())); }
	| unparenthesized_leaf		{ $$ = $1; }

arguments
	: KEYWORD expression		{ $$ = listem ($1, $2, NULL); }
	| expression		{ $$ = cons ($1, make_empty_list()); }
	| KEYWORD expression ',' arguments
		{ $$ = cons ($1, cons ($2, $4)); }
	| expression ',' arguments
		{ $$ = cons ($1, $3); }

leaf	: '(' expression ')'		{ $$ = $2; }
	| unparenthesized_leaf		{ $$ = $1; }

unparenthesized_leaf
	: literal			{ $$ = $1; }
	| variable_name			{ $$ = $1; }
	| METHOD method_body END METHOD_opt
		{ $$ = cons ($1, $2); }
	| statement			{ $$ = $1; }

literal	: LITERAL				{ $$ = $1; }
	| strings				{ $$ = $1; }
	| HASH_T				{ $$ = $1; }
	| HASH_F				{ $$ = $1; }
	| HASH_PAREN list_constants_opt ')'
		{ $$ = $2; }
	| HASH_BRACKET constants_opt ']'
		{ $$ = make_sov ($2); }

strings	: STRING	    { $$ = $1; }
	| STRING component_strings    
		{ $$ = cons (concatenate_symbol, cons ($1, $2)); }

component_strings
	: STRING	{ $$ = cons ($1, make_empty_list()); }
	| STRING component_strings
		{ $$ = cons ($1, $2); }
constants
	: constant			{ $$ = cons ($1, make_empty_list()); }
	| constant ',' constants	{ $$ = cons ($1, $3); }

list_constants
	: constant			{ $$ = listem (pair_symbol,
						       $1,
						       make_empty_list(),
						       NULL);
					}
	| constant '.' constant		{ $$ = listem (pair_symbol, $1, $3, NULL);
					}
	| constant ',' list_constants	{ $$ = listem (pair_symbol, $1, $3, NULL);
					}

constant
	: literal			{ $$ = $1; }
	| KEYWORD			{ $$ = $1; }


/* Statements */

statement
	: begin_statement	{ $$ = $1; }
	| if_statement		{ $$ = $1; }
	| unless_statement	{ $$ = $1; }
	| case_statement	{ $$ = $1; }
	| select_statement	{ $$ = $1; }
	| while_statement	{ $$ = $1; }
	| until_statement	{ $$ = $1; }
	| for_statement		{ $$ = $1; }
	| block_statement	{ $$ = $1; }
/*
	| error END		{ yyerrok; }
*/


begin_statement
	: BEGIN_TOKEN 
		{ 
#ifdef OPTIMIZE_SPECIALIZERS
		    symtab_push_begin ();
#endif
		}
	  body END
		{
#ifdef OPTIMIZE_SPECIALIZERS
		    symtab_pop ();
#endif
		    $$ = $3;
		}


if_statement
	: IF		{ push_intermediate_words ($1); }
		'(' expression ')' then_body
		else_parts
			{ pop_intermediate_words (); }
		END IF_opt
			{ $$ = cons ($1,
				     cons ($4,
					   append_bang (cons ($6,
							      make_empty_list()),
							$7))); }

then_body
	:		{ $$ = false_object; }
	| nonempty_body	{ $$ = $1; }

else_parts
	:		{ $$ = cons(false_object, make_empty_list()); }
	| ELSE		{ $$ = cons (false_object, make_empty_list()); }
	| ELSE nonempty_body	{ $$ = cons ($2, make_empty_list()); }
	| ELSEIF '(' expression ')' body else_parts
			{ $$ = cons (cons (if_symbol,
					   cons ($3,
						 append_bang (cons ($5,
								    make_empty_list()),
							      $6))),
				     make_empty_list()); }


unless_statement
	: UNLESS '(' expression ')' body END UNLESS_opt
		{ $$ = cons ($1, cons ($3, cons ($5, make_empty_list()))); }


case_statement
	: CASE case_body
		END CASE_opt
		{ $$ = cons (cond_symbol, $2); }

case_body
	: { $$ = make_empty_list(); } 
	| case_label 
	  { push_bindings (); }
	  case_tail SEMICOLON_opt
	  { $$ =  cons (cons ($1, !EMPTYLISTP (CAR ($3))
				  ? cons (cons (unbinding_begin_symbol,
					        cons (bindings_top (),
						      CAR ($3))),
				          make_empty_list())
				  : make_empty_list()),
		        CDR ($3));
          }


case_tail
	:	{ $$ = cons (make_empty_list(), make_empty_list()); }
	| ';'	{ $$ = cons (make_empty_list(), make_empty_list()); }
	| ';' case_label
	{ push_bindings (); }
	case_tail
	{ $$ = cons (make_empty_list(),
		     cons (cons ($2, !EMPTYLISTP (CAR ($4))
				     ? cons (cons (unbinding_begin_symbol,
					           cons (bindings_top(),
							 CAR ($4))),
				             make_empty_list())
				     : make_empty_list()),
		          CDR ($4)));
	  pop_bindings ();
      }

	| constituent
	  { $$ = cons (cons ($1, make_empty_list()), make_empty_list()); }

	| constituent ';' case_tail
		{
		 $$ = cons (cons ($1, FIRST ($3)), CDR ($3));
		}
	| constituent ';' case_label { push_bindings(); }
	   case_tail
	{ $$ = cons (cons ($1, make_empty_list()), 
		     cons ( cons($3, !EMPTYLISTP (CAR ($5))
				     ? cons (cons (unbinding_begin_symbol,
						   cons (bindings_top(),
						         CAR ($5))),
					     make_empty_list())
				     : make_empty_list()),
			   CDR ($5)));
		  pop_bindings ();
	      }
/*
	| local_declaration_block ';' case_label case_tail
*/

case_label
	: expression EQUAL_ARROW
		{ $$ = $1; }
	| OTHERWISE EQUAL_ARROW_opt
		{ $$ = else_keyword; }

select_statement
	: SELECT	{ push_intermediate_words ($1); }
		'(' expression test_opt 
			{ pop_intermediate_words (); }
		')' select_body
		END SELECT_opt
		{ $$ = cons ($1, cons ($4, cons ($5 ? $5 : equal_equal_symbol,
						 $8))); }

test_opt
	: 			{ $$ = NULL; }
	| BY expression		{ $$ = $2; }
select_body
	:  { $$ = make_empty_list(); }
	| select_label 
	  { push_bindings (); }
	  select_tail SEMICOLON_opt
	  { $$ =  cons (cons ($1, !EMPTYLISTP (CAR ($3))
				  ? cons (cons (unbinding_begin_symbol,
						cons (bindings_top (),
						      CAR ($3))),
				          make_empty_list())
				  : make_empty_list()),
		      CDR ($3));
          }

select_tail
	:	{ $$ = cons (make_empty_list(), make_empty_list()); }
	| ';'	{ $$ = cons (make_empty_list(), make_empty_list()); }
	| ';' select_label
	{ push_bindings (); }
	select_tail
	{ $$ = cons (make_empty_list(),
		     cons (cons ($2, !EMPTYLISTP (CAR ($4))
				     ? cons (cons (unbinding_begin_symbol,
					           cons (bindings_top(),
							 CAR ($4))),
				    	     make_empty_list())
				     : make_empty_list()),
		           CDR ($4)));
	  pop_bindings ();
      }

	| constituent
	  { $$ = cons (cons ($1, make_empty_list()), make_empty_list()); }

	| constituent ';' select_tail
		{
		 $$ = cons (cons ($1, FIRST ($3)), CDR ($3));
		}
	| constituent ';' select_label { push_bindings(); }
	   select_tail
	{ $$ = cons (cons ($1, make_empty_list()), 
		     cons ( cons($3, !EMPTYLISTP (CAR ($5))
				     ? cons (cons (unbinding_begin_symbol,
						   cons (bindings_top(),
						         CAR ($5))),
					     make_empty_list())
				     : make_empty_list()),
			   CDR ($5)));
		  pop_bindings ();
	      }
/*
	| local_declaration_block ';' select_label select_tail
*/

select_label
	: expressions EQUAL_ARROW
		{ $$ = $1; }
	| '(' expression ',' expressions ')' EQUAL_ARROW
		{  $$ = cons ($2, $4); }
	| OTHERWISE EQUAL_ARROW_opt
		{ $$ = else_keyword; }

while_statement
	: WHILE '(' expression ')' body END WHILE_opt
		{ $$ = cons ($1, cons ($3, cons ($5, make_empty_list()))); }

until_statement
	: UNTIL '(' expression ')' body END UNTIL_opt
		{ $$ = cons ($1, cons ($3, cons ($5, make_empty_list()))); }

for_statement
	: FOR		{ push_intermediate_words ($1); }

		'(' for_clauses_opt for_terminator_opt
			{ pop_intermediate_words (); }
		')' body
		 finally_opt END { pop_intermediate_words (); } FOR_opt
		{ $$ = listem (for_symbol,	
			       $4,
			       append_bang($5, $9),
			       $8,
			       NULL);
		}
for_clauses
	: for_clause comma_opt		{ $$ = cons ($1, make_empty_list()); }
	| for_clause ',' for_clauses	{ $$ = cons ($1, $3); }

for_clauses_opt
	: 			{ $$ = make_empty_list(); }
	| for_clauses		{ $$ = $1; }
	
for_clause
	: variable '=' expression THEN expression
		{ $$ = cons ($1, cons ($3, cons ($5, make_empty_list()))); }
	| variable IN expression
	    { $$ = cons (collection_keyword,
			 cons ($1, cons ($3, make_empty_list()))); }
	| variable FROM expression bound_opt increment_clause_opt
		{ $$ = cons (range_keyword,
			     cons ($1,
				   cons ($3,
					 append_bang($4, $5)))); }

bound_opt
	:	{ $$ = make_empty_list(); }
        | TO expression
		{ $$ = cons ($1, cons ($2, make_empty_list())); }
	| ABOVE expression
		{ $$ = cons ($1, cons ($2, make_empty_list())); }
	| BELOW expression
		{ $$ = cons ($1, cons ($2, make_empty_list())); }

increment_clause_opt
	:	{ $$ = make_empty_list(); }
	| BY expression
		{ $$ = cons ($1, cons ($2, make_empty_list())); }

for_terminator_opt
	:			{ $$ = cons (false_object,
					     make_empty_list()); }
	| UNTIL expression	{ $$ = cons ($2, make_empty_list()); }
	| WHILE expression	{ $$ = cons (cons (not_symbol,
					           cons ($2,
                                                         make_empty_list())),
                                             make_empty_list()); }
	| KEYWORD expression	{ if ($1 == until_keyword) {
				      $$ =cons ($2, make_empty_list());
				  } else if ($1 == while_keyword) {
				      $$ = cons (cons (not_symbol,
						       cons ($2,
							     make_empty_list())),
					         make_empty_list());
				  } else {
					error ("Bogus keyword in if",
						$1,
						NULL);
				  }
				}

finally_opt
	:			{ $$ = make_empty_list(); }
	| FINALLY body		{ $$ = cons ($2, make_empty_list()); }

block_statement
	: BLOCK		{ push_intermediate_words ($1); }
		'(' variable_name ')'
		body
		afterwards_opt
		cleanup_opt
		exceptions
			{ pop_intermediate_words (); 
			  if (! EMPTYLISTP ($9)) {
				warning ("Exceptions not yet implemented!",
					 NULL);
				}
			}
		END BLOCK_opt

		{ $$ = listem (bind_exit_symbol,
			       cons ($4, make_empty_list()),
                               nelistem(unwind_protect_symbol,
					nelistem (bind_symbol,
					       cons (listem (hash_rest_symbol,
							       vals_symbol,
							       $6,
							       NULL),
						     make_empty_list()),
					       $7,
					       listem (apply_symbol,
						       $4,
						       vals_symbol,
						       NULL),
					       NULL),
					$8,
					NULL),
				NULL);
		}

afterwards_opt
	:			{ $$ = make_empty_list(); }
	| AFTERWARDS body 	{ $$ = $2; }

cleanup_opt
	: 		{ $$ = make_empty_list(); }
	| CLEANUP body	{ $$ = $2; }

exceptions
	:	{ $$ = make_empty_list(); }
	| exceptions 
	  EXCEPTION '(' exception_args ')' body
		/* Note that exceptions are consed up from back to front! */
		{ $$ = cons (listem ($2, $4, $6, NULL), $1); }

exception_args
	: variable_name		{ $$ = cons ($1, make_empty_list()); }
	| variable_name COLON_COLON variable_name comma_arguments_opt
		{ $$ = cons (cons ($1, cons ($3, make_empty_list())), $4); }

IF_opt
	:
	| IF

UNLESS_opt
	:
	| UNLESS

CASE_opt
	:
	| CASE

SELECT_opt
	:
	| SELECT

WHILE_opt
	:
	| WHILE

UNTIL_opt
	:
	| UNTIL

FOR_opt
	:
	| FOR

BLOCK_opt
	:
	| BLOCK

defining_form
	: DEFINE modifiers_opt METHOD method_definition
		/* worry about modifiers later. */
		{ $$ = cons (define_method_symbol, $4); }

/* HACK: we should never allow modifiers for DEFINE FUNCTION */
	| DEFINE modifiers_opt FUNCTION function_definition
		{ $$ = cons (define_function_symbol, $4); }

	| DEFINE modifiers_opt GENERIC generic_function_definition
		/* worry about modifiers later. */
		{ $$ = cons (define_generic_function_symbol, $4); }

/*	| DEFINE modifiers_opt defining_word definition */

	| DEFINE VARIABLE bindings
		{ $$ = cons (define_variable_symbol, CAR ($3)); }

	| DEFINE CONSTANT bindings
		{ $$ = cons (define_constant_symbol, CAR ($3)); }

	| DEFINE modifiers_opt CLASS
		{ push_intermediate_words ($3); }
	  class_definition
		{ pop_intermediate_words ();
		  if (EMPTYLISTP ($2)) {
			$$ = cons (define_class_symbol, $5); 
		  } else {
			$$ = cons (define_class_symbol,
				   cons (cons (modifiers_keyword,
					       $2),
					 $5));
		  }
		}
/*
	| DEFINE modifiers_opt defining_word bindings
*/

	| DEFINE MODULE
		{ push_intermediate_words ($2); }
	  module_definition
		{ $$ = cons (define_module_symbol, $4); }

	| DEFINE TEST SYMBOL '(' arguments_opt ')'
	  body END TEST_opt variable_name_opt
		{ $$ = listem (define_test_symbol, $3, $5,
			       empty_string, $7, NULL); }

class_definition
	: variable_name expression_list slot_specs END CLASS_opt
		 variable_name_opt
		{ $$ = cons ($1, cons ($2, $3)); }

slot_specs
	:				{ $$ = make_empty_list(); }
	| slot_spec			{ $$ = cons ($1, make_empty_list()); }
	| slot_spec ';' slot_specs	{ $$ = cons ($1, $3); }

slot_spec
	:
	  initialization_argument_spec { $$ = $1; }
	|
	  slot_modifiers_opt SLOT variable comma_arguments_opt
		{ Object mods, mod;
		  Object allocation, getter_name, slot_type, dynamism;
		  int dynamism_specified = 0, allocation_specified = 0;
		  int slot_type_specified = 0;

		  slot_type = cons (type_keyword,
				    cons (CLASSNAME (object_class),
					  make_empty_list()));
		  allocation = cons (allocation_keyword,
				     cons (instance_symbol,
					   make_empty_list()));
		  dynamism = cons (dynamism_keyword,
				   cons (open_symbol,
					 make_empty_list()));
		  if (PAIRP ($3)) {
		      getter_name = CAR ($3);
		      slot_type_specified = 1;
		      SECOND (slot_type) = SECOND ($3);
		  } else {
		      getter_name = $3;
		  }
		  mods = $1;
		  while ( PAIRP (mods)) {
		      mod = CAR (mods);
		      if (id (mod, open_symbol)) {
			  if (dynamism_specified) {
			      error ("Slot dyanamism specified twice", NULL);
			  }
			  dynamism_specified = 1;
		      } else if (id (mod, sealed_symbol)) {
			  if (dynamism_specified) {
			      error ("Slot dyanamism specified twice", NULL);
			  }
			  SECOND (dynamism) = sealed_symbol;
		      } else if (allocation_word (mod)) {
			  if (allocation_specified) {
			      error ("Slot allocation specified twice", NULL);
			  }
			  allocation_specified = 1;
			  SECOND (allocation) = mod;
			  if ( ! EMPTYLISTP (CDR (mods))) {
			      error ("Slot modifiers follow allocation",
				     mods, NULL);
			  }
		      }
		      mods = CDR (mods);
		  }
		  $$ = cons (getter_name, $4);
		  if (slot_type_specified) {
		      append_bang ($$, slot_type);
		  }
		  if (allocation_specified) {
		      append_bang ($$, allocation);
		  }
		  if (dynamism_specified) {
		      append_bang ($$, dynamism);
		  }
		}

slot_modifiers_opt
	: 				{ $$ = make_empty_list(); }
	| SYMBOL slot_modifiers_opt 	{ $$ = cons ($1, $2); }
	| CLASS slot_modifiers_opt	{ $$ = cons ($1, $2); }

initialization_argument_spec
	: SYMBOL KEYWORD comma_arguments_opt
		{
		  if ($1 != keyword_symbol) {
			error ("Bad initialization argument specification", NULL);
		  } else {
			$$ = cons (init_keyword_keyword, cons ($2, $3));
		  }
		}
	| SYMBOL SYMBOL KEYWORD comma_arguments_opt
		{
		  if ($1 != required_symbol || $2 != keyword_symbol) {
			error ("Bad initialization argument specification", NULL);
		  } else {
			$$ = cons (required_init_keyword_keyword,
				cons ($3, $4));
		  }
		}

method_definition
	: variable_name method_body END METHOD_opt variable_name_opt
		{ $$ = cons ($1, $2); }

function_definition
	: variable_name method_body END FUNCTION_opt variable_name_opt
		{ $$ = cons ($1, $2); }

generic_function_definition
	: variable_name generic_function_body
		{ $$ = cons ($1, $2); }
/*
definition
	: variable_name expression_list_opt item_list_opt END defining_word_opt variable_name_opt
*/

module_definition
	: variable_name module_clauses END MODULE_opt variable_name_opt
		{ $$ = cons ($1, $2); }

module_clauses
	:	{ $$ = make_empty_list(); }
	| module_clause ';' module_clauses
		{ $$ = cons ($1, $3); }

module_clause
	: use_clause		{ $$ = $1; }
	| export_clause		{ $$ = $1; }
	| create_clause		{ $$ = $1; }

use_clause
	: USE variable_name property_list_opt
		{ $$ = cons ($1, cons ($2, $3)); }

export_clause
	: EXPORT item_names	{ $$ = cons ($1, $2); }
create_clause
	: CREATE item_names	{ $$ = cons ($1, $2); }

modifiers
	: SYMBOL		{ $$ = cons ($1, make_empty_list()); }
	| SYMBOL modifiers 	{ $$ = cons ($1, $2); }

expression_list
	: '(' expressions_opt ')'	{ $$ = $2; }

expressions
	: expression			{ $$ = cons ($1, make_empty_list()); }
	| expression ',' expressions	{ $$ = cons ( $1, $3); }

/*
item_list
	: items SEMICOLON_opt

items	: item
	| items ';' item

item	: item_modifiers_and_word item_names property_list_opt
	| item_modifiers_and_word item_names type_designator property_list_opt

item_modifiers_and_word
	: variable_name
	| item_modifiers_and_word variable_name

*/

item_names
	: variable_name			{ $$ = cons ($1, make_empty_list()); }
	| variable_name ',' item_names	{ $$ = cons ($1, $3); }


/* Methods and Generic Functions */
/* n.b. define function form uses this, too -- DMA */
method_body
	: '(' parameter_list_opt ')' 
		{   
#ifdef OPTIMIZE_SPECIALIZERS
		    symtab_push_parameters ($2);
#endif
		}
	  SEMICOLON_opt body
		{   
#ifdef OPTIMIZE_SPECIALIZERS
		    symtab_pop ();
#endif
		    $$ = cons ($2, cons ($6, make_empty_list()));
		}
	| '(' parameter_list_opt ')' EQUAL_ARROW variable ';' body
		{ $$ = cons (append_bang ($2,
					  cons (hash_values_symbol,
						cons ($5, make_empty_list()))),
			     cons ($7, make_empty_list())); }

	| '(' parameter_list_opt ')' EQUAL_ARROW '(' value_list_opt ')' SEMICOLON_opt body
		{ $$ = cons (append_bang ($2,
					  cons (hash_values_symbol, $6)),
			     cons ( $9, make_empty_list())); }
generic_function_body
	: '(' parameter_list_opt ')'
		{ $$ = cons ($2, make_empty_list()); }
	| '(' parameter_list_opt ')' EQUAL_ARROW variable
		{ $$ = cons (append_bang ($2, cons (hash_values_symbol,
					      cons ($5, make_empty_list()))),
			     make_empty_list()); }
	| '(' parameter_list_opt ')' EQUAL_ARROW '(' value_list_opt ')'
		{ $$ = cons (append_bang ($2, cons (hash_values_symbol, $6)),
		 	     make_empty_list()); }

parameter_list
	: parameter		{ $$ = cons ($1, make_empty_list()); }
	| parameter_list ',' parameter
		{ $$ = append_bang ($1, cons ($3, make_empty_list())); }
	| parameter_list ',' next_rest_key_parameter_list
			{ $$ = append_bang ($1, $3); }
	| next_rest_key_parameter_list
			{ $$ = $1; }

next_rest_key_parameter_list
	: HASH_NEXT variable_name
		{ $$ = cons ($1, cons ($2, make_empty_list())); }
	| HASH_NEXT variable_name ',' rest_key_parameter_list
		{ $$ = cons ($1, cons ($2, $4)); }
	| rest_key_parameter_list
		{ $$ = $1; }

rest_key_parameter_list
	: HASH_REST variable_name
		{ $$ = cons ($1, cons ($2, make_empty_list())); }
	| HASH_REST variable_name ',' key_parameter_list
		{ $$ = cons ($1, cons ($2, $4)); }
	| key_parameter_list
		{ $$ = $1; }

key_parameter_list
	: HASH_KEY	{ $$ = cons ($1, make_empty_list()); }
	| HASH_KEY ',' HASH_ALL_KEYS
			{ $$ = cons ($1, cons ($3, make_empty_list())); }
	| HASH_KEY keyword_parameters
			{ $$ = cons ($1, $2); }
	| HASH_ALL_KEYS
		{ $$ = cons ($1, make_empty_list()); }

parameter
	: variable_name type_designator_opt
		{ $$ = ($2 ? cons ($1, cons ($2, make_empty_list()))
			: $1);
		}
	| variable_name EQUAL_EQUAL expression
		{ $$ = cons ($1, cons (cons (singleton_symbol,
					     cons ($3, make_empty_list())),
				       make_empty_list())); }

keyword_parameters
	: keyword_parameter	{ $$ = cons ($1, make_empty_list()); }
	| HASH_ALL_KEYS		{ $$ = cons ($1, make_empty_list()); }
	| keyword_parameter ',' keyword_parameters
			{ $$ = cons ($1, $3); }


keyword_parameter
	: KEYWORD variable_name type_designator_opt default_opt
		{ Object variable = $3 == NULL? $2
                                              : listem ($2, $3, NULL);
		  if ($4) {
		    if ($1) {
			$$ =cons ($1, cons (variable,
					    cons($4, make_empty_list())));
		    } else {
			$$ = cons (variable,
				   cons ($4, make_empty_list ()));
		    }
		  } else {
		      $$ = cons ($1, cons (variable, make_empty_list()));
		  }
		}

	| variable_name type_designator_opt default_opt
		{ Object variable = $2 == NULL? $1
					      : listem ($1, $2, NULL);
		  if ($3) {
		    $$ = cons (variable, cons ($3, make_empty_list()));
		} else {
		    $$ = variable;
		}
	       }

default	:  '=' expression		{ $$ = $2; }

/* Old style */
/*
	'(' expression ')'		{ $$ = $2; }
 */

/* Local Declarations */

/*
local_declaration_block
	: local_declaration ';' body
		{ $$ = cons (bind_symbol,
			     cons ($1, cons ($3, make_empty_list()))); }
*/

local_declaration
	: LET bindings
/*
		{ $$ = $2; }
*/

		{   bindings_increment ();
#ifdef OPTIMIZE_SPECIALIZERS
		    symtab_insert_bindings ($2);
#endif
		    $$ = cons (local_bind_symbol,
			       cons ($2, make_empty_list()));
		}
	| LET HANDLER condition '=' handler
	| LOCAL { methnames = methdefs = make_empty_list(); }
	  local_methods

          {   Object methbindings = cons (append (methnames,
						  cons (cons (values_symbol,
							      methdefs),
							make_empty_list())),
					  make_empty_list()); 
#ifdef OPTIMIZE_SPECIALIZERS
	      symtab_insert_bindings (methbindings);
#endif
	      bindings_increment ();
	      $$ = cons (local_bind_rec_symbol,
			 cons ( methbindings,
			       make_empty_list()));
	  }


condition
	: unparenthesized_operand		{ $$ = $1; }
	| '(' type property_list_opt ')'

handler	: expression				{ $$ = $1; }

local_methods
	: METHOD_opt method_definition
		{
		 methnames = cons (FIRST ($2), methnames);
		 methdefs = cons (cons (method_symbol, CDR ($2)), methdefs);
		}
	| METHOD_opt method_definition ',' 
		{
		 methnames = cons (FIRST ($2), methnames); 
		 methdefs = cons (cons (method_symbol, CDR ($2)), methdefs);
		}
	  local_methods

bindings
	: variable '=' expression
		{ $$ = cons (cons ($1, cons ($3, make_empty_list())),
			     make_empty_list()); }
	| '(' variable_list ')' '=' expression
		  { $$ = cons (append_bang ($2, cons ($5, make_empty_list())),
			     make_empty_list()); }

variable_list
	: variable		{ $$ = cons ($1, make_empty_list()); }
	| variable ',' variable_list
				{ $$ = cons ($1, $3); }
	| HASH_REST variable_name 
		{ $$ = cons ($1, cons ($2, make_empty_list())); }


value_variable_list
	: variable		{ $$ = cons ($1, make_empty_list()); }
	| variable ',' value_variable_list
				{ $$ = cons ($1, $3); }
	| HASH_REST variable
		{ $$ = cons ($1, cons ($2, make_empty_list())); }

variable
	: variable_name type_designator_opt
		{ $$ = ($2 ? cons ($1, cons ($2, make_empty_list()))
			: $1); }

variable_name
	: SYMBOL		{ $$ = $1; }
	| defining_word		{ $$ = $1; }

variable_name_opt
	:			{ $$ = NULL; }
	| variable_name		{ $$ = $1; }

type_designator
	: COLON_COLON type	{ $$ = $2; }

type	: operand		{ $$ = $1; }


/* Property Lists */

property_list
	: property			{ $$ = cons ($1, make_empty_list()); }
	| property  property_list	{ $$ = cons ($1, $2); }

property
	: ',' KEYWORD value
		{ $$ = cons ($2, $3); }

value	: expression			{ $$ = $1; }
	| '{' property_set_opt '}'	{ $$ = $2; }

property_set
	: property_set_member		{ $$ = cons ($1, make_empty_list()); }
	| property_set_member ',' property_set		{ $$ = cons ($1, $3); }

property_set_member
	: property_set_item		{ $$ = $1; }
	| property_set_item EQUAL_ARROW property_set_item
		{ $$ = cons ($1, $3); }

property_set_item
	: variable_name			{ $$ = $1; }


/* Optional Items */

EQUAL_ARROW_opt
	:		{ $$ = NULL; }
	| EQUAL_ARROW	{ $$ = $1; }

METHOD_opt
	:
	| METHOD

FUNCTION_opt
	:
	| FUNCTION

CLASS_opt
	:
	| CLASS

TEST_opt
	:
	| TEST

MODULE_opt
	:
	| MODULE

SEMICOLON_opt
	:
	| ';'

comma_opt
	:
	| ','

comma_arguments_opt
	: 		{ $$ = make_empty_list(); }
	| ',' arguments { $$ = $2; }

arguments_opt
	:		{ $$ = make_empty_list(); }
	| arguments	{ $$ = $1; }

constants_opt
	:		{ $$ = make_empty_list(); }
	| constants	{ $$ = $1; }

list_constants_opt
	: 				{ $$ = cons (list_symbol,
						     make_empty_list()); }
	| list_constants		{ $$ = $1; }

default_opt
	:		{ $$ = NULL; }
	| default	{ $$ = $1; }

/*
defining_word_opt
	:
	| defining_word

expression_list_opt
	:
	| expression_list
*/

expressions_opt
	:			{ $$ = make_empty_list(); }
	| expressions		{ $$ = $1; }

/*
item_list_opt
	:
	| item_list
*/

modifiers_opt
	:			{ $$ = make_empty_list(); }
	| modifiers		{ $$ = $1; }

parameter_list_opt
	:			{ $$ = make_empty_list(); }
	| parameter_list	{ $$ = $1; }

property_list_opt
	:			{ $$ = make_empty_list(); }
	| property_list		{ $$ = $1; }

property_set_opt
	:			{ $$ = make_empty_list(); }
	| property_set		{ $$ = $1; }

type_designator_opt
	:			{ $$ = NULL; }
	| type_designator	{ $$ = $1; }

value_list_opt
	:			{ $$ = make_empty_list(); }
	| value_variable_list		{ $$ = $1; }

%%

void yyerror(char *s)
{
    char line_str[20];
    sprintf (line_str, " [line #%d]", yylineno);
    warning (s,
	     make_byte_string (yytext),
	     make_byte_string (line_str),
	     NULL);
}

/*
 * append_bang appends l2 to l1 if l1 is nonempty.
 * if l1 is empty, it just returns l2.
 */	
static Object
append_bang(Object l1, Object l2)
{
    Object res = l1;

    if (EMPTYLISTP (l1)) {
        return l2;
#if 0
	fprintf(stderr, "Whoa! append! to empty_list!\n");
	abort();
#endif
    }
    while (PAIRP (CDR (l1))) {
	l1 = CDR (l1);
    }
    CDR (l1) = l2;
    return res;
}

/*
 *  nelistem lists together objects with the following provisos:
 *  any objects aside from the first that are empty lists are not
 *  consed into the result.
 */
static Object
nelistem (Object car,...)
{
	Object fst, el, acons, cur;
	va_list args;

	fst = cur = acons = cons (car, make_empty_list ());
	va_start (args, car);
	el = va_arg (args, Object);

	while (el) {
	    if (!EMPTYLISTP (el)) {
		acons = cons (el, make_empty_list());
		CDR (cur) = acons;
		cur = acons;
	    }
	    el = va_arg (args, Object);
	}
	va_end (args);
	return (fst);
}
	    
static void
push_bindings()
{
    binding_stack = cons (make_integer(0), binding_stack);
}

static void
pop_bindings()
{
    binding_stack = CDR (binding_stack);
}


static Object
bindings_top ()
{
    return CAR (binding_stack);
}

static void
bindings_increment ()
{
    CAR (binding_stack) = make_integer (INTVAL (CAR (binding_stack)) + 1);
}

static Object
make_setter_expr (Object place, Object value)
{
    Object newsym;

    if ( ! PAIRP (place)) {
	error("Trying to make a setter from something that's not a place",
	      place, NULL);
    }
    newsym = gensym(1);
    return listem (unbinding_begin_symbol,
		   make_integer (1),
		   cons (local_bind_symbol,
			 cons(cons(cons(newsym,
					cons (value,
					      make_empty_list())),
				   make_empty_list()),
			      make_empty_list())),
		   cons (make_setter_symbol (FIRST (place)),
			 cons (newsym, CDR (place))),
		   newsym,
		   NULL);
}

#define	GENSYM_BUFSIZE	128
char gensymbuf[GENSYM_BUFSIZE];

static Object
gensym(int i)
{
    sprintf( gensymbuf, "\"tmp%d", i);
    return make_symbol (gensymbuf);
}

static int
allocation_word (Object word)
{
    return (id (word, instance_symbol) || id (word, class_symbol)
	    || id (word, each_subclass_symbol) || id (word, constant_symbol)
	    || id (word, virtual_symbol) || id (word, inherited_symbol));
}

#ifdef OPTIMIZE_SPECIALIZERS
static void
symtab_push_begin ()
{
    symtab = cons (make_table (DEFAULT_TABLE_SIZE), symtab);
}

static void
symtab_insert_bindings (Object bindings)
{
    Object variable;

#if 0
    warning ("Got symtab_insert_bindings", bindings, NULL);
#endif

    /*
     * bindings created by parser are always of form
     *  #( #( variable1, variable2, ... , variablen, values))
     */
    bindings = CAR (bindings);
    while ( !EMPTYLISTP (CDR (bindings))) {
	variable = CAR (bindings);
	if (PAIRP (variable)) {
#if 0
	    warning ("  symtab element",
		     CAR (variable),
		     SECOND (variable),
		     NULL);
#endif
	    table_element_setter (CAR (symtab),
				  CAR (variable),
				  SECOND (variable));
	}
	bindings = CDR (bindings);
    }
}

static void
symtab_push_parameters (Object parameters)
{
    Object variable;

    symtab = cons (make_table (DEFAULT_TABLE_SIZE), symtab);
#if 0
    warning ("Got symtab_insert_parameters", parameters, NULL);
#endif

    while (PAIRP (parameters)) {
	variable = CAR (parameters);
	if (! PAIRP (variable) && ! SYMBOLP (variable) ) {
	    /* we got to a keyword parameter or a hash-word */
	    break;
	}
	if (PAIRP (variable)) {
#if 0
   warning ("  symtab element",
		     CAR (variable),
		     SECOND (variable),
		     NULL);
#endif
	    table_element_setter (CAR (symtab),
				  CAR (variable),
				  SECOND (variable));
	}
	parameters = CDR (parameters);
    }
}

static void
symtab_pop ()
{
    symtab = CDR (symtab);
}
#endif
