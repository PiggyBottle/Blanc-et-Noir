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
	Instruction instruction, generateInstructionForMainGame();
	InitVariables initVariables;
	std::vector<MusicFileSystem> beatMaps;
	std::vector<std::vector<int>> beatMapsWithKey;
	ListOfPanels listOfPanels;
	std::vector<MusicSelectionClickableButton> clickableButtons;
	enums::beatMapDifficulty currentSelectedDifficulty;
	MusicSelectionPanel generateMusicSelectionPanel(int index);
	std::vector<BeatMapKeyAndDifficulty> checkForBeatMaps(boost::filesystem::path path, int index);
	void getBeatMaps(), processEvent(SDL_Event e), computeSelectionBarX(double currentTick), calculateMaxNumberOfPanels(), generateListOfPanels(), generateClickableButtons(),assertThatPanelCornersDontCrossLimit(), checkThatMusicIsPlayingWithinRange(), checkIfClickableButtonIsPressed(), processButtonClick(MusicSelectionClickableButton *button), renderClickableButtons(),computePanelY(), backupPanelY();
	void getSongInfo(std::string path, MusicFileSystem *fileSystem), selectPanel(), freeAndChangebgm(), freeAndChangebg(), freeAndChangeAlbumArt(), checkThatDifficultyIsAvailableInNewSong(), renderAlbumArt();
	bool initted = false, selectionIsMinimized, mouseIsBeingDragged, mouseIsClicked, panelAreaIsClicked, proceedToMainGame;
	int currentSelectedMusicIndex, currentNumberOfKeysSelected, mouseX, mouseY, lastClickedX, lastClickedY, selectionBarX, maxNumberOfPanels, findIndexOfElementInBeatMapsWithKey(std::vector<int> *vect, int element);
	double selectionBarTransitionTime;

	TextureWithVariables bg, albumArt;
	unsigned long bgm;
};
