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
static void to_play_state(SDL_Renderer* game, const char *level_name);
static void from_play_state(void);
static void to_main_menu_state(SDL_Renderer* game, char*);
static void from_main_menu_state(void);
static void to_edit_state(SDL_Renderer* game, const char* level_name);
static void from_edit_state(void);

static bool game_set_pause(struct game *game, bool yesno)
{
	music_set_pause(yesno);
	return game->paused = yesno;
}

bool game_init(struct game* game, struct game_screen* screen)
{
	INFO("Startup: %s\nVersion: %s\nAuthor: Brukmoon\n", GAME_NAME, GAME_VERSION);
	INFO("< Game initialization sequence started.");
	SDL_version_info();
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		INFO("SDL activated.");
		screen->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen->width, screen->height, SDL_WINDOW_SHOWN);
		if (screen->window)
		{
			INFO("Window activated.");
			// Renderer with HW acceleration enabled.
			screen->renderer = SDL_CreateRenderer(screen->window, -1, SDL_RENDERER_ACCELERATED);
			if (screen->renderer)
			{
				INFO("Renderer activated.");
				// Stereo, high frequency, low latency
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != -1)
				{
					INFO("Sound system activated.");
					if (fonts_init(FONT_BUFFER_SIZE)) {
						INFO("Text engine activated.");
						INFO("> Game initialization sequence finished.\n");
						game_state_change(game, game_state_main_menu());
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

void game_clean(struct game_screen *screen)
{
	// cleanup, must be called @ exit
	INFO("< Cleanup sequence started.");
	// destroy SDL objects
	SDL_DestroyRenderer(screen->renderer);
	SDL_DestroyWindow(screen->window);
	screen->renderer = NULL;
	screen->window = NULL;
	// destroy text resources
	fonts_destroy();
	// destroy sound
	audio_destroy();
	// exit subsystems	
	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
	INFO("> Cleanup sequence finished.");
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

bool game_state_change(struct game *game, struct game_state *new_state)
{
	INFO("Changing game state to %d.", new_state->id);
	// game->run != NULL
	if (game_running(game))
		new_state->next = game->run;
	game->run = new_state;
	if(game->run->enter)
		game->run->enter(game->screen.renderer, game->run->state_param);
	struct game_state *it = game->run;
	while (it)
	{
		printf("%d -> ", it->id);
		it = it->next;
	}
	return true;
}

void game_state_exit(struct game *game)
{
	if (game_running(game))
	{
		INFO("Exiting state %d.", game->run->id);
		// If the current state has a cleanup function, call it.
		if (game->run->exit)
			game->run->exit();
		struct game_state *temp = game->run;
		game->run = game->run->next;
		free(temp);
		if(game->run && game->run->enter)
			game->run->enter(game->screen.renderer, game->run->state_param);
	}
	else
		ERROR("Can't exit when there is no current state.");
}

void game_state_reset(struct game *game)
{
	while (game->run->next)
		game_state_exit(game);
}

bool game_pause(struct game *game) 
{ 
	INFO("Game paused: %s", game->paused ? "FALSE" : "TRUE");
	return game_set_pause(game, !game->paused); 
}


static void to_play_state(SDL_Renderer *renderer, const char *level_name)
{
	INFO("Game started.\n");
	if (g_level)
		INFO("There is a level already. Using it.");
	else
	{
		level_load(level_name, renderer);
		// set callbacks to play state callbacks
		music_add("music", ".ogg");
		sound_add("jump", ".wav");
		sound_add("death", ".wav");
		sound_add("fall", ".wav");
		music_play("music", 6000);
	}
	camera_init(&g_camera, CAMERA_FIXED);
	player_init(&g_player, renderer);
	player_spawn(&g_player);
}

static void from_play_state(void)
{
	level_clean();
}

static void to_main_menu_state(SDL_Renderer *renderer, char* unused)
{
	UNUSED_PARAMETER(unused);
	INFO("Menu opened.\n");
	// set callbacks to menu state callbacks
	main_menu_load(renderer);
	music_add("menu", ".ogg");
	sound_add("accept", ".wav");
	sound_add("select", ".wav");
	music_play("menu", 4000);
}

static void from_main_menu_state(void)
{
	menu_destroy();
}

static void to_preedit_state(SDL_Renderer *renderer, char* unused)
{
	UNUSED_PARAMETER(unused);
	INFO("Preedit menu opened.\n");
	// set callbacks to menu state callbacks
	preedit_menu_load(renderer);
	music_add("menu", ".ogg");
	sound_add("accept", ".wav");
	sound_add("select", ".wav");
	music_play("menu", 4000);
}

void to_edit_state(SDL_Renderer *renderer, const char *level_name)
{
	INFO("Editor opened.\n");
	if (g_level)
	{
		INFO("There is an already opened level (%s). Using it.", g_level->name);
	}
	else
	{
		level_load(level_name, renderer);
	}
	// set callbacks to menu state callbacks
    player_set_vel_x(&g_player, 0);
	player_set_vel_y(&g_player, 0);
	SDL_ShowCursor(1);
	sound_add("click", ".wav");
	camera_init(&g_camera, CAMERA_FREE);
	player_init(&g_player, renderer);
	player_spawn(&g_player);
}

static void from_edit_state(void)
{
	level_clean();
}


struct game_state *game_state_main_menu(void)
{
	struct game_state *menu = malloc(sizeof(struct game_state));

	menu->id = GAME_STATE_MAIN_MENU;
	menu->state_param = NULL;

	menu->enter = to_main_menu_state;
	menu->exit = from_main_menu_state;

	menu->draw = render_menu;
	menu->process_input = process_input_menu;
	menu->update = update_menu;
	menu->next = NULL;

	return menu;
}

struct game_state *game_state_play(char *level_name)
{
	struct game_state *play = malloc(sizeof(struct game_state));

	play->id = GAME_STATE_PLAY;
	play->state_param = level_name;

	play->enter = to_play_state;
	play->exit = from_play_state;

	play->draw = render_play;
	play->process_input = process_input_play;
	play->update = update_play;
	play->next = NULL;

	return play;
}

struct game_state *game_state_preedit(void)
{
	struct game_state *preedit = malloc(sizeof(struct game_state));

	preedit->id = GAME_STATE_PREEDIT;
	preedit->state_param = NULL;

	preedit->enter = to_preedit_state;
	preedit->exit = from_main_menu_state;

	preedit->draw = render_menu;
	preedit->process_input = process_input_preedit;
	preedit->update = update_menu;

	preedit->next = NULL;

	return preedit;
}

struct game_state *game_state_edit(const char *level_name)
{
	struct game_state *edit = malloc(sizeof(struct game_state));

	edit->id = GAME_STATE_EDIT;
	edit->state_param = level_name;

	edit->enter = to_edit_state;
	edit->exit = from_edit_state;

	edit->draw = render_edit;
	edit->process_input = process_input_edit;
	edit->update = update_edit;

	edit->next = NULL;

	return edit;
}