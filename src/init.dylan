module: dylan
copyright: (c) 2001, Marlais Hackers, LGPL (see "COPYRIGHT" file)

//
// init.dylan
//
// Revision History
// 08-28-2001 dma started file
//

define method make (c :: <class>, #rest args, #key, #all-keys)
  %make(c, args);
end method make;

define constant \== = method(a, b) %identical?(a, b) end;
define constant \~ = method(a) %not(a) end;
define constant \~== = method(a, b) %not(%identical?(a, b)) end;

define constant instance? =
  method (obj, typ :: <type>)
    %instance?(obj, typ);
  end method;

define constant singleton =
  method (o :: <object>) => <singleton>;
    %singleton (o);
  end method;

define method as (c :: <class>, obj :: <object>)
  if (object-class(obj) = c)
    obj;
  else
    error("No method to coerce first object to second", obj, c);
  end if;
end method as;

define method as (sc == <string>, s :: <symbol>)
  %symbol->string(%symbol->name (s));
end method as;

/* <pcb> added so we can use the reader. */
define method as (sc == <string>, s :: <name>)
  %symbol->string(s);
end method as;

define method as (sc == <symbol>, s :: <string>)
  %name->symbol (%string->symbol (s));
end method as;

define method error (msg :: <string>, #rest args)
  %apply(%error, %pair(msg, args));
end method error;

define method warning (msg :: <string>, #rest args)
  %apply(%warning, %pair(msg, args));
end method warning;

define method cerror (#rest args)
  format(*standard-error*, "cerror: called with arguments %=", args);
end method cerror;

define method signal (#rest args)
  %signal-error-jump();
end method signal;

//
//  This just doesn't look right to me!!!!  jnw.
//

define method initialize (instance, #key, #all-keys)
end method initialize;

define method generic-function-methods (gf :: <generic-function>)
  %generic-function-methods(gf);
end method generic-function-methods;

define method add-method (gf :: <generic-function>, meth :: <method>)
  %add-method(gf, meth);
end method add-method;

define method generic-function-mandatory-keywords (gf :: <generic-function>)
  %generic-function-mandatory-keywords(gf);
end method generic-function-mandatory-keywords;

define method function-specializers (func :: <function>)
 => type-sequence :: <sequence>;
  %function-specializers(func);
end method function-specializers;

define method function-return-values (func :: <function>)
 => (type-sequence :: <sequence>, rest-value :: <object>);
  %function-values(func);
end method function-return-values;

define method function-arguments (f :: <function>)
  %function-arguments(f);
end method function-arguments;

define method applicable-method? (m :: <function>, #rest args)
  %apply(%applicable-method?, %pair(m, args));
end method applicable-method?;

define method sorted-applicable-methods (gf :: <generic-function>, #rest args)
  %apply(%sorted-applicable-methods, %pair(gf, args));
end method sorted-applicable-methods;

define method find-method (gf :: <generic-function>, #rest sample-args)
  %find-method(gf, sample-args);
end method find-method;

define method remove-method (gf :: <generic-function>, meth :: <method>)
  %remove-method(gf, meth);
end method remove-method;

define method make (gftype == <generic-function>,
		    #key required, rest, key, all-keys)
  if (instance? (required, <number>))
    required := make (<list>, size: required, fill: <object>);
  end if;
  if (instance? (required, <list>))
    %generic-function-make (list (required, rest, key, all-keys));
  else
    error ("make: bad key value", required:, required);
  end if;
end method make;


define method apply (f :: <function>, #rest args)

  // collect-args: collects the arguments into a single list.
  //  Splices in the contents of the last (collection) argument.
  local method collect-args (args)
	  case
	    empty?(args) => #();
	    empty?(tail(args)) =>
	      if (~instance?(head(args), <sequence>))
		error("apply:  last arg must be a sequence", head(args));
	      else
		as (<list>, head(args));
	      end if;
	    otherwise =>
	      // first reverse the arguments.
	      let res = reverse (args);
	      let argseq = head (res);
	      let res = tail (res);

	      // Then splice in the elements of the sequence.
	      for (val in argseq)
		res := pair (val, res);
	      end for;

	      // now fix up the arguments again
	      reverse!(res);
	  end case;
	end collect-args;

  %apply(f, collect-args(args));
end method apply;

define constant identity =
  method (x :: <object>)
    x;
  end method;

//
// comparisons.
//

// according to DIRM, = should be a generic function so it can be extended
// by user classes. most primitive version just checks if they are ==.

define method \= (o1, o2)
  o1 == o2;
end method \=;

// \~= just calls \= and complements the result.

define constant \~= =
  method (o1, o2)
    ~(o1 = o2);
  end method;

// IRM definition:  < is a generic function.

define method \< (o1, o2)
  error("\<: Can't compare these objects", o1, o2);
end method \<;

// >, <=, and >= are defined by <.

define constant \> =
  method (o1, o2)
    o2 < o1;
  end method;

define constant \<= =
  method (o1, o2)
    ~(o2 < o1);
  end method;

define constant \>= =
  method (o1, o2)
    ~(o1 < o2);
  end method;

// define method =hash (obj)
//   %=hash(obj);
// end method =hash;

//
// classes
//

define constant subtype? =
  method (t1 :: <type>, t2 :: <type>)
    %subtype?(t1, t2);
  end method;

define constant object-class =
  method (o :: <object>) => <class>;
      %object-class (o);
  end method;

define constant all-superclasses =
  method (c :: <class>)
    %all-superclasses(c);
  end method;

define constant direct-superclasses =
  method (c :: <class>)
    %direct-superclasses(c);
  end method;

define constant direct-subclasses =
  method (c :: <class>)
    %direct-subclasses(c);
  end method;

define method shallow-copy (o :: <object>) => new-object :: <object>;
  o;
end method shallow-copy;

define method type-for-copy (o :: <object>) => value :: <class>;
  o.object-class
end method type-for-copy;

//
// Seal is no longer in the DIRM, but Marlais still provides it.
//

define method seal (c :: <class>)
  %seal(c);
end method seal;

define constant slot-initialized? =
  method (obj, slot)
     slot(obj) ~== %uninitialized-slot-value;
  end method;


// limited <integer>

define method limited (int == <integer>, #rest args, #key min, max)
  %limited-integer(args);
end method limited;

define generic union (o1, o2, #rest args);

define method union (t1 :: <type>, t2 :: <type>)
  %union-type(list(t1, t2));
end method union;

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

//
// list.dyl - list operations
//
// Brent Benson
//

//
// list specific operations
//

define method pair (car, cdr)
  %pair(car, cdr);
end method pair;

define method list (#rest els)
  els
end method list;

define method type-for-copy (p :: <pair>)
  <list>;
end method type-for-copy;

define method type-for-copy (l == #())
  <list>;
end method type-for-copy;

define method head (lst :: <list>) => result :: <object>;
  %head(lst);
end method head;

define method tail (lst :: <list>) => result :: <object>;
  %tail(lst);
end method tail;

define method head-setter (obj, p :: <pair>)
  %head-setter(p, obj);
end method head-setter;

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

define method next-state (c :: <collection>, state)
  error ("Don't know how to find next state", c);
end method next-state;

define method current-element (c :: <collection>, state)
  error("Don't know how to find current element", c);
end method current-element;

define method current-key (c :: <collection>, state)
  error ("Don't know how to find current key", c);
end method current-key;

define method current-element-setter (obj,  c :: <collection>, state)
  error ("Don't know how to set element of collection", c);
end method current-element-setter;

define method copy-state (c :: <collection>, state)
  error ("Don't know how to copy state of collection", c);
end method copy-state;


//
// New method chooses the best specific method for the specified
// collection.  Assumes that any type object may be placed in the
// collection by current-element-setter.
//
define method forward-iteration-protocol (c :: <collection>)
  local method best-method (f, #rest args)
	  let (right, wrong) = apply (sorted-applicable-methods, f, args);
	  head (right);
	end method best-method;
  let initial-state = initial-state (c);
  let limit = %forward-collection-limit (c);
  let next-state = best-method (next-state, c, initial-state);
  let finished-state? = best-method (finished-state?, c, initial-state, limit);
  let current-key = best-method (current-key, c, initial-state);
  let current-element = best-method (current-element, c, initial-state);
  let current-element-setter = best-method (current-element-setter,
					    0,
					    c,
					    initial-state);
  let copy-state = best-method (copy-state, c, initial-state);
  values (initial-state,
	  limit,
	  next-state,
	  finished-state?,
	  current-key,
	  current-element,
	  current-element-setter,
	  copy-state);
end method forward-iteration-protocol;


define method backward-iteration-protocol (c :: <collection>)
  local method best-method (f, #rest args)
	  let (right, wrong) = apply (sorted-applicable-methods, f, args);
	  head (right);
	end method best-method;
  let final-state = final-state (c);
  let limit = %backward-collection-limit (c);
  let previous-state = best-method (previous-state, c, final-state);
  let finished-state? = best-method (finished-state?, c, final-state, limit);
  let current-key = best-method (current-key, c, final-state);
  let current-element = best-method (current-element, c, final-state);
  let current-element-setter = best-method (current-element-setter,
					    0,
					    c,
					    final-state);
  let copy-state = best-method (copy-state, c, final-state);
  values (final-state,
	  limit,
	  previous-state,
	  finished-state?,
	  current-key,
	  current-element,
	  current-element-setter,
	  copy-state);
end method backward-iteration-protocol;

define method %forward-collection-limit (c :: <collection>)
 #f
end method %forward-collection-limit;

define method %backward-collection-limit (c :: <collection>)
 #f
end method %backward-collection-limit;

define method finished-state? (c :: <collection>, state, limit)
  state == limit;
end method finished-state?;


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

//
// range.dyl
//
// range operations
//

define class <range> (<sequence>)
  slot from,    init-value:  0, init-keyword: from:;
  slot to,      init-value: #f, init-keyword: to:;
  slot above,   init-value: #f, init-keyword: above:;
  slot below,   init-value: #f, init-keyword: below:;
  slot by,      init-value:  1, init-keyword: by:;
  slot size,    init-value: #f, init-keyword: size:;
end class <range>;

define method initialize (range :: <range>, #rest args)
  let from = range.from;
  let to = range.to;
  let above = range.above;
  let below = range.below;
  let by = range.by;
  let bmax = method (x, y) if (x) max (x, y) else y end if; end method;

  if (by == 0)
    range.size := #f;
  else
    let new-size = range.size;
    if (to)
      let span = truncate/ (to - from, by);
      new-size := as (<small-integer>, span + 1);
    end if;
    if (above)
      if (by < 0)
	let span = truncate/ (above - from, by);
	new-size := as (<small-integer>, bmax (new-size, span));
      else
	new-size := 0;
      end if;
    end if;
    if (below)
      if (by > 0)
	let span = truncate/ (below - from, by);
	new-size := as (<small-integer>, bmax (new-size, span));
      else
	new-size := 0;
      end if;
    end if;
    if (new-size)
      new-size := max (new-size, 0);
    end if;
    range.size := new-size;
  end if;
end method initialize;

define method range (#rest args, #key, #all-keys)
  %apply (make, pair (<range>, args));
end method range;

define method element (range :: <range>, index :: <small-integer>,
		       #key default = %default-object)
  select (range.size)
    0  => if (default == %default-object)
	    error ("element: no elements in range");
	  else
	    default;
	  end if;
    #f => if (index >= 0)
	    range.from + range.by * index;
	  elseif (default == %default-object)
	    error ("element: index out of range", index);
	  else
	    default;
	  end if;
    otherwise =>
      if ((index  >= range.size) | (index < 0))
	  if (default == %default-object)
	    error ("element: index out of range", index);
	  else
	    default;
	  end if;
      else
	range.from + range.by * index;
      end if;
  end select;
end method element;

define method member? (value, range :: <range>, #key test = \==)
  if (test == \==)
    let span = truncate/ (value - range.from, range.by);
    element (range,
	     as (<small-integer>, truncate/ (value - range.from, range.by)),
	     default: #f) == value;
  else
    let result = #f;
    for (x in range,
	 until: result := test (value, x))
    finally
      result;
    end for;
  end if;
end method member?;

define method shallow-copy (r :: <range>)
  range(from: r.from, by: r.by,
    to: r.to, above: r.above, below: r.below);
end method shallow-copy;

define method copy-sequence (r :: <range>, #key start = 0, end: finish = #f)
  if (finish)
    if (negative?(r.by))
      range(from: r[start], by: r.by, above: r[finish])
    else
      range(from: r[start], by: r.by, below: r[finish])
    end;
  else
    range(from: r[start], by: r.by,
      to: r.to, above: r.above, below: r.below);
  end
end method copy-sequence;

define method sort(a :: <range>, #key test = \<, stable = #f)
  as(<list>, sort!(as(<vector>, a), test: test, stable: stable))
end method sort; // <list>

define method sort!(a :: <range>, #key test = \<, stable = #f)
  sort(a, test: test, stable: stable)
end method sort!; // <list>

define method \= (r1 :: <range>, r2 :: <range>)
  r1.from = r2.from & r1.by = r2.by & r1.size = r2.size;
end method \=;

// define method =hash (r :: <range>)
//   =hash (r.from) + =hash (r.by) + =hash (r.size);
// end method =hash;

define method reverse! (f :: <range>)
  if (r.size)
    r.from := r.last;
    r.by := -r.by;
    r.above := #f;
    r.to := #f;
    r.below := #f;
    r;
  else
    error("reverse!: unable to operate on unbounded range");
  end if;
end method reverse!;

define method reverse (r :: <range>)
  if (r.size)
    range (from: r.last, size: r.size, by: -r.by);
  else
    error ("reverse: unable to operate on unbounded range");
  end if;
end method reverse;

define method type-for-copy (r :: <range>)
  <list>;
end method type-for-copy;

//
// range iteration protocol
//

define method initial-state (r :: <range>)
  let x = pair (#f, #f);
  let result = element (r, 0, default: x);
  if (x == result)
    #f;
  else
    0;
  end if;
end method initial-state;

define method next-state (r :: <range>, state)
  let x = pair (#f, #f);
  let result = element (r, state + 1, default: x);
  if (x == result)
    #f;
  else
    state + 1;
  end if;
end method next-state;

define method current-element (r :: <range>, state)
  r[state];
end method current-element;

// end range

//
// string.dyl
//
// string operations
//

define method element (s :: <string>, i :: <small-integer>,
		       #key default = %default-object)
  %string-element(s, i, default);
end method element;

define method element-setter (c :: <character>,
			      s :: <string>,
			      i :: <small-integer>)
  %string-element-setter (s, i, c);
end method element-setter;

define method size (s :: <string>)
  %string-size(s);
end method size;

define method concatenate (s :: <string>, #rest more-strings)
  if (empty? (more-strings))
    s
  else
    apply (concatenate, pair (%string-append2(s, more-strings.head),
			      more-strings.tail));
  end if;
end method concatenate;

define method as (ic == <small-integer>, s :: <string>)
  let zero = as(<small-integer>, '0');
  let total = 0;
  local method accumulate (ch :: <character>)
    total := 10 * total + (as(<small-integer>, ch) - zero);
  end method;
  do(accumulate, s);
  total
end method as;

//
// iteration protocol
//

define constant <integer-state> = union (<small-integer>, singleton(#f));

define method current-key (s :: <collection>, state :: <integer-state>)
  state
end method current-key;

define method initial-state (s :: <string>)
  if (size(s) = 0) #f; else 0; end if;
end method initial-state;

define method next-state (s :: <string>, state :: <integer-state>)
  if (state < s.size - 1)
    state + 1;
  else
    #f;
  end if;
end method next-state;

define method current-element (s :: <string>, state :: <integer-state>)
  %string-element(s, state, %default-object);
end method current-element;

define method current-element-setter (obj,
				      s :: <string>,
				      state :: <integer-state>)
  %string-element-setter(s, state, obj);
end method current-element-setter;

define method copy-state (s :: <string>, state :: <integer-state>)
  state;
end method copy-state;

define method as-uppercase!(s :: <string>)
  map-into(s,as-uppercase,s);
end method;

define method as-lowercase!(s :: <string>)
  map-into(s,as-lowercase,s);
end method;

define method as-uppercase(s :: <string>)
  map(as-uppercase,s);
end method;

define method as-lowercase(s :: <string>)
  map(as-lowercase,s);
end method;

// comparisons

define method \< (s1 :: <string>, s2 :: <string>)
  %string<(s1, s2);
end method \<;

define method \= (s1 :: <string>, s2 :: <string>)
  %string=(s1, s2);
end method \=;

// end string.dyl

//
// vector.dyl
//
// Brent Benson
//

define method vector (#rest els)
  %vector (els);
end method vector;

define method element (v :: <vector>, i :: <small-integer>,
		       #key default = %default-object)
  %vector-element (v, i, default);
end method element;

define method element-setter (obj, v :: <vector>, i :: <small-integer>)
  %vector-element-setter (v, i, obj);
end method element-setter;

define method size (v :: <vector>)
  %vector-size (v);
end method size;

define method dimensions (v :: <vector>)
  list (%vector-size (v));
end method dimensions;

//
// iteration protocol
//

define method initial-state (v :: <vector>)
  if (v.size == 0)
    #f;
  else
    0;
  end if;
end method initial-state;

define method next-state (v :: <vector>, state :: <integer-state>)
  if (state < v.size - 1)
    state + 1;
  else
    #f;
  end if;
end method next-state;

define method current-element (v :: <vector>, state :: <integer-state>)
  %vector-element (v, state, %default-object);
end method current-element;

define method current-element-setter (obj,
				      v :: <vector>,
				      state :: <integer-state>)
  %vector-element-setter (v, state, obj);
end method current-element-setter;

define method copy-state (v :: <vector>, state :: <integer-state>)
  state;
end method copy-state;

define method previous-state (v :: <vector>, state :: <integer-state>)
  if (state <= 0)
    #f;
  else
    state - 1;
  end if;
end method previous-state;

define method final-state (v :: <vector>)
  v.size - 1;
end method final-state;

// end vector.dyl

//
// table.dyl
//
// Brent Benson
//

define method element (t :: <object-table>,
		       key,
		       #key default = %default-object)
  %table-element (t, key, default);
end method element;

define method element-setter (value, t ::<object-table>, key)
  %table-element-setter (t, key, value);
end method element-setter;

define method initial-state (t :: <object-table>)
  %table-initial-state (t);
end method initial-state;

define method next-state (t :: <object-table>, te :: <table-entry>)
  %table-next-state (t, te);
end method next-state;

define method current-element (t :: <object-table>, te :: <table-entry>)
   %table-current-element (t, te);
end method current-element;

define method current-key (t :: <object-table>, te :: <table-entry>)
  %table-current-key (t, te);
end method current-key;

define method current-element-setter (value,
				      t :: <object-table>,
				      te :: <table-entry>)
  %table-current-element-setter (t, te,  value);
end method current-element-setter;

//
// check key-sequence GF method below
//

define method key-sequence (t :: <object-table>)
  let keys = #();
  let (init, limit, next, end?, key, elt) =
    forward-iteration-protocol (t);
  for (state = init then next (t, state),
      until: end? (t, state, limit))
    keys := pair (key (t, state), keys);
  finally keys;
  end for;
end method key-sequence;

// end table.dyl

//
// deque.dyl
//
// Brent Benson
//

define method push (d :: <deque>, new)
  %push (d, new);
end method push;

define method pop (d :: <deque>)
  %pop (d);
end method pop;


define method push-last (d :: <deque>, new)
  %push-last (d, new);
end method push-last;

define method pop-last (d :: <deque>)
  %pop-last (d);
end method pop-last;

define method first (d ::<deque>, #key default = %default-object)
  %deque-first (d, default);
end method first;

define method last (d :: <deque>, #key default = %default-object)
  %deque-last (d, default)
end method last;

// should add specific (define-method last-setter ((d <deque>) new-value) ...)

define method element (d ::<deque>, i :: <small-integer>,
		       #key default = %default-object)
  %deque-element (d, i, default)
end method element;

define method element-setter (new, d :: <deque>, i :: <small-integer>)
  %deque-element-setter (d, i, new)
end method element-setter;

define method add! (d :: <deque>, new)
  %push (d, new);
end method add;

//
// add remove!
//

//
// iteration protocol
//

define method initial-state (d :: <deque>)
  pair (%deque-initial-state (d), 0);
end method initial-state;

define method next-state (d :: <deque>, state :: <pair>)
  state.head := %deque-next-state (d, state.head);
  state.tail := state.tail + 1;
  state
end method next-state;

define method final-state (d :: <deque>)
  pair (%deque-final-state (d), d.size - 1);
end method final-state;

define method previous-state (d :: <deque>, state :: <pair>)
  state.head := %deque-previous-state (d, state.head);
  state.tail := state.tail - 1;
  state
end method previous-state;

define method current-key (d :: <deque>, state :: <pair>)
  state.tail;
end method current-key;

define method current-element (d :: <deque>, state :: <pair>)
  %deque-current-element (d, state.head);
end method current-element;

define method finished-state? (d :: <deque>, state :: <pair>, limit)
  state.head == limit
end method finished-state?;

define method current-element-setter (new-value,
				      d :: <deque>,
				      state ::<pair>)
  %deque-current-element-setter (d, state.head, new-value);
end method current-element-setter;

define method copy-state (d :: <deque>, state :: <pair>)
  pair(state.head, state.tail);
end method copy-state;

define method forward-iteration-protocol (d :: <deque>)

  let initial-state = initial-state (d);
  let limit = %forward-collection-limit (d);
  let next-state = method (d :: <deque>, state :: <pair>)
		     state.head := %deque-next-state (d, state.head);
		     state.tail := state.tail + 1;
		     state
		   end method;
  let finished-state? =  method (d :: <deque>, state :: <pair>, limit)
			   state.head == limit
			 end method;
  let current-key = method (d :: <deque>, state :: <pair>)
		      state.tail;
		    end method;
  let current-element =  method (d :: <deque>, state :: <pair>)
			   %deque-current-element (d, state.head);
			 end method;
  let current-element-setter = method (new-value,
				       d :: <deque>,
				       state ::<pair>)
				 %deque-current-element-setter (d,
								state.head,
								new-value);
			       end method;

  let copy-state = method (d :: <deque>, s :: <pair>)
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

// This method required because the general empty? method assumes that
// the final state is #f.
define method empty?(d :: <deque>)
  if (%deque-initial-state (d))
    #f
  else
    #t
  end
end method empty?;

// end deque.dyl

//
// array.dyl
//
// Brent Benson
//

define method element (a :: <array>,
		       index :: <integer>,
		       #key default = %default-object)
  %array-element (a, index, default);
end method element;

define method element (a :: <array>,
		       indices :: <list>,
		       #key default = %default-object)
  %array-ref (a, indices, default);
end method element;

define method element-setter (new-value, a :: <array>, index :: <integer>)
  %array-element-setter (a, index, new-value);
end method element-setter;

define method element-setter (new-value, a :: <array>, indices :: <list>)
  %array-ref-setter (a, indices, new-value);
end method element-setter;

define method dimensions (a :: <array>)
  %array-dimensions (a);
end method dimensions;

define method size (a :: <array>)
  %array-size (a);
end method size;

define generic rank (a :: <array>) => rank :: <integer>;

define method rank (a :: <array>)
  %array-dimensions (a).size;
end method rank;

define generic row-major-index (a :: <array>, #rest subscripts)
 => index :: <integer>;

define method row-major-index (a :: <array>, #rest subscripts)
  %array-row-major-index (a, subscripts);
end method row-major-index;

define generic aref (a :: <array>, #rest indices);

define method aref (a :: <array>, #rest indices)
  %array-ref (a, indices, %default-object);
end method aref;

define generic aref-setter (new-val, a :: <array>, #rest indices);

define method aref-setter (new-val, a :: <array>, #rest indices)
  %array-ref-setter (a, indices, new-val);
end method aref-setter;

define method aref (v :: <vector>, #rest indices)
  %vector-element (v, indices.first, %default-object);
end method aref;

define method aref-setter (new-val, v :: <vector>, #rest indices)
  %vector-element-setter (v, indices.first, new-val);
end method aref-setter;

define generic dimension (a :: <array>, axis :: <integer>)
 => dimension :: <integer>;

define method dimension (array :: <array>, axis :: <small-integer>)
  element (dimensions (array), axis);
end method dimension;

define method shallow-copy (a :: <array>)
  let new-array = make (<array>, dimensions: dimensions (a));
  for (i from 0 below a.size)
    element-setter (element(a, i), new-array, i);
  end for;
  new-array;
end method shallow-copy;

//
// iteration protocol
//

define method initial-state (a :: <array>) => value :: <integer-state>;
  %array-initial-state (a);
end method initial-state;

define method next-state (a :: <array>, s :: <integer-state>)
  %array-next-state (a, s);
end method next-state;

define method current-element (a :: <array>, s :: <integer-state>)
  %array-current-element (a, s);
end method current-element;

// end array.dyl


//
// numbers
//

//
// number.dyl - generic functions on numbers
//
// Brent Benson
//

//
// misc
//
define method odd? (i :: <small-integer>) => value :: <boolean>;
  %odd?(i);
end method odd?;

define method odd? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-odd?(bi);
end method odd?;

define method even? (i :: <small-integer>) => value :: <boolean>;
  %even?(i);
end method even?;

define method even? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-even?(bi);
end method even?;

define generic zero? (n :: <number>) => value :: <boolean>;

define method zero? (i :: <small-integer>) => value :: <boolean>;
  %int-zero?(i);
end method zero?;

define method zero? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-zero?(bi);
end method zero?;

define method zero? (d :: <double-float>) => value :: <boolean>;
  %double-zero?(d);
end method zero?;

define generic positive? (r :: <real>) => value :: <boolean>;

define method positive? (i :: <small-integer>) => value :: <boolean>;
  %int-positive?(i);
end method positive?;

define method positive? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-positive?(bi);
end method positive?;

define method positive? (d :: <double-float>) => value :: <boolean>;
  %double-positive?(d);
end method positive?;

define generic negative? (r :: <real>) => value :: <boolean>;

define method negative? (i :: <small-integer>) => value :: <boolean>;
  %int-negative?(i);
end method negavite?;

define method negative? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-negative?(bi);
end method negavite?;

define method negative? (d :: <double-float>) => value :: <boolean>;
  %double-negative?(d);
end method negative?;

define method integral? (n :: <number>)
  #f;
end method integral?;

define method integral? (i :: <integer>)
  #t;
end method integral?;

// define method quotient (i1 :: <integer>, i2 :: <integer>)
//   %quotient?(i1, i2);
// end method quotient;

//
// coercions
//

define method as (df-class == <double-float>, i :: <small-integer>)
  %int-to-double(i);
end method as;

define method as (i-class == <small-integer>, df :: <double-float>)
  %double-to-int(df);
end method as;

//
// <big-integer> coercions
//

// <small-integer> <-> <big-integer>

define method as (i-class == <small-integer>, bi :: <big-integer>)
  %bigint->int(bi);
end method as;

define method as (bi-class == <big-integer>, i :: <small-integer>)
  %int->bigint(i);
end method as;

// <double-float> <-> & <big-integer>

define method as (bi-class == <big-integer>, df :: <double-float>)
  %double->bigint(df);
end method as;

define method as (df-class == <double-float>, bi :: <big-integer>)
  %bigint->double(bi);
end method as;

// <character> <-> & <big-integer>

define method as (ch-class == <character>, bi :: <big-integer>)
  %integer->character(%bigint->int(bi));
end method as;

define method as (bi-class == <big-integer>, ch :: <character>)
  %int->bigint(%character->integer(ch));
end method as;

//
// Generic functions for arithmetic operations
//
define generic \+ (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \* (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \- (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \/ (n1 :: <number>, n2 :: <number>) => value :: <number>;

define generic negative (n :: <number>) => value :: <number>;

define generic floor (r :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
define generic ceiling (r :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
define generic round (r :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
define generic truncate (r :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);

define generic floor/ (r1 :: <real>, r2 :: <real>)
 => (quotient :: <integer>, remainder :: <real>);
define generic ceiling/ (r1 :: <real>, r2 :: <real>)
 => (quotient :: <integer>, remainder :: <real>);
define generic round/ (r1 :: <real>, r2 :: <real>)
 => (quotient :: <integer>, remainder :: <real>);
define generic truncate/ (r1 :: <real>, r2 :: <real>)
 => (quotient :: <integer>, remainder :: <real>);

define generic modulo (r1 :: <real>, r2 :: <real>) => modulus :: <real>;
define generic remainder (r1 :: <real>, r2 :: <real>) => remains :: <real>;

define generic \^ (n :: <number>, i :: <integer>) => value :: <number>;

//
// (op <small-integer> <small-integer>)
//
define method \+ (i1 :: <small-integer>, i2 :: <small-integer>)
  %binary-int+ (i1, i2);
end method \+;

define method \- (i1 :: <small-integer>, i2 :: <small-integer>)
  %binary-int- (i1, i2);
end method \-;

define method \* (i1 ::<small-integer>, i2 :: <small-integer>)
  %binary-int* (i1, i2);
end method \*;

define method negative (i :: <small-integer>) => value :: <small-integer>;
  %int-negative (i);
end method negative;

//
// (op <double-float> <double-float>)
//
define method \+ (d1 :: <double-float>, d2 :: <double-float>)
  %binary-double+ (d1, d2);
end method \+;

define method \- (d1 :: <double-float>, d2 :: <double-float>)
  %binary-double- (d1, d2);
end method \-;

define method \* (d1 :: <double-float>, d2 :: <double-float>)
  %binary-double* (d1, d2);
end method \*;

define method \/ (d1 :: <double-float>, d2 :: <double-float>)
  %binary-double/ (d1, d2);
end method \/;

define method negative (d :: <double-float>) => value :: <double-float>;
    %double-negative (d);
end method negative;

//
// (op <integer> <double-float>)
//
define method \+ (i1 :: <integer>, d2 :: <double-float>)
  %binary-double+ (as (<double-float>, i1), d2);
end method \+;

define method \- (i1 :: <integer>, d2 :: <double-float>)
  %binary-double- (as (<double-float>, i1), d2);
end method \-;

define method \* (i1 :: <integer>, d2 :: <double-float>)
  %binary-double* (as (<double-float>, i1), d2);
end method \*;

define method \/ (i1 :: <integer>, d2 :: <double-float>)
  %binary-double/ (as (<double-float>, i1), d2);
end method \/;

//
// (op <double-float> <integer>)
//
define method \+ (d1 :: <double-float>, i2 :: <integer>)
  %binary-double+ (d1, as (<double-float>, i2));
end method \+;

define method \- (d1 :: <double-float>, i2 :: <integer>)
  %binary-double- (d1, as (<double-float>, i2));
end method \-;

define method \* (d1 :: <double-float>, i2 :: <integer>)
  %binary-double* (d1, as (<double-float>, i2));
end method \*;

define method \/ (d1 :: <double-float>, i2 :: <integer>)
  %binary-double/ (d1, as (<double-float>, i2));
end method \/;

//
// (op <big-integer> <big-integer>) & friends.
//

define method \+ (b1 :: <big-integer>, b2 :: <big-integer>)
  %binary-bigint+ (b1, b2);
end method \+;

define method \+ (b :: <big-integer>, i :: <small-integer>)
  %binary-bigint+ (b, as(<big-integer>, i));
end method \+;

define method \+ (i :: <small-integer>, b :: <big-integer>)
  %binary-bigint+ (as(<big-integer>, i), b);
end method \+;

define method \- (b1 :: <big-integer>, b2 :: <big-integer>)
  %binary-bigint- (b1, b2);
end method \-;

define method \- (b :: <big-integer>, i :: <small-integer>)
  %binary-bigint- (b, as(<big-integer>, i));
end method \-;

define method \- (i :: <small-integer>, b :: <big-integer>)
  %binary-bigint- (as(<big-integer>, i), b);
end method \-;

define method \* (b1 ::<big-integer>, b2 :: <big-integer>)
  %binary-bigint* (b1, b2);
end method \*;

define method \* (b :: <big-integer>, i :: <small-integer>)
  %binary-bigint* (b, as(<big-integer>, i));
end method \*;

define method \* (i :: <small-integer>, b :: <big-integer>)
  %binary-bigint* (as(<big-integer>, i), b);
end method \*;

define method \/ (b :: <big-integer>, i :: <small-integer>)
   %binary-bigint/ (b, as(<big-integer>, i));
end method \/;

define method negative (b :: <big-integer>) => <big-integer>;
  %bigint-negative (b);
end method negative;

define method \^ (b :: <big-integer>, e :: <big-integer>) => <big-integer>;
      %bigint-pow(b, e);
end method \^;

define method \^ (b :: <big-integer>, e :: <small-integer>) => <big-integer>;
      %bigint-pow(b, as(<big-integer>, e));
end method \^;

define method \^ (b :: <small-integer>, e :: <big-integer>) => <big-integer>;
      %bigint-pow(as(<big-integer>, b), e);
end method \^;

/* Leave this out if <big-integer> is not implemented

define method \^ (b :: <small-integer>, e :: <small-integer>) => <big-integer>;
      %bigint-pow(as(<big-integer>, b), as(<big-integer>, e));
end method \^;

*/

//
// comparisons
//

define method \= (i :: <integer>, d :: <double-float>)
  as(<double-float>, i) == d;
end method \=;

define method \= (d :: <double-float>, i :: <integer>)
  d == as(<double-float>, i);
end method \=;

define method \< (n1 :: <number>, n2 :: <number>)
  %binary-less-than(n1, n2);
end method \<;

define method max (n1 :: <real>, #rest more-reals)
  reduce (method (x, y) if (x < y) y else x end if; end,
	  n1,
	  more-reals);
end method max;

define method min (n1 :: <real>, #rest more-reals)
  reduce (method (x, y) if (x < y) x else y end if; end,
	  n1,
	  more-reals);
end method min;

// between <big-integer>'s and <small-integer>'s.

define method \= (b1 :: <big-integer>, b2 :: <big-integer>)
  %bigint=(b1, b2);
end method \=;

define method \= (b :: <big-integer>, i :: <small-integer>)
  b = as(<big-integer>, i);
end method \=;

define method \= (i :: <small-integer>, b :: <big-integer>)
  b = as(<big-integer>, i);
end method \=;

define method \< (b1 :: <big-integer>, b2 :: <big-integer>)
  %bigint<(b1, b2);
end method \<;

define method \< (b :: <big-integer>, i :: <small-integer>)
  %bigint<(b, as(<big-integer>, i));
end method \<;

define method \< (i :: <small-integer>, b :: <big-integer>)
  %bigint<(as(<big-integer>, i), b);
end method \<;

//
// other functions
//

define generic abs (n :: <number>) => value :: <number>;

define method abs (i :: <small-integer>)
  %int-abs(i);
end method abs;

define method abs (bi :: <big-integer>)
  %bigint-abs(bi);
end method abs;

define method abs (d :: <double-float>)
  %double-abs(d);
end method abs;

define method ash (i :: <small-integer>, count :: <small-integer>)
  %ash(i, count);
end method ash;

define method logior (#rest integers)
  reduce1 (%binary-logior, integers);
end method logior;

define method logand (#rest integers)
  reduce1 (%binary-logand, integers);
end method logand;

// lcm -- 06/20/95 Marty Hall

define method lcm(int1 :: <integer>, int2 :: <integer>)
 => multiple :: <integer>;
  let result = floor/ (abs(int1 * int2), gcd(int1, int2));
  result;
 end method lcm;


// GCD via Euclid's Algorithm, yielding O(log N) performance, where N is the
// smaller of the two numbers. See _Structure and Interpretation
// of Computer Programs_ pg 44. Note that having an extra case where
// int1 == int2 is not worth it, since then you have an additional test
// in the 95% of cases when int1 and int2 are distinct. This way, you have
// an extra function call in the 5% of cases when int1 is == to int2.
// 6/95 Marty Hall

define method gcd(int1 :: <integer>, int2 :: <integer>)
 => divisor :: <integer>;
  local method gcd-internal(larger :: <integer>, smaller :: <integer>)
	 => divisor :: <integer>;
	  if (smaller == 0)
	    larger;
	  else
	    gcd-internal(smaller, remainder(larger, smaller));
	  end if;
	end method gcd-internal;
  let int1 = abs(int1);
  let int2 = abs(int2);
  if (int1 > int2)
    gcd-internal(int1, int2);
  else
    gcd-internal(int2, int1);
  end if;
end method gcd;

define method floor (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %floor (d);
end method floor;

define method ceiling (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %ceiling (d);
end method ceiling;

define method round (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %round (d);
end method round;

define method truncate (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %truncate (d);
end method truncate;

define method modulo (n1 :: <real>, n2 :: <real>)
 => modulus :: <real>;
  %modulo-double ( as (<double-float>, n1), as (<double-float>, n2));
end method modulo;

define method modulo (i1 :: <small-integer>, i2 :: <small-integer>)
 => modulus :: <small-integer>;
  %modulo (i1, i2);
end method modulo;

define method remainder (n1 :: <real>, n2 :: <real>)
 => remainder :: <real>;
  %remainder-double (as (<double-float>, n1), as (<double-float>, n2));
end method remainder;

define method remainder (i1 :: <integer>, i2 :: <integer>)
 => remainder :: <integer>;
  %remainder-int (i1, i2);
end method remainder;

// Thanks for Marty Hall for making the following get into this file
//
// Exponentiation has the following caveats:
//
// If there's no big integer support on your platform, this will
// give wrong results without warning for larger numbers.
//
// Raising floats to large exponents gives results that are
// limited by the precision of the float implementation.

define method \^ (base :: <number>, exponent :: <integer>)
 => base-to-exponent :: <number>;
  local method power(b :: <number>,
		     e :: <integer>,
		     residual :: <number>) => b-to-e :: <number>;
	  if (e == 0)
	    residual;
	  elseif (e.even?)
	    power (b * b, floor/(e, 2), residual);
	  else
	    power (b, e - 1, residual * b);
	  end if;
	end method power;

  if (exponent.negative?)
    1.0 / power (base, - exponent, 1);
  else
    power (base, exponent, 1);
  end if;
end method \^;

// <pcb> add <big-integer> cases again.
define method modulo (b1 :: <big-integer>, b2 :: <big-integer>)
  %bigint-mod (b1, b2);
end method modulo;

define method modulo (b :: <big-integer>, i :: <small-integer>)
      %bigint-mod (b, as (<big-integer>, i));
end method modulo;

define method modulo (i :: <small-integer>, g :: <big-integer>)
      %bigint-mod (as (<big-integer>, i), b);
end method modulo;

define method floor/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %floor/ (as (<double-float>, n1), as (<double-float>, n2));
end method floor/;

define method ceiling/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %ceiling/ (as (<double-float>, n1), as (<double-float>, n2));
end method ceiling/;

define method round/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %round/ (as (<double-float>, n1), as (<double-float>, n2));
end method round/;

define method truncate/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %truncate/ (as (<double-float>, n1), as (<double-float>, n2));
end method truncate/;

define method truncate/ (i1 :: <small-integer>, i2 :: <small-integer>)
 => (quotient :: <small-integer>, remainder :: <small-integer>);
  %int-truncate/ (i1, i2);
end method truncate/;

//
// Non-DIRM arithmetic operations
//

define method sqrt (i :: <small-integer>)
  %int-sqrt(i);
end method sqrt;

define method sqrt (bi :: <big-integer>)
  %bigint-sqrt(bi);
end method sqrt;

define method sqrt (d :: <double-float>)
  %double-sqrt(d);
end method sqrt;

define method sin (n :: <number>) => value :: <number>;
  %sin (as (<double-float>, n));
end method sin;

define method cos (n :: <number>) => value :: <number>;
  %cos (as (<double-float>, n));
end method cos;

define method atan2 (d1 :: <number>, d2 :: <number>)
  %atan2 (as (<double-float>, d1), as (<double-float>, d2));
end method atan2;

define method exp (n :: <number>)
  %exp(as(<double-float>, n));
end method exp;

define method ln (n :: <number>)
  %ln(as(<double-float>, n));
end method ln;



// end number.dyl

//
// characters
//

//
// character.dyl
//
// Brent Benson
//

define method as (ic == <small-integer>, ch :: <character>)
  %character->integer(ch);
end method as;

define method as (cc == <character>, i :: <small-integer>)
  %integer->character(i);
end method as;

begin
  let uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  let lowercase = "abcdefghijklmnopqrstuvwxyz";

  define method as-lowercase(c :: <character>)
    let i = find-key(uppercase,curry(\=,c));
    if (i)
      lowercase[i]
    else
      c
    end
  end method as-lowercase;

  define method as-uppercase(c :: <character>)
    let i = find-key(lowercase,curry(\=,c));
    if (i)
      uppercase[i]
    else
      c
    end
  end method as-uppercase;
end;

// comparisons

define method \< (c1 :: <character>, c2 :: <character>)
  as(<integer>, c1) < as(<integer>, c2);
end method \<;

// functionals

define method compose(function :: <function>, #rest more-functions)
  if (empty?(more-functions))
    function;
  else
    method (#rest args)
      function(apply(apply(compose, head(more-functions),
			   tail(more-functions)), args));
    end method;
  end if;
end method compose;

define method complement (func :: <function>)
  method (#rest args)
    ~(apply(func, args));
  end method;
end method complement;

define method disjoin ( predicate :: <function>,
		       #rest more-predicates)
  method (#rest args)
    if (empty? (predicates))
      apply (predicate (args))
    else
      apply (predicate, args)
	| apply (apply (disjoin, predicates), args);
    end if;
  end method;
end method disjoin;

define method conjoin (predicate :: <function>,
		       #rest more-predicates)
  method (#rest args)
    if (empty? (more-predicates))
      apply (predciate, args)
    else
      apply (predicate, args)
	& apply (apply (conjoin, more-predicates), args);
    end if;
  end method;
end method conjoin;

define method curry (f :: <function>,
		     #rest curried-args)
  method (#rest regular-args)
    apply (f, concatenate (curried-args, regular-args));
  end method;
end method curry;

define method rcurry (f :: <function>,
		      #rest curried-args)
  method (#rest regular-args)
    apply (f, concatenate (regular-args, curried-args));
  end method;
end  method rcurry;

define method always (obj :: <object>)
  method (#rest args)
    obj
  end method;
end method always;

define method eval(obj)
  %eval (obj);
end method eval;

define method type-union(#rest types)
  apply(%union-type, types);
end method type-union;
