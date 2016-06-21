#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>
#include <bass.h>
#include "Game.h"


int main(int argc, char* args[]) {

	InitVariables var;
	var.screen_width = 1080;
	var.screen_height = 620;
	var.screen_fps = 60;
	var.startup_screen_delay = 2000;
	var.maingame_startup_fadein_time = 1000;
	var.mainGame_bg_alpha = 255;
	var.mainGame_ui_transition_time = 500;
	var.path_width_ratio = (float)(1.0 / 15.0);
	var.path_highlight_alpha = (Uint8)155;
	var.note_radius_ratio = (float)(1.0 / 60.0);
	//Only notes within this buffer time will be blitted
	var.note_buffer_time = (double)1.0;

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