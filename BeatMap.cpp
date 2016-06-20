#include "BeatMap.h"
#include <stdio.h>


BeatMap::BeatMap(SDL_Renderer *r, InitVariables var)
{
	this->Renderer = r;
	this->initVariables = var;
	this->SCREEN_HEIGHT = var.screen_height;
	this->pathWidthRatio = var.path_width_ratio;
	this->SCREEN_WIDTH = var.screen_width;
	this->pathHighlightAlpha = var.path_highlight_alpha;
	this->noteRadiusRatio = var.note_radius_ratio;
	this->beatNoteBufferTime = var.note_buffer_time;
	beatPath = getBeatPath();
}
BeatMap::BeatMap() {}

BeatMap::~BeatMap()
{
}

std::vector<BeatPath> BeatMap::getBeatPath()
{
	StartEnd startEnd;
	startEnd.start = { 8.0 };
	startEnd.end = { 14.0 };
	std::vector<PathMotion> pathMotion;
	PathMotion pM = { enums::LINEAR_SLIDE, 10.00, 11.00, 0.5f,0.7f};
	pathMotion.push_back(pM);
	std::vector<BeatPath> buffer(2);
	std::vector<BeatNote> beatNotes;
	BeatNote bn= { enums::SINGLE_HIT, 10.5,9.0 };
	beatNotes.push_back(bn);
	buffer = { BeatPath(Renderer, 0.5f, SCREEN_WIDTH,initVariables.path_width_ratio,pathHighlightAlpha,noteRadiusRatio,startEnd,pathMotion,beatNotes), BeatPath(Renderer, 0.3f, SCREEN_WIDTH,initVariables.path_width_ratio,pathHighlightAlpha,noteRadiusRatio,startEnd,pathMotion,beatNotes) };
	

	return buffer;
}

void BeatMap::render(Uint32 currentTick, double currentMusicPosition, int timeBarY)
{
	for (std::vector<BeatPath>::iterator i = beatPath.begin(); i != beatPath.end(); ++i)
	{
		i->renderPath(currentTick, currentMusicPosition, timeBarY, beatNoteBufferTime);
	}
}