#include "text.h"
#include "camera.h"
#include "config.h"
#include "common.h"
#include "render.h"
#include "game.h"
#include "input.h"
#include "level.h"
#include "player.h"
#include "menu.h"

// Outputs SDL version info.
static void SDL_version_info(void);
// State transitions.
static void to_play_state(struct game* game);
static void to_menu_state(struct game* game);
static void to_edit_state(struct game* game);

bool g_init(struct game *game)
{
	INFO("Startup: %s\nVersion: %s\nAuthor: Brukmoon\n", GAME_NAME, GAME_VERSION);
	INFO("< Game initialization sequence started.");
	SDL_version_info();
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		INFO("SDL successfully initialized.");
		game->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, game->screen.width, game->screen.height, SDL_WINDOW_SHOWN);
		if (game->window)
		{
			INFO("Window spawned.");
			// Renderer with HW acceleration enabled.
			game->screen.renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
			if (game->screen.renderer)
			{
				INFO("Renderer created.");
				if (init_fonts(10)) {
					INFO("Text engine initialized.");
					INFO("> Game initialization sequence finished.\n");
					game_set_state(game, MENU);
					SDL_ShowCursor(0);
					return true;
				}
			}
		}
	}
	// shouldn't get here unless SDL errors
	ERROR("SDL wasn't initialized!");
	return false;
}

void g_clean(struct game *game)
{
	// cleanup, must be called @ exit
	INFO("Cleanup sequence started.");
	// remove level data
	level_clean(&g_level);
	destroy_menu();
	// destroy SDL objects
	SDL_DestroyRenderer(game->screen.renderer);
	SDL_DestroyWindow(game->window);
	game->screen.renderer = NULL;
	game->window = NULL;
	// destroy text resources
	destroy_fonts();
	// exit subsystems	
	TTF_Quit();
	SDL_Quit();
	INFO("Cleanup sequence finished.");
}

void SDL_version_info(void)
{
	SDL_version SDL_compiled, TTF_compiled, SDL_linked;
	const SDL_version *TTF_linked = TTF_Linked_Version();
	SDL_VERSION(&SDL_compiled);
	SDL_TTF_VERSION(&TTF_compiled);
	SDL_GetVersion(&SDL_linked);
	INFO("SDL compiled version: %d.%d.%d",
		SDL_compiled.major, SDL_compiled.minor, SDL_compiled.patch);
	INFO("SDL_TTF compiled version: %d.%d.%d",
		TTF_compiled.major, TTF_compiled.minor, TTF_compiled.patch);
	INFO("SDL linked version: %d.%d.%d",
		SDL_linked.major, SDL_linked.minor, SDL_linked.patch);
	INFO("SDL_TTF linked version: %d.%d.%d",
		TTF_linked->major, TTF_linked->minor, TTF_linked->patch);
}

void game_set_state(struct game *game, const enum game_state state)
{
	switch (state) // handle transitions
	{
	case MENU:
		to_menu_state(game);
		break;
	case PLAY:
		to_play_state(game);
		break;
	case EDIT:
		to_edit_state(game);
	case EXIT:
		// no transition for EXIT state
		break;
	default:
		ERROR("Unknown state %d.", state);
		break;
	}
	game->state = state;
}

void to_play_state(struct game *game)
{
	INFO("Game started.\n");
	if (!g_level) // level not yet initialized
		level_load(1, game->screen.renderer);
	// set callbacks to play state callbacks
	game->update = update_play;
	game->draw = render_play;
	game->process_input = process_input_play;
	init_camera(&g_camera);
	init_actor(&g_player, game->screen.renderer);
}

void to_menu_state(struct game *game)
{
	INFO("Menu opened.\n");
	// set callbacks to menu state callbacks
	game->update = update_menu;
	game->draw = render_menu;
	game->process_input = process_input_menu;
	load_menu(game->screen.renderer);
}

void to_edit_state(struct game *game)
{
	INFO("Editor opened.\n");
	if (!g_level) // level not yet initialized
		level_load(1, game->screen.renderer);
	// set callbacks to menu state callbacks
	game->update = update_edit;
	game->draw = render_edit;
	game->process_input = process_input_edit;
	g_player.velocity.y = g_player.velocity.x = 0;
	SDL_ShowCursor(1);
	init_camera(&g_camera);
	init_actor(&g_player, game->screen.renderer);
}