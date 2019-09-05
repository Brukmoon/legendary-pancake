#include <stdlib.h>
#include <string.h>
#include "stack.h"

static bool stack_grow(struct stack *s);

bool stack_new(struct stack *s, int element_size)
{
	if (element_size < 0)
		return false;
	s->element_size = element_size;
	s->alloc_length = INITIAL_ALLOC_LENGTH;
	s->elements = malloc(INITIAL_ALLOC_LENGTH*element_size);
	if (!s->elements)
		return false;
	return true;
}

void stack_free(struct stack *s)
{
	free(s->elements);
}

bool stack_empty(const struct stack *s)
{
	return s->length == 0;
}

void stack_push(struct stack *s, void *element)
{
	void *target = NULL;
	if (s->length == s->alloc_length)
		stack_grow(s);
	target = (char*)s->elements + s->length*s->element_size;
	memcpy(target, element, s->element_size);
	++s->length;
}

void stack_pop(struct stack *s, void *element)
{
	if (stack_empty(s))
		return;
	s->free_element(element);
	s->length--;
}

bool stack_grow(struct stack *s)
{
	s->alloc_length *= 2;
	s->elements = realloc(s->elements, s->alloc_length * s->element_size);
	if (!s->elements)
		return false;
	return true;
}
