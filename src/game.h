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

class Game
{
	sf::Texture* loadTexture(const std::string &filename);
public:
	Game();
	~Game();

	struct Level {
		bool event = true;
		unsigned short width;
		unsigned short height;
		std::vector<Object*> objectList[OBJ_COUNT];
		std::map<Object::Position, bool> obstacles;
		std::map<Object::Position, sf::Sprite> tileSprites;
		std::map<Object::Position, Object*> objectMap;
		Stack stack;
		Game* game;

		bool addObject(Object::Position position, ObjectID id);
		bool dragObject(Drag &drag, Object::Position mousePosition);
		bool dragStackObject(Drag &drag, Object::Position mousePosition);
		bool changeObjectColor(Object::Position position);
		bool isPlaceFree(Object::Position position);
		bool isPlaceTaken(Object::Position position);
		bool isOutsideBoard(Object::Position position);
		bool moveObject(Object::Position start, Object::Position end);
		bool removeObject(Object::Position position);
		bool rotateObject(Object::Position position);
		bool rotateStackObject(Object::Position position);
		bool setObstacle(Object::Position position, bool obstacle);
		void setObject(Object* object, short x, short y, ObjectID id, DirectionID direction = DIR_NORTH);
		void setObject(Object* object, Object::Position position, ObjectID id, DirectionID direction = DIR_NORTH);
		void setTile(Object::Position position);
		bool moveFromStack(Object::Position stackPosition, Object::Position mousePosition);
		bool moveToStack(Object::Position dragPosition, Object::Position mousePosition);
		bool moveFromStackToStack(Object::Position dragPosition, Object::Position mousePosition);
		void updateStack();
	} level;

	struct Editor {
	private:
		bool active;
		ObjectID currentObject = OBJ_BEAMER;
	public:
		EditorMode mode;
		sf::Sprite sprite;

		bool isActive();
		ObjectID getObject();
		void setObject(ObjectID id);
		void switchMode();
		void turn(bool editorOn);
	} editor;

	Drag drag;

	void loadLevel(const std::string &id);
	void saveLevel(const std::string &id);
	void calculateLasers();
	void updateDots();
};

#endif
