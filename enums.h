#pragma once
#include <SDL.h>
#include <vector>
#include <bass.h>


namespace enums 
{
	enum gameStates {
		STARTING_SCREEN,
		MAIN_MENU,
		MAIN_GAME,
		TOTAL_STATES
	};

	enum songs {
		UNRAVEL,
		FOR_SEASONS,
		TOTAL_SONGS,
		NO_SONGS
	};

	enum motions {
		LINEAR_SLIDE
	};

	enum noteType {
		SINGLE_HIT
	};
}

struct Instruction {
	int nextState;
	bool quit = false;
	enums::songs songToLoad = enums::NO_SONGS;
};

struct InitVariables {
	int screen_height;
	int screen_width;
	int screen_fps;
	int startup_screen_delay;
	int maingame_startup_fadein_time;
	int mainGame_bg_alpha;
	int mainGame_ui_transition_time;
	float path_width_ratio;
	Uint8 path_highlight_alpha;
	float note_radius_ratio;
};

struct RGB {
	int r;
	int g;
	int b;
};

struct StartEnd {
	std::vector<double> start;
	std::vector<double> end;
};

struct PathMotion {
	enums::motions motion;
	double start_position;
	double end_position;
	float start_x;
	float end_x;
};

struct BeatNote {
	enums::noteType note_type;
	double start_position;
	double end_position;

};
