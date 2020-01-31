#include <fstream>

#include "game.hpp"
#include "auxiliary.hpp"
#include "log.hpp"

Game::Game()
{
	graphics.loadTextures();
	level.game = this;
	levelSet.game = this;
	editor.game = this;
}

Game::~Game()
{

}

bool Game::Level::checkLevelSave(const std::string &id)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + game->levelSet.name + "/" + id + PATH_SAV_SUFFIX;
	std::ifstream levelSetSaveFile(location);
	return levelSetSaveFile.good();
}

void Game::Level::checkLevel()
{
	bool blackObject = false;
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < objectList[type].size(); ++index) {
			if (objectList[type][index]->colorable) {
				if (objectList[type][index]->color == COL_BLACK) {
					blackObject = true;
					break;
				}
			}
		}

		if (blackObject) {
			break;
		}
	}

	if (blackObject) {
		LogWarning("Some objects are of black color");
	}

	if (objectList[OBJ_EMPTY].size() > 0) {
		LogWarning("There are empty objects on a level");
	}

	if (objectList[OBJ_BEAMER].size() == 0) {
		LogWarning("Level does not contain any beamer");
	}

	if (objectList[OBJ_DOT].size() == 0) {
		LogWarning("Level does not contain any dot");
	}
}

void Game::Level::clearLevel()
{
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		stack.objectList[type].clear();
		while (!objectList[type].empty()) {
			delete objectList[type].back();
			objectList[type].pop_back();
		}
	}

	objectMap.clear();
	stack.objectMap.clear();
	stackObjectList.clear();

	for (short y = 0; y < height; ++y) {
		for (short x = 0; x < width; ++x) {
			Position currentPosition = shortToPosition(x, y);
			setTile(currentPosition, false);
		}
	}

	LogInfo("Level cleared");
}

void Game::Level::loadLevel(const unsigned short level, bool ignoreSave)
{
	loadLevel(numberToString(level), ignoreSave);
}

void Game::Level::loadLevel(const std::string &id, bool ignoreSave)
{
	///TODO: error handling
	std::string location = PATH_DATA + PATH_LEV_PREFIX + game->levelSet.name + "/" + id + PATH_LEV_SUFFIX;
	std::ifstream levelFile(location, std::ios::binary | std::ios::in);
	if (levelFile.good()) {
		readByte(levelFile, width);
		readByte(levelFile, height);
		readByte(levelFile, stack.width);
		readByte(levelFile, stack.height);

		updateStack();

		for (short y = 0; y < height; ++y) {
			for (short x = 0; x < width; ++x) {
				Position currentPosition = shortToPosition(x, y);
				objectMap[currentPosition] = nullptr;

				bool obstacle;
				levelFile.read((char*)&obstacle, 1);
				setTile(currentPosition, obstacle);
			}
		}

		unsigned short objectsCount, stackObjectsCount, stackObjectsCountSave;
		readByte(levelFile, objectsCount);

		for (unsigned short obj = 0; obj < objectsCount; ++obj) {
			readObject(levelFile, game->level);
		}

		readByte(levelFile, stackObjectsCount);

		if (checkLevelSave(id) && !ignoreSave) {
			levelFile.close();
			LogInfo("File " + location + " loaded successfully");
			location = PATH_DATA + PATH_LEV_PREFIX + game->levelSet.name + "/" + id + PATH_SAV_SUFFIX;
			levelFile.open(location, std::ios::binary | std::ios::in);
			readByte(levelFile, stackObjectsCountSave);
			if (stackObjectsCount != stackObjectsCountSave) {
				LogError("Corrupted save file!");
				levelFile.close();
				clearLevel();
				loadLevel(id, true);
			}
		}

		for (unsigned short obj = 0; obj < stackObjectsCount; ++obj) {
			readObject(levelFile, game->level, true);
		}

		game->levelId = id;
		levelFile.close();
		LogInfo("File " + location + " loaded successfully");
	} else {
		LogError("Failed to load " + location + " file");
	}
}

void Game::Level::saveLevel(const std::string &id)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + game->levelSet.name + "/" + id + PATH_LEV_SUFFIX;
	std::ofstream levelFile(location, std::ios::binary);
	if (levelFile.good()) {
		writeByte(levelFile, width);
		writeByte(levelFile, height);
		writeByte(levelFile, stack.width);
		writeByte(levelFile, stack.height);

		for (short y = 0; y < height; ++y) {
			for (short x = 0; x < width; ++x) {
				Position currentPosition = shortToPosition(x, y);
				levelFile.write((char*) & (obstacles[currentPosition]), 1);
			}
		}

		unsigned short objectsCount = countObjects(false);
		writeByte(levelFile, objectsCount);

		for (size_t type = 0; type < OBJ_COUNT; ++type) {
			for (size_t index = 0; index < objectList[type].size(); ++index) {
				if (!objectList[type][index]->inStack) {
					Object *object = objectList[type][index];
					writeObject(levelFile, object);
				}
			}
		}

		unsigned short stackObjectsCount = countObjects(true);
		writeByte(levelFile, stackObjectsCount);

		for (size_t type = 0; type < OBJ_COUNT; ++type) {
			for (size_t index = 0; index < objectList[type].size(); ++index) {
				if (objectList[type][index]->inStack) {
					Object *object = objectList[type][index];
					writeObject(levelFile, object);
				}
			}
		}

		checkLevel();
		levelFile.close();
		LogInfo("File " + location + " saved successfully");
	} else {
		LogError("Failed to save " + location + " file");
	}
}

void Game::Level::calculateLasers()
{
	clearDots();

	for (size_t beamerIndex = 0; beamerIndex < objectList[OBJ_BEAMER].size(); ++beamerIndex) {
		Beamer *beamer = static_cast<Beamer*>(objectList[OBJ_BEAMER][beamerIndex]);
		beamer->laser.clear();
		if (!beamer->inStack) {
			RayGen newRayGen = {beamer->direction, beamer->position, beamer->color};
			createRay(beamer, newRayGen);
		}
	}

	updateDots();
}

void Game::Level::createRay(Beamer *beamer, RayGen &rayGen, RayType rayType)
{
	switch (rayType) {
	case RT_TANGLED: {
		RayGen rayGens[2] = {{DIR(rayGen.direction - 2), rayGen.position, rayGen.color}, {DIR(rayGen.direction + 2), rayGen.position, rayGen.color}};
		Ray rays[2] = {{sf::Vertex(rayGen.position, rayGen.color.convertToColor())}, {sf::Vertex(rayGen.position, rayGen.color.convertToColor())}};

		for (unsigned short ray = 0; ray < 2; ++ray) {
			rayGens[ray].stop = true;
			rayGens[ray].end = false;
			rayGens[ray].endAtMiddle = true;
		}

		while (!(rayGens[0].end and rayGens[1].end)) {
			Color previousColors[2] = {rayGens[0].color, rayGens[1].color};
			for (unsigned short ray = 0; ray < 2; ++ray) {
				if (!rayGens[ray].end) {
					rayGens[ray].colorShift = rayStep(beamer, rayGens[ray]).colorShift;
				}
			}

			for (unsigned short ray = 0; ray < 2; ++ray) {
				if (rayGens[1 - ray].colorShift != CLS_NONE) {
					rayGens[ray].color = rayGens[ray].color.shiftColor(CLS_REVERSE(rayGens[1 - ray].colorShift));
				}
				if (previousColors[ray] != rayGens[ray].color) {
					rays[ray].push_back(sf::Vertex(rayGens[ray].position, previousColors[ray].convertToColor()));
				}

				sf::Vertex node(rayGens[ray].position, rayGens[ray].color.convertToColor());
				if (!rayGens[ray].endAtMiddle) {
					node.position.x -= rayGens[ray].delta.x * 0.5f;
					node.position.y -= rayGens[ray].delta.y * 0.5f;
				}

				rays[ray].push_back(node);
			}
		}

		for (unsigned short ray = 0; ray < 2; ++ray) {
			beamer->laser.push_back(rays[ray]);
		}

		break;
	}
	default: {
		Ray ray = {sf::Vertex(rayGen.position, rayGen.color.convertToColor())};

		rayGen.end = false;
		while (!rayGen.end) {
			rayGen.stop = false;
			rayGen.endAtMiddle = true;
			Color previousColor = rayGen.color;
			while (!rayGen.stop) {
				rayStep(beamer, rayGen);
			}

			if (previousColor != rayGen.color) {
				ray.push_back(sf::Vertex(rayGen.position, previousColor.convertToColor()));
			}

			sf::Vertex node(rayGen.position, rayGen.color.convertToColor());
			if (!rayGen.endAtMiddle) {
				node.position.x -= rayGen.delta.x * 0.5f;
				node.position.y -= rayGen.delta.y * 0.5f;
			}

			ray.push_back(node);
		}

		beamer->laser.push_back(ray);
	}

	break;
	}
}

void Game::Level::createRays(Beamer *beamer, std::vector<RayGenElement> rayGens)
{
	for (auto &rayGen : rayGens) {
		createRay(beamer, rayGen.first, rayGen.second);
	}
}

RayGen Game::Level::rayStep(Beamer *beamer, RayGen &rayGen)
{
	if (rayGen.color == COL_BLACK) {
		rayGen.stop = rayGen.end = true;
		return {DIR_NORTH, rayGen.position, COL_BLACK};
	}

	rayGen.colorShift = CLS_NONE;
	rayGen.delta = rayGen.position;
	rayGen.position.moveInDirection(rayGen.direction, 1);
	rayGen.delta = sf::Vector2f(rayGen.position) - rayGen.delta;
	if (isPlaceTaken(rayGen.position)) {
		createRays(beamer, objectMap[rayGen.position]->interaction(rayGen));
	}

	if (isOutsideBoard(rayGen.position) || obstacles[rayGen.position]) {
		rayGen.stop = rayGen.end = true;
		rayGen.endAtMiddle = false;
	}

	return rayGen;
}

void Game::Level::resetLevel(bool ignoreSave)
{
	clearLevel();
	loadLevel(game->levelId, ignoreSave);
}

void Game::Level::clearDots()
{
	for (size_t dotIndex = 0; dotIndex < objectList[OBJ_DOT].size(); ++dotIndex) {
		Dot* dot = (Dot*) objectList[OBJ_DOT][dotIndex];
		dot->actualColor = COL_BLACK_TUPLE;
	}
}

void Game::Level::updateDots()
{
	for (size_t dotIndex = 0; dotIndex < objectList[OBJ_DOT].size(); ++dotIndex) {
		Dot* dot = (Dot*) objectList[OBJ_DOT][dotIndex];
		dot->updateState();
	}
}

unsigned short Game::Level::countObjects()
{
	unsigned short objectsCount = 0;
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		objectsCount += countObjects(type);
	}

	return objectsCount;
}

unsigned short Game::Level::countObjects(bool inStack)
{
	unsigned short objectsCount = 0;
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < (inStack ? stack.objectList[type] : objectList[type]).size(); ++index) {
			if (inStack) {
				objectsCount++;
			} else if (!objectList[type][index]->inStack) {
				objectsCount++;
			}
		}
	}

	return objectsCount;
}

unsigned short Game::Level::countObjects(ObjectID id)
{
	return static_cast<unsigned short>(objectList[id].size());
}

bool Game::Level::checkWin()
{
	bool win = true;
	for (size_t dotIndex = 0; dotIndex < objectList[OBJ_DOT].size(); ++dotIndex) {
		Dot* dot = (Dot*) objectList[OBJ_DOT][dotIndex];
		if (!dot->state) {
			win = false;
			break;
		}
	}

	if (win) {
		LogInfo("Level completed!");
	}

	return win;
}

bool Game::Level::addObject(Position position, ObjectID id)
{
	bool onStack = stack.isOnStack(position);
	bool success = isPlaceFree(position, onStack);

	if (success) {
		newObject(getRelativePosition(position), id, onStack);
	}

	return success;
}

void Game::Level::newObject(Position position, ObjectID id, bool inStack)
{
	if (id == OBJ_BEAMER) {
		if (!inStack) {
			Beamer *beamer = new Beamer();
			setObject(beamer, position, id, DIR_NORTH, inStack);
		} else {
			LogWarning("Can't place a beamer into the stack!");
		}
	} else if (id == OBJ_DOT) {
		if (!inStack) {
			Dot* dot = new Dot();
			setObject(dot, position, id, DIR_NORTH, inStack);
		} else {
			LogWarning("Can't place a dot into the stack!");
		}
	} else if (id == OBJ_MIRROR) {
		Mirror* mirror = new Mirror();
		setObject(mirror, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_BENDER) {
		Bender* bender = new Bender();
		setObject(bender, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_SPLITTER) {
		Splitter* splitter = new Splitter();
		setObject(splitter, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_CONDUIT) {
		Conduit* conduit = new Conduit();
		setObject(conduit, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_FILTER) {
		Filter* filter = new Filter();
		setObject(filter, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_PRISM) {
		Prism* prism = new Prism();
		setObject(prism, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_DOPPLER) {
		Doppler* doppler = new Doppler();
		setObject(doppler, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_TANGLER) {
		Tangler* tangler = new Tangler();
		setObject(tangler, position, id, DIR_NORTH, inStack);
	}
}

bool Game::Level::changeObjectColor(Position mousePosition)
{
	bool onStack = stack.isOnStack(mousePosition);
	Object *object = getObject(mousePosition);
	bool success = isPlaceTaken(mousePosition, onStack);

	if (success) {
		object->color.nextColor();
		if (object->colorable) {
			object->setSpriteColor();
		}
	}

	return success;
}

Object *Game::Level::getObject(Position mousePosition)
{
	bool onStack = stack.isOnStack(mousePosition);
	Position position = getRelativePosition(mousePosition);
	return (onStack ? stack.objectMap[position] : objectMap[position]);
}

Position Game::Level::getRelativePosition(Position mousePosition)
{
	return (stack.isOnStack(mousePosition) ? stack.getRelativePosition(mousePosition) : mousePosition);
}

bool Game::Level::isPlaceFree(Position position)
{
	return (objectMap[position] == nullptr && !obstacles[position] && !isOutsideBoard(position));
}

bool Game::Level::isPlaceFree(Position position, bool onStack)
{
	if (onStack) {
		return stack.isPlaceFree(getRelativePosition(position));
	} else {
		return isPlaceFree(position);
	}
}

bool Game::Level::isPlaceTaken(Position position)
{
	return (objectMap[position] != nullptr && !objectMap[position]->inStack);
}

bool Game::Level::isPlaceTaken(Position position, bool onStack)
{
	if (onStack) {
		return stack.isPlaceTaken(getRelativePosition(position));
	} else {
		return isPlaceTaken(position);
	}
}

bool Game::Level::isOutsideBoard(Position position)
{
	return (position.getX() < 0 || position.getY() < 0 || position.getX() >= width || position.getY() >= height);
}

bool Game::Level::dragObject(Drag & drag, Position position)
{
	bool success = isPlaceTaken(position) && (objectMap[position]->movable || game->editor.isActive());

	if (success) {
		drag.fromStack = false;
		drag.position = position;
		drag.sprite = (objectMap[position]->id == OBJ_BEAMER ? objectMap[position]->baseSprite : objectMap[position]->sprite);
		objectMap[position]->inStack = false;
	}

	return success;
}

bool Game::Level::dragStackObject(Drag & drag, Position mousePosition)
{
	Position stackPosition = stack.getRelativePosition(mousePosition);
	bool success = stack.isPlaceTaken(stackPosition);

	if (success) {
		drag.fromStack = true;
		drag.position = stackPosition;
		drag.sprite = stack.objectMap[stackPosition]->sprite;
		stack.objectMap[stackPosition]->inStack = true;
	}

	return success;
}

bool Game::Level::moveObject(Position start, Position end)
{
	bool success = isPlaceFree(end);

	if (success) {
		objectMap[start]->position = end;
		objectMap[end] = objectMap[start];
		objectMap[start] = nullptr;
		objectMap[end]->updateSprite();
	}

	return success;
}

bool Game::Level::removeObject(Position position)
{
	bool success = isPlaceTaken(position);

	if (success) {
		ObjectID id = objectMap[position]->id;
		objectList[id].erase(std::find(objectList[id].begin(), objectList[id].end(), objectMap[position]));
		delete objectMap[position];
		objectMap[position] = nullptr;
	}

	return success;
}

bool Game::Level::rotateObject(Position mousePosition, bool clockwise)
{
	bool onStack = stack.isOnStack(mousePosition);
	Object *object = getObject(mousePosition);
	bool success = isPlaceTaken(mousePosition, onStack);

	if (success) {
		object->rotate(clockwise, game->editor.isActive());
	}

	return success;
}

bool Game::Level::setObstacle(Position position, bool obstacle)
{
	removeObject(position);
	setTile(position, obstacle);

	return true;
}

void Game::Level::setObject(Object *object, short x, short y, ObjectID id, DirectionID direction, bool inStack, bool stackObject)
{
	setObject(object, shortToPosition(x, y), id, direction, inStack, stackObject);
}

void Game::Level::setObject(Object *object, Position position, ObjectID id, DirectionID direction, bool inStack, bool stackObject)
{
	object->id = id;
	object->position.setPosition(position);
	object->direction = direction;
	object->inStack = inStack;

	object->textures.push_back(game->graphics.textures[id]);
	object->sprite.setOrigin(ORIGIN);
	object->sprite.setPosition(inStack ? object->position + stack.offset : object->position);
	object->sprite.setTexture(*(object->textures)[0]);
	object->sprite.setRotation(direction * 45);

	if (object->colorable) {
		object->setSpriteColor();
	}

	if (!inStack) {
		game->level.objectMap[object->position] = object;
	} else {
		game->level.stack.objectMap[object->position] = object;
		game->level.stack.objectList[id].push_back(object);
	}

	if (stackObject) {
		game->level.stackObjectList.push_back(object);
	}

	switch (id) {
	case OBJ_BEAMER:
		object->textures.push_back(game->graphics.textures[OBJ_COUNT]);
		object->baseSprite.setOrigin(ORIGIN);
		object->baseSprite.setPosition(inStack ? object->position + stack.offset : object->position);
		object->baseSprite.setTexture(*(object->textures)[1]);
		object->baseSprite.setRotation(direction * 45);
		break;
	case OBJ_DOT:
		object->textures.push_back(game->graphics.textures[OBJ_COUNT + 1]);
		break;
	default:
		break;
	}

	game->level.objectList[id].push_back(object);
}

void Game::Level::setTile(Position position, bool obstacle)
{
	obstacles[position] = obstacle;
	tileSprites[position].setTexture(*game->graphics.tiles[static_cast<size_t>(obstacles[position])]);
	tileSprites[position].setPosition(positionToFloat(position));
}

bool Game::Level::moveFromStack(Position stackPosition, Position mousePosition)
{
	bool success = !stack.isPlaceFree(stackPosition) && isPlaceFree(mousePosition);

	if (success) {
		objectMap[mousePosition] = stack.objectMap[stackPosition];
		objectMap[mousePosition]->position = mousePosition;
		objectMap[mousePosition]->sprite.setPosition(mousePosition);
		objectMap[mousePosition]->inStack = false;
		ObjectID id = stack.objectMap[stackPosition]->id;
		stack.objectList[id].erase(std::find(stack.objectList[id].begin(), stack.objectList[id].end(), stack.objectMap[stackPosition]));
		stack.objectMap[stackPosition] = nullptr;
	}

	return success;
}

bool Game::Level::moveToStack(Position dragPosition, Position mousePosition)
{
	Position stackPosition = stack.getRelativePosition(mousePosition);
	bool success = stack.isPlaceFree(stackPosition) && isPlaceTaken(dragPosition);

	if (success) {
		stack.objectMap[stackPosition] = objectMap[dragPosition];
		stack.objectMap[stackPosition]->position = stackPosition;
		stack.objectMap[stackPosition]->sprite.setPosition(stackPosition + stack.offset);
		stack.objectMap[stackPosition]->inStack = true;
		objectMap[dragPosition] = nullptr;
		stack.objectList[stack.objectMap[stackPosition]->id].push_back(stack.objectMap[stackPosition]);
	}

	return success;
}

bool Game::Level::moveFromStackToStack(Position dragPosition, Position mousePosition)
{
	Position stackPosition = stack.getRelativePosition(mousePosition);
	bool success = stack.isPlaceTaken(dragPosition) && stack.isPlaceFree(stackPosition);

	if (success) {
		stack.objectMap[stackPosition] = stack.objectMap[dragPosition];
		stack.objectMap[stackPosition]->position = stackPosition;
		stack.objectMap[stackPosition]->sprite.setPosition(stackPosition + stack.offset);
		stack.objectMap[stackPosition]->inStack = true;
		stack.objectMap[dragPosition] = nullptr;
	}

	return success;
}

void Game::Level::updateStack()
{
	Position offset;
	offset.setPosition(width + STACK_OFFSET_X, STACK_OFFSET_Y);
	stack.offset.setPosition(offset);
	for (size_t row = 0; row < height; ++row) {
		for (size_t column = 0; column < width; ++column) {
			Position currentPosition = shortToPosition(row, column);
			stack.sprites[currentPosition].setTexture(*game->graphics.tiles[TIL_EMPTY]);
			stack.sprites[currentPosition].setPosition(positionToFloat(currentPosition + offset));
		}
	}
}