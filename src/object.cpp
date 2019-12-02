#include "auxiliary.h"
#include "constants.h"
#include "object.h"

void Object::rotate(bool clockwise)
{
	if (rotatable) {
		direction = (direction + (clockwise ? 1 : DIRS - 1)) % DIRS;
		sprite.setRotation(direction * 45);
	}
}

void Object::updateSprite()
{
	sprite.setPosition(position);
}

Object::Position Object::Position::createPosition(short xx, short yy)
{
	Object::Position position;
	position.x = xx;
	position.y = yy;
	return position;
}

Object::Position Object::Position::createPosition(sf::Vector2f vector)
{
	Object::Position position;
	position.x = static_cast<short>((vector.x - OFFSET_X) / TILE_SIZE);
	position.y = static_cast<short>((vector.y - OFFSET_Y) / TILE_SIZE);
	return position;
}

void Object::Position::moveInDirection(unsigned short dir, int length)
{
	x += length * (dir % 4 > 0 ? (dir / 4 > 0 ? -1 : 1) : 0);
	y += length * ((dir + 2) % 4 > 0 ? ((((dir + 2) % 8) / 4 > 0) ? 1 : -1) : 0);
}

bool Object::Position::operator<(const Position &pos) const
{
	return x < pos.x || (x == pos.x && y < pos.y);
}

bool Object::Position::operator==(const Position &pos)
{
	return x == pos.x && y == pos.y;
}

bool Object::Position::operator!=(const Position &pos)
{
	return x != pos.x || y != pos.y;
}

Object::Position::operator sf::Vector2f()
{
	sf::Vector2f vector(OFFSET_X + TILE_SIZE * (x + 0.5f), OFFSET_Y + TILE_SIZE * (y + 0.5f));
	return vector;
}

void Dot::updateState()
{
	state = (actualColor == color);
	sprite.setTexture(*textures[state ? 1 : 0]);
}