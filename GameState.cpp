#include "GameState.h"
#include <stdio.h>
#include <SDL_image.h>
#include <bass.h>

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

StartingScreen::StartingScreen(SDL_Renderer *gRenderer) 
{
	this->Renderer = gRenderer;
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
		//Render texture to screen
		SDL_SetTextureAlphaMod(screen, 155);
		SDL_RenderCopyEx(Renderer, screen, NULL, NULL, 0, NULL, SDL_FLIP_NONE);

		if (!(SDL_GetTicks() - startTime < 3000))
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


MainGame::MainGame(SDL_Renderer *gRenderer, int height, int width)
{
	this->Renderer = gRenderer;
	this->SCREEN_HEIGHT = height;
	this->SCREEN_WIDTH = width;
}

void MainGame::init(Instruction nextInstruction)
{
	//Load Texutures
	const char *z = "gamebg.jpg";
	bg = loadTexture(z, Renderer);
	if (bg == NULL) { printf("error loading texture"); }
	SDL_SetTextureBlendMode(bg, SDL_BLENDMODE_BLEND);

	//Load music/SFX

	const char *listOfSongs[enums::TOTAL_SONGS] = {"Unravel.flac", "ForSeasons.flac" };
	//Last argument in this function should be replaced with "BASS_SAMPLE_LOOP" flag if you want to repeat
	bgm = BASS_StreamCreateFile(false, listOfSongs[nextInstruction.songToLoad], 0, 0, 0);
	BASS_ChannelPlay(bgm, false);

	initted = true;
}

void MainGame::uninit()
{
	if (initted)
	{
		SDL_DestroyTexture(bg);
		initted = false;
	}
}

Instruction MainGame::process(SDL_Event e, Instruction nextInstruction)
{
	if (!initted) { init(nextInstruction); }
	SDL_SetTextureAlphaMod(bg, 55);
	SDL_RenderCopyEx(Renderer, bg, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
	SDL_Rect timeBar = { 0,5 * (SCREEN_HEIGHT / 6),SCREEN_WIDTH,10 };
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &timeBar);
	
	instruction.quit = false;
	instruction.nextState = enums::MAIN_GAME;
	
	return instruction;
}