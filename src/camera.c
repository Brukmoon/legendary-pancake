#include "common.h"
#include "config.h"
#include "camera.h"
#include "level.h"

struct camera g_camera = { CAMERA_FREE, { INITIAL_CAMERA_X, INITIAL_CAMERA_Y } };

void camera_init(struct camera *camera, const enum camera_type t)
{
	if (!camera)
	{
		ERROR("Camera can't be NULL.");
		return;
	}
	g_camera.t = t;
	// Camera defaults to the center of the screen.
	camera_set(camera, (vec2) { INITIAL_CAMERA_X, INITIAL_CAMERA_Y });
	INFO("Camera offsets set to (%d, %d).", camera->position.x, camera->position.y);
	// TODO: Limit scrolling.
}

#define MAX_CAMERA_X g_level->tile_map.width*g_level->tile_map.tile_width-SCREEN_WIDTH
#define MAX_CAMERA_Y g_level->tile_map.height*g_level->tile_map.tile_height-SCREEN_HEIGHT

void camera_set(struct camera *camera, vec2 position)
{
	if (!camera)
	{
		ERROR("Camera can't be NULL.");
		return;
	}
#if CAMERA_LIMIT
	if (camera->t == CAMERA_FIXED)
	{
		// Is camera out of map?
		if (position.x < 0)
			position.x = 0;
		else if (g_level->tile_map.width*g_level->tile_map.tile_width > SCREEN_WIDTH && position.x > MAX_CAMERA_X)
			position.x = MAX_CAMERA_X;
		if (position.y < 0)
			position.y = 0;
		else if (g_level->tile_map.height*g_level->tile_map.tile_height > SCREEN_HEIGHT && position.y > MAX_CAMERA_Y)
			position.y = MAX_CAMERA_Y;
	}
#endif // NO_CAMERA_LIMIT
	camera->position = position;
}

#undef MAX_CAMERA_X
#undef MAX_CAMERA_Y

void camera_scroll(struct camera *camera, const vec2 delta)
{
	if (!camera)
	{
		ERROR("Camera can't be NULL.");
		return;
	}
	camera_set(camera, (vec2) { camera->position.x + delta.x, camera->position.y + delta.y });
}

bool is_visible(const struct camera *camera, const vec2 *const position, const int w, const int h)
{
	if(position->x < camera->position.x+CENTER_X+CENTER_X && 
	   position->x > camera->position.x+CENTER_X-CENTER_X-w &&
	   position->y < camera->position.y+CENTER_Y+CENTER_Y && 
	   position->y > camera->position.y+CENTER_Y-CENTER_Y-h)
		return true;
	return false;
}
