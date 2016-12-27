 /*
 * Camera used for map traversing purposes.
 * @author Michal H.
 *
 */
#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>
#include "config.h"
#include "position.h"

// Center of the screen, relative to which the camera can be placed.
#define CENTER_X SCREEN_WIDTH/2
#define CENTER_Y SCREEN_HEIGHT/2
	
extern struct camera
{
	vec2 position;
} g_camera; // main camera

// Initialize the camera offsets.
void init_camera(struct camera *camera);

// Set camera position to [x;y].
void set_camera(struct camera *camera, const vec2 position);
// Change camera position to [x+d_x;y+d_y]
void scroll_camera(struct camera *camera, const vec2 delta);
// Is the rectangle position:w:h visible on camera?
bool is_visible(const struct camera *camera, const vec2 *const position, const int w, const int h);

#endif // CAMERA_H