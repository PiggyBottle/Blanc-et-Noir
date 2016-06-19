#pragma once
#include <SDL.h>
#include <bass.h>
#include "enums.h"
#include <vector>

class BeatPath
{
public:
	BeatPath(SDL_Renderer *r, float center, int screenWidth, float widthRatio, Uint8 path_highlight_alpha, StartEnd STARTEND,std::vector<PathMotion> PATHMOTION);
	BeatPath();
	~BeatPath();
	void renderPath(Uint32 currentTick, QWORD songPosition, int timeBarY);
private:
	SDL_Renderer *Renderer;
	StartEnd startEnd;
	float pathWidth;
	//pathCenter is what will be used to keep track of what the path should be at when there are no motions going on
	float pathCenter = 0;
	int SCREEN_WIDTH;
	int pathCenterThickness = 5;
	float processPathMotionX(PathMotion pathMotion, QWORD currentPosition);
	RGB pathColor = {255,0,0};
	SDL_Rect generatePathCenter(int timeBarY, QWORD currentPosition);
	bool pathIsOn(QWORD songPosition);
	std::vector<PathMotion> pathMotions;
	Uint8 pathHighlightAlpha;


};

