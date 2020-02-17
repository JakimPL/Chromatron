#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "constants.hpp"
#include "drag.hpp"
#include "object.hpp"
#include "stack.hpp"

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

	void createRay(Beamer *beamer, RayGen &rayGen, RayType rayType = RT_NORMAL);
	void createRays(Beamer *beamer, RayGenList rayGens);
	void rayStep(Beamer *beamer, Ray &ray, RayGen &rayGen);
	void calculateLasers();
};

#endif
