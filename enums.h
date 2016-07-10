#pragma once
#include <SDL.h>
#include <vector>
#include <bass.h>
#include <functional>
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
		EXTREME,
		TOTAL_DIFFICULTIES
	};
	//Used within MusicSelection exclusively at the moment
	enum gameKeys {
		FOUR_KEYS,
		SIX_KEYS,
		TOTAL_KEYS
	};
}

struct Instruction {
	int nextState;
	bool quit = false;
	std::string beatMapSongToLoad;
	int gameKeys = 4;
	std::string songDifficulty;
	std::string beatMapRootFolder;
	std::string beatMapBGtoLoad;
};

struct RGB {
	int r;
	int g;
	int b;
};

struct InitVariables {
	int screen_height;
	int screen_width;
	int screen_fps;
	int startup_screen_delay;
	float musicSelection_albumArt_x;
	//Also used as anchor for difficulty buttons
	float musicSelection_albumArt_y;
	//With respect to screen width
	float musicSelection_albumArt_width_and_height;
	//With respect to album art height;
	float musicSelection_albumArt_errorText_height;
	float musicSelection_albumArt_shadow;
	float musicSelection_bar_minimized_x;
	float musicSelection_bar_maximized_x;
	double musicSelection_bar_transition_time;
	float musicSelection_panel_width;
	float musicSelection_panel_separation;
	std::string musicSelection_font;
	int musicSelection_panel_thickness;
	float musicSelection_panel_text_right_padding;
	int musicSelection_bar_thickness;
	float musicSelection_startGame_button_x;
	float musicSelection_startGame_button_y;
	float musicSelection_startGame_button_width;
	float musicSelection_startGame_button_height;
	float musicSelection_gameKeys_button_x;
	float musicSelection_gameKeys_button_y;
	float musicSelection_gameKeys_button_height;
	float musicSelection_gameKeys_button_width;
	float musicSelection_gameKeys_button_separation;
	float musicSelection_difficulty_button_x;
	float musicSelection_difficulty_button_width;
	float musicSelection_difficulty_button_height;
	float musicSelection_difficulty_button_shadow;
	RGB musicSelection_color_easy;
	RGB musicSelection_color_normal;
	RGB musicSelection_color_hard;
	RGB musicSelection_color_extreme;
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

struct StartEnd {
	std::vector<double> start;
	std::vector<double> end;
};

struct ColorMotion {
	enums::motions motion;
	double start_position;
	double end_position;
	RGB startColor;
	RGB endColor;
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
	std::string bgFileName;
	std::string musicFileName;
	std::string albumArtFileName;
	double musicStartPosition;
	double musicEndPosition;
	std::string beatMapRootFolder;
	std::vector<BeatMapKeyAndDifficulty> difficultyAndKeys;
};

struct MusicSelectionPanel {
	TextureWithVariables songTitleTexture;
	int musicIndex;
	int centerY;
	//Used when dragging panels
	int previousCenterY;
	int width;
};

struct ListOfPanels {
	std::list<MusicSelectionPanel> panels;
	//Relative to beatMapsWithKeys
	int front;
	//Relative to beatMapsWithKeys
	int back;
};

struct MusicSelectionClickableButton {
	int x;
	int y;
	int width;
	int height;
	std::string text;
	TextureWithVariables backgroundTexture;
	TextureWithVariables fontTexture;
};
