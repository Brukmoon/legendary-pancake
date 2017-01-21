#include "text_box.h"

struct text_box* text_box_create(struct text_box* parent, const SDL_Rect skeleton, int max_length)
{
	struct text_box* text_box = malloc(sizeof(struct text_box));
	text_box->curr_length = 0;
	text_box->max_length = max_length;
	text_box->r = skeleton;
	text_box->text = malloc(max_length + 1);
	SDL_strlcpy(text_box->text, "level", max_length);
	text_box->next = NULL;
	text_box->prev = parent;
	if (parent) // Parent exists.
	{
		parent->next = text_box;
		//INFO("Linked button [%d;%d] with button [%d;%d].", parent->r.x, parent->r.y, butt->position.x, butt->position.y);
	}
	return text_box;
}

void text_box_destroy(struct text_box *box)
{
	free(box->text);
	free(box);
}