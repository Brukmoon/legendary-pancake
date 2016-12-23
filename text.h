#ifndef TEXT_H
#define TEXT_H

#include <SDL_ttf.h>
#include "position.h"

#define FONT_SIZE 25
// good monospace font
#define FONT_TYPE "Consolas.ttf"

// One font rules them all.
TTF_Font *font;
static struct
{
	TTF_Font *data[5];
	int size[5];
} cache;
static int cache_count = 0;

bool font_init(int size);
bool font_cache(int size);
void add_to_cache(TTF_Font* font, int size);

// Text --> SDL_Texture
SDL_Texture* create_text_texture(SDL_Renderer* renderer, const char* text, int size, SDL_Color color);

#endif // TEXT_H