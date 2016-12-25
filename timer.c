#include <SDL.h>
#include "timer.h"

void timer_reset(struct timer *t)
{
	t->paused = false;
	t->start_ticks = SDL_GetTicks();
	t->ticks = 0;
}

int timer_ticks(struct timer *t)
{
	if (!t->paused)
		t->ticks = SDL_GetTicks() - t->start_ticks;
	return t->ticks;
}