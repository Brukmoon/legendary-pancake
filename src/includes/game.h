/*
 * Core of the game engine. State machine and callbacks.
 *
 * @author Michal H.
 *
 * game.init
 *	game.draw
 *	game.process_input
 *	game.update
 * game.clean
 *
 **/
#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <stdbool.h>

struct game;

struct game_screen
{
	SDL_Window* window;
	size_t width, height; // Width and height of the screen.
	SDL_Renderer* renderer; // Rendering target.
};

// Game state machine.
struct game
{
	bool paused;
	struct game_screen screen;
	struct game_state *run;
};

enum game_mode
{
	MODE_NORMAL,
	MODE_EXTERMINATION,
	MODE_PACIFIC,
	MODE_REPLAY
} game_mode;

// Initialize. Must be called exactly once, after declaration of game state machine.
bool game_init(struct game* game, struct game_screen* screen);
// Clean the resources. After calling this function, calling any engine function will probably result in an error. 
void game_clean(struct game_screen *screen);
// Is the game running?
bool game_running(const struct game *game); 
// Set game paused.
bool game_pause(struct game *game);

enum game_state_id
{
	GAME_STATE_MAIN_MENU,
	GAME_STATE_PLAY,
	GAME_STATE_PREEDIT,
	GAME_STATE_PREPLAY,
	GAME_STATE_MODE,
	GAME_STATE_EDIT,
	GAME_STATE_REPLAY,
	GAME_STATE_COUNT
};

struct game_state
{
	enum game_state_id id;
	void* state_param;
	// Enter state.
	bool(*enter)(SDL_Renderer*, void*);
	// State clean-up.
	void(*exit)();

	// Game state callbacks.
	void(*draw)(SDL_Renderer*);
	bool(*process_input)(struct game*);
	void(*update)(struct game*);
	// Next game state.
	struct game_state* next;
};

struct game_state *game_state_main_menu(void);
struct game_state *game_state_play(char* level_name);
struct game_state *game_state_edit(char* level_name);
struct game_state *game_state_preedit(void);
struct game_state *game_state_preplay(void);
struct game_state *game_state_mode(char *level_name);
struct game_state *game_state_replay(char *replay_name);

// add a new state to the stack and set it as current
bool game_state_change(struct game *game, struct game_state *new_state);
// exit current state
void game_state_exit(struct game *game);
// empty the state stack
void game_state_reset(struct game *game);

// Texture type currently selected. Needed by both the event handler and renderer.
extern int curr_sprite_num;

#endif // GAME_H
