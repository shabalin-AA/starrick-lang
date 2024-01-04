#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "da.h"
#include "value.h"
#include "verb.h"

typedef struct {
  size_t capacity;
  size_t q;
  Value* values;
} da_Value;

da_Value __stack;

typedef struct {
	size_t capacity;
	size_t q;
	Verb* values;
} da_Verb;

da_Verb __verbs;

Verb* find_verb(const char word[VERB_LEN]) {
	for (int i = __verbs.q-1; i >= 0; i--) {
		if (strcmp(__verbs.values[i].word, word) == 0) 
			return &__verbs.values[i];
	}
	return NULL;
}

void push_value(Value v) {
  DA_PUSH(&__stack, Value, v);
}

void trace() {
	printf("---- trace ----\n");
	for (size_t i = 0; i < __stack.q; i++) {
		println_Value(__stack.values[i]);
	}
}

void checkargc(int argc, const char* msg) {
	if (__stack.q < (size_t)argc) {
		printf("ERROR: too few arguments. %s\n", msg);
		trace();
		exit(1);
	}
}

Value pop_value() {
	checkargc(1, "cant pop from empty stack");
  Value v = __stack.values[__stack.q-1];
  DA_POP(&__stack, Value);
  return v;
}

Value last_value() {
	checkargc(1, "cant get last value from empty stack");
  Value v = __stack.values[__stack.q-1];
  return v;
}

void checktype(Value* v, VALUE_TYPE required_type, const char* msg) {
	if (v->type != required_type) {
		printf("ERROR: %s\n", msg);
		trace();
		exit(1);
	}
}

void __quit(void) {
	exit(0);
}

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
	Value a = last_value();
	push_value(a);
}

void __pop(void) {
	pop_value();
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
	Value b = last_value();
	push_value(a);
	push_value(b);
}

typedef enum {
	S_READ,
	S_EVAL
} STATE;

void eval(const char* src, size_t beg, size_t end) {
	if (end - beg < 1) return;
	char* word = malloc(end - beg);
	strncpy(word, &src[beg], end - beg);
	word[end - beg - 1] = 0;
	// printf("word found: \"%s\"\n", word);
	double number;
	Verb* verb;
	if ((number = atof(word))) {
		Value v = {
			.type = VALUE_TYPE_NUMBER,
			.as.number = number
		};
		push_value(v);
	}
	else if ((verb = find_verb(word))) {
		// printf("verb found: \"%s\"\n", verb->word);
		doverb(verb);
	}
	free(word);
}

void push_verb(Verb v) {
	DA_PUSH(&__verbs, Verb, v);
}

void push_builtin_verb(builtin_fn fn, char word[VERB_LEN]) {
	Verb verb;
	DA_INIT(&verb.fns, builtin_fn);
	DA_PUSH(&verb.fns, builtin_fn, fn);
	strcpy(verb.word, word);
	DA_PUSH(&__verbs, Verb, verb);
}

void init() {
	DA_INIT(&__stack, Value);
	DA_INIT(&__verbs, Verb);
	push_builtin_verb(__quit, "quit");
	push_builtin_verb(__add, "+");
	push_builtin_verb(__mul, "*");
	push_builtin_verb(__sub, "-");
	push_builtin_verb(__div, "/");
	push_builtin_verb(__cp, "cp");
	push_builtin_verb(__cp2, "cp2");
	push_builtin_verb(__flip, "flip");
	push_builtin_verb(__pop, "pop");
}

void deinit() {
	DA_DEINIT(&__stack);
	for (size_t i = 0; i < __verbs.q; i++) {
		DA_DEINIT(&__verbs.values[i].fns);
	}
	DA_DEINIT(&__verbs);
}

int main() {
	init();
  char src[256];
	while (1) {
		memset(src, 0, 256);
		printf("\n> ");
		fgets(src, 256, stdin);
		// printf("entered: \"%s\"\n", src);
		STATE __state = S_READ;
		size_t beg = 0, end = 0;
		for (size_t ip = 0; ip <= strlen(src); ip++) {
			if (__state == S_READ) {
				end++;
				if (isspace(src[ip])) __state = S_EVAL;
			}
			if (__state == S_EVAL) {
				eval(src, beg, end);
				beg = end;
				if (!isspace(src[ip])) {
					__state = S_READ;
					beg = ip;
					end = ip + 1;
				}
			}
		}
		trace();
	}
	deinit();
  return 0;
}
