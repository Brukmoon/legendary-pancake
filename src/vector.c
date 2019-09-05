#include "vector.h"

#include <math.h>

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

bool vec2_similar(vec2 const* one, vec2 const* two, const coord margin)
{
	return (abs(one->x - two->x) <= margin) && (abs(one->y - two->y) <= margin);
}

vec2 vec2_scale(vec2 const* vec, unsigned scale)
{
	return (vec2) { vec->x*scale, vec->y*scale };
}
