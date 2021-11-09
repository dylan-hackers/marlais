module: dylan

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
