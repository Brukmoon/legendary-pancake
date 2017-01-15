#include "sprite.h"
#include "texture.h"
#include "common.h"
#include "hash.h"
// Level sprite manager
int texture_add(const char* name, SDL_Renderer *renderer)
{
	// Texture ID (unique).
	static int id = -1;
	id++;
	SDL_Texture *texture = load_texture(renderer, name);
	if (!texture) // error in texture loading
		return -1;
	texture_container[id] = texture;
	INFO("Added texture %s (ID %d).", name, id);
	return id;
}

inline SDL_Texture* texture_get(int id) { return texture_container[id]; }

void sprite_add(const char *name, int texture_id, SDL_Rect *target)
{
	INFO("Adding sprite %s (#%d) [%d, %d, %d, %d].", name, texture_id, target->x, target->y, target->w, target->h);
	if (!texture_get(texture_id))
	{
		ERROR("Texture #%d does not exist in the resource container. Call texture_add first to load the texture before adding a sprite.", texture_id);
		return;
	}
	struct sprite_bucket *new_bucket = (struct sprite_bucket*) malloc(sizeof(struct sprite_bucket));
	if (!new_bucket)
	{
		ERROR("Not enough memory.");
		return;
	}
	new_bucket->sprite.target = target;
	new_bucket->sprite.texture_id = texture_id;
	SDL_strlcpy(new_bucket->key, name, RESOURCE_LENGTH);
	new_bucket->next = NULL;
	int index = hash_s(name) % SPRITE_ARR_SIZE;
	struct sprite_bucket* iter = sprites_container.data[index], *prev = NULL;
	while (iter)
	{
		prev = iter;
		iter = iter->next;
	}
	if (!prev)
	{
		sprites_container.data[index] = new_bucket;
	}
	else
		prev->next = new_bucket;
}

void destroy_sprites(void)
{
	for (int i = 0; i < SPRITE_ARR_SIZE; ++i)
	{
		while (sprites_container.data[i])
		{
#ifdef _DEBUG
			int ll_position = 0; // position in linked list
			ll_position++;
#endif // _DEBUG
			struct sprite_bucket *iter = sprites_container.data[i];
			sprites_container.data[i] = iter->next;
#ifdef _DEBUG
			INFO("Freeing sprite %s from index %d (position %d).", iter->key, i, ll_position);
#endif // _DEBUG
			free(iter->sprite.target);
			free(iter);
		}
	}
}

SDL_Texture *sprite_get(const char* name, SDL_Rect** target)
{
	unsigned long index = hash_s(name) % SPRITE_ARR_SIZE;
	struct sprite_bucket* iter = sprites_container.data[index];
	while (iter)
	{
		if (SDL_strcmp(name, iter->key) == 0) // string match
		{
			*target = iter->sprite.target;
			return texture_get(iter->sprite.texture_id);
		}
		iter = iter->next;
	}
	ERROR("Texture %s not found. Use function sprite_add first to load the sprite into the resource manager.", name);
	return NULL;
}