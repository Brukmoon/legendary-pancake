/*
 * Core of the game engine. State machine and callbacks.
 *
 * @author Michal H.
 *
 * pseudocode:
 * game.init
 * while(game_running)
 *     game.draw
 *     game.process_input
 *     game.update
 * game.clean
 *
 **/
#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <stdbool.h>

struct game;
// State machine.
enum game_state
{
	NONE = -1, // NOT INIT
	EXIT,      // program exiting
	MENU,      // in-menu
	PLAY,      // in-game
	EDIT       // level editor
};

// Static game callbacks.
// Initialize. Must be called exactly once, after declaration of game state machine.
bool g_init(struct game* game);
// Clean the resources. After calling this function, calling any engine function will probably result in an error. 
void g_clean(struct game* game);

// Game state machine.
// There should be a single instance declared at the beginning of the program.
struct game
{
	// State should be handled by game_set_state.
	enum game_state state;
	bool paused;
	SDL_Window* window;
	struct
	{
		size_t width, height; // Width and height of the screen.
		SDL_Renderer* renderer; // Rendering target.
	} screen;

	// Game state callbacks.
	bool(*init)(struct game*);
	void(*draw)(SDL_Renderer*);
	void(*process_input)(struct game*);
	void(*update)(void);
	void(*clean)(struct game*);
};

// Is the game running?
inline bool game_running(const struct game *game) { return game->state; }
// Set game paused.
bool game_set_pause(struct game *game, bool yesno);
bool game_pause(struct game *game);
// Set game state.
void game_set_state(struct game* game, const enum game_state state);

#endif // GAME_H
