#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL.h>

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* file_name);

#endif // TEXTURE_H