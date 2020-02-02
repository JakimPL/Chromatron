#include "constants.hpp"
#include "position.hpp"

void Position::moveInDirection(unsigned short dir, int length)
{
	x += length * (dir % 4 > 0 ? (dir / 4 > 0 ? -1 : 1) : 0);
	y += length * ((dir + 2) % 4 > 0 ? ((((dir + 2) % 8) / 4 > 0) ? 1 : -1) : 0);
}

Position Position::operator+(const Position &pos)
{
	Position newPosition;
	newPosition.x = x + pos.x;
	newPosition.y = y + pos.y;
	return newPosition;
}

Position Position::operator-(const Position &pos)
{
	Position newPosition;
	newPosition.x = x - pos.x;
	newPosition.y = y - pos.y;
	return newPosition;
}

Position Position::operator-()
{
	Position newPosition;
	newPosition.x = -x;
	newPosition.y = -y;
	return newPosition;
}

bool Position::operator<(const Position &pos) const
{
	return x < pos.x or (x == pos.x && y < pos.y);
}

bool Position::operator==(const Position &pos) const
{
	return x == pos.x and y == pos.y;
}

bool Position::operator!=(const Position &pos) const
{
	return x != pos.x or y != pos.y;
}

Position::operator sf::Vector2f()
{
	sf::Vector2f vector(1 + TILE_SIZE * (x + OFFSET_X + 0.5f), 1 + TILE_SIZE * (y + OFFSET_Y + 0.5f));
	return vector;
}

bool Position::isNull()
{
	return x == -1 && y == -1;
}

void Position::setNull()
{
	x = -1;
	y = -1;
}

short Position::getX()
{
	return x;
}

short Position::getY()
{
	return y;
}

void Position::setPosition(short xx, short yy)
{
	x = xx;
	y = yy;
}

void Position::setPosition(Position position)
{
	x = position.x;
	y = position.y;
}