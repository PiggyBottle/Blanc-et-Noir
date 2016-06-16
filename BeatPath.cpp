#include "BeatPath.h"
#include <stdio.h>



BeatPath::BeatPath(SDL_Renderer *r, int center, int screenWidth, float widthRatio)
{
	this->Renderer = r;
	this->pathCenter = center;
	this->SCREEN_WIDTH = screenWidth;
	this->pathWidth = (int)(widthRatio * ((float)SCREEN_WIDTH));
}


BeatPath::~BeatPath()
{
}

void BeatPath::renderPath(Uint32 currentTick, QWORD songPosition, int timeBarY)
{
	SDL_Rect centerOfPath = generatePathCenter(timeBarY, currentTick);
	//Draw Path Center
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &centerOfPath);

	//Draw borders
	centerOfPath.w = 1;
	centerOfPath.x = pathCenter - pathWidth;
	SDL_RenderFillRect(Renderer, &centerOfPath);
	centerOfPath.x = pathCenter + pathWidth;
	SDL_RenderFillRect(Renderer, &centerOfPath);

	//Highlight path
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
	centerOfPath.x = pathCenter - pathWidth;
	centerOfPath.w = 2 * pathWidth;
	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 50);
	SDL_RenderFillRect(Renderer, &centerOfPath);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);



}

SDL_Rect BeatPath::generatePathCenter(int timeBarY, Uint32 currentTick)
{
	SDL_Rect rect = {0,0,pathCenterThickness,timeBarY};
	rect.x = pathCenter - (0.5*(pathCenterThickness));
	return rect;
}