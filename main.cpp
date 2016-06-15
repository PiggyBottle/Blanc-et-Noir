#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>
#include <bass.h>
#include "Game.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int SCREEN_FPS = 60;

int main(int argc, char* args[]) {

	InitVariables var;
	var.screen_width = 1280;
	var.screen_height = 720;
	var.screen_fps = 60;
	var.startup_screen_delay = 1000;
	var.maingame_startup_fadein_time = 1000;
	var.mainGame_bg_alpha = 55;

	Game game = Game(var);

	if (game.init()) {
		printf("initialized successfully\n");
	}

	//Main loop flag
	bool quit = false;
	//Event handler
	SDL_Event event;
	//Custom event for NULL input
	SDL_Event no_event;
	no_event.type = SDL_USEREVENT;
	no_event.user.code = 2;
	no_event.user.data1 = NULL;
	no_event.user.data2 = NULL;

	//Game Loop
	while (!quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			//User requests quit
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			else
			{
				quit = game.newFrame(event);
				if (quit) { break; }
			}
		}
		if (!quit)
		{
			quit = game.newFrame(no_event);
		}
		
	}

	game.uninit();
	return 0;
}