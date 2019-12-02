#include <fstream>
#include <iostream>

#include "auxiliary.h"
#include "constants.h"
#include "game.h"

Game::Game()
{
	for (size_t index = 0; index < OBJ_COUNT; ++index) {
		textures.push_back(loadTexture(IMG_NAMES[index]));
	}

	textures.push_back(loadTexture(IMG_NAMES[OBJ_DOTF]));
}

Game::~Game()
{
	for (size_t index = 0; index < textures.size(); ++index) {
		delete textures[index];
	}
}

// Load a sprite from a file
sf::Texture* Game::loadTexture(const std::string &filename)
{
	std::string location = PATH_DATA + PATH_IMG_PREFIX + filename + PATH_IMG_SUFFIX;
	sf::Texture* image = new sf::Texture();
	if (!image->loadFromFile(location)) {
		throw std::runtime_error("failed to load " + location + " image file");
	}

	return image;
}

// Load a level from a file
void Game::loadLevel(const std::string &id)
{
	///TODO: error handling
	level.game = this;

	std::string location = PATH_DATA + PATH_LEV_PREFIX + id + PATH_LEV_SUFFIX;
	std::ifstream levelFile(location, std::ios::binary | std::ios::in);
	if (levelFile.good()) {
		// Read level dimensions
		readByte(&levelFile, level.width);
		readByte(&levelFile, level.height);

		// Resize obstacle map
		for (short y = 0; y < level.height; ++y) {
			for (short x = 0; x < level.width; ++x) {
				Object::Position currentPosition;
				currentPosition.setPosition(x, y);

				// Fill object map with OBJ_EMPTY
				level.objectMap[currentPosition] = nullptr;

				// Read level obstacle data
				bool temp;
				levelFile.read((char*)&temp, 1);
				level.obstacles[currentPosition] = temp;
			}
		}

		// Read objects data
		unsigned short objectsCount;
		readByte(&levelFile, objectsCount);

		for (unsigned short obj = 0; obj < objectsCount; ++obj) {
			// Read the object's type
			unsigned short id;
			readByte(&levelFile, id);

			// Read object's position
			unsigned short x, y;
			readByte(&levelFile, x);
			readByte(&levelFile, y);

			// Add an object
			if (id == OBJ_BEAMER) {
				unsigned short red, green, blue;
				unsigned short direction;

				readByte(&levelFile, red);
				readByte(&levelFile, green);
				readByte(&levelFile, blue);
				readByte(&levelFile, direction);

				Color color(red > 0, green > 0, blue > 0);

				Beamer* beamer = new Beamer(color);
				setObject(beamer, x, y, id, direction);
			} else if (id == OBJ_DOT) {
				unsigned short red, green, blue;

				readByte(&levelFile, red);
				readByte(&levelFile, green);
				readByte(&levelFile, blue);

				Color color(red > 0, green > 0, blue > 0);

				Dot* dot = new Dot(color);
				setObject(dot, x, y, id);
			} else if (id == OBJ_MIRROR) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Mirror* mirror = new Mirror();
				setObject(mirror, x, y, id, direction);
			} else if (id == OBJ_BENDER) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Bender* bender = new Bender();
				setObject(bender, x, y, id, direction);
			}
		}
	} else {
		throw std::runtime_error("failed to load " + location + " file");
	}
}

void Game::calculateLasers()
{
	// Clear dots' actual color
	for (size_t dotIndex = 0; dotIndex < level.objectList[OBJ_DOT].size(); ++dotIndex) {
		Dot* dot = (Dot*) level.objectList[OBJ_DOT][dotIndex];
		dot->actualColor = {false, false, false};
	}

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

				if (level.objectMap[now] != nullptr) {
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

				if (level.isOutsideBoard(now)) {
					stop = end = true;
				}
			}

			ray.push_back(sf::Vertex(now, sfColor));
		}

		beamer->laser.push_back(ray);
	}

	updateDots();
}

void Game::updateDots()
{
	for (size_t dotIndex = 0; dotIndex < level.objectList[OBJ_DOT].size(); ++dotIndex) {
		Dot* dot = (Dot*) level.objectList[OBJ_DOT][dotIndex];
		dot->updateState();
	}
}

void Game::setObject(Object* object, short x, short y, unsigned short id, unsigned short direction)
{
	object->id = id;
	object->position.setPosition(x, y);
	object->direction = direction;

	object->sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
	object->sprite.setPosition(object->position);
	object->textures.push_back(textures[id]);
	object->sprite.setTexture(*(object->textures)[0]);
	object->sprite.setRotation(direction * 45);

	if (id == OBJ_DOT) {
		object->textures.push_back(textures[OBJ_COUNT]);
	}

	level.objectList[id].push_back(object);
	level.objectMap[object->position] = object;
}

void Game::setObject(Object* object, Object::Position position, unsigned short id, unsigned short direction)
{
	setObject(object, position.getX(), position.getY(), id, direction);
}

bool Game::Level::addObject(unsigned short id, Object::Position position)
{
	bool success = isPlaceFree(position);

	if (success) {
		if (id == OBJ_BEAMER) {
			Beamer* beamer = new Beamer();
			this->game->setObject(beamer, position, id, DIR_NORTH);
		} else if (id == OBJ_DOT) {
			Dot* dot = new Dot();
			this->game->setObject(dot, position, id, DIR_NORTH);
		} else if (id == OBJ_MIRROR) {
			Mirror* mirror = new Mirror();
			this->game->setObject(mirror, position, id, DIR_NORTH);
		} else if (id == OBJ_BENDER) {
			Bender* bender = new Bender();
			this->game->setObject(bender, position, id, DIR_NORTH);
		}
	}

	return success;
}

bool Game::Level::isPlaceFree(Object::Position position)
{
	return (objectMap[position] == nullptr && !obstacles[position] && !isOutsideBoard(position));
}

bool Game::Level::isOutsideBoard(Object::Position position)
{
	return (position.getX() < 0 || position.getY() < 0 || position.getX() >= width || position.getY() >= height);
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
	bool success = (objectMap[position] != nullptr);

	unsigned short id = objectMap[position]->id;

	if (success) {
		objectList[id].erase(std::find(objectList[id].begin(), objectList[id].end(), objectMap[position]));
		delete objectMap[position];
		objectMap[position] = nullptr;
	}

	return success;
}

bool Game::Editor::isActive()
{
	return active;
}

void Game::Editor::switchMode()
{
	mode = !mode;
}

void Game::Editor::turnOn()
{
	active = true;
	sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
}

void Game::Editor::turnOff()
{
	active = false;
}

void Game::Editor::setObject(unsigned short id)
{
	if (active) {
		currentObject = id;
	}

	sprite.setTexture(*textures[id]);
}

unsigned short Game::Editor::getObject()
{
	return currentObject;
}