#ifndef STACK_H
#define STACK_H
#define INITIAL_ALLOC_LENGTH 3

#include <stdbool.h>

struct stack
{
	void *elements;
	int element_size;
	int length;
	int alloc_length;
	void(*free_element)(void*ptr);
};

bool stack_new(struct stack *s, int element_size);
void stack_free(struct stack *s);
bool stack_empty(const struct stack *s);
void stack_push(struct stack *s, void *element);
void stack_pop(struct stack *s, void *element);

#endif // STACK_H