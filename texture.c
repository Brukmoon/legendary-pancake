#include <SDL_image.h>

#include "texture.h"

// WIP
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file_name)
{
	SDL_Surface *surface = IMG_Load(file_name);
	if (!surface)
		return NULL;
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture)
		return NULL;
	return texture;
}