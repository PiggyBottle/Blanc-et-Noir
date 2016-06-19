#include "GameState.h"
#include <stdio.h>
#include <SDL_image.h>
#include <cmath>

GameState::GameState() 
{
}

GameState::GameState(SDL_Renderer *gRenderer)
{
	this->Renderer = gRenderer;
}

GameState::~GameState()
{
}

SDL_Texture *GameState::loadTexture(const char* path, SDL_Renderer *R) {
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(R, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture! SDL Error: %s\n", SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

StartingScreen::StartingScreen()
{
}

StartingScreen::~StartingScreen()
{
}

StartingScreen::StartingScreen(SDL_Renderer *gRenderer, int delay) 
{
	this->Renderer = gRenderer;
	fadeDelay = delay;
}

void StartingScreen::init()
{
	startTime = SDL_GetTicks();
	//Load texture
	const char * z = "startingmenuscreen.jpg";
	screen = loadTexture(z,Renderer);
	SDL_SetTextureBlendMode(screen, SDL_BLENDMODE_BLEND);
	initted = true;
}

void StartingScreen::uninit()
{
	if (initted == true) {
		SDL_DestroyTexture(screen);
		initted = false;
	}
}

Instruction StartingScreen::process(SDL_Event e, Instruction nextInstruction)
{
	if (!initted) {
		init();
		printf("in starting screen");
	}
	else
	{
		int currentTick = SDL_GetTicks();
		//Render texture to screen
		SDL_SetTextureAlphaMod(screen,(Uint8) (255.0 * (((float)currentTick - (float)startTime) / (float)fadeDelay)));
		SDL_RenderCopyEx(Renderer, screen, NULL, NULL, 0, NULL, SDL_FLIP_NONE);


		if (!(currentTick - startTime < fadeDelay))
		{
			uninit();
			instruction.nextState = enums::MAIN_MENU;
		}
		else
		{
			instruction.nextState = enums::STARTING_SCREEN;
		}
	}
	return instruction;
}

MainMenu::MainMenu(SDL_Renderer *gRenderer)
{
	this->Renderer = gRenderer;
}

MainMenu::~MainMenu() 
{
}

void MainMenu::init()
{
	//Load texture
	const char * z = "startingmenuscreen.jpg";
	screen = loadTexture(z, Renderer);
	initted = true;
}

void MainMenu::uninit()
{
	if (initted)
	{
		SDL_DestroyTexture(screen);
		initted = false;
	}
}

Instruction MainMenu::process(SDL_Event e, Instruction nextInstruction)
{
	if (!initted)
	{
		init();
		printf("In main menu");
	}
	SDL_RenderCopyEx(Renderer, screen, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
	instruction.quit = false;
	if (e.type == SDL_KEYDOWN)
	{
		instruction.songToLoad = enums::UNRAVEL;
		instruction.nextState = enums::MAIN_GAME;
		uninit();
	}
	else
	{
		instruction.nextState = enums::MAIN_MENU;
	}
	return instruction;
}
