#include "level.hpp"
#include "auxiliary.hpp"
#include "log.hpp"

bool Level::checkLevelSave(const std::string &id)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + game->levelSet.name + "/" + id + PATH_SAV_SUFFIX;
	std::ifstream levelSetSaveFile(location);
	return levelSetSaveFile.good();
}

void Level::checkLevel()
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

void Level::clearLevel()
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

void Level::loadLevel(const unsigned short level, bool ignoreSave)
{
	loadLevel(numberToString(level), ignoreSave);
}

void Level::loadLevel(const std::string &id, bool ignoreSave)
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

		if (checkLevelSave(id) and !ignoreSave) {
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

void Level::saveLevel(const std::string &id)
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

void Level::calculateLasers()
{
	clearDots();
	for (Object *object : objectList[OBJ_BEAMER]) {
		Beamer* beamer = (Beamer*) object;
		beamer->laser.clear();
		if (!beamer->inStack) {
			RayGen newRayGen = {beamer->direction, beamer->position, beamer->color};
			createRay(beamer, newRayGen);
		}
	}

	updateDots();
}

void Level::createRay(Beamer *beamer, RayGen &rayGen, RayType rayType)
{
	switch (rayType) {
	case RT_TANGLED: {
		RayGen rayGens[2] = {{DIR(rayGen.direction - 2), rayGen.position, rayGen.color}, {DIR(rayGen.direction + 2), rayGen.position, rayGen.color}};
		Ray rays[2] = {{sf::Vertex(rayGen.position, rayGen.color.convertToRealColor())}, {sf::Vertex(rayGen.position, rayGen.color.convertToRealColor())}};

		for (unsigned short ray = 0; ray < 2; ++ray) {
			rayGens[ray].stop = true;
			rayGens[ray].end = false;
			rayGens[ray].endAtMiddle = true;
		}

		while (!(rayGens[0].end and rayGens[1].end)) {
			Color previousColors[2] = {rayGens[0].color, rayGens[1].color};
			for (unsigned short ray = 0; ray < 2; ++ray) {
				if (!rayGens[ray].end) {
					rayStep(beamer, rays[ray], rayGens[ray]);
				}
			}

			for (unsigned short ray = 0; ray < 2; ++ray) {
				if (!(isOutsideBoard(rayGens[ray].position) or obstacles[rayGens[ray].position])) {
					if (rayGens[1 - ray].colorShift != CLS_NONE) {
						rayGens[ray].color = rayGens[ray].color.shiftColor(CLS_REVERSE(rayGens[1 - ray].colorShift));
					}
					if (previousColors[ray] != rayGens[ray].color) {
						addNode(rays[ray], sf::Vertex(rayGens[ray].position, previousColors[ray].convertToRealColor()));
					}
				}

				sf::Vertex node(rayGens[ray].position, rayGens[ray].color.convertToRealColor());
				if (!rayGens[ray].endAtMiddle) {
					node.position.x -= rayGens[ray].delta.x * 0.5f;
					node.position.y -= rayGens[ray].delta.y * 0.5f;
				}

				addNode(rays[ray], node);
			}
		}

		for (unsigned short ray = 0; ray < 2; ++ray) {
			beamer->laser.push_back(rays[ray]);
		}

		break;
	}
	default: {
		Ray ray = {sf::Vertex(rayGen.position, rayGen.color.convertToRealColor())};

		rayGen.end = false;
		while (!rayGen.end) {
			rayGen.stop = false;
			rayGen.endAtMiddle = true;
			Color previousColor = rayGen.color;
			while (!rayGen.stop) {
				rayStep(beamer, ray, rayGen);
			}

			if (previousColor != rayGen.color) {
				addNode(ray, sf::Vertex(rayGen.position, previousColor.convertToRealColor()));
			}

			sf::Vertex node(rayGen.position, rayGen.color.convertToRealColor());
			if (!rayGen.endAtMiddle) {
				node.position.x -= rayGen.delta.x * 0.5f;
				node.position.y -= rayGen.delta.y * 0.5f;
			}

			addNode(ray, node);
		}

		beamer->laser.push_back(ray);
	}

	break;
	}
}

void Level::createRays(Beamer *beamer, RayGenList rayGens)
{
	for (RayGenElement &rayGen : rayGens) {
		createRay(beamer, rayGen.first, rayGen.second);
	}
}

void Level::rayStep(Beamer *beamer, Ray &ray, RayGen &rayGen)
{
	if (rayGen.color == COL_BLACK) {
		rayGen.stop = rayGen.end = true;
		return;
	}

	rayGen.colorShift = CLS_NONE;
	rayGen.delta = rayGen.position;
	rayGen.position.moveInDirection(rayGen.direction, 1);
	rayGen.delta = sf::Vector2f(rayGen.position) - rayGen.delta;

	if (isPlaceTaken(rayGen.position)) {
		createRays(beamer, objectMap[rayGen.position]->interaction(ray, rayGen));
	}

	if (isOutsideBoard(rayGen.position) or obstacles[rayGen.position]) {
		rayGen.stop = rayGen.end = true;
		rayGen.endAtMiddle = false;
	}
}

void Level::resetLevel(bool ignoreSave)
{
	clearLevel();
	loadLevel(game->levelId, ignoreSave);
}

void Level::clearDots()
{
	for (Object *object : objectList[OBJ_DOT]) {
		Dot* dot = (Dot*) object;
		dot->actualColor = COL_BLACK;
	}
}

void Level::updateDots()
{
	for (Object *object : objectList[OBJ_DOT]) {
		Dot* dot = (Dot*) object;
		dot->updateState();
	}
}

unsigned short Level::countObjects()
{
	unsigned short objectsCount = 0;
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		objectsCount += countObjects(type);
	}

	return objectsCount;
}

unsigned short Level::countObjects(bool inStack)
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

unsigned short Level::countObjects(ObjectID id)
{
	return static_cast<unsigned short>(objectList[id].size());
}

bool Level::checkWin()
{
	bool win = true;
	for (Object *object : objectList[OBJ_DOT]) {
		Dot* dot = (Dot*) object;
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

bool Level::addObject(Position position, ObjectID id)
{
	bool onStack = stack.isOnStack(position);
	bool success = isPlaceFree(position, onStack);

	if (success) {
		newObject(getRelativePosition(position), id, onStack);
	}

	return success;
}

void Level::newObject(Position position, ObjectID id, bool inStack)
{
	///TODO: abstraction
	typeMap[id];
	if (id == OBJ_BEAMER) {
		if (!inStack) {
			Beamer *beamer = new Beamer();
			beamer->setObject(game, position, id, DIR_NORTH, inStack);
		} else {
			LogWarning("Can't place a beamer into the stack!");
		}
	} else if (id == OBJ_DOT) {
		if (!inStack) {
			Dot* dot = new Dot();
			dot->setObject(game, position, id, DIR_NORTH, inStack);
		} else {
			LogWarning("Can't place a dot into the stack!");
		}
	} else if (id == OBJ_MIRROR) {
		Mirror* mirror = new Mirror();
		mirror->setObject(game, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_BENDER) {
		Bender* bender = new Bender();
		bender->setObject(game, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_SPLITTER) {
		Splitter* splitter = new Splitter();
		splitter->setObject(game, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_CONDUIT) {
		Conduit* conduit = new Conduit();
		conduit->setObject(game, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_FILTER) {
		Filter* filter = new Filter();
		filter->setObject(game, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_PRISM) {
		Prism* prism = new Prism();
		prism->setObject(game, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_DOPPLER) {
		Doppler* doppler = new Doppler();
		doppler->setObject(game, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_TANGLER) {
		Tangler* tangler = new Tangler();
		tangler->setObject(game, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_TELEPORTER) {
		Teleporter* teleporter = new Teleporter();
		teleporter->setObject(game, position, id, DIR_NORTH, inStack);
	}
}

bool Level::changeObjectColor(Position mousePosition)
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

Object *Level::getObject(Position mousePosition)
{
	bool onStack = stack.isOnStack(mousePosition);
	Position position = getRelativePosition(mousePosition);
	return (onStack ? stack.objectMap[position] : objectMap[position]);
}

Position Level::getRelativePosition(Position mousePosition)
{
	return (stack.isOnStack(mousePosition) ? stack.getRelativePosition(mousePosition) : mousePosition);
}

bool Level::isPlaceFree(Position position)
{
	return (objectMap[position] == nullptr and !obstacles[position] and !isOutsideBoard(position));
}

bool Level::isPlaceFree(Position position, bool onStack)
{
	if (onStack) {
		return stack.isPlaceFree(getRelativePosition(position));
	} else {
		return isPlaceFree(position);
	}
}

bool Level::isPlaceTaken(Position position)
{
	return (objectMap[position] != nullptr and !objectMap[position]->inStack);
}

bool Level::isPlaceTaken(Position position, bool onStack)
{
	if (onStack) {
		return stack.isPlaceTaken(getRelativePosition(position));
	} else {
		return isPlaceTaken(position);
	}
}

bool Level::isOutsideBoard(Position position)
{
	return (position.getX() < 0 or position.getY() < 0 or position.getX() >= width or position.getY() >= height);
}

bool Level::dragObject(Drag & drag, Position position)
{
	bool success = isPlaceTaken(position) and (objectMap[position]->movable or game->editor.isActive());

	if (success) {
		drag.fromStack = false;
		drag.position = position;
		drag.sprite = (objectMap[position]->id == OBJ_BEAMER ? objectMap[position]->baseSprite : objectMap[position]->sprite);
		objectMap[position]->inStack = false;
	}

	return success;
}

bool Level::dragStackObject(Drag & drag, Position mousePosition)
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

bool Level::moveObject(Position start, Position end)
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

bool Level::removeObject(Position position)
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

bool Level::rotateObject(Position mousePosition, bool clockwise)
{
	bool onStack = stack.isOnStack(mousePosition);
	Object *object = getObject(mousePosition);
	bool success = isPlaceTaken(mousePosition, onStack);

	if (success) {
		object->rotate(clockwise, game->editor.isActive());
	}

	return success;
}

bool Level::setObstacle(Position position, bool obstacle)
{
	removeObject(position);
	setTile(position, obstacle);

	return true;
}

void Level::setTile(Position position, bool obstacle)
{
	obstacles[position] = obstacle;
	tileSprites[position].setTexture(*game->graphics.tiles[static_cast<size_t>(obstacles[position])]);
	tileSprites[position].setPosition(positionToFloat(position));
}

bool Level::moveFromStack(Position stackPosition, Position mousePosition)
{
	bool success = !stack.isPlaceFree(stackPosition) and isPlaceFree(mousePosition);

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

bool Level::moveToStack(Position dragPosition, Position mousePosition)
{
	Position stackPosition = stack.getRelativePosition(mousePosition);
	bool success = stack.isPlaceFree(stackPosition) and isPlaceTaken(dragPosition);

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

bool Level::moveFromStackToStack(Position dragPosition, Position mousePosition)
{
	Position stackPosition = stack.getRelativePosition(mousePosition);
	bool success = stack.isPlaceTaken(dragPosition) and stack.isPlaceFree(stackPosition);

	if (success) {
		stack.objectMap[stackPosition] = stack.objectMap[dragPosition];
		stack.objectMap[stackPosition]->position = stackPosition;
		stack.objectMap[stackPosition]->sprite.setPosition(stackPosition + stack.offset);
		stack.objectMap[stackPosition]->inStack = true;
		stack.objectMap[dragPosition] = nullptr;
	}

	return success;
}

void Level::updateStack()
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
