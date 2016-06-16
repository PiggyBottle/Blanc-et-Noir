#pragma once
#include <SDL.h>
#include <bass.h>
#include "enums.h"

class BeatPath
{
public:
	BeatPath(SDL_Renderer *r, int center, int screenWidth, float widthRatio);
	~BeatPath();
	void renderPath(Uint32 currentTick, QWORD songPosition, int timeBarY);
private:
	SDL_Renderer *Renderer;
	int SCREEN_WIDTH;
	int pathCenter = 0;
	int pathWidth;
	int pathCenterThickness = 5;
	RGB pathColor = {255,0,0};
	SDL_Rect generatePathCenter(int timeBarY, Uint32 currentTick);


};

