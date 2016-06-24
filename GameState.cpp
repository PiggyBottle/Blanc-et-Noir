#include "GameState.h"
#include <iostream>
#include <SDL_image.h>
#include <SDL_ttf.h>
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
SDL_Texture *GameState::loadTexture(std::string path, SDL_Renderer *R) {
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		std::cout << "Unable to load image! SDL_image Error: " << IMG_GetError() << std::endl;
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(R, loadedSurface);
		if (newTexture == NULL)
		{
			std::cout << "Unable to create texture! SDL Error: " << SDL_GetError() << std::endl;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

SDL_Texture *GameState::loadFont(SDL_Renderer *R, std::string fontName, int fontSize, std::string textureText, SDL_Color textColor)
{
	SDL_Texture *newTexture = NULL;
	TTF_Font *font = TTF_OpenFont(fontName.c_str(), fontSize);
	SDL_Surface *textSurface = TTF_RenderText_Blended(font, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(R, textSurface);
		if (newTexture == NULL)
		{
			std::cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
		}
		else {
		}
	}
	SDL_FreeSurface(textSurface);
	TTF_CloseFont(font);
	font = NULL;
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
	screen = loadTexture("startingmenuscreen.jpg",Renderer);
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
		std::cout << "In starting screen" << std::endl;
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
			instruction.nextState = enums::MUSIC_SELECTION;
		}
		else
		{
			instruction.nextState = enums::STARTING_SCREEN;
		}
	}
	return instruction;
}

