#ifndef TEXT_H
#define TEXT_H

#include <SDL_ttf.h>
#include "position.h"

#define FONT_SIZE 25
// good monospace font
#define FONT_TYPE "Consolas.ttf"

struct bucket
{
	struct bucket *next;
	int key;
	TTF_Font *font;
};

static struct font_container
{
	struct bucket **hash_array;
	int max_size;
} g_fonts;

static inline int hash_code(int key, int htable_size) { return key%htable_size; }
TTF_Font *add_font(struct font_container *table, int size);
TTF_Font *get_font(struct font_container *table, int size);

bool init_fonts(int buffer_size);
void destroy_fonts();

// Text --> SDL_Texture
SDL_Texture* create_text_texture(SDL_Renderer* renderer, const char* text, int size, SDL_Color color);

#endif // TEXT_H