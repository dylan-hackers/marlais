module: dylan

// synchro.dylan
//
// Authors:
//  Ingo Albrecht
//

//
// <synchronization>
//

define open abstract class <synchronization> (<object>)
end class;

define open generic wait-for(object :: <synchronization>,
                             #key timeout :: false-or(<real>))
 => (success? :: <boolean>);

define open generic release(object :: <synchronization>, #key)
 => ();

//
// <lock>
//

define open abstract class <lock> (<synchronization>)
end class;

define method make(cls == <lock>, #rest inits, #key, #all-keys)
 => (lock :: <lock>);
  apply(make, <simple-lock>, inits);
end method;

//
// <exclusive-lock>
//

define open abstract class <exclusive-lock> (<lock>)
end class;

define open generic owned?(lock :: <exclusive-lock>)
 => (owned? :: <boolean>);

define method make(cls == <exclusive-lock>, #rest inits, #key, #all-keys)
 => (lock :: <exclusive-lock>);
  apply(make, <simple-lock>, inits);
end method;

//
// <semaphore>
//

define open primary class <semaphore> (<lock>)
end class;

define sealed method wait-for(sem :: <semaphore>,
                              #key timeout :: false-or(<real>) = #f)
 => (success? :: <boolean>);
  #f
end method;

define sealed method release(sem :: <semaphore>, #key, #all-keys)
 => ();
end method;

//
// <simple-lock>
//

define open primary class <simple-lock> (<exclusive-lock>)
end class;

define sealed method wait-for(lock :: <simple-lock>,
                       #key timeout :: false-or(<real>) = #f)
 => (success? :: <boolean>);
  #f
end method;

define sealed method release(lock :: <simple-lock>, #key, #all-keys)
 => ();
  #f
end method;

define sealed method owned?(lock :: <simple-lock>)
 => (owned? :: <boolean>);
  #f
end method;

//
// <recursive-lock>
//

define open primary class <recursive-lock> (<exclusive-lock>)
end class;

define sealed method wait-for(lock :: <recursive-lock>,
                              #key timeout :: false-or(<real>) = #f)
 => (success? :: <boolean>);
  #f
end method;

define sealed method release(lock :: <recursive-lock>, #key, #all-keys)
 => ();
  #f
end method;

define sealed method owned?(lock :: <recursive-lock>)
 => (owned? :: <boolean>);
  #f
end method;

//
// <read-write-lock>
//

define open primary class <read-write-lock> (<exclusive-lock>)
end class;

define sealed method wait-for(lock :: <read-write-lock>,
                              #key timeout :: false-or(<real>) = #f,
                                   mode :: <read-write-lock-mode> = read:)
 => (success? :: <boolean>);
  #f
end method;

define sealed method release(lock :: <read-write-lock>, #key, #all-keys)
 => ();
  #f
end method;

define sealed method owned?(lock :: <recursive-lock>)
 => (owned? :: <boolean>);
  #f
end method;

//
// <notification>
//

define sealed class <notification> (<synchronization>)
end class;

define sealed method wait-for(not :: <notification>,
                       #key timeout :: false-or(<real>) = #f)
 => (success? :: <boolean>);
  #f
end method;

define sealed method release(not :: <notification>, #key, #all-keys)
 => ();
  #f
end method;

define method release-all(not :: <notification>)
 => ();
  #f
end function;
