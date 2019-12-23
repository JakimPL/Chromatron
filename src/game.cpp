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

void Game::Level::clearLevel()
{
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		while (!objectList[type].empty()) {
			delete objectList[type].back();
			objectList[type].pop_back();
		}
	}

	objectMap.clear();
	stackObjectList.clear();

	for (short y = 0; y < height; ++y) {
		for (short x = 0; x < width; ++x) {
			Object::Position currentPosition = shortToPosition(x, y);
			setTile(currentPosition, false);
		}
	}
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
				Object::Position currentPosition = shortToPosition(x, y);
				objectMap[currentPosition] = nullptr;

				bool obstacle;
				levelFile.read((char*)&obstacle, 1);
				setTile(currentPosition, obstacle);
			}
		}

		unsigned short objectsCount;
		readByte(levelFile, objectsCount);

		for (unsigned short obj = 0; obj < objectsCount; ++obj) {
			readObject(levelFile, game->level);
		}

		if (checkLevelSave(id) && !ignoreSave) {
			levelFile.close();
			LogInfo("File " + location + " loaded successfully");
			location = PATH_DATA + PATH_LEV_PREFIX + game->levelSet.name + "/" + id + PATH_SAV_SUFFIX;
			levelFile.open(location, std::ios::binary | std::ios::in);
		}

		unsigned short stackObjectsCount;
		readByte(levelFile, stackObjectsCount);

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
				Object::Position currentPosition = shortToPosition(x, y);
				levelFile.write((char*) & (obstacles[currentPosition]), 1);
			}
		}

		unsigned short objectsCount = countObjects(false);
		writeByte(levelFile, objectsCount);

		for (size_t type = 0; type < OBJ_COUNT; ++type) {
			for (size_t index = 0; index < objectList[type].size(); ++index) {
				if (!objectList[type][index]->inStack) {
					Object* object = objectList[type][index];
					writeObject(levelFile, object);
				}
			}
		}

		unsigned short stackObjectsCount = countObjects(true);
		writeByte(levelFile, stackObjectsCount);

		for (size_t type = 0; type < OBJ_COUNT; ++type) {
			for (size_t index = 0; index < objectList[type].size(); ++index) {
				if (objectList[type][index]->inStack) {
					Object* object = objectList[type][index];
					writeObject(levelFile, object);
				}
			}
		}

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
		Beamer* beamer = static_cast<Beamer*>(objectList[OBJ_BEAMER][beamerIndex]);
		beamer->laser.clear();
		if (!beamer->inStack) {
			createRay(beamer, beamer->direction, beamer->position, beamer->color);
		}
	}

	updateDots();
}

void Game::Level::createRay(Beamer* beamer, unsigned short direction, Object::Position position, Color col)
{
	unsigned short dir = direction;
	Object::Position now = position;
	Color color = col;
	sf::Color sfColor = color.convertToColor();
	sf::Vector2f delta;

	Ray ray;
	ray.push_back(sf::Vertex(position, sfColor));

	bool end = false;
	while (!end) {
		bool stop = false;
		bool endAtMiddle = true;
		while (!stop) {
			delta = now;
			now.moveInDirection(dir, 1);
			delta = sf::Vector2f(now) - delta;

			if (isPlaceTaken(now)) {
				if (objectMap[now]->id == OBJ_BEAMER) {
					stop = end = true;
				} else if (objectMap[now]->id == OBJ_DOT) {
					Dot* dot = static_cast<Dot*>(objectMap[now]);
					dot->actualColor = dot->actualColor + color;
				} else if (objectMap[now]->id == OBJ_MIRROR) {
					Mirror* mirror = static_cast<Mirror*>(objectMap[now]);
					short diff = (DIR_COUNT + mirror->direction - dir) % DIR_COUNT - 4;
					if (std::abs(diff) <= 1) {
						stop = true;
						dir = (DIR_COUNT + dir - (diff == 0 ? 4 : 2 * diff)) % DIR_COUNT;
					} else {
						stop = end = true;
					}
				} else if (objectMap[now]->id == OBJ_BENDER) {
					Bender* mirror = static_cast<Bender*>(objectMap[now]);
					short diff = (DIR_COUNT + mirror->direction - dir + 7) % DIR_COUNT - 4;
					if (-2 <= diff && diff < 2) {
						stop = true;
						dir = (DIR_COUNT + dir + (2 * diff + 5)) % DIR_COUNT;
					} else {
						stop = end = true;
					}
				} else if (objectMap[now]->id == OBJ_SPLITTER) {
					Splitter* splitter = static_cast<Splitter*>(objectMap[now]);
					short diff = (DIR_COUNT + splitter->direction - dir) % (DIR_COUNT / 2) - 2;
					if (diff == 0) {
						stop = end = true;
					} else if (std::abs(diff) == 1) {
						unsigned short newDirection = (DIR_COUNT + dir + 2 * diff) % DIR_COUNT;
						createRay(beamer, newDirection, now, col);
					}
				} else if (objectMap[now]->id == OBJ_CONDUIT) {
					Conduit* conduit = static_cast<Conduit*>(objectMap[now]);
					short diff = (DIR_COUNT + conduit->direction - dir) % (DIR_COUNT / 2) - 2;
					if (diff != 0) {
						stop = end = true;
					}
				} else if (objectMap[now]->id == OBJ_FILTER) {
					Conduit* filter = static_cast<Conduit*>(objectMap[now]);
					short diff = (DIR_COUNT + filter->direction - dir + 2) % (DIR_COUNT / 2) - 2;
					if (diff == 0) {
						Color newColor = (filter->color * col);
						if (!newColor.isBlack()) {
							createRay(beamer, dir, now, newColor);
						}
					}
					stop = end = true;
				}
			}

			if (isOutsideBoard(now) || obstacles[now]) {
				stop = end = true;
				endAtMiddle = false;
			}
		}

		sf::Vertex node(now, sfColor);
		if (!endAtMiddle) {
			node.position.x -= delta.x * 0.5f;
			node.position.y -= delta.y * 0.5f;
		}

		ray.push_back(node);
	}

	beamer->laser.push_back(ray);
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
		for (size_t index = 0; index < objectList[type].size(); ++index) {
			objectsCount++;
		}
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
		LogInfo("Level cleared!");
	}

	return win;
}

bool Game::Level::addObject(Object::Position position, ObjectID id)
{
	bool onStack = stack.isOnStack(position);
	bool success = isPlaceFree(position, onStack);

	if (success) {
		newObject(getRelativePosition(position), id, onStack);
	}

	return success;
}

void Game::Level::newObject(Object::Position position, ObjectID id, bool inStack)
{
	if (id == OBJ_BEAMER) {
		Beamer* beamer = new Beamer();
		setObject(beamer, position, id, DIR_NORTH, inStack);
	} else if (id == OBJ_DOT) {
		Dot* dot = new Dot();
		setObject(dot, position, id, DIR_NORTH, inStack);
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

bool Game::Level::changeObjectColor(Object::Position mousePosition)
{
	bool onStack = stack.isOnStack(mousePosition);
	Object* object = getObject(mousePosition);
	bool success = isPlaceTaken(mousePosition, onStack);

	if (success) {
		object->color.nextColor();
		if (object->colorable) {
			object->setSpriteColor();
		}
	}

	return success;
}

Object* Game::Level::getObject(Object::Position mousePosition)
{
	bool onStack = stack.isOnStack(mousePosition);
	Object::Position position = getRelativePosition(mousePosition);
	return (onStack ? stack.objectMap[position] : objectMap[position]);
}

Object::Position Game::Level::getRelativePosition(Object::Position mousePosition)
{
	return (stack.isOnStack(mousePosition) ? stack.getRelativePosition(mousePosition) : mousePosition);
}

bool Game::Level::isPlaceFree(Object::Position position)
{
	return (objectMap[position] == nullptr && !obstacles[position] && !isOutsideBoard(position));
}

bool Game::Level::isPlaceFree(Object::Position position, bool onStack)
{
	if (onStack) {
		return stack.isPlaceFree(getRelativePosition(position));
	} else {
		return isPlaceFree(position);
	}
}

bool Game::Level::isPlaceTaken(Object::Position position)
{
	return (objectMap[position] != nullptr && !objectMap[position]->inStack);
}

bool Game::Level::isPlaceTaken(Object::Position position, bool onStack)
{
	if (onStack) {
		return stack.isPlaceTaken(getRelativePosition(position));
	} else {
		return isPlaceTaken(position);
	}
}

bool Game::Level::isOutsideBoard(Object::Position position)
{
	return (position.getX() < 0 || position.getY() < 0 || position.getX() >= width || position.getY() >= height);
}

bool Game::Level::dragObject(Drag & drag, Object::Position position)
{
	bool success = isPlaceTaken(position) && (objectMap[position]->movable || game->editor.isActive());

	if (success) {
		drag.fromStack = false;
		drag.position = position;
		drag.sprite = objectMap[position]->sprite;
		objectMap[position]->inStack = false;
	}

	return success;
}

bool Game::Level::dragStackObject(Drag & drag, Object::Position mousePosition)
{
	Object::Position stackPosition = stack.getRelativePosition(mousePosition);
	bool success = stack.isPlaceTaken(stackPosition);

	if (success) {
		drag.fromStack = true;
		drag.position = stackPosition;
		drag.sprite = stack.objectMap[stackPosition]->sprite;
		stack.objectMap[stackPosition]->inStack = true;
	}

	return success;
}

bool Game::Level::moveObject(Object::Position start, Object::Position end)
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

bool Game::Level::removeObject(Object::Position position)
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

bool Game::Level::rotateObject(Object::Position mousePosition, bool clockwise)
{
	bool onStack = stack.isOnStack(mousePosition);
	Object* object = getObject(mousePosition);
	bool success = isPlaceTaken(mousePosition, onStack);

	if (success) {
		object->rotate(clockwise, game->editor.isActive());
	}

	return success;
}

bool Game::Level::setObstacle(Object::Position position, bool obstacle)
{
	removeObject(position);
	setTile(position, obstacle);

	return true;
}

void Game::Level::setObject(Object* object, short x, short y, ObjectID id, DirectionID direction, bool inStack, bool stackObject)
{
	setObject(object, shortToPosition(x, y), id, direction, inStack, stackObject);
}

void Game::Level::setObject(Object* object, Object::Position position, ObjectID id, DirectionID direction, bool inStack, bool stackObject)
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

void Game::Level::setTile(Object::Position position, bool obstacle)
{
	obstacles[position] = obstacle;
	tileSprites[position].setTexture(*game->graphics.tiles[static_cast<size_t>(obstacles[position])]);
	tileSprites[position].setPosition(positionToFloat(position));
}

bool Game::Level::moveFromStack(Object::Position stackPosition, Object::Position mousePosition)
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

bool Game::Level::moveToStack(Object::Position dragPosition, Object::Position mousePosition)
{
	Object::Position stackPosition = stack.getRelativePosition(mousePosition);
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

bool Game::Level::moveFromStackToStack(Object::Position dragPosition, Object::Position mousePosition)
{
	Object::Position stackPosition = stack.getRelativePosition(mousePosition);
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
	Object::Position offset;
	offset.setPosition(width + STACK_OFFSET_X, STACK_OFFSET_Y);
	stack.offset.setPosition(offset);
	for (size_t row = 0; row < height; ++row) {
		for (size_t column = 0; column < width; ++column) {
			Object::Position currentPosition = shortToPosition(row, column);
			stack.sprites[currentPosition].setTexture(*game->graphics.tiles[TIL_EMPTY]);
			stack.sprites[currentPosition].setPosition(positionToFloat(currentPosition + offset));
		}
	}
}