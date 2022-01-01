module: dylan

//
// object.dylan
//
// Authors:
//

//
// Base implementation of make and initialize
//

define generic make(c :: <class>, #rest args, #key, #all-keys)
  => (instance :: <object>);

define method make (c :: <class>, #rest args, #key, #all-keys)
  %make(c, args);
end method make;

define generic initialize (instance, #rest args, #key, #all-keys)
  => (objects);

define method initialize (instance, #rest args, #key, #all-keys)
end method initialize;

// define method =hash (obj)
//   %=hash(obj);
// end method =hash;

//
// Slot initialization
//

define constant slot-initialized? =
  method (obj, slot)
     slot(obj) ~== %uninitialized-slot-value;
  end method;

//
// Type predicates
//

define constant instance? =
  method (obj, typ :: <type>)
    %instance?(obj, typ);
  end method;

define constant subtype? =
  method (t1 :: <type>, t2 :: <type>)
    %subtype?(t1, t2);
  end method;

//
// Type creation
//

define constant singleton =
  method (o :: <object>) => <singleton>;
    %make-singleton (o);
  end method;

define constant subclass =
  method (c :: <class>) => <subclass>;
    %make-subclass (c);
  end method;

define constant type-union =
  method (#rest types)
    %make-union (types);
  end method;

define generic limited (c :: <class>, #rest keys, #key, #all-keys)
  => (type :: <type>);

define method limited (int == <integer>, #rest args, #key min, max)
 => (limited-type :: <limited-integer>);
  %make-limited-integer(args);
end method;

//
// Class information
//

define constant class-abstract? =
  method (c :: <class>)
    %class-abstract?(c);
  end method;

define constant class-primary? =
  method (c :: <class>)
    %class-primary?(c);
  end method;

define constant class-sealed? =
  method (c :: <class>)
    %class-sealed?(c);
  end method;

define constant class-builtin? =
  method (c :: <class>)
    %class-builtin?(c);
  end method;

define constant class-defined? =
  method (c :: <class>)
    %class-defined?(c);
  end method;

define constant class-immediate? =
  method (c :: <class>)
    %class-immediate?(c);
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

//
// Copying
//

define generic shallow-copy (o :: <object>)
  => (new-object :: <object>);

define generic type-for-copy (o :: <object>)
  => (type :: <class>);

define method shallow-copy (o :: <object>)
 => (new-object :: <object>);
  o;
end method shallow-copy;

define method type-for-copy (o :: <object>)
 => (type :: <class>);
  o.object-class
end method type-for-copy;

//
// Coercion
//

define generic as (c :: <class>, obj :: <object>)
  => (object :: <object>);

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
