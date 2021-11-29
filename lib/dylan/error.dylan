module: dylan

//
// error.dylan
//
// Authors:
//

define method error (msg :: <string>, #rest args)
  %apply(%error, %pair(msg, args));
end method error;

define method warning (msg :: <string>, #rest args)
  %apply(%warning, %pair(msg, args));
end method warning;

define method cerror (#rest args)
  format(*standard-error*, "cerror: called with arguments %=", args);
end method cerror;

define method signal (#rest args)
  %signal-error-jump();
end method signal;
