#include <SDL.h>
#include <iostream>
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
	var.screen_fps = 600;
	var.startup_screen_delay = 2000;
	//MusicSelection variables
	{
		var.musicSelection_bar_thickness = 4;
		var.musicSelection_bar_maximized_x = 0.4f;
		var.musicSelection_bar_minimized_x = 0.1f;
		var.musicSelection_bar_transition_time = 0.1;
		var.musicSelection_panel_width = 0.8f;
		var.musicSelection_panel_separation = 0.1f;
		var.musicSelection_panel_font = "Noto.otf";
	}
	//MainGame variables
	{
		var.maingame_startup_fadein_time = 1000;
		var.mainGame_bg_alpha = 255;
		var.mainGame_ui_transition_time = 500;
		var.timeBar_thickness = 2;
		var.timeBar_position = 1.f / 12.f;
		var.combo_font = "Noto.otf";
		var.note_hit_font = "Noto.otf";
		var.combo_and_note_hit_update_buffer = 0.5;
		var.combo_and_note_hit_separator = 0.2f;
		var.combo_and_note_hit_animation_time = 0.1;
		var.combo_max_height = 0.30f;
		var.note_hit_max_height = 0.1f;
		var.combo_offset_height = (float)(18.0 / 310.0);
		var.keySeparation_thickness = 2;
		var.path_highlight_alpha = (Uint8)155;
		var.note_radius_ratio = (float)(1.0 / 40.0);
		//Only notes within this buffer time will be blitted
		var.note_buffer_time = (double)1.0;
		//Notes later than this buffer time will count as a break
		var.keyBinds = { SDLK_s,SDLK_d,SDLK_f,SDLK_j,SDLK_k,SDLK_l };
		var.okay_hit_buffer_time = 0.3;
		var.perfect_hit_buffer_time = 0.1;
	}

	Game game = Game(var);

	if (game.init()) {
		std::cout << "initialized successfully" << std::endl;
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