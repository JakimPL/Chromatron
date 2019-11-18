#ifndef GAME_H
#define GAME_H

#define OBJ_COUNT 5

#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

class Game
{
	sf::Texture* loadTexture(const std::string &filename);
public:
	Game();
	~Game();

	unsigned short boardWidth;
	unsigned short boardHeight;

	std::vector<sf::Texture*> textures;
	unsigned int SCREEN_WIDTH = 640;
	unsigned int SCREEN_HEIGHT = 480;
	int OFFSET_X = 32;
	int OFFSET_Y = 32;
	int TILE_SIZE = 24;
	int OUTLINE_SIZE = 1;

	std::string PATH_DATA = "./data/";
	std::string PATH_LEV_PREFIX = "LEV/level";
	std::string PATH_LEV_SUFFIX = ".lev";
	std::string PATH_IMG_PREFIX = "IMG/";
	std::string PATH_IMG_SUFFIX = ".png";

	std::vector<std::string> IMG_NAMES = {"empty", "beamer", "dot", "mirror", "bender"};
};

#endif
