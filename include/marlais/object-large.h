
typedef ObjectHeader *Object;

/* Constants for boxed integers */
#define MARLAIS_INTEGER_MIN   (MARLAIS_INT_MIN)
#define MARLAIS_INTEGER_MAX   (MARLAIS_INT_MAX)
#define MARLAIS_INTEGER_WIDTH (sizeof(DyInteger) * 8)
#define MARLAIS_INTEGER_PRI    MARLAIS_INT_PRI

/* All objects are pointers */
static inline bool POINTERP(Object obj) {
  return true;
}

/* Type predicates */
#define DEFINE_POINTERP_PREDICATE(_name, _type)                \
  static inline bool _name(Object obj) {                   \
    return (POINTERP(obj) && (POINTERTYPE(obj) == _type)); \
  }
DEFINE_POINTERP_PREDICATE(TRUEP, True);
DEFINE_POINTERP_PREDICATE(FALSEP, False);
DEFINE_POINTERP_PREDICATE(EMPTYLISTP, EmptyList);
DEFINE_POINTERP_PREDICATE(CHARP, Character);
DEFINE_POINTERP_PREDICATE(WCHARP, WideCharacter);
DEFINE_POINTERP_PREDICATE(UCHARP, UnicodeCharacter);
DEFINE_POINTERP_PREDICATE(INTEGERP, Integer);
DEFINE_POINTERP_PREDICATE(RATIOP, Ratio);
DEFINE_POINTERP_PREDICATE(EOFP, EndOfFile);
DEFINE_POINTERP_PREDICATE(UNSPECP, UnspecifiedValue);
DEFINE_POINTERP_PREDICATE(UNINITSLOTP, UninitializedValue);
DEFINE_POINTERP_PREDICATE(SFLOATP, SingleFloat);
DEFINE_POINTERP_PREDICATE(DFLOATP, DoubleFloat);
DEFINE_POINTERP_PREDICATE(PAIRP, Pair);
DEFINE_POINTERP_PREDICATE(BYTESTRP, ByteString);
DEFINE_POINTERP_PREDICATE(SOVP, SimpleObjectVector);
DEFINE_POINTERP_PREDICATE(TEP, TableEntry);
DEFINE_POINTERP_PREDICATE(TABLEP, ObjectTable);
DEFINE_POINTERP_PREDICATE(DEP, DequeEntry);
DEFINE_POINTERP_PREDICATE(DEQUEP, Deque);
DEFINE_POINTERP_PREDICATE(ARRAYP, Array);
DEFINE_POINTERP_PREDICATE(CONDP, Condition);
DEFINE_POINTERP_PREDICATE(NAMEP, Name);
DEFINE_POINTERP_PREDICATE(SYMBOLP, Symbol);
DEFINE_POINTERP_PREDICATE(SLOTDP, SlotDescriptor);
DEFINE_POINTERP_PREDICATE(INSTANCEP, Instance);
DEFINE_POINTERP_PREDICATE(CLASSP, Class);
DEFINE_POINTERP_PREDICATE(SINGLETONP, Singleton);
DEFINE_POINTERP_PREDICATE(SUBCLASSP, Subclass);
DEFINE_POINTERP_PREDICATE(LIMINTP, LimitedIntType);
DEFINE_POINTERP_PREDICATE(UNIONP, UnionType);
DEFINE_POINTERP_PREDICATE(PRIMP, Primitive);
DEFINE_POINTERP_PREDICATE(GFUNP, GenericFunction);
DEFINE_POINTERP_PREDICATE(METHODP, Method);
DEFINE_POINTERP_PREDICATE(NMETHP, NextMethod);
DEFINE_POINTERP_PREDICATE(VALUESP, Values);
DEFINE_POINTERP_PREDICATE(EXITP, Exit);
DEFINE_POINTERP_PREDICATE(UNWINDP, Unwind);
#ifdef NO_COMMON_DYLAN_SPEC
DEFINE_POINTERP_PREDICATE(STREAMP, Stream);
#endif
DEFINE_POINTERP_PREDICATE(FOREIGNP, ForeignPtr);
DEFINE_POINTERP_PREDICATE(ENVIRONMENTP, Environment);
DEFINE_POINTERP_PREDICATE(HDLP, ObjectHandle);
DEFINE_POINTERP_PREDICATE(MPFP, MPFloat);
DEFINE_POINTERP_PREDICATE(MPQP, MPRatio);
DEFINE_POINTERP_PREDICATE(MPZP, MPInteger);
#undef DEFINE_POINTERP_PREDICATE
