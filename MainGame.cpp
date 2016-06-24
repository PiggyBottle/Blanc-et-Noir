#include "MainGame.h"
#include "BeatPath.h"
#include <bass.h>
#include <bassflac.h>
#include <cmath>
#include <string>






MainGame::MainGame(SDL_Renderer *gRenderer, InitVariables var)
{
	this->Renderer = gRenderer;
	this->SCREEN_HEIGHT = var.screen_height;
	this->SCREEN_WIDTH = var.screen_width;
	this->startUpFadeTime = var.maingame_startup_fadein_time;
	this->bgAlpha = var.mainGame_bg_alpha;
	this->uiTransitionTime = var.mainGame_ui_transition_time;
	this->timeBar = { 0,5 * (SCREEN_HEIGHT / 6),SCREEN_WIDTH,var.timeBar_thickness };
	this->pathWidthRatio = var.path_width_ratio;
	this->initVariables = var;
}

void MainGame::init(Instruction nextInstruction)
{
	//Make object start-up before proceeding with game
	startUpTick = currentTick;
	startUpFadeInBackgroundFinishTime = currentTick;
	uiHasFinishedTransitioning = false;
	
	//Load Textures
	bg = loadTexture("gamebg2.jpg", Renderer);
	if (bg == NULL) { std::cout << "Error loading texture" << std::endl; }
	SDL_SetTextureBlendMode(bg, SDL_BLENDMODE_BLEND);

	//Load Beat Map
	beatMap = BeatMap(Renderer, initVariables, nextInstruction.gameKeys);

	

	//Load music/SFX
	std::string songToLoad = "Music/" + nextInstruction.songToLoad + '/' + nextInstruction.songToLoad + ".flac";
	//Last argument in this function should be replaced with "BASS_SAMPLE_LOOP" flag if you want to repeat
	bgm = BASS_FLAC_StreamCreateFile(false, songToLoad.c_str(), 0, 0, 0);
	//BASS_ChannelSetPosition(bgm, 1000000, BASS_POS_BYTE);

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
	double currentSongPosition = BASS_ChannelBytes2Seconds(bgm, (BASS_ChannelGetPosition(bgm, BASS_POS_BYTE)));

	if ((e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) && (std::find(initVariables.keyBinds.begin(), initVariables.keyBinds.end(), e.key.keysym.sym) != initVariables.keyBinds.end())) 
	{ 
		std::cout << currentSongPosition << std::endl;
		enums::noteHit hit = beatMap.processInput(e,currentSongPosition); 
		if (hit == enums::PERFECT) { std::cout << "Perfect!" << std::endl; }
		else if (hit == enums::OKAY) { std::cout << "Okay" << std::endl; }
		else if (hit == enums::MISS) { std::cout << "BREAK" << std::endl; }
		//else if (hit == enums::NO_HIT) { std::cout << "No Hit" << std::endl; }
	}
	//Blit Background image
	SDL_SetTextureAlphaMod(bg, processBgAlpha());
	SDL_RenderCopyEx(Renderer, bg, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
	
	//Compute beatMap variables
	std::vector<enums::noteHit> hits = beatMap.computeVariables(currentSongPosition);
	for (auto i = hits.begin(); i!=hits.end(); ++i)
	{
		switch (*i) {
		case enums::PERFECT: std::cout << "PERFECT" << std::endl; break;
		case enums::MISS: std::cout << "BREAK" << std::endl; break;
	} }

	//Blit map
	beatMap.render(currentTick, currentSongPosition, timeBarY);

	//Blit UI
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	if (!uiHasFinishedTransitioning) { timeBarY = processTimeBarY(); timeBar.y = timeBarY - ((int)(0.5f * ((float)timeBar.h)));}
	SDL_RenderFillRect(Renderer, &timeBar);

	instruction.quit = false;
	instruction.nextState = enums::MAIN_GAME;


	return instruction;
}

int MainGame::processTimeBarY()
{
	int timeRemaining = currentTick - (startUpTick + startUpFadeTime);
	if (timeRemaining < 0) {
		return SCREEN_HEIGHT;
	} else if (timeRemaining < uiTransitionTime)
	{
		//y = 1.34 sin(2.3x). y = 1 when x = 1
		//Animation speed can be changed by altering ui transition time in main.cpp
		float multiplier = (((float)1.34) * (float)(std::sin(2.3*((float)timeRemaining / (float)uiTransitionTime))));
		float offset = (((float)1) / ((float)6)) * SCREEN_HEIGHT * multiplier;
		//printf("%d\n", offset);
		return (int)(SCREEN_HEIGHT - offset);
	}
	else
	{
		uiHasFinishedTransitioning = true;
		BASS_ChannelPlay(bgm, false);
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


