#ifndef _INTERNALS_H_
#define _INTERNALS_H_


#include <stdlib.h>
#include "value.h"
#include "da.h"

typedef enum {
	S_READ,
	S_EVAL
} STATE;

STATE __state;
da_Value __stack;

void trace() {
	printf("---- trace ----\n");
	for (size_t i = 0; i < __stack.q; i++) {
		println_Value(__stack.values[i]);
	}
}

struct {
	size_t capacity;
	size_t q;
	da_Value** values;
} __stack_to_push;

#define __cur_stack (__stack_to_push.values[__stack_to_push.q-1])

void checkargc(int argc, const char* msg) {
	if (__cur_stack->q < (size_t)argc) {
		printf("ERROR: too few arguments. %s\n", msg);
		trace();
		exit(1);
	}
}

void checktype(Value* v, VALUE_TYPE required_type, const char* msg) {
	if (v->type != required_type) {
		printf("ERROR: %s\n", msg);
		trace();
		exit(1);
	}
}

void push_value(Value v) {
	DA_PUSH(__cur_stack, Value, v); 
}

Value pop_value() {
	checkargc(1, "cant pop from empty stack");
  Value v = __cur_stack->values[__cur_stack->q-1];
  DA_POP(__cur_stack, Value);
  return v;
}

Value* last_value() {
	checkargc(1, "cant get last value from empty stack");
  Value* v = &(__cur_stack->values[__cur_stack->q-1]);
  return v;
}


#endif