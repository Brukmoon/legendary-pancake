#include <SDL_mixer.h>
#include <stdlib.h>

#include "actor.h"
#include "text.h"
#include "camera.h"
#include "config.h"
#include "common.h"
#include "render.h"
#include "game.h"
#include "input.h"
#include "level.h"
#include "sound.h"
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
		INFO("SDL activated.");
		game->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, game->screen.width, game->screen.height, SDL_WINDOW_SHOWN);
		if (game->window)
		{
			INFO("Window activated.");
			// Renderer with HW acceleration enabled.
			game->screen.renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
			if (game->screen.renderer)
			{
				INFO("Renderer activated.");
				// Stereo, high frequency, low latency
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != -1)
				{
					INFO("Sound system activated.");
					if (fonts_init(10)) {
						INFO("Text engine activated.");
						INFO("> Game initialization sequence finished.\n");
						game_set_state(game, MENU);
						SDL_ShowCursor(0);
						return true;
					}
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
	menu_destroy();
	// destroy SDL objects
	SDL_DestroyRenderer(game->screen.renderer);
	SDL_DestroyWindow(game->window);
	game->screen.renderer = NULL;
	game->window = NULL;
	// destroy text resources
	fonts_destroy();
	// destroy sound
	audio_destroy();
	// exit subsystems	
	Mix_CloseAudio();
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

bool game_pause(struct game *game) 
{ 
	return game_set_pause(game, !game->paused); 
}

bool game_set_pause(struct game *game, bool yesno) 
{
	music_set_pause(yesno);
	return game->paused = yesno; 
}

void to_play_state(struct game *game)
{
	INFO("Game started.\n");
	level_load(1, game->screen.renderer); // load level 1
	// set callbacks to play state callbacks
	game->update = update_play;
	game->draw = render_play;
	game->process_input = process_input_play;
	music_add("music", ".ogg");
	sound_add("jump", ".wav");
	sound_add("death", ".wav");
	sound_add("fall", ".wav");
	music_play("music", 6000);
	camera_init(&g_camera, CAMERA_FIXED);
	player_init(&g_player, game->screen.renderer);
}

void to_menu_state(struct game *game)
{
	INFO("Menu opened.\n");
	// set callbacks to menu state callbacks
	game->update = update_menu;
	game->draw = render_menu;
	game->process_input = process_input_menu;
	menu_load(game->screen.renderer);
	music_add("menu", ".ogg");
	sound_add("accept", ".wav");
	sound_add("select", ".wav");
	music_play("menu", 4000);
}

void to_edit_state(struct game *game)
{
	INFO("Editor opened.\n");
#if PYTHON_ON
	system("python ./assets/gen_empty_map.py");
#endif // PYTHON_ON
	//if (!g_level) // level not yet initialized
		level_load(0, game->screen.renderer);
	// set callbacks to menu state callbacks
	game->update = update_edit;
	game->draw = render_edit;
	game->process_input = process_input_edit;
    player_set_vel_x(&g_player, 0);
	player_set_vel_y(&g_player, 0);
	SDL_ShowCursor(1);
	camera_init(&g_camera, CAMERA_FREE);
	player_init(&g_player, game->screen.renderer);
}