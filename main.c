#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "verb.h"
#include "value.h"
#include "internals.h"
#include "builtin-verbs.h"

void eval(const char* src, size_t beg, size_t end) {
	if (end - beg < 1) return;
	if (src[beg] == '[') {
		da_Value* arr = malloc(sizeof(da_Value));
		DA_INIT(arr, Value);
		Value v = {
			.type = VALUE_TYPE_ARRAY,
			.as.ref = arr,
			.bounded = false
		};
		push_value(v);
		new_current(arr);
		eval(src, beg+1, end);
		return;
	}
	if (src[end-2] == ']') {
		eval(src, beg, end-1);
		if (__cur_stack == &__stack) {
			printf("ERROR: unmatched bracket.\n");
			exit(1);
		}
		pop_current();
		return;
	}
	if (src[beg] == '{') {
		Verb* verb = malloc(sizeof(Verb));
		*verb = init_Verb("anonymous");
		Value v = { 
			.type = VALUE_TYPE_VERB, 
			.as.ref = verb,
			.bounded = false 
		};
		push_value(v);
		new_current(&verb->todo);
		__record_verb = true;
		eval(src, beg+1, end);
		return;
	}
	if (src[end-2] == '}') {
		eval(src, beg, end-1);
		if (__cur_stack == &__stack) {
			printf("ERROR: unmatched curly bracket.\n");
			exit(1);
		}
		__record_verb = false;
		pop_current();
		return;
	}
	char* word = malloc(end - beg);
	strncpy(word, &src[beg], end - beg);
	word[end - beg - 1] = 0;
	// printf("evaluating: \"%s\"\n", word);
	double number;
	Verb* verb;
	Noun* noun;
	if ((number = atof(word))) {
		Value v = {
			.type = VALUE_TYPE_NUMBER,
			.as.number = number
		};
		push_value(v);
	}
	else if ((verb = find_verb(word))) {
		// printf("verb found: \"%s\"\n", verb->word);
		if (__record_verb) {
			Value v = {
				.type = VALUE_TYPE_VERB,
				.as.ref = verb,
				.bounded = false
			};
			push_value(v);
		}
		else doverb(verb);
	}
	else if ((noun = find_noun(word))) {
		push_value(noun->value);
	}
	else {
		Value v = {
			.type = VALUE_TYPE_ID
		};
		strcpy(v.as.identifier, word);
		push_value(v);
	}
	free(word);
}

void push_builtin_verb(Proc proc, char word[VERB_LEN]) {
	Verb verb;
	DA_INIT(&verb.todo, Value);
	Value v = { .type = VALUE_TYPE_PROC, .as.proc = proc };
	DA_PUSH(&verb.todo, Value, v);
	strcpy(verb.word, word);
	DA_PUSH(&__verbs, Verb, verb);
}

void init() {
	DA_INIT(&__stack, Value);
	DA_INIT(&__stack_to_push, da_Value*);
	DA_PUSH(&__stack_to_push, da_Value*, &__stack);
	DA_INIT(&__verbs, Verb);
	DA_INIT(&__nouns, Noun);
	push_builtin_verb(__quit, "quit");
	push_builtin_verb(trace, "trace");
	push_builtin_verb(__add, "+");
	push_builtin_verb(__mul, "*");
	push_builtin_verb(__sub, "-");
	push_builtin_verb(__div, "/");
	push_builtin_verb(__cp, "cp");
	push_builtin_verb(__cp2, "cp2");
	push_builtin_verb(__flip, "flip");
	push_builtin_verb(__pop, "pop");
	push_builtin_verb(__bind, "bind");
}

void deinit() {
	DA_DEINIT(&__stack);
	for (size_t i = 0; i < __verbs.q; i++) {
		deinit_Verb(__verbs.values[i]);
	}
	DA_DEINIT(&__verbs);
	for (size_t i = 0; i < __nouns.q; i++) {
		deinit_Noun(__nouns.values[i]);
	}
	DA_DEINIT(&__nouns);
}

int main() {
	init();
  char src[256];
	__state = S_READ;
	__record_verb = false;
	while (1) {
		memset(src, 0, 256);
		printf("\n> ");
		fgets(src, 256, stdin);
		// printf("entered: \"%s\"\n", src);
		size_t beg = 0, end = 0;
		for (size_t ip = 0; ip <= strlen(src); ip++) {
			if (__state == S_READ) {
				end++;
				if (isspace(src[ip])) __state = S_EVAL;
			}
			else {
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
