#ifndef POSITION_H
#define POSITION_H

#include <SFML/Graphics.hpp>
#include "color.hpp"

struct Position {
private:
	short x = -1;
	short y = -1;
public:
	Position operator+(const Position &pos);
	Position operator-(const Position &pos);
	Position operator-();
	bool operator<(const Position &pos) const;
	bool operator==(const Position &pos) const;
	bool operator!=(const Position &pos) const;
	operator sf::Vector2f();

	short getX();
	short getY();
	bool isNull();
	void setNull();
	void moveInDirection(unsigned short dir, int length);
	void setPosition(short xx, short yy);
	void setPosition(Position position);
};

#endif