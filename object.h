#ifndef OBJECT_H
#define OBJECT_H

#include <SDL.h>

struct object_state
{
// Must be at least LEVEL_RESOURCE_LENGTH (level.c).
#define STATE_MAX_LENGTH 30
	struct object_state *next;
	char name[STATE_MAX_LENGTH];
};

static struct object
{
	struct object *next;
	SDL_Rect skeleton;
	struct object_state *root, *curr;
} *objects;

void object_add(const char *name, const SDL_Rect skeleton);


#endif // OBJECT_H