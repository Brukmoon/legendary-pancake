 /*
 * Camera used for map traversing purposes.
 * @author Michal H.
 *
 */
#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

#include "config.h"
#include "vector.h"

// Center of the screen, relative to which the camera can be placed.
#define CENTER_X SCREEN_WIDTH/2
#define CENTER_Y SCREEN_HEIGHT/2

// Initial camera position.
enum initial_camera_coords
{
	INITIAL_CAMERA_X = 0,
	INITIAL_CAMERA_Y = 0
};

enum camera_type
{
	CAMERA_FREE,
	CAMERA_FIXED
};
	
extern struct camera
{
	enum camera_type t;
	vec2 position;
} g_camera; // main camera

// Initialize the camera offsets.
void camera_init(struct camera *camera, const enum camera_type t);

// Set camera position to [x;y].
void camera_set(struct camera *camera, vec2 position);
// Change camera position to [x+d_x;y+d_y]
void camera_scroll(struct camera *camera, const vec2 delta);
// Is the rectangle position:w:h visible on camera?
bool is_visible(const struct camera *camera, const vec2 *const position, const int w, const int h);

#endif // CAMERA_H