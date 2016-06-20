#pragma once
#include <SDL.h>
#include <bass.h>
#include "enums.h"
#include <vector>

class BeatPath
{
public:
	BeatPath(SDL_Renderer *r, float center, int screenWidth, float widthRatio, Uint8 path_highlight_alpha,float note_radius_ratio, StartEnd STARTEND,std::vector<PathMotion> PATHMOTION, std::vector<PathMotion> WIDTHMOTION, std::vector<BeatNote> beat_notes);
	BeatPath();
	~BeatPath();
	void renderPath(Uint32 currentTick, double songPosition, int timeBarY, double beatnote_buffer_time);
private:
	SDL_Renderer *Renderer;
	StartEnd startEnd;
	float pathWidth, noteRadiusRatio;
	//pathCenter is what will be used to keep track of what the path should be at when there are no motions going on
	float pathCenter = 0;
	int SCREEN_WIDTH;
	int pathCenterThickness = 5;
	int currentPathWidth,currentCenterOfPath;
	void computeCenterOfPath(double currentPosition);
	void computePathWidth(double currentPosition);
	float processPathMotionX(PathMotion pathMotion, double currentPosition, float *endpoint);
	RGB pathColor = { 51, 204, 255 };
	bool pathIsOn(double songPosition);
	std::vector<PathMotion> pathMotions,pathWidthMotions;
	Uint8 pathHighlightAlpha;
	std::vector<BeatNote> beatNotes;
	void drawPathCenter(int centerOfPath, int timeBarY);
	void drawBeatNotes(double songPosition, int timeBarY, double beatnote_buffer_time, int center_of_path);
	void drawPathHighlight(double songPosition, int timeBarY);
	void drawBorders(int timeBarY);
	void renderBeatNotes(double songPosition, int timeBarY, double beatnote_buffer_time,int center_of_path, std::vector<BeatNote>::iterator beat_note);
	


};

