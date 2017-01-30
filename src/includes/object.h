#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <SDL.h>

#include "animation.h"
#include "vector.h"

struct object
{
	struct object* next;

	char* name;
	vec2 spawn;
	struct animation_table t;
};

static struct object *g_object_head, *g_object_root;

void object_add(const char* name, vec2 spawn, SDL_Renderer* renderer);
void object_destroy(void);
// animation
void object_update(void);
void object_draw(SDL_Renderer* renderer);


#endif // OBJECT_H