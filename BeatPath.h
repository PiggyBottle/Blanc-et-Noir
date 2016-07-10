#pragma once
#include <SDL.h>
#include <bass.h>
#include "enums.h"
#include <vector>

class BeatPath
{
public:
	BeatPath(SDL_Renderer *r, SDL_Texture *note, float center, float width, RGB color, InitVariables var, StartEnd STARTEND, std::vector<PathMotion> PATHMOTION, std::vector<PathMotion> WIDTHMOTION,std::vector<ColorMotion> COLORMOTION, std::vector<BeatNote> beat_notes);
	BeatPath();
	~BeatPath();
	void renderPath(double songPosition, int timeBarY, double beatnote_buffer_time);
	bool isOn = false;
	RGB currentPathColor;
	std::vector<int> getCurrentPathWidthCoordinates();
	double getNextBeatTime();
	enums::noteHit registerKey(int key, double songPosition), deregisterKey(int key, double songPosition), computeVariables(double songPosition);
	//Tells functions if a hold-note is being held
	bool isHolding = false;
private:
	SDL_Renderer *Renderer;
	SDL_Texture *noteTexture=NULL;
	StartEnd startEnd;
	float pathWidth, noteRadiusRatio;
	//pathCenter is what will be used to keep track of what the path should be at when there are no motions going on
	float pathCenter = 0;
	int SCREEN_WIDTH;
	int pathCenterThickness = 5;
	int currentPathWidth,currentCenterOfPath;
	void computeCenterOfPath(double currentPosition);
	void computePathWidth(double currentPosition);
	void computePathColor(double currentPosition);
	float processPathMotionX(PathMotion pathMotion, double currentPosition, float *endpoint);
	RGB pathColor, processColorMotion(ColorMotion colorMotion, double currentPosition);
	bool pathIsOn(double songPosition);
	std::vector<PathMotion> pathMotions,pathWidthMotions;
	std::vector<ColorMotion> colorMotions;
	Uint8 pathHighlightAlpha;
	std::vector<BeatNote> beatNotes;
	std::vector<int> registeredKeys;
	void drawPathCenter(int centerOfPath, int timeBarY);
	void drawBeatNotes(double songPosition, int timeBarY, double beatnote_buffer_time, int center_of_path);
	void drawPathHighlight(int timeBarY);
	void drawBorders(int timeBarY);
	void renderBeatNotes(double songPosition, int timeBarY, double beatnote_buffer_time,int center_of_path, std::vector<BeatNote>::iterator beat_note);
	InitVariables initVariables;
	enums::noteHit computeBeatNotes(double songPosition);
	


};

