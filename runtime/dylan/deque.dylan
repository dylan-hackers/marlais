module: dylan

//
// deque.dylan
//
// Authors:
//   Brent Benson
//

//
// Sizing
//

// This method required because the general empty? method assumes that
// the final state is #f.
define method empty? (d :: <deque>)
  if (%deque-initial-state (d))
    #f
  else
    #t
  end
end method empty?;

define method size (d :: <deque>)
  %deque-size (d);
end method;

//
// Push and pop
//

define method push (d :: <deque>, new)
  %deque-push (d, new);
end method push;

define method pop (d :: <deque>)
  %deque-pop (d);
end method pop;

define method push-last (d :: <deque>, new)
  %deque-push-last (d, new);
end method push-last;

define method pop-last (d :: <deque>)
  %deque-pop-last (d);
end method pop-last;

//
// Element access
//

define method first (d ::<deque>, #key default = %default-object)
  %deque-first (d, default);
end method first;

define method last (d :: <deque>, #key default = %default-object)
  %deque-last (d, default)
end method last;

// TODO mising first-setter and last-setter? check the spec.

define method element (d ::<deque>, i :: <small-integer>,
		       #key default = %default-object)
  %deque-element (d, i, default)
end method element;

define method element-setter (new, d :: <deque>, i :: <small-integer>)
  %deque-element-setter (d, i, new)
end method element-setter;

//
// Insertion and removal
//

define method add! (d :: <deque>, new)
  %deque-push (d, new);
end method add;

define method remove! (d :: <deque>, value,
                       #key test = \==, count)
 => (sequence2 :: <sequence>);
  %deque-remove! (d, value, test, count);
end method remove!;

//
// Coercion
//

define method as (cls == <deque>, s :: <list>)
  %list->deque (s);
end method;

define method as (cls == <deque>, s :: <vector>)
  %vector->deque (s);
end method;

define method as (cls == <list>, s :: <deque>)
  %deque->list (s);
end method;

define method as (cls == <vector>, s :: <deque>)
  %deque->vector (s);
end method;

//
// Iteration protocol
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
  let limit = forward-limit (d);
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
