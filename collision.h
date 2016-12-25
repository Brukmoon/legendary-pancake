#ifndef COLLISION_H
#define COLLISION_H

#include <SDL.h>
#include <stdbool.h>

struct level;

// Does the rect collide with the tilemap?
bool tilemap_collision(const struct level *level, const SDL_Rect* rect);

#endif // COLLISION_H