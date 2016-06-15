#pragma once

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
}

struct Instruction {
	int nextState;
	bool quit = false;
	enums::songs songToLoad = enums::NO_SONGS;
};