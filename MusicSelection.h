#pragma once
#include <boost/filesystem.hpp>
#include "GameState.h"
class MusicSelection :
	public GameState
{
public:
	MusicSelection(SDL_Renderer *gRenderer, InitVariables iV);
	~MusicSelection();
	void init();
	void uninit();
	Instruction process(SDL_Event e, Instruction nextInstruction);
private:
	SDL_Renderer *Renderer = NULL;
	Instruction instruction;
	InitVariables initVariables;
	std::vector<MusicFileSystem> beatMaps;
	std::vector<std::vector<int>> beatMapsWithKey;
	ListOfPanels listOfPanels;
	MusicSelectionPanel generateMusicSelectionPanel(int index);
	std::vector<BeatMapKeyAndDifficulty> checkForBeatMaps(boost::filesystem::path path, int index);
	void getBeatMaps(), processEvent(SDL_Event e), computeSelectionBarX(double currentTick), calculateMaxNumberOfPanels(), generateListOfPanels();
	std::string getSongInfo(std::string path);
	bool initted = false, selectionIsMinimized;
	int currentSelectedMusicIndex, currentNumberOfKeysSelected, mouseX, mouseY, selectionBarX, maxNumberOfPanels, findIndexOfElementInBeatMapsWithKey(std::vector<int> *vect, int element);
	double selectionBarTransitionTime;

	//Temporary texture
	SDL_Texture *screen = NULL;
};
