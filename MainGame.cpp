#include "MainGame.h"
#include <bass.h>






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