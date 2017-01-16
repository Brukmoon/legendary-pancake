#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include <SDL.h>

struct text_box
{
	SDL_Rect r;
	char *text;
	int c_length;
	int max_length;
};

#endif // TEXT_BOX_H