#include "animation.h"
#include "hash.h"
#include "common.h"
#include "config.h"

#define BUFFER_SIZE 256
// max animation name length
#define MAX_AN_LENGTH 30
// max command length
#define MAX_COMM_LENGTH 21

// initialize an animation_table
static void animation_table_init(struct animation_table* t);

void frame_add(struct animation* a, const char* sprite_name)
{
	struct frame* f = malloc(sizeof(struct frame));
	if (!f)
	{
		ERROR("Not enough memory!");
		return;
	}
	size_t s = SDL_strlen(sprite_name) + 1;
	f->sprite_name = malloc(s);
	if (!f->sprite_name)
	{
		ERROR("Not enough memory!");
		return;
	}
	f->next = NULL;
	SDL_strlcpy(f->sprite_name, sprite_name, s);
	// add frame to the linked list
	if (!a->tail)
	{
		a->tail = f;
		a->curr = f;
	}
	else
		a->head->next = f;
	a->head = f;
}

void frame_destroy(struct frame* f)
{
	if (!f)
		return;
	INFO("Destroying frame %s.", f->sprite_name);
	free(f->sprite_name);
	free(f);
}

static void animation_table_init(struct animation_table* t)
{
	// nullify the table
	for (int i = 0; i < ANIMATION_ARR_SIZE; ++i)
	{
		t->a[i] = NULL;
		t->curr = NULL;
	}
}

void animation_table_load(const char* name, struct animation_table* t, SDL_Renderer* renderer)
{
	animation_table_init(t);
	// 4 is reserved space for file extension
	size_t s = SDL_strlen(name) + SDL_strlen(IMG_PATH) + 4;
	char* file_name = malloc(s);
	if (!file_name)
	{
		ERROR("Not enough memory!");
		return;
	}
	SDL_strlcpy(file_name, IMG_PATH, s);
	SDL_strlcat(file_name, name, s);
	SDL_strlcat(file_name, ".an", s);
	FILE* f = NULL;
	f = fopen(file_name, "r");
	if (f != NULL)
	{
		char buffer[BUFFER_SIZE], command[MAX_COMM_LENGTH], texture_name[MAX_AN_LENGTH];
		struct animation* a = NULL;
		// until you get to the end of file
		while (!feof(f)) 
		{
			fgets(buffer, BUFFER_SIZE, f);
			// Parse first string and ignore everything else except a newline.
			sscanf(buffer, "%s%*[^\n]", command);
			if (SDL_strcmp(command, "ANIMATION") == 0)
			{
				char animation_name[MAX_AN_LENGTH];
				unsigned delay = 0;
				sscanf(buffer, "%*s%s%d", animation_name, &delay);
				animation_create(&a, animation_name, &delay);
				INFO("< Loading animation %s", animation_name);
			}
			else if (SDL_strcmp(command, "ANIMATION_END") == 0)
			{
				animation_table_add(t, a);
				a = NULL;
				INFO("> Animation loaded.");
			}
			else if (SDL_strcmp(command, "FRAME") == 0)
			{
				SDL_Rect r = { 0, 0, 0, 0 };
				char sprite_name[MAX_AN_LENGTH];
				sscanf(buffer, "%*s%s%d%d%d%d", sprite_name, &r.x, &r.y, &r.w, &r.h);
				// INFO("Loading frame %s [%d;%d]", sprite_name, r.x, r.y);
				sprite_add(sprite_name, texture_name, r);
				frame_add(a, sprite_name);
			}
			else if (SDL_strcmp(command, "TEXTURE") == 0)
			{
				sscanf(buffer, "%*s%s", texture_name);
				texture_add(texture_name, renderer);
			}
		}
	}
	else
	{
		ERROR("Couldn't open animation file %s.", file_name);
	}
	free(file_name);
	fclose(f);
}

void animation_create(struct animation** a, char const* name,  unsigned const* delay)
{
	(*a) = malloc(sizeof(struct animation));
	if (!(*a))
	{
		ERROR("Not enough memory!");
		return;
	}
	(*a)->curr = (*a)->head = (*a)->tail = NULL;
	(*a)->next = NULL;
	(*a)->delay = *delay;
	(*a)->delay_counter = 0;
	(*a)->name = malloc(MAX_AN_LENGTH);
	if (!(*a)->name)
	{
		ERROR("Not enough memory!");
		return;
	}
	SDL_strlcpy((*a)->name, name, MAX_AN_LENGTH);
}

void animation_destroy(struct animation* a)
{
	if (!a)
		return;
	struct frame* iter = a->tail;
	while (iter)
	{
		iter = iter->next;
		frame_destroy(a->tail);
		a->tail = iter;
	}
	free(a->name);
	free(a);
}

void animation_table_add(struct animation_table* t, struct animation* a)
{
	// find where to put it
	size_t index = hash_s(a->name) % ANIMATION_ARR_SIZE;
	// get to the end of the linked list at index
	struct animation* iter = t->a[index], *prev = NULL;
	while (iter)
	{
		prev = iter;
		iter = iter->next;
	}
	// push it
	if (!prev) // ll empty
		t->a[index] = a;
	else
		prev->next = a;
	INFO("Animation %s loaded to index %d.", a->name, index);
}

void animation_set(const char *name, struct animation_table* t)
{
	size_t index = hash_s(name) % ANIMATION_ARR_SIZE;
	struct animation* iter = t->a[index];
	while (iter)
	{
		if (SDL_strcmp(name, iter->name) == 0)
		{
			t->curr = iter;
			return;
		}
		iter = iter->next;
	}
	ERROR("Animation %s not found.", name);
}

void animation_next(struct animation_table* t)
{
	t->curr->curr = t->curr->curr->next;
	if (!t->curr->curr) 
		// start over again
		t->curr->curr = t->curr->tail;
}

void animation_table_destroy(struct animation_table* t)
{
	for (size_t i = 0; i < ANIMATION_ARR_SIZE; ++i)
	{
		struct animation* iter = t->a[i];
		while (iter)
		{
			t->a[i] = iter->next;
			animation_destroy(iter);
			iter = t->a[i];
		}
		t->a[i] = NULL;
	}
}
