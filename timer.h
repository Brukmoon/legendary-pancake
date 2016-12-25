#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

struct timer
{
	int start_ticks;
	int ticks;
	bool paused;
};

inline void timer_pause(struct timer *t) { t->paused = !t->paused; }
void timer_reset(struct timer *t);
int timer_ticks(struct timer *t);

#endif // TIMER_H