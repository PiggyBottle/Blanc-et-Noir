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
	this->uiTransitionTime = var.mainGame_ui_transition_time;
	this->timeBar = { 0,5 * (SCREEN_HEIGHT / 6),SCREEN_WIDTH,10 };
}

void MainGame::init(Instruction nextInstruction)
{
	//Make object start-up before proceeding with game
	startingUp = true;
	startUpTick = currentTick;
	startUpFadeInBackgroundFinishTime = currentTick;
	
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

	//Blit Background image
	SDL_SetTextureAlphaMod(bg, processBgAlpha());
	SDL_RenderCopyEx(Renderer, bg, NULL, NULL, 0, NULL, SDL_FLIP_NONE);

	//Blit UI
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	
	timeBar.y = processTimeBarY();
	SDL_RenderFillRect(Renderer, &timeBar);

	if (startingUp)
	{
		int timeSinceStartup = currentTick - startUpTick;
		int timeSinceFinishStartup = currentTick - startUpFadeInBackgroundFinishTime;
		if (timeSinceFinishStartup < uiTransitionTime)
		{ 
			uiTransitionIn(currentTick);
		}
		else { startingUp = false; }
	} else
	{
		
	}
	

	instruction.quit = false;
	instruction.nextState = enums::MAIN_GAME;


	return instruction;
}

int MainGame::processTimeBarY()
{
	int timeRemaining = currentTick - (startUpTick + startUpFadeTime);
	//printf("%d\n", timeRemaining);
	if (timeRemaining < 0) {
		return 0;
	} else if (timeRemaining < uiTransitionTime)
	{
		float a = (1.34 * (float)(std::sin(2.3*((float)timeRemaining / (float)uiTransitionTime))));
		int b = (((float)1) / ((float)6)) * SCREEN_HEIGHT * a;
		printf("%d\n", b);
		return SCREEN_HEIGHT - b;
	}
	else
	{
		return 5 * (SCREEN_HEIGHT / 6);
		
	}
}

Uint8 MainGame::processBgAlpha()
{
	int timeSinceStartup = currentTick - startUpTick;
	if (timeSinceStartup < startUpFadeTime)
	{
		startUpFadeInBackgroundFinishTime = currentTick;
		return (Uint8)(((float)bgAlpha)*((float)timeSinceStartup / (float)startUpFadeTime));
	}
	else { return bgAlpha; }
}


void MainGame::uiTransitionIn(int currentTick)
{

}