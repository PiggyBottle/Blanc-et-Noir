#pragma once
#include "BeatPath.h"
#include <SDL.h>
#include <bass.h>


class BeatMap
{
public:
	BeatMap(SDL_Renderer *r, InitVariables var, int number_of_keys);
	BeatMap();
	~BeatMap();
	void render(Uint32 currentTick, double currentMusicPosition, int timeBarY);
private:
	SDL_Renderer *Renderer;
	std::vector<BeatPath> beatPath, getBeatPath();
	InitVariables initVariables;
	int SCREEN_HEIGHT, SCREEN_WIDTH, numberOfKeys, keySeparationThickness;
	std::vector<int> keyCoordinates;
	double beatNoteBufferTime;
	void generateKeyCoordinates();
	
};

