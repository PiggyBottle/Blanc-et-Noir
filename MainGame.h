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
	void init(Instruction nextInstruction);
	Uint8 processBgAlpha();
	Uint32 currentTick, startUpTick;
	SDL_Renderer *Renderer;
	SDL_Texture *bg = NULL, *note = NULL;
	SDL_Rect timeBar;
	Instruction instruction;
	bool initted = false;
	int SCREEN_HEIGHT, SCREEN_WIDTH, startUpFadeTime, bgAlpha, uiTransitionTime, startUpFadeInBackgroundFinishTime;
	int processTimeBarY();
	unsigned long bgm;
	float pathWidthRatio;
};