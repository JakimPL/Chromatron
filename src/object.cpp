#include "object.hpp"
#include "auxiliary.hpp"
#include "constants.hpp"

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
	setSpriteColor();
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

Splitter::Splitter()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

Conduit::Conduit()
{
	rotatable = false;
	movable = false;
	colorable = false;
}

Filter::Filter(Color col)
{
	rotatable = true;
	movable = true;
	colorable = true;
	color = col;
}

Prism::Prism()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

Doppler::Doppler()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

Tangler::Tangler()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

void Object::rotate(bool clockwise, bool force)
{
	if (rotatable || force) {
		direction = static_cast<DirectionID>((static_cast<unsigned short>(direction) + (clockwise ? 1 : DIR_COUNT - 1)) % DIR_COUNT);
		baseSprite.setRotation(direction * 45);
		sprite.setRotation(direction * 45);
	}
}

void Object::updateSprite()
{
	sprite.setPosition(position);
	baseSprite.setPosition(position);
}

void Object::setSpriteColor()
{
	sprite.setColor(color.convertToColor());
}

void Object::Position::moveInDirection(unsigned short dir, int length)
{
	x += length * (dir % 4 > 0 ? (dir / 4 > 0 ? -1 : 1) : 0);
	y += length * ((dir + 2) % 4 > 0 ? ((((dir + 2) % 8) / 4 > 0) ? 1 : -1) : 0);
}

Object::Position Object::Position::operator+(const Position &pos)
{
	Object::Position newPosition;
	newPosition.x = x + pos.x;
	newPosition.y = y + pos.y;
	return newPosition;
}

Object::Position Object::Position::operator-(const Position &pos)
{
	Object::Position newPosition;
	newPosition.x = x - pos.x;
	newPosition.y = y - pos.y;
	return newPosition;
}

Object::Position Object::Position::operator-()
{
	Object::Position newPosition;
	newPosition.x = -x;
	newPosition.y = -y;
	return newPosition;
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
	sf::Vector2f vector(1 + TILE_SIZE * (x + OFFSET_X + 0.5f), 1 + TILE_SIZE * (y + OFFSET_Y + 0.5f));
	return vector;
}

bool Object::Position::isNull()
{
	return x == -1 && y == -1;
}

void Object::Position::setNull()
{
	x = -1;
	y = -1;
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

void Object::Position::setPosition(Position position)
{
	x = position.x;
	y = position.y;
}

void Dot::updateState()
{
	state = (actualColor == color);
	sprite.setTexture(*textures[state ? 1 : 0]);

	direction = DIR_NORTH;
	sprite.setRotation(0);
}