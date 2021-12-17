module: dylan

define constant <module-or-name> = type-union(<module>, <symbol>);

define method set-module (module-or-name :: <module-or-name>)
  %set-module (module-or-name);
end method;

define method find-module (name :: <symbol>)
 => (module :: false-or(<module>));
  %find-module (name);
end method;

define method module-name (module :: <module>)
 => (name :: <symbol>);
  %module-name (module);
end method;

define method module-environment (module :: <module>)
 => (environment :: <environment>);
  %module-environment (module);
end method;
