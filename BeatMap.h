#pragma once
#include "BeatPath.h"
#include <SDL.h>
#include <bass.h>


struct Path_Beat_Pair {
	int path;
	double first_beat;
};

class BeatMap
{
public:
	BeatMap(SDL_Renderer *r, InitVariables var, Instruction nextInstruction);
	BeatMap();
	~BeatMap();
	void render(Uint32 currentTick, double currentMusicPosition, int timeBarY, SDL_Texture *note);
	enums::noteHit BeatMap::processInput(SDL_Event e, double songPosition);
	std::vector<enums::noteHit> computeVariables(double songPosition);
private:
	SDL_Renderer *Renderer;
	std::vector<BeatPath> beatPath, getBeatPath();
	InitVariables initVariables;
	Instruction instruction;
	int SCREEN_HEIGHT, SCREEN_WIDTH, numberOfKeys, keySeparationThickness;
	int getLinkedPath(int start, int end, double songPosition);
	std::vector<int> keyCoordinates;
	std::vector<KeyStatus> keyStatuses;
	double beatNoteBufferTime;
	void generateKeyCoordinates(), generateKeyStatuses(std::vector<int> key_binds);
	bool thereIsAnOverlap(int start, int end, std::vector<int> pathCoordinates);
	float parseStringToFraction(std::string line);
	std::vector<double> parseStringToVectorOfDoubles(std::string line);
	std::vector<std::string> parseStringToVectorOfTrimmedStrings(std::string line);
	std::vector<PathMotion> parseStringVectorToPathMotionVector(std::vector<std::string> strings);
	std::vector<BeatNote> parseStringVectorToBeatNoteVector(std::vector<std::string> strings);
	
};

