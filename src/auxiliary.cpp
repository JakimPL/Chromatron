#include "auxiliary.hpp"
#include "log.hpp"

sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color)
{
	sf::RectangleShape rectangle(sf::Vector2f(w, h));
	rectangle.setPosition(x, y);
	rectangle.setFillColor(color);
	return rectangle;
}

unsigned short countDigit(unsigned short n)
{
	unsigned short count = 0;
	while (n != 0) {
		n = n / 10;
		count++;
	}
	return count;
}

std::string numberToString(unsigned short number)
{
	if (number > 0 && number < 1000) {
		std::string id = std::to_string(number);
		id = std::string(3 - countDigit(number), '0').append(id);
		return id;
	} else {
		LogError("Wrong level number");
		return "";
	}
}

void readByte(std::ifstream &file, unsigned short &var)
{
	char buffer;
	file.read(&buffer, 1);
	var = static_cast<unsigned short>(buffer);
}

void readObject(std::ifstream &file, Game::Level &level, bool stackObject)
{
	unsigned short id;
	readByte(file, id);

	unsigned short inStack;
	readByte(file, inStack);

	unsigned short x, y;
	readByte(file, x);
	readByte(file, y);

	if (id == OBJ_BEAMER) {
		unsigned short red, green, blue;
		unsigned short direction;

		readByte(file, red);
		readByte(file, green);
		readByte(file, blue);
		readByte(file, direction);

		Color color(red > 0, green > 0, blue > 0);

		Beamer* beamer = new Beamer(color);
		beamer->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0, stackObject);
	} else if (id == OBJ_DOT) {
		unsigned short red, green, blue;

		readByte(file, red);
		readByte(file, green);
		readByte(file, blue);

		Color color(red > 0, green > 0, blue > 0);

		Dot* dot = new Dot(color);
		dot->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), DIR_NORTH, inStack > 0, stackObject);
	} else if (id == OBJ_MIRROR) {
		unsigned short direction;

		readByte(file, direction);

		Mirror* mirror = new Mirror();
		mirror->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0, stackObject);
	} else if (id == OBJ_BENDER) {
		unsigned short direction;

		readByte(file, direction);

		Bender* bender = new Bender();
		bender->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0, stackObject);
	} else if (id == OBJ_SPLITTER) {
		unsigned short direction;

		readByte(file, direction);

		Splitter* splitter = new Splitter();
		splitter->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0, stackObject);
	} else if (id == OBJ_CONDUIT) {
		unsigned short direction;

		readByte(file, direction);

		Conduit* conduit = new Conduit();
		conduit->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0, stackObject);
	} else if (id == OBJ_FILTER) {
		unsigned short red, green, blue;
		unsigned short direction;

		readByte(file, red);
		readByte(file, green);
		readByte(file, blue);
		readByte(file, direction);

		Color color(red > 0, green > 0, blue > 0);

		Filter* filter = new Filter(color);
		filter->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0, stackObject);
	} else if (id == OBJ_PRISM) {
		unsigned short direction;

		readByte(file, direction);

		Prism* prism = new Prism();
		prism->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0, stackObject);
	} else if (id == OBJ_DOPPLER) {
		unsigned short direction;

		readByte(file, direction);

		Doppler* doppler = new Doppler();
		doppler->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0, stackObject);
	} else if (id == OBJ_TANGLER) {
		unsigned short direction;

		readByte(file, direction);

		Tangler* tangler = new Tangler();
		tangler->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0, stackObject);
	} else if (id == OBJ_TELEPORTER) {
		Teleporter* teleporter = new Teleporter();
		teleporter->setObject(level.game, shortToPosition(x, y), static_cast<ObjectID>(id), DIR_NORTH, inStack > 0, stackObject);
	}
}

void writeObject(std::ofstream &file, Object* object)
{
	if (object != nullptr) {
		object->writeObject(file);
	} else {
		writeByte(file, OBJ_EMPTY);
	}
}

void writeByte(std::ofstream &file, unsigned short var)
{
	char buffer = static_cast<char>(var);
	file.write(&buffer, 1);
}

Position floatToPosition(sf::Vector2f vector)
{
	return shortToPosition(static_cast<short>(vector.x / TILE_SIZE - OFFSET_X), static_cast<short>(vector.y / TILE_SIZE - OFFSET_Y));
}

Position floatToPosition(float x, float y)
{
	return floatToPosition(sf::Vector2f(x, y));
}

Position shortToPosition(short x, short y)
{
	Position position;
	position.setPosition(x, y);
	return position;
}

sf::Vector2f positionToFloat(Position position)
{
	return sf::Vector2f((position.getX() + OFFSET_X) * TILE_SIZE, (position.getY() + OFFSET_Y) * TILE_SIZE);
}

sf::Vector2f positionToFloat(short x, short y)
{
	return sf::Vector2f((x + OFFSET_X) * TILE_SIZE, (y + OFFSET_Y) * TILE_SIZE);
}