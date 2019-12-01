#ifndef GAME_H
#define GAME_H

#include <vector>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "object.h"

class Game
{
	sf::Texture* loadTexture(const std::string &filename);
	void setObject(Object* object, unsigned short x, unsigned short y, unsigned short id, unsigned short direction = 0);
	static double moveInDirection_x(unsigned short dir, double length);
	static double moveInDirection_y(unsigned short dir, double length);
public:
	Game();
	~Game();

	struct Level {
		unsigned short width;
		unsigned short height;
		std::vector<std::vector<bool>> obstacles;
		std::vector<Object*> objectList[OBJ_COUNT];
		std::map<unsigned short, std::map<unsigned short, Object*>> objectMap;
		Game* game;
	} level;

	void loadLevel(const std::string &id);
	void calculateLasers();
	void updateDots();
};

#endif
