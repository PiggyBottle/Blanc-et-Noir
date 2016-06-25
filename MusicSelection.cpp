#include "MusicSelection.h"
#include <boost/filesystem.hpp>



MusicSelection::MusicSelection(SDL_Renderer *gRenderer)
{
	this->Renderer = gRenderer;
}


MusicSelection::~MusicSelection()
{
}

void MusicSelection::init()
{
	//Load texture
	screen = loadTexture("startingmenuscreen.jpg", Renderer);
	initted = true;
}

void MusicSelection::uninit()
{
	if (initted)
	{
		SDL_DestroyTexture(screen);
		initted = false;
	}
}

Instruction MusicSelection::process(SDL_Event e, Instruction nextInstruction)
{
	if (!initted)
	{
		init();
		std::cout << "In main menu" << std::endl;

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
		}
	}
	//SDL_RenderCopyEx(Renderer, screen, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
	//instruction.quit = false;
	//if (e.type == SDL_KEYDOWN)
	//{
	//	//instruction.songToLoad = enums::UNRAVEL;
	//	instruction.nextState = enums::MAIN_GAME;
	//	instruction.gameKeys = 4;
	//	uninit();
	//}
	//else
	//{
	//	instruction.nextState = enums::MUSIC_SELECTION;
	//}
	//return instruction;
}
