/*
 * Game loop.
 * @author Michal H.
 *
 **/
#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <stdbool.h>

struct game;
enum game_state
{
	NONE = -1, // NOT INIT
	EXIT,      // program exiting
	MENU,      // in-menu
	PLAY,      // in-game
	EDIT       // level editor
};

// game loop callbacks
bool g_init(struct game* game);
void g_clean(struct game* game);

struct game
{
	enum game_state state;
	SDL_Window* window;
	struct
	{
		int width;
		int height;
		SDL_Renderer* renderer;
	} screen;

	bool(*init)(struct game*);
	void(*clean)(struct game*);
	void(*process_input)(struct game*);
	void(*update)(void);
	void(*draw)(SDL_Renderer*);
};

// Is the game running?
inline bool game_running(const struct game *game) { return game->state; }
// Set current game state.
void game_set_state(struct game* game, const enum game_state state);

#endif // GAME_H
