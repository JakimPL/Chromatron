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
				bool obstacle;
				levelFile.read((char*)&obstacle, 1);
				level.obstacles[currentPosition] = obstacle;
				level.setTile(currentPosition);
			}
		}

		// Read ObjectID' data
		unsigned short ObjectIDCount;
		readByte(&levelFile, ObjectIDCount);

		for (unsigned short obj = 0; obj < ObjectIDCount; ++obj) {
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
				level.setObject(beamer, x, y, static_cast<ObjectID>(id), static_cast<DirectionID>(direction));
			} else if (id == OBJ_DOT) {
				unsigned short red, green, blue;

				readByte(&levelFile, red);
				readByte(&levelFile, green);
				readByte(&levelFile, blue);

				Color color(red > 0, green > 0, blue > 0);

				Dot* dot = new Dot(color);
				level.setObject(dot, x, y, static_cast<ObjectID>(id));
			} else if (id == OBJ_MIRROR) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Mirror* mirror = new Mirror();
				level.setObject(mirror, x, y, static_cast<ObjectID>(id), static_cast<DirectionID>(direction));
			} else if (id == OBJ_BENDER) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Bender* bender = new Bender();
				level.setObject(bender, x, y, static_cast<ObjectID>(id), static_cast<DirectionID>(direction));
			}
		}

		unsigned short width, height;
		readByte(&levelFile, width);
		readByte(&levelFile, height);

		for (size_t row = 0; row < height; ++row) {
			for (size_t column = 0; column < width; ++column) {
				unsigned short object;
				readByte(&levelFile, object);

				Object::Position currentPosition = shortToPosition(row, column);
				object = level.stack.map[currentPosition];
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
		// Save level dimensions
		writeByte(&levelFile, level.width);
		writeByte(&levelFile, level.height);

		// Resize obstacle map
		for (short y = 0; y < level.height; ++y) {
			for (short x = 0; x < level.width; ++x) {
				Object::Position currentPosition;
				currentPosition.setPosition(x, y);

				// Save level obstacle data
				levelFile.write((char*) & (level.obstacles[currentPosition]), 1);
			}
		}

		// Save ObjectID' data
		unsigned short ObjectIDCount = 0;
		for (size_t type = 0; type < OBJ_COUNT; ++type) {
			for (size_t index = 0; index < level.objectList[type].size(); ++index) {
				ObjectIDCount++;
			}
		}
		writeByte(&levelFile, ObjectIDCount);

		for (size_t type = 0; type < OBJ_COUNT; ++type) {
			for (size_t index = 0; index < level.objectList[type].size(); ++index) {
				Object* object = level.objectList[type][index];

				// Add an object
				writeByte(&levelFile, object->id);
				writeByte(&levelFile, object->position.getX());
				writeByte(&levelFile, object->position.getY());

				if (object->id == OBJ_BEAMER) {
					Beamer* beamer = (Beamer*) object;

					writeByte(&levelFile, beamer->color.red);
					writeByte(&levelFile, beamer->color.green);
					writeByte(&levelFile, beamer->color.blue);
					writeByte(&levelFile, beamer->direction);
				} else if (object->id == OBJ_DOT) {
					Dot* dot = (Dot*) object;

					writeByte(&levelFile, dot->color.red);
					writeByte(&levelFile, dot->color.green);
					writeByte(&levelFile, dot->color.blue);
				} else if (object->id == OBJ_MIRROR) {
					Mirror* mirror = (Mirror*) object;

					writeByte(&levelFile, mirror->direction);
				} else if (object->id == OBJ_BENDER) {
					Bender* bender = (Bender*) object;

					writeByte(&levelFile, bender->direction);
				}
			}
		}

		levelFile.close();

	} else {
		throw std::runtime_error("failed to save " + location + " file");
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

bool Game::Level::isOutsideBoard(Object::Position position)
{
	return (position.getX() < 0 || position.getY() < 0 || position.getX() >= width || position.getY() >= height);
}

bool Game::Level::dragObject(Drag &drag, Object::Position position)
{
	bool success = (objectMap[position] != nullptr) && (objectMap[position]->movable || game->editor.isActive());

	if (success) {
		drag.position = position;
		drag.sprite = objectMap[position]->sprite;
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
	bool success = (objectMap[position] != nullptr);

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
	bool success = (objectMap[position] != nullptr);

	if (success) {
		objectMap[position]->rotate(true, game->editor.isActive());
	}

	return success;
}

bool Game::Level::setObstacle(Object::Position position, bool obstacle)
{
	removeObject(position);
	obstacles[position] = obstacle;

	return true;
}

void Game::Level::setObject(Object* object, short x, short y, ObjectID id, DirectionID direction)
{
	object->id = id;
	object->position.setPosition(x, y);
	object->direction = direction;

	object->sprite.setOrigin(ORIGIN);
	object->sprite.setPosition(object->position);
	object->textures.push_back(textures[id]);
	object->sprite.setTexture(*(object->textures)[0]);
	object->sprite.setRotation(direction * 45);

	if (id == OBJ_DOT) {
		object->textures.push_back(textures[OBJ_COUNT]);
	}

	game->level.objectList[id].push_back(object);
	game->level.objectMap[object->position] = object;
}

void Game::Level::setObject(Object* object, Object::Position position, ObjectID id, DirectionID direction)
{
	setObject(object, position.getX(), position.getY(), id, direction);
}

void Game::Level::setTile(Object::Position position)
{
	tileSprites[position].setTexture(*tiles[static_cast<size_t>(obstacles[position])]);
	tileSprites[position].setPosition(positionToFloat(position));
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