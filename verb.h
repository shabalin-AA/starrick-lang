
typedef void (*builtin_fn)(void);

typedef struct {
	size_t capacity;
	size_t q;
	builtin_fn* values;
} da_builtin_fn;

#define VERB_LEN 128

typedef struct {
	char word[VERB_LEN];
	da_builtin_fn fns;
} Verb;

void doverb(const Verb* verb) {
	for (size_t i = 0; i < verb->fns.q; i++) {
		(verb->fns.values[i])();
	}
}
