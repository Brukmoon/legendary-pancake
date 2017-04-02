#include <SDL_mixer.h>

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
#include "sprite.h"
#include "timer.h"
#include "menu.h"

// Outputs SDL version info.
static void SDL_version_info(void);
// Outputs state stack.
static void state_stack_print(const struct game *game);

// State transitions.
static bool to_play_state(SDL_Renderer* game, const char *level_name);
static void from_play_state(void);
static bool to_main_menu_state(SDL_Renderer* game, char*);
static void from_main_menu_state(void);
static bool to_edit_state(SDL_Renderer* game, const char* level_name);
static void from_edit_state(void);
// choose level
static bool to_preplay_state(SDL_Renderer *renderer, char* unused);
// choose mode
static bool to_preplay_mode_state(SDL_Renderer *renderer, char *unused);

static bool game_set_pause(struct game *game, bool yesno)
{
	// Also pause music.
	music_set_pause(yesno);
	return game->paused = yesno;
}

bool game_init(struct game* game, struct game_screen* screen)
{
	INFO("Startup: %s", GAME_NAME);
	INFO("Version: %s", GAME_VERSION);
	INFO("Author: Brukmoon\n");
	INFO("< Game initialization sequence started.");
	// Logs version info.
	SDL_version_info();
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		INFO("SDL activated.");
		screen->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen->width, screen->height, 
			SDL_WINDOW_SHOWN);
		if (screen->window)
		{
			INFO("Window activated.");
			SDL_DisplayMode mode;
			SDL_GetWindowDisplayMode(screen->window, &mode);
			INFO("Display mode: %d px %d px %d Hz", mode.w, mode.h, mode.refresh_rate);
			// Renderer with HW acceleration enabled.
			screen->renderer = SDL_CreateRenderer(screen->window, -1, SDL_RENDERER_ACCELERATED);
			if (screen->renderer)
			{
				INFO("Renderer activated.");
				// Stereo, high frequency, low latency.
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != -1)
				{
					INFO("Sound system activated.");
					if (fonts_init(FONT_BUFFER_SIZE)) {
						INFO("Text engine activated.");
						SDL_ShowCursor(0);
						INFO("> Game initialization sequence finished.\n");
						game_state_change(game, game_state_main_menu());
						timer_reset(&g_timer);
						// default mode is normal
						game_mode = MODE_NORMAL;
						return true;
					}
				}
			}
		}
	}
	// shouldn't get here unless SDL errors
	ERROR("SDL wasn't initialized. %s", SDL_GetError());
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
	// destroy textures and sprites
	sprites_destroy();
	// exit subsystems	
	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
	INFO("> Cleanup sequence finished.");
}

void SDL_version_info(void)
{
	// version is either compiled or linked. Compiled SHOULD match linked.
	SDL_version SDL_compiled, TTF_compiled, SDL_linked;
	// TTF is the font library.
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

static void state_stack_print(const struct game *game)
{
	// print current state stack
	struct game_state *iter = game->run;
	INFO("Current state stack:");
	INFO("[");
	while (iter)
	{
		INFO("%d", iter->id);
		iter = iter->next;
	}
	INFO("]");
}

bool game_state_change(struct game *game, struct game_state *new_state)
{
	// game->run != NULL
	if (game_running(game))
		new_state->next = game->run;
	game->run = new_state;
	state_stack_print(game);
	INFO("Game state changed to %d.", new_state->id);
	if (game->run->enter)
		if (!game->run->enter(game->screen.renderer, game->run->state_param))
			return false;
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
		// free the current state
		struct game_state *temp = game->run;
		// switch to next state
		game->run = game->run->next;
		free(temp);
		// if the next state is not null and has an enter transition function, call it
		if(game->run && game->run->enter)
			game->run->enter(game->screen.renderer, game->run->state_param);
	}
	else
		INFO("Can't exit when there is no current state.");
	// print the game stack
	state_stack_print(game);
}

void game_state_reset(struct game *game)
{
	INFO("< Exiting all states.");
	// clean up the state stack
	while (game->run->next)
		game_state_exit(game);
	INFO("> All states exited.");
}

bool game_pause(struct game *game) 
{ 
	INFO("Game paused: %s", game->paused ? "FALSE" : "TRUE");
	return game_set_pause(game, !game->paused); 
}

static bool to_play_state(SDL_Renderer *renderer, const char *level_name)
{
	INFO("< Start playing.");
	if (g_level)
		INFO("There is a level already. Using it.");
	else
	{
		// load level
		// init level data
		if (!level_load(level_name, renderer))
			return false;
		// music has format <level_name>_music.ogg
		size_t music_name_length = SDL_strlen(level_name) + SDL_strlen("_music") + 1;
		char *music_name = malloc(music_name_length);
		SDL_strlcpy(music_name, g_level->name, music_name_length);
		SDL_strlcat(music_name, "_music", music_name_length);
		music_add(music_name, ".ogg");
		music_play(music_name, 6000);
		free(music_name);
		// sounds required by the engine
		sound_add("jump", ".wav");
		sound_add("win", ".wav");
		sound_add("death", ".wav");
		sound_add("fall", ".wav");
		sound_add("shoot", ".wav");
		sound_add("pick", ".wav");
		// custom cursor
		texture_add(IMG_PATH"arrow.png", renderer);
		sprite_add("arrow", IMG_PATH"arrow.png", (SDL_Rect) { 0, 0, 32, 32 });
	}
	// window-system-specific cursor is hidden in the playstate
	SDL_ShowCursor(false);
	// camera is fixed @ player
	camera_init(&g_camera, CAMERA_FIXED);
	player_spawn(&g_player);
	INFO("> Play started.");
	return true;
}

static void from_play_state(void)
{
	/*
	* For now, we keep audio and graphics intact, because of frequent animation and sound reuse.
	* This can be changed easily, just call audio_destroy.
	*
	*/
	level_clean();
	player_destroy(&g_player);
}

static bool to_main_menu_state(SDL_Renderer *renderer, char* unused)
{
	// state contains no variable, this macro is here so no warning is shown
	UNUSED_PARAMETER(unused);
	INFO("< Opening main menu.");
	// set callbacks to menu state callbacks
	main_menu_load(renderer);
	music_add("menu", ".ogg");
	sound_add("accept", ".wav");
	sound_add("select", ".wav");
	music_play("menu", 4000);
	INFO("> Menu opened.");
	return true;
}

static void from_main_menu_state(void)
{
	menu_destroy();
}

static bool to_preedit_state(SDL_Renderer *renderer, char* unused)
{
	UNUSED_PARAMETER(unused);
	INFO("< Opening preedit menu.");
	// set callbacks to menu state callbacks
	preedit_menu_load(renderer);
	music_play("menu", 4000);
	INFO("> Preedit menu opened.");
	return true;
}

static bool to_preplay_state(SDL_Renderer *renderer, char* unused)
{
	UNUSED_PARAMETER(unused);
	INFO("< Opening preedit menu.");
	// set callbacks to menu state callbacks
	preplay_menu_load(renderer);
	music_play("menu", 4000);
	INFO("> Preedit menu opened.");
	return true;
}

static bool to_preplay_mode_state(SDL_Renderer *renderer, char *unused)
{
	UNUSED_PARAMETER(unused);
	INFO("< Opening preedit menu.");
	// set callbacks to menu state callbacks
	preplay_difficulty_menu_load(renderer);
	music_play("menu", 4000);
	INFO("> Preedit menu opened.");
	return true;
}

static bool to_edit_state(SDL_Renderer *renderer, const char *level_name)
{
	INFO("< Opening editor.");
	if (g_level)
	{
		INFO("There is an already opened level (%s). Using it.", g_level->name);
	}
	else
	{
		// no loaded level, load a new one
		if (!level_load(level_name, renderer))
			return false;
	}
	// we move the player, but he is invisible (not spawned). Default speed 0
    player_set_vel_x(&g_player, 0);
	player_set_vel_y(&g_player, 0);
	SDL_ShowCursor(true);
	music_add("menux", ".ogg");
	sound_add("click", ".wav");
	music_play("menux", 6000);
	camera_init(&g_camera, CAMERA_FREE);
	INFO("> Editor opened.");
	return true;
}

static void from_edit_state(void)
{
	level_clean();
	player_destroy(&g_player);
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

struct game_state *game_state_preplay(void)
{
	struct game_state *preplay = malloc(sizeof(struct game_state));

	preplay->id = GAME_STATE_PREPLAY;
	preplay->state_param = NULL;

	preplay->enter = to_preplay_state;
	preplay->exit = from_main_menu_state;

	preplay->draw = render_menu;
	preplay->process_input = process_input_preplay;
	preplay->update = update_menu;

	preplay->next = NULL;

	return preplay;
}

struct game_state *game_state_mode(char *level_name)
{
	struct game_state *play = malloc(sizeof(struct game_state));

	play->id = GAME_STATE_MODE;
	play->state_param = level_name;

	play->enter = to_preplay_mode_state;
	play->exit = from_main_menu_state;

	play->draw = render_menu;
	play->process_input = process_input_mode;
	play->update = update_menu;
	play->next = NULL;

	return play;
}

struct game_state *game_state_edit(char *level_name)
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