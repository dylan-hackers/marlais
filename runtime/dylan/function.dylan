module: dylan

//
// function.dylan
//
// Authors:
//

// Generic methods on functions

define method function-specializers (func :: <function>)
 => type-sequence :: <sequence>;
  %function-specializers(func);
end method function-specializers;

define method function-return-values (func :: <function>)
 => (type-sequence :: <sequence>, rest-value :: <object>);
  %function-values(func);
end method function-return-values;

define method function-arguments (f :: <function>)
  %function-arguments(f);
end method function-arguments;

// Methods on generic functions

define method generic-function-methods (gf :: <generic-function>)
  %generic-function-methods(gf);
end method generic-function-methods;

define method add-method (gf :: <generic-function>, meth :: <method>)
  %add-method(gf, meth);
end method add-method;

define method generic-function-mandatory-keywords (gf :: <generic-function>)
  %generic-function-mandatory-keywords(gf);
end method generic-function-mandatory-keywords;

define method applicable-method? (m :: <function>, #rest args)
  %apply(%applicable-method?, pair(m, args));
end method applicable-method?;

define method sorted-applicable-methods (gf :: <generic-function>, #rest args)
  %apply(%sorted-applicable-methods, pair(gf, args));
end method sorted-applicable-methods;

define method find-method (gf :: <generic-function>, #rest sample-args)
  %find-method(gf, sample-args);
end method find-method;

define method remove-method (gf :: <generic-function>, meth :: <method>)
  %remove-method(gf, meth);
end method remove-method;

define method make (gftype == <generic-function>,
		    #key required, rest, key, all-keys)
  if (instance? (required, <number>))
    required := make (<list>, size: required, fill: <object>);
  end if;
  if (instance? (required, <list>))
    %generic-function-make (list (required, rest, key, all-keys));
  else
    error ("make: bad key value", required:, required);
  end if;
end method make;

// Function application

define method apply (f :: <function>, #rest args)

  // collect-args: collects the arguments into a single list.
  //  Splices in the contents of the last (collection) argument.
  local method collect-args (args)
	  case
	    empty?(args) => #();
	    empty?(tail(args)) =>
	      if (~instance?(head(args), <sequence>))
		error("apply:  last arg must be a sequence", head(args));
	      else
		as (<list>, head(args));
	      end if;
	    otherwise =>
	      // first reverse the arguments.
	      let res = reverse (args);
	      let argseq = head (res);
	      let res = tail (res);

	      // Then splice in the elements of the sequence.
	      for (val in argseq)
		res := pair (val, res);
	      end for;

	      // now fix up the arguments again
	      reverse!(res);
	  end case;
	end collect-args;

  %apply(f, collect-args(args));
end method apply;
