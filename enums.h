#pragma once
#include <SDL.h>
#include <vector>
#include <bass.h>

namespace enums 
{
	enum gameStates {
		STARTING_SCREEN,
		MUSIC_SELECTION,
		MAIN_GAME,
		TOTAL_STATES
	};

	enum motions {
		LINEAR_SLIDE,
		HALF_SINE_SLIDE,
		FULL_SINE_SLIDE
		//https ://www.wolframalpha.com/input/?i=plot+y+%3D+0.5(sin(x-pi%2F2)+%2B+1)
	};

	enum noteType {
		SINGLE_HIT,
		SINGLE_HOLD
	};

	enum noteHit {
		NO_HIT,
		PERFECT,
		OKAY,
		MISS
	};
}

struct Instruction {
	int nextState;
	bool quit = false;
	std::string songToLoad;
	int gameKeys = 4;
	std::string songDifficulty;
};

struct InitVariables {
	int screen_height;
	int screen_width;
	int screen_fps;
	int startup_screen_delay;
	int maingame_startup_fadein_time;
	int mainGame_bg_alpha;
	int mainGame_ui_transition_time;
	int timeBar_thickness;
	float timeBar_position;
	int keySeparation_thickness;
	Uint8 path_highlight_alpha;
	float note_radius_ratio;
	double note_buffer_time;
	std::vector<int> keyBinds;
	double okay_hit_buffer_time;
	double perfect_hit_buffer_time;
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
	float amplitude;
	//Don't forget to update parsing function in BeatMap.cpp when you add more variables here!
};

struct BeatNote {
	enums::noteType note_type;
	double start_position;
	double end_position;

};

//To be used exclusively by BeatMap to keep track of key presses
struct KeyStatus {
	int key;
	bool already_pressed;
	int linked_path;
};
