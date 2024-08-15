/*
 * Object representation with tagging on the lowest bits.
 *
 * Values are tagged to distinguish three cases:
 *
 *  Pointer:   PPPPPPPPPPPPPPPPPPPPPPPPPPPPPP00  (P=pointer address bit)
 *  Immediate: DDDDDDDDDDDDDDDDDDDDDDDDDDSSSS01  (D=immediate data, S=secondary tag)
 *  Integer:   IIIIIIIIIIIIIIIIIIIIIIIIIIIIII10  (I=immediate integer data)
 *
 */

/* Type for marlais objects */
typedef void *Object;

/* Determine the representation of an object */
extern marlais_repr_t marlais_object_repr (Object obj);

/* Type for handling value tags */
typedef uintptr_t marlais_tag_t;
#define MARLAIS_TAG_SHIFT         (0)
#define MARLAIS_TAG_WIDTH         (2)
#define MARLAIS_TAG_POINTER       (0x0)
#define MARLAIS_TAG_IMMEDIATE     (0x1)
#define MARLAIS_TAG_INTEGER       (0x2)
#define MARLAIS_TAG_MASK          (0x3)

/* Type for handling value subtags */
typedef uintptr_t marlais_subtag_t;
#define MARLAIS_SUB_COUNT         (16)
#define MARLAIS_SUB_SHIFT         (2)
#define MARLAIS_SUB_WIDTH         (4)
#define MARLAIS_SUB_TRUE          (0x0 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_FALSE         (0x1 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_EMPTYLIST     (0x2 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_EOF           (0x3 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_UNSPECIFIED   (0x4 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_UNINITIALIZED (0x5 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_CHARACTER     (0x6 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_WCHAR         (0x7 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_UCHAR         (0x8 << MARLAIS_SUB_SHIFT)
#define MARLAIS_SUB_MASK          (0xf << MARLAIS_SUB_SHIFT)

/* Types corresponding to subtags (exactly 16 entries) */
#define MARLAIS_SUB_TYPES {                    \
    True, False,                               \
    EmptyList, EndOfFile,                      \
    UnspecifiedValue, UninitializedValue,      \
    Character, WideCharacter,                  \
    UnicodeCharacter, Uninitialized,           \
    Uninitialized, Uninitialized,              \
    Uninitialized, Uninitialized,              \
    Uninitialized, Uninitialized,              \
  }

/* Shift distances for value fields */
#define MARLAIS_INTEGER_SHIFT   (2)
#define MARLAIS_IMMEDIATE_SHIFT (6)

/* Constants for tagged integers */
#define MARLAIS_INTEGER_MIN   (MARLAIS_INT_MIN >> MARLAIS_INTEGER_SHIFT)
#define MARLAIS_INTEGER_MAX   (MARLAIS_INT_MAX >> MARLAIS_INTEGER_SHIFT)
#define MARLAIS_INTEGER_WIDTH ((sizeof(marlais_int_t) * 8) - MARLAIS_TAG_WIDTH)
#define MARLAIS_INTEGER_PRI    MARLAIS_INT_PRI

/* Immediate constants */
#define MARLAIS_TRUE          ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_TRUE))
#define MARLAIS_FALSE         ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_FALSE))
#define MARLAIS_NIL           ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_EMPTYLIST))
#define MARLAIS_EOF           ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_EOF))
#define MARLAIS_UNSPECIFIED   ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_UNSPECIFIED))
#define MARLAIS_UNINITIALIZED ((Object)(MARLAIS_TAG_IMMEDIATE|MARLAIS_SUB_UNINITIALIZED))

/* Field extraction */
static inline marlais_tag_t marlais_get_tag(Object obj) {
  return (((marlais_uint_t)obj) & MARLAIS_TAG_MASK);
}
static inline marlais_subtag_t marlais_get_subtag(Object obj) {
  return (((marlais_uint_t)obj) & MARLAIS_SUB_MASK);
}
static inline marlais_int_t marlais_get_int(Object obj) {
  return (((marlais_int_t)obj) >> MARLAIS_INTEGER_SHIFT);
}
static inline marlais_uint_t marlais_get_uint(Object obj) {
  return (((marlais_uint_t)obj) >> MARLAIS_INTEGER_SHIFT);
}
static inline marlais_uint_t marlais_untag_immediate(Object obj) {
  return (((marlais_uint_t)obj) >> MARLAIS_IMMEDIATE_SHIFT);
}
static inline marlais_repr_t marlais_pointer_repr (Object obj) {
  return ((const marlais_header_t *)obj)->object_repr;
}

/* Tag type predicates */
static inline bool marlais_object_pointer_p(Object obj) {
  return (marlais_get_tag(obj) == MARLAIS_TAG_POINTER);
}
static inline bool marlais_object_immediate_p(Object obj) {
  return (marlais_get_tag(obj) == MARLAIS_TAG_IMMEDIATE);
}
static inline bool marlais_is_integer_p(Object obj) {
  return (marlais_get_tag(obj) == MARLAIS_TAG_INTEGER);
}

/* Immediate extraction */
static inline char CHARVAL(Object obj) {
  return ((char)(marlais_untag_immediate(obj) & 0xFF));
}
#ifdef MARLAIS_ENABLE_WCHAR
static inline wchar_t WCHARVAL(Object obj) {
  return ((wchar_t)marlais_untag_immediate(obj));
}
#endif
#ifdef MARLAIS_ENABLE_UCHAR
static inline UChar32 UCHARVAL(Object obj) {
  return ((UChar32)marlais_untag_immediate(obj));
}
#endif

/* Immediate composition */
static inline Object marlais_make_immediate(marlais_subtag_t sub, marlais_uint_t val) {
  return (Object)(MARLAIS_TAG_IMMEDIATE|sub|(val << MARLAIS_IMMEDIATE_SHIFT));
}
static inline Object MAKE_CHAR(char ch) {
  return marlais_make_immediate(MARLAIS_SUB_CHARACTER, (ch & 0xFF));
}
#ifdef MARLAIS_ENABLE_WCHAR
static inline Object MAKE_WCHAR(wchar_t ch) {
  return marlais_make_immediate(MARLAIS_SUB_WCHAR, ch);
}
#endif
#ifdef MARLAIS_ENABLE_UCHAR
static inline Object MAKE_UCHAR(UChar32 ch) {
  return marlais_make_immediate(MARLAIS_SUB_UCHAR, ch);
}
#endif
static inline Object MAKE_INT(marlais_int_t ch) {
  return (Object)(MARLAIS_TAG_INTEGER|(((marlais_uint_t)ch) << MARLAIS_INTEGER_SHIFT));
}

/* Immediate type predicates */
#define DEFINE_IMMEDP_PREDICATE(_name, _sub)            \
  static inline bool _name(Object obj) {                \
    return (marlais_object_immediate_p(obj)             \
            && (marlais_get_subtag(obj) == (_sub)));    \
  }
DEFINE_IMMEDP_PREDICATE(marlais_is_true_p, MARLAIS_SUB_TRUE);
DEFINE_IMMEDP_PREDICATE(marlais_is_false_p, MARLAIS_SUB_FALSE);
DEFINE_IMMEDP_PREDICATE(marlais_is_nil_p, MARLAIS_SUB_EMPTYLIST);
DEFINE_IMMEDP_PREDICATE(marlais_is_eof_p, MARLAIS_SUB_EOF);
DEFINE_IMMEDP_PREDICATE(marlais_is_unspec_p, MARLAIS_SUB_UNSPECIFIED);
DEFINE_IMMEDP_PREDICATE(marlais_is_uninit_p, MARLAIS_SUB_UNINITIALIZED);
DEFINE_IMMEDP_PREDICATE(marlais_is_bchar_p, MARLAIS_SUB_CHARACTER);
DEFINE_IMMEDP_PREDICATE(marlais_is_wchar_p, MARLAIS_SUB_WCHAR);
DEFINE_IMMEDP_PREDICATE(marlais_is_uchar_p, MARLAIS_SUB_UCHAR);
#undef DEFINE_IMMEDP_PREDICATE

/* Pointer type predicates */
#define DEFINE_POINTERP_PREDICATE(_name, _repr)                         \
  static inline bool _name(Object obj) {                                \
    return (marlais_object_pointer_p(obj)                               \
            && (marlais_pointer_repr(obj) == (_repr)));                 \
  }
DEFINE_POINTERP_PREDICATE(marlais_is_sfloat_p, SingleFloat);
DEFINE_POINTERP_PREDICATE(marlais_is_dfloat_p, DoubleFloat);
DEFINE_POINTERP_PREDICATE(marlais_is_efloat_p, ExtendedFloat);
DEFINE_POINTERP_PREDICATE(marlais_is_ratio_p, Ratio);
DEFINE_POINTERP_PREDICATE(marlais_is_pair_p, Pair);
DEFINE_POINTERP_PREDICATE(marlais_is_bytevector_p, ByteVector);
DEFINE_POINTERP_PREDICATE(marlais_is_array_p, ObjectArray);
DEFINE_POINTERP_PREDICATE(marlais_is_table_p, ObjectTable);
DEFINE_POINTERP_PREDICATE(marlais_is_tableentry_p, ObjectTableEntry);
DEFINE_POINTERP_PREDICATE(marlais_is_deque_p, ObjectDeque);
DEFINE_POINTERP_PREDICATE(marlais_is_dequeentry_p, ObjectDequeEntry);
DEFINE_POINTERP_PREDICATE(marlais_is_vector_p, ObjectVector);
DEFINE_POINTERP_PREDICATE(marlais_is_bstring_p, ByteString);
DEFINE_POINTERP_PREDICATE(marlais_is_wstring_p, WideString);
DEFINE_POINTERP_PREDICATE(marlais_is_ustring_p, UnicodeString);
DEFINE_POINTERP_PREDICATE(marlais_is_condition_p, Condition);
DEFINE_POINTERP_PREDICATE(marlais_is_name_p, Name);
DEFINE_POINTERP_PREDICATE(marlais_is_symbol_p, Symbol);
DEFINE_POINTERP_PREDICATE(marlais_is_slotd_p, SlotDescriptor);
DEFINE_POINTERP_PREDICATE(marlais_is_instance_p, Instance);
DEFINE_POINTERP_PREDICATE(marlais_is_class_p, Class);
DEFINE_POINTERP_PREDICATE(marlais_is_singleton_p, Singleton);
DEFINE_POINTERP_PREDICATE(marlais_is_subclass_p, Subclass);
DEFINE_POINTERP_PREDICATE(marlais_is_limint_p, LimitedIntType);
DEFINE_POINTERP_PREDICATE(marlais_is_union_p, UnionType);
DEFINE_POINTERP_PREDICATE(marlais_is_primitive_p, Primitive);
DEFINE_POINTERP_PREDICATE(marlais_is_generic_p, GenericFunction);
DEFINE_POINTERP_PREDICATE(marlais_is_method_p, Method);
DEFINE_POINTERP_PREDICATE(marlais_is_nextmethod_p, NextMethod);
DEFINE_POINTERP_PREDICATE(marlais_is_values_p, Values);
DEFINE_POINTERP_PREDICATE(marlais_is_unwindfunction_p, UnwindFunction);
DEFINE_POINTERP_PREDICATE(marlais_is_unwindprotect_p, UnwindProtect);
DEFINE_POINTERP_PREDICATE(marlais_is_foreignptr_p, ForeignPtr);
DEFINE_POINTERP_PREDICATE(marlais_is_environment_p, Environment);
DEFINE_POINTERP_PREDICATE(marlais_is_module_p, Module);
DEFINE_POINTERP_PREDICATE(marlais_is_stdio_p, StdioHandle);
DEFINE_POINTERP_PREDICATE(marlais_is_handle_p, ObjectHandle);
DEFINE_POINTERP_PREDICATE(marlais_is_mpf_p, BigFloat);
DEFINE_POINTERP_PREDICATE(marlais_is_mpq_p, BigRatio);
DEFINE_POINTERP_PREDICATE(marlais_is_mpz_p, BigInteger);
#undef DEFINE_POINTERP_PREDICATE
