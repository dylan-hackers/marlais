module: dylan

//
// make
//

define generic make(c :: <class>, #rest args, #key, #all-keys)
  => (instance :: <object>);

define method make (c :: <class>, #rest args, #key, #all-keys)
  %make(c, args);
end method make;

define constant instance? =
  method (obj, typ :: <type>)
    %instance?(obj, typ);
  end method;

define constant singleton =
  method (o :: <object>) => <singleton>;
    %singleton (o);
  end method;

//
//  This just doesn't look right to me!!!!  jnw.
//

define generic initialize (instance, #rest args, #key, #all-keys)
  => (objects);

define method initialize (instance, #rest args, #key, #all-keys)
end method initialize;

define constant identity =
  method (x :: <object>)
    x;
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
