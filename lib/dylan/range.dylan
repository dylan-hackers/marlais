module: dylan

//
// range.dylan
//
// Authors:
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
