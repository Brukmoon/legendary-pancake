#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <SDL.h>

#include "animation.h"
#include "vector.h"

struct actor;

struct object
{
	struct object* next;

	char* name;
	SDL_Rect skeleton;

	bool is_active;
	struct animation_table t;
};

static struct object *g_object_head, *g_object_root;
void object_add(const char* name, SDL_Rect spawn, SDL_Renderer* renderer);
void object_destroy(void);

// animation
void object_update_all(SDL_Renderer* renderer);
void object_draw(SDL_Renderer* renderer);
// write objects in-game to the level file
void object_write_to_file(FILE* f);

struct missile
{
	struct missile* next, *prev;

	int degrees;
	SDL_Rect skeleton;
	float velocity;
	struct animation_table t;
};

static struct missile *g_missile_head, *g_missile_root;

void missile_fire(struct player *source, const float* velocity, SDL_Renderer *renderer);
void missile_draw(SDL_Renderer* renderer);
static void missile_destroy(void);
static void missile_remove(struct missile* m);
static void missile_update(SDL_Renderer* renderer);

#endif // OBJECT_H