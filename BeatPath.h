#pragma once
#include <SDL.h>
#include <bass.h>
#include "enums.h"
#include <vector>

class BeatPath
{
public:
	BeatPath(SDL_Renderer *r, int center, int screenWidth, float widthRatio, StartEnd startEnd);
	BeatPath();
	~BeatPath();
	void renderPath(Uint32 currentTick, QWORD songPosition, int timeBarY);
private:
	SDL_Renderer *Renderer;
	StartEnd startEnd;
	int pathWidth;
	int pathCenter = 0;
	int SCREEN_WIDTH;
	int pathCenterThickness = 5;
	RGB pathColor = {255,0,0};
	SDL_Rect generatePathCenter(int timeBarY, Uint32 currentTick);
	bool pathIsOn(QWORD songPosition);


};

