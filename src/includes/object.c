#include "actor.h"
#include "collision.h"
#include "camera.h"
#include "common.h"
#include "level.h"
#include "object.h"
#include "sound.h"

static struct object *g_object_head = NULL, *g_object_root = NULL;
static struct missile *g_missile_hea= NULL, *g_missile_root = NULL;

static void	object_collision(struct object* obj);

static bool object_init(struct object** o, const SDL_Rect* spawn, const char* name, SDL_Renderer *renderer)
{
	(*o) = malloc(sizeof(struct object));
	if (!*o)
	{
		ERROR("Not enough memory!");
		return false;
	}
	(*o)->skeleton = *spawn;
	(*o)->next = NULL;
	(*o)->is_active = true;
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


void object_add(const char* name, SDL_Rect spawn, SDL_Renderer* renderer)
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
		if (iter->is_active)
		{
			dest.w = iter->skeleton.w;
			dest.h = iter->skeleton.h;
			dest.x = iter->skeleton.x - g_camera.position.x;
			dest.y = iter->skeleton.y - g_camera.position.y;
			SDL_Rect* src = NULL;
			SDL_Texture* t = NULL;
			t = sprite_get(iter->t.curr->curr->sprite_name, &src);
			SDL_RenderCopy(renderer, t, src, &dest);
		}
		iter = iter->next;
	}
	missile_draw(renderer);
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
	missile_destroy();
}

void object_update(void)
{
	struct object* iter = g_object_root;
	while (iter)
	{
		if (iter->is_active)
		{
			iter->t.curr->delay_counter += 1000 / FPS;
			if (iter->t.curr->delay_counter > iter->t.curr->delay)
			{
				animation_next(&iter->t);
				iter->t.curr->delay_counter = 0;
			}
			object_collision(iter);
		}
		iter = iter->next;
	}
	missile_update();
}

static void	object_collision(struct object* obj)
{
	if (rects_collide(&obj->skeleton, &g_player.actor.skeleton))
	{
		obj->is_active = false;
		if (SDL_strcmp(obj->name, "collect") == 0)
		{
			sound_play("pick");
			g_player.collect += 1;
		}
	}
}

void missile_fire(struct player *source, const float* velocity, SDL_Renderer *renderer)
{
	if (player_can_shoot(source))
	{
		struct missile* m = malloc(sizeof(struct missile));
		if (m)
		{
			if (!g_missile_root)
			{
				m->next = m->prev = NULL;
				g_missile_root = m;
				g_missile_head = m;
			}
			else
			{
				m->next = NULL;
				m->prev = g_missile_head;
				g_missile_head->next = m;
				g_missile_head = m;
			}
			m->skeleton = source->actor.skeleton;
			m->velocity = *velocity;
			m->degrees = 0;
			animation_table_load("shoot", &m->t, renderer);
			animation_set("shoot", &m->t);
			sound_play("shoot");
			source->collect--;
			INFO("Missile fired!.");
		}
		else
			ERROR("Not enough memory!");
	}
}

// TODO: move to missiles_destroy
static void missile_destroy(void)
{
	struct missile* iter = g_missile_root;
	while (iter)
	{
		iter = g_missile_root->next;
		animation_table_destroy(&g_missile_root->t);
		free(g_missile_root);
		g_missile_root = iter;
	}
}

static void missile_remove(struct missile* m)
{
	if (m->next || m->prev)
	{
		if (m->next && m->prev)
		{
			m->prev->next = m->next;
			m->next->prev = m->prev;
		}
		else
		{
			if (!m->next)
			{
				m->prev->next = NULL;
				g_missile_head = m->prev;
			}
			else
			{
				m->next->prev = NULL;
				g_missile_root = m->next;
			}
		}

	}
	else // empty
		g_missile_root = g_missile_head = NULL;

	animation_table_destroy(&m->t);
	free(m);
}

static void missile_update(void)
{
	struct missile* iter = g_missile_root;
	while (iter)
	{
		iter->skeleton.x += (int)iter->velocity;
		iter->t.curr->delay_counter += 1000 / FPS;
		if (iter->t.curr->delay_counter > iter->t.curr->delay)
		{
			animation_next(&iter->t);
			iter->t.curr->delay_counter = 0;
		}
		if (tilemap_collision(g_level, &iter->skeleton, TILE_COLLISION))
		{
			missile_remove(iter);
			// redo updating
			missile_update();
			return;
		}
		iter = iter->next;
	}
}

void missile_draw(SDL_Renderer* renderer)
{
	struct missile* iter = g_missile_root;
	SDL_Rect dest;
	while (iter)
	{
		dest.w = iter->skeleton.w;
		dest.h = iter->skeleton.h;
		dest.x = iter->skeleton.x - g_camera.position.x;
		dest.y = iter->skeleton.y - g_camera.position.y;
		SDL_Rect* src = NULL;
		SDL_Texture* t = NULL;
		//t = sprite_get(iter->t.curr->curr->sprite_name, &src);
		t = sprite_get("bamboo", &src);
		/*if(iter->velocity>=0)
			SDL_RenderCopy(renderer, t, src, &dest);
		else
			SDL_RenderCopyEx(renderer, t, src, &dest, 0,NULL,SDL_FLIP_HORIZONTAL);*/
		iter->degrees += 20;
		SDL_RenderCopyEx(renderer, t, src, &dest, iter->degrees, NULL, SDL_FLIP_NONE);
		iter = iter->next;
	}
}