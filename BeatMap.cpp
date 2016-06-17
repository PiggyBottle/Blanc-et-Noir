#include "BeatMap.h"
#include <stdio.h>


BeatMap::BeatMap(SDL_Renderer *r, InitVariables var)
{
	this->Renderer = r;
	this->initVariables = var;
	this->SCREEN_HEIGHT = var.screen_height;
	this->pathWidthRatio = var.path_width_ratio;
	this->SCREEN_WIDTH = var.screen_width;
	beatPath = getBeatPath();
}
BeatMap::BeatMap() {}

BeatMap::~BeatMap()
{
}

std::vector<BeatPath> BeatMap::getBeatPath()
{
	StartEnd startEnd;
	startEnd.start = { 800 };
	startEnd.end = { 1400 };
	std::vector<PathMotion> pathMotion;
	PathMotion pM = { enums::LINEAR_SLIDE, 1000, 1100, 0.5f,0.7f};
	pathMotion.push_back(pM);
	std::vector<BeatPath> buffer(2);
	buffer = { BeatPath(Renderer, 0.5, SCREEN_WIDTH,initVariables.path_width_ratio,startEnd,pathMotion), BeatPath(Renderer, 0.3, SCREEN_WIDTH,initVariables.path_width_ratio,startEnd,pathMotion) };
	

	return buffer;
}

void BeatMap::render(Uint32 currentTick, QWORD currentMusicPosition, int timeBarY)
{
	for (std::vector<BeatPath>::iterator i = beatPath.begin(); i != beatPath.end(); ++i)
	{
		i->renderPath(currentTick, currentMusicPosition, timeBarY);
	}
}