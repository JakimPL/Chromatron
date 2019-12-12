#ifndef GAME_H
#define GAME_H

#include <vector>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "drag.h"
#include "graphics.h"
#include "object.h"
#include "stack.h"

class Game
{
public:
	Game();
	~Game();

	struct Level {
		bool event = true;
		unsigned short width;
		unsigned short height;
		std::vector<Object*> objectList[OBJ_COUNT];
		std::vector<Object*> stackObjectList;
		std::map<Object::Position, Object*> objectMap;
		std::map<Object::Position, bool> obstacles;
		std::map<Object::Position, sf::Sprite> tileSprites;
		Stack stack;
		Game* game;

		bool addObject(Object::Position position, ObjectID id);
		bool addToStack(Object::Position position);
		bool dragObject(Drag &drag, Object::Position mousePosition);
		bool dragStackObject(Drag &drag, Object::Position mousePosition);
		bool changeObjectColor(Object::Position mousePosition);
		Object* getObject(Object::Position mousePosition);
		Object::Position getRelativePosition(Object::Position mousePosition);
		bool isPlaceFree(Object::Position position);
		bool isPlaceFree(Object::Position position, bool onStack);
		bool isPlaceTaken(Object::Position position);
		bool isPlaceTaken(Object::Position position, bool onStack);
		bool isOutsideBoard(Object::Position position);
		bool moveObject(Object::Position start, Object::Position end);
		void newObject(Object::Position position, ObjectID id, bool inStack);
		bool removeObject(Object::Position position);
		bool rotateObject(Object::Position mousePosition, bool clockwise = true);
		void setObject(Object* object, short x, short y, ObjectID id, DirectionID direction = DIR_NORTH, bool inStack = false, bool stackObject = false);
		void setObject(Object* object, Object::Position position, ObjectID id, DirectionID direction = DIR_NORTH, bool inStack = false, bool stackObject = false);
		bool setObstacle(Object::Position position, bool obstacle);
		void setTile(Object::Position position, bool obstacle);
		bool moveFromStack(Object::Position stackPosition, Object::Position mousePosition);
		bool moveToStack(Object::Position dragPosition, Object::Position mousePosition);
		bool moveFromStackToStack(Object::Position dragPosition, Object::Position mousePosition);

		bool checkLevelSave(const std::string &id);
		void clearLevel();
		void resetLevel();
		void loadLevel(const unsigned short level);
		void loadLevel(const std::string &id);
		void saveLevel(const std::string &id);

		unsigned short countObjects();
		unsigned short countObjects(bool inStack);
		bool checkWin();
		void calculateLasers();
		void clearDots();
		void updateDots();
		void updateStack();
	} level;

	struct LevelSet {
		std::string name;
		unsigned short levels;
		unsigned short currentLevel;
		std::vector<LevelState> levelStates;
		Game* game;

		bool checkSetSave(const std::string &levelSetName);
		void loadSet(const std::string &levelSetName);
		void saveCurrentLevel(const std::string &levelSetName);
		void saveSet(const std::string &levelSetName);
		void saveCurrentLevel();
		void saveSet();

		bool isLevelFirst();
		bool isLevelLast();
		void unlockNextLevel();
	} levelSet;

	struct Editor {
	private:
		bool active;
		ObjectID currentObject = OBJ_BEAMER;
	public:
		EditorMode mode;
		sf::Sprite sprite;
		Game* game;

		bool isActive();
		ObjectID getObject();
		void setObject(ObjectID id);
		void switchMode();
		void turn(bool editorOn);
	} editor;

	std::string levelId;
	Graphics graphics;
	Drag drag;
};

#endif
