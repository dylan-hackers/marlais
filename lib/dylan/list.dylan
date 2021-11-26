module: dylan

//
// list.dyl - list operations
//
// Brent Benson
//

//
// pair
//

define method pair (car, cdr)
  %pair(car, cdr);
end method pair;

//
// list
//

// NOTE This is important and must be loaded early.
//      It might be better implemented as a primitive.
//define method list (#rest els)
//  els
//end method list;

//
// type-for-copy
//

define method type-for-copy (p :: <pair>)
  <list>;
end method type-for-copy;

define method type-for-copy (l == #())
  <list>;
end method type-for-copy;

//
// head
//

define method head (lst :: <list>) => result :: <object>;
  %head(lst);
end method head;

//
// head-setter
//

define method head-setter (obj, p :: <pair>)
  %head-setter(p, obj);
end method head-setter;

//
// tail
//

define method tail (lst :: <list>) => result :: <object>;
  %tail(lst);
end method tail;

//
// tail-setter
//

define method tail-setter (obj, p :: <pair>)
  %tail-setter(p, obj);
end method tail-setter;

//
// sequence GF methods for <list>
//

define method add (l :: <list>, el)
  pair(el, copy-sequence(l)); 	// can't share structure.
end method add;

define method add! (l :: <list>, el)
  pair(el, l);
end method add!;

define method add-new (l :: <list>, el, #key test = \==)
  if (~member? (el, l, test: test))
    add (l, el)
  else
    l
  end if;
end method add-new;

define method add-new! (l :: <list>, el, #key test = \==)
  if (~member? (el, l, test: test))
    add! (l, el)
  else
    l
  end if;
end method add-new;

define method remove (l :: <list>, el, #key test = \==, count)
  local method help (lst, el, c)
	  case
	    empty? (lst) => #();
	    test (lst.head, el) => if (count & count <= c)
				   copy-sequence(lst);
				 else
				   help (lst.tail, el, c + 1);
				 end if;
	    otherwise => pair (lst.head, help (lst.tail, el, c));
	  end case;
	end method help;
  help (l, el, 0);
end method remove;

//
// remove! does not currently share any storage!
//
define method remove! (l :: <list>, el, #key test = \==, count)
  local method help (lst, el, c)
	  case
	    empty? (lst) => #();
	    test (lst.head, el) => if (count & count <= c)
				   copy-sequence(lst);
				 else
				   help (lst.tail, el, c + 1);
				 end if;
	    otherwise => pair (lst.head, help (lst.tail, el, c));
	  end case;
	end method help;
  help (l, el, 0);
end method remove!;

define method choose (pred :: <function>, l :: <list>)
  case
    empty? (l) => l;
    pred (l.head) => pair (l.head, choose (pred, l.tail));
    otherwise => choose (pred, l.tail);
  end case;
end method choose;

define method choose-by (pred :: <function>,
			 test-list :: <list>,
			 value-list :: <list>)
  case
    empty? (test-list) & empty? (value-list) => #();
    empty? (test-list) | empty? (value-list)
      => error ("choose-by: test-list and value-list must have same size",
		test-list,
		value-list);
    pred (test-list.head) => pair (value-list.head,
				   choose-by (pred,
					      test-list.tail,
					      value-list.tail));
    otherwise => choose-by (pred, test-list.tail, value-list.tail);
  end case;
end method choose-by;

//
// intersection is not stable.  Elements are presented in reverse order
// from their appearance in l1.
//
define method intersection (l1 :: <list>, l2 :: <list>, #key test = \==)
  let result = #();
  for (cur in l1)
    if (member? (cur, l2, test: test))
      result := pair (cur, result)
    end if;
  finally
    result;
  end for;
end method intersection;

define method union (l1 :: <list>, l2 :: <list>, #key test = \==)
  for (cur in l1)
    l2 := add-new! (l2, cur, test: test);
  finally
    l2;
  end for;
end method union;

define method remove-duplicates (l :: <list>, #key test = \==)
  local method help (l)
	  case
	    empty? (l) => #();
	    member? (l.head, l.tail, test: test) => help (l.tail);
	    otherwise => pair (l.head, help (l.tail));
	  end case;
	end help;
  help (l);
end method remove-duplicates;

define method concatenate (s :: <list>, #rest more-sequences)
  local method help (s :: <sequence>, more :: <list>)
	  if (empty? (more))
	    s;
	  else
	    help (%list-append (s, more.head), more.tail);
	  end if;
	end method help;
  help (s, more-sequences);
end method concatenate;

define method reverse (l :: <list>)
  %list-reverse(l);
end method reverse;

define method reverse! (l :: <list>)
  %list-reverse!(l);
end method reverse!;

define method sort(a :: <list>, #key test = \<, stable = #f)
  as(<list>, sort!(as(<vector>, a), test: test, stable: stable))
end method sort; // <list>

define method sort!(a :: <list>, #key test = \<, stable = #f)
  sort(a, test: test, stable: stable)
end method sort!; // <list>

define method first-setter (obj, l :: <list>)
  %head-setter (l, obj);
end method first-setter;

define method second-setter (obj, l :: <list>)
  %head-setter (l.tail, obj);
end method second-setter;

define method third-setter (obj, l :: <list>)
  %head-setter (l.tail.tail);
end method third-setter;

define method last (l :: <list>, #key default = %default-object)
  %list-last (l, default);
end method last;

//
// faster versions of collection operations for <list>.
//

define method size (l :: <list>)
  %list-length(l);
end method size;

define method empty? (l == #())
  #t;
end method empty?;

define method empty? (l :: <list>)
  #f;
end method empty?;

define method map1 (f :: <function>, l :: <list>)
  %list-map1(f, l);
end method map1;

define method map (f :: <function>, l :: <list>, #rest more-lists)
  if (empty?(more-lists))
    map1(f, l);
  else
    local method help (lists)
	    if (empty?(head(lists)))
	      #();
	    else
	      pair(apply(f, map1(head, lists)), help(map1(tail, lists)));
	    end if;
	  end help;
    help(pair(l, more-lists));
  end if;
end method help;

define method reduce (f :: <function>, i, l :: <list>)
  %list-reduce(f, i, l);
end method reduce;

define method reduce1 (f :: <function>, l :: <list>)
  %list-reduce1(f, l);
end method reduce1;

define method member? (el, l :: <list>, #key test = \==)
  %list-member?(el, l, test);
end method member?;

define method first (l :: <list>, #key default = %default-object)
  %first(l, default);
end method first;

define method second (l :: <list>, #key default = %default-object)
  %second(l, default);
end method second;

define method third (l :: <list>, #key default = %default-object)
  %third(l, default);
end method third;

define method element (l :: <list>, i :: <small-integer>,
		       #key default = %default-object)
  %list-element(l, i, default);
end method element;

define method element-setter (val, l :: <list>, i :: <small-integer>)
  %list-element-setter(l, i, val);
end method element-setter;

//
// iteration protocol
//

define method initial-state (l :: <list>)
  if (l == #())
    #f;
  else
    l;
  end if;
end method initial-state;

define method next-state (l :: <list>, s :: <list>)
  case
    empty?(s) => #f;
    empty?(tail(s)) => #f;
    otherwise => tail(s);
  end case;
end method next-state;

define method current-element (l :: <list>, s :: <list>)
  head(s);
end method current-element;

define method current-element-setter (obj, l :: <list>, s :: <pair>)
  %head-setter(s, obj);
end method current-element-setter;

define method copy-state (l :: <list>, s :: <list>)
  copy-sequence(s);
end method copy-state;


define method forward-iteration-protocol (l :: <list>)

  let initial-state = initial-state (l);
  let limit = %forward-collection-limit (l);
  let next-state = method (l :: <list>, s :: <list>)
		     case
		       empty?(s) => #f;
		       empty?(tail(s)) => #f;
		       otherwise => tail(s);
		     end case;
		   end method;
  let finished-state? = method (l :: <list>, state, limit)
			  state == limit;
			end method;
  let current-key = method (l :: <collection>, state)
		      l.size - state.size;
		    end method;
  let current-element = method (l :: <list>, s :: <list>)
			  head (s);
			end method;
  let current-element-setter = method (obj, l :: <list>, s :: <pair>)
				 %head-setter(s, obj);
			       end method;
  let copy-state = method (l :: <list>, s :: <list>)
		     copy-sequence (s)
		   end method;

  values (initial-state,
	  limit,
	  next-state,
	  finished-state?,
	  current-key,
	  current-element,
	  current-element-setter,
	  copy-state);
end method forward-iteration-protocol;

//
// comparisons
//

define method \= (p1 :: <pair>, p2 :: <pair>)
  head(p1) = head(p2) & tail(p1) = tail(p2);
end method \=;

// end list.dyl
