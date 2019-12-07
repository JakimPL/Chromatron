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

void Game::loadLevel(const std::string &id)
{
	///TODO: error handling
	level.game = this;

	std::string location = PATH_DATA + PATH_LEV_PREFIX + id + PATH_LEV_SUFFIX;
	std::ifstream levelFile(location, std::ios::binary | std::ios::in);
	if (levelFile.good()) {
		readByte(levelFile, level.width);
		readByte(levelFile, level.height);

		// Resize obstacle map
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

		unsigned short objectId;
		readByte(levelFile, objectId);

		for (unsigned short obj = 0; obj < objectId; ++obj) {
			readObject(levelFile, level, false);
		}

		unsigned short stackWidth, stackHeight;
		readByte(levelFile, stackWidth);
		readByte(levelFile, stackHeight);

		for (size_t row = 0; row < stackWidth; ++row) {
			for (size_t column = 0; column < stackHeight; ++column) {
				readObject(levelFile, level, true);
			}
		}

		level.updateStack();
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

		for (short y = 0; y < level.height; ++y) {
			for (short x = 0; x < level.width; ++x) {
				Object::Position currentPosition;
				currentPosition.setPosition(x, y);

				levelFile.write((char*) & (level.obstacles[currentPosition]), 1);
			}
		}

		unsigned short objectId = 0;
		for (size_t type = 0; type < OBJ_COUNT; ++type) {
			for (size_t index = 0; index < level.objectList[type].size(); ++index) {
				objectId++;
			}
		}
		writeByte(levelFile, objectId);

		for (size_t type = 0; type < OBJ_COUNT; ++type) {
			for (size_t index = 0; index < level.objectList[type].size(); ++index) {
				Object* object = level.objectList[type][index];
				writeObject(levelFile, level, object, false);
			}
		}

		writeByte(levelFile, level.stack.width);
		writeByte(levelFile, level.stack.height);

		for (size_t row = 0; row < level.stack.width; ++row) {
			for (size_t column = 0; column < level.stack.height; ++column) {
				Object::Position currentPosition = shortToPosition(row, column);
				Object* object = level.stack.objectMap[currentPosition];
				writeObject(levelFile, level, object, true);
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

		Ray ray;
		ray.push_back(sf::Vertex(beamer->position, sfColor));

		bool end = false;
		while (!end) {
			bool stop = false;
			while (!stop) {
				now.moveInDirection(dir, 1);

				if (level.objectMap[now] != nullptr && !level.objectMap[now]->inStack) {
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
				}
			}

			ray.push_back(sf::Vertex(now, sfColor));
		}

		beamer->laser.push_back(ray);
	}

	updateDots();
}

void Game::clearDots()
{
	for (size_t dotIndex = 0; dotIndex < level.objectList[OBJ_DOT].size(); ++dotIndex) {
		Dot* dot = (Dot*) level.objectList[OBJ_DOT][dotIndex];
		dot->actualColor = {false, false, false};
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
	bool success = isPlaceFree(position);

	if (success) {
		if (id == OBJ_BEAMER) {
			Beamer* beamer = new Beamer();
			setObject(beamer, position, id, DIR_NORTH);
		} else if (id == OBJ_DOT) {
			Dot* dot = new Dot();
			setObject(dot, position, id, DIR_NORTH);
		} else if (id == OBJ_MIRROR) {
			Mirror* mirror = new Mirror();
			setObject(mirror, position, id, DIR_NORTH);
		} else if (id == OBJ_BENDER) {
			Bender* bender = new Bender();
			setObject(bender, position, id, DIR_NORTH);
		}
	}

	return success;
}

bool Game::Level::changeObjectColor(Object::Position position)
{
	bool success = (objectMap[position] != nullptr && objectMap[position]->colorable);

	if (success) {
		objectMap[position]->color.nextColor();
	}

	return success;
}

bool Game::Level::isPlaceFree(Object::Position position)
{
	return (objectMap[position] == nullptr && !obstacles[position] && !isOutsideBoard(position));
}

bool Game::Level::isPlaceTaken(Object::Position position)
{
	return (objectMap[position] != nullptr && !objectMap[position]->inStack);
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

	ObjectID id = objectMap[position]->id;

	if (success) {
		objectList[id].erase(std::find(objectList[id].begin(), objectList[id].end(), objectMap[position]));
		delete objectMap[position];
		objectMap[position] = nullptr;
	}

	return success;
}

bool Game::Level::rotateObject(Object::Position position)
{
	bool success = isPlaceTaken(position);

	if (success) {
		objectMap[position]->rotate(true, game->editor.isActive());
	}

	return success;
}

bool Game::Level::rotateStackObject(Object::Position position)
{
	Object::Position stackPosition = stack.getRelativePosition(position);
	bool success = stack.isPlaceTaken(stackPosition);

	if (success) {
		stack.objectMap[stackPosition]->rotate(true, game->editor.isActive());
	}

	return success;
}

bool Game::Level::setObstacle(Object::Position position, bool obstacle)
{
	removeObject(position);
	obstacles[position] = obstacle;

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

	if (id == OBJ_DOT) {
		object->textures.push_back(textures[OBJ_COUNT]);
	}

	game->level.objectList[id].push_back(object);
	game->level.objectMap[object->position] = object;

	if (inStack) {
		stack.objectMap[position];
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