#pragma once
#include "BeatPath.h"
#include <SDL.h>
#include <bass.h>


class BeatMap
{
public:
	BeatMap(SDL_Renderer *r, InitVariables var);
	BeatMap();
	~BeatMap();
	void render(Uint32 currentTick, double currentMusicPosition, int timeBarY);
private:
	SDL_Renderer *Renderer;
	std::vector<BeatPath> beatPath, getBeatPath();
	InitVariables initVariables;
	int SCREEN_HEIGHT, SCREEN_WIDTH;
	float pathWidthRatio, noteRadiusRatio;
	Uint8 pathHighlightAlpha;
	double beatNoteBufferTime;
	
};

