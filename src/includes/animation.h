#ifndef ANIMATION_H
#define ANIMATION_H

#include "sprite.h"

// Animation frame.
struct frame
{
	char *sprite;
	struct frame* next;
};

struct animation
{
	struct frame* head, *tail, *curr;
	unsigned count;
};


#endif // ANIMATION_H