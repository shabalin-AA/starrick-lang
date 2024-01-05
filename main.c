#include <ctype.h>
#include <string.h>
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
			.as.ref = arr
		};
		push_value(v);
		DA_PUSH(&__stack_to_push, da_Value*, arr);
		eval(src, beg+1, end);
		return;
	}
	if (src[end-2] == ']') {
		eval(src, beg, end-1);
		if (__cur_stack == &__stack) {
			printf("ERROR: unmatched bracket.\n");
			exit(1);
		}
		DA_POP(&__stack_to_push, da_Value*);
		return;
	}
	char* word = malloc(end - beg);
	strncpy(word, &src[beg], end - beg);
	word[end - beg - 1] = 0;
	// printf("evaluating: \"%s\"\n", word);
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

void push_builtin_verb(builtin_fn fn, char word[VERB_LEN]) {
	Verb verb;
	DA_INIT(&verb.fns, builtin_fn);
	DA_PUSH(&verb.fns, builtin_fn, fn);
	strcpy(verb.word, word);
	DA_PUSH(&__verbs, Verb, verb);
}

void init() {
	DA_INIT(&__stack, Value);
	DA_INIT(&__stack_to_push, da_Value*);
	DA_PUSH(&__stack_to_push, da_Value*, &__stack);
	DA_INIT(&__verbs, Verb);
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
		__state = S_READ;
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
