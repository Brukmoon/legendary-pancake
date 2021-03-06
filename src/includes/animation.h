#ifndef ANIMATION_H
#define ANIMATION_H

#include "sprite.h"

// Average number of animations per table
#define ANIMATION_ARR_SIZE 5

// Animation frame.
struct frame
{
	char *sprite_name;
	struct frame* next;
};

struct animation
{
	// next in htable
	struct animation* next;

	int delay_counter, delay;
	// name of the animation
	char *name;
	struct frame* head, *tail, *curr;
};

// List of animations for one entity.
struct animation_table
{
	// htable
	struct animation* a[ANIMATION_ARR_SIZE];
	// currently active animation
	struct animation* curr;
};

// add a frame to an animation. sprite must already exist in the sprite manager (call sprite_add)
static void frame_add(struct animation* a, const char* sprite_name);
static void frame_destroy(struct frame* f);

static void animation_create(struct animation** a, char const* name, unsigned const* delay);
static void animation_destroy(struct animation* a);

// add animation to table
void animation_table_add(struct animation_table* t, struct animation* a);
// set active animation
void animation_set(const char *name, struct animation_table* t);
void animation_next(struct animation_table* t);

// loads animation table from file (.an)
void animation_table_load(const char* name, struct animation_table* t, SDL_Renderer* renderer);
void animation_table_destroy(struct animation_table* t);

#endif // ANIMATION_H