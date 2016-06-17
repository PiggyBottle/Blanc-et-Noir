#pragma once
#include <SDL.h>
#include <bass.h>
#include "enums.h"
#include <vector>

class BeatPath
{
public:
	BeatPath(SDL_Renderer *r, int center, int screenWidth, float widthRatio, std::vector<QWORD> start_times, std::vector<QWORD> end_times);
	BeatPath();
	~BeatPath();
	int pathCenter = 0;
	std::vector<QWORD> startTimes,endTimes;
	void renderPath(Uint32 currentTick, QWORD songPosition, int timeBarY);
private:
	SDL_Renderer *Renderer;
	int pathWidth;
	int SCREEN_WIDTH;
	int pathCenterThickness = 5;
	RGB pathColor = {255,0,0};
	SDL_Rect generatePathCenter(int timeBarY, Uint32 currentTick);
	bool pathIsOn(QWORD songPosition);


};

