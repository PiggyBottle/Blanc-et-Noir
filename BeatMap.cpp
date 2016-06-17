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
	//std::vector<QWORD> a(1), b(1);
	std::vector<BeatPath> buffer(2);
	std::vector<QWORD> a= { 1000 };
	std::vector<QWORD> b = { 1400 };
	buffer = { BeatPath(Renderer, SCREEN_WIDTH / 2, SCREEN_WIDTH,initVariables.path_width_ratio,a,b),BeatPath(Renderer, SCREEN_WIDTH / 3, SCREEN_WIDTH,initVariables.path_width_ratio,a,b) };
	

	return buffer;
}

void BeatMap::render(Uint32 currentTick, QWORD currentMusicPosition, int timeBarY)
{
	for (std::vector<BeatPath>::iterator i = beatPath.begin(); i != beatPath.end(); ++i)
	{
		i->renderPath(currentTick, currentMusicPosition, timeBarY);
	}
}