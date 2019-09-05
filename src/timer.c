#include <SDL.h>

#include "timer.h"
#include "text.h"

#include "common.h"

void timer_reset(struct timer *t)
{
	t->paused = false;
	t->last_tick_time = SDL_GetTicks();
	t->delta = 0;
	t->draw_time_seconds = -1;
	SDL_DestroyTexture(t->time);
	t->time = NULL;
}

int timer_ticks(struct timer *t)
{
	t->delta = SDL_GetTicks() - t->last_tick_time;
	// TODO: Fix pausing.
	if (t->paused)
	{
		t->last_tick_time += t->delta;
		t->delta = 0;
	}
	return t->delta;
}

void timer_set_pause(struct timer *t, bool yesno) { t->paused = yesno; }
