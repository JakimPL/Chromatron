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

	std::vector<sf::Texture*> textures;
	unsigned int SCREEN_WIDTH = 1024;
	unsigned int SCREEN_HEIGHT = 768;
	int OFFSET_X = 64;
	int OFFSET_Y = 64;
	int TILE_SIZE = 40;
	int OUTLINE_SIZE = 1;

	std::string PATH_DATA = "./data/";
	std::string PATH_LEV_PREFIX = "LEV/level";
	std::string PATH_LEV_SUFFIX = ".lev";
	std::string PATH_IMG_PREFIX = "IMG/";
	std::string PATH_IMG_SUFFIX = ".png";

	std::vector<std::string> IMG_NAMES = {"empty", "beamer", "dot", "mirror", "bender"};
};

#endif
