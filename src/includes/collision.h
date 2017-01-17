/*
 * Collision detection.
 * @author Michal H.
 *
 **/
#ifndef COLLISION_H
#define COLLISION_H

#include <SDL.h>
#include <stdbool.h>

// (forward declaration)
struct level;

// AABB collision detection.
bool rects_collide(const SDL_Rect* rect1, const SDL_Rect *rect2);
// Does a rectangle collide with the tilemap?
bool tilemap_collision(const struct level *level, const SDL_Rect* rect);

#endif // COLLISION_H