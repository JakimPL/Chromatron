#ifndef GAME_H
#define GAME_H

#include <vector>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "drag.h"
#include "object.h"

enum EditorMode {
	ED_EDIT_OBJECTS,
	ED_ADD_OR_REMOVE_OBJECTS,
	ED_ADD_OR_REMOVE_OBSTACLES,
	ED_COUNT
};

class Game
{
	sf::Texture* loadTexture(const std::string &filename);
	void setObject(Object* object, short x, short y, unsigned short id, unsigned short direction = 0);
	void setObject(Object* object, Object::Position position, unsigned short id, unsigned short direction = 0);
public:
	Game();
	~Game();

	struct Level {
		bool event = true;
		unsigned short width;
		unsigned short height;
		std::vector<Object*> objectList[OBJ_COUNT];
		std::map<Object::Position, bool> obstacles;
		std::map<Object::Position, Object*> objectMap;
		Game* game;

		bool addObject(unsigned short id, Object::Position position);
		bool dragObject(Drag &drag, Object::Position position);
		bool changeObjectColor(Object::Position position);
		bool isPlaceFree(Object::Position position);
		bool isOutsideBoard(Object::Position position);
		bool moveObject(Object::Position start, Object::Position end);
		bool removeObject(Object::Position position);
		bool rotateObject(Object::Position position);
		bool setObstacle(Object::Position position, bool obstacle);
	} level;

	struct Editor {
	private:
		bool active;
		unsigned short currentObject = OBJ_BEAMER;
	public:
		EditorMode mode;
		sf::Sprite sprite;

		bool isActive();
		unsigned short getObject();
		void setObject(unsigned short id);
		void switchMode();
		void turn(bool editorOn);
	} editor;

	void loadLevel(const std::string &id);
	void saveLevel(const std::string &id);
	void calculateLasers();
	void updateDots();
};

#endif
