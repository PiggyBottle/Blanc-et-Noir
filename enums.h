#pragma once
#include <SDL.h>
#include <vector>
#include <bass.h>
#include <list>

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
		PERFECT,
		OKAY,
		BREAK,
		NO_HIT
	};

	enum beatMapDifficulty {
		EASY,
		NORMAL,
		HARD,
		EXTREME
	};

	enum gameKeys {
		FOUR_KEYS,
		SIX_KEYS,
		TOTAL_KEYS
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
	float musicSelection_bar_minimized_x;
	float musicSelection_bar_maximized_x;
	double musicSelection_bar_transition_time;
	float musicSelection_panel_width;
	float musicSelection_panel_separation;
	std::string musicSelection_panel_font;
	int musicSelection_bar_thickness;
	int maingame_startup_fadein_time;
	int mainGame_bg_alpha;
	int mainGame_ui_transition_time;
	int timeBar_thickness;
	float timeBar_position;
	std::string combo_font;
	std::string note_hit_font;
	double combo_and_note_hit_update_buffer;
	float combo_and_note_hit_separator;
	double combo_and_note_hit_animation_time;
	float combo_max_height;
	float note_hit_max_height;
	//The amount to raise the combo square by so that it isn't too far from the notehit indicator
	float combo_offset_height;
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

//To be used exclusively for MainGame to keep track of combo/notehit blits
struct MostRecentNoteHit {
	bool comboHasBeenUpdated;
	enums::noteHit hit;
	double comboUpdateTime;
	int combo;
};

struct TextureWithVariables {
	SDL_Texture *texture;
	int height;
	int width;
};

struct BeatMapKeyAndDifficulty {
	int numberOfKeys;
	enums::beatMapDifficulty difficulty;
};

//For MusicSelection
struct MusicFileSystem {
	std::string songName;
	std::string beatMapRootFolder;
	std::vector<BeatMapKeyAndDifficulty> difficultyAndKeys;
};

struct MusicSelectionPanel {
	TextureWithVariables songTitleTexture;
	int musicIndex;
	int centerX;
};

struct ListOfPanels {
	std::list<MusicSelectionPanel> panels;
	//Relative to beatMapsWithKeys
	int front;
	//Relative to beatMapsWithKeys
	int back;
};
