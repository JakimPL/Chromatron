#ifndef GAME_H
#define GAME_H

#include <vector>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>
#include "constants.hpp"
#include "drag.hpp"
#include "graphics.hpp"
#include "object.hpp"
#include "stack.hpp"

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
		std::map<Position, Object*> objectMap;
		std::map<Position, bool> obstacles;
		std::map<Position, sf::Sprite> tileSprites;
		Stack stack;
		Game* game;

		bool addObject(Position position, ObjectID id);
		bool addToStack(Position position);
		bool dragObject(Drag &drag, Position mousePosition);
		bool dragStackObject(Drag &drag, Position mousePosition);
		bool changeObjectColor(Position mousePosition);
		Object *getObject(Position mousePosition);
		Position getRelativePosition(Position mousePosition);
		bool isPlaceFree(Position position);
		bool isPlaceFree(Position position, bool onStack);
		bool isPlaceTaken(Position position);
		bool isPlaceTaken(Position position, bool onStack);
		bool isOutsideBoard(Position position);
		bool moveObject(Position start, Position end);
		void newObject(Position position, ObjectID id, bool inStack);
		bool removeObject(Position position);
		bool rotateObject(Position mousePosition, bool clockwise = true);
		void setObject(Object *object, short x, short y, ObjectID id, DirectionID direction = DIR_NORTH, bool inStack = false, bool stackObject = false);
		void setObject(Object *object, Position position, ObjectID id, DirectionID direction = DIR_NORTH, bool inStack = false, bool stackObject = false);
		bool setObstacle(Position position, bool obstacle);
		void setTile(Position position, bool obstacle);
		bool moveFromStack(Position stackPosition, Position mousePosition);
		bool moveToStack(Position dragPosition, Position mousePosition);
		bool moveFromStackToStack(Position dragPosition, Position mousePosition);

		bool checkLevelSave(const std::string &id);
		void checkLevel();
		void clearLevel();
		void resetLevel(bool ignoreSave = false);
		void loadLevel(const unsigned short level, bool ignoreSave = false);
		void loadLevel(const std::string &id, bool ignoreSave = false);
		void saveLevel(const std::string &id);

		unsigned short countObjects();
		unsigned short countObjects(bool inStack);
		unsigned short countObjects(ObjectID id);
		bool checkWin();
		void clearDots();
		void updateDots();
		void updateStack();

		void createRay(Beamer *beamer, RayGen rayGen);
		void createRays(Beamer *beamer, std::vector<RayGenElement> rayGens);
		void createTangledRay(Beamer *beamer, RayGen rayGen);
		ColorShift rayStep(Beamer *beamer, RayGen &rayGen);
		void calculateLasers();
	} level;

	struct LevelSet {
	private:
		unsigned short currentLevel;
	public:
		std::string name;
		unsigned short levels;
		std::vector<LevelState> levelStates;
		Game* game;

		bool checkSetSave(const std::string &levelSetName);
		void loadSet(const std::string &levelSetName);
		void saveCurrentLevel(const std::string &levelSetName);
		bool checkSet(const std::string &levelSetName);
		void saveSet(const std::string &levelSetName, bool save = true);
		void saveCurrentLevel();
		void saveSet(bool save = true);

		unsigned short getCurrentLevel();
		void nextLevel();
		void previousLevel();

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