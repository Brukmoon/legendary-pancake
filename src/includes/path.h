#ifndef PATH_H
#define PATH_H

#include "vector.h"

// path ll
struct waypoint
{
	struct waypoint* next;
	struct waypoint* prev;
	vec2 pos;
};

void path_find(vec2 start, vec2 goal, struct waypoint** path);
// mem cleanup
void path_destroy(struct waypoint** path);

#endif // PATH_H