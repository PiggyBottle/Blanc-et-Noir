#include "MainGame.h"
#include "BeatPath.h"
#include <bass.h>
#include <bassflac.h>
#include <cmath>
#include <string>
#include <algorithm>
#include <fstream>






MainGame::MainGame(SDL_Renderer *gRenderer, InitVariables var)
{
	this->Renderer = gRenderer;
	this->SCREEN_HEIGHT = var.screen_height;
	this->SCREEN_WIDTH = var.screen_width;
	this->startUpFadeTime = var.maingame_startup_fadein_time;
	this->bgAlpha = var.mainGame_bg_alpha;
	this->uiTransitionTime = var.mainGame_ui_transition_time;
	this->timeBar = { 0,(int)((1.f - var.timeBar_position) * ((float)SCREEN_HEIGHT)) ,SCREEN_WIDTH,var.timeBar_thickness };
	this->initVariables = var;
}

void MainGame::init(Instruction nextInstruction)
{
	//Make object start-up before proceeding with game
	startUpTick = currentTick;
	startUpFadeInBackgroundFinishTime = currentTick;
	uiHasFinishedTransitioning = false;
	
	//Load Textures
	bg = loadTexture("Music/"+nextInstruction.songToLoad + "/bg_blurred.jpg", Renderer);
	if (bg == NULL) { std::cout << "Error loading texture" << std::endl; }
	SDL_SetTextureBlendMode(bg, SDL_BLENDMODE_BLEND);
	note = loadTexture("beatnote.png", Renderer);

	//Load noteHits
	SDL_Color color= { 0,0,0 };
	noteHitTypes[enums::PERFECT] = loadFont(Renderer, initVariables.note_hit_font, 70, "PERFECT", color);
	noteHitTypes[enums::OKAY] = loadFont(Renderer, initVariables.note_hit_font, 70, "OKAY", color);
	noteHitTypes[enums::BREAK] = loadFont(Renderer, initVariables.note_hit_font, 70, "BREAK", color);
	for (int i = 0; i < enums::NO_HIT; i++) {
		SDL_SetTextureBlendMode(noteHitTypes[i].texture, SDL_BLENDMODE_BLEND);
	}

	//Load Beat Map
	beatMap = BeatMap(Renderer, initVariables, nextInstruction,note);

	//Load music/SFX
	std::string songToLoad = "Music/" + nextInstruction.songToLoad + '/' + nextInstruction.songToLoad + ".mp3";
	//Last argument in this function should be replaced with "BASS_SAMPLE_LOOP" flag if you want to repeat
	bgm = BASS_StreamCreateFile(false, songToLoad.c_str(), 0, 0, 0);
	//BASS_ChannelSetPosition(bgm, BASS_ChannelSeconds2Bytes(bgm,20), BASS_POS_BYTE);
	sfx = BASS_StreamCreateFile(false, "brightest-hat.aif", 0, 0, 0);

	mostRecentNoteHit = { false,enums::NO_HIT, 0.0, 0 };
	initted = true;
}

void MainGame::uninit()
{
	if (initted)
	{
		beatMap = BeatMap();
		BASS_StreamFree(bgm);
		BASS_StreamFree(sfx);
		SDL_DestroyTexture(bg);
		SDL_DestroyTexture(note);
		for (int i = 0; i < enums::NO_HIT; i++) { SDL_DestroyTexture(noteHitTypes[i].texture); }
		initted = false;
	}
}

Instruction MainGame::process(SDL_Event e, Instruction nextInstruction)
{
	currentTick = SDL_GetTicks();

	if (!initted) { init(nextInstruction); }

	double currentSongPosition = BASS_ChannelBytes2Seconds(bgm, (BASS_ChannelGetPosition(bgm, BASS_POS_BYTE)));
	std::vector<enums::noteHit> hits;
	if ((e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) && (std::find(initVariables.keyBinds.begin(), initVariables.keyBinds.end(), e.key.keysym.sym) != initVariables.keyBinds.end())) 
	{ 
		hits.push_back(beatMap.processInput(e,currentSongPosition));
	}

	//For mapping purposes
	if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
	{
		uninit();
		return nextInstruction;
	}

	//Blit Background image
	SDL_SetTextureAlphaMod(bg, processBgAlpha());
	SDL_RenderCopyEx(Renderer, bg, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
	
	//Compute beatMap variables
	beatMap.computeVariables(currentSongPosition, &hits);

	//Compute combo
	computeCombo(&hits, currentSongPosition);


	//Play note sound if there was a hit
	if (std::find(hits.begin(), hits.end(), enums::OKAY) != hits.end() || std::find(hits.begin(), hits.end(), enums::PERFECT) != hits.end())
	{
		BASS_ChannelSetPosition(sfx, 0, BASS_POS_BYTE); BASS_ChannelPlay(sfx, false);
	}

	//Blit map
	beatMap.render(currentTick, currentSongPosition, timeBarY);

	//Blit UI
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	if (!uiHasFinishedTransitioning) { timeBarY = processTimeBarY(); timeBar.y = timeBarY - ((int)(0.5f * ((float)timeBar.h)));}
	SDL_RenderFillRect(Renderer, &timeBar);
	//Unfinalized UI design
	SDL_Rect topLeft = { 0,0,SCREEN_WIDTH *5/12,SCREEN_HEIGHT/20 };
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 150);
	SDL_RenderFillRect(Renderer, &topLeft);

	//Blit combo and hits
	renderNoteHitAndCombo(currentSongPosition);



	nextInstruction.quit = false;
	nextInstruction.nextState = enums::MAIN_GAME;


	return nextInstruction;
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
		float offset = initVariables.timeBar_position * SCREEN_HEIGHT * multiplier;
		//printf("%d\n", offset);
		return (int)(SCREEN_HEIGHT - offset);
	}
	else
	{
		uiHasFinishedTransitioning = true;
		BASS_ChannelPlay(bgm, false);
		return ((int)((1.f - initVariables.timeBar_position) * SCREEN_HEIGHT));
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

void MainGame::computeCombo(std::vector<enums::noteHit> *hits, double currentSongPosition)
{
	std::vector<enums::noteHit> hitsAccepted = { enums::BREAK,enums::PERFECT,enums::OKAY };
	for (std::vector<enums::noteHit>::iterator i = hits->begin(); i != hits->end(); ++i)
	{
		if (std::find(hitsAccepted.begin(), hitsAccepted.end(), *i) != hitsAccepted.end())
		{
			mostRecentNoteHit.comboHasBeenUpdated = true;
			if (*i == enums::PERFECT || *i == enums::OKAY) { ++mostRecentNoteHit.combo; }
			else if (*i == enums::BREAK) { mostRecentNoteHit.combo = 0; }
		}
	}
	if (mostRecentNoteHit.comboHasBeenUpdated) {
		mostRecentNoteHit.hit = computeMostRecentNoteHit(hits);
		mostRecentNoteHit.comboUpdateTime = currentSongPosition;
		mostRecentNoteHit.comboHasBeenUpdated = false;
	}
}

enums::noteHit MainGame::computeMostRecentNoteHit(std::vector<enums::noteHit> *hits)
{
	enums::noteHit hit = enums::NO_HIT;
	std::vector<enums::noteHit> hitsAccepted = { enums::BREAK,enums::PERFECT,enums::OKAY };
	for (size_t i = hits->size(); i-- > 0;)
	{
		if (std::find(hitsAccepted.begin(), hitsAccepted.end(), (*hits)[i]) != hitsAccepted.end() && (*hits)[i] != hit)
		{
			hit = (*hits)[i];
			break;
		}
	}
	return hit;
}

void MainGame::renderNoteHitAndCombo(double currentSongPosition) {
	double timeDelta = currentSongPosition - mostRecentNoteHit.comboUpdateTime;
	if (timeDelta > initVariables.combo_and_note_hit_update_buffer || mostRecentNoteHit.hit == enums::NO_HIT) { return; }

	//Compute note hit type
	double widthToHeightRatio = ((double)noteHitTypes[mostRecentNoteHit.hit].width) / ((double)noteHitTypes[mostRecentNoteHit.hit].height);
	float maxHeight = initVariables.note_hit_max_height;
	int height = (int)(maxHeight * ((float)SCREEN_HEIGHT));
	int width =(int)(widthToHeightRatio * ((double)height));
	double timeDeltaPercent = timeDelta / initVariables.combo_and_note_hit_animation_time;
	if (timeDeltaPercent > 1) { timeDeltaPercent = 1; };
	timeDeltaPercent = 1.0 - (timeDeltaPercent * 0.3);
	int modifiedWidth = (int)(((double)width) * timeDeltaPercent);
	int modifiedHeight = (int)(((double)height) * timeDeltaPercent);
	int separatorY = (int)(initVariables.combo_and_note_hit_separator * ((float)SCREEN_HEIGHT));

	//Blit note hit type
	SDL_Rect temp = {(SCREEN_WIDTH - modifiedWidth)/2, separatorY - modifiedHeight, modifiedWidth,modifiedHeight};
	SDL_RenderCopyEx(Renderer, noteHitTypes[mostRecentNoteHit.hit].texture, NULL, &temp, 0, 0, SDL_FLIP_NONE);

	//Compute combo
	SDL_Color color = {0,0,0};
	TextureWithVariables comboTexture = loadFont(Renderer, initVariables.combo_font, 70, std::to_string(mostRecentNoteHit.combo), color);
	widthToHeightRatio = ((double)comboTexture.width) / ((double)comboTexture.height);
	maxHeight = initVariables.combo_max_height;
	height = (int)(maxHeight * ((float)SCREEN_HEIGHT));
	width =(int)(widthToHeightRatio * ((double)height));
	modifiedWidth = (int)(((double)width) * timeDeltaPercent);
	modifiedHeight = (int)(((double)height) * timeDeltaPercent);
	temp.x = (SCREEN_WIDTH - modifiedWidth) / 2;
	temp.y = separatorY - (int)(initVariables.combo_offset_height * ((float)SCREEN_HEIGHT));
	temp.w = modifiedWidth;
	temp.h = modifiedHeight;
	SDL_RenderCopyEx(Renderer, comboTexture.texture, NULL, &temp, 0, 0, SDL_FLIP_NONE);
	SDL_DestroyTexture(comboTexture.texture);
	
}