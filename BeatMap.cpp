#include "BeatMap.h"
#include <iostream>


BeatMap::BeatMap(SDL_Renderer *r, InitVariables var, int number_of_keys)
{
	this->Renderer = r;
	this->initVariables = var;
	this->SCREEN_HEIGHT = var.screen_height;
	this->SCREEN_WIDTH = var.screen_width;
	this->beatNoteBufferTime = var.note_buffer_time;
	this->keySeparationThickness = var.keySeparation_thickness;

	this->numberOfKeys = number_of_keys;
	generateKeyCoordinates();

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
	for (int i = 0; i < 15; i++)
	{
		bn.start_position += 0.1f;
		beatNotes.push_back(bn);
	}
	buffer.push_back(BeatPath(Renderer, 0.6f,initVariables,startEnd,pathMotion,widthMotion, beatNotes));
	pM.start_x = 0.4f; pM.end_x = 0.2f;
	pathMotion[0] = pM;
	buffer.push_back(BeatPath(Renderer, 0.4f,initVariables,startEnd, pathMotion, widthMotion, beatNotes));
	

	return buffer;
}

void BeatMap::generateKeyCoordinates()
{
	std::vector<int> coordinates(numberOfKeys+1);
	int interval = (int)(((float)SCREEN_WIDTH) / ((float)numberOfKeys));

	coordinates[0] = 0;
	for (int i = 0; i < numberOfKeys; i++) { coordinates[i+1] = (i+1) * interval; }

	keyCoordinates = coordinates;
}

void BeatMap::render(Uint32 currentTick, double currentMusicPosition, int timeBarY)
{
	for (std::vector<BeatPath>::iterator i = beatPath.begin(); i != beatPath.end(); ++i)
	{
		i->renderPath(currentTick, currentMusicPosition, timeBarY, beatNoteBufferTime);
	}

	SDL_Rect a = { 0,timeBarY,keySeparationThickness,SCREEN_HEIGHT - timeBarY };
	for (int i = 1; i < numberOfKeys; i++)
	{
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		a.x = keyCoordinates[i] - ((int)(0.5f * ((float)keySeparationThickness)));
		SDL_RenderFillRect(Renderer, &a);
	}
}