#include <fstream>
#include <iostream>

#include "game.h"
#include "auxiliary.h"

Game::Game()
{
	for (size_t index = 0; index < OBJ_COUNT; ++index) {
		textures.push_back(loadTexture(IMG_OBJECT_NAMES[index]));
	}

	textures.push_back(loadTexture(IMG_OBJECT_NAMES[OBJ_COUNT]));

	for (size_t index = 0; index < TIL_COUNT; ++index) {
		tiles.push_back(loadTexture(IMG_TILE_NAMES[index]));
	}
}

Game::~Game()
{
	for (size_t index = 0; index < textures.size(); ++index) {
		delete textures[index];
	}

	for (size_t index = 0; index < tiles.size(); ++index) {
		delete tiles[index];
	}
}

sf::Texture* Game::loadTexture(const std::string &filename)
{
	std::string location = PATH_DATA + PATH_IMG_PREFIX + filename + PATH_IMG_SUFFIX;
	sf::Texture* image = new sf::Texture();
	if (!image->loadFromFile(location)) {
		throw std::runtime_error("failed to load " + location + " image file");
	}

	return image;
}

unsigned short Game::Level::countObjects(bool inStack)
{
	unsigned short objectsCount = 0;
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < (inStack ? stack.objectList[type] : objectList[type]).size(); ++index) {
			objectsCount++;
		}
	}

	return objectsCount;
}

void Game::loadLevel(const std::string &id)
{
	///TODO: error handling
	level.game = this;

	std::string location = PATH_DATA + PATH_LEV_PREFIX + id + PATH_LEV_SUFFIX;
	std::ifstream levelFile(location, std::ios::binary | std::ios::in);
	if (levelFile.good()) {
		readByte(levelFile, level.width);
		readByte(levelFile, level.height);
		readByte(levelFile, level.stack.width);
		readByte(levelFile, level.stack.height);

		level.updateStack();

		for (short y = 0; y < level.height; ++y) {
			for (short x = 0; x < level.width; ++x) {
				Object::Position currentPosition;
				currentPosition.setPosition(x, y);
				level.objectMap[currentPosition] = nullptr;

				bool obstacle;
				levelFile.read((char*)&obstacle, 1);
				level.obstacles[currentPosition] = obstacle;
				level.setTile(currentPosition);
			}
		}

		unsigned short objectsCount;
		readByte(levelFile, objectsCount);

		for (unsigned short obj = 0; obj < objectsCount; ++obj) {
			readObject(levelFile, level);
		}

		levelFile.close();
	} else {
		throw std::runtime_error("failed to load " + location + " file");
	}
}

void Game::saveLevel(const std::string &id)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + id + PATH_LEV_SUFFIX;
	std::ofstream levelFile(location, std::ios::binary);
	if (levelFile.good()) {
		writeByte(levelFile, level.width);
		writeByte(levelFile, level.height);
		writeByte(levelFile, level.stack.width);
		writeByte(levelFile, level.stack.height);

		for (short y = 0; y < level.height; ++y) {
			for (short x = 0; x < level.width; ++x) {
				Object::Position currentPosition;
				currentPosition.setPosition(x, y);
				levelFile.write((char*) & (level.obstacles[currentPosition]), 1);
			}
		}

		unsigned short objectsCount = level.countObjects(false);
		writeByte(levelFile, objectsCount);

		for (size_t type = 0; type < OBJ_COUNT; ++type) {
			for (size_t index = 0; index < level.objectList[type].size(); ++index) {
				Object* object = level.objectList[type][index];
				writeObject(levelFile, object);
			}
		}

		levelFile.close();

	} else {
		throw std::runtime_error("failed to save " + location + " file");
	}
}

void Game::calculateLasers()
{
	clearDots();

	for (size_t beamerIndex = 0; beamerIndex < level.objectList[OBJ_BEAMER].size(); ++beamerIndex) {
		Beamer* beamer = (Beamer*) level.objectList[OBJ_BEAMER][beamerIndex];
		beamer->laser.clear();

		unsigned short dir = beamer->direction;
		Object::Position now = beamer->position;
		Color color = beamer->color;
		sf::Color sfColor = color.convertToColor();

		sf::Vector2f delta;

		Ray ray;
		ray.push_back(sf::Vertex(beamer->position, sfColor));

		bool end = beamer->inStack;
		while (!end) {
			bool stop = false;
			bool endAtMiddle = true;
			while (!stop) {
				delta = now;
				now.moveInDirection(dir, 1);
				delta = sf::Vector2f(now) - delta;

				if (level.isPlaceTaken(now)) {
					if (level.objectMap[now]->id == OBJ_BEAMER) {
						stop = end = true;
					} else if (level.objectMap[now]->id == OBJ_DOT) {
						Dot* dot = (Dot*) level.objectMap[now];
						dot->actualColor = dot->actualColor + color;
					} else if (level.objectMap[now]->id == OBJ_MIRROR) {
						Mirror* mirror = (Mirror*) level.objectMap[now];
						int diff = (DIR_COUNT + mirror->direction - dir) % DIR_COUNT - 4;
						if (std::abs(diff) <= 1) {
							stop = true;
							dir = (DIR_COUNT + dir - (diff == 0 ? 4 : 2 * diff)) % DIR_COUNT;
						} else {
							stop = end = true;
						}
					}
				}

				if (level.isOutsideBoard(now) || level.obstacles[now]) {
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

	updateDots();
}

void Game::clearDots()
{
	for (size_t dotIndex = 0; dotIndex < level.objectList[OBJ_DOT].size(); ++dotIndex) {
		Dot* dot = (Dot*) level.objectList[OBJ_DOT][dotIndex];
		dot->actualColor = COL_BLACK_TUPLE;
	}
}

void Game::updateDots()
{
	for (size_t dotIndex = 0; dotIndex < level.objectList[OBJ_DOT].size(); ++dotIndex) {
		Dot* dot = (Dot*) level.objectList[OBJ_DOT][dotIndex];
		dot->updateState();
		dot->setSpriteColor();
	}
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
	}
}

bool Game::Level::changeObjectColor(Object::Position mousePosition)
{
	bool onStack = stack.isOnStack(mousePosition);
	Object* object = getObject(mousePosition);
	bool success = isPlaceTaken(mousePosition, onStack);

	if (success) {
		object->color.nextColor();
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

bool Game::Level::dragObject(Drag &drag, Object::Position position)
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

bool Game::Level::dragStackObject(Drag &drag, Object::Position mousePosition)
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

bool Game::Level::rotateObject(Object::Position mousePosition)
{
	bool onStack = stack.isOnStack(mousePosition);
	Object* object = getObject(mousePosition);
	bool success = isPlaceTaken(mousePosition, onStack);

	if (success) {
		object->rotate(true, game->editor.isActive());
	}

	return success;
}

bool Game::Level::setObstacle(Object::Position position, bool obstacle)
{
	removeObject(position);
	obstacles[position] = obstacle;
	setTile(position);

	return true;
}

void Game::Level::setObject(Object* object, short x, short y, ObjectID id, DirectionID direction, bool inStack)
{
	setObject(object, shortToPosition(x, y), id, direction, inStack);
}

void Game::Level::setObject(Object* object, Object::Position position, ObjectID id, DirectionID direction, bool inStack)
{
	object->id = id;
	object->position.setPosition(position);
	object->direction = direction;
	object->inStack = inStack;

	object->sprite.setOrigin(ORIGIN);
	object->sprite.setPosition(inStack ? object->position + stack.offset : object->position);
	object->textures.push_back(textures[id]);
	object->sprite.setTexture(*(object->textures)[0]);
	object->sprite.setRotation(direction * 45);

	game->level.objectList[id].push_back(object);

	if (!inStack) {
		game->level.objectMap[object->position] = object;
	} else {
		game->level.stack.objectMap[object->position] = object;
		game->level.stack.objectList[id].push_back(object);
	}

	if (id == OBJ_DOT) {
		object->textures.push_back(textures[OBJ_COUNT]);
	}
}

void Game::Level::setTile(Object::Position position)
{
	tileSprites[position].setTexture(*tiles[static_cast<size_t>(obstacles[position])]);
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
			stack.sprites[currentPosition].setTexture(*tiles[TIL_EMPTY]);
			stack.sprites[currentPosition].setPosition(positionToFloat(currentPosition + offset));
		}
	}
}

bool Game::Editor::isActive()
{
	return active;
}

void Game::Editor::switchMode()
{
	mode = static_cast<EditorMode>((mode + 1) % static_cast<int>(ED_COUNT));
}

void Game::Editor::turn(bool editorOn)
{
	active = editorOn;
	sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
}

void Game::Editor::setObject(ObjectID id)
{
	if (active) {
		currentObject = id;
	}

	sprite.setTexture(*textures[id]);
}

ObjectID Game::Editor::getObject()
{
	return currentObject;
}