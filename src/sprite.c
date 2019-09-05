#include "config.h"
#include "sprite.h"
#include "texture.h"
#include "common.h"
#include "hash.h"

// Level sprite manager
const char* texture_add(const char* name, SDL_Renderer *renderer)
{
	struct texture_bucket* new_bucket = (struct texture_bucket*) malloc(sizeof(struct texture_bucket));
	if (!new_bucket)
	{
		ERROR("Not enough memory!");
		return NULL;
	}
	SDL_Texture *texture = load_texture(renderer, name);
	if (!texture) // error in texture loading
		return NULL;
	new_bucket->texture = texture;
	size_t name_len = SDL_strlen(name) + 1;
	new_bucket->key = malloc(name_len);
	new_bucket->next = NULL;
	if (!new_bucket->key)
	{
		ERROR("Not enough memory.");
		return NULL;
	}
	SDL_strlcpy(new_bucket->key, name, name_len);

	// where to save it in the array
	int id = hash_s(name) % TEXTURE_ARR_SIZE;
	struct texture_bucket* iter = textures_container.data[id], *prev = NULL;
	while (iter)
	{
		prev = iter;
		iter = iter->next;
	}
	if (!prev)
	{
		textures_container.data[id] = new_bucket;
	}
	else
		prev->next = new_bucket;
	INFO("Texture %s loaded to index %d.", name, id);
	return name;
}

SDL_Texture* texture_get(const char* name) 
{ 
	int id = hash_s(name) % TEXTURE_ARR_SIZE;	
	struct texture_bucket* iter = textures_container.data[id];
	while (iter)
	{
		if (SDL_strcmp(iter->key, name) == 0) // string match
			return iter->texture;
		iter = iter->next;
	}
	// texture not found
	return NULL;
}

void textures_destroy(void)
{
	for (int i = 0; i < TEXTURE_ARR_SIZE; ++i)
	{
		while (textures_container.data[i])
		{
			int ll_position = 0; // position in linked list
			ll_position++;
			struct texture_bucket *iter = textures_container.data[i];
			textures_container.data[i] = iter->next;
			INFO("Freeing texture %s from index %d (position %d).", iter->key, i, ll_position);
			free(iter->key);
			SDL_DestroyTexture(iter->texture);
			free(iter);
		}
	}
}

void sprite_add(const char *name, const char* texture_name, const SDL_Rect target)
{
	INFO("Adding sprite %s (%s) [%d, %d, %d, %d].", name, texture_name, target.x, target.y, target.w, target.h);
	if (!texture_get(texture_name))
	{
		ERROR("Texture %s does not exist in the resource container. Call texture_add first to load the texture before adding a sprite.", texture_name);
		return;
	}
	if (sprite_get(name, NULL))
	{
		INFO("Sprite %s already added. Skipping.", name);
		return;
	}
	struct sprite_bucket *new_bucket = (struct sprite_bucket*) malloc(sizeof(struct sprite_bucket));
	if (!new_bucket)
	{
		ERROR("Not enough memory.");
		return;
	}
	new_bucket->sprite.target = malloc(sizeof(SDL_Rect));
	if (!new_bucket->sprite.target)
	{
		ERROR("Not enough memory!");
		return;
	}
	new_bucket->sprite.target->h = target.h;
	new_bucket->sprite.target->w = target.w;
	new_bucket->sprite.target->x = target.x;
	new_bucket->sprite.target->y = target.y;

	size_t name_len = SDL_strlen(texture_name) + 1;
	new_bucket->sprite.texture = malloc(name_len);
	SDL_strlcpy(new_bucket->sprite.texture, texture_name, name_len);
	name_len = SDL_strlen(name) + 1;
	new_bucket->key = malloc(name_len);
	SDL_strlcpy(new_bucket->key, name, name_len);
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

void sprites_destroy(void)
{
	for (int i = 0; i < SPRITE_ARR_SIZE; ++i)
	{
		while (sprites_container.data[i])
		{
			int ll_position = 0; // position in linked list
			ll_position++;
			struct sprite_bucket *iter = sprites_container.data[i];
			sprites_container.data[i] = iter->next;
			INFO("Freeing sprite %s from index %d (position %d).", iter->key, i, ll_position);
			free(iter->key);
			free(iter->sprite.texture);
			free(iter->sprite.target);
			free(iter);
		}
	}
	textures_destroy();
}

SDL_Texture *sprite_get(const char* name, SDL_Rect** target)
{
	unsigned long index = hash_s(name) % SPRITE_ARR_SIZE;
	struct sprite_bucket* iter = sprites_container.data[index];
	while (iter)
	{
		if (SDL_strcmp(name, iter->key) == 0) // string match
		{
			if(target)
				*target = iter->sprite.target;
			return texture_get(iter->sprite.texture);
		}
		iter = iter->next;
	}
	return NULL;
}
