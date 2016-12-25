/*
 * Dead Zone game engine.
 * @author Michal H.
 *
 **/
#define UNUSED_PARAMETER(p) (void)p
#include <stdlib.h>
#include "config.h"
#include "game.h"
//#include "test.h"

// framerate limited to 60 FPS
#define FPS 1

int main(int argc, char* argv[]) {
	UNUSED_PARAMETER(argc);
	UNUSED_PARAMETER(argv);
	// Global game state
	struct game game = {
		NONE, // RUNNING?
		false, // paused?
		NULL, // window not yet created
		{
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
		NULL // renderer not yet created
		},
		// callbacks
		g_init,  // initialization function, must be called before entering the game loop
		// draw, events, update
		NULL,
		NULL,
		NULL,
		g_clean // cleaner function, should be made an atexit callback or called every time the program exits
	};
	if (!game.init(&game))
		return EXIT_FAILURE;
	double time = (double)SDL_GetTicks();
	// game loop
	while (game_running(&game)) {
		if (time > SDL_GetTicks()) // rendering rate can't exceed logic rate
			game.draw(game.screen.renderer);
		game.process_input(&game);
		if(!game.paused)
			game.update();
		int delay = (int)(time - SDL_GetTicks());
		if (delay > 0)
			SDL_Delay(delay);
		time += 1000.0 / FPS; // frames per 1000 MS (1s) --> FPS
	}
	game.clean(&game);
	return EXIT_SUCCESS;
}