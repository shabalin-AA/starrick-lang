#ifndef _VERB_H_
#define _VERB_H_


#include <stdio.h>
#include "da.h"
#include "value.h"
#include "internals.h"

#define VERB_LEN 16

typedef struct {
	char word[VERB_LEN];
	da_Value todo;
} Verb;

Verb init_Verb(char word[VERB_LEN]) {
	Verb verb;
	DA_INIT(&verb.todo, Value);
	strcpy(verb.word, word);
	return verb;
}

void deinit_Verb(Verb v) {
	DA_DEINIT(&v.todo);
}

void doverb(const Verb* verb) {
	for (size_t i = 0; i < verb->todo.q; i++) {
		Value* v = &verb->todo.values[i];
		if (v->type == VALUE_TYPE_PROC) (v->as.proc)();
		else if (v->type == VALUE_TYPE_VERB) {
			doverb((Verb*)v->as.ref);
		}
		else push_value(*v);
	}
}

void println_Verb(Verb verb) {
	printf("<verb> %s\n", verb.word);
}

typedef struct {
	size_t capacity;
	size_t q;
	Verb* values;
} da_Verb;


#endif