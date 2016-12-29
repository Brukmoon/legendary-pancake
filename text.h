#ifndef TEXT_H
#define TEXT_H

#include <SDL_ttf.h>
#include "vector.h"

#define FONT_SIZE 25

struct bucket
{
	struct bucket *next;
	int key;

	SDL_Color color;
	TTF_Font *font;
};

static struct font_container
{
	struct bucket **hash_array;
	int max_size;
} g_fonts;

static inline int hash_code(int key, int buffer_size) { return key%buffer_size; }
TTF_Font *font_add(struct font_container *table, int size, const SDL_Color *color);
TTF_Font *font_get(struct font_container *table, int size, const SDL_Color *color);

bool fonts_init(int buffer_size);
void fonts_destroy(void);

// Text --> SDL_Texture
SDL_Texture* create_text_texture(SDL_Renderer* renderer, const char* text, int size, SDL_Color color);

#endif // TEXT_H