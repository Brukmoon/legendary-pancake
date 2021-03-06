#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <SDL.h>

struct timer
{
	int last_tick_time;
	int delta;
	bool paused;

	int draw_time_seconds;
	SDL_Texture* time;
} g_timer;

inline void timer_set_pause(struct timer *t, bool yesno) { t->paused = yesno; }
void timer_reset(struct timer *t);
int timer_ticks(struct timer *t);

#endif // TIMER_H