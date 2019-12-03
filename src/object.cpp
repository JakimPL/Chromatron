#include "auxiliary.h"
#include "constants.h"
#include "object.h"

// Objects' constructors
Object::Object(Color col) : color(col)
{

}

Beamer::Beamer(Color col)
{
	rotatable = false;
	movable = false;
	colorable = true;
	color = col;
}

Dot::Dot(Color col) : actualColor(COL_BLACK_TUPLE)
{
	rotatable = false;
	movable = false;
	colorable = true;
	color = col;
	sprite.setColor(color.convertToColor());
}

Mirror::Mirror()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

Bender::Bender()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

// Class functions
void Object::rotate(bool clockwise, bool force)
{
	if (rotatable || force) {
		direction = (direction + (clockwise ? 1 : DIR_COUNT - 1)) % DIR_COUNT;
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

bool Object::Position::operator==(const Position &pos) const
{
	return x == pos.x && y == pos.y;
}

bool Object::Position::operator!=(const Position &pos) const
{
	return x != pos.x || y != pos.y;
}

Object::Position::operator sf::Vector2f()
{
	sf::Vector2f vector(OFFSET_X + TILE_SIZE * (x + 0.5f), OFFSET_Y + TILE_SIZE * (y + 0.5f));
	return vector;
}

short Object::Position::getX()
{
	return x;
}

short Object::Position::getY()
{
	return y;
}

void Object::Position::setPosition(short xx, short yy)
{
	x = xx;
	y = yy;
}

void Dot::updateState()
{
	state = (actualColor == color);
	sprite.setTexture(*textures[state ? 1 : 0]);

	direction = DIR_NORTH;
	sprite.setRotation(0);
}