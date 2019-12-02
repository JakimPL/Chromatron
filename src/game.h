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
	void setObject(Object* object, short x, short y, unsigned short id, unsigned short direction = 0);
public:
	Game();
	~Game();

	struct Level {
		unsigned short width;
		unsigned short height;
		std::vector<Object*> objectList[OBJ_COUNT];
		std::map<short, std::map<short, bool>> obstacles;
		std::map<short, std::map<short, Object*>> objectMap;
		Game* game;
	} level;

	void loadLevel(const std::string &id);
	void calculateLasers();
	void updateDots();
};

#endif
