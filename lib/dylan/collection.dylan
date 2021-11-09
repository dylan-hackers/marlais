module: dylan

//
// collections
//

//
// collection.dyl - portable collection functions
//
// Brent Benson
//

//
// collection GF definitions
//

define generic size (collection :: <collection>)
 => integer-or-false :: <object>;
define generic empty? (collection :: <collection>) => is-empty :: <boolean>;
define generic do (procedure :: <function>,
		   collection :: <collection>,
		   #rest more-collections) => false :: <boolean>;
define generic map (procedure :: <function>,
		    collection :: <collection>,
		    #rest more-collections) => value :: <collection>;
define generic map-as (class :: <class>,
		       procedure :: <function>,
		       collection :: <collection>,
		       #rest more-collections) => value :: <collection>;
define generic map-into (mutable-collection :: <mutable-collection>,
			 procedure :: <function>,
			 collection :: <collection>,
			 #rest more-cols) => value :: <mutable-collection>;
define generic any? (procedure :: <function>,
		     collection :: <collection>,
		     #rest more-collections) => value :: <object>;
//
//
//
define generic every? (procedure :: <function>,
		       collection :: <collection>,
		       #rest more-collections) => value :: <boolean>;
define generic reduce (procedure :: <function>,
		       initial-value :: <object>,
		       collection :: <collection>) => value :: <object>;
define generic reduce1 (procedure :: <function>,
			collection :: <collection>) => value :: <object>;
define generic member? (value,
			collection :: <collection>,
			#key test) => value :: <boolean>;
define generic find-key (collection :: <collection>,
			 procedure :: <function>,
			 #key skip, failure) => key :: <object>;
define generic replace-elements! (mutable-collection :: <mutable-collection>,
				  predicate :: <function>,
				  new-value-fn :: <function>,
				  #key count) => value :: <mutable-collection>;
define generic fill! (mutable-collection :: <mutable-collection>,
		      value,
		      #key start,
		      end: finish)
 => value :: <mutable-collection>;

define generic key-test (c :: <collection>) => value :: <function>;

// iteration protocol
define generic element (c :: <collection>, key, #rest rest);

//
// most general methods for collection gfs.
//
define method size (c :: <collection>)
  let the-size = 0;
  for (value in c)
    the-size := the-size + 1;
  finally
    the-size;
  end for;
end method size;

define method shallow-copy (c :: <collection>)
  let new-collection = make (type-for-copy (c), size: c.size);
  for (obj in c, i from 0 below c.size)
    new-collection[i] := obj;
  finally
    new-collection;
  end for;
end method shallow-copy;

define method type-for-copy (c :: <mutable-collection>)
  object-class(c);
end method type-for-copy;

define method type-for-copy (p :: <byte-string>)
  <byte-string>;
end method type-for-copy;

define method empty? (c :: <collection>)
  if (initial-state(c))
    #f;
  else
    #t;
  end if;
end method empty?;

// map1 and map2 aren't part of the spec, but are included here
// for bootstrapping purposes.
//

define method map1 (f :: <function>, c :: <collection>)
  let new = make (type-for-copy (c), size: c.size);
  for (v in c, i = 0 then i + 1)
    new[i] := f (v);
  finally
    new;
  end for;
end method map1;

define method map1* (functions :: <collection>, c :: <collection>)
  let new = make (type-for-copy (c), size: c.size);
  for (v in c, f in functions, i = 0 then i + 1)
    new[i] := f (v);
  finally
    new;
  end for;
end method map1*;

define method map2 (f :: <function>, c1 :: <collection>, c2 :: <collection>)
  let new = make (type-for-copy (c1), size: c1.size);
  for (v1 in c1, v2 in c2, i = 0 then i + 1)
    new[i] := f (v1, v2);
  finally
    new;
  end for;
end method map2;

define method map2* (functions :: <collection>,
		    c1 :: <collection>,
		    c2 :: <collection>)
  let new = make (type-for-copy (c1), size: c1.size);
  for (v1 in c1, v2 in c2, f in functions, i = 0 then i + 1)
    new[i] := f (v1, v2);
  finally
    new;
  end for;
end method map2*;

define method map3* (functions :: <collection>,
		     c1 :: <collection>,
		     c2 :: <collection>,
		     c3 :: <collection>)
  let new = make (type-for-copy (c1), size: c1.size);
  for (v1 in c1, v2 in c2, v3 in c3, f in functions, i = 0 then i + 1)
    new[i] := f (v1, v2, v3);
  finally
    new;
  end for;
end method map3*;

//
// map the function forward-iteration-protocol across collections
//
define method map-forward-iteration-protocol (c :: <collection>,
					      #rest more-collections)
  let (initial-state,
       limit,
       next-state,
       finished-state?,
       current-key,
       current-element,
       current-element-setter,
       copy-state)
    = c.forward-iteration-protocol;
  let initial-states = #();
  let limits = #();
  let next-states = #();
  let finished-state?s = #();
  let current-keys = #();
  let current-elements = #();
  let current-element-setters = #();
  let copy-states = #();

  unless (empty? (more-collections))
    let (c-initial-states,
	 c-limits,
	 c-next-states,
	 c-finished-state?s,
	 c-current-keys,
	 c-current-elements,
	 c-current-element-setters,
	 c-copy-states)
      = apply (map-forward-iteration-protocol, more-collections);
    initial-states := c-initial-states;
    limits := c-limits;
    next-states := c-next-states;
    finished-state?s := c-finished-state?s;
    current-keys := c-current-keys;
    current-elements := c-current-elements;
    current-element-setters := c-current-element-setters;
    copy-states := c-copy-states;
  end unless;
  values (pair (initial-state, initial-states),
	  pair (limit, limits),
	  pair (next-state, next-states),
	  pair (finished-state?, finished-state?s),
	  pair (current-key, current-keys),
	  pair (current-element, current-elements),
	  pair (current-element-setter, current-element-setters),
	  pair (copy-state, copy-states));
end method map-forward-iteration-protocol;

define method do (f :: <function>, c :: <collection>, #rest more-collections)
  let collections = pair (c, more-collections);
  let (initial-states,
       limits,
       next-states,
       finished-state?s,
       current-keys,
       current-elements,
       current-element-setters,
       copy-states)
    = apply (map-forward-iteration-protocol, collections);
  for (states = initial-states then map2*(next-states, collections, states),
       while: ~reduce (method (x, y) x & y end,
		       #t,
		       map3*(finished-state?s, collections, states, limits)))
    apply(f, map2*(current-elements, collections, states));
  finally
    #f;
  end for;
end method do;

define method map (f :: <function>, c :: <collection>, #rest more-collections)
  let collections = pair (c, more-collections);
  let (initial-states,
       limits,
       next-states,
       finished-state?s,
       current-keys,
       current-elements,
       current-element-setters,
       copy-states)
    = apply (map-forward-iteration-protocol, collections);
  let new = make (type-for-copy (c), size: c.size);
  for (states = initial-states then map2* (next-states, collections, states),
       i = 0 then i + 1,
       while: ~reduce (method (x, y) x & y end,
		       #t,
		       map3*(finished-state?s, collections, states, limits)))
    new [i] := apply (f, map2* (current-elements, collections, states));
  finally
    new;
  end for;
end method map;

define method map-as (class :: <class>, f :: <function>, c :: <collection>,
		      #rest more-collections)
  let collections = pair (c, more-collections);
  let (initial-states,
       limits,
       next-states,
       finished-state?s,
       current-keys,
       current-elements,
       current-element-setters,
       copy-states)
    = apply (map-forward-iteration-protocol, collections);
  let new = make (class, size: c.size);
  for (states = initial-states then map2* (next-states, collections, states),
       i = 0 then i + 1,
       while: ~reduce (method (x, y) x & y end,
		       #t,
		       map3*(finished-state?s, collections, states, limits)))
    new[i] := apply (f, map2* (current-elements, collections, states));
  finally
    new;
  end for;
end method map-as;

define method map-into (mc :: <mutable-collection>,
			f :: <function>,
			collection :: <collection>,
			#rest more-collections)
  let collections = pair (collection, more-collections);
  let (initial-states,
       limits,
       next-states,
       finished-state?s,
       current-keys,
       current-elements,
       current-element-setters,
       copy-states)
    = apply (map-forward-iteration-protocol, collections);
  for (states = initial-states then map2* (next-states, collections, states),
       i = 0 then i + 1,
       while: ~reduce (method (x, y) x & y end,
		       #t,
		       map3*(finished-state?s, collections, states, limits)))
    mc[i] := apply (f, map2* (current-elements, collections, states));
  finally
    mc;
  end for;
end method map-into;

define method any? (f :: <function>, c :: <collection>,
		    #rest more-collections)
  let collections = pair (c, more-collections);
  let ret = #f;
  let (initial-states,
       limits,
       next-states,
       finished-state?s,
       current-keys,
       current-elements,
       current-element-setters,
       copy-states)
    = apply (map-forward-iteration-protocol, collections);
  for (states = initial-states then map2* (next-states, collections, states),
       i = 0 then i + 1,
       until: ret |
	 reduce (method (x, y) x & y end,
		 #t,
		 map3*(finished-state?s, collections, states, limits)))
    ret := apply (f, map2* (current-elements, collections, states));
  finally
    ret
  end for;
end method any?;

define method every? (f :: <function>, c :: <collection>,
		      #rest more-collections)
  let collections = pair (c, more-collections);
  let ret = #t;
  let (initial-states,
       limits,
       next-states,
       finished-state?s,
       current-keys,
       current-elements,
       current-element-setters,
       copy-states)
    = apply (map-forward-iteration-protocol, collections);
  for (states = initial-states then map2* (next-states, collections, states),
       i = 0 then i + 1,
       while: ret & ~reduce (method (x, y) x & y end,
			     #t,
			     map3*(finished-state?s,
				   collections,
				   states,
				   limits)))
    ret := apply (f, map2* (current-elements, collections, states));
  finally
    ret
  end for;
end method every?;

define method reduce (f :: <function>, init-value, c :: <collection>)
  let result = init-value;
  for (current-value in c)
    result := f (result, current-value);
  finally
    result;
  end for;
end method reduce;

define method reduce1 (f :: <function>, c :: <collection>)
  let (initial-state,
       limit,
       next-state,
       finished-state?,
       current-key,
       current-element,
       current-element-setter,
       copy-state)
    = forward-iteration-protocol (c);
  let value = current-element (c, initial-state);
  for (state = next-state (c, initial-state) then next-state (c, state),
       until: finished-state?(c, state, limit))
    value := f (value, current-element (c, state));
  finally
    value;
  end for;
end method reduce1;

define method member? (value, c :: <collection>, #key test = \==)
  let ret = #f;
  for (val in c,
       while: ~ret)
    ret := test (val, value);
  finally
    ret;
  end for;
end method member?;

define method find-key(c :: <collection>, f :: <function>,
                        #key skip = 0, failure)
  let (state, limit, next, end?, key, elt) =
    forward-iteration-protocol(c);
  for (i from 1 to skip,
       until: end?(c, state, limit))
    state := next(c, state);
  end for;
  let result = failure;
  let done = #f;
  for (state = state then next(c, state),
      until: done | end?(c, state, limit))
    if (f(elt(c, state)))
      result := key(c, state);
      done := #t;
    end if;
  end for;
  result
end method find-key;

define method replace-elements! (mc :: <mutable-collection>,
				 pred :: <function>,
				 new-value-fn :: <function>,
				 #key count)
  let (initial-state,
       limit,
       next-state,
       finished-state?,
       current-key,
       current-element,
       current-element-setter,
       copy-state)
    = forward-iteration-protocol (mc);
  for (state = initial-state then next-state (mc, state),
       cur-count = 0 then cur-count + 1,
       while: ~finished-state (mc, state, limit) & (cur-count <= count))
    let old-value = current-element (mc, state);
    if (pred (old-value))
      current-element (mc, state) := new-value (fn, old-value);
    end if;
  end for;
end method replace-elements!;

define method fill! (ms :: <mutable-sequence>, value, #key start = 0,
		     end: finish = ms.size - 1)
  for (i :: <small-integer> from start to finish)
    ms[i] := value;
  finally
    ms;
  end for;
end method fill!;

//
// generic functions for sequences
//

//define generic size-setter (n :: <small-integer>,
//			    s :: <stretchy-sequence>) => <integer>;
define generic add (s :: <sequence>,
		    new-element) => value :: <sequence>;
define generic add! (sequence1 :: <sequence>,
		     new-element) => sequence2 :: <sequence>;
define generic add-new (sequence :: <sequence>,
			new-element,
			#key test) => new-sequence :: <sequence>;
define generic add-new! (sequence1 :: <sequence>,
			 new-element,
			 #key test) => sequence2 :: <sequence>;
define generic remove (sequence :: <sequence>,
		       value,
		       #key test, count) => new-sequence :: <sequence>;
define generic remove! (sequence1 :: <sequence>,
			value,
			#key test, count) => sequence2 :: <sequence>;
define generic choose (predicate :: <function>,
		       sequence :: <sequence>) => new-sequence :: <sequence>;
define generic choose-by (predicate :: <function>,
			  test-sequence :: <sequence>,
			  value-sequence :: <sequence>)
 => new-sequence :: <sequence>;
define generic intersection (sequence1 :: <sequence>,
			     sequence2 :: <sequence>,
			     #key test) => new-sequence :: <sequence>;
//define generic union (sequence1 :: <sequence>,
//		      sequence2 :: <sequence>,
//		      #key test) => new-sequence :: <sequence>;
define generic remove-duplicates (sequence :: <sequence>,
				  #key test) => new-sequence :: <sequence>;
define generic remove-duplicates! (sequence1 :: <sequence>,
				   #key test) => sequence2 :: <sequence>;
define generic copy-sequence (source :: <sequence>,
			      #key start, end: finish)
 => new-sequence :: <sequence>;
define generic concatenate-as (class :: <class>,
			       sequence1 :: <sequence>,
			       #rest more-sequences)
 => new-sequence :: <sequence>;
//
//
define generic concatenate (sequence1 :: <sequence>,
			    #rest sequences) => new-sequence :: <sequence>;
define generic replace-subsequence! (mutable-sequence :: <sequence>,
				     insert-sequence :: <sequence>,
				     #key start,
				     end: limit) => sequence :: <sequence>;
define generic reverse (sequence :: <sequence>) => new-sequence :: <sequence>;
define generic reverse! (sequence1 :: <sequence>) => sequence2 :: <sequence>;
define generic sort (sequence :: <sequence>,
		     #key test, stable) => new-sequence :: <sequence>;
define generic sort! (sequence1 :: <sequence>,
		      #key test, stable) => sequence2 :: <sequence>;
define generic first (sequence :: <sequence>) => value :: <object>;
define generic second (sequence :: <sequence>) => value :: <object>;
define generic third (sequence :: <sequence>) => value :: <object>;
define generic first-setter (new-value :: <object>, sequence :: <sequence>)
			      => new-value :: <object>;
define generic second-setter (new-value :: <object>, sequence :: <sequence>)
			      => new-value :: <object>;
define generic third-setter (new-value :: <object>, sequence :: <sequence>)
			      => new-value :: <object>;
define generic last (sequence :: <sequence>) => value :: <object>;
define generic subsequence-position (big :: <sequence>,
				     pattern :: <sequence>,
				     #key test, count) => index :: <integer>;

//
// sequence GF methods
//

define method \= (s1 :: <sequence>, s2 :: <sequence>)
  if (s1.size ~= s2.size)
    #f;
  else
    let result = #t;
    for (v1 in s1,
	 v2 in s2,
	 while: result)
      result := v1 = v2;
    finally
      result;
    end for;
  end if;
end method \=;

define method key-test (sequence :: <sequence>)
  \==;
end method key-test;

//
// most general methods for sequence GFs

define method add (s :: <sequence>, new-el)
  let new = make (type-for-copy (s), size: s.size + 1);
  let (initial-state1,
       limit1,
       next-state1,
       finished-state?1,
       current-key1,
       current-element1,
       current-element-setter1,
       copy-state1)
    = forward-iteration-protocol (s);
  let (initial-state2,
       limit2,
       next-state2,
       finished-state?2,
       current-key2,
       current-element2,
       current-element-setter2,
       copy-state2)
    = forward-iteration-protocol (new);
  for (state1 = initial-state1 then next-state1 (s, state1),
       state2 = initial-state2 then next-state2 (new, state2),
       while: ~finished-state?1 (s, state1, limit1))
    current-element (new, state2) := current-element (s, state1);
  finally
    current-element (new, state2) := new-el;
    new;
  end for;
end method add;

define method add! (s :: <sequence>, new-el)
  let new = make (type-for-copy (s), size: s.size + 1);
  let (initial-state1,
       limit1,
       next-state1,
       finished-state?1,
       current-key1,
       current-element1,
       current-element-setter1,
       copy-state1)
    = forward-iteration-protocol (s);
  let (initial-state2,
       limit2,
       next-state2,
       finished-state?2,
       current-key2,
       current-element2,
       current-element-setter2,
       copy-state2)
    = forward-iteration-protocol (new);
  for (state1 = initial-state1 then next-state1 (s, state1),
       state2 = initial-state2 then next-state2 (new, state2),
       while: ~finished-state?2 (new, state2, limit2))
    current-element (new, state2) := current-element (s, state1);
  finally
    current-element (new, state2) := new-el;
    new;
  end for;
end method add;

define method add-new (s :: <sequence>, new-el, #key test = \==)
  if (member? (new-el, s, test: test))
    s;
  else
    add (s, new-el);
  end if;
end method add-new;

define method add-new! (s :: <sequence>, new-el, #key test = \==)
  if (member? (new-el, s, test: test))
    s;
  else
    add! (s, new-el);
  end if;
end method add-new!;

define method remove (s :: <sequence>, value, #key test = \==, count)
  let lst = as (<list>, s);
  as (type-for-copy (s), remove (lst, value, test: test, count: count));
end method remove;

//
// Default remove! method can't share storage!
//
define method remove! (s :: <sequence>, value, #key test = \==, count)
  let lst = as (<list>, s);
  let class = type-for-copy (s);
  as (class, remove (lst, value, test: test, count: count));
end method remove!;

define method choose (pred :: <function>, s :: <sequence>)
  let lst = as (<list>, s);
  as (type-for-copy (s), choose (pred, s));
end method choose;

define method choose-by (pred :: <function>,
			 ts :: <sequence>,
			 vs :: <sequence>)
  let tlist = as (<list>, ts);
  let vlist = as (<list>, vs);
  as (type-for-copy (vs), choose-by (pred, tlist, vlist));
end method choose-by;

define method intersection (s1 :: <sequence>, s2 :: <sequence>,
			    #key test = \==)
  let new-list = #();
  for (cur in s1)
    if (member? (cur, s2, test: test))
      new-list := pair (el, new-list);
    end if;
  finally
    as (type-for-copy (s1), new-list);
  end for;
end method intersection;

define method union (s1 :: <sequence>, s2 :: <sequence>,
		     #key test = \==)
  let new = copy-sequence (s2);
  for (cur in s1)
    new := add-new! (new, cur, test: test);
  finally
    new;
  end for;
end method union;

define method remove-duplicates (s :: <sequence>, #key test = \==)
  let new-list = #();
  for (val1 in s)
    let already-there = #f;
    for (val2 in s,
	 until: already-there)
      if (test (val1, val2))
	already-there := #t;
      end if;
    end for;
    unless (already-there)
      new-list := pair (val1, new-list);
    end unless;
  end for;
  as (s.type-for-copy, new-list);
end method remove-duplicates;

define method copy-sequence (s :: <sequence>,
				 #key start = 0, end: finish = s.size)
  if (finish > s.size)
       error ("Not enough elements in sequence to be copied,\ncopy-sequence",
	      s, "start: ", start, "end:", finish);
  end if;
  let new = make (type-for-copy (s), size: finish - start);
  let (new-initial-state,
       new-limit,
       new-next-state,
       new-finished-state?,
       new-current-key,
       new-current-element,
       new-current-element-setter,
       new-copy-state)
    = forward-iteration-protocol (new);
  let (s-initial-state,
       s-limit,
       s-next-state,
       s-finished-state?,
       s-current-key,
       s-current-element,
       s-current-element-setter,
       s-copy-state)
    = forward-iteration-protocol (s);
  for (i from 0 below start)
    s-initial-state := s-next-state (s, s-initial-state);
  end for;
  for (s-state = s-initial-state then s-next-state (s, s-state),
       new-state = new-initial-state then new-next-state (new, new-state),
       until: new-finished-state?(new, new-state, new-limit) |
       s-finished-state?(s, s-state, s-limit))
    new-current-element (new, new-state) :=
      s-current-element(s, s-state);
   finally
     new;
  end for;
end method copy-sequence;

define method concatenate-as (class :: <class>, s :: <sequence>,
			      #rest more-seqs)
  let new = apply (concatenate, s, more-seqs);
  as (class, new);
end method concatenate-as;

define method concatenate (s :: <sequence>, #rest more-seq)
  local
    method help (s, more)
      if (empty? (more))
	s
      else
	help (concatenate2 (s, more.head), more.tail)
      end if;
    end method help,
    method concatenate2 (s1 :: <sequence>, s2 :: <sequence>)
      warning("in concatenate2", s1, s2);
      let size1 = s1.size;
      let new-size = size1 + s2.size;
      let new = make (type-for-copy (s1), size: new-size);
      for (i from 0 to new-size - 1)
	if (i < size1)
	  new[i] := s1[i];
	else
	  new[i] := s2 [i - size1];
	end if;
	print (new);
      finally new;
      end for;
    end method help;

  help (s, more-seq);
end method concatenate;


define method replace-subsequence! (ms :: <mutable-sequence>,
                                    is :: <sequence>,
				    #key start = 0, end: limit = #f)
  unless (limit) limit := ms.size end;
  let os = ms;
  let delete = limit - start;
  let insert = is.size;
  let delta = insert - delete;
  unless (delta = 0)
    let copy = type-for-copy(ms);
    os := make(copy, size: ms.size + delta);
    for (i from 0 below start)
      os[i] := ms[i];
    end;
    for (i from limit below ms.size)
      os[i + delta] := ms[i];
    end;
  end;
  for (i from start, j from 0 below insert)
    os[i] := is[j];
  end for;
  os;
end method replace-subsequence!;

define method reverse (s :: <sequence>)
  let seq-size = s.size;
  let new = make (type-for-copy(s), size: seq-size);
  for (i = 0 then i + 1
	 while: i < seq-size)
    new[i] := s [seq-size - i - 1];
  finally
    new;
  end for;
end method reverse;

define method reverse! (s :: <sequence>)
  let seq-size = s.size;
  let seq-size/2 = truncate/ (seq-size, 2);
  for (i = 0 then i + 1,
       while: i < seq-size/2)
    let temp = s[i];
    let j = seq-size - i - 1;
    s[i] := s[j];
    s[j] := temp;
  finally
    s;
  end for;
end method reverse!;

define method sort (s :: <sequence>, #key test = \<, stable = #t)
  sort! (copy-sequence (s), test: test, stable: stable);
end method sort;

define method sort!(a :: <sequence>, #key test = \<, stable = #f)
  let quicksort! =
   method(a, test)
     local method sort-partition(l, r)
        let i = l; let j = r;
        let x = a[truncate/(l + r, 2)];
        until (i > j)
          while (test(a[i], x)) i := i + 1 end;
          while (test(x, a[j])) j := j - 1 end;
          if (i <= j)
            let w = a[i];
            a[i] := a[j];
            a[j] := w;
            i := i + 1;
	    j := j - 1;
          end if;
        end until;
        if (l < j) sort-partition(l,j) end;
        if (i < r) sort-partition(i,r) end;
       end method sort-partition;
       sort-partition(0, a.size - 1);
       a
    end method;

  let insertion-sort! =
    method(a, test)
      for (i from 1 below a.size)
        let x = a[i];
        let j = i - 1;
        while (j >= 0 & test(x, a[j]))
          a[j + 1] := a[j];
          j := j - 1;
        end while;
        a[j + 1] := x;
      end for;
      a
    end method;
  (if (stable) insertion-sort! else quicksort! end)(a, test)
end method sort!;

define method first (s :: <sequence>, #key default = %default-object)
  element(s, 0, default: default);
end method first;

define method second (s :: <sequence>, #key default = %default-object)
  element(s, 1, default: default);
end method second;

define method third (s :: <sequence>, #key default = %default-object)
  element(s, 2, default: default);
end method third;

define method first-setter (el, s :: <sequence>)
  s[0] := el;
end method first-setter;

define method second-setter (el, s :: <sequence>)
  s[1] := el;
end method second-setter;

define method third-setter (el, s :: <sequence>)
  s[2] := el;
end method third-setter;

define method last (s :: <sequence>, #key default = %default-object)
  let sz = size(s);
  if (sz = 0 | sz = #f)
    if (default == %default-object)
      if (sz = 0)
	error("last applied to empty sequence");
      else
	error("last applied to unbounded sequence");
      end if;
    else
      default;
    end if;
  else
    s[sz - 1];
  end if;
end method last;

define method last-setter (new-value, s :: <sequence>)
  let sz = size(s);
  if (sz = 0)
    error("last-setter applied to empty sequence");
  else
    if (~sz)
      error("last-setter applied to unbounded sequence");
    else
      s[sz - 1] := new-value;
    end if;
  end if;
end method last-setter;

define method subsequence-position(in-string :: <sequence>,
				   match :: <sequence> ,
				   #key test = \= , count = 1)
  begin
  let len1 = length(match);
  let len2 = length(in-string);
  let maxi = len2 - len1;
  let i = 0;
  let local-count = count;
  let exitval = #f;
    while((i <= maxi) & (exitval == #f) & (local-count > 0))
       for(j from 0 below len1,
           while: test(match[j] , in-string[i + j]))
       finally if (j = len1)
                 local-count := local-count - 1;
                 if (local-count = 0) exitval := i; end if;
                 end if;
       end for;
  i := i + 1;
  end while;
  exitval;
  end;
end method subsequence-position;

//
// convert from one collection type to another
//
define method as (new-class :: <class>, c :: <collection>)
  if (instance? (c, new-class))
    c;
  else
    let new = make (new-class, size: c.size);
    let (initial-state-c,
	 limit-c,
	 next-state-c,
	 finished-state?-c,
	 current-key-c,
	 current-element-c,
	 current-element-c-setter,
	 copy-state-c)
      = forward-iteration-protocol (c);
    let (initial-state-new,
	 limit-new,
	 next-state-new,
	 finished-state?-new,
	 current-key-new,
	 current-element-new,
	 current-element-new-setter,
	 copy-state-new)
      = forward-iteration-protocol (new);
    for (state-c = initial-state-c then next-state-c (c, state-c),
	 state-new = initial-state-new then next-state-new (new, state-new),
	 until: finished-state?-c (c, state-c, limit-c))
      current-element-new (new, state-new) := current-element-c (c, state-c);
    finally
      new
    end for;
  end if;
end method as;

define method key-sequence (s :: <sequence>)
  range (from: 0, size: s.size);
end method key-sequence;

// end collection.dyl
