#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include <SDL.h>

struct text_box
{
	SDL_Rect r;
	char *text;
	int curr_length;
	int max_length;
	struct text_box *next, *prev;
};

struct text_box* text_box_create(struct text_box* parent, const SDL_Rect skeleton, int max_length);
void text_box_destroy(struct text_box *box);

#endif // TEXT_BOX_H
