#include <stdio.h>

#include "animation.h"
#include "hash.h"
#include "common.h"
#include "config.h"

#define BUFFER_SIZE 256
#define MAX_AN_LENGTH 30

void frame_add(struct animation* a, const char* sprite)
{
	struct frame* f = malloc(sizeof(struct frame));
	if (!f)
	{
		ERROR("Not enough memory!");
		return;
	}
	size_t s = SDL_strlen(sprite) + 1;
	f->sprite_name = malloc(s);
	if (!f->sprite_name)
	{
		ERROR("Not enough memory!");
		return;
	}
	f->next = NULL;
	SDL_strlcpy(f->sprite_name, sprite, s);
	if (!a->tail)
	{
		a->tail = f;
		a->curr = f;
	}
	else
		a->head->next = f;
	a->head = f;
}

void frame_destroy(struct frame** f)
{
	INFO("Destroying frame %s.", (*f)->sprite_name);
	free((*f)->sprite_name);
	free(*f);
}

void animation_table_load(const char* name, struct animation_table* t, SDL_Renderer* renderer)
{
	// TODO: Replace +20.
	size_t s = SDL_strlen(name) + 20;
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
	fopen_s(&f, file_name, "r");
	if (f != NULL)
	{
		char buffer[BUFFER_SIZE], command[21], texture_name[MAX_AN_LENGTH];
		struct animation* a = NULL;
		while (!feof(f)) 
		{
			fgets(buffer, BUFFER_SIZE, f);
			// Parse first string and ignore everything else except a newline.
			sscanf_s(buffer, "%s%*[^\n]", command, 21);
			if (SDL_strcmp(command, "ANIMATION") == 0)
			{
				char animation_name[MAX_AN_LENGTH];
				sscanf_s(buffer, "%*s%s", animation_name, MAX_AN_LENGTH);
				animation_create(&a, animation_name);
				INFO("Loading animation %s", animation_name);
			}
			else if (SDL_strcmp(command, "ANIMATION_END") == 0)
			{
				animation_table_add(t, a);
				a = NULL;
			}
			else if (SDL_strcmp(command, "FRAME") == 0)
			{
				SDL_Rect r;
				char sprite_name[MAX_AN_LENGTH];
				sscanf_s(buffer, "%*s%s%d%d%d%d", sprite_name, MAX_AN_LENGTH, &r.x, &r.y, &r.w, &r.h);
				INFO("Loading frame %s [%d;%d]", sprite_name, r.x, r.y);
				sprite_add(sprite_name, texture_name, r);
				frame_add(a, sprite_name);
			}
			else if (SDL_strcmp(command, "TEXTURE") == 0)
			{
				sscanf_s(buffer, "%*s%s", texture_name, MAX_AN_LENGTH);
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

void animation_create(struct animation** a, const char* name)
{
	(*a) = malloc(sizeof(struct animation));
	if (!(*a))
	{
		ERROR("Not enough memory!");
		return;
	}
	(*a)->curr = (*a)->head = (*a)->tail = NULL;
	(*a)->next = NULL;
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
	if (a == NULL)
		return;
	struct frame* iter = a->tail;
	while (iter)
	{
		iter = iter->next;
		frame_destroy(&a->tail);
		a->tail = iter;
	}
	free(a->name);
	free(a);
}

void animation_table_add(struct animation_table* t, struct animation* a)
{
	size_t index = hash_s(a->name) % ANIMATION_ARR_SIZE;
	struct animation* iter = t->a[index], *prev = NULL;
	while (iter)
	{
		prev = iter;
		iter = iter->next;
	}
	if (!prev)
	{
		t->a[index] = a;
	}
	else
		prev->next = a;
	INFO("Animation %s loaded to index %d.", a->name, index);
}

void animation_table_set(struct animation_table* t, const char *name)
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

void animation_table_destroy(struct animation_table* t)
{
	for (int i = 0; i < ANIMATION_ARR_SIZE; ++i)
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

void animation_table_next(struct animation_table* t)
{
	t->curr->curr = t->curr->curr->next;
	if (!t->curr->curr)
		t->curr->curr = t->curr->tail;
}