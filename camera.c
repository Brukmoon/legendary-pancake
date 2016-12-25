#include "common.h"
#include "camera.h"

struct camera g_camera = { 0, 0 };

void init_camera(struct camera *camera)
{
	if (!camera)
	{
		INFO("Camera can't be NULL.");
		return;
	}
	// Camera defaults to the center of the screen.
	set_camera(camera, (const struct vec2) { 0, 0 });
	INFO("Camera offsets set to (%d, %d).", camera->position.x, camera->position.y);
	// TODO: Limit scrolling.
}

void set_camera(struct camera *camera, const struct vec2 position)
{
	if (!camera)
	{
		INFO("Camera can't be NULL.");
		return;
	}
	camera->position = position;
}

void scroll_camera(struct camera *camera, const coord d_x, const coord d_y)
{
	if (!camera)
	{
		INFO("Camera can't be NULL.");
		return;
	}
	set_camera(camera, (struct vec2) { camera->position.x + d_x, camera->position.y + d_y });
}

bool is_visible(const struct camera *camera, const struct vec2 *const position, const int w, const int h)
{
	if(position->x < camera->position.x+CENTER_X+CENTER_X && 
	   position->x > camera->position.x+CENTER_X-CENTER_X-w &&
	   position->y < camera->position.y+CENTER_Y+CENTER_Y && 
	   position->y > camera->position.y+CENTER_Y-CENTER_Y-h)
		return true;
	return false;
}
