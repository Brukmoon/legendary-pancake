/*
 * Dead Zone game engine.
 *
 * @author Michal H.
 * Welcome to my personal hell.
 *
 **/
#include <stdlib.h>
#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _MSC_VER

#include "config.h"
#include "game.h"

#ifndef UNUSED_PARAMETER
 // Suppress warning: unused parameter.
#define UNUSED_PARAMETER(p) (void)p
#endif // UNUSED_PARAMETER

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
		false, // paused? 
		{
			NULL,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			NULL
		},
		NULL
	};

	if (!game_init(&game, &game.screen))
		return EXIT_FAILURE;
	
	// Overflow @ 2^32-1 is safe. Happens in ~50 days.
	Uint32 time = SDL_GetTicks();
	// Game loop: process events -> update if not paused -> render to screen -> sync FPS
	while (game_running(&game)) 
	{
		if (!game.run->process_input(&game))
			continue; // State changed, re-evaluate.
		game.run->update(&game);
		if (time > SDL_GetTicks()) // rendering rate can't exceed logic rate
			game.run->draw(game.screen.renderer);
		Sint32 delay = time - SDL_GetTicks();
		if (delay > 0)
			SDL_Delay(delay);
		time += 1000 / FPS; // frames per 1000 MS (1s) --> FPS
	}
	game_clean(&game.screen);
	
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