#include "common.h"
#include "text.h"

#define FONT_FILE "data/"FONT_TYPE

SDL_Texture* create_text_texture(SDL_Renderer* renderer, const char* text, int size, SDL_Color color)
{
	if (!font)
	{
		ERROR("Font not initialized.");
		return NULL;
	}
	font_init(size);
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
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

bool font_init(int size)
{
	if (TTF_Init() < 0)
	{
		ERROR("TTF engine couldn't be initialized. %s", TTF_GetError());
		return false;
	}
	if (font_cache(size))
		return true;
	else
	{
		font = TTF_OpenFont(FONT_FILE, size);
		if(cache_count <= 4)
			add_to_cache(font, size);
	}
	if (!font)
	{
		ERROR("Font %s couldn't be opened.", FONT_FILE);
		return false;
	}
	return true;
}

bool font_cache(int size)
{
	for (int i = 0; i < cache_count; ++i)
		if (cache.size[i] == size)
		{
			font = cache.data[i];
			return true;
		}
	return false;
}

void add_to_cache(TTF_Font *font, int size)
{
	cache.data[cache_count] = font;
	cache.size[cache_count] = size;
	cache_count++;
}