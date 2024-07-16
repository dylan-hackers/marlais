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
#include <marlais/object/sequence.h>

/* Exported functions */

Object
marlais_make_list (Object car,...)
{
    Object fst, el, acons, cur;
    va_list args;

    fst = cur = acons = marlais_cons (car, MARLAIS_NIL);
    va_start (args, car);
    el = va_arg (args, Object);

    while (el) {
        acons = marlais_cons (el, MARLAIS_NIL);
        CDR (cur) = acons;
        cur = acons;
        el = va_arg (args, Object);
    }
    va_end (args);
    return (fst);
}

Object
marlais_copy_list (Object lst)
{
    Object result, *tmp_ptr;

    result = MARLAIS_NIL;
    tmp_ptr = &result;
    for (tmp_ptr = &result;
         PAIRP (lst);
         tmp_ptr = &CDR (*tmp_ptr), lst = CDR (lst)) {
        *tmp_ptr = marlais_cons (CAR (lst), MARLAIS_NIL);
    }
    return result;
}

Object
marlais_cons (Object car, Object cdr)
{
    Object obj = marlais_allocate_object (Pair, sizeof (struct pair));

    CAR (obj) = car;
    CDR (obj) = cdr;
    return (obj);
}

Object
marlais_make_pair_entrypoint (Object args)
{
    return marlais_cons (MARLAIS_FALSE, MARLAIS_FALSE); /* TODO who knows ?? */
}

Object
marlais_make_list_entrypoint (Object args)
{
    int size;
    Object size_obj, fill_obj, res;

    marlais_make_sequence_entry(args, &size, &size_obj, &fill_obj, "<list>");

    /* actually fabricate the list */
    if (size == 0) {
        return (MARLAIS_NIL);
    } else {
        res = MARLAIS_NIL;
        while (size) {
            res = marlais_cons (fill_obj, res);
            size--;
        }
        return (res);
    }
}

Object
marlais_car (Object lst)
{
    return (EMPTYLISTP (lst) ? MARLAIS_NIL : CAR (lst));
}

Object
marlais_cdr (Object lst)
{
    return (EMPTYLISTP (lst) ? MARLAIS_NIL : CDR (lst));
}

int
marlais_list_length (Object lst)
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
        CDR (back_list) = MARLAIS_NIL;

        /* TODO don't modify the list */

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

Object
marlais_second (Object lst)
{
    return SECOND (lst);
}

Object
marlais_third (Object lst)
{
    return THIRD (lst);
}

Object
marlais_map1 (Object (*fun) (Object), Object lst)
{
    if (EMPTYLISTP (lst)) {
        return (MARLAIS_NIL);
    } else {
        return (marlais_cons ((*fun) (CAR (lst)), marlais_map1 (fun, CDR (lst))));
    }
}

Object
marlais_map2 (Object (*fun) (Object, Object), Object l1, Object l2)
{
    if (EMPTYLISTP (l1) || EMPTYLISTP (l2)) {
        return (MARLAIS_NIL);
    } else {
        return (marlais_cons ((*fun) (CAR (l1), CAR (l2)), marlais_map2 (fun, CDR (l1), CDR (l2))));
    }
}

Object
marlais_map_apply1 (Object fun, Object lst)
{
    if (EMPTYLISTP (lst)) {
        return (MARLAIS_NIL);
    } else {
        return (marlais_cons (marlais_apply (fun, marlais_cons (CAR (lst), MARLAIS_NIL)),
                      marlais_map_apply1 (fun, (CDR (lst)))));
    }
}

Object
marlais_map_apply2 (Object fun, Object l1, Object l2)
{
    if (EMPTYLISTP (l1) || EMPTYLISTP (l2)) {
        return (MARLAIS_NIL);
    } else {
        return (marlais_cons (marlais_apply (fun, marlais_make_list (CAR (l1), CAR (l2),
                                                  NULL)),
                      marlais_map_apply2 (fun, CDR (l1), CDR (l2))));
    }
}

Object
marlais_append (Object l1, Object l2)
{
    if (EMPTYLISTP (l1)) {
        return (l2);
    } else {
        return (marlais_cons (CAR (l1), marlais_append (CDR (l1), l2)));
    }
}

Object
marlais_append_bang(Object l1, Object l2)
{
    Object res = l1;

    if (EMPTYLISTP (l1)) {
        return (l2);
    }
    while (PAIRP (CDR (l1))) {
        l1 = CDR (l1);
    }
    CDR (l1) = l2;
    return (res);
}

Object
marlais_list_reverse (Object lst)
{
    Object last;

    last = MARLAIS_NIL;
    while (!EMPTYLISTP (lst)) {
        last = marlais_cons (CAR (lst), last);
        lst = CDR (lst);
    }
    return (last);
}

Object
marlais_list_reverse_bang (Object lst)
{
    Object cur, next;

    cur = MARLAIS_NIL;
    while (!EMPTYLISTP (lst)) {
        next = CDR (lst);
        CDR (lst) = cur;
        cur = lst;
        lst = next;
    }
    return (cur);
}

bool
marlais_member_p (Object obj, Object lst)
{
    while (PAIRP (lst)) {
        if (marlais_identical_p (obj, CAR (lst))) {
            return true;
        }
        lst = CDR (lst);
    }
    return false;
}

bool
marlais_member_test_p (Object obj, Object lst, Object test)
{
    Object l = lst;
    while (!EMPTYLISTP (l)) {
        if (test != MARLAIS_FALSE) {
            if (marlais_apply (test, marlais_make_list (obj, CAR (l), NULL)) != MARLAIS_FALSE) {
                return true;
            }
        } else {
            if (marlais_identical_p (obj, CAR (l))) {
                return true;
            }
        }
        l = CDR (l);
    }
    return false;
}
