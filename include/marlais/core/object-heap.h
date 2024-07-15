
struct empty {
    ObjectHeader header;
};

enum condtype {
    SimpleError, TypeError, SimpleWarning,
    SimpleRestart, Abort
};

struct condition {
    ObjectHeader header;
    enum condtype condtype;
};

#define CONDCTYPE(obj)    (((struct condition *)obj)->condtype)

struct instance {
    ObjectHeader header;
    Object class;
    Object *slots;
};

#define INSTCLASS(obj)    (((struct instance *)obj)->class)
#define INSTSLOTS(obj)    (((struct instance *)obj)->slots)

struct object_handle {
    ObjectHeader header;
    Object the_object;
};

#define HDLOBJ(obj)      (((struct object_handle *)obj)->the_object)
