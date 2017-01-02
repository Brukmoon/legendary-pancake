#include <SDL_image.h>

#include "common.h"
#include "texture.h"

// WIP
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file_name)
{
	SDL_Surface *surface = IMG_Load(file_name);
	if (!surface)
	{
		ERROR("Couldn't load image %s.", file_name);
		return NULL;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture)
	{
		ERROR("Couldn't create texture %s.", file_name);
		return NULL;
	}
	return texture;
}