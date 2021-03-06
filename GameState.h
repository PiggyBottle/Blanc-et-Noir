#pragma once
#include "enums.h"
#include <SDL.h>
#include <iostream>

class GameState
{
public:
	GameState(SDL_Renderer *gRenderer);
	GameState::GameState();
	~GameState();
	TextureWithVariables loadTexture(std::string path, SDL_Renderer *R, bool to_clip = false, RGB rgb = {0,0,0});
	TextureWithVariables loadFont(SDL_Renderer *R, std::string fontName, int fontSize, std::string textureText, SDL_Color textColor);
	virtual Instruction process(SDL_Event e, Instruction nextInstruction) { std::cout << "wrong" << std::endl; return instruction; };
	virtual void uninit() {};
private:
	Instruction instruction;
	SDL_Renderer *Renderer = NULL;	
};

class StartingScreen: public GameState
{
public:
	StartingScreen(SDL_Renderer *gRenderer, int delay);
	StartingScreen();
	~StartingScreen();
	void init();
	void uninit();
	virtual Instruction process(SDL_Event e, Instruction nextInstruction);
private:
	bool initted = false;
	TextureWithVariables screen;
	long startTime;
	SDL_Renderer *Renderer = NULL;
	Instruction instruction;
	int fadeDelay;
};


