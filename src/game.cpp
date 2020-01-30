#include <fstream>
#include <iostream>

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
			createRay(beamer, beamer->direction, beamer->position, beamer->color);
		}
	}

	updateDots();
}

void Game::Level::createRay(Beamer *beamer, unsigned short direction, Position position, Color color)
{
	unsigned short dir = direction;
	Position now = position;
	Color col = color;
	sf::Vector2f delta;
	Ray ray = {sf::Vertex(position, col.convertToColor())};

	bool end = false;
	while (!end) {
		bool stop = false;
		bool endAtMiddle = true;
		Color previousColor = col;
		while (!stop) {
			rayStep(beamer, now, col, delta, dir, stop, end, endAtMiddle);
		}

		if (previousColor != col) {
			ray.push_back(sf::Vertex(now, previousColor.convertToColor()));
		}

		sf::Vertex node(now, col.convertToColor());
		if (!endAtMiddle) {
			node.position.x -= delta.x * 0.5f;
			node.position.y -= delta.y * 0.5f;
		}

		ray.push_back(node);
	}

	beamer->laser.push_back(ray);
}

void Game::Level::createTangledRay(Beamer *beamer, unsigned short direction, Position position, Color color)
{
	unsigned short dirs[2] = {DIR(direction - 2), DIR(direction + 2)};
	Position nows[2] = {position, position};
	Color cols[2] = {color, color};
	sf::Vector2f deltas[2];
	Ray rays[2] = {{sf::Vertex(position, color.convertToColor())}, {sf::Vertex(position, color.convertToColor())}};

	bool stops[2] = {true, true};
	bool ends[2] = {false, false};
	bool endAtMiddle = true;
	while (!(ends[0] and ends[1])) {
		Color previousColors[2] = {cols[0], cols[1]};
		ColorShift colorShifts[2] = {CLS_NONE, CLS_NONE};
		for (unsigned short ray = 0; ray < 2; ++ray) {
			if (!ends[ray]) {
				colorShifts[ray] = rayStep(beamer, nows[ray], cols[ray], deltas[ray], dirs[ray], stops[ray], ends[ray], endAtMiddle);
			}
		}

		for (unsigned short ray = 0; ray < 2; ++ray) {
			if (colorShifts[1 - ray] != CLS_NONE) {
				cols[ray] = cols[ray].shiftColor(CLS_REVERSE(colorShifts[1 - ray]));
			}
			if (previousColors[ray] != cols[ray]) {
				rays[ray].push_back(sf::Vertex(nows[ray], previousColors[ray].convertToColor()));
			}

			sf::Vertex node(nows[ray], cols[ray].convertToColor());
			if (!endAtMiddle) {
				node.position.x -= deltas[ray].x * 0.5f;
				node.position.y -= deltas[ray].y * 0.5f;
			}

			rays[ray].push_back(node);
		}
	}

	beamer->laser.push_back(rays[0]);
	beamer->laser.push_back(rays[1]);
}

ColorShift Game::Level::rayStep(Beamer *beamer, Position &now, Color &color, sf::Vector2f &delta, unsigned short &direction, bool &stop, bool &end, bool &endAtMiddle)
{
	if (color == COL_BLACK) {
		stop = end = true;
		return CLS_NONE;
	}

	ColorShift colorShift = CLS_NONE;

	delta = now;
	now.moveInDirection(direction, 1);
	delta = sf::Vector2f(now) - delta;

	if (isPlaceTaken(now)) {
		if (objectMap[now]->id == OBJ_BEAMER) {
			stop = end = true;
		} else if (objectMap[now]->id == OBJ_DOT) {
			Dot* dot = static_cast<Dot*>(objectMap[now]);
			dot->actualColor = dot->actualColor + color;
		} else if (objectMap[now]->id == OBJ_MIRROR) {
			Mirror* mirror = static_cast<Mirror*>(objectMap[now]);
			short diff = (DIR_COUNT + mirror->direction - direction) % DIR_COUNT - 4;
			if (ABS(diff) <= 1) {
				stop = true;
				direction = (DIR_COUNT + direction - (diff == 0 ? 4 : 2 * diff)) % DIR_COUNT;
			} else {
				stop = end = true;
			}
		} else if (objectMap[now]->id == OBJ_BENDER) {
			Bender* mirror = static_cast<Bender*>(objectMap[now]);
			short diff = (DIR_COUNT + mirror->direction - direction + 7) % DIR_COUNT - 4;
			if (-2 <= diff && diff < 2) {
				stop = true;
				direction = (DIR_COUNT + direction + (2 * diff + 5)) % DIR_COUNT;
			} else {
				stop = end = true;
			}
		} else if (objectMap[now]->id == OBJ_SPLITTER) {
			Splitter* splitter = static_cast<Splitter*>(objectMap[now]);
			short diff = (DIR_COUNT + splitter->direction - direction) % (DIR_COUNT / 2) - 2;
			if (diff == 0) {
				stop = end = true;
			} else if (ABS(diff) == 1) {
				unsigned short newDirection = (DIR_COUNT + direction + 2 * diff) % DIR_COUNT;
				createRay(beamer, newDirection, now, color);
			}
		} else if (objectMap[now]->id == OBJ_CONDUIT) {
			Conduit* conduit = static_cast<Conduit*>(objectMap[now]);
			short diff = (DIR_COUNT + conduit->direction - direction) % (DIR_COUNT / 2) - 2;
			if (diff != 0) {
				stop = end = true;
			}
		} else if (objectMap[now]->id == OBJ_FILTER) {
			Filter* filter = static_cast<Filter*>(objectMap[now]);
			short diff = (DIR_COUNT + filter->direction - direction + 2) % (DIR_COUNT / 2) - 2;
			if (diff == 0) {
				Color newColor = (filter->color * color);
				if (newColor != COL_BLACK) {
					if (color != newColor) {
						color = newColor;
						stop = true;
					}
				} else {
					stop = end = true;
				}
			} else {
				stop = end = true;
			}
		} else if (objectMap[now]->id == OBJ_PRISM) {
			Prism* prism = static_cast<Prism*>(objectMap[now]);
			short diff = (DIR_COUNT + prism->direction - direction) % DIR_COUNT;
			if (color.red) {
				if ((diff / 2) % 2 != 0) {
					createRay(beamer, direction, now, colors[COL_RED]);
				}
			}
			if (color.green) {
				if (diff > 1 && ((diff + 1) % 3 != 1)) {
					createRay(beamer, (DIR_COUNT + direction - ((diff + 1) / 3) * 2 + 3) % DIR_COUNT, now, colors[COL_GREEN]);
				}
			}
			if (color.blue) {
				if (diff > 1 && ((diff + 1) % 3 != 1)) {
					createRay(beamer, (DIR_COUNT + direction - ((diff + 1) % 3) * 2 + 2) % DIR_COUNT, now, colors[COL_BLUE]);
				}
			}
			stop = end = true;
		} else if (objectMap[now]->id == OBJ_DOPPLER) {
			Doppler* doppler = static_cast<Doppler*>(objectMap[now]);
			short diff = (DIR_COUNT + doppler->direction - direction) % DIR_COUNT - 4;
			if ((diff + 2) % 4 == 0) {
				colorShift = static_cast<ColorShift>(1 + ((diff + 2) / 4));
				Color newColor = color.shiftColor(colorShift);
				if (newColor != COL_WHITE) {
					color = newColor;
					stop = true;
				}
			} else {
				stop = end = true;
			}
		} else if (objectMap[now]->id == OBJ_TANGLER) {
			Tangler* tangler = static_cast<Tangler*>(objectMap[now]);
			short diff = (DIR_COUNT + tangler->direction - direction) % DIR_COUNT - 4;
			if (diff == 0) {
				createTangledRay(beamer, direction, now, color);
			}
			stop = end = true;
		}
	}

	if (isOutsideBoard(now) || obstacles[now]) {
		stop = end = true;
		endAtMiddle = false;
	}

	return colorShift;
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