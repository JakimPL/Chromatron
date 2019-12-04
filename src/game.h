#ifndef GAME_H
#define GAME_H

#include <vector>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "drag.h"
#include "object.h"
#include "stack.h"

enum EditorMode {
	ED_EDIT_OBJECTS,
	ED_ADD_OR_REMOVE_OBJECTS,
	ED_ADD_OR_REMOVE_OBSTACLES,
	ED_COUNT
};

class Game
{
	sf::Texture* loadTexture(const std::string &filename);
	void setObject(Object* object, short x, short y, Objects id, Directions direction = DIR_NORTH);
	void setObject(Object* object, Object::Position position, Objects id, Directions direction = DIR_NORTH);
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
		Stack stack;
		Game* game;

		bool addObject(Object::Position position, Objects id);
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
		Objects currentObject = OBJ_BEAMER;
	public:
		EditorMode mode;
		sf::Sprite sprite;

		bool isActive();
		Objects getObject();
		void setObject(Objects id);
		void switchMode();
		void turn(bool editorOn);
	} editor;

	void loadLevel(const std::string &id);
	void saveLevel(const std::string &id);
	void calculateLasers();
	void updateDots();
};

#endif
