#include "camera.h"
#include "common.h"
#include "object.h"

static struct object *g_object_head = NULL, *g_object_root = NULL;


static bool object_init(struct object** o, const vec2* spawn, const char* name, SDL_Renderer *renderer)
{
	(*o) = malloc(sizeof(struct object));
	if (!*o)
	{
		ERROR("Not enough memory!");
		return false;
	}
	(*o)->spawn = *spawn;
	(*o)->next = NULL;
	size_t name_length = SDL_strlen(name) + 1;
	(*o)->name = malloc(name_length);
	if (!(*o)->name)
	{
		ERROR("Not enough memory!");
		return false;
	}
	SDL_strlcpy((*o)->name, name, name_length);
	animation_table_load(name, &(*o)->t, renderer);
	animation_set("rotate", &(*o)->t);
	return true;
}


void object_add(const char* name, vec2 spawn, SDL_Renderer* renderer)
{
	struct object* obj = NULL;
	if (object_init(&obj, &spawn, name, renderer))
	{
		if (!g_object_root)
		{
			g_object_root = obj;
			g_object_head = obj;
		}
		else
		{
			g_object_head->next = obj;
			g_object_head = obj;
		}
		INFO("Object %s added.", name);
	}
	else
		ERROR("Object %s couldn't be added.", name);
}

void object_draw(SDL_Renderer* renderer)
{
	struct object* iter = g_object_root;
	SDL_Rect dest;
	while (iter)
	{
		dest.w = dest.h = 32; // Draw a bit larger.
		dest.x = iter->spawn.x - g_camera.position.x;
		dest.y = iter->spawn.y - g_camera.position.y;
		SDL_Rect* src = NULL;
		SDL_Texture* t = NULL;
		t = sprite_get(iter->t.curr->curr->sprite_name, &src);
		SDL_RenderCopy(renderer, t, src, &dest);
		iter = iter->next;
	}
}

void object_destroy(void)
{
	struct object* iter = g_object_root;
	while (iter)
	{
		iter = g_object_root->next;
		animation_table_destroy(&g_object_root->t);

		free(g_object_root->name);
		free(g_object_root);
		g_object_root = iter;
	}
}

void object_update(void)
{
	struct object* iter = g_object_root;
	while (iter)
	{
		iter->t.curr->delay_counter += 1000 / FPS;
		if (iter->t.curr->delay_counter > iter->t.curr->delay)
		{
			animation_next(&iter->t);
			iter->t.curr->delay_counter = 0;
		}
		iter = iter->next;
	}
}