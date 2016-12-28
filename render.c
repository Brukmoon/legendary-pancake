#include "camera.h"
#include "common.h"
#include "graphics.h"
#include "game.h"
#include "level.h"
#include "menu.h"
#include "player.h"
#include "render.h"
#ifdef _DEBUG
#include "text.h"
#endif // _DEBUG

#ifndef BLANK_TILE
#define BLANK_TILE 0
#endif // BLANK_TILE

#define TILE_HEIGHT g_level->tile_map.tile_height
#define TILE_WIDTH g_level->tile_map.tile_width

#define MAP_NOT_OVERFLOW (y>=0&&y<=g_level->tile_map.height&&x>=0&&x<=g_level->tile_map.width)

static void render_grid(SDL_Renderer *const renderer, const SDL_Color color);

void render_map(SDL_Renderer* const renderer)
{
	SDL_RenderCopy(renderer, g_level->background, NULL, NULL);
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
			}
		}
}

void render_grid(SDL_Renderer *const renderer, const SDL_Color color)
{
	int start_y = (g_camera.position.y) / TILE_HEIGHT, start_x = (g_camera.position.x) / TILE_WIDTH;
	for (int y = start_y; y < g_level->tile_map.height; ++y)
		for (int x = start_x; x < g_level->tile_map.width; ++x)
		{
			if (!is_visible(&g_camera, &(const vec2) { x*TILE_WIDTH, y*TILE_HEIGHT }, TILE_WIDTH, TILE_HEIGHT))
				break;
			if (MAP_NOT_OVERFLOW)
			{
				hollow_rect(renderer, x*TILE_WIDTH - (g_camera.position.x), y*TILE_HEIGHT - (g_camera.position.y), TILE_WIDTH, TILE_HEIGHT, color);
			} 
		}
}

#undef MAP_NOT_OVERFLOW
#undef TILE_HEIGHT
#undef TILE_WIDTH

#ifdef _DEBUG

static void render_debug_console(SDL_Renderer* const renderer)
{
	char buffer[256];
	sprintf_s(buffer, 256, "Player coordinates: [%d;%d]", g_player.skeleton.x, g_player.skeleton.y);
	SDL_Texture *p_coord = create_text_texture(renderer, buffer, 12, (SDL_Color) { 0, 0, 0 });
	SDL_Rect rect;
	rect.x = rect.y = 0;
	SDL_QueryTexture(p_coord, NULL, NULL, &rect.w, &rect.h);
	SDL_RenderCopy(renderer, p_coord, NULL, &rect);
	SDL_DestroyTexture(p_coord);
}
#endif // _DEBUG

void render_menu_interface(SDL_Renderer* const renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 170, 170, 1);
	static SDL_Rect rect = { 100, 50, 1800, 900 };
	static bool direction = 0;
	if (direction)
		rect.x++;
	else
		rect.x--;
	if (rect.x > 1800 || rect.x < 0)
		direction = !direction;
	SDL_RenderCopy(renderer, curr_menu->background, &rect, NULL);
	draw_menu(renderer);
}

static void draw_player_info(const struct actor *player, SDL_Renderer *renderer)
{
	fill_rect(renderer, SCREEN_WIDTH - 105, 9, 100, 11, (SDL_Color) { 255, 0, 0, 1 });
	hollow_rect(renderer, SCREEN_WIDTH - 105, 9, 100, 11, (SDL_Color) { 0, 0, 0, 1 });
	if (player->jump_count < 2)
		fill_rect(renderer, SCREEN_WIDTH - 110, 5, 5, 10, (SDL_Color) { 255, 255, 0, 1 });
	if (player->jump_count < 1)
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
	render_map(renderer);
#ifdef _DEBUG
	render_grid(renderer, (SDL_Color) { 214, 214, 214, 1 });
	render_debug_console(renderer);
#endif // _DEBUG
	draw_actor(&g_player, renderer);
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
	render_grid(renderer, (SDL_Color) { 0, 0, 0, 1 });
	SDL_RenderPresent(renderer);
}