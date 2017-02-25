#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

typedef int coord;
typedef struct
{
	coord x, y;
} vec2;

bool vec2_equal(vec2 const* one, vec2 const* two);
void vec2_swap(vec2* one, vec2* two);

typedef struct
{
	float x, y;
} vec2f;

typedef struct
{
	coord x, y, z;
} vec3;

#endif // VECTOR_H