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

da_Noun __nouns;

Noun* find_noun(const char word[NOUN_LEN]) {
	for (int i = __nouns.q-1; i >= 0; i--) {
		if (strcmp(__nouns.values[i].word, word) == 0) 
			return &__nouns.values[i];
	}
	return NULL;
}

void push_noun(Noun n) {
	DA_PUSH(&__nouns, Noun, n);
}

void __bind(void) {
	checkargc(2, "can bind only with 2 values");
	Value id = pop_value();
	checktype(&id, VALUE_TYPE_ID, "can bind only identifiers");
	Value v = pop_value();
	if (v.type == VALUE_TYPE_VERB) {
		Verb* verb = (Verb*)v.as.ref;
		strcpy(verb->word, id.as.identifier);
		push_verb(*verb);
	}
	else {
		push_noun(init_Noun(id.as.identifier, v));
	}
}

void __repeat(void) {
	checkargc(2, "can repeat verb for n times only");
	Value verb = pop_value();
	checktype(&verb, VALUE_TYPE_VERB, "can repeat only a verb");
	Value times = pop_value();
	checktype(&times, VALUE_TYPE_NUMBER, "times must be a number");
	for (int i = 0; i < (int)times.as.number; i++) {
		doverb((Verb*)verb.as.ref);
	}
}

void __ss(void) {
	Value v = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = __cur_stack->q,
		.bounded = false
	};
	push_value(v);
}

void __hash(void) {
	checkargc(1, "need value to get its size");
	Value* v = last_value();
	size_t size = 0;
	if (v->type == VALUE_TYPE_ARRAY) 
		size = ((da_Value*)v->as.ref)->q;
	Value hash = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = size,
		.bounded = false
	};
	push_value(hash);
}

void __map(void) {
	checkargc(2, "need verb and array to map");
	Value verb_val = pop_value();
	checktype(&verb_val, VALUE_TYPE_VERB, "need a verb to map");
	Value arr_val = pop_value();
	checktype(&arr_val, VALUE_TYPE_ARRAY, "need an array to map");
	da_Value* new_arr = malloc(sizeof(da_Value));
	DA_INIT(new_arr, Value);
	Value new_arr_val = {
		.type = VALUE_TYPE_ARRAY,
		.as.ref = new_arr,
		.bounded = false
	};
	push_value(new_arr_val);
	new_current(new_arr);
	da_Value* arr = (da_Value*)arr_val.as.ref;
	Verb* verb = (Verb*)verb_val.as.ref;
	for (size_t i = 0; i < arr->q; i++) {
		push_value(arr->values[i]);
		doverb(verb);
	}
	pop_current();
}


#endif