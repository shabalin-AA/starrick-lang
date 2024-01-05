#ifndef _VALUE_H_
#define _VALUE_H_


#include <stdio.h>
#include "da.h"

typedef enum {
  VALUE_TYPE_NUMBER,
  VALUE_TYPE_STRING,
	VALUE_TYPE_ARRAY
} VALUE_TYPE;

typedef struct {
  VALUE_TYPE type;
  union {
    double number;
		void* ref;
  } as;
} Value;

typedef struct {
  size_t capacity;
  size_t q;
  Value* values;
} da_Value;

void println_Value(Value v) {
	if (v.type == VALUE_TYPE_NUMBER) printf("<number> %g\n", v.as.number);
	else if (v.type == VALUE_TYPE_ARRAY) {
		printf("<array>\n");
		da_Value* arr = (da_Value*)v.as.ref;
		for (size_t i = 0; i < arr->q; i++) {
			println_Value(arr->values[i]);
		}
	}
}

void deinit_Value(Value v) {
	if (v.type == VALUE_TYPE_ARRAY) {
		da_Value* arr = (da_Value*)v.as.ref;
		for (size_t i = 0; i < arr->q; i++) {
			deinit_Value(arr->values[i]);
		}
		DA_DEINIT(arr);
	}
}

Value copy_value(Value* a) {
	Value b = { .type = a->type, .as = a->as };
	return b;
}


#endif