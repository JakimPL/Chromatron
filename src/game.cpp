#include <fstream>
#include <iostream>

#include "auxiliary.h"
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
		level.obstacles.resize(level.height, std::vector<bool>(level.width));
		for (short y = 0; y < level.height; ++y) {
			for (short x = 0; x < level.width; ++x) {
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

				Beamer *beamer = new Beamer(color);
				setObject(beamer, x, y, id, direction);
				level.objectList[OBJ_BEAMER].push_back(beamer);
			} else if (id == OBJ_DOT) {
				unsigned short red, green, blue;

				readByte(&levelFile, red);
				readByte(&levelFile, green);
				readByte(&levelFile, blue);

				Color color(red > 0, green > 0, blue > 0);

				Dot *dot = new Dot(color);
				setObject(dot, x, y, id);
				level.objectList[OBJ_DOT].push_back(dot);
			} else if (id == OBJ_MIRROR) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Mirror *mirror = new Mirror();
				setObject(mirror, x, y, id, direction);
				level.objectList[OBJ_MIRROR].push_back(mirror);
			} else if (id == OBJ_BENDER) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Bender *bender = new Bender();
				level.objectList[OBJ_BENDER].push_back(bender);
			}
		}
	} else {
		throw std::runtime_error("failed to load " + location + " file");
	}
}

void Game::calculateLasers()
{
	for (size_t beamerIndex = 0; beamerIndex < level.objectList[OBJ_BEAMER].size(); ++beamerIndex) {
		Beamer* beamer = (Beamer*) level.objectList[OBJ_BEAMER][beamerIndex];
		beamer->laser.clear();

		double xx_start, xx, yy_start, yy;
		xx = xx_start = static_cast<double>(beamer->x);
		yy = yy_start = static_cast<double>(beamer->y);
		Color col = beamer->color;
		unsigned short dir = beamer->direction;

		bool end = false;
		while (!end) {
			bool stop = false;
			while (!stop) {
				xx += moveInDirection_x(dir, 1);
				yy += moveInDirection_y(dir, 1);

				for (size_t mirrorIndex = 0; mirrorIndex < level.objectList[OBJ_MIRROR].size(); ++mirrorIndex) {
					Mirror* mirror = (Mirror*) level.objectList[OBJ_MIRROR][mirrorIndex];
					if (mirror->x == static_cast<unsigned short>(xx) && mirror->y == static_cast<unsigned short>(yy)) {
						int diff = (DIRS + mirror->direction - dir) % DIRS - 4;
						if (std::abs(diff) <= 1) {
							stop = true;
							dir = (DIRS + dir - (diff == 0 ? 4 : 2 * diff)) % DIRS;
						} else {
							stop = end = true;
						}
					}
				}

				for (size_t dotIndex = 0; dotIndex < level.objectList[OBJ_DOT].size(); ++dotIndex) {
					Dot *dot = (Dot*) level.objectList[OBJ_DOT][dotIndex];
					if (dot->x == static_cast<unsigned short>(xx) && dot->y == static_cast<unsigned short>(yy)) {
						dot->actualColor = dot->actualColor + col;
					}
				}

				if (xx < 0 || yy < 0 || xx >= level.width || yy >= level.height) {
					stop = end = true;
				}
			}

			Ray ray(OFFSET_X + (xx_start + 0.5) * TILE_SIZE, OFFSET_Y + (yy_start + 0.5) * TILE_SIZE, OFFSET_X + (xx + 0.5) * TILE_SIZE, OFFSET_Y + (yy + 0.5) * TILE_SIZE, col);
			beamer->laser.push_back(ray);

			xx_start = xx;
			yy_start = yy;
		}
	}
}

void Game::updateDots()
{
	for (size_t dotIndex = 0; dotIndex < level.objectList[OBJ_DOT].size(); ++dotIndex) {
		Dot *dot = (Dot*) level.objectList[OBJ_DOT][dotIndex];
		dot->updateState();
		bool state = dot->state;
		dot->sprite.setTexture(*textures[state ? OBJ_DOTF : OBJ_DOT]);
	}
}

void Game::setObject(Object* object, unsigned short x, unsigned short y, unsigned short id, unsigned short direction)
{
	///TODO: proper object initializer
	object->x = x;
	object->y = y;
	object->id = id;
	object->direction = direction;
	object->sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
	object->sprite.setPosition(OFFSET_X + TILE_SIZE * (x + 0.5), OFFSET_Y + TILE_SIZE * (y + 0.5));
	object->sprite.setTexture(*textures[id]);
	object->sprite.setRotation(direction * 45);
}

// Move in a direction
double Game::moveInDirection_x(unsigned short dir, double length)
{
	return length * (dir % 4 > 0 ? (dir / 4 > 0 ? -1 : 1) : 0);
}

double Game::moveInDirection_y(unsigned short dir, double length)
{
	return length * ((dir + 2) % 4 > 0 ? ((((dir + 2) % 8) / 4 > 0) ? 1 : -1) : 0);
}