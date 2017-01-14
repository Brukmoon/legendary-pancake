/*
 * Dead Zone game engine.
 * WELCOME TO HELL!
 * @author Michal H.
 *
 **/
#define UNUSED_PARAMETER(p) (void)p
#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _MSC_VER
#include <stdlib.h>

#include "config.h"
#include "game.h"
//#include "test.h"
#include "sound.h"

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
	
	Uint32 time = SDL_GetTicks();
	// game loop
	while (game_running(&game)) {
		game.process_input(&game);
		if (!game.paused)
			game.update();
		if (time > SDL_GetTicks()) // rendering rate can't exceed logic rate
			game.draw(game.screen.renderer);
		int delay = time - SDL_GetTicks();
		if (delay > 0)
			SDL_Delay(delay);
		time += 1000 / FPS; // frames per 1000 MS (1s) --> FPS
	}
	game.clean(&game);
	// Memory leak detection --> work only in VS IDE, implicit output to Debug window.
#ifdef _MSC_VER
	_CrtDumpMemoryLeaks();
#endif // _MSC_VER
	return EXIT_SUCCESS;
}