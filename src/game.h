#ifndef GAME_H
#define GAME_H

#define OBJ_COUNT 5
#define DIRS 8

#include <vector>
#include <string>

#include <SFML/Graphics.hpp>
#include "object.h"

class Game
{
	sf::Texture* loadTexture(const std::string &filename);
	void setObject(Object *object, unsigned short x, unsigned short y, unsigned short id, unsigned short direction = 0);
	static double moveInDirection_x(unsigned short dir, double length);
	static double moveInDirection_y(unsigned short dir, double length);
public:
	Game();
	~Game();

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
	std::vector<std::string> IMG_NAMES = {"empty", "beamer", "dotB", "mirror", "bender", "dotF"};

	struct Level {
		unsigned short width;
		unsigned short height;
		std::vector<std::vector<bool>> obstacles;
		std::vector<Object*> objectList[OBJ_COUNT];
		Game* game;
	} level;

	void loadLevel(const std::string &id);
	void calculateLasers();
	void updateDots();
};

#endif
