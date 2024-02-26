#ifndef _VALUE_H_
#define _VALUE_H_


#include <stdio.h>
#include <stdbool.h>
#include "da.h"

typedef enum {
  VALUE_TYPE_NUMBER,
	VALUE_TYPE_ARRAY,
	VALUE_TYPE_PROC,
	VALUE_TYPE_VERB,
	VALUE_TYPE_VERB_REF,
	VALUE_TYPE_ID,
	VALUE_TYPE_NOUN
} VALUE_TYPE;

typedef void (*Proc)(void);

#define ID_LEN 16

typedef struct {
  VALUE_TYPE type;
  union {
    double number;
		Proc proc;
		char identifier[ID_LEN];
		void* ref;
  } as;
	bool bounded;
} Value;

typedef struct {
  size_t capacity;
  size_t q;
  Value* values;
} da_Value;

#define NOUN_LEN 16

typedef struct {
	char word[NOUN_LEN];
	Value value;
} Noun;

#define VERB_LEN 16

typedef struct {
	char word[VERB_LEN];
	da_Value todo;
} Verb;

void println_Value(Value v) {
	if (v.type == VALUE_TYPE_NUMBER) 
		printf("<number> %lf\n", v.as.number);
	else if (v.type == VALUE_TYPE_ARRAY) {
		puts("<array>");
		puts("[");
		da_Value* arr = (da_Value*)v.as.ref;
		for (size_t i = 0; i < arr->q; i++) {
			println_Value(arr->values[i]);
		}
		puts("]");
	}
	else if (v.type == VALUE_TYPE_ID) {
		printf("<identifier> %s\n", v.as.identifier);
	}
	else if (v.type == VALUE_TYPE_PROC) {
		printf("<proc>\n");
	}
	else if (v.type == VALUE_TYPE_VERB) {
		printf("<verb> %s\n", ((Verb*)v.as.ref)->word);
	}
	else if (v.type == VALUE_TYPE_NOUN) {
		printf("<noun> %s\n", ((Noun*)v.as.ref)->word);
	}
}

void deinit_Value(Value v) {
	if (v.bounded) return;
	if (v.type == VALUE_TYPE_ARRAY) {
		da_Value* arr = (da_Value*)v.as.ref;
		for (size_t i = 0; i < arr->q; i++) {
			deinit_Value(arr->values[i]);
		}
		DA_DEINIT(arr);
	}
}

Value copy_value(Value* a) {
	Value b = { .type = a->type, .as = a->as, .bounded = a->bounded };
	if (a->type == VALUE_TYPE_ARRAY) {
		da_Value* arr = (da_Value*)a->as.ref;
		da_Value* new_arr = malloc(sizeof(da_Value));
		DA_INIT(new_arr, Value);
		for (size_t i = 0; i < arr->q; i++) {
			DA_PUSH(new_arr, copy_value(&arr->values[i]));
		}
		b.as.ref = new_arr;
	}
	return b;
}

typedef struct {
	size_t capacity;
	size_t q;
	Noun* values;
} da_Noun;

void println_Noun(Noun noun) {
	printf("<noun> %s\n", noun.word);
	println_Value(noun.value);
}

Noun init_Noun(char word[NOUN_LEN], Value v) {
	Noun n;
	strcpy(n.word, word);
	n.value = v;
	n.value.bounded = true;
	return n;
}

void deinit_Noun(Noun noun) {
	deinit_Value(noun.value);
}


#endif