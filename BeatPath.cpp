#include "BeatPath.h"
#include <SDL2_gfxPrimitives.h>
#include <algorithm>
#include <cmath>

BeatPath::BeatPath() {}
BeatPath::BeatPath(SDL_Renderer *r, SDL_Texture *note, float center, float width, RGB color, InitVariables var, StartEnd STARTEND, std::vector<PathMotion> PATHMOTION, std::vector<PathMotion> WIDTHMOTION,std::vector<ColorMotion> COLORMOTION, std::vector<BeatNote> beat_notes)
{
	this->Renderer = r;
	this->pathCenter = center;
	this->pathColor = color;
	this->currentPathColor = color;
	this->SCREEN_WIDTH = var.screen_width;
	this->pathWidth = width;
	this->pathHighlightAlpha = var.path_highlight_alpha;
	this->noteRadiusRatio = var.note_radius_ratio;
	this->initVariables = var;
	this->noteTexture = note;
	beatNotes = beat_notes;
	
	startEnd = STARTEND;
	pathMotions = PATHMOTION;
	pathWidthMotions = WIDTHMOTION;
	colorMotions = COLORMOTION;

}


BeatPath::~BeatPath()
{
}

enums::noteHit BeatPath::computeVariables(double songPosition)
{
	computeCenterOfPath(songPosition);
	computePathWidth(songPosition);
	computePathColor(songPosition);
	isOn = pathIsOn(songPosition);
	return computeBeatNotes(songPosition);
}

void BeatPath::renderPath(double songPosition, int timeBarY, double beatnote_buffer_time)
{
	if (!isOn) { return; }

	//Highlight path
	drawPathHighlight(timeBarY);

	//Draw Path Center
	drawPathCenter(currentCenterOfPath, timeBarY);

	//Draw borders
	drawBorders(timeBarY);

	//Draw beat notes
	drawBeatNotes(songPosition, timeBarY, beatnote_buffer_time, currentCenterOfPath);

}

std::vector<int> BeatPath::getCurrentPathWidthCoordinates()
{
	std::vector<int>start_end(2);
	start_end[0] = currentCenterOfPath - currentPathWidth;
	start_end[1] = currentCenterOfPath + currentPathWidth;

	return start_end;
}

double BeatPath::getNextBeatTime()
{
	if (!beatNotes.empty()) {
		return beatNotes[0].start_position;
	}
	else { return -1.0; }
}

enums::noteHit BeatPath::registerKey(int key, double songPosition)
{
	//Add key to registeredKeys
	if (std::find(registeredKeys.begin(), registeredKeys.end(), key) == registeredKeys.end()) { registeredKeys.push_back(key); }

	enums::noteHit hit = enums::NO_HIT;
	//If there is already a hold, then theres nothing else to be calculated.
	if (isHolding) { return hit; }

	//Calculate hit accuracy.
	//The fact that this function is called means there is a guaranteed hit
	double beatNoteTimeDelta = std::abs(beatNotes[0].start_position - songPosition);
	if (beatNoteTimeDelta <= initVariables.perfect_hit_buffer_time)
	{
		hit = enums::PERFECT;
	}
	else if (beatNoteTimeDelta <= initVariables.okay_hit_buffer_time)
	{
		hit = enums::OKAY;
	}
	if (beatNotes[0].note_type == enums::SINGLE_HIT){ beatNotes.erase(beatNotes.begin()); }
	else if (beatNotes[0].note_type == enums::SINGLE_HOLD) { isHolding = true; }

	return hit;
}

enums::noteHit BeatPath::deregisterKey(int key, double songPosition)
{
	//Might need to check if this works 100%
	if (std::find(registeredKeys.begin(), registeredKeys.end(), key) != registeredKeys.end()) 
	{ 
		registeredKeys.erase(std::remove(registeredKeys.begin(), registeredKeys.end(), key), registeredKeys.end());
	}
	
	enums::noteHit hit = enums::NO_HIT;
	if ((!isHolding) || (!registeredKeys.empty())) { return hit; }

	if (beatNotes[0].end_position - songPosition > initVariables.okay_hit_buffer_time)
	{
		hit = enums::BREAK;
	}
	else if (beatNotes[0].end_position - songPosition > initVariables.perfect_hit_buffer_time)
	{
		hit = enums::OKAY;
	}
	else { hit = enums::PERFECT; }
	
	isHolding = false;
	beatNotes.erase(beatNotes.begin());
	return hit;

}

void BeatPath::drawBorders(int timeBarY)
{
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_Rect border = { 0,0,1,timeBarY };
	border.x = currentCenterOfPath - currentPathWidth;
	SDL_RenderFillRect(Renderer, &border);
	border.x = currentCenterOfPath + currentPathWidth;
	SDL_RenderFillRect(Renderer, &border);

}

void BeatPath::computePathColor(double currentPosition)
{
	for (std::vector<ColorMotion>::iterator i = colorMotions.begin(); i != colorMotions.end(); ++i)
	{
		//Including '=' to account for 'instant color-changes'
		if (i->end_position <= currentPosition)
		{
			currentPathColor = i->endColor;
		}
		else if (i->start_position < currentPosition && currentPosition < i->end_position)
		{
			currentPathColor = processColorMotion(*i, currentPosition);
			return;
		}
	}
}

RGB BeatPath::processColorMotion(ColorMotion colorMotion, double currentPosition)
{
	pathColor = colorMotion.startColor;
	RGB color = { 255,255,255 };
	if (colorMotion.motion == enums::LINEAR_SLIDE)
	{
		int start[3] = { colorMotion.startColor.r, colorMotion.startColor.g, colorMotion.startColor.b };
		int end[3] = { colorMotion.endColor.r, colorMotion.endColor.g, colorMotion.endColor.b };
		int *final[3] = { &color.r, &color.g, &color.b };
		for (int i = 0; i < 3; ++i)
		{
			*final[i] = start[i] + ((int)(((currentPosition - colorMotion.start_position) / (colorMotion.end_position - colorMotion.start_position)) * ((double)(end[i] - start[i]))));
		}
		return color;
	}
	return color;
}

enums::noteHit BeatPath::computeBeatNotes(double songPosition)
{
	enums::noteHit hit = enums::NO_HIT;
	bool thereAreExpiredNotes = false;
	if (!beatNotes.empty()) {
		thereAreExpiredNotes = (beatNotes[0].note_type == enums::SINGLE_HIT && songPosition - beatNotes[0].start_position > initVariables.okay_hit_buffer_time) || (beatNotes[0].note_type == enums::SINGLE_HOLD && (songPosition > beatNotes[0].end_position || (songPosition - beatNotes[0].start_position > initVariables.okay_hit_buffer_time && !isHolding)));
	}
	//Delete expired notes and register a break
	while (thereAreExpiredNotes)
	{ 
		if (beatNotes[0].note_type == enums::SINGLE_HIT || (beatNotes[0].note_type == enums::SINGLE_HOLD && !isHolding)) { hit = enums::BREAK; }
		else if (beatNotes[0].note_type == enums::SINGLE_HOLD && songPosition > beatNotes[0].end_position  && isHolding) { hit = enums::PERFECT; }
		isHolding = false;
		beatNotes.erase(beatNotes.begin());
		if (!beatNotes.empty())
		{
		thereAreExpiredNotes = (beatNotes[0].note_type == enums::SINGLE_HIT && songPosition - beatNotes[0].start_position > initVariables.okay_hit_buffer_time) || (beatNotes[0].note_type == enums::SINGLE_HOLD && (songPosition > beatNotes[0].end_position || (songPosition - beatNotes[0].start_position > initVariables.okay_hit_buffer_time && !isHolding)));
		}
		else { thereAreExpiredNotes = false; }
	}
	return hit;
}

void BeatPath::drawBeatNotes(double songPosition, int timeBarY, double beatnote_buffer_time, int center_of_path)
{

	for (std::vector<BeatNote>::iterator i = beatNotes.begin(); i != beatNotes.end(); ++i)
	{

		//Draw active notes
		if (i->start_position - songPosition > beatnote_buffer_time) { break; }
		renderBeatNotes(songPosition, timeBarY, beatnote_buffer_time, center_of_path, i);
	}
}


void BeatPath::renderBeatNotes(double songPosition, int timeBarY, double beatnote_buffer_time, int center_of_path, std::vector<BeatNote>::iterator beat_note)
{
	
	Sint16 x[4], y[4], noteCenterY;

	//Compute noteCenterY
	double noteToBufferRatio = (beat_note->start_position - songPosition) / (beatnote_buffer_time);
	if (noteToBufferRatio < 0) { 
		noteToBufferRatio = 0; 
	}
	noteCenterY = (Sint16) (((double)(timeBarY)) * (((double)1) - noteToBufferRatio));

	//Experimental usage of rings as notes
	int noteRadius = (int)(noteRadiusRatio * ((float)SCREEN_WIDTH));
	if (beat_note->note_type == enums::SINGLE_HIT) {
		SDL_Rect noteRect = { center_of_path - noteRadius,noteCenterY - noteRadius,noteRadius * 2,noteRadius * 2 };
		SDL_RenderCopyEx(Renderer, noteTexture, NULL, &noteRect, NULL, 0, SDL_FLIP_NONE);
	}
	else { SDL_Rect noteRect = {center_of_path-noteRadius,noteCenterY,noteRadius*2,noteRadius};
	SDL_Rect partialRect = {0,50,100,50};
	SDL_RenderCopyEx(Renderer, noteTexture, &partialRect, &noteRect, NULL, 0, SDL_FLIP_NONE);
	}

	////Compute Vertex Array
	//x[0] = (Sint16)center_of_path;
	//y[0] = noteCenterY - ((Sint16)(noteRadiusRatio * ((float)SCREEN_WIDTH)));
	//x[1] = (Sint16)center_of_path - ((Sint16)(noteRadiusRatio * ((float)SCREEN_WIDTH)));
	//y[1] = noteCenterY;
	//x[2] = (Sint16)center_of_path;
	//y[2] = noteCenterY + ((Sint16)(noteRadiusRatio * ((float)SCREEN_WIDTH)));
	//x[3] = (Sint16)center_of_path + ((Sint16)(noteRadiusRatio * ((float)SCREEN_WIDTH)));
	//y[3] = noteCenterY;

	////Render note
	//filledPolygonRGBA(Renderer,x,y,4,0,0,0,255);
	


	//That's all that's needed for SINGLE_HITs.
	if (beat_note->note_type == enums::SINGLE_HIT) { return; }

	//Blitting for SINGLE_HOLDs.
	double holdNoteToBufferRatio = (beat_note->end_position - songPosition) / (beatnote_buffer_time);
	if (holdNoteToBufferRatio > 1) { holdNoteToBufferRatio = 1; }
	if (holdNoteToBufferRatio < 0) { holdNoteToBufferRatio = 0; }
	int holdNoteCenterY = (int) (((double)(timeBarY)) * (((double)1) - holdNoteToBufferRatio));
	/*SDL_Rect holdRect = { center_of_path - ((int)(noteRadiusRatio * ((float)SCREEN_WIDTH))), holdNoteCenterY, (int)(2.f * ((float)SCREEN_WIDTH) * noteRadiusRatio), ((int)noteCenterY) - holdNoteCenterY };
	SDL_RenderFillRect(Renderer, &holdRect);*/
	
	//Experimental usage of rings as notes
	float ratio = 0.25f;
	int thickness = (int)(ratio*((float)noteRadius));
	SDL_Rect holdRect = { center_of_path - noteRadius,holdNoteCenterY,thickness,((int)noteCenterY) - holdNoteCenterY };
	SDL_RenderFillRect(Renderer, &holdRect);
	holdRect.x = center_of_path + noteRadius - thickness;
	SDL_RenderFillRect(Renderer, &holdRect);
	SDL_Rect partialRect = { 0,0,100,50 }, noteRect = {center_of_path-noteRadius,holdNoteCenterY-noteRadius,2*noteRadius,noteRadius};
	SDL_RenderCopyEx(Renderer, noteTexture, &partialRect, &noteRect, NULL, 0, SDL_FLIP_NONE);




	



}

void BeatPath::drawPathHighlight(int timeBarY)
{
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
	SDL_Rect highlight = { 0,0,0,timeBarY };
	highlight.x = currentCenterOfPath - currentPathWidth;
	highlight.w = 2 * currentPathWidth;
	SDL_SetRenderDrawColor(Renderer, currentPathColor.r, currentPathColor.g, currentPathColor.b, 155);
	SDL_RenderFillRect(Renderer, &highlight);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

}

void BeatPath::computePathWidth(double currentPosition)
{
	for (std::vector<PathMotion>::iterator i = pathWidthMotions.begin(); i != pathWidthMotions.end(); ++i)
	{
		if (i->end_position < currentPosition)
		{
			pathWidth = i->end_x;
		}
		else if (i->start_position < currentPosition && currentPosition < i->end_position)
		{
			currentPathWidth =(int)((((float)SCREEN_WIDTH) * processPathMotionX(*i, currentPosition, &pathWidth)));
			return;
		}
	}
	currentPathWidth = (int)(((float)SCREEN_WIDTH) * pathWidth);
}


void BeatPath::computeCenterOfPath(double currentPosition)
{
	for (std::vector<PathMotion>::iterator i = pathMotions.begin(); i != pathMotions.end(); ++i)
	{ 
		if (i->end_position < currentPosition)
		{
			pathCenter = i->end_x;
		}
		else if (i->start_position < currentPosition && currentPosition < i->end_position)
		{
			currentCenterOfPath =(int)((((float)SCREEN_WIDTH) * processPathMotionX(*i, currentPosition, &pathCenter)));
			return;
		}
	}


	currentCenterOfPath =(int)( (((float)SCREEN_WIDTH)*(pathCenter)));
}

void BeatPath::drawPathCenter(int centerOfPath, int timeBarY)
{
	SDL_Rect rect = {centerOfPath - ((int)(0.5f*((float)pathCenterThickness))),0,pathCenterThickness,timeBarY};
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	//SDL_RenderFillRect(Renderer, &rect);
}


float BeatPath::processPathMotionX(PathMotion pathMotion, double currentPosition, float *endpoint)
{
	if (pathMotion.motion == enums::LINEAR_SLIDE)
	{
		*endpoint = pathMotion.end_x;
		return (((float)pathMotion.start_x) + (((float)(pathMotion.end_x - pathMotion.start_x)) * (((float)(currentPosition - pathMotion.start_position))/((float)(pathMotion.end_position - pathMotion.start_position)))));
	}
	else if (pathMotion.motion == enums::HALF_SINE_SLIDE)
	{
		*endpoint = pathMotion.end_x;
		return (float)(pathMotion.start_x + (std::sin(((currentPosition - pathMotion.start_position) / (pathMotion.end_position - pathMotion.start_position)) * 1.5708) * (pathMotion.amplitude - pathMotion.start_x)));
	}
	else if (pathMotion.motion == enums::FULL_SINE_SLIDE)
	{
		*endpoint = pathMotion.end_x;
		return (float)(pathMotion.start_x + (std::sin(((currentPosition - pathMotion.start_position) / (pathMotion.end_position - pathMotion.start_position)) * 3.141) * (pathMotion.amplitude - pathMotion.start_x)));
	}
	return 0;
}
bool BeatPath::pathIsOn(double songPosition)
{
	std::vector<double>::const_iterator a, b;
	a = startEnd.start.begin();
	b = startEnd.end.begin();
	while (a != startEnd.start.end() && b != startEnd.end.end()) 
	{
		if (songPosition > *a && songPosition < *b)
		{
			return true;
		}
		++a; ++b;
	}
	return false;
}