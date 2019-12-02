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

// Load a sprite
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
				// Fill object map with OBJ_EMPTY
				level.objectMap[x][y] = nullptr;

				// Read level obstacle data
				bool temp;
				levelFile.read((char*)&temp, 1);
				level.obstacles[x][y] = temp;
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

				auto x = now.x;
				auto y = now.y;

				if (level.objectMap[x][y] != nullptr) {
					if (level.objectMap[x][y]->id == OBJ_BEAMER) {
						stop = end = true;
					} else if (level.objectMap[x][y]->id == OBJ_DOT) {
						Dot* dot = (Dot*) level.objectMap[x][y];
						dot->actualColor = dot->actualColor + color;
					} else if (level.objectMap[x][y]->id == OBJ_MIRROR) {
						Mirror* mirror = (Mirror*) level.objectMap[x][y];
						int diff = (DIRS + mirror->direction - dir) % DIRS - 4;
						if (std::abs(diff) <= 1) {
							stop = true;
							dir = (DIRS + dir - (diff == 0 ? 4 : 2 * diff)) % DIRS;
						} else {
							stop = end = true;
						}
					}
				}

				if (x < 0 || y < 0 || x >= level.width || y >= level.height) {
					stop = end = true;
				}
			}

			// Add a new node to the ray
			ray.push_back(sf::Vertex(now, sfColor));
		}

		// Add the created ray
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

void Game::setObject(Object * object, short x, short y, unsigned short id, unsigned short direction)
{
	object->position.x = x;
	object->position.y = y;
	object->id = id;
	object->direction = direction;
	object->sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
	object->sprite.setPosition(object->position);

	object->textures.push_back(textures[id]);
	object->sprite.setTexture(*(object->textures)[0]);
	object->sprite.setRotation(direction * 45);

	if (id == OBJ_DOT) {
		object->textures.push_back(textures[OBJ_DOTF]);
	}

	level.objectList[id].push_back(object);
	level.objectMap[x][y] = object;
}