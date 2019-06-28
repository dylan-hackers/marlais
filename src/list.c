/*

   list.c

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
   0.6 Revisions Copyright 2001, Douglas M. Auclair.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include "list.h"

#include "apply.h"
#include "boolean.h"
#include "globaldefs.h"
#include "error.h"
#include "number.h"
#include "prim.h"
#include "symbol.h"
#include "sequence.h"

/* globals */

/* primitives */

/* local function prototypes */
static Object car (Object pair);
static Object cdr (Object pair);
static Object first (Object pair, Object default_ob);
static Object second_d (Object pair, Object default_ob);
static Object third_d (Object pair, Object default_ob);
static Object set_car (Object pair, Object val);
static Object set_cdr (Object pair, Object val);
static Object list_element (Object pair, Object index, Object default_ob);
static Object list_element_setter (Object pair, Object index, Object obj);
static Object list_last (Object lst, Object default_ob);
Object list_sort (Object lst, Object test);
Object list_sort_bang (Object lst, Object test);

static struct primitive list_prims[] =
{
    {"%pair", prim_2, cons},
    {"%head", prim_1, car},
    {"%tail", prim_1, cdr},
    {"%first", prim_2, first},
    {"%second", prim_2, second_d},
    {"%third", prim_2, third_d},
    {"%head-setter", prim_2, set_car},
    {"%tail-setter", prim_2, set_cdr},
    {"%list-element", prim_3, list_element},
    {"%list-element-setter", prim_3, list_element_setter},
    {"%list-map1", prim_2, list_map1},
    {"%list-append", prim_2, append},
    {"%list-member?", prim_3, member_p},
    {"%list-reduce", prim_3, list_reduce},
    {"%list-reduce1", prim_2, list_reduce1},
    {"%list-length", prim_1, list_length_int},
    {"%list-reverse!", prim_1, list_reverse_bang},
    {"%list-reverse", prim_1, list_reverse},
    {"%list-last", prim_2, list_last},
    {"%list-sort", prim_2, list_sort},
    {"%list-sort!", prim_2, list_sort_bang},
};

void
init_list_prims (void)
{
    int num;

    num = sizeof (list_prims) / sizeof (struct primitive);

    init_prims (num, list_prims);
}

#ifndef SMALL_OBJECTS

/*
 * Cheesy global to make make_empty_list run faster
 */

static Object ___empty_list = NULL;

#endif

/*
 * Creates a unique empty_list value for use in constructing lists
 */
void
initialize_empty_list ()
{

#ifndef SMALL_OBJECTS
    Object obj;

    if (___empty_list == NULL) {
	___empty_list = allocate_object (sizeof (struct object));

	TYPE (___empty_list) = EmptyList;
    } else {
	error ("initialize_empty_list: second attempt at initialization",
	       NULL);
    }
#endif
}

#ifndef SMALL_OBJECTS
/*
 * Returns the unique empty_list value
 */
Object
make_empty_list (void)
{
    return ___empty_list;
}
#endif

/* This gets called with (make <pair> args)
 * Added to pass conformance tests.
 */
Object
make_pair_driver (Object args)
{
    return cons (false_object, false_object);	/* who knows ?? */
}

/* This gets called with (make <list> args) */
Object
make_list_driver (Object args)
{
    int size;
    Object size_obj, fill_obj, res;

    make_sequence_driver(args, &size, &size_obj, &fill_obj, "<list>");

    /* actually fabricate the list */
    if (size == 0) {
	return (make_empty_list ());
    } else {
	res = make_empty_list ();
	while (size) {
	    res = cons (fill_obj, res);
	    size--;
	}
	return (res);
    }
}

Object
cons (Object car, Object cdr)
{
    Object obj = allocate_object (sizeof (struct pair));

    PAIRTYPE (obj) = Pair;
    CAR (obj) = car;
    CDR (obj) = cdr;
    return (obj);
}

static Object
car (Object lst)
{
    return (EMPTYLISTP (lst) ? lst : CAR (lst));
}

static Object
cdr (Object lst)
{
    return (EMPTYLISTP (lst) ? lst : CDR (lst));
}

static Object nth(Object lst, Object default_ob, const char* where,
		  int test, Object (*fn)(Object))
{
    if (test) {
	return (*fn)(lst);
    } else if (default_ob == default_object) {
	char err_msg[80];
	sprintf(err_msg, "list has no %s element", where);
	return error (err_msg, lst, NULL);
    } else {
	return default_ob;
    }
}

static Object
first (Object lst, Object default_ob)
{
  return nth(lst, default_ob, "first", PAIRP(lst), car);
}

Object
second (Object lst)
{
    return SECOND (lst);
}

static Object
second_d (Object lst, Object default_ob)
{
  return nth(lst, default_ob, "second", PAIRP(lst) && PAIRP(CDR(lst)), second);
}

Object
third (Object lst)
{
    return THIRD (lst);
}

static Object
third_d (Object lst, Object default_ob)
{
  return nth(lst, default_ob, "third",
	     PAIRP (lst) && PAIRP (CDR (lst)) && PAIRP (CDR (CDR (lst))),
	     third);
}

Object
map (Object (*fun) (Object), Object lst)
{
    if (EMPTYLISTP (lst)) {
	return (make_empty_list ());
    } else {
	return (cons ((*fun) (CAR (lst)), map (fun, CDR (lst))));
    }
}

Object
map2 (Object (*fun) (Object, Object), Object l1, Object l2)
{
    if (EMPTYLISTP (l1) || EMPTYLISTP (l2)) {
	return (make_empty_list ());
    } else {
	return (cons ((*fun) (CAR (l1), CAR (l2)), map2 (fun, CDR (l1), CDR (l2))));
    }
}

Object
list_map1 (Object fun, Object lst)
{
    if (EMPTYLISTP (lst)) {
	return (make_empty_list ());
    } else {
	return (cons (apply (fun, cons (CAR (lst), make_empty_list ())),
		      list_map1 (fun, (CDR (lst)))));
    }
}

Object
list_map2 (Object fun, Object l1, Object l2)
{
    if (EMPTYLISTP (l1) || EMPTYLISTP (l2)) {
	return (make_empty_list ());
    } else {
	return (cons (apply (fun, listem (CAR (l1), CAR (l2),
					  NULL)),
		      list_map2 (fun, CDR (l1), CDR (l2))));
    }
}

Object
list_length_int (Object lst)
{
    int len = list_length (lst);

    if (len < 0) {
	return false_object;
    } else {
	return make_integer (len);
    }
}

Object
append (Object l1, Object l2)
{
    if (EMPTYLISTP (l1)) {
	return (l2);
    } else {
	return (cons (CAR (l1), append (CDR (l1), l2)));
    }
}

int
member (Object obj, Object lst)
{
    while (PAIRP (lst)) {
	if (obj == CAR (lst)) {
	    return 1;
	}
	lst = CDR (lst);
    }
    return 0;
}

Object
member_p (Object obj, Object lst, Object test)
{
    Object l = lst;
    while (!EMPTYLISTP (l)) {
	if (test != false_object) {
	    if (apply (test, listem (obj, CAR (l), NULL)) != false_object) {
		return (true_object);
	    }
	} else {
	    if (id_p (obj, CAR (l), make_empty_list ()) != false_object) {
		return (true_object);
	    }
	}
	l = CDR (l);
    }
    return (false_object);
}

Object
listem (Object car,...)
{
    Object fst, el, acons, cur;
    va_list args;

    fst = cur = acons = cons (car, make_empty_list ());
    va_start (args, car);
    el = va_arg (args, Object);

    while (el) {
	acons = cons (el, make_empty_list ());
	CDR (cur) = acons;
	cur = acons;
	el = va_arg (args, Object);
    }
    va_end (args);
    return (fst);
}

Object
list_reduce (Object fun, Object init, Object lst)
{
    Object val;

    val = init;
    while (!EMPTYLISTP (lst)) {
	val = apply (fun, listem (val, CAR (lst), NULL));
	lst = CDR (lst);
    }
    return (val);
}

Object
list_reduce1 (Object fun, Object lst)
{
    return list_reduce(fun, CAR(lst), CDR(lst));
}

int
list_length (Object lst)
{
    int len;
    Object fore_list, back_list, next;

    if (EMPTYLISTP (lst)) {
	return 0;
    } else if (CDR (lst) == lst) {
	return -1;
    } else {
	len = 1;
	back_list = lst;
	fore_list = CDR (lst);
	CDR (back_list) = make_empty_list ();

	/* Reverse pointers in the list and see if we end up at the head. */
	while (PAIRP (fore_list)) {
	    next = CDR (fore_list);
	    CDR (fore_list) = back_list;
	    back_list = fore_list;
	    fore_list = next;
	    len++;
	}
	if ((back_list == lst) && (PAIRP (CDR (back_list)))) {
	    /* We ended up at the head and had at least 2 elements,
	     *  thus there must be a cycle.
	     */
	    len = -1;
	}
	/* Reverse the pointers again to repair the list. */
	while (PAIRP (back_list)) {
	    next = CDR (back_list);
	    CDR (back_list) = fore_list;
	    fore_list = back_list;
	    back_list = next;
	}
	return len;
    }
}

static Object
set_car (Object pair, Object val)
{
    CAR (pair) = val;
    return (val);
}

static Object
set_cdr (Object pair, Object val)
{
    CDR (pair) = val;
    return (val);
}

static Object
list_element (Object pair, Object index, Object default_ob)
{
    int i;
    Object lst;

    i = INTVAL (index);
    lst = pair;
    if (EMPTYLISTP (lst)) {
	if (default_ob == default_object) {
	    error ("element: no such element", index, pair, NULL);
	} else {
	    return default_ob;
	}
    }
    while (i) {
	i--;
	lst = CDR (lst);
	if (EMPTYLISTP (lst)) {
	    if (default_ob == default_object) {
		error ("element: no such element", index, pair, NULL);
	    } else {
		return default_ob;
	    }
	}
    }
    return (CAR (lst));
}

static Object
list_element_setter (Object pair, Object index, Object obj)
{
    int i, el;
    Object lst;

    i = 0;
    el = INTVAL (index);
    lst = pair;
    if (EMPTYLISTP (lst)) {
	return error ("element-setter: list is empty", NULL);
    }
    while (!EMPTYLISTP (lst)) {
	if (i == el) {
	    CAR (lst) = obj;
	    return (obj);
	}
	i++;
	lst = CDR (lst);
    }
    return error ("element-setter: index too large for list",
		  pair,
		  index,
		  NULL);
}

Object
list_reverse_bang (Object lst)
{
    Object cur, next;

    cur = make_empty_list ();
    while (!EMPTYLISTP (lst)) {
	next = CDR (lst);
	CDR (lst) = cur;
	cur = lst;
	lst = next;
    }
    return (cur);
}

Object
list_reverse (Object lst)
{
    Object last;

    last = make_empty_list ();
    while (!EMPTYLISTP (lst)) {
	last = cons (CAR (lst), last);
	lst = CDR (lst);
    }
    return (last);
}

static Object
list_last (Object lst, Object default_ob)
{
    Object last;

    if (EMPTYLISTP (lst)) {
	if (default_ob == default_object) {
	    error ("attempt to get last of empty list", NULL);
	} else {
	    return default_ob;
	}
    }
    while (!EMPTYLISTP (lst)) {
	last = CAR (lst);
	lst = CDR (lst);
    }
    return (last);
}

int
list_equal (Object l1, Object l2)
{
    if (id_p (l1, l2, make_empty_list ()) != false_object) {
	return (1);
    }
    if (PAIRP (l1) && PAIRP (l2)) {
	return (list_equal (CAR (l1), CAR (l2)) &&
		list_equal (CDR (l1), CDR (l2)));
    } else {
	return (0);
    }
}

Object
copy_list (Object lst)
{
    Object result, *tmp_ptr;

    result = make_empty_list ();
    tmp_ptr = &result;
    for (tmp_ptr = &result;
	 PAIRP (lst);
	 tmp_ptr = &CDR (*tmp_ptr), lst = CDR (lst)) {
	*tmp_ptr = cons (CAR (lst), make_empty_list ());
    }
    return result;
}

Object
add_new_at_end (Object *lst, Object elt)
{
    Object ret = *lst;

    while (PAIRP (*lst)) {
	if (CAR (*lst) == elt) {
	    return ret;
	}
	lst = &CDR (*lst);
    }
    *lst = cons (elt, make_empty_list ());
    return ret;
}


/* Can't use qsort in sorting as use of test function will call
 * function_specializers, which applies qsort, leading to all sorts
 * or horrid consequences, as Unix qsort is not multi-thread or
 * hierarchically nestable (even though it could be!
 */

Object
list_sort (Object lst, Object test)
{
    /* Fill this in! */
}

Object
list_sort_bang (Object lst, Object test)
{
    /* Fill this in! */
}
