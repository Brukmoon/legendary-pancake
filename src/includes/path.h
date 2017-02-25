#ifndef PATH_H
#define PATH_H

#include "vector.h"

// path ll
struct position
{
	struct position* next;
	struct position* prev;
	vec2 pos;
};

void path_find(vec2 start, vec2 goal, struct position** path);
// mem cleanup
void path_destroy(struct position** path);

#endif // PATH_H