#ifndef TEXT_H
#define TEXT_H

#include <SDL_ttf.h>

#include "vector.h"

#define FONT_BUFFER_SIZE 5

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

TTF_Font *font_add(struct font_container *table, int size, const SDL_Color *color);
TTF_Font *font_get(struct font_container *table, int size, const SDL_Color *color);

bool fonts_init(int buffer_size);
void fonts_destroy(void);

// Text --> SDL_Texture
/*
 * BEWARE: There is no GC mechanism included. Remember to SDL_DestroyTexture!
 */
SDL_Texture* create_text_texture(SDL_Renderer* renderer, const char* text, int size, SDL_Color color);
// Draws text, then deletes the texture. Very heavy, use with caution (in Debug only).
void draw_text(const char *text, int font_size, const SDL_Color color, vec2 destination, SDL_Renderer *renderer);

#endif // TEXT_H