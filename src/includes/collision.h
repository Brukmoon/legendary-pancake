/*
 * Collision detection.
 * @author Michal H.
 *
 **/
#ifndef COLLISION_H
#define COLLISION_H

#include <SDL.h>
#include <stdbool.h>

struct level;

// AABB collision detection.
bool rects_collide(const SDL_Rect* rect1, const SDL_Rect *rect2);
// Does the rect collide with the tilemap?
bool tilemap_collision(const struct level *level, const SDL_Rect* rect);

#endif // COLLISION_H