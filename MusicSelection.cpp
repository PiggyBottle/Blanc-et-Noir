#include "MusicSelection.h"
#include <bass.h>
#include <fstream>



MusicSelection::MusicSelection(SDL_Renderer *gRenderer, InitVariables iV)
{
	this->Renderer = gRenderer;
	initVariables = iV;
}


MusicSelection::~MusicSelection()
{
}

void MusicSelection::init()
{
	std::cout << "In main menu" << std::endl;

	//Generate beatmaps and indices
	std::vector<int> bMWK;
	for (int i = 0; i < enums::TOTAL_KEYS; i++) { beatMapsWithKey.push_back(bMWK); }
	getBeatMaps();

	//Generate panels
	calculateMaxNumberOfPanels();
	currentSelectedMusicIndex = 0;
	currentNumberOfKeysSelected = enums::SIX_KEYS;
	currentSelectedDifficulty = enums::EASY;
	generateListOfPanels();

	//Generate buttons
	generateClickableButtons();

	//Load background
	freeAndChangebg();
	//Load album art
	freeAndChangeAlbumArt();
	//Load music
	freeAndChangebgm();
	checkThatMusicIsPlayingWithinRange();

	//To prevent selectionBar from jumping around when just initted
	mouseX = initVariables.screen_width;
	mouseIsBeingDragged = false;
	mouseIsClicked = false;
	panelAreaIsClicked = false;
	selectionIsMinimized = true;
	selectionBarTransitionTime = 0.0;
	
	proceedToMainGame = false;
	initted = true;
}

void MusicSelection::uninit()
{
	if (initted)
	{
		SDL_DestroyTexture(bg.texture);
		SDL_DestroyTexture(albumArt.texture);
		for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
		{
			SDL_DestroyTexture(i->songTitleTexture.texture);
		}
		for (std::vector<MusicSelectionClickableButton>::iterator i = clickableButtons.begin(); i != clickableButtons.end(); ++i)
		{
			SDL_DestroyTexture(i->fontTexture.texture);
			SDL_DestroyTexture(i->backgroundTexture.texture);
		}

		BASS_StreamFree(bgm);

		initted = false;
	}
}

Instruction MusicSelection::process(SDL_Event e, Instruction nextInstruction)
{
	if (!initted)
	{
		init();
	}

	double currentTick = ((double)SDL_GetTicks()) / 1000.0;

	//Process event
	processEvent(e);

	//Check if can proceed to main game
	if (proceedToMainGame) { uninit();  return generateInstructionForMainGame(); }

	//Compute selection bar x
	computeSelectionBarX(currentTick);

	//Compute panel y
	computePanelY();

	//Repeat music if necessary
	checkThatMusicIsPlayingWithinRange();

	//Render background
	SDL_RenderCopy(Renderer, bg.texture, NULL, NULL);

	//Render clickable buttons
	renderClickableButtons();

	//Render Album Art
	renderAlbumArt();

	//Render panels
	renderPanels();

	//Render selection bar
	SDL_Rect selectionBar = {selectionBarX-(initVariables.musicSelection_bar_thickness/2),0,initVariables.musicSelection_bar_thickness,initVariables.screen_height};
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &selectionBar);

	instruction.nextState = enums::MUSIC_SELECTION;
	return instruction;


}

void MusicSelection::computeSelectionBarX(double currentTick)
{
	if (mouseX < selectionBarX && selectionIsMinimized == true)
	{
		selectionBarTransitionTime = currentTick;
		selectionIsMinimized = false;
	}
	else if (mouseX > selectionBarX && selectionIsMinimized == false)
	{
		selectionBarTransitionTime = currentTick;
		selectionIsMinimized = true;
	}

	if (selectionIsMinimized) 
	{	
		if (currentTick - selectionBarTransitionTime > initVariables.musicSelection_bar_transition_time)
		{
			selectionBarX = (int)(initVariables.musicSelection_bar_minimized_x * ((float)initVariables.screen_width));
		}
		else
		{
			selectionBarX = (int)(initVariables.screen_width * ((float)(initVariables.musicSelection_bar_maximized_x + (std::sin(((currentTick - selectionBarTransitionTime) / (initVariables.musicSelection_bar_transition_time)) * 1.5708) * (initVariables.musicSelection_bar_minimized_x - initVariables.musicSelection_bar_maximized_x)))));

		}
	}
	else if (!selectionIsMinimized) 
	{
		if (currentTick - selectionBarTransitionTime > initVariables.musicSelection_bar_transition_time)
		{
			selectionBarX = (int)(initVariables.musicSelection_bar_maximized_x * ((float)initVariables.screen_width));
		}
		else
		{
			selectionBarX = (int)(initVariables.screen_width * ((float)(initVariables.musicSelection_bar_minimized_x + (std::sin(((currentTick - selectionBarTransitionTime) / (initVariables.musicSelection_bar_transition_time)) * 1.5708) * (initVariables.musicSelection_bar_maximized_x - initVariables.musicSelection_bar_minimized_x)))));
		}
	}
}

void MusicSelection::computePanelY()
{
	if (!(panelAreaIsClicked && mouseIsClicked && mouseIsBeingDragged && (!listOfPanels.panels.empty()))) { return; }
	
	int mouseDelta = mouseY - lastClickedY;
	for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
	{
		i->centerY = i->previousCenterY + mouseDelta;
	}

	//Generate new panels on top/bottom to fill the gaps if possible
	while (listOfPanels.panels.front().centerY - listOfPanels.panels.front().width > 0 && listOfPanels.front > 0)
	{
		listOfPanels.back--;
		listOfPanels.panels.pop_back();
		listOfPanels.front--;
		int centerYofFront = listOfPanels.panels.front().centerY;
		listOfPanels.panels.push_front(generateMusicSelectionPanel(beatMapsWithKey[currentNumberOfKeysSelected][listOfPanels.front]));
		listOfPanels.panels.front().centerY = centerYofFront - (2* listOfPanels.panels.front().width) - ((int)(initVariables.musicSelection_panel_separation * ((float)initVariables.screen_height)));
		listOfPanels.panels.front().previousCenterY = listOfPanels.panels.front().centerY - mouseDelta;
	}
	while (listOfPanels.panels.back().centerY + listOfPanels.panels.back().width < initVariables.screen_height && listOfPanels.back < ((int)beatMapsWithKey[currentNumberOfKeysSelected].size()) - 1)
	{
		listOfPanels.back++;
		listOfPanels.panels.pop_front();
		int centerYofBack = listOfPanels.panels.back().centerY;
		listOfPanels.front++;
		listOfPanels.panels.push_back(generateMusicSelectionPanel(beatMapsWithKey[currentNumberOfKeysSelected][listOfPanels.back]));
		listOfPanels.panels.back().centerY = centerYofBack + (2* listOfPanels.panels.back().width) + ((int)(initVariables.musicSelection_panel_separation * ((float)initVariables.screen_height)));
		listOfPanels.panels.back().previousCenterY = listOfPanels.panels.back().centerY - mouseDelta;
	}

	assertThatPanelCornersDontCrossLimit();
}

void MusicSelection::processEvent(SDL_Event e)
{
	if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
	{
		mouseX = e.motion.x;
		mouseY = e.motion.y;
	}

	if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
	{
		lastClickedX = mouseX;
		lastClickedY = mouseY;
		mouseIsClicked = true;
		if (mouseX < selectionBarX) { panelAreaIsClicked = true; backupPanelY(); }
		else { panelAreaIsClicked = false; checkIfClickableButtonIsPressed(); }
	}
	else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
	{
		//Check if the click happened in the panel area
		if (mouseIsClicked && (!mouseIsBeingDragged) && mouseX < selectionBarX) { selectPanel(); }

		mouseIsBeingDragged = false;
		mouseIsClicked = false;
		panelAreaIsClicked = false;
	}
	else if (e.type == SDL_MOUSEMOTION)
	{
		if (mouseIsClicked)
		{
			//To prevent undefined behaviour when mouse is dragged into/out of selection bar
			if ((panelAreaIsClicked && mouseX > selectionBarX) || (!panelAreaIsClicked && mouseX < selectionBarX))
			{
				mouseIsBeingDragged = false;
				mouseIsClicked = false;
				panelAreaIsClicked = false;
			}
			else if ((panelAreaIsClicked && mouseX < selectionBarX) || (!panelAreaIsClicked && mouseX > selectionBarX))
			{
				mouseIsBeingDragged = true;
			}
		}
	}
}

void MusicSelection::getBeatMaps()
{
	boost::filesystem::path p(".\\Music");
	boost::filesystem::directory_iterator end_itr;
	std::vector<std::string> roots;
	beatMaps.clear();
	int currentBeatMapIndex = 0;
	if (boost::filesystem::exists(p))
	{
		for (boost::filesystem::directory_iterator root(p); root != end_itr; ++root)
		{
			if (boost::filesystem::is_directory(root->path()) && boost::filesystem::exists(root->path() / "songinfo.txt" ))
			{
				MusicFileSystem fileSystem;
				fileSystem.beatMapRootFolder = root->path().string();
				getSongInfo(root->path().string() + "/songinfo.txt", &fileSystem);
				fileSystem.difficultyAndKeys = checkForBeatMaps(root->path(), currentBeatMapIndex);
				beatMaps.push_back(fileSystem);
				++currentBeatMapIndex;
			}
		}
	}
}

std::vector<BeatMapKeyAndDifficulty> MusicSelection::checkForBeatMaps(boost::filesystem::path path, int index)
{
	std::vector<BeatMapKeyAndDifficulty> bMKAD;
	bMKAD.clear();
	std::vector<std::string> keys = { "4key","6key" };
	bool fourKeyAlreadyAdded = false, sixKeyAlreadyAdded = false;
	std::vector<std::string> difficulties = { "Easy","Normal","Hard","Extreme" };
	for (std::vector<std::string>::iterator i = keys.begin(); i != keys.end(); ++i)
	{
		//Check keys
		if (boost::filesystem::exists(path / *i))
		{
			for (std::vector<std::string>::iterator j = difficulties.begin(); j != difficulties.end(); ++j)
			{
				if (boost::filesystem::exists(path / *i / *j))
				{
					if (!boost::filesystem::is_empty(path / *i / *j))
					{
						BeatMapKeyAndDifficulty keyAndDifficulty;
						if (*i == "4key") 
						{ 
							keyAndDifficulty.numberOfKeys = enums::FOUR_KEYS;
							if (!fourKeyAlreadyAdded)
							{
								beatMapsWithKey[enums::FOUR_KEYS].push_back(index);
								fourKeyAlreadyAdded = true;
							}
						}
						else if (*i == "6key") 
						{ 
							keyAndDifficulty.numberOfKeys = enums::SIX_KEYS; 
							if (!sixKeyAlreadyAdded)
							{
								beatMapsWithKey[enums::SIX_KEYS].push_back(index); 
								sixKeyAlreadyAdded = true;
							}
						}

						if (*j == "Easy") { keyAndDifficulty.difficulty = enums::EASY; }
						else if (*j == "Normal") { keyAndDifficulty.difficulty = enums::NORMAL; }
						else if (*j == "Hard") { keyAndDifficulty.difficulty = enums::HARD; }
						else if (*j == "Extreme") { keyAndDifficulty.difficulty = enums::EXTREME; }
						bMKAD.push_back(keyAndDifficulty);
					}
				}
			}
		}
	}
	return bMKAD;
}

void MusicSelection::getSongInfo(std::string path, MusicFileSystem *fileSystem)
{
	std::ifstream file(path);
	std::string line;
	//Default string to use when there is no album art specified
	fileSystem->albumArtFileName = "No Image";

	while (std::getline(file, line))
	{
		if (line == "#songname")
		{
			std::string line2;
			std::getline(file, line2);
			fileSystem->songName = line2;
		}
		else if (line == "#bgfilename")
		{
			std::string line2;
			std::getline(file, line2);
			fileSystem->bgFileName = line2;
		}
		else if (line == "#musicfilename")
		{
			std::string line2;
			std::getline(file, line2);
			fileSystem->musicFileName = line2;
		}
		else if (line == "#musicstartend")
		{
			std::string line2;
			std::getline(file, line2);
			fileSystem->musicStartPosition = std::stod(line2);
			std::getline(file, line2);
			fileSystem->musicEndPosition = std::stod(line2);
		}
		else if (line == "#albumartfilename")
		{
			std::string line2;
			std::getline(file, line2);
			fileSystem->albumArtFileName = line2;
		}
	}
	return;
}

void MusicSelection::calculateMaxNumberOfPanels()
{
	maxNumberOfPanels = ((int)floor(1.f / (initVariables.musicSelection_panel_separation + (2.f * initVariables.musicSelection_panel_width)))) + 1;
}

void MusicSelection::generateClickableButtons()
{
	MusicSelectionClickableButton button;
	//Note: Don't destroy generated textures that are created here! They are removed during uninit

	//Generate number of keys buttons
	{
		button.x = (int)(initVariables.musicSelection_gameKeys_button_x * ((float)initVariables.screen_width));
		button.y = (int)(initVariables.musicSelection_gameKeys_button_y * ((float)initVariables.screen_height));
		button.width = (int)(initVariables.musicSelection_gameKeys_button_width * ((float)initVariables.screen_width));
		button.height = (int)(initVariables.musicSelection_gameKeys_button_height * ((float)initVariables.screen_height));
		button.backgroundTexture.texture = NULL;
		button.text = "4 Key";
		SDL_Color color = { 255,255,255 };
		button.fontTexture = loadFont(Renderer, initVariables.musicSelection_font, 48, button.text, color);
		clickableButtons.push_back(button);

		button.x += button.width + ((int)(initVariables.musicSelection_gameKeys_button_separation * ((float)initVariables.screen_width)));
		button.text = "6 Key";
		button.fontTexture = loadFont(Renderer, initVariables.musicSelection_font, 48, button.text, color);
		clickableButtons.push_back(button);
	}

	//Generate difficulty buttons
	{
		int availableSpace = (int)(initVariables.musicSelection_albumArt_width_and_height * ((float)initVariables.screen_width));
		int totalSeparationSpace = availableSpace - ((int)(((float)initVariables.screen_height) * ((float)enums::TOTAL_DIFFICULTIES) * initVariables.musicSelection_difficulty_button_height));
		int separation = totalSeparationSpace / (enums::TOTAL_DIFFICULTIES - 1);

		button.y = (int)(initVariables.musicSelection_albumArt_y * ((float)initVariables.screen_height));
		button.x = (int)(((float)initVariables.screen_width) * initVariables.musicSelection_difficulty_button_x);
		button.width = (int)(((float)initVariables.screen_width) * initVariables.musicSelection_difficulty_button_width);
		button.height = (int)(((float)initVariables.screen_height) * initVariables.musicSelection_difficulty_button_height);
		button.backgroundTexture = loadTexture("round rectangle.png", Renderer);
		button.text = "EASY";
		SDL_Color color = { 0,0,0 };
		button.fontTexture = loadFont(Renderer, initVariables.musicSelection_font, 48, button.text, color);
		clickableButtons.push_back(button);

		button.y += (separation + button.height);
		button.text = "NORMAL";
		button.fontTexture = loadFont(Renderer, initVariables.musicSelection_font, 48, button.text, color);
		clickableButtons.push_back(button);

		button.y += (separation + button.height);
		button.text = "HARD";
		button.fontTexture = loadFont(Renderer, initVariables.musicSelection_font, 48, button.text, color);
		clickableButtons.push_back(button);

		button.y += (separation + button.height);
		button.text = "EXTREME";
		button.fontTexture = loadFont(Renderer, initVariables.musicSelection_font, 48, button.text, color);
		clickableButtons.push_back(button);
	}


	//Generate start game button
	{
		button.x = (int)(((float)initVariables.screen_width) * initVariables.musicSelection_startGame_button_x);
		button.y = (int)(((float)initVariables.screen_height) * initVariables.musicSelection_startGame_button_y);
		button.width = (int)(((float)initVariables.screen_width) * initVariables.musicSelection_startGame_button_width);
		button.height = (int)(((float)initVariables.screen_height) * initVariables.musicSelection_startGame_button_height);
		button.backgroundTexture.texture = NULL;
		button.text = "START";
		SDL_Color color = { 255,255,255 };
		button.fontTexture = loadFont(Renderer, initVariables.musicSelection_font, 48, button.text, color);
		clickableButtons.push_back(button);
	}
}

void MusicSelection::checkIfClickableButtonIsPressed()
{
	for (std::vector<MusicSelectionClickableButton>::iterator i = clickableButtons.begin(); i != clickableButtons.end(); ++i)
	{
		if (mouseX > i->x && mouseX < (i->x + i->width) && mouseY > i->y && mouseY < (i->y + i->height))
		{
			processButtonClick(&(*i));
		}
	}
}

void MusicSelection::processButtonClick(MusicSelectionClickableButton *button)
{
	if (button->text == "START"  && (!listOfPanels.panels.empty())) { proceedToMainGame = true; }
	else if (button->text == "4 Key" || button->text == "6 Key")
	{
		if (button->text == "4 Key" && currentNumberOfKeysSelected != enums::FOUR_KEYS) { currentNumberOfKeysSelected = enums::FOUR_KEYS; generateListOfPanels(); }
		else if (button->text == "6 Key" && currentNumberOfKeysSelected != enums::SIX_KEYS) { currentNumberOfKeysSelected = enums::SIX_KEYS; generateListOfPanels(); }
	}
	else if (button->text == "EASY" || button->text == "NORMAL" || button->text == "HARD" || button->text == "EXTREME")
	{
		enums::beatMapDifficulty selectedDifficulty;
		if (button->text == "EASY") { selectedDifficulty = enums::EASY; }
		else if (button->text == "NORMAL") { selectedDifficulty = enums::NORMAL; }
		else if (button->text == "HARD") { selectedDifficulty = enums::HARD; }
		else if (button->text == "EXTREME") { selectedDifficulty = enums::EXTREME; }
		
		if (selectedDifficulty != currentSelectedDifficulty && (std::find(beatMapDifficulties.begin(), beatMapDifficulties.end(), selectedDifficulty) != beatMapDifficulties.end()))
		{
			currentSelectedDifficulty = selectedDifficulty;
		}
	}
}

void MusicSelection::renderClickableButtons()
{
	for (std::vector<MusicSelectionClickableButton>::iterator i = clickableButtons.begin(); i != clickableButtons.end(); ++i)
	{
		if (i->text == "EASY" || i->text == "NORMAL" || i->text == "HARD" || i->text == "EXTREME")
		{
			SDL_Rect backgroundRect = { i->x, i->y, i->width, i->height };
			SDL_Rect shadowRect = backgroundRect;
			//Render shadow
			SDL_SetTextureColorMod(i->backgroundTexture.texture, 0, 0, 0);
			SDL_SetTextureAlphaMod(i->backgroundTexture.texture, 120);
			shadowRect.x = backgroundRect.x + (int)(initVariables.musicSelection_difficulty_button_shadow * ((float)initVariables.screen_width));
			shadowRect.y = backgroundRect.y + (int)(initVariables.musicSelection_difficulty_button_shadow * ((float)initVariables.screen_width));
			SDL_RenderCopy(Renderer, i->backgroundTexture.texture, NULL, &shadowRect);
			SDL_SetTextureAlphaMod(i->backgroundTexture.texture, 255);

			//Change colors respectively
			RGB difficultyColor;
			if (i->text == "EASY") { difficultyColor = initVariables.musicSelection_color_easy; }
			else if (i->text == "NORMAL") { difficultyColor = initVariables.musicSelection_color_normal; }
			else if (i->text == "HARD") { difficultyColor = initVariables.musicSelection_color_hard; }
			else if (i->text == "EXTREME") { difficultyColor = initVariables.musicSelection_color_extreme; }
			//Render background
			SDL_SetTextureColorMod(i->backgroundTexture.texture, difficultyColor.r, difficultyColor.g, difficultyColor.b);
			SDL_RenderCopy(Renderer, i->backgroundTexture.texture, NULL, &backgroundRect);
			//Render text
			SDL_Rect textRect;
			int centerOfButtonX = i->x + ((i->width) / 2);
			int centerOfButtonY = i->y + ((i->height) / 2);
			float widthToHeightRatio = ((float)i->fontTexture.width) / ((float)i->fontTexture.height);
			textRect.h = i->height;
			textRect.w = (int)(widthToHeightRatio * ((float)textRect.h));
			textRect.x = centerOfButtonX - (textRect.w / 2);
			textRect.y = centerOfButtonY - (textRect.h / 2);
			SDL_RenderCopy(Renderer, i->fontTexture.texture, NULL, &textRect);
		}
		else
		{
			if (i->backgroundTexture.texture == NULL)
			{
				SDL_Rect backgroundRect = { i->x, i->y, i->width, i->height };
				SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 155);
				SDL_RenderFillRect(Renderer, &backgroundRect);
			}
			if (i->fontTexture.texture != NULL)
			{
				SDL_Rect textRect = { i->x, i->y, i->width, i->height };
				SDL_RenderCopy(Renderer, i->fontTexture.texture, NULL, &textRect);
			}
		}
	}
}

void MusicSelection::renderAlbumArt()
{
	//Basing width and height off screen_width
	int albumArtWidthAndHeight = (int)(initVariables.musicSelection_albumArt_width_and_height * ((float)initVariables.screen_width));
	SDL_Rect albumArtRect = { (int)(initVariables.musicSelection_albumArt_x * ((float)initVariables.screen_width)),(int)(initVariables.musicSelection_albumArt_y * ((float)initVariables.screen_height)),albumArtWidthAndHeight,albumArtWidthAndHeight };
	SDL_Rect albumArtShadowRect = albumArtRect;
	albumArtShadowRect.x = albumArtRect.x + (int)(initVariables.musicSelection_albumArt_shadow * ((float)initVariables.screen_width));
	albumArtShadowRect.y = albumArtRect.y + (int)(initVariables.musicSelection_albumArt_shadow * ((float)initVariables.screen_width));
	//Render Shadow
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 125);
	SDL_RenderFillRect(Renderer, &albumArtShadowRect);

	//Render album art
	if (beatMaps[currentSelectedMusicIndex].albumArtFileName == "No Image" || listOfPanels.panels.empty())
	{
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(Renderer, &albumArtRect);
	}
	else { SDL_RenderCopy(Renderer, albumArt.texture, NULL, &albumArtRect); return; }

	//Render text (if applicable)
	TextureWithVariables texture;
	std::string text;
	SDL_Color color = { 255,255,255 };
	if (listOfPanels.panels.empty()) { text = "No songs found"; }
	else { text = beatMaps[currentSelectedMusicIndex].albumArtFileName; }
	texture = loadFont(Renderer, "Noto.otf", 72, text, color);
	float widthToHeightRatio = ((float)texture.width) / ((float)texture.height);
	SDL_Rect errorTextRect;
	errorTextRect.h = (int)(((float)initVariables.screen_height) *(initVariables.musicSelection_albumArt_errorText_height * initVariables.musicSelection_albumArt_width_and_height));
	errorTextRect.w = (int)(widthToHeightRatio * ((float)errorTextRect.h));
	int albumArtHalfWidth = (int)(((float)initVariables.screen_width) * (initVariables.musicSelection_albumArt_width_and_height / 2.f));
	int centerOfAlbumArtX = ((int)(((float)initVariables.screen_width) * initVariables.musicSelection_albumArt_x))+ albumArtHalfWidth;
	int centerOfAlbumArtY = ((int)(((float)initVariables.screen_height) * initVariables.musicSelection_albumArt_y))+ albumArtHalfWidth;
	errorTextRect.x = centerOfAlbumArtX - (errorTextRect.w/2);
	errorTextRect.y = centerOfAlbumArtY - (errorTextRect.h / 2);

	SDL_RenderCopy(Renderer, texture.texture, NULL, &errorTextRect);
	SDL_DestroyTexture(texture.texture);
}

void MusicSelection::renderPanels()
{
	for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
	{
		//Render highlight
		SDL_Rect highlightRect = { 0,i->centerY - i->width,selectionBarX,2 * i->width };
		RGB color;
		SDL_SetRenderDrawBlendMode(Renderer,SDL_BLENDMODE_BLEND);
		switch (currentSelectedDifficulty)
		{
		case enums::EASY:
			color = initVariables.musicSelection_color_easy; break;
		case enums::NORMAL:
			color = initVariables.musicSelection_color_normal; break;
		case enums::HARD:
			color = initVariables.musicSelection_color_hard; break;
		case enums::EXTREME:
			color = initVariables.musicSelection_color_extreme; break;
		}
		SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, 155);
		SDL_RenderFillRect(Renderer, &highlightRect);

		//Render text
		float widthToHeightRatio = ((float)i->songTitleTexture.width) / ((float)i->songTitleTexture.height);
		int textPadding = (int)(initVariables.musicSelection_panel_text_right_padding * ((float)initVariables.screen_width));
		//Using initVariable's width instead of panel's width because panel's one might change
		int textPanelHeight = (int)(initVariables.musicSelection_panel_width * ((float)initVariables.screen_height));
		int textWidth = (int)(widthToHeightRatio * ((float)textPanelHeight));
		SDL_Rect textRect = { selectionBarX - textWidth - textPadding,i->centerY - textPanelHeight,textWidth,textPanelHeight };
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderCopyEx(Renderer, i->songTitleTexture.texture, NULL, &textRect, 0, 0, SDL_FLIP_NONE);

		//Render panel borders
		SDL_Rect panelRect = { 0,i->centerY-i->width-(initVariables.musicSelection_panel_thickness/2), selectionBarX, initVariables.musicSelection_panel_thickness };
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(Renderer, &panelRect);
		panelRect.y = i->centerY + i->width - (initVariables.musicSelection_panel_thickness/2);
		SDL_RenderFillRect(Renderer, &panelRect);
	}
}

void MusicSelection::generateListOfPanels()
{
	listOfPanels.panels.clear();

	//Ensure that there is at least one song that has respective number of keys
	if (beatMapsWithKey[currentNumberOfKeysSelected].empty()) { return; }

	//If can't find selected index inside beatmapsWithKey, change it to the nearest valid one
	if (std::find(beatMapsWithKey[currentNumberOfKeysSelected].begin(), beatMapsWithKey[currentNumberOfKeysSelected].end(), currentSelectedMusicIndex) == beatMapsWithKey[currentNumberOfKeysSelected].end())
	{
		int buffer = 1;
		//This won't go into an infinite loop because we ensured that beatMapsWith selected key isnt empty
		while (true)
		{
			if (std::find(beatMapsWithKey[currentNumberOfKeysSelected].begin(), beatMapsWithKey[currentNumberOfKeysSelected].end(), currentSelectedMusicIndex + buffer) != beatMapsWithKey[currentNumberOfKeysSelected].end())
			{
				currentSelectedMusicIndex += buffer;
				break;
			}
			else if (std::find(beatMapsWithKey[currentNumberOfKeysSelected].begin(), beatMapsWithKey[currentNumberOfKeysSelected].end(), currentSelectedMusicIndex - buffer) != beatMapsWithKey[currentNumberOfKeysSelected].end())
			{
				currentSelectedMusicIndex -= buffer;
				break;
			}
			buffer++;
		}
	}

	int indexOfMusicRelativeToBeatMapsWithKey = findIndexOfElementInBeatMapsWithKey(&beatMapsWithKey[currentNumberOfKeysSelected], currentSelectedMusicIndex);
	listOfPanels.panels.push_back(generateMusicSelectionPanel(currentSelectedMusicIndex));
	listOfPanels.back = indexOfMusicRelativeToBeatMapsWithKey;
	listOfPanels.front = indexOfMusicRelativeToBeatMapsWithKey;

	//Generate panels below selected index. If not enough, generate panels above
	while ((listOfPanels.back+1) < ((int)beatMapsWithKey[currentNumberOfKeysSelected].size()) && ((int)listOfPanels.panels.size()) <= maxNumberOfPanels)
	{
		++listOfPanels.back;
		listOfPanels.panels.push_back(generateMusicSelectionPanel(beatMapsWithKey[currentNumberOfKeysSelected][listOfPanels.back]));
	}
	while (listOfPanels.front != 0 && ((int)listOfPanels.panels.size()) <= maxNumberOfPanels)
	{
		--listOfPanels.front;
		listOfPanels.panels.push_front(generateMusicSelectionPanel(beatMapsWithKey[currentNumberOfKeysSelected][listOfPanels.front]));
	}

	//Adjust positions
	float center = initVariables.musicSelection_panel_width;
	for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
	{
		i->centerY = (int)(center * ((float)initVariables.screen_height));
		center += (initVariables.musicSelection_panel_separation + (2.f * initVariables.musicSelection_panel_width));
	}
	assertThatPanelCornersDontCrossLimit();

	//Check that difficulty exists
	checkThatDifficultyIsAvailableInNewSong();
}

void MusicSelection::assertThatPanelCornersDontCrossLimit()
{
	//Program crashes when you try to dereference list.begin()/list.end() when it's empty
	if (listOfPanels.panels.empty()) { return; }

	if (listOfPanels.panels.front().centerY > (initVariables.screen_height / 2))
	{
		int amountToDecrease = listOfPanels.panels.front().centerY - (initVariables.screen_height / 2);
		for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
		{
			i->centerY -= amountToDecrease;
		}
	}
	if (listOfPanels.panels.back().centerY < (initVariables.screen_height / 2))
	{
		int amountToIncrease = (initVariables.screen_height / 2) - listOfPanels.panels.back().centerY;
		for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
		{
			i->centerY += amountToIncrease;
		}
	}
}

MusicSelectionPanel MusicSelection::generateMusicSelectionPanel(int index)
{
	SDL_Color color = { 0,0,0 };
	MusicSelectionPanel msp = { loadFont(Renderer,initVariables.musicSelection_font,48,beatMaps[index].songName, color), index, 0, 0, (int)(initVariables.musicSelection_panel_width * ((float)initVariables.screen_height)) };
	return msp;
}

int MusicSelection::findIndexOfElementInBeatMapsWithKey(std::vector<int> *vect, int element)
{
	//Using linear search at the moment. Do improve searching algorithm in the future
	for (size_t i = 0; i < vect->size(); i++)
	{
		if ((*vect)[i] == element)
		{
			return (int)i;
		}
	}
	return -1;
}

void MusicSelection::selectPanel()
{
	//Check that click occured inside a panel that isn't the currently selected panel
	for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
	{
		if (mouseY < i->centerY + i->width && mouseY > i->centerY - i->width && i->musicIndex != currentSelectedMusicIndex)
		{
			//Register the change
			currentSelectedMusicIndex = i->musicIndex;
			freeAndChangebgm();
			freeAndChangebg();
			freeAndChangeAlbumArt();
			checkThatDifficultyIsAvailableInNewSong();

			break;
		}
	}
}

void MusicSelection::checkThatMusicIsPlayingWithinRange()
{
	double currentPosition = BASS_ChannelBytes2Seconds(bgm, (BASS_ChannelGetPosition(bgm, BASS_POS_BYTE)));
	if (currentPosition > beatMaps[currentSelectedMusicIndex].musicEndPosition || currentPosition < beatMaps[currentSelectedMusicIndex].musicStartPosition)
	{
		BASS_ChannelSetPosition(bgm, BASS_ChannelSeconds2Bytes(bgm, beatMaps[currentSelectedMusicIndex].musicStartPosition), BASS_POS_BYTE);
		BASS_ChannelPlay(bgm, false);
	}
}

void MusicSelection::checkThatDifficultyIsAvailableInNewSong()
{
	BeatMapKeyAndDifficulty bMKAD;
	bMKAD.difficulty = currentSelectedDifficulty;
	bMKAD.numberOfKeys = currentNumberOfKeysSelected;
	beatMapDifficulties.clear();
	
	bool found = false;
	enums::beatMapDifficulty easiestKnownDifficulty = enums::EXTREME;
	for (std::vector<BeatMapKeyAndDifficulty>::iterator i = beatMaps[currentSelectedMusicIndex].difficultyAndKeys.begin(); i != beatMaps[currentSelectedMusicIndex].difficultyAndKeys.end(); ++i)
	{
		if (i->numberOfKeys == currentNumberOfKeysSelected)
		{
			if (i->difficulty == currentSelectedDifficulty) { found = true; }
			else if (i->difficulty < easiestKnownDifficulty) { easiestKnownDifficulty = i->difficulty; }
			beatMapDifficulties.push_back(i->difficulty);
		}
	}
	if (!found) { currentSelectedDifficulty = easiestKnownDifficulty; }
}

void MusicSelection::backupPanelY()
{
	for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
	{
		i->previousCenterY = i->centerY;
	}
}

void MusicSelection::freeAndChangebgm()
{
	BASS_StreamFree(bgm);
	std::string songToLoad = beatMaps[currentSelectedMusicIndex].beatMapRootFolder + '/' + beatMaps[currentSelectedMusicIndex].musicFileName;
	bgm = BASS_StreamCreateFile(false, songToLoad.c_str(), 0, 0, 0);
}

void MusicSelection::freeAndChangebg()
{
	SDL_DestroyTexture(bg.texture);
	bg = loadTexture(beatMaps[currentSelectedMusicIndex].beatMapRootFolder + '/' + beatMaps[currentSelectedMusicIndex].bgFileName, Renderer);
}

void MusicSelection::freeAndChangeAlbumArt()
{
	SDL_DestroyTexture(albumArt.texture);
	if (listOfPanels.panels.empty()) { albumArt.texture = NULL; return; }
	else if (beatMaps[currentSelectedMusicIndex].albumArtFileName == "No Image") { return; }
	albumArt = loadTexture(beatMaps[currentSelectedMusicIndex].beatMapRootFolder + '/' + beatMaps[currentSelectedMusicIndex].albumArtFileName, Renderer);
}

Instruction MusicSelection::generateInstructionForMainGame()
{
	Instruction instruction;
	instruction.quit = false;
	switch (currentNumberOfKeysSelected)
	{
	case enums::FOUR_KEYS:
		instruction.gameKeys = 4; break;
	case enums::SIX_KEYS:
		instruction.gameKeys = 6; break;
	}
	switch (currentSelectedDifficulty)
	{
	case enums::EASY:
		instruction.songDifficulty = "Easy"; break;
	case enums::NORMAL:
		instruction.songDifficulty = "Normal"; break;
	case enums::HARD:
		instruction.songDifficulty = "Hard"; break;
	case enums::EXTREME:
		instruction.songDifficulty = "Extreme"; break;
	}
	instruction.beatMapSongToLoad = beatMaps[currentSelectedMusicIndex].musicFileName;
	instruction.beatMapBGtoLoad = beatMaps[currentSelectedMusicIndex].bgFileName;
	instruction.beatMapRootFolder = beatMaps[currentSelectedMusicIndex].beatMapRootFolder;
	instruction.nextState = enums::MAIN_GAME;
	return instruction;
}