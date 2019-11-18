#ifndef GAME_H
#define GAME_H

#include <vector>

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
};

#endif
