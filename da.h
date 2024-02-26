#ifndef _DA_H_
#define _DA_H_


#include <stdlib.h>

#define DA_0_SIZE 8
#define DA_SCALE 2

#define DA_INIT(DA, TYPE) \
  do { \
    (DA)->capacity = DA_0_SIZE; \
    (DA)->q = 0; \
    (DA)->values = malloc(sizeof(TYPE) * (DA)->capacity); \
  } while(0)

#define DA_PUSH(DA, VALUE) \
  do { \
		if ((DA)->values == NULL) { \
	    (DA)->capacity = DA_0_SIZE; \
	    (DA)->q = 0; \
	    (DA)->values = malloc(sizeof(VALUE) * (DA)->capacity); \
		} \
    if ((DA)->q == (DA)->capacity) { \
      (DA)->capacity *= DA_SCALE; \
      (DA)->values = realloc((DA)->values, sizeof(VALUE) * (DA)->capacity); \
    } \
    (DA)->values[(DA)->q++] = VALUE; \
  } while(0)

#define DA_POP(DA) \
  do { \
    (DA)->q--; \
    if ((DA)->q > DA_0_SIZE && (DA)->capacity / (DA)->q >= DA_SCALE) { \
      (DA)->capacity /= DA_SCALE; \
      (DA)->values = realloc((DA)->values, sizeof((DA)->values[0]) * (DA)->capacity); \
    } \
  } while(0)

#define DA_DEINIT(DA) free((DA)->values)
			
		
#endif