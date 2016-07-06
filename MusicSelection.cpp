#include "MusicSelection.h"
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
	currentSelectedMusicIndex = 1;
	currentNumberOfKeysSelected = enums::SIX_KEYS;
	generateListOfPanels();

	//Load background
	bg = loadTexture(beatMaps[currentSelectedMusicIndex].beatMapRootFolder + "/" + beatMaps[currentSelectedMusicIndex].bgFileName, Renderer);

	//To prevent selectionBar from jumping around when just initted
	mouseX = initVariables.screen_width;
	selectionIsMinimized = true;
	selectionBarTransitionTime = 0.0;
	

	initted = true;
}

void MusicSelection::uninit()
{
	if (initted)
	{
		SDL_DestroyTexture(bg);
		for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
		{
			SDL_DestroyTexture(i->songTitleTexture.texture);
		}

		initted = false;
	}
}

Instruction MusicSelection::process(SDL_Event e, Instruction nextInstruction)
{
	if (!initted)
	{
		init();
		/*
		boost::filesystem::path p(".\\Music");
		boost::filesystem::directory_iterator end_itr;
		std::vector<std::string> songs;
		if (boost::filesystem::exists(p))
		{
			while (true) {
				for (boost::filesystem::directory_iterator song(p); song != end_itr; ++song)
				{
					if (boost::filesystem::is_directory(song->path()))
					{
						songs.push_back(song->path().filename().string());
					}
				}

				for (std::vector<std::string>::iterator i = songs.begin(); i != songs.end(); ++i)
				{
					std::cout << *i << std::endl;
				}
				int selection;
				std::cin >> selection;
				if (boost::filesystem::is_empty(p / songs[selection])) { continue; }
				std::vector<std::string> keys;
				for (boost::filesystem::directory_iterator key(p / songs[selection]); key != end_itr; ++key)
				{
					if (boost::filesystem::is_directory(key->path()))
					{
						if (key->path().filename().string() == "4key" || key->path().filename().string() == "6key" || key->path().filename().string() == "8key")
						{
							keys.push_back(key->path().filename().string());
						}
					}
				}
				for (auto i = keys.begin(); i != keys.end(); ++i)
				{
					std::cout << *i << std::endl;
				}
				int selection2;
				std::cin >> selection2;
				if (boost::filesystem::is_empty(p / songs[selection] / keys[selection2])) { continue; }
				std::vector<std::string> difficulties;
				for (boost::filesystem::directory_iterator difficulty(p / songs[selection] / keys[selection2]); difficulty != end_itr; ++difficulty)
				{
					if (boost::filesystem::is_directory(difficulty->path()))
					{
						if (difficulty->path().filename().string() == "Easy" || difficulty->path().filename().string() == "Normal" || difficulty->path().filename().string() == "Hard" || difficulty->path().filename().string() == "Extreme")
						{
							difficulties.push_back(difficulty->path().filename().string());
						}
					}
				}
				for (auto i = difficulties.begin(); i != difficulties.end(); ++i)
				{
					std::cout << *i << std::endl;
				}
				int selection3;
				std::cin >> selection3;
				if (boost::filesystem::is_empty(p / songs[selection] / keys[selection2] / difficulties[selection3])) { continue; }
				instruction.quit = false;
				instruction.nextState = enums::MAIN_GAME;
				instruction.songToLoad = songs[selection];
				if (keys[selection2] == "4key") { instruction.gameKeys = 4; }
				else if (keys[selection2] == "6key") { instruction.gameKeys = 6; }
				instruction.songDifficulty = difficulties[selection3];
				uninit();
				return instruction;
				
			}
		} */
	}

	double currentTick = ((double)SDL_GetTicks()) / 1000.0;

	//Process event
	processEvent(e);

	//Compute selection bar x
	computeSelectionBarX(currentTick);

	//Render background
	SDL_RenderCopy(Renderer, bg, NULL, NULL);

	//Render panel borders
	for (std::list<MusicSelectionPanel>::iterator i = listOfPanels.panels.begin(); i != listOfPanels.panels.end(); ++i)
	{

		//Render highlight
		SDL_Rect highlightRect = { 0,i->centerY - i->width,selectionBarX,2 * i->width };
		SDL_SetRenderDrawBlendMode(Renderer,SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(Renderer, 51, 204, 255, 155);
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

void MusicSelection::processEvent(SDL_Event e)
{
	if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
	{
		mouseX = e.motion.x;
		mouseY = e.motion.y;
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
				getSongInfo(root->path().string() + "/songinfo.txt",&fileSystem.songName, &fileSystem.bgFileName);
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
							keyAndDifficulty.numberOfKeys = 4;
							if (!fourKeyAlreadyAdded)
							{
								beatMapsWithKey[enums::FOUR_KEYS].push_back(index);
								fourKeyAlreadyAdded = true;
							}
						}
						else if (*i == "6key") 
						{ 
							keyAndDifficulty.numberOfKeys = 6; 
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

void MusicSelection::getSongInfo(std::string path, std::string *songName, std::string *bgFileName)
{
	std::ifstream file(path);
	std::string line;
	while (std::getline(file, line))
	{
		if (line == "#songname")
		{
			std::string line2;
			std::getline(file, line2);
			*songName = line2;
		}
		else if (line == "#bgfilename")
		{
			std::string line2;
			std::getline(file, line2);
			*bgFileName = line2;
		}
	}
	return;
}

void MusicSelection::calculateMaxNumberOfPanels()
{
	maxNumberOfPanels = ((int)floor(1.f / (initVariables.musicSelection_panel_separation + (2.f * initVariables.musicSelection_panel_width)))) + 1;
	std::cout << maxNumberOfPanels << std::endl;
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
	while ((listOfPanels.back+1) < ((int)beatMapsWithKey[currentNumberOfKeysSelected].size()) && ((int)listOfPanels.panels.size()) < maxNumberOfPanels)
	{
		++listOfPanels.back;
		listOfPanels.panels.push_back(generateMusicSelectionPanel(beatMapsWithKey[currentNumberOfKeysSelected][listOfPanels.back]));
	}
	while (listOfPanels.front != 0 && ((int)listOfPanels.panels.size()) < maxNumberOfPanels)
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
}

void MusicSelection::assertThatPanelCornersDontCrossLimit()
{
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
	MusicSelectionPanel msp = { loadFont(Renderer,initVariables.musicSelection_panel_font,48,beatMaps[index].songName, color), index, 0, (int)(initVariables.musicSelection_panel_width * ((float)initVariables.screen_height)) };
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