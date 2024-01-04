#include <stdio.h>

typedef enum {
  VALUE_TYPE_NUMBER,
  VALUE_TYPE_STRING
} VALUE_TYPE;

typedef struct {
  VALUE_TYPE type;
  union {
    double number;
  } as;
} Value;

void println_Value(Value v) {
	if (v.type == VALUE_TYPE_NUMBER) printf("%g\n", v.as.number);
}
