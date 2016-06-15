#pragma once
#include "enums.h"
#include <SDL.h>
#include <stdio.h>


class GameState
{
public:
	GameState(SDL_Renderer *gRenderer);
	GameState::GameState();
	~GameState();
	SDL_Texture *loadTexture(const char* path, SDL_Renderer *R);
	virtual Instruction process(SDL_Event e, Instruction nextInstruction) { printf("wrong"); return instruction; };
	virtual void uninit() {};
private:
	Instruction instruction;
	SDL_Renderer *Renderer = NULL;	
};

class StartingScreen: public GameState
{
public:
	StartingScreen(SDL_Renderer *gRenderer);
	StartingScreen();
	~StartingScreen();
	void init();
	void uninit();
	virtual Instruction process(SDL_Event e, Instruction nextInstruction);
private:
	bool initted = false;
	SDL_Texture *screen = NULL;
	long startTime;
	SDL_Renderer *Renderer = NULL;
	Instruction instruction;
};

class MainMenu : public GameState
{
public:
	MainMenu(SDL_Renderer *gRenderer);
	~MainMenu();
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

