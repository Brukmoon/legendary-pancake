#include "actor.h"
#include "camera.h"
#include "common.h"
#include "config.h"
#include "graphics.h"
#include "game.h"
#include "level.h"
#include "menu.h"
#include "render.h"
#if SHOW_CONSOLE
#include "text.h"
#endif // _DEBUG

#ifndef BLANK_TILE
#define BLANK_TILE 0
#endif // BLANK_TILE

#define TILE_HEIGHT g_level->tile_map.tile_height
#define TILE_WIDTH g_level->tile_map.tile_width
#define COLL_RECT_SIZE TILE_WIDTH/4

static void render_map_grid(SDL_Renderer *const renderer);

#define MAP_NOT_OVERFLOW (y>=0&&y<=g_level->tile_map.height&&x>=0&&x<=g_level->tile_map.width)

void render_map(SDL_Renderer* const renderer, const enum render_map_flags f)
{
	//Render background
	SDL_RenderCopy(renderer, g_level->background, NULL, NULL);

	static float scrollingOffset = 0;
	scrollingOffset -= 0.5f;
	int w = 0, h = 0;
	SDL_QueryTexture(g_level->d_background, NULL, NULL, &w, &h);
	if ((int)scrollingOffset < -w)
	{
		scrollingOffset = 0;
	}
	SDL_Rect dst_rect = { (int)scrollingOffset, 0, w, h };
	SDL_RenderCopy(renderer, g_level->d_background, NULL, &dst_rect);
	dst_rect.x = (int)scrollingOffset + w;
	SDL_RenderCopy(renderer, g_level->d_background, NULL, &dst_rect);


	int start_y = g_camera.position.y/TILE_HEIGHT, start_x = g_camera.position.x/TILE_WIDTH;
	for (int y = start_y; y < g_level->tile_map.height; ++y)
		for (int x = start_x; x < g_level->tile_map.width; ++x)
		{
			if (!is_visible(&g_camera, &(const vec2) { x*TILE_WIDTH, y*TILE_HEIGHT }, TILE_WIDTH, TILE_HEIGHT))
				break;
			if (MAP_NOT_OVERFLOW)
			{
				if (g_level->tile_map.map[TMAP_TEXTURE_LAYER][y][x] != BLANK_TILE)
				{
					SDL_RenderCopy(renderer, g_level->tileset, &(const struct SDL_Rect){ (((int)g_level->tile_map.map[TMAP_TEXTURE_LAYER][y][x] % 16)-1)*TILE_WIDTH, ((int)g_level->tile_map.map[TMAP_TEXTURE_LAYER][y][x]/16)*TILE_HEIGHT,TILE_WIDTH, TILE_HEIGHT },
						&(const struct SDL_Rect){ x*TILE_WIDTH - (g_camera.position.x), y*TILE_HEIGHT - (g_camera.position.y), TILE_WIDTH, TILE_HEIGHT });
				}
				if (f & RENDER_COLL)
				{
					if (g_level->tile_map.map[TMAP_COLLISION_LAYER][y][x] == 1)
						fill_rect(renderer, (x + 1)*TILE_WIDTH - (g_camera.position.x) - COLL_RECT_SIZE, y*TILE_HEIGHT - (g_camera.position.y),
							COLL_RECT_SIZE, COLL_RECT_SIZE, (SDL_Color) {
						255, 0, 0, 1
					});
					else if (g_level->tile_map.map[TMAP_COLLISION_LAYER][y][x] == 2)
						fill_rect(renderer, (x + 1)*TILE_WIDTH - (g_camera.position.x) - COLL_RECT_SIZE, y*TILE_HEIGHT - (g_camera.position.y),
							COLL_RECT_SIZE, COLL_RECT_SIZE, (SDL_Color) {
						0, 255, 0, 1
					});
				}
			}
		}
	if (f & RENDER_GRID)
		render_map_grid(renderer);
}

#undef MAP_NOT_OVERFLOW
#undef TILE_HEIGHT
#undef TILE_WIDTH

#if SHOW_CONSOLE

static void render_debug_console(SDL_Renderer* const renderer)
{
	char buffer[50];
	// renderer
	SDL_RendererInfo rendererInfo;
	SDL_GetRendererInfo(renderer, &rendererInfo);
	sprintf_s(buffer, 50, "Graphics API: %s %d x %d", rendererInfo.name, SCREEN_WIDTH, SCREEN_HEIGHT);
	draw_text(buffer, 12, (SDL_Color) { 0, 0, 0 }, (vec2) { 0, 0 }, renderer);
	// coordinates
	sprintf_s(buffer, 50, "Player coordinates: [%d;%d]", g_player.actor.skeleton.x, g_player.actor.skeleton.y);
	draw_text(buffer, 12, (SDL_Color) { 0, 0, 0 }, (vec2) { 0, 12 }, renderer);
	// speed
	sprintf_s(buffer, 50, "Player speed: [%f;%f]", g_player.actor.velocity.x, g_player.actor.velocity.y);
	draw_text(buffer, 12, (SDL_Color) { 0, 0, 0 }, (vec2) { 0, 24 }, renderer);
	// state
	switch (g_player.actor.state)
	{
	case AIR:
		sprintf_s(buffer, 50, "AIR");
		break;
	case GROUND:
		sprintf_s(buffer, 50, "GROUND");
		break;
	case LADDER:
		sprintf_s(buffer, 50, "LADDER");
		break;
	default:
		sprintf_s(buffer, 50, "Unknown");
		break;
	}
	draw_text(buffer, 12, (SDL_Color) { 0, 0, 0 }, (vec2) { 0, 36 }, renderer);
}
#endif // _DEBUG

void render_menu_interface(SDL_Renderer* const renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
	menu_draw(g_menu, renderer);
}

static void draw_player_info(const struct player *player, SDL_Renderer *renderer)
{
	fill_rect(renderer, SCREEN_WIDTH - 105, 9, player->actor.hitpoints, 11, (SDL_Color) { 255, 0, 0, 1 });
	hollow_rect(renderer, SCREEN_WIDTH - 105, 9, 100, 11, (SDL_Color) { 0, 0, 0, 1 });
	if (player->actor.jump_count < 2)
		fill_rect(renderer, SCREEN_WIDTH - 110, 5, 5, 10, (SDL_Color) { 255, 255, 0, 1 });
	if (player->actor.jump_count < 1)
		fill_rect(renderer, SCREEN_WIDTH - 110, 15, 5, 10, (SDL_Color) { 255, 255, 0, 1 });
	hollow_rect(renderer, SCREEN_WIDTH - 110, 5, 5, 10, (SDL_Color) { 0, 0, 0, 1 });
	hollow_rect(renderer, SCREEN_WIDTH - 110, 15, 5, 10, (SDL_Color) { 0, 0, 0, 1 });

	SDL_Rect src_rect = (SDL_Rect) { 0, 0, 32, 32 };
	SDL_Rect dest_rect = (SDL_Rect) { SCREEN_WIDTH-135, 5, 20, 20 };
	SDL_RenderCopy(renderer, player->texture, &src_rect, &dest_rect);
}

void render_play(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
	SDL_RenderClear(renderer);
#if SHOW_GRID
	render_map(renderer, RENDER_GRID);
#else
	render_map(renderer, RENDER_BASE);
#endif // SHOW_GRID
#if SHOW_CONSOLE
	render_debug_console(renderer);
#endif // SHOW_CONSOLE
	player_draw(&g_player, renderer);
	draw_player_info(&g_player, renderer);
	SDL_RenderPresent(renderer);
}

void render_menu(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 112, 146, 190, 1);
	SDL_RenderClear(renderer);
	render_menu_interface(renderer);
	SDL_RenderPresent(renderer);
}

void render_edit(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
	SDL_RenderClear(renderer);
	render_map(renderer, RENDER_ALL);
	int x = 0, y = 0;
	SDL_GetMouseState(&x, &y);
	// cursor
	hollow_rect(renderer, x-(g_player.actor.skeleton.w)/2, y- (g_player.actor.skeleton.h) / 2, g_player.actor.skeleton.w, g_player.actor.skeleton.h,
		(SDL_Color) {
		255, 0, 0, 0
	});
	// current sprite
	SDL_RenderCopy(renderer, g_level->tileset, &(const struct SDL_Rect){ ((curr_sprite_num%16)-1)* g_level->tile_map.tile_width, (curr_sprite_num / 16)*g_level->tile_map.tile_height, g_level->tile_map.tile_width, g_level->tile_map.tile_height },
		&(const struct SDL_Rect){ x - (g_player.actor.skeleton.w) / 2, y - (g_player.actor.skeleton.h) / 2, g_level->tile_map.tile_width, g_level->tile_map.tile_height });
	// spawn
	hollow_rect(renderer, g_player.actor.spawn.x - g_camera.position.x,g_player.actor.spawn.y - g_camera.position.y, g_player.actor.skeleton.w, g_player.actor.skeleton.h,
		(SDL_Color) {
		0, 0, 255, 0
	});
	SDL_RenderPresent(renderer);
}

static void render_map_grid(SDL_Renderer *const renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
	for (int i = 0; i < g_level->tile_map.width+1; ++i)
		SDL_RenderDrawLine(renderer, i*g_level->tile_map.tile_width - (g_camera.position.x), 0 - (g_camera.position.y), i*g_level->tile_map.tile_width - (g_camera.position.x), g_level->tile_map.tile_height*g_level->tile_map.height - (g_camera.position.y));
	for (int i = 0; i < g_level->tile_map.height+1; ++i)
		SDL_RenderDrawLine(renderer, 0 - (g_camera.position.x), i*g_level->tile_map.tile_height - (g_camera.position.y), g_level->tile_map.tile_width*g_level->tile_map.width - (g_camera.position.x), i*g_level->tile_map.tile_height - (g_camera.position.y));
}