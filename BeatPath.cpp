#include "BeatPath.h"
#include <stdio.h>
#include <SDL2_gfxPrimitives.h>

BeatPath::BeatPath() {}
BeatPath::BeatPath(SDL_Renderer *r, float center, int screenWidth, float widthRatio, Uint8 path_highlight_alpha, float note_radius_ratio,StartEnd STARTEND, std::vector<PathMotion> PATHMOTION, std::vector<BeatNote> beat_notes)
{
	this->Renderer = r;
	this->pathCenter = center;
	this->SCREEN_WIDTH = screenWidth;
	this->pathWidth = widthRatio;
	this->pathHighlightAlpha = path_highlight_alpha;
	this->noteRadiusRatio = note_radius_ratio;
	beatNotes = beat_notes;
	
	startEnd = STARTEND;
	pathMotions = PATHMOTION;

}


BeatPath::~BeatPath()
{
}

void BeatPath::renderPath(Uint32 currentTick, double songPosition, int timeBarY, double beatnote_buffer_time)
{
	int centerOfPath = computeCenterOfPath(songPosition);
	if (!pathIsOn(songPosition)) { return; }

	//Highlight path
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
	SDL_Rect highlight = { 0,0,0,timeBarY };
	highlight.x = centerOfPath - ((int)(((float)SCREEN_WIDTH) * pathWidth));
	highlight.w = (int)(((float)SCREEN_WIDTH)*(2.0 * pathWidth));
	SDL_SetRenderDrawColor(Renderer,51, 204, 255, 155);
	SDL_RenderFillRect(Renderer, &highlight);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

	//Draw Path Center
	drawPathCenter(centerOfPath, timeBarY);

	//Draw borders
	SDL_Rect border = { 0,0,1,timeBarY };
	border.x = centerOfPath - ((int)(((float)SCREEN_WIDTH) * pathWidth));
	SDL_RenderFillRect(Renderer, &border);
	border.x = centerOfPath + ((int)(((float)SCREEN_WIDTH) * pathWidth));
	SDL_RenderFillRect(Renderer, &border);

	//Draw beat notes
	drawBeatNotes(songPosition, timeBarY, beatnote_buffer_time, centerOfPath);

}

void BeatPath::drawBeatNotes(double songPosition, int timeBarY, double beatnote_buffer_time, int center_of_path)
{
	for (std::vector<BeatNote>::iterator i = beatNotes.begin(); i != beatNotes.end(); ++i)
	{
		//Delete notes that have been hit

		//Delete expired notes (and register a break)

		//Draw active notes
		if (i->start_position - songPosition > beatnote_buffer_time) { break; }
		renderBeatNotes(songPosition, timeBarY, beatnote_buffer_time, center_of_path, i);
	}
}


void BeatPath::renderBeatNotes(double songPosition, int timeBarY, double beatnote_buffer_time, int center_of_path, std::vector<BeatNote>::iterator beat_note)
{
	
	Sint16 x[4], y[4], noteCenterY;

	//Compute noteCenterY
	double noteToBufferRatio = (beat_note->start_position - songPosition) / (beatnote_buffer_time);
	noteCenterY = (Sint16) (((double)(SCREEN_WIDTH - timeBarY)) * (((double)1) - noteToBufferRatio));
	
	//Compute Vertex Array
	x[0] = (Sint16)center_of_path;
	y[0] = noteCenterY - ((Sint16)(noteRadiusRatio * ((float)SCREEN_WIDTH)));
	x[1] = (Sint16)center_of_path - ((Sint16)(noteRadiusRatio * ((float)SCREEN_WIDTH)));
	y[1] = noteCenterY;
	x[2] = (Sint16)center_of_path;
	y[2] = noteCenterY + ((Sint16)(noteRadiusRatio * ((float)SCREEN_WIDTH)));
	x[3] = (Sint16)center_of_path + ((Sint16)(noteRadiusRatio * ((float)SCREEN_WIDTH)));
	y[3] = noteCenterY;

	//Render note
	filledPolygonRGBA(Renderer,x,y,4,0,0,0,255);
	
}

int BeatPath::computeCenterOfPath(double currentPosition)
{
	int centerOfPath;

	for (std::vector<PathMotion>::iterator i = pathMotions.begin(); i != pathMotions.end(); ++i)
	{ 
		if (i->start_position < currentPosition && currentPosition < i->end_position)
		{
			centerOfPath =(int)((((float)SCREEN_WIDTH) * processPathMotionX(*i, currentPosition)));
			return centerOfPath;
		}
	}


	centerOfPath =(int)( (((float)SCREEN_WIDTH)*(pathCenter)));
	return centerOfPath;
}

void BeatPath::drawPathCenter(int centerOfPath, int timeBarY)
{
	SDL_Rect rect = {centerOfPath - ((int)(0.5f*((float)pathCenterThickness))),0,pathCenterThickness,timeBarY};
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &rect);
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