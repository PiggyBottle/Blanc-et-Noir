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
	generateKeyStatuses(var.keyBinds);

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
	BeatNote bn= { enums::SINGLE_HIT, 10.5,10.5 };
	for (int i = 0; i < 1; i++)
	{
		bn.start_position += 0.1;
		bn.end_position += 0.1;
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

void BeatMap::generateKeyStatuses(std::vector<int> key_binds)
{
	std::vector<KeyStatus> kS;
	int start, end;
	switch (numberOfKeys) 
	{
	case 4:
		start = 1; end = 5; break;
	case 6:
		start = 0; end = 6; break;
	}
	KeyStatus key = { 0,false,-1};
	for (int i = start; i < end; i++) 
	{
		key.key = key_binds[i]; kS.push_back(key);
	}
	keyStatuses = kS;
}

bool BeatMap::thereIsAnOverlap(int start, int end, std::vector<int> pathCoordinates)
{
	return (start < pathCoordinates[1] && end > pathCoordinates[0]);
}

bool BeatMap::render(Uint32 currentTick, double currentMusicPosition, int timeBarY)
{
	bool thereIsABreak = false;
	for (std::vector<BeatPath>::iterator i = beatPath.begin(); i != beatPath.end(); ++i)
	{
		if (i->renderPath(currentTick, currentMusicPosition, timeBarY, beatNoteBufferTime)) {
			thereIsABreak = true;
		}
	}

	SDL_Rect a = { 0,timeBarY,keySeparationThickness,SCREEN_HEIGHT - timeBarY };
	for (int i = 1; i < numberOfKeys; i++)
	{
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		a.x = keyCoordinates[i] - ((int)(0.5f * ((float)keySeparationThickness)));
		SDL_RenderFillRect(Renderer, &a);
	}
	return thereIsABreak;
}

enums::noteHit BeatMap::processInput(SDL_Event e, double songPosition)
{
	//Determine which key is pressed
	enums::noteHit hit = enums::NO_HIT;
	int key = -1;
	for (size_t i = 0, ilen = keyStatuses.size(); i < ilen; ++i)
	{
		if (e.key.keysym.sym == keyStatuses[i].key) { key = i; break; }
	}
	if (key == -1) { return hit; }

	//Evaluate key-up/key-down scenarios
	if (e.type == SDL_KEYUP && keyStatuses[key].already_pressed)
	{
		keyStatuses[key].already_pressed = false;
		if (keyStatuses[key].linked_path != -1)
		{
		//Release linked path here
		//_________________
		keyStatuses[key].linked_path = -1;
		}
	}
	else if (e.type == SDL_KEYDOWN && !keyStatuses[key].already_pressed)
	{
		keyStatuses[key].already_pressed = true;
		keyStatuses[key].linked_path = getLinkedPath(keyCoordinates[key],keyCoordinates[key+1], songPosition);
		if (keyStatuses[key].linked_path != -1) {
			//Register key with linked_path here
			hit = beatPath[keyStatuses[key].linked_path].registerKey(key, songPosition);
		}
	}
	return hit;
}

int BeatMap::getLinkedPath(int start, int end, double songPosition)
{
	std::vector<Path_Beat_Pair> pathBeatPairs;
	Path_Beat_Pair pathBeatPair;
	double nextBeatTime;
	
	//Get all paths that have upcoming beats
	for (size_t i = 0, ilen = beatPath.size(); i < ilen; i++)
	{
		if (beatPath[i].isOn && thereIsAnOverlap(start, end, beatPath[i].getCurrentPathWidthCoordinates()))
		{
			nextBeatTime = beatPath[i].getNextBeatTime();
			if (nextBeatTime != -1 && nextBeatTime - songPosition < initVariables.okay_hit_buffer_time)
			{ 
				pathBeatPair.first_beat = nextBeatTime;
				pathBeatPair.path = i;
				pathBeatPairs.push_back(pathBeatPair);
			}
		}
	}
	if (pathBeatPairs.empty()) { return -1; }

	//Pick the path with the nearest beat
	pathBeatPair = pathBeatPairs[0];
	for (size_t i = 0, ilen = pathBeatPairs.size(); i < ilen; i++)
	{
		if (pathBeatPairs[i].first_beat < pathBeatPair.first_beat)
		{
			pathBeatPair = pathBeatPairs[i];
		}
	}
	return pathBeatPair.path;

}