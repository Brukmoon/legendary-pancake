/*
 * Dead Zone game engine.
 *
 * @author Michal H.
 * Welcome to my personal hell.
 *
 **/
// Suppress warning: unused parameter.
#define UNUSED_PARAMETER(p) (void)p

#include <stdlib.h>
#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _MSC_VER

#include "config.h"
#include "game.h"

/*
 * Warning: SDL defines its own SDL_Main function which performs some initialization
 * prior to SDL_Init. Why? Portability?
 * I have no idea. But if you have two libraries that do this, you have a big 
 * problem. Refer to SDL_main.c if you run into multiple definitions errors.
 *
 */
int main(int argc, char* argv[]) {
	// Unused for now. Suppress warning.
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
	
	// Overflow @ 2^32-1 is safe. Happens in ~50 days.
	Uint32 time = SDL_GetTicks();
	// Game loop: process events -> update if not paused -> render to screen -> sync FPS
	while (game_running(&game)) {
		game.process_input(&game);
		if (!game.paused)
			game.update(&game);
		if (time > SDL_GetTicks()) // rendering rate can't exceed logic rate
			game.draw(game.screen.renderer);
		Sint32 delay = time - SDL_GetTicks();
		if (delay > 0)
			SDL_Delay(delay);
		time += 1000 / FPS; // frames per 1000 MS (1s) --> FPS
	}
	game.clean(&game);
#ifdef _MSC_VER
	/* 
	 * Memory leak detection --> works only with VS libraries, implicit output to Debug window.
	 * (Debug -> Windows -> Output)
	 * To redirect the output, refer to _CrtSetReportMode.
	 */
	_CrtDumpMemoryLeaks();
#endif // _MSC_VER
	return EXIT_SUCCESS;
}