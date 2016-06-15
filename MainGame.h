#pragma once
#include "enums.h"
#include "GameState.h"
#include <SDL.h>




class MainGame : public GameState
{
public:
	MainGame(SDL_Renderer *gRenderer, InitVariables var);
	void uninit();
	Instruction process(SDL_Event e, Instruction nextInstruction);
private:
	void init(Instruction nextInstruction), uiTransitionIn(int currentTick);
	Uint8 processBgAlpha();
	SDL_Renderer *Renderer;
	bool initted = false, startingUp = false;
	Instruction instruction;
	SDL_Texture *bg = NULL, *note = NULL;
	int SCREEN_HEIGHT, SCREEN_WIDTH, startUpFadeTime, bgAlpha, uiTransitionTime, startUpFadeInBackgroundFinishTime;
	unsigned long bgm;
	Uint32 currentTick, startUpTick;
};