#include "BeatMap.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

BeatMap::BeatMap(SDL_Renderer *r, InitVariables var, Instruction nextInstruction)
{
	this->Renderer = r;
	this->initVariables = var;
	this->instruction = nextInstruction;
	this->SCREEN_HEIGHT = var.screen_height;
	this->SCREEN_WIDTH = var.screen_width;
	this->beatNoteBufferTime = var.note_buffer_time;
	this->keySeparationThickness = var.keySeparation_thickness;

	this->numberOfKeys = nextInstruction.gameKeys;
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
	boost::filesystem::path p("Music\\" + instruction.songToLoad + "\\" + std::to_string(instruction.gameKeys) + "key\\" + instruction.songDifficulty);
	boost::filesystem::directory_iterator end_itr;
	std::vector<std::string> pathDirectories;
	for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
	{
		if (boost::filesystem::is_regular_file(itr->path()) && itr->path().extension().string() == ".txt")
		{
			pathDirectories.push_back(itr->path().string());
		}
	}
	std::vector<BeatPath> beatPath;
	for (std::vector<std::string>::iterator i = pathDirectories.begin(); i != pathDirectories.end(); ++i)
	{
		float startPosition, startWidth;
		RGB startColor;
		StartEnd startEnd;
		std::vector<PathMotion> pathMotion,widthMotion;
		std::vector<BeatNote> beatNotes;

		std::ifstream file(*i);
		std::string line;
		while (std::getline(file, line))
		{
			if (line == "#startposition")
			{
				std::string line2; std::getline(file, line2);
				startPosition = parseStringToFraction(line2);
			}
			else if (line == "#startwidth")
			{
				std::string line2; std::getline(file, line2);
				startWidth = parseStringToFraction(line2);
			}
			if (line == "#startcolor")
			{
				std::string line2; std::getline(file, line2);
				boost::algorithm::trim_if(line2, boost::is_any_of("() "));
				std::vector<std::string> lines;
				boost::split(lines, line2, boost::is_any_of("|"));
				startColor.r = std::stoi(lines[0]);
				startColor.g = std::stoi(lines[1]);
				startColor.b = std::stoi(lines[2]);
			}
			else if (line == "#startend")
			{
				std::string line2, line3; std::getline(file, line2); std::getline(file, line3);
				startEnd.start = parseStringToVectorOfDoubles(line2);
				startEnd.end = parseStringToVectorOfDoubles(line3);
			}
			else if (line == "#pathmotion")
			{
				std::string line2; std::getline(file, line2);
				pathMotion = parseStringVectorToPathMotionVector(parseStringToVectorOfTrimmedStrings(line2));
			}
			else if (line == "#widthmotion")
			{
				std::string line2; std::getline(file, line2);
				widthMotion = parseStringVectorToPathMotionVector(parseStringToVectorOfTrimmedStrings(line2));
			}
			else if (line == "#beatnote")
			{
				std::string line2; std::getline(file, line2);
				beatNotes = parseStringVectorToBeatNoteVector(parseStringToVectorOfTrimmedStrings(line2));
			}
		}
		beatPath.push_back(BeatPath(Renderer, startPosition, startWidth, startColor, initVariables, startEnd, pathMotion, widthMotion, beatNotes));
	}
	return beatPath;
}

float BeatMap::parseStringToFraction(std::string line)
{
	std::vector<std::string> lines;
	std::vector<float> floats;
	float number;
	boost::replace_all(line, " ", "");
	boost::split(lines, line, boost::is_any_of("/"));
	for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); ++i)
	{
		std::stringstream stream(*i);
		stream >> number;
		floats.push_back(number);
	}
	switch (floats.size())
	{
	case 2:
		number = ((float)floats[0]) / ((float)floats[1]); break;
	case 1:
		number = floats[0]; break;
	default:
		number = -1.f;
	}
	return number;
}

std::vector<double> BeatMap::parseStringToVectorOfDoubles(std::string line)
{
	std::vector<std::string> lines;
	std::vector<double> doubles;
	double number;
	boost::split(lines, line, boost::is_any_of(","));
	for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); ++i)
	{
		boost::algorithm::trim(*i);
		std::stringstream stream(*i);
		stream >> number;
		doubles.push_back(number);
	}
	return doubles;
}

std::vector<std::string> BeatMap::parseStringToVectorOfTrimmedStrings(std::string line)
{
	std::vector<std::string> strings;
	boost::split(strings, line, boost::is_any_of(","));
	for (std::vector<std::string>::iterator i = strings.begin(); i != strings.end(); ++i)
	{
		boost::algorithm::trim_if(*i, boost::is_any_of("() "));
	}
	return strings;
}

std::vector<PathMotion> BeatMap::parseStringVectorToPathMotionVector(std::vector<std::string> strings)
{
	std::vector<PathMotion> pathMotion;
	std::vector<std::string> lines;
	for (std::vector<std::string>::iterator i = strings.begin(); i != strings.end(); ++i)
	{
		boost::split(lines, *i, boost::is_any_of("|"));
		PathMotion pM;
		pM.motion = (enums::motions)std::stoi(lines[0]);
		pM.start_position = std::stod(lines[1]);
		pM.end_position = std::stod(lines[2]);
		pM.start_x = parseStringToFraction(lines[3]);
		pM.end_x = parseStringToFraction(lines[4]);
		if (pM.motion == enums::HALF_SINE_SLIDE || pM.motion == enums::FULL_SINE_SLIDE) { pM.amplitude = parseStringToFraction(lines[5]); }
		//Add more functions here for future motions that involve special parameters
		pathMotion.push_back(pM);
	}
	return pathMotion;
}

std::vector<BeatNote> BeatMap::parseStringVectorToBeatNoteVector(std::vector<std::string> strings)
{
	std::vector<BeatNote> beatNote;
	std::vector<std::string> lines;
	for (std::vector<std::string>::iterator i = strings.begin(); i != strings.end(); ++i)
	{
		boost::split(lines, *i, boost::is_any_of("|"));
		BeatNote bN;
		bN.note_type = (enums::noteType)std::stoi(lines[0]);
		bN.start_position = std::stod(lines[1]);
		//Safety precaution: auto make end_position = start_position if note type == SINGLE_HIT
		if (bN.note_type == enums::SINGLE_HIT) { bN.end_position = std::stod(lines[1]); }
		else { bN.end_position = std::stod(lines[2]); }

		beatNote.push_back(bN);
	}
	return beatNote;
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

void BeatMap::render(Uint32 currentTick, double currentMusicPosition, int timeBarY, SDL_Texture *note)
{
	for (std::vector<BeatPath>::iterator i = beatPath.begin(); i != beatPath.end(); ++i)
	{
		i->renderPath( currentMusicPosition, timeBarY, beatNoteBufferTime,note);
	}

	SDL_Rect a = { 0,timeBarY,keySeparationThickness,SCREEN_HEIGHT - timeBarY };
	for (int i = 1; i < numberOfKeys; i++)
	{
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		a.x = keyCoordinates[i] - ((int)(0.5f * ((float)keySeparationThickness)));
		SDL_RenderFillRect(Renderer, &a);
	}
}

std::vector<enums::noteHit> BeatMap::computeVariables(double songPosition)
{
	std::vector<enums::noteHit> hits;
	for (size_t i = 0, ilen = keyStatuses.size(); i < ilen; ++i)
	{
		if (keyStatuses[i].linked_path == -1) { continue; }
		if (!thereIsAnOverlap(keyCoordinates[i], keyCoordinates[i + 1], beatPath[keyStatuses[i].linked_path].getCurrentPathWidthCoordinates()))
		{
			hits.push_back(beatPath[keyStatuses[i].linked_path].deregisterKey((int)i,songPosition));
			keyStatuses[i].linked_path = -1;
		}
	}

	for (std::vector<BeatPath>::iterator i = beatPath.begin(); i != beatPath.end(); ++i)
	{
		hits.push_back(i->computeVariables(songPosition));
	}
	return hits;
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
		hit = beatPath[keyStatuses[key].linked_path].deregisterKey(key, songPosition);
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
			if (nextBeatTime != -1 && (nextBeatTime - songPosition < initVariables.okay_hit_buffer_time || beatPath[i].isHolding))
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