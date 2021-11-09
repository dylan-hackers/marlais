module: dylan

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
