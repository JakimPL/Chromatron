#include <fstream>
#include <iostream>

#include "auxiliary.h"
#include "game.h"

Game::Game()
{
	for (size_t index = 0; index < OBJ_COUNT; ++index) {
		textures.push_back(loadTexture(IMG_NAMES[index]));
	}
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
				setObject(beamer, x, y, id);

				level.objectList.push_back(beamer);
				level.beamerList.push_back(beamer);
			} else if (id == OBJ_DOT) {
				unsigned short red, green, blue;

				readByte(&levelFile, red);
				readByte(&levelFile, green);
				readByte(&levelFile, blue);

				Color color(red > 0, green > 0, blue > 0);

				Dot *dot = new Dot(color);
				setObject(dot, x, y, id);

				level.objectList.push_back(dot);
			} else if (id == OBJ_MIRROR) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Mirror *mirror = new Mirror();
				setObject(mirror, x, y, id);

				level.objectList.push_back(mirror);
				level.mirrorList.push_back(mirror);
			} else if (id == OBJ_BENDER) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Bender *bender = new Bender();
				level.objectList.push_back(bender);
			}
		}
	} else {
		throw std::runtime_error("failed to load " + location + " file");
	}
}

void Game::calculateLasers()
{
	for (size_t beamer = 0; beamer < level.beamerList.size(); ++beamer) {
		level.beamerList[beamer]->laser.clear();

		double xx_start, xx, yy_start, yy;
		xx = xx_start = static_cast<double>(level.beamerList[beamer]->x);
		yy = yy_start = static_cast<double>(level.beamerList[beamer]->y);
		Color col = level.beamerList[beamer]->color;
		unsigned short dir = level.beamerList[beamer]->direction;

		bool middlePoint = false;
		bool end = false;
		while (!end) {
			bool stop = false;
			while (!stop) {
				xx += moveInDirection_x(dir, 1);
				yy += moveInDirection_y(dir, 1);

				for (size_t mirror = 0; mirror < level.mirrorList.size(); ++mirror) {
					if (level.mirrorList[mirror]->x == static_cast<unsigned short>(xx) && level.mirrorList[mirror]->y == static_cast<unsigned short>(yy)) {
						stop = end = true;
						middlePoint = true;

						///TODO: reflections
					}
				}

				if (xx < 0 || yy < 0 || xx >= level.width || yy >= level.height) {
					stop = end = true;
				}
			}

			if (!middlePoint) {
				xx -= moveInDirection_x(dir, 0.5);
				yy -= moveInDirection_y(dir, 0.5);
			}

			Ray ray(OFFSET_X + (xx_start + 0.5) * TILE_SIZE, OFFSET_Y + (yy_start + 0.5) * TILE_SIZE, OFFSET_X + (xx + 0.5) * TILE_SIZE, OFFSET_Y + (yy + 0.5) * TILE_SIZE, col);
			level.beamerList[beamer]->laser.push_back(ray);

			xx_start = xx;
			yy_start = yy;
		}
	}
}

void Game::setObject(Object* object, unsigned short x, unsigned short y, unsigned short id)
{
	///TODO: proper object initializer
	object->x = x;
	object->y = y;
	object->id = id;
	object->sprite.setPosition(OFFSET_X + TILE_SIZE * x, OFFSET_Y + TILE_SIZE * y);
	object->sprite.setTexture(*textures[id]);
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