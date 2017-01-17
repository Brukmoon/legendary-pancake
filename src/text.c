#include "config.h"
#include "common.h"
#include "hash.h"
#include "text.h"

static bool colors_equal(const SDL_Color *first, const SDL_Color *second);

SDL_Texture* create_text_texture(SDL_Renderer* renderer, const char* text, int size, SDL_Color color)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font_get(&g_fonts, size, &color), text, color);
	if (!surface)
	{
		ERROR("Couldn't create %s text surface.", text);
		return NULL;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
	{
		ERROR("Couldn't create %s text font_texture.", text);
		return NULL;
	}
	SDL_FreeSurface(surface);
	return texture;
}

void draw_text(const char *text, int font_size, const SDL_Color color, vec2 destination, SDL_Renderer *renderer)
{
	SDL_Texture *p_coord = create_text_texture(renderer, text, font_size, color);
	SDL_Rect rect;
	rect.x = destination.x;
	rect.y = destination.y;
	SDL_QueryTexture(p_coord, NULL, NULL, &rect.w, &rect.h);
	SDL_RenderCopy(renderer, p_coord, NULL, &rect);
	SDL_DestroyTexture(p_coord);
}

bool fonts_init(int buffer_size)
{
	if (TTF_Init() < 0)
	{
		ERROR("TTF engine couldn't be initialized. %s", TTF_GetError());
		return false;
	}
	g_fonts.hash_array = malloc(sizeof(struct bucket*) * buffer_size);
	if (!g_fonts.hash_array)
	{
		ERROR("Not enough memory!");
		return false;
	}
	g_fonts.max_size = buffer_size;
	for (int i = 0; i < buffer_size; ++i)
		g_fonts.hash_array[i] = NULL;
	return true;
}

TTF_Font* font_add(struct font_container *table, int size, const SDL_Color *color)
{
	// Create a new bucket.
	struct bucket* new_bucket = (struct bucket*) malloc(sizeof(struct bucket));
	if (!new_bucket)
	{
		ERROR("Not enough memory.");
		return NULL;
	}
	new_bucket->font = TTF_OpenFont(FONT_FILE, size);
	if (!new_bucket->font)
	{
		ERROR("Font %s couldn't be opened. %s", FONT_FILE, TTF_GetError());
		return NULL;
	}
	new_bucket->next = NULL;
	new_bucket->key = size;
	new_bucket->color = *color;
	// Find the index.
	int index = hash_i(new_bucket->key) % table->max_size;
	struct bucket* iter = table->hash_array[index], *prev = NULL;
	while (iter)
	{
		prev = iter;
		iter = iter->next;
	}
	if (prev) // Bucket not empty.
		prev->next = new_bucket;
	else // Bucket empty.
		table->hash_array[index] = new_bucket;
	return new_bucket->font;
}

void fonts_destroy(void)
{
	for (int i = 0; i < g_fonts.max_size; ++i)
	{
		while (g_fonts.hash_array[i])
		{
			INFO("Destroying font %d on index %d.", g_fonts.hash_array[i]->key, i);
			struct bucket *iter = g_fonts.hash_array[i];
			g_fonts.hash_array[i] = iter->next;
			TTF_CloseFont(iter->font);
			free(iter);
		}
	}
	free(g_fonts.hash_array);
}

bool colors_equal(const SDL_Color *first, const SDL_Color *second) 
{ 
	return first->a == second->a && first->b == second->b && first->g == second->g && first->a == second->a; 
}

TTF_Font *font_get(struct font_container *table, int size, const SDL_Color *color)
{
	int index = hash_i(size) % table->max_size;
	struct bucket* iter = table->hash_array[index], *prev = NULL;
	while (iter && iter->key != size && !colors_equal(color, &iter->color))
	{
		prev = iter;
		iter = iter->next;
	}
	if (!iter) // not found
		return font_add(&g_fonts, size, color);
	return iter->font;
}