/* object.c -- see COPYRIGHT for use */

#include "common.h"

#include "alloc.h"
#include "error.h"
#include "number.h"

#ifdef SMALL_OBJECTS

ObjectType
object_type (Object obj)
{
    if (POINTERP (obj)) {
        return (PAIRTYPE (obj));
    } else if (IMMEDP (obj)) {
        switch (SUBPART (obj)) {
        case TRUESUB:
            return (True);
        case FALSESUB:
            return (False);
        case EMPTYSUB:
            return (EmptyList);
        case CHARSUB:
            return (Character);
        case EOFSUB:
            return (EndOfFile);
        case UNSPECSUB:
            return (Unspecified);
        case UNINITSUB:
            return (UninitializedSlotValue);
        default:
            marlais_fatal ("internal error: object with unknown immediate tag");
        }
    } else {
        return (Integer);
    }
	return (Uninitialized);
}

#else

ObjectType
object_type (Object obj) {
  return POINTERTYPE(obj);
}

#endif

Object
make_handle (Object an_object)
{
    Object new_handle;

    new_handle = marlais_allocate_object (ObjectHandle, sizeof (struct object_handle));

    HDLOBJ (new_handle) = an_object;
    return (new_handle);
}
