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

#include <marlais/object/list.h>

#include <marlais/core/apply.h>
#include <marlais/core/eval.h>
#include <marlais/object/prim.h>
#include <marlais/object/sequence.h>

/* Primitives */

static Object prim_head_setter (Object pair, Object val);
static Object prim_tail_setter (Object pair, Object val);

static Object prim_list_first (Object pair, Object default_ob);
static Object prim_list_second (Object pair, Object default_ob);
static Object prim_list_third (Object pair, Object default_ob);
static Object prim_list_last (Object lst, Object default_ob);

static Object prim_list_length (Object lst);
static Object prim_list_element (Object pair, Object index, Object default_ob);
static Object prim_list_element_setter (Object pair, Object index, Object obj);
static Object prim_list_member_p (Object obj, Object lst, Object test);

static Object prim_list_reduce (Object fun, Object init, Object lst);
static Object prim_list_reduce1 (Object fun, Object lst);

static struct primitive list_prims[] =
{
    {"list", prim_0_rest, marlais_copy_list},
    {"pair", prim_2, marlais_cons},

    {"%head", prim_1, marlais_car},
    {"%tail", prim_1, marlais_cdr},
    {"%head-setter", prim_2, prim_head_setter},
    {"%tail-setter", prim_2, prim_tail_setter},

    {"%list-first", prim_2, prim_list_first},
    {"%list-second", prim_2, prim_list_second},
    {"%list-third", prim_2, prim_list_third},
    {"%list-last", prim_2, prim_list_last},

    {"%list-length",         prim_1, prim_list_length},
    {"%list-element",        prim_3, prim_list_element},
    {"%list-element-setter", prim_3, prim_list_element_setter},

    {"%list-member?", prim_3, prim_list_member_p},

    {"%list-append",  prim_2, marlais_append},
    {"%list-append!", prim_2, marlais_append_bang}, /* not used yet */

    {"%list-reverse",  prim_1, marlais_list_reverse},
    {"%list-reverse!", prim_1, marlais_list_reverse_bang},

    {"%list-map1", prim_2, marlais_map_apply1},
    {"%list-reduce", prim_3, prim_list_reduce},
    {"%list-reduce1", prim_2, prim_list_reduce1},
};

/* Exported functions */

void
marlais_register_list (void)
{
  MARLAIS_REGISTER_PRIMS (list_prims);
}

static Object nth(Object lst, Object default_ob, const char* where,
                  int test, Object (*fn)(Object))
{
    if (test) {
        return (*fn)(lst);
    } else if (default_ob == marlais_default) {
        char err_msg[80];
        sprintf(err_msg, "list has no %s element", where);
        return marlais_error (err_msg, lst, NULL);
    } else {
        return default_ob;
    }
}

static Object
prim_list_first (Object lst, Object default_ob)
{
  return nth(lst, default_ob, "first",
             PAIRP(lst),
             marlais_car);
}

static Object
prim_list_second (Object lst, Object default_ob)
{
  return nth(lst, default_ob, "second",
             PAIRP(lst) && PAIRP(CDR(lst)),
             marlais_second);
}

static Object
prim_list_third (Object lst, Object default_ob)
{
  return nth(lst, default_ob, "third",
             PAIRP (lst) && PAIRP (CDR (lst)) && PAIRP (CDR (CDR (lst))),
             marlais_third);
}

static Object
prim_list_last (Object lst, Object default_ob)
{
    Object last = MARLAIS_UNSPECIFIED;

    if (EMPTYLISTP (lst)) {
        if (default_ob == marlais_default) {
            marlais_error ("attempt to get last of empty list", NULL);
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

static Object
prim_list_member_p (Object obj, Object lst, Object test)
{
  return marlais_make_boolean (marlais_member_test_p (obj, lst, test));
}

static Object
prim_head_setter (Object pair, Object val)
{
    CAR (pair) = val;
    return (val);
}

static Object
prim_tail_setter (Object pair, Object val)
{
    CDR (pair) = val;
    return (val);
}

static Object
prim_list_length (Object lst)
{
    int len = marlais_list_length (lst);

    if (len < 0) {
        return MARLAIS_FALSE;
    } else {
        return marlais_make_integer (len);
    }
}

static Object
prim_list_element (Object pair, Object index, Object default_ob)
{
    int i;
    Object lst;

    i = INTVAL (index);
    lst = pair;
    if (EMPTYLISTP (lst)) {
        if (default_ob == marlais_default) {
            marlais_error ("element: no such element", index, pair, NULL);
        } else {
            return default_ob;
        }
    }
    while (i) {
        i--;
        lst = CDR (lst);
        if (EMPTYLISTP (lst)) {
            if (default_ob == marlais_default) {
                marlais_error ("element: no such element", index, pair, NULL);
            } else {
                return default_ob;
            }
        }
    }
    return (CAR (lst));
}

static Object
prim_list_element_setter (Object pair, Object index, Object obj)
{
    int i, el;
    Object lst;

    i = 0;
    el = INTVAL (index);
    lst = pair;
    if (EMPTYLISTP (lst)) {
        return marlais_error ("element-setter: list is empty", NULL);
    }
    while (!EMPTYLISTP (lst)) {
        if (i == el) {
            CAR (lst) = obj;
            return (obj);
        }
        i++;
        lst = CDR (lst);
    }
    return marlais_error ("element-setter: index too large for list",
                          pair,
                          index,
                          NULL);
}

static Object
prim_list_reduce (Object fun, Object init, Object lst)
{
    Object val;

    val = init;
    while (!EMPTYLISTP (lst)) {
        val = marlais_apply (fun, marlais_make_list (val, CAR (lst), NULL));
        lst = CDR (lst);
    }
    return (val);
}

static Object
prim_list_reduce1 (Object fun, Object lst)
{
    return prim_list_reduce(fun, CAR(lst), CDR(lst));
}
