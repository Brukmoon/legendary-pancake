#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>

struct ssheet_element
{
	const char** name;
	SDL_Rect *position;
};
struct spritesheet
{
	SDL_Texture *spritesheet_texture;
	struct ssheet_element *el;
};

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file_name);

#endif // TEXTURE_H