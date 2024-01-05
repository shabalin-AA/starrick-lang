#ifndef _BUILTIN_VERBS_H_
#define _BUILTIN_VERBS_H_


#include "internals.h"

da_Verb __verbs;

Verb* find_verb(const char word[VERB_LEN]) {
	for (int i = __verbs.q-1; i >= 0; i--) {
		if (strcmp(__verbs.values[i].word, word) == 0) 
			return &__verbs.values[i];
	}
	return NULL;
}

void push_verb(Verb v) {
	DA_PUSH(&__verbs, Verb, v);
}

void __quit(void) { exit(0); }

void __add(void) {
	checkargc(2, "can add only 2 numbers");
	Value a = pop_value();
	Value b = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cannot add not a number");
	checktype(&b, VALUE_TYPE_NUMBER, "cannot add not a number");
	a.as.number += b.as.number;
	push_value(a);
}

void __mul(void) {
	checkargc(2, "can multiply only 2 numbers");
	Value a = pop_value();
	Value b = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cannot multiply not a number");
	checktype(&b, VALUE_TYPE_NUMBER, "cannot multiply not a number");
	a.as.number *= b.as.number;
	push_value(a);
}

void __sub(void) {
	checkargc(2, "can subtract only 2 numbers");
	Value a = pop_value();
	Value b = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cannot subtract not a number");
	checktype(&b, VALUE_TYPE_NUMBER, "cannot subtract not a number");
	a.as.number -= b.as.number;
	push_value(a);
}

void __div(void) {
	checkargc(2, "can divide only 2 numbers");
	Value a = pop_value();
	Value b = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cannot divide not a number");
	checktype(&b, VALUE_TYPE_NUMBER, "cannot divide not a number");
	a.as.number /= b.as.number;
	push_value(a);
}

void __cp(void) {
	checkargc(1, "no value to copy");
	Value* a = last_value();
	push_value(copy_value(a));
}

void __pop(void) {
	deinit_Value(pop_value());
}

void __flip(void) {
	checkargc(2, "can flip only 2 values");
	Value a = pop_value();
	Value b = pop_value();
	push_value(a);
	push_value(b);
}

void __cp2(void) {
	checkargc(2, "can copy 2nd only with 2 values");
	Value a = pop_value();
	Value* b = last_value();
	push_value(a);
	push_value(copy_value(b));
}


#endif