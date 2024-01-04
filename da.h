#include <stdlib.h>

#define DA_0_SIZE 8
#define DA_SCALE 2

#define DA_INIT(DA, TYPE) \
  do { \
    (DA)->capacity = DA_0_SIZE; \
    (DA)->q = 0; \
    (DA)->values = malloc(sizeof(TYPE) * (DA)->capacity); \
  } while(0)

#define DA_PUSH(DA, TYPE, VALUE) \
  do { \
    if ((DA)->q == (DA)->capacity) { \
      (DA)->capacity *= DA_SCALE; \
      (DA)->values = realloc((DA)->values, sizeof(TYPE) * (DA)->capacity); \
    } \
    (DA)->values[(DA)->q++] = VALUE; \
  } while(0)

#define DA_POP(DA, TYPE) \
  do { \
    (DA)->q--; \
    if ((DA)->q > DA_0_SIZE && (DA)->capacity / (DA)->q >= DA_SCALE) { \
      (DA)->capacity /= DA_SCALE; \
      (DA)->values = realloc((DA)->values, sizeof(TYPE) * (DA)->capacity); \
    } \
  } while(0)

#define DA_DEINIT(DA) \
		free((DA)->values)