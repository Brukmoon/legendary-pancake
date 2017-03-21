#include <SDL.h>

#include "timer.h"

void timer_reset(struct timer *t)
{
	t->paused = false;
	t->last_tick_time = SDL_GetTicks();
	t->delta = 0;
}

int timer_ticks(struct timer *t)
{
	if (!t->paused)
		t->delta = SDL_GetTicks() - t->last_tick_time;
	return t->delta;
}