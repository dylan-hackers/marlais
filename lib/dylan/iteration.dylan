module: dylan

//
// Iteration protocol generics
//

define generic forward-iteration-protocol (c :: <collection>)
  => (initial-state, limit, next-state, finished-state?, current-key,
      current-element, current-element-setter, copy-state);

define generic backward-iteration-protocol (c :: <collection>)
  => (initial-state, limit, previous-state, finished-state?, current-key,
      current-element, current-element-setter, copy-state);

//
// Methods for generic implementation
//

define generic initial-state (c :: <collection>)
  => (state :: <object>);
define generic final-state (c :: <collection>)
  => (state :: <object>);
define generic forward-limit (c :: <collection>)
  => (limit :: <object>);
define generic backward-limit (c :: <collection>)
  => (limit :: <object>);
define generic next-state (c :: <collection>, state :: <object>)
  => (state :: <object>);
define generic previous-state (c :: <collection>, state :: <object>)
  => (state :: <object>);
define generic initial-state? (c :: <collection>, state :: <object>)
  => (initial? :: <boolean>);
define generic finished-state? (c :: <collection>, state :: <object>, limit :: <object>)
  => (finished? :: <boolean>);
define generic current-key (c :: <collection>, state :: <object>)
  => (key :: <object>);
define generic current-element (c :: <collection>, state :: <object>)
  => (element :: <object>);
define generic current-element-setter (element :: <object>, c :: <collection>, state :: <object>)
  => (element :: <object>);

//
// Default implementation for limits
//

define method forward-limit (c :: <collection>)
  #f
end method next-state;

define method backward-limit (c :: <collection>)
  #f
end method next-state;

define method finished-state? (c :: <collection>, state, limit)
  => (finished? :: <boolean>);
  state == limit;
end method finished-state?;

define method current-key (s :: <collection>, state)
  state
end method current-key;

define method copy-state (s :: <collection>, state)
  state
end method copy-state;

//
// Default methods for generic implementation
//

define method initial-state (c :: <collection>)
  error ("Don't know how to find initial state", c);
end method next-state;

define method final-state (c :: <collection>)
  error ("Don't know how to find final state", c);
end method next-state;

define method next-state (c :: <collection>, state)
  error ("Don't know how to find next state", c);
end method next-state;

define method previous-state (c :: <collection>, state)
  error ("Don't know how to find next state", c);
end method next-state;

define method current-element (c :: <collection>, state)
  error("Don't know how to find current element", c);
end method current-element;

define method current-element-setter (e,  c :: <collection>, state)
  error ("Don't know how to set current element", c);
end method current-element-setter;

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
  let limit = forward-limit (c);
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
  let limit = backward-limit (c);
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
