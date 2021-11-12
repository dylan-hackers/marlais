/*
   Data Representation:

   pointer:   PPPPPPPPPPPPPPPPPPPPPPPPPPPPPP00  (P=pointer address bit)
   immed:     DDDDDDDDDDDDDDDDDDDDDDDDDDSSSS01  (D=immediate data, S=secondary tag)
   integer:   IIIIIIIIIIIIIIIIIIIIIIIIIIIIII10  (I=immediate integer data)

 */

/* Type for marlais objects */
typedef void *Object;

/* Type for handling value tags */
typedef DyUnsigned MarlaisTag;
#define MARLAIS_TAG_POINTER       (0x0)
#define MARLAIS_TAG_IMMEDIATE     (0x1)
#define MARLAIS_TAG_INTEGER       (0x2)
#define MARLAIS_TAG_MASK          (0x3)

/* Type for handling value subtags */
typedef DyUnsigned MarlaisSub;
#define MARLAIS_SUB_SHIFT         2
#define MARLAIS_SUB_TRUE          (0x0 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_FALSE         (0x1 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_EMPTYLIST     (0x2 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_EOF           (0x3 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_UNSPECIFIED   (0x4 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_UNINITIALIZED (0x5 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_CHARACTER     (0x6 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_WCHAR         (0x7 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_MASK          (0xf << MARLAIS_SUB_SHIFT)

/* Shift distances for value fields */
#define MARLAIS_INTEGER_SHIFT   (2)
#define MARLAIS_IMMEDIATE_SHIFT (6)

/* Immediate constants */
#define TRUEVAL         ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_TRUE))
#define FALSEVAL        ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_FALSE))
#define EMPTYLISTVAL    ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_EMPTYLIST))
#define EOFVAL          ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_EOF))
#define UNSPECVAL       ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_UNSPECIFIED))
#define UNINITVAL       ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_UNINITIALIZED))
#define MAX_SMALL_INT   (INTPTR_MAX >> MARLAIS_INTEGER_SHIFT)

/* Field extraction */
static inline DyUnsigned TAGPART(Object obj) {
  return (((DyUnsigned)obj) & MARLAIS_TAG_MASK);
}
static inline DyUnsigned SUBPART(Object obj) {
  return (((DyUnsigned)obj) & MARLAIS_SUB_MASK);
}
static inline DyUnsigned INTEGERPART(Object obj) {
  return (((DyUnsigned)obj) >> MARLAIS_INTEGER_SHIFT);
}
static inline DyUnsigned IMMEDPART(Object obj) {
  return (((DyUnsigned)obj) >> MARLAIS_IMMEDIATE_SHIFT);
}

/* Tag type predicates */
static inline bool POINTERP(Object obj) {
  return (TAGPART(obj) == MARLAIS_TAG_POINTER);
}
static inline bool IMMEDP(Object obj) {
  return (TAGPART(obj) == MARLAIS_TAG_IMMEDIATE);
}
static inline bool INTEGERP(Object obj) {
  return (TAGPART(obj) == MARLAIS_TAG_INTEGER);
}

/* Immediate extraction */
static inline DyInteger INTVAL(Object obj) {
  return ((DyInteger)INTEGERPART(obj));
}
static inline char CHARVAL(Object obj) {
  return ((char)(IMMEDPART(obj) & 0xFF));
}
#ifdef MARLAIS_ENABLE_WCHAR
static inline wchar_t WCHARVAL(Object obj) {
  return ((wchar_t)IMMEDPART(obj));
}
#endif

/* Immediate composition */
static inline Object MAKE_IMMEDIATE(MarlaisSub sub, DyUnsigned val) {
  return (Object)(MARLAIS_TAG_IMMEDIATE|sub|(val << MARLAIS_IMMEDIATE_SHIFT));
}
static inline Object MAKE_CHAR(char ch) {
  return MAKE_IMMEDIATE(MARLAIS_SUB_CHARACTER, (ch & 0xFF));
}
#ifdef MARLAIS_ENABLE_WCHAR
static inline Object MAKE_WCHAR(wchar_t ch) {
  return MAKE_IMMEDIATE(MARLAIS_SUB_WCHAR, ch);
}
#endif
static inline Object MAKE_INT(DyInteger ch) {
  return (Object)(MARLAIS_TAG_INTEGER|(((DyUnsigned)ch) << MARLAIS_INTEGER_SHIFT));
}

/* Immediate type predicates */
#define DEFINE_IMMEDP_PREDICATE(_name, _sub)        \
  static inline bool _name(Object obj) {            \
    return (IMMEDP(obj) && (SUBPART(obj) == _sub)); \
  }
DEFINE_IMMEDP_PREDICATE(TRUEP, MARLAIS_SUB_TRUE);
DEFINE_IMMEDP_PREDICATE(FALSEP, MARLAIS_SUB_FALSE);
DEFINE_IMMEDP_PREDICATE(EMPTYLISTP, MARLAIS_SUB_EMPTYLIST);
DEFINE_IMMEDP_PREDICATE(EOFP, MARLAIS_SUB_EOF);
DEFINE_IMMEDP_PREDICATE(UNSPECP, MARLAIS_SUB_UNSPECIFIED);
DEFINE_IMMEDP_PREDICATE(UNINITSLOTP, MARLAIS_SUB_UNINITIALIZED);
DEFINE_IMMEDP_PREDICATE(CHARP, MARLAIS_SUB_CHARACTER);
DEFINE_IMMEDP_PREDICATE(WCHARP, MARLAIS_SUB_WCHAR);
#undef DEFINE_IMMEDP_PREDICATE

/* Pointer type predicates */
#define DEFINE_POINTERP_PREDICATE(_name, _type)            \
  static inline bool _name(Object obj) {                   \
    return (POINTERP(obj) && (POINTERTYPE(obj) == _type)); \
  }
DEFINE_POINTERP_PREDICATE(BIGINTP, BigInteger);
DEFINE_POINTERP_PREDICATE(SFLOATP, SingleFloat);
DEFINE_POINTERP_PREDICATE(DFLOATP, DoubleFloat);
DEFINE_POINTERP_PREDICATE(PAIRP, Pair);
DEFINE_POINTERP_PREDICATE(BYTESTRP, ByteString);
DEFINE_POINTERP_PREDICATE(SOVP, SimpleObjectVector);
DEFINE_POINTERP_PREDICATE(RATIOP, Ratio);
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
#undef DEFINE_POINTERP_PREDICATE
