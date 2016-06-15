#pragma once
#include <SDL.h>
#include <memory>
#include "enums.h"
#include "GameState.h"



class Game
{
public:
	Game(InitVariables var);
	~Game();
	bool init();
	void uninit();
	bool newFrame(SDL_Event e);
private:
	int SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_FPS;
	SDL_Window *gWindow;
	SDL_Renderer *gRenderer;
	GameState *states[enums::TOTAL_STATES];
	int currentState;
	Instruction nextInstruction, previousInstruction;
	void uninitializeAllStates();
	Uint32 msPerFrame, prevFrameTime, currentFrameTime;
	InitVariables initVariables;


};

