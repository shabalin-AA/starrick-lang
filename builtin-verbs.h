#ifndef _BUILTIN_VERBS_H_
#define _BUILTIN_VERBS_H_


#include "internals.h"

da_Verb __verbs = {0};

Verb* find_verb(const char word[VERB_LEN]) {
	for (int i = __verbs.q-1; i >= 0; i--) {
		if (strcmp(__verbs.values[i].word, word) == 0) 
			return &__verbs.values[i];
	}
	return NULL;
}

void push_verb(Verb v) { DA_PUSH(&__verbs, v); }

void __quit(void) { exit(0); }

void __print(void) {
	checkargc(1, "need value to print");
	println_Value(__cur_stack->values[__cur_stack->q-1]);
}

void __add(void) {
	checkargc(2, "can add only 2 numbers");
	Value a = pop_value();
	Value b = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cannot add not a number");
	checktype(&b, VALUE_TYPE_NUMBER, "cannot add not a number");
	a.as.number += b.as.number;
	push_value(a);
	deinit_Value(b);
}

void __mul(void) {
	checkargc(2, "can multiply only 2 numbers");
	Value a = pop_value();
	Value b = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cannot multiply not a number");
	checktype(&b, VALUE_TYPE_NUMBER, "cannot multiply not a number");
	a.as.number *= b.as.number;
	push_value(a);
	deinit_Value(b);
}

void __sub(void) {
	checkargc(2, "can subtract only 2 numbers");
	Value b = pop_value();
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cannot subtract not a number");
	checktype(&b, VALUE_TYPE_NUMBER, "cannot subtract not a number");
	a.as.number -= b.as.number;
	push_value(a);
	deinit_Value(b);
}

void __div(void) {
	checkargc(2, "can divide only 2 numbers");
	Value b = pop_value();
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cannot divide not a number");
	checktype(&b, VALUE_TYPE_NUMBER, "cannot divide not a number");
	a.as.number /= b.as.number;
	push_value(a);
	deinit_Value(b);
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

da_Noun __nouns = {0};

Noun* find_noun(const char word[NOUN_LEN]) {
	for (int i = __nouns.q-1; i >= 0; i--) {
		if (strcmp(__nouns.values[i].word, word) == 0) 
			return &__nouns.values[i];
	}
	return NULL;
}

void push_noun(Noun n) {
	DA_PUSH(&__nouns, n);
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
	deinit_Value(id);
	deinit_Value(v);
}

void __repeat(void) {
	checkargc(2, "can repeat verb for n times only");
	Value verb = pop_value();
	checktype(&verb, VALUE_TYPE_VERB_REF, "can repeat only a verb");
	Value times = pop_value();
	checktype(&times, VALUE_TYPE_NUMBER, "times must be a number");
	for (int i = 0; i < (int)times.as.number; i++) {
		doverb((Verb*)verb.as.ref);
	}
	deinit_Value(verb);
	deinit_Value(times);
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
	checktype(&verb_val, VALUE_TYPE_VERB_REF, "need a verb to map");
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
	deinit_Value(verb_val);
	deinit_Value(arr_val);
}

void __gr(void) {
	checkargc(2, "need 2 arguments for comparison");
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value b = pop_value();
	checktype(&b, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value res = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = a.as.number > b.as.number,
		.bounded = false
	};
	push_value(res);
	deinit_Value(a);
	deinit_Value(b);
}

void __ls(void) {
	checkargc(2, "need 2 arguments for comparison");
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value b = pop_value();
	checktype(&b, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value res = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = a.as.number < b.as.number,
		.bounded = false
	};
	push_value(res);
	deinit_Value(a);
	deinit_Value(b);
}

void __eq(void) {
	checkargc(2, "need 2 arguments for comparison");
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value b = pop_value();
	checktype(&b, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value res = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = a.as.number == b.as.number,
		.bounded = false
	};
	push_value(res);
	deinit_Value(a);
	deinit_Value(b);
}

void __greq(void) {
	checkargc(2, "need 2 arguments for comparison");
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value b = pop_value();
	checktype(&b, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value res = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = a.as.number >= b.as.number,
		.bounded = false
	};
	push_value(res);
	deinit_Value(a);
	deinit_Value(b);
}

void __lseq(void) {
	checkargc(2, "need 2 arguments for comparison");
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value b = pop_value();
	checktype(&b, VALUE_TYPE_NUMBER, "cant compare not numbers");
	Value res = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = a.as.number <= b.as.number,
		.bounded = false
	};
	push_value(res);
	deinit_Value(a);
	deinit_Value(b);
}

void __and(void) {
	checkargc(2, "need 2 arguments for and");
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cant calc logic for not numbers");
	Value b = pop_value();
	checktype(&b, VALUE_TYPE_NUMBER, "cant calc logic for not numbers");
	Value res = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = a.as.number && b.as.number,
		.bounded = false
	};
	push_value(res);
	deinit_Value(a);
	deinit_Value(b);
}

void __or(void) {
	checkargc(2, "need 2 arguments for or");
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cant calc logic for not numbers");
	Value b = pop_value();
	checktype(&b, VALUE_TYPE_NUMBER, "cant calc logic for not numbers");
	Value res = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = a.as.number || b.as.number,
		.bounded = false
	};
	push_value(res);
	deinit_Value(a);
	deinit_Value(b);
}

void __not(void) {
	checkargc(1, "need a value for not");
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "cant calc logic for not numbers");
	Value res = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = (1 - a.as.number),
		.bounded = false
	};
	push_value(res);
	deinit_Value(a);
}

void __join(void) {
	checkargc(2, "can join only 2 values");
	Value b = pop_value();
	Value a = pop_value();
	if (a.type == VALUE_TYPE_NUMBER) {
		da_Value* new_arr = malloc(sizeof(da_Value));
		DA_INIT(new_arr, Value);
		Value new_arr_val = {
			.type = VALUE_TYPE_ARRAY,
			.as.ref = new_arr,
			.bounded = false
		};
		DA_PUSH(new_arr, a);
		if (b.type == VALUE_TYPE_NUMBER) {
			DA_PUSH(new_arr, b);			
		}
		else if (b.type == VALUE_TYPE_ARRAY) {
			da_Value* arr = (da_Value*)b.as.ref;
			for (size_t i = 0; i < arr->q; i++) {
				DA_PUSH(new_arr, arr->values[i]);
			}
			free(arr->values);
		}
		else checktype(&b, VALUE_TYPE_ARRAY, "can join only numbers or arrays");
		push_value(new_arr_val);
	}
	else if (a.type == VALUE_TYPE_ARRAY) {
		da_Value* new_arr = (da_Value*)a.as.ref;
		if (b.type == VALUE_TYPE_NUMBER) {
			DA_PUSH(new_arr, b);			
		}
		else if (b.type == VALUE_TYPE_ARRAY) {
			da_Value* arr = (da_Value*)b.as.ref;
			for (size_t i = 0; i < arr->q; i++) {
				DA_PUSH(new_arr, arr->values[i]);
			}
			free(arr->values);
		}
		push_value(a);
	}
	else checktype(&a, VALUE_TYPE_ARRAY, "can join only numbers or arrays");
}

void __reverse(void) {
	checkargc(1, "need value to reverse");
	Value* arr_val = last_value();
	checktype(arr_val, VALUE_TYPE_ARRAY, "can reverse only array");
	da_Value* arr = (da_Value*)arr_val->as.ref;
	Value temp;
	for (size_t i = 0; i < arr->q/2; i++) {
		temp = arr->values[i];
		arr->values[i] = arr->values[arr->q-1 - i];
		arr->values[arr->q-1 - i] = temp;
	}
}

void __range(void) {
	checkargc(1, "need a value to range");
	Value lim = pop_value();
	checktype(&lim, VALUE_TYPE_NUMBER, "can range only to number");
	da_Value* range = malloc(sizeof(da_Value));
	Value range_val = {
		.type = VALUE_TYPE_ARRAY,
		.as.ref = range,
		.bounded = false
	};
	for (int i = 0; i < (int)lim.as.number; i++) {
		Value new_val = {
			.type = VALUE_TYPE_NUMBER,
			.as.number = (double)i,
			.bounded = false
		};
		DA_PUSH(range, new_val);
	}
	push_value(range_val);
	deinit_Value(lim);
}

void __take(void) {
	checkargc(2, "need array and quantity to take");
	Value q = pop_value();
	checktype(&q, VALUE_TYPE_NUMBER, "quantity must be a number");
	Value* arr_val = last_value();
	checktype(arr_val, VALUE_TYPE_ARRAY, "need array to take from");
	da_Value* arr = (da_Value*)arr_val->as.ref;
	new_current(arr);
	size_t to_pop = arr->q - (size_t)q.as.number;
	for (size_t i = 0; i < to_pop; i++) __pop();
	pop_current();
	deinit_Value(q);
}

void __reduce(void) {
	checkargc(2, "need array and verb to reduce");
	Value verb_val = pop_value();
	checktype(&verb_val, VALUE_TYPE_VERB_REF, "need verb to reduce");
	Verb* verb = (Verb*)verb_val.as.ref;
	Value arr_val = pop_value();
	checktype(&arr_val, VALUE_TYPE_ARRAY, "need array to reduce");
	da_Value* arr = (da_Value*)arr_val.as.ref;
	new_current(arr);
	while (arr->q > 1) doverb(verb);
	pop_current();
	push_value(arr->values[0]);
	deinit_Value(verb_val);
	deinit_Value(arr_val);
}


void __filter(void) {
	checkargc(2, "[filter] need verb and array");
	Value verb_val = pop_value();
	checktype(&verb_val, VALUE_TYPE_VERB_REF, "[filter] need a verb");
	Value arr_val = pop_value();
	checktype(&arr_val, VALUE_TYPE_ARRAY, "[filter] need an array");
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
    Value pred_res = pop_value();
    checktype(&pred_res, VALUE_TYPE_NUMBER, "[filter] verb must return 0 or 1");
    if (pred_res.as.number) {
      push_value(arr->values[i]);
    }
	}
	pop_current();
	deinit_Value(verb_val);
	deinit_Value(arr_val);
}


void __rnd(void) {
  Value res = {
    .type = VALUE_TYPE_NUMBER,
    .as.number = rand(),
    .bounded = false
  };
  push_value(res);
}


void __mod(void) {
  checkargc(2, "[%] need 2 numbers for mod");
	Value a = pop_value();
	checktype(&a, VALUE_TYPE_NUMBER, "[%] can't mod not a number");
	Value b = pop_value();
	checktype(&b, VALUE_TYPE_NUMBER, "[%] can't mod not a number");
	Value res = {
		.type = VALUE_TYPE_NUMBER,
		.as.number = (long long)a.as.number % (long long)b.as.number,
		.bounded = false
	};
	push_value(res);
	deinit_Value(a);
	deinit_Value(b);
}



#endif