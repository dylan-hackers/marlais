module: dylan

//
// vector.dylan
//
// Authors:
//   Brent Benson
//

//
// Generic methods on <vector>
//

define method dimensions (v :: <vector>)
  list (size (v));
end method dimensions;

//
// Generic iteration on <vector>
//

define method initial-state (v :: <vector>)
  if (v.size == 0)
    #f;
  else
    0;
  end if;
end method initial-state;

define method final-state (v :: <vector>)
  if (v.size == 0)
    #f
  else
    v.size - 1;
  end if;
end method final-state;

define method next-state (v :: <vector>, state :: <integer-state>)
  if (state < v.size - 1)
    state + 1;
  else
    #f;
  end if;
end method next-state;

define method previous-state (v :: <vector>, state :: <integer-state>)
  if (state <= 0)
    #f;
  else
    state - 1;
  end if;
end method previous-state;

//
// Methods on <byte-vector>
//

define method size (v :: <byte-vector>)
  %bytevector-size (v);
end method size;

define method element-type (v :: <byte-vector>)
  <byte>
end method element-type;

define method element (v :: <byte-vector>, i :: <small-integer>,
		       #key default = %default-object)
 => (value :: <byte>);
  %bytevector-element (v, i, default);
end method element;

define method element-setter (obj :: <byte>, v :: <byte-vector>, i :: <small-integer>)
 => (value :: <byte>);
  %bytevector-element-setter (v, i, obj);
end method element-setter;

//
// Iteration on <byte-vector>
//

define method current-element (v :: <byte-vector>, state :: <integer-state>)
  %bytevector-element (v, state, %default-object);
end method current-element;

define method current-element-setter (obj,
				      v :: <byte-vector>,
				      state :: <integer-state>)
  %bytevector-element-setter (v, state, obj);
end method current-element-setter;

//
// Methods on <simple-object-vector>
//

define method size (v :: <simple-object-vector>)
  %vector-size (v);
end method size;

define method element (v :: <simple-object-vector>, i :: <small-integer>,
		       #key default = %default-object)
  %vector-element (v, i, default);
end method element;

define method element-setter (obj, v :: <simple-object-vector>, i :: <small-integer>)
  %vector-element-setter (v, i, obj);
end method element-setter;

//
// Iteration on <simple-object-vector>
//

define method current-element (v :: <simple-object-vector>, state :: <integer-state>)
  %vector-element (v, state, %default-object);
end method current-element;

define method current-element-setter (obj,
				      v :: <simple-object-vector>,
				      state :: <integer-state>)
  %vector-element-setter (v, state, obj);
end method current-element-setter;
