#ifndef MARLAIS_CONFIG_H
#define MARLAIS_CONFIG_H

/* Enable readline support */
#define MARLAIS_ENABLE_READLINE

/* Enable big integer support */
#undef MARLAIS_ENABLE_BIG_INTEGERS
/* Enable method caching */
#define MARLAIS_ENABLE_METHOD_CACHING
/* Enable tail call optimization */
#define MARLAIS_ENABLE_TAIL_CALL_OPTIMIZATION

/* Select class precedence algorithm */
#undef MARLAIS_CLASS_PRECEDENCE_CLOS
#define MARLAIS_CLASS_PRECEDENCE_LL

/* Select object model */
#undef MARLAIS_OBJECT_MODEL_LARGE
#define MARLAIS_OBJECT_MODEL_SMALL

#define MAX_STRING_SIZE 10240
#define MAX_SYMBOL_SIZE 1024
#define MAX_NUMBER_SIZE 255

#endif
