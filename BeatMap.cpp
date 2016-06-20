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
	startEnd.start = { 3.0 };
	startEnd.end = { 15.0 };
	std::vector<PathMotion> pathMotion,widthMotion;
	PathMotion pM = { enums::LINEAR_SLIDE, 5.00, 5.50, 0.6f,0.8f};
	pathMotion.push_back(pM);
	PathMotion wM = { enums::LINEAR_SLIDE, 6.50, 7, 0.3f,(1.0f / 15.0f) };
	widthMotion.push_back(wM);
	std::vector<BeatPath> buffer;
	std::vector<BeatNote> beatNotes;
	BeatNote bn= { enums::SINGLE_HIT, 10.5,9.0 };
	beatNotes.push_back(bn);
	buffer.push_back(BeatPath(Renderer, 0.6f, SCREEN_WIDTH,initVariables.path_width_ratio,pathHighlightAlpha,noteRadiusRatio,startEnd,pathMotion,widthMotion, beatNotes));
	pM.start_x = 0.4f; pM.end_x = 0.2f;
	pathMotion[0] = pM;
	buffer.push_back(BeatPath(Renderer, 0.4f, SCREEN_WIDTH, initVariables.path_width_ratio, pathHighlightAlpha, noteRadiusRatio, startEnd, pathMotion, widthMotion, beatNotes));
	

	return buffer;
}

void BeatMap::render(Uint32 currentTick, double currentMusicPosition, int timeBarY)
{
	for (std::vector<BeatPath>::iterator i = beatPath.begin(); i != beatPath.end(); ++i)
	{
		i->renderPath(currentTick, currentMusicPosition, timeBarY, beatNoteBufferTime);
	}
}