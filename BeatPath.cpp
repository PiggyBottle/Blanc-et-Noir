#include "BeatPath.h"
#include <stdio.h>

BeatPath::BeatPath() {}
BeatPath::BeatPath(SDL_Renderer *r, float center, int screenWidth, float widthRatio, Uint8 path_highlight_alpha, float note_radius_ratio,StartEnd STARTEND, std::vector<PathMotion> PATHMOTION)
{
	this->Renderer = r;
	this->pathCenter = center;
	this->SCREEN_WIDTH = screenWidth;
	this->pathWidth = widthRatio;
	this->pathHighlightAlpha = path_highlight_alpha;
	this->noteRadiusRatio = note_radius_ratio;
	
	startEnd = STARTEND;
	pathMotions = PATHMOTION;

}


BeatPath::~BeatPath()
{
}

void BeatPath::renderPath(Uint32 currentTick, double songPosition, int timeBarY)
{
	SDL_Rect centerOfPath = generatePathCenter(timeBarY, songPosition);
	if (!pathIsOn(songPosition)) { return; }

	//Highlight path
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
	SDL_Rect highlight = { 0,0,0,timeBarY };
	highlight.x = centerOfPath.x - ((int)(((float)SCREEN_WIDTH) * pathWidth));
	highlight.w = (int)(((float)SCREEN_WIDTH)*(2.0 * pathWidth));
	SDL_SetRenderDrawColor(Renderer,51, 204, 255, 155);
	SDL_RenderFillRect(Renderer, &highlight);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

	//Draw Path Center
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &centerOfPath);

	//Draw borders
	SDL_Rect border = { 0,0,1,timeBarY };
	border.x = centerOfPath.x - ((int)(((float)SCREEN_WIDTH) * pathWidth));
	SDL_RenderFillRect(Renderer, &border);
	border.x = centerOfPath.x + ((int)(((float)SCREEN_WIDTH) * pathWidth));
	SDL_RenderFillRect(Renderer, &border);

	//Draw beat notes
	drawBeatNotes(songPosition, timeBarY);

}

void BeatPath::drawBeatNotes(double songPosition, int timeBarY)
{
	for (std::vector<BeatNote>::iterator i = beatNotes.begin(); i != beatNotes.end(); ++i)
	{
		//Delete notes that have been hit

		//Delete expired notes (and register a break)

		//Draw active notes
	}
}


SDL_Rect BeatPath::generatePathCenter(int timeBarY, double currentPosition)
{
	SDL_Rect rect = {0,0,pathCenterThickness,timeBarY};

	for (std::vector<PathMotion>::iterator i = pathMotions.begin(); i != pathMotions.end(); ++i)
	{ 
		if (i->start_position < currentPosition && currentPosition < i->end_position)
		{
			rect.x =(int)(((float)SCREEN_WIDTH) * processPathMotionX(*i, currentPosition));
			return rect;
		}
	}


	rect.x =(int)( (((float)SCREEN_WIDTH)*(pathCenter)) - (0.5*(pathCenterThickness)));
	return rect;
}

float BeatPath::processPathMotionX(PathMotion pathMotion, double currentPosition)
{
	if (pathMotion.motion == enums::LINEAR_SLIDE)
	{
		pathCenter = pathMotion.end_x;
		return (((float)pathMotion.start_x) + (((float)(pathMotion.end_x - pathMotion.start_x)) * (((float)(currentPosition - pathMotion.start_position))/((float)(pathMotion.end_position - pathMotion.start_position)))));
	}
	return 0;
}
bool BeatPath::pathIsOn(double songPosition)
{
	std::vector<double>::const_iterator a, b;
	a = startEnd.start.begin();
	b = startEnd.end.begin();
	while (a != startEnd.start.end() && b != startEnd.end.end()) 
	{
		if (songPosition > *a && songPosition < *b)
		{
			return true;
		}
		++a; ++b;
	}
	return false;
}