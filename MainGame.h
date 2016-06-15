#pragma once
#include "enums.h"
#include "GameState.h"
#include <SDL.h>




class MainGame : public GameState
{
public:
	MainGame(SDL_Renderer *gRenderer, int height, int width);
	void uninit();
	Instruction process(SDL_Event e, Instruction nextInstruction);
private:
	void init(Instruction nextInstruction);
	SDL_Renderer *Renderer;
	bool initted = false;
	Instruction instruction;
	SDL_Texture *bg = NULL;
	int SCREEN_HEIGHT, SCREEN_WIDTH;
	unsigned long bgm;
};