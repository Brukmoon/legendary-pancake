#include "vector.h"

bool vec2_equal(vec2 const* one, vec2 const* two)
{
	return (one->x == two->x && one->y == two->y);
}

void vec2_swap(vec2* one, vec2* two)
{
	vec2 temp = *one;
	*one = *two;
	*two = temp;
}