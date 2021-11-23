/*

   classprec-clos.c

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

   Copyright, 1994, Joseph N. Wilson.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include <marlais/classprec.h>

#include <marlais/list.h>
#include <marlais/print.h>

/* Internal function declarations */

static void print_pnode (Object pnode);
static void print_slist (Object slist);

static void construct_slist (Object *sptr, Object class);
static void insert_precedes_arc (Object pred_node, Object succ_node);
static void insert_succeeds_arc (Object succ_node, Object pred_node);
static void record_precedence (Object slist, Object pred_class,
                               Object succ_class);
static void decorate_slist_with_precedence (Object slist, Object class);
static void remove_one_predecessor_arc (Object node, Object node_to_remove);
static void remove_predecessor_arcs (Object node);
static void remove_one_successor_arc (Object node, Object node_to_remove);
static void remove_successor_from_predecessors (Object node);
static void remove_node_from_slist (Object *slist, Object node);
static int direct_superclassp (Object super, Object sub);
static Object find_minimal_elements (Object slist);

/* Internal macros */

#define MAKE_PNODE(class) \
  (marlais_cons (class, \
         marlais_cons (marlais_make_nil (),               \
               (marlais_cons (marlais_make_nil (),                \
                      marlais_make_nil ())))))

#define PNODE_CLASS(pnode)        (CAR (pnode))
#define PNODE_SUCCESSORS(pnode)   (CAR (CDR (pnode)))
#define PNODE_PREDECESSORS(pnode) (CAR (CDR (CDR (pnode))))

/* Exported functions */

Object
marlais_compute_class_precedence_list (Object class)
{
  Object slist, class_list, *candidate_list_ptr;
  Object minimal_element_set, node;
  Object precedence_list_rev;
  Object a_set;

  slist = marlais_make_nil ();

  /* Prepare list S */
  construct_slist (&slist, class);
  decorate_slist_with_precedence (slist, class);

#if EBUG
  fprintf (stdout, "** Initial slist **\n  ");
  print_slist (slist);
  fprintf (stdout, "\n");
#endif

  /*
    We now have the slist decorated with all the precedences in
    a graph initially rooted at the node for class.  At each step, we
    remove the appropriate minimal element from the graph
    (there may be more than one), and insert it into the precedence
    list.  The prec list is maintained in reverse order.

  */

  precedence_list_rev = marlais_make_nil ();
  minimal_element_set = marlais_cons (CAR (slist), marlais_make_nil ());
  while (PAIRP (minimal_element_set)) {

#if EBUG
    fprintf (stdout, " ** Minimal elements **\n   ");
    for (a_set = minimal_element_set;
         PAIRP (a_set);
         a_set = CDR (a_set)) {
      marlais_print_object (standard_output_symbol,
                            CLASSNAME (PNODE_CLASS (CAR (a_set))),
                            0);
      fprintf (stdout, " ");
    }
    fprintf (stderr, "\n");
#endif
    if (EMPTYLISTP (CDR (minimal_element_set))) {
      /*
       * There is a unique minimal element in the graph.
       *  Add it to the precedence list, remove it from the
       * predecessors list of all its successors, and remove
       *  it from slist.
       */
      node = CAR (minimal_element_set);
      remove_predecessor_arcs (node);
      precedence_list_rev = marlais_cons (PNODE_CLASS (node),
                                  precedence_list_rev);
      remove_node_from_slist (&slist, node);
      minimal_element_set = find_minimal_elements (slist);
    } else {
      /*
       * We have more than one minimal element.
       * We must choose the one that has a direct subclass that is
       * furthest to the right in the precedence list.  That is,
       * the one that is closest to the head of precedence_list_rev.
       */
      for (class_list = precedence_list_rev;
           PAIRP (class_list);
           class_list = CDR (class_list)) {
        for (candidate_list_ptr = &minimal_element_set;
             PAIRP (*candidate_list_ptr);
             candidate_list_ptr = &CDR (*candidate_list_ptr)) {
          if (direct_superclassp (PNODE_CLASS (CAR (*candidate_list_ptr)),
                                  CAR (class_list))) {
            break;
          }
        }
        if (PAIRP (*candidate_list_ptr)) {
          /* We found the right candidate.
           * Remove the predecessor arcs for this node.
           * Remove it from the slist.
           * Update the precedence list.
           * Update the min. element set
           */
          node = CAR (*candidate_list_ptr);
          remove_predecessor_arcs (node);
          precedence_list_rev = marlais_cons (PNODE_CLASS (node),
                                      precedence_list_rev);
          remove_node_from_slist (&slist, node);
          *candidate_list_ptr = CDR (*candidate_list_ptr);
#if 0
          /* This interesting bug leads to a monotonic class
           * ordering. Such an ordering is, in a sense more
           * desirable than the class ordering that is yielded
           * by the normal rule.
           */
          if (EMPTYLISTP (minimal_element_set)) {
            minimal_element_set = find_minimal_elements (slist);
          }
#else
          minimal_element_set = find_minimal_elements (slist);
#endif
          break;
        }
      }
      if (EMPTYLISTP (class_list)) {
        marlais_error ("Whoa!  the class list was empty making precedence list",
                       NULL);
      }
    }
  }
  if (PAIRP (slist)) {
    marlais_error ("Unable to construct class precedence list", class, NULL);
  }
  /* Cache the result */
  CLASSPRECLIST (class) = list_reverse_bang (precedence_list_rev);
  return CLASSPRECLIST (class);
}

/* Internal functions */

static void
print_pnode (Object pnode)
{
  Object nlist;

  fprintf (stderr, "[%s]\n Successors: ",
           SYMBOLNAME (CLASSNAME (PNODE_CLASS (pnode))));
  for (nlist = PNODE_SUCCESSORS (pnode);
       PAIRP (nlist);
       nlist = CDR (nlist)) {
    fprintf (stderr, "%s ",
             SYMBOLNAME (CLASSNAME (PNODE_CLASS (CAR (nlist)))));
  }
  fprintf (stderr, "\n Predecessors: ");
  for (nlist = PNODE_PREDECESSORS (pnode);
       PAIRP (nlist);
       nlist = CDR (nlist)) {
    fprintf (stderr, "%s ",
             SYMBOLNAME (CLASSNAME (PNODE_CLASS (CAR (nlist)))));
  }
  fprintf (stderr, "\n");
}

static void
print_slist (Object slist)
{
  Object p;

  for (p = slist; PAIRP (p); p = CDR (p)) {
    marlais_print_object (standard_output_symbol, CLASSNAME (PNODE_CLASS (CAR (p))), 0);
    fprintf (stdout, " ");
  }
}

static void
construct_slist (Object *sptr, Object class)
{
  Object *tmp_sptr = sptr;
  Object sclist;

  while (PAIRP (*tmp_sptr)) {
    if (class == PNODE_CLASS (CAR (*tmp_sptr)))
      break;
    tmp_sptr = &CDR (*tmp_sptr);
  }
  if (EMPTYLISTP (*tmp_sptr)) {
    *tmp_sptr = marlais_cons (MAKE_PNODE (class), marlais_make_nil ());
  }
  for (sclist = CLASSSUPERS (class);
       PAIRP (sclist);
       sclist = CDR (sclist)) {
    construct_slist (sptr, CAR (sclist));
  }
}

static void
insert_precedes_arc (Object pred_node, Object succ_node)
{
  add_new_at_end (&PNODE_SUCCESSORS (pred_node), (succ_node));
}

static void
insert_succeeds_arc (Object succ_node, Object pred_node)
{
  add_new_at_end (&PNODE_PREDECESSORS (succ_node), pred_node);
/*
  PNODE_PREDECESSORS (succ_node) = marlais_cons (pred_node,
  PNODE_PREDECESSORS (succ_node));
 */
}

static void
record_precedence (Object slist, Object pred_class, Object succ_class)
{
  Object q, p;

  /* find nodes associated with pred and succ classes */
  for (q = slist; PNODE_CLASS (CAR (q)) != pred_class; q = CDR (q)) ;
  for (p = slist; PNODE_CLASS (CAR (p)) != succ_class; p = CDR (p)) ;
  insert_precedes_arc (CAR (q), CAR (p));
  insert_succeeds_arc (CAR (p), CAR (q));
}

static void
decorate_slist_with_precedence (Object slist, Object class)
{
  Object q, p;

  for (q = marlais_cons (class, marlais_make_nil ()), p = CLASSSUPERS (class);
       PAIRP (p);
       q = p, p = CDR (p)) {
    record_precedence (slist, CAR (q), CAR (p));
  }
  for (p = CLASSSUPERS (class); PAIRP (p); p = CDR (p)) {
    decorate_slist_with_precedence (slist, CAR (p));
  }
}

static void
remove_predecessor_arcs (Object node)
{
  Object succ_list;

  for (succ_list = PNODE_SUCCESSORS (node);
       PAIRP (succ_list);
       succ_list = CDR (succ_list)) {
    remove_one_predecessor_arc (CAR (succ_list), node);
  }
}

static void
remove_one_predecessor_arc (Object node, Object node_to_remove)
{
  Object *tmp_ptr;

  for (tmp_ptr = &PNODE_PREDECESSORS (node);
       CAR (*tmp_ptr) != node_to_remove;
       tmp_ptr = &CDR (*tmp_ptr)) ;

  *tmp_ptr = CDR (*tmp_ptr);
}

static void
remove_one_successor_arc (Object node, Object node_to_remove)
{
  Object *tmp_ptr;

  for (tmp_ptr = &PNODE_SUCCESSORS (node);
       CAR (*tmp_ptr) != node_to_remove;
       tmp_ptr = &CDR (*tmp_ptr)) ;

  *tmp_ptr = CDR (*tmp_ptr);
}

static void
remove_successor_from_predecessors (Object node)
{
  Object pred_list;

  for (pred_list = PNODE_PREDECESSORS (node);
       PAIRP (pred_list);
       pred_list = CDR (pred_list)) {
    remove_one_successor_arc (CAR (pred_list), node);
  }
}

static void
remove_node_from_slist (Object *slist, Object node)
{
  Object *tmp_ptr;

  tmp_ptr = slist;
  while (PAIRP (*tmp_ptr)) {
    if (CAR (*tmp_ptr) == node) {
      *tmp_ptr = CDR (*tmp_ptr);
      return;
    }
    tmp_ptr = &CDR (*tmp_ptr);
  }
  /*
   * If we get here, then the object being removed has already been
   * removed by virtue of having been encountered as a successor
   * of a class included twice in the inheritance hierarchy
   */
}

static int
direct_superclassp (Object super, Object sub)
{
  Object supers;

  for (supers = CLASSSUPERS (sub);
       PAIRP (supers);
       supers = CDR (supers)) {
    if (CAR (supers) == super)
      return 1;
  }
  return 0;
}

static Object
find_minimal_elements (Object slist)
{
  Object mins = marlais_make_nil ();

  while (PAIRP (slist)) {
    if (EMPTYLISTP (PNODE_PREDECESSORS (CAR (slist)))) {
      mins = marlais_cons (CAR (slist), mins);
    }
    slist = CDR (slist);
  }
  return mins;
}
