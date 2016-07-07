#pragma once
#include "enums.h"
#include "GameState.h"
#include "BeatMap.h"
#include <SDL.h>




class MainGame : public GameState
{
public:
	MainGame(SDL_Renderer *gRenderer, InitVariables var);
	void uninit();
	Instruction process(SDL_Event e, Instruction nextInstruction);
private:
	void init(Instruction nextInstruction), computeCombo(std::vector<enums::noteHit> *hits, double currentSongPosition), renderNoteHitAndCombo(double currentSongPosition);
	enums::noteHit computeMostRecentNoteHit(std::vector<enums::noteHit> *hits);
	MostRecentNoteHit mostRecentNoteHit;
	Uint8 processBgAlpha();
	Uint32 currentTick, startUpTick;
	SDL_Renderer *Renderer;
	TextureWithVariables bg, note;
	TextureWithVariables noteHitTypes[enums::NO_HIT];
	SDL_Rect timeBar;
	Instruction instruction;
	bool initted = false, uiHasFinishedTransitioning = false;
	int SCREEN_HEIGHT, SCREEN_WIDTH, startUpFadeTime, bgAlpha, uiTransitionTime, startUpFadeInBackgroundFinishTime, timeBarY;
	int processTimeBarY();
	unsigned long bgm, sfx;
	BeatMap beatMap;
	InitVariables initVariables;
};