#ifndef PATH_H
#define PATH_H

#include "vector.h"

struct position
{
	struct position* next;
	vec2 pos;
};

void path_find(vec2 start, vec2 goal, struct position** path);
void path_destroy(struct position** path);

#endif // PATH_H