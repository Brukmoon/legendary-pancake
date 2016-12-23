#ifndef COLLISION_H
#define COLLISION_H

#include <SDL.h>
#include <stdbool.h>
#include "level.h"

bool tilemap_collision(const struct level *level, const SDL_Rect* rect);

#endif // COLLISION_H