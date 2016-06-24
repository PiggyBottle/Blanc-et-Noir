#pragma once
#include "GameState.h"
class MusicSelection :
	public GameState
{
public:
	MusicSelection(SDL_Renderer *gRenderer);
	~MusicSelection();
	void init();
	void uninit();
	Instruction process(SDL_Event e, Instruction nextInstruction);
private:
	Instruction instruction;
	bool initted = false;
	SDL_Renderer *Renderer = NULL;

	//Temporary texture
	SDL_Texture *screen = NULL;
};
