#include "MainGame.h"
#include <bass.h>
#include <cmath>






MainGame::MainGame(SDL_Renderer *gRenderer, InitVariables var)
{
	this->Renderer = gRenderer;
	this->SCREEN_HEIGHT = var.screen_height;
	this->SCREEN_WIDTH = var.screen_width;
	this->startUpFadeTime = var.maingame_startup_fadein_time;
	this->bgAlpha = var.mainGame_bg_alpha;
}

void MainGame::init(Instruction nextInstruction)
{
	//Make object start-up before proceeding with game
	startingUp = true;
	startUpTick = currentTick;
	
	//Load Texutures
	char *z = "gamebg.jpg";
	bg = loadTexture(z, Renderer);
	if (bg == NULL) { printf("error loading texture"); }
	SDL_SetTextureBlendMode(bg, SDL_BLENDMODE_BLEND);

	

	//Load music/SFX

	const char *listOfSongs[enums::TOTAL_SONGS] = { "Unravel.flac", "ForSeasons.flac" };
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
	currentTick = SDL_GetTicks();

	if (!initted) { init(nextInstruction); }

	if (startingUp)
	{
		int timeSinceStartup = currentTick - startUpTick;
		if (timeSinceStartup < startUpFadeTime)
		{
			startUpFadeIn(timeSinceStartup);
		}
		else { startingUp = false; }
	} else
	{
		SDL_SetTextureAlphaMod(bg, bgAlpha);
		SDL_RenderCopyEx(Renderer, bg, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
	}
	
	SDL_Rect timeBar = { 0,5 * (SCREEN_HEIGHT / 6),SCREEN_WIDTH,10 };
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &timeBar);

	instruction.quit = false;
	instruction.nextState = enums::MAIN_GAME;


	return instruction;
}

void MainGame::startUpFadeIn(int timeSinceStartup)
{
	SDL_SetTextureAlphaMod(bg, (Uint8)(((float)bgAlpha)*((float)timeSinceStartup / (float)startUpFadeTime)));
	SDL_RenderCopyEx(Renderer, bg, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
}