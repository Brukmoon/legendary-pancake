#include "common.h"
#include "camera.h"
#include "level.h"

struct camera g_camera = { 0, 0 };

void init_camera(struct camera *camera)
{
	if (!camera)
	{
		ERROR("Camera can't be NULL.");
		return;
	}
	// Camera defaults to the center of the screen.
	set_camera(camera, (vec2) { 0, 0 });
	INFO("Camera offsets set to (%d, %d).", camera->position.x, camera->position.y);
	// TODO: Limit scrolling.
}

#define MAX_CAMERA_X g_level->tile_map.width*g_level->tile_map.tile_width-SCREEN_WIDTH
#define MAX_CAMERA_Y g_level->tile_map.height*g_level->tile_map.tile_height-SCREEN_HEIGHT

void set_camera(struct camera *camera, vec2 position)
{
	if (!camera)
	{
		ERROR("Camera can't be NULL.");
		return;
	}
	// Is camera out of map?
	if (position.x < 0)
		position.x = 0;
	else if (g_level->tile_map.width*g_level->tile_map.tile_width > SCREEN_WIDTH && position.x > MAX_CAMERA_X)
		position.x = MAX_CAMERA_X;
	if (position.y < 0)
		position.y = 0;
	else if (g_level->tile_map.height*g_level->tile_map.tile_height > SCREEN_HEIGHT && position.y > MAX_CAMERA_Y)
		position.y = MAX_CAMERA_Y;
	camera->position = position;
}

#undef MAX_CAMERA_X
#undef MAX_CAMERA_Y

void scroll_camera(struct camera *camera, const vec2 delta)
{
	if (!camera)
	{
		ERROR("Camera can't be NULL.");
		return;
	}
	set_camera(camera, (vec2) { camera->position.x + delta.x, camera->position.y + delta.y });
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
